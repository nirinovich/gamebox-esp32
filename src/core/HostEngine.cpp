#include <gamehub/core/HostEngine.hpp>
#include <algorithm>
#include <sstream>

namespace gamehub {
namespace core {

HostEngine::HostEngine(INetworkAdapter* adapter, size_t maxRooms)
    : m_network(adapter), m_roomManager(maxRooms) {}

std::string HostEngine::extractJsonString(const std::string& json, const std::string& key) {
    std::string needle = "\"" + key + "\":";
    auto pos = json.find(needle);
    if (pos == std::string::npos) return "";
    auto start = json.find("\"", pos + needle.size());
    if (start == std::string::npos) return "";
    auto end = json.find("\"", start + 1);
    if (end == std::string::npos) return "";
    return json.substr(start + 1, end - start - 1);
}

void HostEngine::onClientConnect(uint32_t sessionId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sessions[sessionId] = std::make_unique<Session>(sessionId);
    if (m_network) {
        m_network->sendToClient(sessionId, m_roomManager.serializeDirectory());
    }
}

void HostEngine::onClientDisconnect(uint32_t sessionId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    detachPlayerFromRoom(sessionId);
    m_sessions.erase(sessionId);
}

void HostEngine::detachPlayerFromRoom(uint32_t sessionId) {
    Room* room = m_roomManager.findRoomByPlayer(sessionId);
    if (room) {
        std::string code = room->getCode();
        auto it = m_roomPlayers.find(code);
        if (it != m_roomPlayers.end()) {
            for (uint32_t other : it->second) {
                if (other != 0 && other != sessionId && m_network) {
                    m_network->sendToClient(other, "{\"type\":\"opponent_left\"}");
                }
            }
            m_roomPlayers.erase(it);
        }
        m_roomManager.handleDisconnect(sessionId);
        if (m_network) {
            m_network->broadcast(m_roomManager.serializeDirectory());
        }
    }
    m_sessionPlayerMap.erase(sessionId);

    auto itS = m_sessions.find(sessionId);
    if (itS != m_sessions.end()) {
        itS->second->setRoom(nullptr);
    }
}

void HostEngine::broadcastToRoom(const std::string& roomCode, const std::string& message) {
    if (!m_network) return;
    auto it = m_roomPlayers.find(roomCode);
    if (it != m_roomPlayers.end()) {
        for (uint32_t sid : it->second) {
            if (sid != 0) {
                m_network->sendToClient(sid, message);
            }
        }
    }
}

void HostEngine::broadcastStateToRoom(Room* room) {
    if (!room || !m_network) return;
    auto it = m_roomPlayers.find(room->getCode());
    if (it == m_roomPlayers.end()) return;

    for (uint32_t sid : it->second) {
        if (sid != 0) {
            uint32_t pIdx = m_sessionPlayerMap[sid];
            m_network->sendToClient(sid, room->serializeStateForPlayer(pIdx));
        }
    }
}

std::string HostEngine::onClientMessage(uint32_t sessionId, const std::string& rawMessage) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // 0a. Ping / Latency Check
    if (rawMessage.find("\"ping\"") != std::string::npos) {
        std::string ts = "0";
        auto tsPos = rawMessage.find("\"ts\":");
        if (tsPos != std::string::npos) {
            auto start = tsPos + 5;
            auto end = rawMessage.find_first_of(",}", start);
            if (end != std::string::npos) ts = rawMessage.substr(start, end - start);
            else ts = rawMessage.substr(start);
        }
        std::string pong = "{\"type\":\"pong\",\"ts\":" + ts + "}";
        if (m_network) m_network->sendToClient(sessionId, pong);
        return pong;
    }

    // 0b. Query Live Room Directory
    if (rawMessage.find("\"cmd\":\"get_rooms\"") != std::string::npos ||
        rawMessage.find("\"get_rooms\"") != std::string::npos) {
        std::string dir = m_roomManager.serializeDirectory();
        if (m_network) m_network->sendToClient(sessionId, dir);
        return dir;
    }

    // 1. Create Room (Multiplayer)
    if (rawMessage.find("\"cmd\":\"create\"") != std::string::npos) {
        detachPlayerFromRoom(sessionId);

        std::string gId = extractJsonString(rawMessage, "game");
        const auto* regEntry = !gId.empty() ? GameRegistry::instance().find(gId) : nullptr;

        size_t maxPlayers = 2;
        if (regEntry) {
            maxPlayers = regEntry->descriptor.maxPlayers;
        }
        if (rawMessage.find("\"players\":2") != std::string::npos || rawMessage.find("\"max\":2") != std::string::npos) {
            maxPlayers = 2;
        } else if (rawMessage.find("\"players\":3") != std::string::npos || rawMessage.find("\"max\":3") != std::string::npos) {
            maxPlayers = 3;
        }

        std::string hostName = extractJsonString(rawMessage, "host");
        if (hostName.empty()) hostName = extractJsonString(rawMessage, "name");
        if (hostName.empty()) hostName = "Player 1";

        GameId gameId = regEntry ? regEntry->descriptor.id : GameId(gId.empty() ? "domino_pvp" : gId);
        Room* newRoom = m_roomManager.createRoom(gameId, sessionId, hostName, maxPlayers);
        if (!newRoom) {
            std::string err = "{\"type\":\"error\",\"msg\":\"Room capacity reached (max 4).\"}";
            if (m_network) m_network->sendToClient(sessionId, err);
            return err;
        }

        m_sessionPlayerMap[sessionId] = 1;
        m_roomPlayers[newRoom->getCode()] = {sessionId};

        if (m_network) {
            m_network->broadcast(m_roomManager.serializeDirectory());
        }

        std::ostringstream oss;
        oss << "{\"type\":\"room_created\",\"code\":\"" << newRoom->getCode()
            << "\",\"game\":\"" << gameId.c_str()
            << "\",\"player\":1,\"host\":\"" << hostName
            << "\",\"max\":" << maxPlayers << "}";
        std::string resp = oss.str();
        if (m_network) m_network->sendToClient(sessionId, resp);
        return resp;
    }

    // 2. Join Multiplayer Room
    if (rawMessage.find("\"cmd\":\"join\"") != std::string::npos) {
        detachPlayerFromRoom(sessionId);
        std::string code = extractJsonString(rawMessage, "code");
        if (code.empty()) code = extractJsonString(rawMessage, "room");

        Room* room = m_roomManager.joinRoom(code, sessionId);
        if (!room) {
            std::string err = "{\"type\":\"error\",\"msg\":\"Room not found or already full.\"}";
            if (m_network) m_network->sendToClient(sessionId, err);
            return err;
        }

        uint32_t pNum = static_cast<uint32_t>(room->getPlayerCount());
        m_sessionPlayerMap[sessionId] = pNum;
        m_roomPlayers[code].push_back(sessionId);

        std::string gname = room->getGameId().c_str();

        if (m_network) {
            m_network->broadcast(m_roomManager.serializeDirectory());
            std::ostringstream oss;
            oss << "{\"type\":\"room_joined\",\"code\":\"" << code
                << "\",\"game\":\"" << gname
                << "\",\"player\":" << pNum
                << ",\"host\":\"" << room->getHostName()
                << "\",\"max\":" << room->getMaxPlayers() << "}";
            m_network->sendToClient(sessionId, oss.str());

            if (room->getPlayerCount() >= room->getMaxPlayers()) {
                if (room->getGame()) {
                    room->getGame()->setPlayerCount(static_cast<int>(room->getPlayerCount()));
                }
                for (uint32_t sidPlayer : m_roomPlayers[code]) {
                    m_network->sendToClient(sidPlayer, "{\"type\":\"room_ready\"}");
                }
                broadcastStateToRoom(room);
            } else {
                std::ostringstream ossCount;
                ossCount << "{\"type\":\"room_waiting_update\",\"count\":" << room->getPlayerCount() << ",\"max\":" << room->getMaxPlayers() << "}";
                for (uint32_t sidPlayer : m_roomPlayers[code]) {
                    m_network->sendToClient(sidPlayer, ossCount.str());
                }
            }
        }
        return "";
    }

    // 2b. Start Room Early (Host starts 3P game with 2 players)
    if (rawMessage.find("\"cmd\":\"start_room\"") != std::string::npos ||
        rawMessage.find("\"cmd\":\"start_early\"") != std::string::npos) {
        Room* room = m_roomManager.findRoomByPlayer(sessionId);
        if (room && room->getPlayerCount() >= 2 && !room->isFinished()) {
            room->startEarly();
            if (m_network) {
                for (uint32_t sidPlayer : m_roomPlayers[room->getCode()]) {
                    m_network->sendToClient(sidPlayer, "{\"type\":\"room_ready\"}");
                }
            }
            broadcastStateToRoom(room);
        }
        return "";
    }

    // 3. Leave / Cancel Match
    if (rawMessage.find("\"cmd\":\"leave\"") != std::string::npos) {
        detachPlayerFromRoom(sessionId);
        std::string resp = "{\"type\":\"left\"}";
        if (m_network) m_network->sendToClient(sessionId, resp);
        return resp;
    }

    // 3b. Rematch in Multiplayer Room
    if (rawMessage.find("\"cmd\":\"rematch\"") != std::string::npos) {
        Room* room = m_roomManager.findRoomByPlayer(sessionId);
        if (room && room->isFinished()) {
            uint32_t pNum = m_sessionPlayerMap[sessionId];
            room->requestRematch(pNum);

            if (room->isRematchReady()) {
                room->resetForRematch();
                for (uint32_t sidPlayer : m_roomPlayers[room->getCode()]) {
                    if (m_network) m_network->sendToClient(sidPlayer, "{\"type\":\"room_ready\"}");
                }
                broadcastStateToRoom(room);
            } else {
                std::ostringstream oss;
                oss << "{\"type\":\"rematch_update\",\"requested\":" << room->getRematchRequestCount()
                    << ",\"total\":" << room->getPlayerCount() << "}";
                for (uint32_t sidPlayer : m_roomPlayers[room->getCode()]) {
                    if (m_network) m_network->sendToClient(sidPlayer, oss.str());
                }
            }
        }
        return "";
    }

    // 4. Start Solo Game (Auto-detach from any multiplayer room)
    if (rawMessage.find("\"cmd\":\"start\"") != std::string::npos ||
        rawMessage.find("\"start\"") != std::string::npos) {
        detachPlayerFromRoom(sessionId);

        std::string gId = extractJsonString(rawMessage, "game");
        const auto* regEntry = !gId.empty() ? GameRegistry::instance().find(gId) : nullptr;
        GameId gameId = regEntry ? regEntry->descriptor.id : GameId(gId.empty() ? "domino_solo" : gId);
        Room* room = m_roomManager.createSoloRoom(gameId, sessionId);

        auto itS = m_sessions.find(sessionId);
        if (itS == m_sessions.end()) {
            itS = m_sessions.emplace(sessionId, std::make_unique<Session>(sessionId)).first;
        }
        itS->second->setRoom(room);
        m_sessionPlayerMap[sessionId] = 1;

        std::string reply = room ? room->serializeStateForPlayer(1) : "{\"type\":\"error\",\"msg\":\"Room capacity reached\"}";
        if (m_network && !reply.empty()) {
            m_network->sendToClient(sessionId, reply);
        }
        return reply;
    }

    // 5. In-Game Actions in Active Room (Multiplayer or Solo)
    Room* activeRoom = m_roomManager.findRoomByPlayer(sessionId);
    if (activeRoom && !activeRoom->isFinished()) {
        uint32_t playerNum = 1;
        auto itP = m_sessionPlayerMap.find(sessionId);
        if (itP != m_sessionPlayerMap.end()) {
            playerNum = itP->second;
        }

        activeRoom->handleInput(playerNum, rawMessage);
        if (activeRoom->getMaxPlayers() > 1) {
            broadcastStateToRoom(activeRoom);
            return "";
        } else {
            std::string reply = activeRoom->serializeStateForPlayer(playerNum);
            if (m_network) m_network->sendToClient(sessionId, reply);
            return reply;
        }
    }

    // 6. Fallback to Session handling (ping, custom messages)
    auto itS = m_sessions.find(sessionId);
    if (itS == m_sessions.end()) {
        itS = m_sessions.emplace(sessionId, std::make_unique<Session>(sessionId)).first;
    }
    std::string reply = itS->second->handleMessage(rawMessage);
    if (m_network && !reply.empty()) {
        m_network->sendToClient(sessionId, reply);
    }
    return reply;
}

void HostEngine::tick(float dt) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // Unified ticking loop across all active rooms in RoomManager
    for (const auto& roomPtr : m_roomManager.getRooms()) {
        Room* r = roomPtr.get();
        if (!r || r->isFinished() || !r->shouldTick()) continue;

        bool isRealTime = r->getGame() && r->getGame()->isRealTime();

        if (isRealTime) {
            r->tick(dt);
            if (r->getMaxPlayers() > 1) {
                broadcastStateToRoom(r);
            } else if (!r->getPlayers().empty()) {
                if (m_network) {
                    m_network->sendToClient(r->getPlayers()[0], r->serializeStateForPlayer(1));
                }
            }
        }
    }
}

} // namespace core
} // namespace gamehub
