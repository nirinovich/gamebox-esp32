#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "INetworkAdapter.hpp"
#include "RoomManager.hpp"
#include "Session.hpp"
#include "GameTypes.hpp"

namespace gamehub {
namespace core {

/**
 * @brief Authoritative headless orchestrator for the Gamebox platform.
 * Pure C++17 core domain class coordinating connections, rooms, routing, and ticking.
 * Eliminates platform code duplication between DesktopServer and ESPAsyncWebServer.
 */
class HostEngine {
public:
    explicit HostEngine(INetworkAdapter* adapter = nullptr, size_t maxRooms = 4);
    virtual ~HostEngine() = default;

    void onClientConnect(uint32_t sessionId);
    void onClientDisconnect(uint32_t sessionId);
    std::string onClientMessage(uint32_t sessionId, const std::string& rawMessage);
    void tick(float dt);

    void detachPlayerFromRoom(uint32_t sessionId);
    void broadcastToRoom(const std::string& roomCode, const std::string& message);
    void broadcastStateToRoom(Room* room);

    RoomManager& getRoomManager() { return m_roomManager; }
    const RoomManager& getRoomManager() const { return m_roomManager; }

    void setNetworkAdapter(INetworkAdapter* adapter) { m_network = adapter; }

private:
    static std::string extractJsonString(const std::string& json, const std::string& key);

    INetworkAdapter* m_network{nullptr};
    RoomManager m_roomManager;
    std::unordered_map<uint32_t, std::unique_ptr<Session>> m_sessions;
    std::unordered_map<uint32_t, uint32_t> m_sessionPlayerMap; // sessionId -> playerNum (1, 2, 3)
    std::unordered_map<std::string, std::vector<uint32_t>> m_roomPlayers; // roomCode -> [sessionIds]
    std::mutex m_mutex;
};

} // namespace core
} // namespace gamehub
