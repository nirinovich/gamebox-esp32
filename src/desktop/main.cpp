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

    // Disconnect Handler
    server.setDisconnectHandler([&](uint32_t sessionId) {
        std::lock_guard<std::mutex> lock(engineMutex);
        sessions.erase(sessionId);

        // Find room and notify opponent if multiplayer
        gamehub::core::Room* room = roomManager.findRoomByPlayer(sessionId);
        if (room) {
            std::string code = room->getCode();
            auto it = roomPlayers.find(code);
            if (it != roomPlayers.end()) {
                uint32_t other = (it->second.first == sessionId) ? it->second.second : it->second.first;
                if (other != 0) {
                    server.sendToClient(other, "{\"type\":\"opponent_left\"}");
                }
                roomPlayers.erase(it);
            }
            roomManager.handleDisconnect(sessionId);
        }
        sessionPlayerMap.erase(sessionId);
    });

    // Message Handler
    server.setMessageHandler([&](uint32_t sessionId, const std::string& msg) -> std::string {
        std::lock_guard<std::mutex> lock(engineMutex);

        auto it = sessions.find(sessionId);
        if (it == sessions.end()) {
            it = sessions.emplace(sessionId, std::make_unique<gamehub::core::Session>(sessionId)).first;
        }

        // 1. Create Multiplayer Room
        if (msg.find("\"cmd\":\"create\"") != std::string::npos) {
            gamehub::core::GameType gtype = gamehub::core::GameType::TICTACTOE_PVP;
            if (msg.find("\"snake_duel\"") != std::string::npos) {
                gtype = gamehub::core::GameType::SNAKE_DUEL;
            }

            gamehub::core::Room* newRoom = roomManager.createRoom(gtype, sessionId);
            if (!newRoom) {
                return "{\"type\":\"error\",\"msg\":\"Room capacity reached (max 4).\"}";
            }

            sessionPlayerMap[sessionId] = 1;
            roomPlayers[newRoom->getCode()] = {sessionId, 0};

            std::ostringstream oss;
            oss << "{\"type\":\"room_created\",\"code\":\"" << newRoom->getCode() << "\",\"player\":1}";
            return oss.str();
        }

        // 2. Join Multiplayer Room
        if (msg.find("\"cmd\":\"join\"") != std::string::npos) {
            auto codePos = msg.find("\"code\":");
            if (codePos == std::string::npos) return "{\"type\":\"error\",\"msg\":\"Missing code\"}";

            auto start = msg.find("\"", codePos + 7) + 1;
            auto end = msg.find("\"", start);
            std::string code = msg.substr(start, end - start);

            gamehub::core::Room* room = roomManager.joinRoom(code, sessionId);
            if (!room) {
                return "{\"type\":\"error\",\"msg\":\"Room not found or already full.\"}";
            }

            sessionPlayerMap[sessionId] = 2;
            roomPlayers[code].second = sessionId;

            // Notify joiner
            std::ostringstream oss;
            oss << "{\"type\":\"room_joined\",\"code\":\"" << code << "\",\"player\":2}";
            server.sendToClient(sessionId, oss.str());

            // Notify host and start game
            std::string state = room->serializeState();
            uint32_t hostSession = roomPlayers[code].first;
            server.sendToClient(hostSession, "{\"type\":\"room_ready\"}");
            server.sendToClient(hostSession, state);
            server.sendToClient(sessionId, state);

            return "";
        }

        // 3. Multiplayer Moves & Inputs
        gamehub::core::Room* activeRoom = roomManager.findRoomByPlayer(sessionId);
        if (activeRoom && activeRoom->getPlayerCount() >= 2) {
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

        // 4. Fallback to session handling (Solo games, ping, etc.)
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
                    if (r && r->getGameType() == gamehub::core::GameType::SNAKE_DUEL && !r->isFinished()) {
                        r->tick(0.066f);
                        std::string state = r->serializeState();
                        server.sendToClient(pair.second.first, state);
                        server.sendToClient(pair.second.second, state);
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
