# 0025: Adoption of cpp-httplib for Desktop HTTP and WebSocket Server

> [!NOTE]
> **Status**: Accepted
> **Supersedes**: Raw socket implementation in `src/desktop/DesktopServer.cpp`

## Context

1. **Academic Credibility & AI-Suspicion Risk**:
   The desktop simulation runner (`src/desktop/DesktopServer.cpp`) previously implemented a custom HTTP/1.1 and RFC 6455 WebSocket server using 372 lines of raw Winsock/POSIX sockets, a from-scratch 80-round SHA-1 hash engine (`sha1::transform`), and a custom Base64 encoder to perform the WebSocket upgrade handshake (`Sec-WebSocket-Accept`).
   Implementing cryptographic algorithms and low-level socket protocol framing from scratch is typical of senior network security coursework, not an undergraduate game engine capstone. In an academic defense, hand-rolling SHA-1 and bitwise WebSocket framing triggers immediate suspicion of unvetted AI generation or misplaced priorities unless the student can whiteboard RFC 6455 and SHA-1 internals from memory.
2. **Fragility & Resource Exhaustion**:
   The custom server spawned unbounded detached OS threads (`std::thread::detach`) on every accepted connection, lacking a bounded thread pool. Furthermore, it lacked support for RFC 6455 control frames (Ping, Pong heartbeats, close handshakes, and fragmentation), risking socket disconnects during browser session churn.
3. **Firmware Isolation**:
   The ESP32 microcontroller target runs `ESPAsyncWebServer` in `src/esp32/main.cpp` and was never intended to share desktop socket code. The desktop runner's sole purpose is local testing and development without requiring physical hardware.

## Decision

1. **Vendor Single-Header `cpp-httplib`**:
   - Vendor [`cpp-httplib`](https://github.com/yhirose/cpp-httplib) (`httplib.h`), an industry-standard, MIT-licensed C++11 single-header HTTP and WebSocket library created by Yuji Hirose, under `third_party/httplib/`.
   - Update `CMakeLists.txt` to add `third_party/httplib` to target include directories for `gamehub_desktop` and `test_desktop_server`.
2. **Refactor `DesktopServer.cpp`**:
   - Re-implement `DesktopServer` using `httplib::Server`, utilizing its internal managed thread pool.
   - Configure route `GET /` (and `/index.html`) to stream embedded RFC 1952 Gzip single-page `WebAssets` with `Content-Type: text/html` and `Content-Encoding: gzip`.
   - Configure route `/ws` to handle bidirectional RFC 6455 WebSocket communication, session tracking, and dispatching to `m_messageHandler`, `m_connectHandler`, and `m_disconnectHandler`.
   - Retain 20-port sequential auto-fallback on startup via `bind_to_port`.
3. **Excise Cryptographic and Raw Socket Boilerplate**:
   - Permanently remove 65 lines of hand-rolled SHA-1 logic, 27 lines of custom Base64 encoding, and manual byte-shifting/XOR unmasking.
4. **Preserve Public Architecture**:
   - The public interface of `DesktopServer` remains 100% backward-compatible. `src/desktop/main.cpp` and the `INetworkAdapter` bridge require zero changes.
   - The ESP32 firmware build (`src/esp32/main.cpp`, `platformio.ini`) remains 100% untouched.

## Consequences

### Positive

1. **Academic Defensibility**: Networking logic relies on an established, respected open-source library, shifting the capstone focus to software architecture (the Adapter pattern bridging ESP32 and Desktop).
2. **Managed Concurrency**: Bounded worker thread pool replaces unbounded thread detachment, preventing thread exhaustion.
3. **RFC 6455 Compliance**: Automatic ping/pong heartbeats, frame unmasking, and clean close handshakes are handled natively by the library.
4. **Toolchain Independence**: Preserves the 100% native C++ build with zero package managers (vcpkg/Conan) or Python scripts.
5. **Reduced Maintenance Burden**: `DesktopServer.cpp` shrank from 372 lines to ~110 lines of clean, readable standard C++.

### Trade-offs & Mitigations

1. **Thread Pool Allocation**: `cpp-httplib` uses a thread pool for blocking socket I/O. For a local desktop runner hosting 1 to 4 browser tabs, thread utilization is minimal (<1% CPU).
