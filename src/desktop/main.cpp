#include "DesktopServer.hpp"
#include <gamehub/core/Session.hpp>
#include <gamehub/core/RoomManager.hpp>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>
#include <sstream>

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

    gamehub::core::RoomManager roomManager(4);
    std::unordered_map<uint32_t, std::unique_ptr<gamehub::core::Session>> sessions;
    std::unordered_map<uint32_t, uint32_t> sessionPlayerMap; // sessionId -> playerIndex in room (1 or 2)
    std::unordered_map<std::string, std::pair<uint32_t, uint32_t>> roomPlayers; // roomCode -> (p1Session, p2Session)
    std::mutex engineMutex;

    gamehub::desktop::DesktopServer server(initialPort);

    static auto extractJsonString = [](const std::string& json, const std::string& key) -> std::string {
        std::string needle = "\"" + key + "\":";
        auto pos = json.find(needle);
        if (pos == std::string::npos) return "";
        auto start = json.find("\"", pos + needle.size());
        if (start == std::string::npos) return "";
        auto end = json.find("\"", start + 1);
        if (end == std::string::npos) return "";
        return json.substr(start + 1, end - start - 1);
    };

    auto detachPlayerFromRoom = [&](uint32_t sid) {
        gamehub::core::Room* room = roomManager.findRoomByPlayer(sid);
        if (room) {
            std::string code = room->getCode();
            auto itR = roomPlayers.find(code);
            if (itR != roomPlayers.end()) {
                uint32_t other = (itR->second.first == sid) ? itR->second.second : itR->second.first;
                if (other != 0) {
                    server.sendToClient(other, "{\"type\":\"opponent_left\"}");
                }
                roomPlayers.erase(itR);
            }
            roomManager.handleDisconnect(sid);
            server.broadcast(roomManager.serializeDirectory());
        }
        sessionPlayerMap.erase(sid);
    };

    // Disconnect Handler
    server.setDisconnectHandler([&](uint32_t sessionId) {
        std::lock_guard<std::mutex> lock(engineMutex);
        sessions.erase(sessionId);
        detachPlayerFromRoom(sessionId);
    });

    // Message Handler
    server.setMessageHandler([&](uint32_t sessionId, const std::string& msg) -> std::string {
        std::lock_guard<std::mutex> lock(engineMutex);

        auto it = sessions.find(sessionId);
        if (it == sessions.end()) {
            it = sessions.emplace(sessionId, std::make_unique<gamehub::core::Session>(sessionId)).first;
        }

        // 0. Query Live Room Directory
        if (msg.find("\"cmd\":\"get_rooms\"") != std::string::npos) {
            return roomManager.serializeDirectory();
        }

        // 1. Create Multiplayer Room (Auto-detach previous room)
        if (msg.find("\"cmd\":\"create\"") != std::string::npos) {
            detachPlayerFromRoom(sessionId);

            gamehub::core::GameType gtype = gamehub::core::GameType::TICTACTOE_PVP;
            if (msg.find("\"snake_duel\"") != std::string::npos) {
                gtype = gamehub::core::GameType::SNAKE_DUEL;
            } else if (msg.find("\"connect4_pvp\"") != std::string::npos) {
                gtype = gamehub::core::GameType::CONNECT_FOUR_PVP;
            } else if (msg.find("\"pong_duel\"") != std::string::npos) {
                gtype = gamehub::core::GameType::PONG_DUEL;
            } else if (msg.find("\"tron_duel\"") != std::string::npos) {
                gtype = gamehub::core::GameType::TRON_DUEL;
            } else if (msg.find("\"battleship_pvp\"") != std::string::npos) {
                gtype = gamehub::core::GameType::BATTLESHIP_PVP;
            }

            std::string hostName = extractJsonString(msg, "host");
            if (hostName.empty()) hostName = "Player 1";

            gamehub::core::Room* newRoom = roomManager.createRoom(gtype, sessionId, hostName);
            if (!newRoom) {
                return "{\"type\":\"error\",\"msg\":\"Room capacity reached (max 4).\"}";
            }

            sessionPlayerMap[sessionId] = 1;
            roomPlayers[newRoom->getCode()] = {sessionId, 0};

            // Broadcast updated directory to all clients in lobby
            server.broadcast(roomManager.serializeDirectory());

            std::ostringstream oss;
            oss << "{\"type\":\"room_created\",\"code\":\"" << newRoom->getCode() << "\",\"player\":1,\"host\":\"" << hostName << "\"}";
            return oss.str();
        }

        // 2. Join Multiplayer Room (Auto-detach previous room)
        if (msg.find("\"cmd\":\"join\"") != std::string::npos) {
            detachPlayerFromRoom(sessionId);

            std::string code = extractJsonString(msg, "code");
            if (code.empty()) return "{\"type\":\"error\",\"msg\":\"Missing code\"}";

            gamehub::core::Room* room = roomManager.joinRoom(code, sessionId);
            if (!room) {
                return "{\"type\":\"error\",\"msg\":\"Room not found or already full.\"}";
            }

            sessionPlayerMap[sessionId] = 2;
            roomPlayers[code].second = sessionId;

            // Broadcast updated directory (room is no longer joinable)
            server.broadcast(roomManager.serializeDirectory());

            // Notify joiner
            std::ostringstream oss;
            oss << "{\"type\":\"room_joined\",\"code\":\"" << code << "\",\"player\":2,\"host\":\"" << room->getHostName() << "\"}";
            server.sendToClient(sessionId, oss.str());

            // Notify host and start game
            std::string state = room->serializeState();
            uint32_t hostSession = roomPlayers[code].first;
            server.sendToClient(hostSession, "{\"type\":\"room_ready\"}");
            server.sendToClient(hostSession, state);
            server.sendToClient(sessionId, state);

            return "";
        }

        // 3. Leave / Cancel Room
        if (msg.find("\"cmd\":\"leave\"") != std::string::npos) {
            detachPlayerFromRoom(sessionId);
            return "{\"type\":\"left\"}";
        }

        // 4. Start Solo Game (Auto-detach from any previous multiplayer room!)
        if (msg.find("\"cmd\":\"start\"") != std::string::npos) {
            detachPlayerFromRoom(sessionId);
            return it->second->handleMessage(msg);
        }

        // 5. Multiplayer Moves & Inputs (only route if active and NOT finished!)
        gamehub::core::Room* activeRoom = roomManager.findRoomByPlayer(sessionId);
        if (activeRoom && activeRoom->getPlayerCount() >= 2 && !activeRoom->isFinished()) {
            uint32_t playerNum = sessionPlayerMap[sessionId];
            activeRoom->handleInput(playerNum, msg);
            std::string state = activeRoom->serializeState();

            // Broadcast to both players
            auto itRoom = roomPlayers.find(activeRoom->getCode());
            if (itRoom != roomPlayers.end()) {
                server.sendToClient(itRoom->second.first, state);
                server.sendToClient(itRoom->second.second, state);
            }
            return "";
        }

        // 6. Fallback to session handling (Solo games, ping, etc.)
        return it->second->handleMessage(msg);
    });

    if (!server.start(true)) {
        std::cerr << "[ERROR] Failed to start desktop server on port " << initialPort << " (or consecutive fallback ports)." << std::endl;
        return 1;
    }

    uint16_t activePort = server.getPort();
    std::cout << "[INFO] Server running on http://localhost:" << activePort << std::endl;
    std::cout << "[INFO] Multiplayer lobby, solo games, and room codes active!" << std::endl;
    std::cout << "[INFO] Press Enter in this console to stop the server..." << std::endl;

    // Background thread for real-time multiplayer ticking (Snake Duel)
    std::atomic<bool> ticking{true};
    std::thread tickThread([&]() {
        while (ticking) {
            std::this_thread::sleep_for(std::chrono::milliseconds(66)); // 15 Hz
            std::lock_guard<std::mutex> lock(engineMutex);
            for (auto& pair : roomPlayers) {
                if (pair.second.first != 0 && pair.second.second != 0) {
                    gamehub::core::Room* r = roomManager.findRoomByCode(pair.first);
                    if (r && !r->isFinished()) {
                        auto gt = r->getGameType();
                        if (gt == gamehub::core::GameType::SNAKE_DUEL ||
                            gt == gamehub::core::GameType::PONG_DUEL ||
                            gt == gamehub::core::GameType::TRON_DUEL) {
                            r->tick(0.066f);
                            std::string state = r->serializeState();
                            server.sendToClient(pair.second.first, state);
                            server.sendToClient(pair.second.second, state);
                        }
                    }
                }
            }
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
