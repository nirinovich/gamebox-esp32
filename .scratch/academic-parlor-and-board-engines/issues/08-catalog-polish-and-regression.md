# 08: Platform Catalog Polish & End-to-End Regression Verification

**What to build:** Unifies the platform lobby and client shell around the academic parlor and board roster. Features custom vector icons and descriptions for Malagasy Dominoes, Texas Hold'em, and Ludo; updates RoomDirectory tabs (Solo AI, 2P Duel, 4P Parlor Table); and executes full regression suites across C++ unit tests, desktop server runners, and firmware builds.

**Blocked by:** 03: Malagasy Dominoes Tactile UI/UX & Mobile Hand Overhaul, 05: Texas Hold'em Client Renderer & Mobile Betting Interface, 07: Ludo Client Renderer & Interactive Track Presentation

**Status:** resolved

- [x] `Lobby.tsx` and `gameStore.ts` present exclusively Malagasy Dominoes, Texas Hold'em Poker, and Ludo with polished descriptions and category tags.
- [x] RoomDirectory displays correct player capacity (`max: 4` for Table games, `max: 2` for Duels).
- [x] Vector icons for cards, dominoes, and dice are integrated cleanly into the icon component system.
- [x] Desktop server runner and unit tests build and pass 100% green without deprecation warnings.
- [x] Automated regression tests verify end-to-end room lifecycle: create room, join, solo bot launch, and rematch across all three games.
