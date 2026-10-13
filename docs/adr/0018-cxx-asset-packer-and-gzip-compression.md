# Native C++ Asset Packer and Gzip Flash Compression

## Context
1. **Auxiliary Python Build Dependency**: Gamebox previously depended on an auxiliary Python script (`scripts/generate_web_assets.py`) to convert `frontend/dist/index.html` into an embedded C++ header (`WebAssets.hpp`). For an academic Computer Science project evaluated as a pure C++ systems capstone, relying on an external Python interpreter weakened the 100% C++ toolchain promise.
2. **Flash Memory Consumption on ESP32-S3**: The uncompressed single-file web client bundle was 105 KB. Embedded directly into microcontroller flash memory (`PROGMEM`) as an uncompressed hex byte array, it consumed substantial flash storage that could otherwise be preserved for additional game modes, bot heuristics, and firmware partitions.
3. **Repository Noise & Git Diff Bloat**: The uncompressed hex array spanned 5,270 lines (660 KB), creating massive git diffs and code review noise on every frontend modification.
4. **Uncompressed HTTP Transport Over Wi-Fi**: Both the native Desktop runner and the ESP32-S3 captive portal server delivered the web client without HTTP compression, ignoring standard RFC 1952 `Content-Encoding: gzip` support built into all modern mobile and desktop Client applications.

## Decision
1. **Implement Standalone C++ Asset Packer (`tools/asset_packer.cpp`)**:
   - Author a 100% native C++17 command-line utility with zero third-party package dependencies.
   - Embed an in-tree Deflate/LZ77 compressor and CRC-32 table generator that formats input files into standard RFC 1952 Gzip containers.
   - Delete the legacy `scripts/generate_web_assets.py` script from the repository.
2. **Incorporate Gzip Compression into Web Assets**:
   - Compress `frontend/dist/index.html` (105 KB) down to ~38 KB (a 64% flash memory footprint reduction).
   - Shrink the generated C++ header from 5,270 lines down to under 2,000 lines.
   - Expose `WebAssets::isGzipped() = true` and `WebAssets::getUncompressedSize()` alongside the existing zero-copy `getIndexHtmlData()` buffer accessors.
3. **Enable `Content-Encoding: gzip` HTTP Streaming Across All Hosts**:
   - Update `DesktopServer.cpp` to emit `Content-Encoding: gzip\r\n` on HTTP GET responses.
   - Update `src/esp32/main.cpp` captive portal routes (`/`, `/generate_204`, `/hotspot-detect.html`, `/ncsi.txt`) to attach `Content-Encoding: gzip` to responses.
4. **Integrate into CMake**:
   - Add `asset_packer` and custom target `generate_assets` in `CMakeLists.txt` for deterministic, single-command asset rebuilding.

## Consequences

### Positive
1. **Zero External Build Dependencies**: The build pipeline is 100% C++17, requiring no Python interpreter or third-party packages to generate embedded web headers.
2. **64% Flash Memory Reduction**: ClientShell size in microcontroller flash drops from 105 KB to 38 KB, conserving flash storage for game logic and firmware partitions.
3. **Repository Cleanliness**: The generated C++ asset header drops from 5,270 lines to under 2,000 lines, dramatically reducing git diff churn.
4. **Faster Captive Portal Pop-ups**: Mobile clients download 64% fewer bytes over 2.4 GHz Wi-Fi, lowering network transmission time and CPU power consumption.
5. **In-Tree Testability**: The compressor and decompressor utility (`GzipCompressor.hpp`) enables automated in-memory round-trip integrity verification.

### Trade-offs & Mitigations
1. **Build-Time CPU Overhead**: Running LZ77 compression during the build adds a few hundred milliseconds, but CMake timestamp tracking ensures it only runs when `frontend/dist/index.html` changes.
2. **In-Tree Code Maintenance**: Maintaining RFC 1951 Deflate and RFC 1952 Gzip compression in-tree requires maintaining ~300 lines of modern C++17 code, mitigated by self-contained unit tests.
