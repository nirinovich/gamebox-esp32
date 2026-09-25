#include <gamehub/core/RoomManager.hpp>
#include <algorithm>
#include <random>

#include <sstream>

namespace gamehub {
namespace core {

RoomManager::RoomManager(size_t maxRooms) : m_maxRooms(maxRooms) {}

std::string RoomManager::generateUniqueCode() {
    static const char CHARS[] = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789"; // Omit ambiguous 0/O and 1/I
    static std::mt19937 rng(1337);
    std::uniform_int_distribution<size_t> dist(0, sizeof(CHARS) - 2);

    for (int attempts = 0; attempts < 1000; ++attempts) {
        std::string code;
        for (int i = 0; i < 4; ++i) {
            code += CHARS[dist(rng)];
        }
        if (!findRoomByCode(code)) {
            return code;
        }
    }
    return "AAAA";
}

Room* RoomManager::createRoom(GameType type, PlayerId hostId, const std::string& hostName, size_t maxPlayers) {
    if (m_rooms.size() >= m_maxRooms) {
        return nullptr;
    }

    std::string code = generateUniqueCode();
    auto room = std::make_unique<Room>(code, type, hostName, maxPlayers);
    room->init();
    room->addPlayer(hostId);

    Room* ptr = room.get();
    m_rooms.push_back(std::move(room));
    m_playerToRoom[hostId] = code;
    return ptr;
}

void RoomManager::removeRoom(const std::string& code) {
    std::vector<PlayerId> playersToRemove;
    for (const auto& pair : m_playerToRoom) {
        if (pair.second == code) {
            playersToRemove.push_back(pair.first);
        }
    }
    for (PlayerId id : playersToRemove) {
        m_playerToRoom.erase(id);
    }
    m_rooms.erase(std::remove_if(m_rooms.begin(), m_rooms.end(),
        [&code](const std::unique_ptr<Room>& r) {
            return r->getCode() == code;
        }), m_rooms.end());
}

std::string RoomManager::serializeDirectory() const {
    std::ostringstream oss;
    oss << "{\"type\":\"room_directory\",\"rooms\":[";
    bool first = true;
    for (const auto& r : m_rooms) {
        if (r->getPlayerCount() < r->getMaxPlayers() && !r->isFinished()) {
            if (!first) oss << ",";
            first = false;
            std::string gname = "ttt_pvp";
            switch (r->getGameType()) {
                case GameType::SNAKE_DUEL: gname = "snake_duel"; break;
                case GameType::TICTACTOE_PVP: gname = "ttt_pvp"; break;
                case GameType::CONNECT_FOUR_PVP: gname = "connect4_pvp"; break;
                case GameType::PONG_DUEL: gname = "pong_duel"; break;
                case GameType::TRON_DUEL: gname = "tron_duel"; break;
                case GameType::BATTLESHIP_PVP: gname = "battleship_pvp"; break;
                case GameType::DOMINO_PVP: gname = "domino_pvp"; break;
                default: break;
            }
            oss << "{\"code\":\"" << r->getCode() << "\","
                << "\"game\":\"" << gname << "\","
                << "\"host\":\"" << r->getHostName() << "\","
                << "\"players\":" << r->getPlayerCount() << ","
                << "\"max\":" << r->getMaxPlayers() << "}";
        }
    }
    oss << "]}";
    return oss.str();
}

Room* RoomManager::joinRoom(const std::string& code, PlayerId playerId) {
    Room* room = findRoomByCode(code);
    if (!room) return nullptr;

    if (room->getPlayerCount() >= room->getMaxPlayers()) {
        return nullptr; // Room full
    }

    room->addPlayer(playerId);
    m_playerToRoom[playerId] = code;
    return room;
}

Room* RoomManager::findRoomByCode(const std::string& code) {
    for (const auto& r : m_rooms) {
        if (r->getCode() == code) {
            return r.get();
        }
    }
    return nullptr;
}

Room* RoomManager::findRoomByPlayer(PlayerId playerId) {
    auto it = m_playerToRoom.find(playerId);
    if (it != m_playerToRoom.end()) {
        return findRoomByCode(it->second);
    }
    return nullptr;
}

void RoomManager::handleDisconnect(PlayerId playerId) {
    auto it = m_playerToRoom.find(playerId);
    if (it == m_playerToRoom.end()) return;

    std::string code = it->second;
    m_playerToRoom.erase(it);

    // Also remove any other players mapped to this room
    std::vector<PlayerId> othersToRemove;
    for (const auto& pair : m_playerToRoom) {
        if (pair.second == code) {
            othersToRemove.push_back(pair.first);
        }
    }
    for (PlayerId id : othersToRemove) {
        m_playerToRoom.erase(id);
    }

    // Destroy the room
    m_rooms.erase(std::remove_if(m_rooms.begin(), m_rooms.end(),
        [&code](const std::unique_ptr<Room>& r) {
            return r->getCode() == code;
        }), m_rooms.end());
}

void RoomManager::tickAll(float dt) {
    for (auto& room : m_rooms) {
        room->tick(dt);
    }
}

} // namespace core
} // namespace gamehub
