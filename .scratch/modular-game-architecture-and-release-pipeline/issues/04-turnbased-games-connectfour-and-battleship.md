# 04: Turn-Based & Grid Games Migration (ConnectFour & Battleship)

**What to build:**
Migrate `ConnectFour` and `Battleship` into `games/connectfour/` and `games/battleship/`, including AI strategy injection, perspective-filtered state snapshots, and co-located tests.

**Blocked by:** 02: Reference Game Migration (Snake & TicTacToe) as Vertical Slices

**Status:** resolved

- [x] Create `games/connectfour/host/` (`ConnectFourGame.hpp`, `ConnectFourGame.cpp`, `test_connectfour.cpp`) and `games/connectfour/client/` (`ConnectFourRenderer.ts`, `index.ts`)
- [x] Create `games/battleship/host/` (`BattleshipGame.hpp`, `BattleshipGame.cpp`, `test_battleship.cpp`) and `games/battleship/client/` (`BattleshipRenderer.ts`, `index.ts`)
- [x] Verify perspective filtering and AI strategy execution with tests
