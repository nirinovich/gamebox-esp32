# 01: Purge Legacy Tutorial Games & Clean Platform Registry

**What to build:** Completely purge all six clichéd tutorial games (Snake, Pong, Tic-Tac-Toe, Tron, Battleship, and Connect Four) from the Host engine, GameFactory, CMake build targets, unit test suites, ClientShell, and Lobby directory. Establishes a clean platform baseline that builds, runs, and only references the target academic parlor and board games.

**Blocked by:** None (can start immediately)

**Status:** resolved

- [x] Legacy game directories `games/snake/`, `games/pong/`, `games/tictactoe/`, `games/tron/`, `games/battleship/`, and `games/connectfour/` are removed.
- [x] Legacy enum values (`SNAKE_SOLO`, `SNAKE_DUEL`, `TICTACTOE_SOLO`, `TICTACTOE_PVP`, `CONNECT_FOUR_SOLO`, `CONNECT_FOUR_PVP`, `PONG_SOLO`, `PONG_DUEL`, `TRON_SOLO`, `TRON_DUEL`, `BATTLESHIP_SOLO`, `BATTLESHIP_PVP`) are excised from the decoupled GameRegistry architecture.
- [x] `HostEngine.cpp` and `RoomManager.cpp` route dynamically via `GameRegistry` without legacy fallbacks.
- [x] `CMakeLists.txt` is updated to remove compilation of legacy game libraries and obsolete test executables.
- [x] `frontend/src/registry/gameRegistry.ts` and `frontend/src/state/gameStore.ts` only list active non-legacy games.
- [x] Build succeeds green on desktop runner (9/9 CTest passed) and frontend Vitest suite (4/4 test files, 22 tests passed).
