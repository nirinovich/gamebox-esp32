#include "DesktopServer.hpp"
#include <gamehub/web/WebAssets.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using socket_t = SOCKET;
#define IS_INVALID_SOCKET(s) ((s) == INVALID_SOCKET)
#define CLOSE_SOCKET(s) closesocket(s)
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
using socket_t = int;
#define IS_INVALID_SOCKET(s) ((s) < 0)
#define CLOSE_SOCKET(s) close(s)
#endif

namespace gamehub {
namespace desktop {

// --- Minimal Portable SHA-1 Implementation ---
namespace sha1 {
    inline uint32_t rol(uint32_t value, size_t bits) { return (value << bits) | (value >> (32 - bits)); }

    inline void transform(uint32_t state[5], const uint8_t buffer[64]) {
        uint32_t a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];
        uint32_t block[80];
        for (size_t i = 0; i < 16; ++i) {
            block[i] = (buffer[4 * i] << 24) | (buffer[4 * i + 1] << 16) | (buffer[4 * i + 2] << 8) | (buffer[4 * i + 3]);
        }
        for (size_t i = 16; i < 80; ++i) {
            block[i] = rol(block[i - 3] ^ block[i - 8] ^ block[i - 14] ^ block[i - 16], 1);
        }
        for (size_t i = 0; i < 20; ++i) {
            uint32_t t = rol(a, 5) + ((b & c) | (~b & d)) + e + block[i] + 0x5A827999;
            e = d; d = c; c = rol(b, 30); b = a; a = t;
        }
        for (size_t i = 20; i < 40; ++i) {
            uint32_t t = rol(a, 5) + (b ^ c ^ d) + e + block[i] + 0x6ED9EBA1;
            e = d; d = c; c = rol(b, 30); b = a; a = t;
        }
        for (size_t i = 40; i < 60; ++i) {
            uint32_t t = rol(a, 5) + ((b & c) | (b & d) | (c & d)) + e + block[i] + 0x8F1BBCDC;
            e = d; d = c; c = rol(b, 30); b = a; a = t;
        }
        for (size_t i = 60; i < 80; ++i) {
            uint32_t t = rol(a, 5) + (b ^ c ^ d) + e + block[i] + 0xCA62C1D6;
            e = d; d = c; c = rol(b, 30); b = a; a = t;
        }
        state[0] += a; state[1] += b; state[2] += c; state[3] += d; state[4] += e;
    }

    inline void calculate(const std::string& input, uint8_t digest[20]) {
        uint32_t state[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
        uint64_t totalBits = input.size() * 8;
        size_t offset = 0;
        uint8_t buffer[64];

        while (offset + 64 <= input.size()) {
            memcpy(buffer, input.data() + offset, 64);
            transform(state, buffer);
            offset += 64;
        }

        size_t remaining = input.size() - offset;
        memcpy(buffer, input.data() + offset, remaining);
        buffer[remaining++] = 0x80;
        if (remaining > 56) {
            memset(buffer + remaining, 0, 64 - remaining);
            transform(state, buffer);
            remaining = 0;
        }
        memset(buffer + remaining, 0, 56 - remaining);
        for (int i = 7; i >= 0; --i) {
            buffer[56 + (7 - i)] = static_cast<uint8_t>((totalBits >> (i * 8)) & 0xFF);
        }
        transform(state, buffer);

        for (int i = 0; i < 5; ++i) {
            digest[i * 4] = static_cast<uint8_t>((state[i] >> 24) & 0xFF);
            digest[i * 4 + 1] = static_cast<uint8_t>((state[i] >> 16) & 0xFF);
            digest[i * 4 + 2] = static_cast<uint8_t>((state[i] >> 8) & 0xFF);
            digest[i * 4 + 3] = static_cast<uint8_t>(state[i] & 0xFF);
        }
    }
} // namespace sha1

// --- Minimal Base64 Implementation ---
static const char BASE64_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
inline std::string base64Encode(const uint8_t* data, size_t len) {
    std::string ret;
    int i = 0, j = 0;
    uint8_t charArray3[3], charArray4[4];
    while (len--) {
        charArray3[i++] = *(data++);
        if (i == 3) {
            charArray4[0] = (charArray3[0] & 0xfc) >> 2;
            charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
            charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
            charArray4[3] = charArray3[2] & 0x3f;
            for (i = 0; i < 4; i++) ret += BASE64_CHARS[charArray4[i]];
            i = 0;
        }
    }
    if (i) {
        for (j = i; j < 3; j++) charArray3[j] = '\0';
        charArray4[0] = (charArray3[0] & 0xfc) >> 2;
        charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
        charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
        for (j = 0; j < i + 1; j++) ret += BASE64_CHARS[charArray4[j]];
        while ((i++ < 3)) ret += '=';
    }
    return ret;
}

DesktopServer::DesktopServer(uint16_t port) : m_port(port) {
#if defined(_WIN32)
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

DesktopServer::~DesktopServer() {
    stop();
#if defined(_WIN32)
    WSACleanup();
#endif
}

bool DesktopServer::start(bool autoFallback) {
    if (m_running) return true;

    uint16_t startPort = m_port;
    const uint16_t maxAttempts = autoFallback ? 20 : 1;

    for (uint16_t i = 0; i < maxAttempts; ++i) {
        uint16_t currentTryPort = startPort + i;

        socket_t s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (IS_INVALID_SOCKET(s)) return false;

        int opt = 1;
        setsockopt(s, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(currentTryPort);

        if (bind(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
            CLOSE_SOCKET(s);
            if (autoFallback && i + 1 < maxAttempts) {
                std::cout << "[DesktopServer] Port " << currentTryPort << " is in use. Trying port " << (currentTryPort + 1) << "..." << std::endl;
                continue;
            }
            return false;
        }

        if (listen(s, 10) != 0) {
            CLOSE_SOCKET(s);
            return false;
        }

        m_port = currentTryPort;
        m_serverSocket = static_cast<uintptr_t>(s);
        m_running = true;

        m_acceptThread = std::thread(&DesktopServer::acceptLoop, this);
        std::cout << "[DesktopServer] Listening on http://localhost:" << m_port << std::endl;
        return true;
    }

    return false;
}

void DesktopServer::stop() {
    if (!m_running) return;
    m_running = false;

    if (m_serverSocket != 0) {
        CLOSE_SOCKET(static_cast<socket_t>(m_serverSocket));
        m_serverSocket = 0;
    }

    if (m_acceptThread.joinable()) {
        m_acceptThread.join();
    }
}

void DesktopServer::acceptLoop() {
    while (m_running) {
        sockaddr_in clientAddr{};
        int clientLen = sizeof(clientAddr);
        socket_t client = accept(static_cast<socket_t>(m_serverSocket), reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
        if (IS_INVALID_SOCKET(client)) {
            if (!m_running) break;
            continue;
        }

        uint32_t sessionId = m_nextSessionId++;
        std::thread([this, client, sessionId]() {
            handleClient(static_cast<uintptr_t>(client), sessionId);
        }).detach();
    }
}

void DesktopServer::handleClient(uintptr_t clientSocket, uint32_t sessionId) {
    socket_t client = static_cast<socket_t>(clientSocket);
    char buffer[4096];
    int bytesRead = recv(client, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        CLOSE_SOCKET(client);
        return;
    }
    buffer[bytesRead] = '\0';
    std::string request(buffer, bytesRead);

    // Check if WebSocket upgrade request
    if (request.find("Upgrade: websocket") != std::string::npos || request.find("Upgrade: WebSocket") != std::string::npos) {
        auto keyPos = request.find("Sec-WebSocket-Key: ");
        if (keyPos == std::string::npos) {
            CLOSE_SOCKET(client);
            return;
        }
        auto keyEnd = request.find("\r\n", keyPos);
        std::string clientKey = request.substr(keyPos + 19, keyEnd - (keyPos + 19));

        // RFC 6455 handshake calculation
        std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        uint8_t digest[20];
        sha1::calculate(clientKey + magic, digest);
        std::string acceptKey = base64Encode(digest, 20);

        std::ostringstream response;
        response << "HTTP/1.1 101 Switching Protocols\r\n"
                 << "Upgrade: websocket\r\n"
                 << "Connection: Upgrade\r\n"
                 << "Sec-WebSocket-Accept: " << acceptKey << "\r\n\r\n";

        std::string respStr = response.str();
        send(client, respStr.data(), static_cast<int>(respStr.size()), 0);

        {
            std::lock_guard<std::mutex> lk(m_clientsMutex);
            m_clients[sessionId] = clientSocket;
        }

        // Frame read loop
        while (m_running) {
            uint8_t header[2];
            int r = recv(client, reinterpret_cast<char*>(header), 2, 0);
            if (r <= 0) break;

            uint8_t opcode = header[0] & 0x0F;
            bool masked = (header[1] & 0x80) != 0;
            uint64_t payloadLen = header[1] & 0x7F;

            if (opcode == 0x08) break; // Close frame

            if (payloadLen == 126) {
                uint8_t ext[2];
                recv(client, reinterpret_cast<char*>(ext), 2, 0);
                payloadLen = (ext[0] << 8) | ext[1];
            } else if (payloadLen == 127) {
                uint8_t ext[8];
                recv(client, reinterpret_cast<char*>(ext), 8, 0);
                payloadLen = 0;
                for (int i = 0; i < 8; ++i) payloadLen = (payloadLen << 8) | ext[i];
            }

            uint8_t maskKey[4] = {0};
            if (masked) {
                recv(client, reinterpret_cast<char*>(maskKey), 4, 0);
            }

            std::vector<char> payload(payloadLen);
            size_t totalReceived = 0;
            while (totalReceived < payloadLen) {
                int n = recv(client, payload.data() + totalReceived, static_cast<int>(payloadLen - totalReceived), 0);
                if (n <= 0) break;
                totalReceived += n;
            }

            if (masked) {
                for (size_t i = 0; i < payloadLen; ++i) {
                    payload[i] ^= maskKey[i % 4];
                }
            }

            std::string payloadStr(payload.data(), payload.size());
            std::string reply;
            if (m_messageHandler) {
                reply = m_messageHandler(sessionId, payloadStr);
            }

            if (!reply.empty()) {
                sendToClient(sessionId, reply);
            }
        }

        {
            std::lock_guard<std::mutex> lk(m_clientsMutex);
            m_clients.erase(sessionId);
        }
        if (m_disconnectHandler) {
            m_disconnectHandler(sessionId);
        }
    } else {
        // HTTP GET request: Serve embedded single-page application
        std::string html = web::WebAssets::getIndexHtml();
        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n"
                 << "Content-Type: text/html; charset=UTF-8\r\n"
                 << "Content-Length: " << html.size() << "\r\n"
                 << "Connection: close\r\n\r\n"
                 << html;
        std::string respStr = response.str();
        send(client, respStr.data(), static_cast<int>(respStr.size()), 0);
    }

    CLOSE_SOCKET(client);
}

bool DesktopServer::sendToClient(uint32_t sessionId, const std::string& message) {
    uintptr_t sock = 0;
    {
        std::lock_guard<std::mutex> lk(m_clientsMutex);
        auto it = m_clients.find(sessionId);
        if (it == m_clients.end()) return false;
        sock = it->second;
    }

    std::vector<uint8_t> frame;
    frame.push_back(0x81); // FIN + Text opcode
    if (message.size() < 126) {
        frame.push_back(static_cast<uint8_t>(message.size()));
    } else if (message.size() <= 0xFFFF) {
        frame.push_back(126);
        frame.push_back(static_cast<uint8_t>((message.size() >> 8) & 0xFF));
        frame.push_back(static_cast<uint8_t>(message.size() & 0xFF));
    }
    frame.insert(frame.end(), message.begin(), message.end());
    int res = send(static_cast<socket_t>(sock), reinterpret_cast<const char*>(frame.data()), static_cast<int>(frame.size()), 0);
    return res > 0;
}

} // namespace desktop
} // namespace gamehub
