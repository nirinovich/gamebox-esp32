# 02: Archivage du moteur de Poker (TexasHoldemGame)

**What to build:**
Archive the Texas Hold'em Poker engine to streamline the project and focus the curriculum on the three target games (Domino, Ludo, Katro) per ADR-0022.
- Remove `PokerGame.cpp` from the active CMake build targets (`gamehub_core`) and `test_poker.cpp` from CTest.
- De-register `poker` and `poker_solo` from active runtime `GameRegistry`.
- Retain `games/poker/` as an archived reference without breaking compilation or runtime catalogs.

**Blocked by:** None (can start immediately)

**Status:** resolved

- [x] `PokerGame.cpp` detached from active `gamehub_core` library in `CMakeLists.txt`
- [x] Poker entries removed or commented out from active registry in `GameRegistry.hpp`
- [x] Build succeeds cleanly and existing tests pass
