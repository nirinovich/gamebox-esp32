#include "DesktopServer.hpp"
#include <httplib.h>
#include <gamehub/web/WebAssets.hpp>
#include <iostream>

namespace gamehub {
namespace desktop {

DesktopServer::DesktopServer(uint16_t port) : m_port(port) {}

DesktopServer::~DesktopServer() {
    stop();
}

void DesktopServer::setupRoutes(httplib::Server& server) {
    // 1. Serve embedded ClientShell (RFC 1952 Gzip compressed)
    auto serveSpa = [](const httplib::Request&, httplib::Response& res) {
        std::string_view gzipPayload = web::WebAssets::getGzipPayloadView();
        res.set_content(gzipPayload.data(), gzipPayload.size(), "text/html; charset=UTF-8");
        res.set_header("Content-Encoding", "gzip");
    };

    server.Get("/", serveSpa);
    server.Get("/index.html", serveSpa);

    // 2. RFC 6455 WebSocket routes
    auto wsHandler = [this](const httplib::Request& /*req*/, httplib::ws::WebSocket& ws) {
        uint32_t sessionId = m_nextSessionId++;

        {
            std::lock_guard<std::mutex> lk(m_clientsMutex);
            m_clients[sessionId] = &ws;
        }

        if (m_connectHandler) {
            m_connectHandler(sessionId);
        }

        std::string msg;
        while (m_running) {
            auto res = ws.read(msg);
            if (res == httplib::ws::Fail) {
                break;
            }
            if (res == httplib::ws::Timeout) {
                continue;
            }

            std::string reply;
            if (m_messageHandler) {
                reply = m_messageHandler(sessionId, msg);
            }
            if (!reply.empty()) {
                ws.send(reply);
            }
        }

        {
            std::lock_guard<std::mutex> lk(m_clientsMutex);
            m_clients.erase(sessionId);
        }

        if (m_disconnectHandler) {
            m_disconnectHandler(sessionId);
        }
    };

    server.WebSocket("/ws", wsHandler);
    server.WebSocket("/", wsHandler);
}

bool DesktopServer::start(bool autoFallback) {
    if (m_running) return true;

    uint16_t startPort = m_port;
    const uint16_t maxAttempts = autoFallback ? 20 : 1;

    for (uint16_t i = 0; i < maxAttempts; ++i) {
        uint16_t currentTryPort = startPort + i;
        auto server = std::make_unique<httplib::Server>();
        setupRoutes(*server);

        if (server->bind_to_port("0.0.0.0", currentTryPort)) {
            m_port = currentTryPort;
            m_server = std::move(server);
            m_running = true;

            m_serverThread = std::thread([this]() {
                m_server->listen_after_bind();
                m_running = false;
            });

            m_server->wait_until_ready();
            std::cout << "[DesktopServer (httplib)] Listening on http://localhost:" << m_port << std::endl;
            return true;
        }

        if (autoFallback && i + 1 < maxAttempts) {
            std::cout << "[DesktopServer] Port " << currentTryPort << " is in use. Trying port " << (currentTryPort + 1) << "..." << std::endl;
        }
    }

    return false;
}

void DesktopServer::stop() {
    if (!m_running && !m_server) return;
    m_running = false;

    if (m_server) {
        m_server->stop();
    }

    if (m_serverThread.joinable()) {
        m_serverThread.join();
    }

    {
        std::lock_guard<std::mutex> lk(m_clientsMutex);
        m_clients.clear();
    }
    m_server.reset();
}

bool DesktopServer::sendToClient(uint32_t sessionId, const std::string& message) {
    std::lock_guard<std::mutex> lk(m_clientsMutex);
    auto it = m_clients.find(sessionId);
    if (it == m_clients.end() || !it->second) return false;
    if (it->second->is_open()) {
        return it->second->send(message);
    }
    return false;
}

void DesktopServer::broadcast(const std::string& message) {
    std::lock_guard<std::mutex> lk(m_clientsMutex);
    for (const auto& kv : m_clients) {
        if (kv.second && kv.second->is_open()) {
            kv.second->send(message);
        }
    }
}

} // namespace desktop
} // namespace gamehub
