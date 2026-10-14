# 06: Intégration de bout en bout et vérification de non-régression

**What to build:**
Verify the complete integrated system with the three refactored games:
- Ensure all games register properly with `GameRegistry` and broadcast accurate catalogues to clients.
- Verify that state snapshots (`serializeStateForPlayer`) remain 100% compatible with client Canvas 2D renderers without frontend changes.
- Execute full desktop server tests, host engine integration tests, and platform builds.

**Blocked by:** 02 (Archivage du moteur de Poker), 03 (Refonte POO de Dominoes), 04 (Refonte POO de Ludo), 05 (Refonte POO de Katro)

**Status:** resolved

- [x] All 3 refactored games compile and integrate cleanly with `HostEngine` and `RoomManager`
- [x] Full CTest test suite passes (`ctest --output-on-failure`)
- [x] Desktop server runs and serves valid snapshots over WebSockets
