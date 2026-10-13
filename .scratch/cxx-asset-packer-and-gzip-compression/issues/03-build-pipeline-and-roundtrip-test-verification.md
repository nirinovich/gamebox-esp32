# 03: Build Pipeline Integration and Round-Trip Test Verification

**What to build:** Integration of `asset_packer` into `CMakeLists.txt` via an executable target and a custom target `generate_assets`, safe deletion of the legacy `scripts/generate_web_assets.py` script, and an automated round-trip decompression integrity test in `tests/test_tracer_bullet.cpp` verifying byte-for-byte fidelity with the input HTML.

**Blocked by:** 01: Standalone C++ Asset Packer with In-Tree Gzip Compression, 02: Gzip HTTP Delivery and Captive Portal Integration

**Status:** resolved

- [x] `CMakeLists.txt` builds `asset_packer` and provides a custom target `generate_assets`.
- [x] `scripts/generate_web_assets.py` is safely deleted from the repository.
- [x] Automated tests in `test_tracer_bullet.cpp` assert valid Gzip header magic bytes and successful decompression matching the source file.
- [x] All `ctest` test suites pass cleanly.
