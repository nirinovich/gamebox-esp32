# 02: Gzip HTTP Delivery and Captive Portal Integration

**What to build:** Updates to both the Desktop runner HTTP server (`DesktopServer.cpp`) and the ESP32 captive portal server (`src/esp32/main.cpp`) to emit the `Content-Encoding: gzip` HTTP response header when serving the gzipped single-page application payload, enabling transparent decompression across all desktop and mobile browsers.

**Blocked by:** 01: Standalone C++ Asset Packer with In-Tree Gzip Compression

**Status:** resolved

- [x] `DesktopServer.cpp` serves `GET /` with HTTP status 200, `Content-Encoding: gzip`, and binary gzipped body.
- [x] `src/esp32/main.cpp` serves all captive portal routes (`/`, `/generate_204`, `/hotspot-detect.html`, `/ncsi.txt`, `/connecttest.txt`) with `Content-Encoding: gzip`.
- [x] Connected browsers render the ClientShell without decompression errors.
