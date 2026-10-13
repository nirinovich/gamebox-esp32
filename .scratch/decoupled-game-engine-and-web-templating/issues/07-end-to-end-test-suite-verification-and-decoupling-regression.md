# 07: End-to-End Test Suite Verification and Decoupling Regression

**What to build:**
Validate that the entire project passes both C++ and TypeScript test suites (`ctest` and `npm test`). Implement a decoupled regression test demonstrating that a new game added solely inside `games/` is exposed to the lobby and can run without touching any engine core file. Rebuild the embedded asset header (`WebAssets.hpp`) and verify desktop runner compilation.

**Blocked by:** 06: Full Game Migration and Deprecation of Central Enums

**Status:** resolved

- [x] Run all C++ unit and integration tests via `ctest`.
- [x] Run all Vitest client tests via `npm test`.
- [x] Build desktop runner binary (`gamehub_desktop`).
- [x] Rebuild embedded assets via `asset_packer` and verify binary size within Flash limits.
- [x] Verify zero git modifications to engine core when testing a decoupled mock game module.
