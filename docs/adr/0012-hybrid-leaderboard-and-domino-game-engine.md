# 12. Hybrid Leaderboard Architecture and 1-3 Player Draw Dominoes Engine

Date: 2026-09-25

## Status

Accepted

## Context

1. Snake Solo displayed a "High" HUD label, but no score was persisted or loaded either locally or on the Host, leaving players without persistent record progression.
2. The user requested extending the Gamebox platform with a classic Domino game supporting up to 3 players, available in both Solo and Multiplayer modes.
3. The server room architecture previously assumed a strict 2-player capacity (`std::pair<uint32_t, uint32_t>`), which would not accommodate 3-player Domino sessions.

## Decision

1. **Hybrid Leaderboard System**:
   - The Client persists the player`s `PersonalBest` and game win counters in browser `localStorage`.
   - The Host maintains the session `HostRecord` in memory and replicates it in state broadcasts.
   - The HUD updates in real-time, displaying celebrations when personal records are broken.

2. **1-3 Player Draw Dominoes (`DominoGame`)**:
   - Implements standard Double-Six set (28 tiles, `[0|0]` through `[6|6]`).
   - Supports 2 or 3 Players (Solo vs Bots, or 2P / 3P PvP).
   - Hand dealing: 7 tiles per player, remaining tiles placed in the `Boneyard`.
   - Sequential turn rotation: tile placement on matching open ends, drawing from the Boneyard when unable to play, passing only when the Boneyard is empty.
   - Heuristic Greedy/Pip-Maximizing Bot Controller for Solo play.
   - Resolution on empty hand ("Domino!") or lowest `PipSum` when the board is blocked.

3. **Room Scaling for 3 Players**:
   - Upgraded `Room::m_maxPlayers` and `RoomPlayers` map to support dynamic lists (`std::vector<uint32_t>`), allowing 2-player duels and 3-player domino games seamlessly.

## Consequences

- Full high score motivation and persistence without requiring user account infrastructure.
- Expands the platform to 7 polymorphic games / 11 game modes.
- Room infrastructure is now extensible to N players.
