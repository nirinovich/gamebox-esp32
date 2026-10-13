# 03: Classic Real-Time Games Migration (Pong & Tron)

**What to build:**
Migrate `Pong` and `Tron` into self-contained `GameModule` packages under `games/pong/` and `games/tron/`, packaging their authoritative tick physics, co-located unit tests, and Canvas 2D renderers.

**Blocked by:** 02: Reference Game Migration (Snake & TicTacToe) as Vertical Slices

**Status:** resolved

- [x] Create `games/pong/host/` (`PongGame.hpp`, `PongGame.cpp`, `test_pong.cpp`) and `games/pong/client/` (`PongRenderer.ts`, `index.ts`)
- [x] Create `games/tron/host/` (`TronGame.hpp`, `TronGame.cpp`, `test_tron.cpp`) and `games/tron/client/` (`TronRenderer.ts`, `index.ts`)
- [x] Wire tests in CMake and verify green execution
