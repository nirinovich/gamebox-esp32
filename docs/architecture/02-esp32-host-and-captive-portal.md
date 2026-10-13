# 02. Host Firmware & Captive Portal Architecture

This document examines how the ESP32-S3 microcontroller operates as a standalone, zero-installation game host. It details the SoftAP Wi-Fi configuration, DNS wildcard interception, operating system captive probe routes, and flash memory asset streaming.

---

## 📡 1. SoftAP (Software Access Point) Configuration

The ESP32-S3 acts as an autonomous Wi-Fi Access Point without requiring an external internet router or local network infrastructure.

### Firmware Initialization
Inside [`src/esp32/main.cpp`](file:///c:/Users/Nikita/Documents/C++/src/esp32/main.cpp):
```cpp
// 1. Wi-Fi SoftAP Configuration
WiFi.mode(WIFI_AP);
WiFi.softAP(AP_SSID); // SSID = "Gamebox", open security (no password)
IPAddress apIP = WiFi.softAPIP();
```

* **SSID**: `Gamebox`
* **Security**: Open (`WIFI_AUTH_OPEN`) to minimize friction for connecting players.
* **Default IP Address**: `192.168.4.1` (subnet `255.255.255.0`).
* **DHCP Server**: Built into the ESP32 Wi-Fi stack; leases IP addresses starting from `192.168.4.2` to connecting clients with minimal lease times to handle player churn.

---

## 🌐 2. DNS Wildcard Redirection (Port 53 Spoofing)

When a mobile device or laptop connects to a Wi-Fi network without internet access, its operating system performs automated DNS lookups for specific verification hostnames.

To intercept these requests and direct the device to the local web server:
1. The firmware launches an instance of `DNSServer` on UDP port 53.
2. The server uses a wildcard (`"*"`) rule to resolve **all** hostnames to the ESP32's local IP address (`192.168.4.1`).

```cpp
static const byte DNS_PORT = 53;
DNSServer dnsServer;

// Start DNS server redirecting all domain requests to apIP
dnsServer.start(DNS_PORT, "*", apIP);
```

During each iteration of `loop()`, incoming UDP packets are serviced:
```cpp
void loop() {
    // ...
    dnsServer.processNextRequest();
    // ...
}
```

Whether a device queries `apple.com`, `connectivitycheck.gstatic.com`, or `example.org`, the response is always `192.168.4.1`.

---

## 🎯 3. Operating System Captive Probe Interception

Every major operating system tests new Wi-Fi connections using a **CaptiveProbe** to detect whether login credentials or terms acceptance are required:

| Operating System | Detection Hostname / Endpoint | Expected Success Status |
| :--- | :--- | :--- |
| **Android** | `connectivitycheck.gstatic.com/generate_204`<br>`/gen_204` | HTTP 204 No Content |
| **Apple iOS / macOS** | `captive.apple.com/hotspot-detect.html`<br>`/canonical.html` | `<HTML><HEAD><TITLE>Success</TITLE></HEAD><BODY>Success</BODY></HTML>` |
| **Windows 10/11** | `www.msftconnecttest.com/connecttest.txt`<br>`/ncsi.txt` | Text: `Microsoft Connect Test` |

### How Gamebox Triggers the Captive Assistant
Instead of returning the expected "Success" or "204" payloads, `ESPAsyncWebServer` binds all known probe endpoints to serve the primary application single-page application (SPA) with `HTTP 200 OK`:

```cpp
auto serveSPA = [](AsyncWebServerRequest* request) {
    AsyncWebServerResponse* response = request->beginResponse(
        200, "text/html",
        gamehub::web::WebAssets::getIndexHtmlData(),
        gamehub::web::WebAssets::getIndexHtmlSize()
    );
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
};

server.on("/", HTTP_GET, serveSPA);
server.on("/generate_204", HTTP_GET, serveSPA);       // Android
server.on("/gen_204", HTTP_GET, serveSPA);            // Android
server.on("/hotspot-detect.html", HTTP_GET, serveSPA); // Apple iOS / macOS
server.on("/canonical.html", HTTP_GET, serveSPA);     // Apple
server.on("/ncsi.txt", HTTP_GET, serveSPA);           // Windows
server.on("/connecttest.txt", HTTP_GET, serveSPA);     // Windows

// Wildcard fallback: redirect any unknown request to the root URL
server.onNotFound([](AsyncWebServerRequest* request) {
    request->redirect("/");
});
```

Because the OS receives an unexpected HTML response rather than its anticipated probe payload, it triggers the system's **Captive Network Assistant** (CNA). The phone or laptop immediately launches an in-app browser presenting the game lobby.

---

## 💾 4. Zero-Copy Flash Memory Asset Streaming

Serving web pages from microcontrollers often suffers from performance bottlenecks or crashes due to RAM limitations:
- **LittleFS / SPIFFS Filesystem Overhead**: Mounting flash partitions incurs flash wear, partition alignment headaches, and filesystem RAM buffers.
- **RAM Heap Allocations**: Reading a 100 KB file into heap memory on an ESP32 risks heap fragmentation and crashes (`Guru Meditation Error: Alloc failed`).

Gamebox circumvents these issues by embedding the gzipped single-file web bundle directly as a **FlashChunk** into the firmware code segment via [`WebAssets.hpp`](file:///c:/Users/Nikita/Documents/C++/include/gamehub/web/WebAssets.hpp).

```mermaid
graph LR
    Flash["ESP32 Flash Memory (PROGMEM)<br/>WebAssets::getIndexHtmlData()<br/>28 KB Gzipped"]
    AsyncServer["ESPAsyncWebServer Request Handler"]
    WiFiChip["ESP32 Wi-Fi Radio"]
    Client["Client Browser"]

    Flash -->|Direct Pointer (Zero Copy)| AsyncServer
    AsyncServer -->|Content-Encoding: gzip| WiFiChip
    WiFiChip -->|Over-the-Air Transmission| Client
```

### Advantages of FlashChunk Streaming:
1. **Zero RAM Heap Consumption**: `request->beginResponse()` receives a direct pointer to flash memory. No intermediate RAM buffer is allocated.
2. **73% Smaller Footprint**: Gzip compression shrinks the payload from 105 KB to 28 KB, saving 75 KB of flash storage for C++ game logic and assets.
3. **Instant Latency**: Transmitting 28 KB over 802.11 Wi-Fi takes under 50ms, ensuring captive portal popups open instantaneously without stalling the microcontroller.
