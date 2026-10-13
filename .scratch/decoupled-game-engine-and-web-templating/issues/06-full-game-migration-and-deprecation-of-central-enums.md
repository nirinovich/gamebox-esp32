# 06: Full Game Migration and Deprecation of Central Enums

**What to build:**
Migrate all remaining games (Snake, Tic-Tac-Toe, Connect Four, Pong, Tron, Battleship) to self-register via `GameRegistry`. Completely delete `enum class GameType`, the `GameFactory.hpp` switch statement, and the hardcoded `GAME_CATALOGUE` array in `gameStore.ts`. Drive virtual D-Pad mounting purely from the active game's declared `InputTemplate`.

**Blocked by:** 04: Client Generic ActionToolbar and ActionModal Presentation, 05: Client Build-Time Plugin Discovery via Vite Glob

**Status:** resolved

- [x] Migrate Snake, Tic-Tac-Toe, Connect Four, Pong, Tron, and Battleship to register via `REGISTER_GAME`.
- [x] Remove `enum class GameType` and central factory switch statements across the C++ codebase.
- [x] Remove hardcoded `GAME_CATALOGUE` in `gameStore.ts` and populate lobby entirely from Host directory feed.
- [x] Drive `VirtualDPad` mounting in `GameCanvas.tsx` via `InputTemplate` rather than game name substring matching.
