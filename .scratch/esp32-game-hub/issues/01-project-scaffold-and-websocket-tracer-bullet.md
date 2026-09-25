# 01: Project Scaffold & End-to-End WebSocket Tracer Bullet

**What to build:** The vertical end-to-end foundation connecting browser Client to C++ Host. Provides the dual-target project structure (native desktop runner and PlatformIO ESP32 configuration) with the base HTML5 canvas UI embedded as a flash string. Running the desktop executable and opening `http://localhost:8080` (or connecting to the ESP32) displays a connected status badge, sends ping packets over WebSockets, and receives pongs back with live latency metrics.

**Blocked by:** None (can start immediately).

**Status:** resolved

- [x] Desktop runner builds cleanly with CMake/C++17 and runs a local HTTP/WebSocket server on port 8080.
- [x] PlatformIO environment is configured for ESP32-S3 with `ESPAsyncWebServer`, `AsyncTCP`, and `ArduinoJson`.
- [x] Web assets are embedded in C++ flash memory as a raw string literal and served at `/`.
- [x] Client browser connects via WebSocket, exchanges ping/pong messages, and updates connection status.
- [x] End-to-end test passes verifying that the server responds to client websocket connections.
