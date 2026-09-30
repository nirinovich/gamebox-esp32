#pragma once

#include <cstdint>
#include <string>

namespace gamehub {
namespace core {

/**
 * @brief Abstract interface for platform-specific network transports.
 * Decouples the pure C++ core domain engine from Winsock/DesktopServer and ESPAsyncWebServer.
 */
class INetworkAdapter {
public:
    virtual ~INetworkAdapter() = default;

    /**
     * @brief Send a text frame directly to a single connected client session.
     */
    virtual void sendToClient(uint32_t sessionId, const std::string& message) = 0;

    /**
     * @brief Broadcast a text frame to all connected clients in the lobby/host.
     */
    virtual void broadcast(const std::string& message) = 0;
};

} // namespace core
} // namespace gamehub
