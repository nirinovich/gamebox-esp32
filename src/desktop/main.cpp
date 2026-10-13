#include "DesktopServer.hpp"
#include <gamehub/core/HostEngine.hpp>
#include <gamehub/core/INetworkAdapter.hpp>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

namespace {

class DesktopNetworkAdapter : public gamehub::core::INetworkAdapter {
public:
    explicit DesktopNetworkAdapter(gamehub::desktop::DesktopServer& server) : m_server(server) {}
    void sendToClient(uint32_t sessionId, const std::string& message) override {
        m_server.sendToClient(sessionId, message);
    }
    void broadcast(const std::string& message) override {
        m_server.broadcast(message);
    }
private:
    gamehub::desktop::DesktopServer& m_server;
};

} // namespace

int main(int argc, char* argv[]) {
    std::cout << "=========================================" << std::endl;
    std::cout << "     ESP32 Game Hub - Desktop Runner     " << std::endl;
    std::cout << "=========================================" << std::endl;

    uint16_t initialPort = 8080;
    if (const char* envPort = std::getenv("PORT")) {
        try { initialPort = static_cast<uint16_t>(std::stoi(envPort)); } catch (...) {}
    } else if (const char* envGhPort = std::getenv("GAMEHUB_PORT")) {
        try { initialPort = static_cast<uint16_t>(std::stoi(envGhPort)); } catch (...) {}
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--port" && i + 1 < argc) {
            try { initialPort = static_cast<uint16_t>(std::stoi(argv[++i])); } catch (...) {}
        } else if (arg.rfind("--port=", 0) == 0) {
            try { initialPort = static_cast<uint16_t>(std::stoi(arg.substr(7))); } catch (...) {}
        } else if (!arg.empty() && std::isdigit(static_cast<unsigned char>(arg[0]))) {
            try { initialPort = static_cast<uint16_t>(std::stoi(arg)); } catch (...) {}
        }
    }

    gamehub::desktop::DesktopServer server(initialPort);
    DesktopNetworkAdapter adapter(server);
    gamehub::core::HostEngine engine(&adapter, 4);

    server.setConnectHandler([&](uint32_t sessionId) {
        engine.onClientConnect(sessionId);
    });

    server.setDisconnectHandler([&](uint32_t sessionId) {
        engine.onClientDisconnect(sessionId);
    });

    server.setMessageHandler([&](uint32_t sessionId, const std::string& msg) -> std::string {
        return engine.onClientMessage(sessionId, msg);
    });

    if (!server.start(true)) {
        std::cerr << "[ERROR] Failed to start desktop server on port " << initialPort << " (or consecutive fallback ports)." << std::endl;
        return 1;
    }

    uint16_t activePort = server.getPort();
    std::cout << "[INFO] Server running on http://localhost:" << activePort << std::endl;
    std::cout << "[INFO] Authoritative HostEngine active with GoF State & Strategy patterns!" << std::endl;
    std::cout << "[INFO] Press Enter in this console to stop the server..." << std::endl;

    // Real-time ticking loop (15 Hz)
    std::atomic<bool> ticking{true};
    std::thread tickThread([&]() {
        while (ticking) {
            std::this_thread::sleep_for(std::chrono::milliseconds(66));
            engine.tick(0.066f);
        }
    });

    std::cin.get();

    std::cout << "[INFO] Stopping server..." << std::endl;
    ticking = false;
    if (tickThread.joinable()) tickThread.join();
    server.stop();
    std::cout << "[INFO] Server stopped cleanly. Goodbye!" << std::endl;

    return 0;
}
