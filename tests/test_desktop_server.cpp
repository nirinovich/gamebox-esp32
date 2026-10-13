#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <gamehub/web/WebAssets.hpp>
#include "../src/desktop/DesktopServer.hpp"

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
#include <arpa/inet.h>
#include <unistd.h>
using socket_t = int;
#define IS_INVALID_SOCKET(s) ((s) < 0)
#define CLOSE_SOCKET(s) close(s)
#endif

int main() {
    std::cout << "[TEST] Running DesktopServer HTTP GET Integration Test..." << std::endl;

#if defined(_WIN32)
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    // Start DesktopServer on port 18080 (or auto-fallback)
    gamehub::desktop::DesktopServer server(18080);
    bool started = server.start(true);
    assert(started && "DesktopServer must start successfully");
    uint16_t port = server.getPort();
    std::cout << "  Server running on port: " << port << std::endl;

    // Give the server thread a moment to enter accept loop
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Connect client socket
    socket_t clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert(!IS_INVALID_SOCKET(clientSock) && "Client socket must be valid");

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    int connRes = connect(clientSock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
    assert(connRes == 0 && "Client connection to DesktopServer must succeed");

    // Send HTTP GET request
    std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
    int sent = send(clientSock, request.data(), static_cast<int>(request.size()), 0);
    assert(sent == static_cast<int>(request.size()) && "Full HTTP GET request must be sent");

    // Read full response
    std::vector<char> responseBuffer;
    char chunk[4096];
    int bytesRead = 0;
    while ((bytesRead = recv(clientSock, chunk, sizeof(chunk), 0)) > 0) {
        responseBuffer.insert(responseBuffer.end(), chunk, chunk + bytesRead);
    }
    CLOSE_SOCKET(clientSock);

    std::string response(responseBuffer.begin(), responseBuffer.end());
    assert(!response.empty() && "HTTP response must not be empty");

    // 1. Verify HTTP status code is 200 OK
    assert(response.find("HTTP/1.1 200 OK") != std::string::npos && "HTTP status code must be 200 OK");
    std::cout << "  [PASS] HTTP status code is 200 OK." << std::endl;

    // 2. Verify Content-Encoding: gzip header is present
    assert(response.find("Content-Encoding: gzip") != std::string::npos && "Content-Encoding: gzip header must be present");
    std::cout << "  [PASS] Content-Encoding: gzip header present." << std::endl;

    // 3. Verify response payload size matches WebAssets::getIndexHtmlSize()
    size_t headerEnd = response.find("\r\n\r\n");
    assert(headerEnd != std::string::npos && "HTTP response must contain header-body delimiter");
    size_t bodySize = responseBuffer.size() - (headerEnd + 4);
    assert(bodySize == gamehub::web::WebAssets::getIndexHtmlSize() && "Response payload size must match WebAssets size");
    std::cout << "  [PASS] Payload size matches WebAssets size (" << bodySize << " bytes)." << std::endl;

    server.stop();

#if defined(_WIN32)
    WSACleanup();
#endif

    std::cout << "[TEST] DesktopServer Integration Test Passed!" << std::endl;
    return 0;
}
