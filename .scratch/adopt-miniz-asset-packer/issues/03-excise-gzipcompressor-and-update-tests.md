# 03: Excise Hand-Rolled GzipCompressor and Update Native Tests

**What to build:** Permanently delete `include/gamehub/util/GzipCompressor.hpp`. Update `tests/test_tracer_bullet.cpp` to verify decompression using `miniz`. Run the full `ctest` and `npm test` suites to verify zero regressions.

**Blocked by:** 02: Refactor asset_packer CLI with miniz and Round-Trip Verification

**Status:** resolved

- [x] `include/gamehub/util/GzipCompressor.hpp` is permanently deleted from the repository.
- [x] `tests/test_tracer_bullet.cpp` verifies `WebAssets` contains valid RFC 1952 Gzip data decompressed via `miniz`.
- [x] All 11 CTest suites pass (100%).
- [x] All 37 Vitest frontend tests pass (100%).
