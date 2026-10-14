# 02: Refactor asset_packer CLI with miniz and Round-Trip Verification

**What to build:** Update `tools/asset_packer.cpp` to use `miniz` for RFC 1951 Deflate compression and CRC-32 checksum calculation. Format compressed data into a valid RFC 1952 Gzip stream. Execute in-memory decompression verification before generating the `WebAssets.hpp` header.

**Blocked by:** 01: Vendor miniz and Wire CMake Build Target

**Status:** resolved

- [x] `asset_packer.cpp` includes `miniz.h` and removes dependency on `GzipCompressor.hpp`.
- [x] Gzip payload matches RFC 1952 specification (10-byte header, raw Deflate stream via `miniz`, 8-byte trailer with `mz_crc32` and uncompressed length).
- [x] Round-trip verification passes: decompressed payload matches original `index.html` byte-for-byte.
- [x] Generated `WebAssets.hpp` maintains the zero-copy flash runtime API (`getIndexHtmlData()`, `getIndexHtmlSize()`, `getGzipPayloadView()`).
- [x] Running `asset_packer` produces a valid `WebAssets.hpp` with reduced flash footprint.
