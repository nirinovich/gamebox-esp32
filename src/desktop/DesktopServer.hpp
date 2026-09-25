#pragma once

#include <cstdint>
#include <string>
#include <atomic>
#include <thread>
#include <vector>
#include <memory>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <gamehub/core/Session.hpp>

namespace gamehub {
namespace desktop {

class DesktopServer {
public:
    explicit DesktopServer(uint16_t port = 8080);
    ~DesktopServer();

    bool start(bool autoFallback = true);
    void stop();
    bool isRunning() const { return m_running; }
    uint16_t getPort() const { return m_port; }

    using MessageHandler = std::function<std::string(uint32_t sessionId, const std::string& msg)>;
    void setMessageHandler(MessageHandler handler) { m_messageHandler = handler; }

    using DisconnectHandler = std::function<void(uint32_t sessionId)>;
    void setDisconnectHandler(DisconnectHandler handler) { m_disconnectHandler = handler; }

    bool sendToClient(uint32_t sessionId, const std::string& message);

private:
    void acceptLoop();
    void handleClient(uintptr_t clientSocket, uint32_t sessionId);
    
    uint16_t m_port;
    std::atomic<bool> m_running{false};
    uintptr_t m_serverSocket{0};
    std::thread m_acceptThread;
    std::atomic<uint32_t> m_nextSessionId{1};
    MessageHandler m_messageHandler;
    DisconnectHandler m_disconnectHandler;

    std::unordered_map<uint32_t, uintptr_t> m_clients;
    std::mutex m_clientsMutex;
};

} // namespace desktop
} // namespace gamehub
