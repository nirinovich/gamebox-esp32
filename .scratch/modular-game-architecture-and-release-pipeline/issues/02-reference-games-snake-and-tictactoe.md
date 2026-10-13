# 02: Reference Game Migration (Snake & TicTacToe) as Vertical Slices

**What to build:**
Establish the self-contained `GameModule` standard by migrating `Snake` and `TicTacToe` into `games/snake/` and `games/tictactoe/`. Co-locate C++ rules, state models, unit tests, and Canvas 2D renderers. Register both in `GameRegistry.hpp` and `gameRegistry.ts`.

**Blocked by:** 01: Build System Multi-Root Support & Single-Source Versioning

**Status:** resolved

- [x] Create `games/snake/host/` with `SnakeGame.hpp`, `SnakeGame.cpp`, and `test_snake.cpp`
- [x] Create `games/snake/client/` with `SnakeRenderer.ts`, `SnakeRenderer.test.ts`, and `index.ts`
- [x] Create `games/tictactoe/host/` with `TicTacToeGame.hpp`, `TicTacToeGame.cpp`, and `test_tictactoe.cpp`
- [x] Create `games/tictactoe/client/` with `TicTacToeRenderer.ts` and `index.ts`
- [x] Wire tests in CMake and verify both C++ unit tests and frontend tests pass
