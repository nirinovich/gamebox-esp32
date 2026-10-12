# 06: Parity Test Suite, Captive Portal Verification, and Final Cutover

**What to build:** An automated Vitest test suite testing WebSocket state transitions and protocol schemas, verification of bundle size budgets (<45 KB minified) and chunk boundaries (<8 KB), verification of captive portal endpoints, and final cutover of the generated `WebAssets.hpp`.

**Blocked by:** 05: Real-Time 60 FPS Games Migration and Touch Ergonomics

**Status:** resolved

- [x] Vitest test suite passes all protocol ingestion and state transition tests headlessly
- [x] Inlined `dist/index.html` bundle size is verified to be under 45 KB minified (gzip: ~24.7 KB)
- [x] Generated `WebAssets.hpp` chunks are verified to be under 8 KB each
- [x] CMake desktop test suite (`ctest --test-dir build`) passes 100%
- [x] ESP32 captive portal endpoints (`/`, `/generate_204`, `/hotspot-detect.html`, `/ncsi.txt`) serve the new Preact application
- [x] Final cutover removes legacy raw HTML string chunks, keeping clean generated header
