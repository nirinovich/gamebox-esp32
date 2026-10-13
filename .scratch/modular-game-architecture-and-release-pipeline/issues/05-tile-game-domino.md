# 05: Complex Tile Game Migration (DominoGame)

**What to build:**
Migrate `DominoGame` into `games/domino/` with Boneyard mechanics, PipSum tie-breaking, player-specific hand snapshots, and Canvas 2D domino tile renderers.

**Blocked by:** 02: Reference Game Migration (Snake & TicTacToe) as Vertical Slices

**Status:** resolved

- [x] Create `games/domino/host/` (`DominoGame.hpp`, `DominoGame.cpp`, `test_domino.cpp`)
- [x] Create `games/domino/client/` (`DominoRenderer.ts`, `index.ts`)
- [x] Verify Domino rules and PipSum calculations pass in co-located unit tests
