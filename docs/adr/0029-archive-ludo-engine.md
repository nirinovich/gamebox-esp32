# ADR 0029: Archive Ludo Engine and Focus on Traditional Malagasy Games

Status: accepted

## Context & Decision

With the introduction and integration of **Fanorona** (ADR 0028) alongside **Katro** (ADR 0025) and **Malagasy Tsipy Dominoes** (ADR 0012), the project's game catalog now features authentic traditional Malagasy strategic board and parlor games.

To streamline the catalog, reduce binary footprint, and preserve the cultural and pedagogical focus:
1. **Archive `LudoGame`**:
   - `games/ludo/host/LudoGame.cpp` is removed from active `gamehub_core` library in `CMakeLists.txt`.
   - `test_ludo` and `ColocatedLudoTest` are removed from the active test runner.
   - PlatformIO build filter in `platformio.ini` is updated to exclude `games/ludo/**` from hardware flashing (`-<../games/ludo/**>`), matching the Texas Hold'em archival pattern (ADR 0022).
   - In-tree sources under `games/ludo/` are preserved for historical reference and potential standalone reuse.
2. **Catalog & HostEngine Alignment**:
   - `tests/test_host_engine.cpp` transitions test coverage from Ludo to Fanorona (`fanorona_solo` and `fanorona_duel`).
   - The active runtime catalog served by `RoomManager::serializeCatalog()` now consists strictly of:
     - **Fanorona** (`fanorona_solo`, `fanorona_duel`, `fanorona_pvp`)
     - **Katro** (`katro_solo`, `katro_pvp`, `katro_duel`)
     - **Dominoes** (`domino_solo`, `domino_duel`, `domino_pvp`)
   - `CONTEXT.md` and `README.md` are updated to mark `LudoGame` as archived.

## Consequences

- The platform's active board games are now focused on Malagasy traditions (Fanorona, Katro) along with Malagasy Tsipy Dominoes.
- All active C++ unit and integration tests (11/11 CTest suites) and frontend Vitest suites pass cleanly.
- Microcontroller flash usage and compile times on ESP32-S3 and desktop runners are preserved.
