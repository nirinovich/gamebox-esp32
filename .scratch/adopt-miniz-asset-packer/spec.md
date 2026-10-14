# Spec: Adopt miniz for In-Tree Gzip Asset Compression

Status: resolved

## Problem Statement

The platform currently embeds its single-page client shell into C++ firmware and desktop executables via a custom, hand-rolled RFC 1951 Deflate and Huffman bit-packing engine (`GzipCompressor.hpp`). 

For an undergraduate Computer Science capstone evaluation, hand-writing a 450-line production data compressor from scratch presents three acute problems:
1. **Academic Credibility & AI-Suspicion Risk**: Writing dynamic Huffman tree reconstruction, canonical code generators, and sliding-window LZ77 matchers from scratch is graduate-level coursework. Evaluators immediately suspect unvetted AI generation if a 2nd-year student cannot derive Huffman trees on a whiteboard.
2. **Maintenance & Edge-Case Fragility**: The custom compressor implements only a rudimentary subset of Deflate (greedy matching without full dynamic trees), risking suboptimal compression ratios or corruption on unexpected HTML payload structures.
3. **Repository Noise**: 453 lines of fragile bit-twiddling code reside in the core utility directory alongside domain logic.

## Solution

Replace the hand-rolled compression engine with [**`miniz`**](https://github.com/richgel999/miniz), a battle-tested, single-file public-domain/MIT C library created by Rich Geldreich (used extensively in game engines like Godot and embedded systems).

The packaging utility is refactored to delegate RFC 1951 Deflate compression and CRC-32 checksum calculation to `miniz`, while preserving:
- A **100% Native C++ build toolchain** with zero external package manager or Python dependencies (preserving ADR 0018).
- The exact **zero-copy Flash ROM runtime interface** (`WebAssets::getIndexHtmlData()`, `WebAssets::getIndexHtmlSize()`, and `WebAssets::getGzipPayloadView()`), requiring zero changes to desktop or ESP32 server route handlers.
- Automated round-trip decompression verification both at asset packaging time and in automated native test suites.

## User Stories

1. As a computer science student defending my capstone, I want my build pipeline to use an industry-standard embedded compression library (`miniz`) rather than a hand-rolled Huffman engine, so that my professors evaluate my system architecture without suspecting artificial code bloat.
2. As an embedded systems developer, I want `asset_packer` to compress the web bundle using full dynamic Huffman trees and lazy LZ77 matching, so that flash memory utilization on the ESP32 is reduced by at least 60% compared to raw HTML.
3. As a desktop developer, I want `asset_packer` to compile cleanly under MSVC, GCC, and Clang as a standalone native binary without external package managers (vcpkg/Conan), so that the project builds out of the box with CMake.
4. As a firmware developer, I want the generated web asset header to place the compressed payload in read-only Flash memory (`PROGMEM`), so that serving HTTP requests consumes 0 bytes of dynamic RAM on the ESP32.
5. As a network server developer, I want the runtime `WebAssets` class interface to remain strictly backward-compatible, so that `DesktopServer` and `ESPAsyncWebServer` continue streaming Gzip payloads with zero code modifications.
6. As a build engineer, I want `asset_packer` to perform an immediate round-trip decompression and byte-for-byte comparison against the input file, so that any compression defect fails the build before committing.
7. As an automated test runner, I want `test_tracer_bullet` to decompress the embedded `WebAssets` payload using `miniz` and assert string equality with the original HTML, so that asset integrity regressions are caught by `ctest`.
8. As a project maintainer, I want the hand-rolled `GzipCompressor.hpp` completely excised from the repository, so that unnecessary code complexity is permanently removed.
9. As an architecture reviewer, I want ADR 0022 recorded to supersede ADR 0018, so that the evolution from custom Deflate to `miniz` is transparently documented in the project records.

## Implementation Decisions

1. **Vendor Location**:
   - Place `miniz.h` and `miniz.c` under `third_party/miniz/` as a self-contained, in-tree vendor dependency.
   - Configure CMake so that `third_party/miniz/miniz.c` is compiled into the `asset_packer` build target and the native test targets.

2. **Asset Packer Refactoring**:
   - Refactor the asset packer utility to construct a valid RFC 1952 Gzip container:
     - 10-byte Gzip header (`0x1f, 0x8b`, Deflate method `0x08`, flags `0x00`, mtime `0`, extra flags `0`, OS `0xff`).
     - Raw Deflate compressed payload generated via `tdefl_compress_mem_to_mem` (using maximum compression level).
     - 8-byte Gzip trailer containing the CRC-32 checksum (via `mz_crc32`) and the 32-bit little-endian uncompressed input size.
   - Retain the C++ header generator that formats the Gzip byte array into the target `WebAssets` class.
   - Retain the mandatory round-trip verification using `tinfl_decompress_mem_to_mem` before writing the output header.

3. **Excise Hand-Rolled Compressor**:
   - Delete `include/gamehub/util/GzipCompressor.hpp`.
   - Remove references to `GzipCompressor.hpp` from `CMakeLists.txt` and native test suites.

4. **Preserve Runtime Contract**:
   - The generated header will preserve all static accessor signatures:
     - `getIndexHtmlData()` -> `const uint8_t*`
     - `getIndexHtmlSize()` -> `size_t`
     - `getUncompressedSize()` -> `size_t`
     - `isGzipped()` -> `bool`
     - `getGzipPayloadView()` -> `std::string_view`
   - Neither `src/desktop/DesktopServer.cpp` nor `src/esp32/main.cpp` will be modified.

5. **Architecture Documentation**:
   - Author `docs/adr/0022-replace-hand-rolled-compression-with-miniz.md` superseding ADR 0018.

## Testing Decisions

1. **Seam Selection**:
   - **Seam 1: Build-Time Round-Trip Integrity**: The `asset_packer` binary itself executes decompression immediately after compression, asserting exact byte equality between the decompressed output and the raw input file. Any mismatch returns exit code 1.
   - **Seam 2: Native Regression Suite (`test_tracer_bullet`)**: Unit test linked against `miniz` decompresses `WebAssets::getIndexHtmlData()` and verifies the HTML payload starts with `<!DOCTYPE html>` and matches the uncompressed size.
   - **Seam 3: Desktop Server HTTP Integration (`test_desktop_server`)**: Validates that HTTP GET requests against `DesktopServer` return `200 OK`, `Content-Encoding: gzip`, and a body whose length matches `WebAssets::getIndexHtmlSize()`.

## Out of Scope

- Runtime on-device Gzip compression on the ESP32 (all compression is static and performed offline at build time).
- Migration to LittleFS / SPIFFS filesystem partitions (the single-binary zero-copy PROGMEM model is retained).
- Introduction of external package managers (vcpkg, Conan, Homebrew).

## Further Notes

- `miniz` is dedicated to the public domain (and alternatively licensed under MIT), making it fully permissible for academic and open-source distribution.
