# ADR 0028: Fanorona Malagasy Engine and Decoupled Client Adapter

Status: accepted

## Context & Decision

A complete implementation of the traditional Malagasy board game **Fanorona** (Fanoron-tsivy 5x9) was authored on feature branch `dev-fanorona`. However, it branched from an older pre-ADR-0020 codebase that:
1. Hardcoded game types into the monolithic `GameType` enum and `GameFactory.hpp`.
2. Hacked manual branching into `src/core/HostEngine.cpp` and `src/core/Room.cpp`.
3. Used non-canonical canvas styles (Tailwind slate `#0f172a`, arbitrary button colors) disconnected from the Gamebox Design System (`#14151b`, `#ef3938`, `#ffffff`).

We decided to integrate Fanorona into the current `dev` base following our modern decoupled architecture while preserving the author's core engine and rules verbatim ("Don't change the code really that much"):
1. **Decentralized Host Self-Registration (`GameRegistry`)**:
   - `FanoronaGame` self-registers via `REGISTER_GAME("fanorona_solo", ...)` and `REGISTER_GAME("fanorona_duel", ...)` inside `games/fanorona/host/FanoronaGame.cpp`.
   - Zero centralized enum pollution and zero manual switches added to `HostEngine.cpp` or `Room.cpp`.
   - Implements `getMoveSeq() const override { return m_moveCounter; }` and notifies `m_listener->onGameFinished(m_winner)` upon victory or immobilization detection per ADR 0027.
2. **Decoupled Client Discovery & Plugin Export**:
   - `games/fanorona/client/index.ts` exports `plugin: ClientPlugin` with `gameIds: ['fanorona', 'fanorona_solo', 'fanorona_duel']`, automatically discovered by Vite's `import.meta.glob`.
   - `Icon.tsx` maps `fanorona_solo` and `fanorona_duel` to clean geometric Lucide icons (`Compass` and `Grid2X2`).
   - `GameOverModal.tsx` surfaces authoritative reason text (`state.reason`) describing victory conditions (all stones captured or immobilization).
3. **Anti-Slop Craft Floor Compliance on Canvas Surfaces**:
   - Canvas backdrop aligns to Obsidian `#14151b` (`--gb-bg-canvas`).
   - On-canvas tactical choice prompt buttons (*Aspiration* and *Percussion*) and the turn completion button (*Achever le tour*) are styled using canonical Gamebox tokens (`#ef3938`, `#1c1d25`, `#ffffff`) and console typography.
   - Traditional board grid, stone styling, and particle juice are fully preserved.

## Consequences

- Fanorona Malagasy is fully playable in Solo (vs Bot) and 2-Player Duel modes.
- Core engine rules, topological graph checks, approach/withdrawal captures, and bot heuristics remain 100% faithful to the author's implementation.
- All host and client test suites compile and pass cleanly across Debug and Release targets.
