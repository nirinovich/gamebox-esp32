# Spec: Adopt cpp-httplib for Desktop HTTP and WebSocket Server

Status: resolved

## Problem Statement

The desktop runner currently implements a custom HTTP/1.1 and RFC 6455 WebSocket server in `src/desktop/DesktopServer.cpp` using 372 lines of raw Berkeley and Winsock sockets, hand-rolled SHA-1 cryptographic hashing, custom Base64 encoding, manual bitmask unmasking, and unbounded detached OS threads (`std::thread::detach`).

For an undergraduate Computer Science capstone evaluation, hand-rolling cryptographic algorithms (SHA-1) and low-level protocol framing from scratch presents two acute issues:
1. **Academic Credibility & AI-Suspicion Risk**: Implementing SHA-1 transformation rounds and RFC 6455 socket bitmasks from scratch is typical of 4th-year Network Security or Protocols coursework, not an authoritative game engine capstone. Evaluators immediately suspect unvetted AI generation if a sophomore cannot derive SHA-1 schedule constants or RFC 6455 masking rules on a whiteboard.
2. **Maintenance & Fragility**: Unbounded thread spawning on connection accept risks thread exhaustion, and the rudimentary frame parser lacks support for RFC 6455 control frames (Ping/Pong heartbeats, fragmentation, graceful close frames), risking connection drops under browser churn.

## Solution

Replace the custom raw-socket implementation in `DesktopServer.cpp` with [**`cpp-httplib`**](https://github.com/yhirose/cpp-httplib) (`httplib.h`), an industry-standard, single-header C++11 HTTP and WebSocket library created by Yuji Hirose.

Vendoring `httplib.h` into `third_party/httplib/` preserves:
- A **100% Native C++ build toolchain** with zero external package manager (vcpkg/Conan) or Python dependencies.
- The exact public interface of `DesktopServer` (`start()`, `stop()`, `sendToClient()`, `broadcast()`, `setMessageHandler()`, `setConnectHandler()`, `setDisconnectHandler()`), requiring zero changes to `src/desktop/main.cpp` or the `INetworkAdapter` bridge.
- **Zero changes to the ESP32 microcontroller firmware**, which continues to run its specialized `ESPAsyncWebServer` in `src/esp32/main.cpp`.
- Automated regression verification via `test_desktop_server` and `ctest`.

## User Stories

1. As a computer science student defending my capstone, I want the desktop runner to use an industry-standard single-header library (`cpp-httplib`) rather than hand-rolled SHA-1 hashing and socket bitmasking, so that evaluators evaluate my game engine architecture rather than suspecting AI-generated cryptographic bloat.
2. As a desktop developer, I want `DesktopServer` to compile cleanly across Windows (MSVC) and Linux (GCC/Clang) without external package managers (vcpkg, Conan), so that the desktop target builds out of the box with CMake.
3. As a developer testing locally, I want `DesktopServer` to serve the embedded RFC 1952 Gzip single-page ClientShell (`WebAssets`) with proper HTTP headers (`Content-Encoding: gzip`, `Content-Type: text/html`), so that local browsers load the exact same bundle as the ESP32 captive portal.
4. As a developer running multiple local browser tabs, I want `DesktopServer` to manage WebSocket connections (`/ws`) using a managed thread pool, so that local connection churn does not exhaust system OS threads.
5. As a frontend client connected to the desktop runner, I want the WebSocket server to handle standard RFC 6455 control frames (Ping, Pong, Close) seamlessly, so that browser sessions stay alive without unexpected socket resets.
6. As a game engine developer, I want the existing `DesktopServer` interface (`sendToClient`, `broadcast`, `setMessageHandler`, `setConnectHandler`, `setDisconnectHandler`) to remain unchanged, so that `src/desktop/main.cpp` and `HostEngine` require zero modifications.
7. As an automated test runner, I want `test_desktop_server` to verify HTTP `200 OK`, `Content-Encoding: gzip`, payload length equality with `WebAssets`, and clean socket teardown, so that desktop server regressions are caught by `ctest`.
8. As a project maintainer, I want ADR 0023 recorded to document the architectural adoption of `cpp-httplib` and the excision of raw socket and cryptographic boilerplate.

## Implementation Decisions

1. **Vendor Location**:
   - Place `httplib.h` under `third_party/httplib/` as a self-contained, in-tree vendor dependency.
   - Configure CMake to add `third_party/httplib` to target include directories for `gamehub_desktop` and `test_desktop_server`.

2. **Interface Preservation**:
   - The public interface of `DesktopServer` will remain strictly backward-compatible:
     - `start(bool autoFallback)`
     - `stop()`
     - `isRunning()`
     - `getPort()`
     - `setMessageHandler(...)`
     - `setConnectHandler(...)`
     - `setDisconnectHandler(...)`
     - `sendToClient(uint32_t sessionId, const std::string& message)`
     - `broadcast(const std::string& message)`
   - `src/desktop/main.cpp` and `src/core/HostEngine.cpp` will not be modified.

3. **Routing and Handlers**:
   - HTTP route `GET /` serves `gamehub::web::WebAssets::getGzipPayloadView()` with headers `Content-Type: text/html; charset=UTF-8` and `Content-Encoding: gzip`.
   - WebSocket route `/ws` registers incoming sessions, dispatches messages to `m_messageHandler`, invokes connect/disconnect hooks, and stores client references for `sendToClient` and `broadcast`.

4. **Port Auto-Fallback**:
   - Retain sequential port trial (attempting up to 20 consecutive ports starting from the requested base port) if the default port is already bound.

5. **Excision of Cryptographic and Raw Socket Boilerplate**:
   - Delete custom SHA-1 transformation namespace and Base64 encoder from `DesktopServer.cpp`.
   - Eliminate manual byte shifting, masking/unmasking, and raw POSIX/Winsock accept loops.

6. **Microcontroller Firmware Isolation**:
   - No modifications to `src/esp32/main.cpp` or `platformio.ini`.

7. **Architecture Documentation**:
   - Author `docs/adr/0023-adopt-cpp-httplib-for-desktop-server.md`.

## Testing Decisions

1. **Seam 1: Native Desktop Server Integration Test (`test_desktop_server`)**:
   - The primary integration test establishes a real TCP connection to `DesktopServer`, issues `GET / HTTP/1.1`, and validates:
     - HTTP status is `200 OK`.
     - Header contains `Content-Encoding: gzip`.
     - Body length matches `WebAssets::getIndexHtmlSize()`.
     - Server shuts down cleanly without hanging worker threads.
2. **Seam 2: Native CTest Regression Suite**:
   - Run full `ctest -C Debug` asserting 11/11 tests pass.
3. **Seam 3: Interactive End-to-End Verification**:
   - Launch `gamehub_desktop.exe`, verify WebSocket connection and game lobby loading in a local browser session.

## Out of Scope

- Changes to ESP32 firmware networking (`ESPAsyncWebServer` is retained).
- TLS / SSL / HTTPS / WSS configuration (the desktop runner and captive portal operate over plaintext local networks).
- Additional HTTP API endpoints beyond `GET /` and WebSocket `/ws`.

## Further Notes

- `cpp-httplib` is MIT licensed, making it permissible for academic and open-source distribution.
- Vendoring requires only a single header file (`httplib.h`), adding zero build configuration complexity.
