#if defined(ESP_PLATFORM) || defined(ARDUINO)
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <gamehub/web/WebAssets.hpp>
#include <gamehub/core/Session.hpp>
#include <gamehub/core/RoomManager.hpp>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <sstream>

static const char* AP_SSID = "Gamebox";
static const byte DNS_PORT = 53;

DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

gamehub::core::RoomManager roomManager(4);
std::unordered_map<uint32_t, std::unique_ptr<gamehub::core::Session>> sessions;
std::unordered_map<uint32_t, uint32_t> sessionPlayerMap;
std::unordered_map<std::string, std::pair<uint32_t, uint32_t>> roomPlayers;
std::mutex engineMutex;

TaskHandle_t gameTaskHandle = NULL;

static std::string extractJsonString(const std::string& json, const std::string& key) {
    std::string needle = "\"" + key + "\":";
    auto pos = json.find(needle);
    if (pos == std::string::npos) return "";
    auto start = json.find("\"", pos + needle.size());
    if (start == std::string::npos) return "";
    auto end = json.find("\"", start + 1);
    if (end == std::string::npos) return "";
    return json.substr(start + 1, end - start - 1);
}

void broadcastToRoom(const std::string& code, const std::string& message) {
    auto it = roomPlayers.find(code);
    if (it != roomPlayers.end()) {
        if (it->second.first != 0) ws.text(it->second.first, message.c_str());
        if (it->second.second != 0) ws.text(it->second.second, message.c_str());
    }
}

void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        std::lock_guard<std::mutex> lock(engineMutex);
        sessions[client->id()] = std::make_unique<gamehub::core::Session>(client->id());
        client->text(roomManager.serializeDirectory().c_str());
    } else if (type == WS_EVT_DISCONNECT) {
        std::lock_guard<std::mutex> lock(engineMutex);
        uint32_t sid = client->id();
        sessions.erase(sid);

        gamehub::core::Room* room = roomManager.findRoomByPlayer(sid);
        if (room) {
            std::string code = room->getCode();
            auto it = roomPlayers.find(code);
            if (it != roomPlayers.end()) {
                uint32_t other = (it->second.first == sid) ? it->second.second : it->second.first;
                if (other != 0) {
                    ws.text(other, "{\"type\":\"opponent_left\"}");
                }
                roomPlayers.erase(it);
            }
            roomManager.handleDisconnect(sid);
            ws.textAll(roomManager.serializeDirectory().c_str());
        }
        sessionPlayerMap.erase(sid);
    } else if (type == WS_EVT_DATA) {
        AwsFrameInfo* info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            std::string msg((char*)data, len);
            std::lock_guard<std::mutex> lock(engineMutex);
            uint32_t sid = client->id();

            // 0. Query Live Room Directory
            if (msg.find("\"cmd\":\"get_rooms\"") != std::string::npos) {
                client->text(roomManager.serializeDirectory().c_str());
                return;
            }

            // 1. Create Multiplayer Room
            if (msg.find("\"cmd\":\"create\"") != std::string::npos) {
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

                gamehub::core::Room* newRoom = roomManager.createRoom(gtype, sid, hostName);
                if (!newRoom) {
                    client->text("{\"type\":\"error\",\"msg\":\"Room capacity reached (max 4).\"}");
                    return;
                }

                sessionPlayerMap[sid] = 1;
                roomPlayers[newRoom->getCode()] = {sid, 0};

                // Broadcast directory to all clients
                ws.textAll(roomManager.serializeDirectory().c_str());

                std::ostringstream oss;
                oss << "{\"type\":\"room_created\",\"code\":\"" << newRoom->getCode() << "\",\"player\":1,\"host\":\"" << hostName << "\"}";
                client->text(oss.str().c_str());
                return;
            }

            // 2. Join Multiplayer Room
            if (msg.find("\"cmd\":\"join\"") != std::string::npos) {
                std::string code = extractJsonString(msg, "code");
                if (code.empty()) return;

                gamehub::core::Room* room = roomManager.joinRoom(code, sid);
                if (!room) {
                    client->text("{\"type\":\"error\",\"msg\":\"Room not found or already full.\"}");
                    return;
                }

                sessionPlayerMap[sid] = 2;
                roomPlayers[code].second = sid;

                // Broadcast directory update
                ws.textAll(roomManager.serializeDirectory().c_str());

                std::ostringstream oss;
                oss << "{\"type\":\"room_joined\",\"code\":\"" << code << "\",\"player\":2,\"host\":\"" << room->getHostName() << "\"}";
                client->text(oss.str().c_str());

                std::string state = room->serializeState();
                uint32_t hostSession = roomPlayers[code].first;
                ws.text(hostSession, "{\"type\":\"room_ready\"}");
                broadcastToRoom(code, state);
                return;
            }

            // 3. Leave / Cancel Room
            if (msg.find("\"cmd\":\"leave\"") != std::string::npos) {
                gamehub::core::Room* room = roomManager.findRoomByPlayer(sid);
                if (room) {
                    std::string code = room->getCode();
                    auto it = roomPlayers.find(code);
                    if (it != roomPlayers.end()) {
                        uint32_t other = (it->second.first == sid) ? it->second.second : it->second.first;
                        if (other != 0) {
                            ws.text(other, "{\"type\":\"opponent_left\"}");
                        }
                        roomPlayers.erase(it);
                    }
                    roomManager.handleDisconnect(sid);
                    ws.textAll(roomManager.serializeDirectory().c_str());
                }
                sessionPlayerMap.erase(sid);
                client->text("{\"type\":\"left\"}");
                return;
            }

            // 4. Multiplayer Move & Input
            gamehub::core::Room* activeRoom = roomManager.findRoomByPlayer(sid);
            if (activeRoom && activeRoom->getPlayerCount() >= 2) {
                uint32_t playerNum = sessionPlayerMap[sid];
                activeRoom->handleInput(playerNum, msg);
                std::string state = activeRoom->serializeState();
                broadcastToRoom(activeRoom->getCode(), state);
                return;
            }

            // 5. Solo fallback
            auto it = sessions.find(sid);
            if (it != sessions.end()) {
                std::string reply = it->second->handleMessage(msg);
                if (!reply.empty()) {
                    client->text(reply.c_str());
                }
            }
        }
    }
}

// Dedicated FreeRTOS Task pinned to Core 1 for real-time game simulation
void GameEngineTask(void* parameter) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(66); // 15 Hz tick loop

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

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
                        broadcastToRoom(pair.first, state);
                    }
                }
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);

    // 1. Wi-Fi SoftAP Configuration
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID);
    IPAddress apIP = WiFi.softAPIP();
    Serial.printf("[WiFi] SoftAP started: %s (IP: %s)\n", AP_SSID, apIP.toString().c_str());

    // 2. Captive Portal DNS Server (Port 53 wildcard redirection)
    dnsServer.start(DNS_PORT, "*", apIP);

    // 3. WebSocket Configuration
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    // 4. Captive Portal Auto-Popup Endpoints
    auto serveSPA = [](AsyncWebServerRequest* request) {
        request->send(200, "text/html", gamehub::web::WebAssets::getIndexHtml().c_str());
    };

    server.on("/", HTTP_GET, serveSPA);
    server.on("/generate_204", HTTP_GET, serveSPA);       // Android
    server.on("/gen_204", HTTP_GET, serveSPA);            // Android
    server.on("/hotspot-detect.html", HTTP_GET, serveSPA); // Apple iOS / macOS
    server.on("/canonical.html", HTTP_GET, serveSPA);     // Apple
    server.on("/ncsi.txt", HTTP_GET, serveSPA);           // Windows
    server.on("/connecttest.txt", HTTP_GET, serveSPA);     // Windows

    server.onNotFound([](AsyncWebServerRequest* request) {
        request->redirect("/");
    });

    server.begin();
    Serial.println("[HTTP] Server ready on Core 0.");

    // 5. Pin Game Engine Task to Core 1
    xTaskCreatePinnedToCore(
        GameEngineTask,
        "GameEngineTask",
        8192,
        NULL,
        1,
        &gameTaskHandle,
        1 // Core 1
    );
    Serial.println("[RTOS] GameEngineTask running on Core 1.");
}

void loop() {
    dnsServer.processNextRequest();
    ws.cleanupClients();
    delay(10);
}
#endif
