# 02: Refactor DesktopServer with cpp-httplib and Port Auto-Fallback

**What to build:** Refactor `src/desktop/DesktopServer.cpp` to use `cpp-httplib`'s `httplib::Server`. Implement `GET /` route serving embedded Gzip `WebAssets`, WebSocket `/ws` route handling bidirectional messaging with active connection tracking, and 20-port sequential auto-fallback on startup. Excise hand-rolled SHA-1, Base64, raw socket bitmasks, and unbounded `std::thread::detach` loops.

**Blocked by:** 01: Vendor cpp-httplib and Wire CMake Build Targets

**Status:** resolved

- [x] `DesktopServer.cpp` refactored to use `httplib::Server` with managed thread pool.
- [x] Hand-rolled SHA-1 hashing, Base64 encoding, and socket bitmasking completely excised.
- [x] `GET /` serves `WebAssets::getGzipPayloadView()` with `Content-Type: text/html` and `Content-Encoding: gzip`.
- [x] WebSocket `/ws` route handles bidirectional messaging, session registration, connect, and disconnect hooks.
- [x] Port auto-fallback gracefully attempts sequential ports (up to 20 ports) if the initial port is busy.
- [x] Public `DesktopServer` class API remains 100% backward-compatible (`src/desktop/main.cpp` requires zero changes).
