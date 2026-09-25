#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Room.hpp"
#include "GameTypes.hpp"

namespace gamehub {
namespace core {

class RoomManager {
public:
    explicit RoomManager(size_t maxRooms = 4);
    virtual ~RoomManager() = default;

    Room* createRoom(GameType type, PlayerId hostId);
    Room* joinRoom(const std::string& code, PlayerId playerId);
    Room* findRoomByCode(const std::string& code);
    Room* findRoomByPlayer(PlayerId playerId);
    void handleDisconnect(PlayerId playerId);
    void tickAll(float dt);

    size_t getRoomCount() const { return m_rooms.size(); }
    size_t getMaxRooms() const { return m_maxRooms; }

private:
    std::string generateUniqueCode();

    size_t m_maxRooms;
    std::vector<std::unique_ptr<Room>> m_rooms;
    std::unordered_map<PlayerId, std::string> m_playerToRoom;
};

} // namespace core
} // namespace gamehub
