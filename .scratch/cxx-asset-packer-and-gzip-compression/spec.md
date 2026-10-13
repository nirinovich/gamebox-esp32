# Spec: C++ Native Asset Packer and Gzip Flash Compression

Status: ready-for-agent

## Problem Statement

Gamebox currently relies on an external Python build script (`scripts/generate_web_assets.py`) to convert the single-file web client bundle (`index.html`) into an embedded C++ header (`WebAssets.hpp`). This design introduces major operational, architectural, and educational drawbacks:

1. **Broken 100% C++ Academic Rigor**: For an academic Computer Science capstone being evaluated by an instructor, having an auxiliary Python dependency undermines the "pure modern C++17 system" project goal. The toolchain should demonstrate mastery of native C++ I/O streams, binary encoding, and build tool orchestration without relying on interpreted scripting languages.
2. **Flash Memory Inefficiency on the Host Microcontroller**: The uncompressed client HTML bundle is currently 105 KB. When embedded directly into ESP32-S3 flash memory as an uncompressed binary array, it consumes significant flash storage, limiting available flash space for additional game models, assets, and firmware partitions.
3. **Network Latency & Wi-Fi Captive Portal Stalls**: Delivering an uncompressed 105 KB payload over the ESP32 SoftAP Wi-Fi network takes noticeably longer, especially on mobile devices with high radio latency or when multiple clients connect to the captive portal concurrently.
4. **Massive Git Repository Noise**: The generated header contains over 5,200 lines of raw hexadecimal byte literals (`0x3c, 0x21, ...`), creating 660 KB of commit noise, bloating git history, and impairing code reviews whenever frontend UI components are updated.
5. **Absence of Standard HTTP Compression**: Modern HTTP clients and mobile web browsers natively support gzip compression via `Content-Encoding: gzip`, but the existing C++ web servers (both Desktop and ESP32) only serve uncompressed raw bytes.

## Solution

A native, zero-external-dependency C++ command-line asset packaging utility integrated into the build pipeline that compresses the single-file web bundle using standard RFC 1952 Gzip compression and generates the embedded C++ asset header:

1. **100% C++ Build Toolchain**: A standalone native C++ utility (`asset_packer`) built on the host machine using standard C++17 streams and filesystem APIs, replacing the legacy Python script entirely.
2. **In-Tree Zero-Dependency Gzip Compressor**: An embedded, single-header deflate/gzip compression implementation included directly within the repository source tree, eliminating any need for package managers (`vcpkg`, `conan`), external DLLs, or system packages.
3. **Dramatic Flash & Line-Count Footprint Reduction**: Gzip compression reduces the 105 KB client bundle down to approximately 28 KB (a ~73% size reduction), shrinking the generated C++ hexadecimal array from over 5,200 lines down to roughly 1,400 lines and saving 75 KB of flash memory on the ESP32.
4. **Gzip HTTP Delivery Across Both Host Platforms**: Both the Desktop HTTP runner and the ESP32 captive portal server are updated to emit the `Content-Encoding: gzip` HTTP response header, allowing browsers to automatically and transparently decompress the web client.
5. **Automated CMake Build Target**: Integration into CMake via a dedicated custom command and target, ensuring the C++ asset packer compiles and runs deterministically during build time without manual developer intervention.

## User Stories

1. As an instructor evaluating the codebase, I want all build utilities, scripts, and code generation tools to be written in standard C++, so that the repository represents a pure 100% C++ software engineering project.
2. As a software engineering student presenting to an instructor, I want the project to build cleanly without requiring Python to be installed or invoked in the build pipeline, so that the grading environment has zero external runtime friction.
3. As a developer modifying frontend UI components, I want an automated CMake target to regenerate the C++ asset header, so that I can update web assets with a single build command.
4. As an embedded firmware developer flashing an ESP32-S3 board, I want the embedded web payload to occupy minimum flash memory, so that memory constraints and flash partition margins are respected.
5. As a mobile player connecting to the Gamebox Wi-Fi captive portal, I want the client web application to download in under 50ms, so that the captive network login prompt pops up instantly.
6. As a desktop player connecting to the local runner at localhost:8080, I want the embedded HTTP server to return standard gzipped responses, so that modern browser compression standards are honored.
7. As a mobile player on a congested Wi-Fi Access Point, I want the compressed payload to minimize packet transmission, so that initial page loads do not stall the real-time 15 Hz game engine tick loop.
8. As a code reviewer inspecting git diffs, I want the generated C++ asset header to be significantly smaller in line count, so that repository commits are cleaner and easier to inspect.
9. As a developer cloning the project on Windows using MSVC, I want the C++ asset packing tool to compile with zero third-party package dependencies, so that I do not need to install `vcpkg` or download external `zlib.dll` libraries.
10. As a developer building on Linux or macOS, I want the C++ asset packing tool to compile seamlessly using standard GCC or Clang, so that cross-platform parity is strictly preserved.
11. As a developer running unit tests, I want the asset packer to fail with a clear non-zero exit code and diagnostic error message if the input HTML file does not exist, so that build failures are immediately diagnosable.
12. As a host server streaming assets from flash memory, I want zero-copy byte array pointers to remain valid and contiguous, so that `ESPAsyncWebServer` can stream directly from PROGMEM without heap buffer allocation.
13. As a developer running test harnesses, I want automated tests to verify that the gzipped payload decompresses to the exact byte-for-byte content of the input HTML file, so that asset integrity regressions are caught automatically.
14. As an architecture reviewer, I want an Architectural Decision Record (ADR) documenting the transition to C++ native asset packing and Gzip flash compression, so that future maintainers understand the rationale and trade-offs.

## Implementation Decisions

1. **Host-Targeted Utility Module**: A dedicated C++ utility will be created under the project's tool directory. It will compile using the host toolchain (native MSVC on Windows, GCC/Clang on POSIX) and execute during build time.
2. **In-Tree Compression Header**: A self-contained, header-only deflate/gzip compressor will be maintained in the source tree to ensure zero external dependency management. It will implement standard RFC 1952 gzip container formatting with appropriate header flags and CRC-32 checksums.
3. **Replacement of Legacy Generation Tooling**: The legacy Python asset generator will be completely removed from the repository. All references in documentation and build systems will point to the native C++ generator.
4. **Web Asset Header Interface Stability**: The generated C++ asset class will retain its existing public interface (`getIndexHtmlData()`, `getIndexHtmlSize()`, `getIndexHtml()`, `getIndexHtmlView()`) while adding a boolean indicator (`isGzipped() = true`) to enable downstream callers to query compression state safely.
5. **HTTP Server Compression Headers**: Both the Desktop HTTP server implementation and the ESP32 captive portal request handlers will configure `Content-Encoding: gzip` on all responses serving the primary HTML single-page application.
6. **Build System Orchestration**: The root build configuration will define the executable target for the asset packaging utility and establish custom commands to execute the binary when the input HTML file is newer than the output C++ header.

## Testing Decisions

1. **High-Seam Behavioral Testing**: Tests should verify the external contract of the tool and transport, not private compression internals.
2. **Round-Trip Integrity Verification**: An automated test executable will invoke the compression pipeline on sample content and verify that decompressing the output reproduces the exact original input byte stream without corruption.
3. **HTTP Server Gzip Response Verification**: Integration test suites will simulate an HTTP GET request to the desktop runner and verify that:
   - The HTTP status code is 200 OK.
   - The `Content-Encoding: gzip` header is present.
   - The response payload size matches `WebAssets::getIndexHtmlSize()`.
4. **Prior Art in Codebase**: Follows the existing headless testing pattern established in `tests/test_tracer_bullet.cpp` and `tests/test_host_engine.cpp`.

## Out of Scope

1. **Dynamic Runtime Compression**: Compressing responses on the fly during request handling is out of scope. All compression is performed offline at build time to preserve ESP32 CPU cycles for the 15 Hz authoritative game simulation.
2. **WebSocket Message Compression (permessage-deflate)**: WebSocket frames for game state updates remain uncompressed JSON snapshots because individual game packets are small (<500 bytes) and compression would introduce CPU latency on the microcontroller.
3. **Multi-File Asset Bundling**: Combining multiple individual CSS/JS files inside the C++ packer is out of scope. Vite's single-file bundling plugin handles frontend inlining into a single HTML file before the C++ asset packer runs.

## Further Notes

- The resulting compressed asset size (~28 KB) is well below the 65,535-byte threshold for MSVC string limits, although retaining the contiguous byte array format remains optimal for ESP32 PROGMEM zero-copy streaming.
- An Architectural Decision Record (`docs/adr/0018-cxx-asset-packer-and-gzip-compression.md`) will record this decision as an immutable project record.
