#if defined(ESP_PLATFORM) || defined(ARDUINO)
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <gamehub/web/WebAssets.hpp>
#include <gamehub/core/HostEngine.hpp>
#include <gamehub/core/INetworkAdapter.hpp>
#include <memory>

static const char* AP_SSID = "Gamebox";
static const byte DNS_PORT = 53;

DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

class ESP32NetworkAdapter : public gamehub::core::INetworkAdapter {
public:
    explicit ESP32NetworkAdapter(AsyncWebSocket& websocket) : m_ws(websocket) {}
    void sendToClient(uint32_t sessionId, const std::string& message) override {
        AsyncWebSocketClient* client = m_ws.client(sessionId);
        if (client && client->status() == WS_CONNECTED) {
            // Avoid queue backlog and heap exhaustion by dropping stale tick state frames in real-time games
            bool isRealtimeState = message.find("\"type\":\"state\"") != std::string::npos &&
                                  (message.find("\"game\":\"snake\"") != std::string::npos ||
                                   message.find("\"game\":\"pong\"") != std::string::npos ||
                                   message.find("\"game\":\"tron\"") != std::string::npos);
            if (isRealtimeState && client->queueLen() >= 2) {
                return;
            }
            client->text(message.c_str());
        }
    }
    void broadcast(const std::string& message) override {
        for (auto& client : m_ws.getClients()) {
            if (client.status() == WS_CONNECTED) {
                bool isRealtimeState = message.find("\"type\":\"state\"") != std::string::npos &&
                                      (message.find("\"game\":\"snake\"") != std::string::npos ||
                                       message.find("\"game\":\"pong\"") != std::string::npos ||
                                       message.find("\"game\":\"tron\"") != std::string::npos);
                if (isRealtimeState && client.queueLen() >= 2) {
                    continue;
                }
                client.text(message.c_str());
            }
        }
    }
private:
    AsyncWebSocket& m_ws;
};

ESP32NetworkAdapter netAdapter(ws);
gamehub::core::HostEngine engine(&netAdapter, 4);

void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        client->setCloseClientOnQueueFull(false);
        engine.onClientConnect(client->id());
    } else if (type == WS_EVT_DISCONNECT) {
        engine.onClientDisconnect(client->id());
    } else if (type == WS_EVT_DATA) {
        AwsFrameInfo* info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            std::string msg((char*)data, len);
            engine.onClientMessage(client->id(), msg);
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

    // 4. Captive Portal Auto-Popup Endpoints (Zero-copy stream directly from flash memory)
    auto serveSPA = [](AsyncWebServerRequest* request) {
        request->send(200, "text/html", gamehub::web::WebAssets::getIndexHtmlData(), gamehub::web::WebAssets::getIndexHtmlSize());
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

    Serial.println("[GameHub] Core 0 event loop active (15 Hz game engine + captive portal).");
}

void loop() {
    static uint32_t lastTick = 0;
    uint32_t now = millis();
    if (now - lastTick >= 66) {
        lastTick = now;
        engine.tick(0.066f);
    }
    dnsServer.processNextRequest();
    ws.cleanupClients();
    delay(5);
}
#endif
