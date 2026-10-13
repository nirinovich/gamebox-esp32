# 11. Room Lifecycle Auto-Detachment and Neo-Arcade Client Polish

Date: 2026-09-25

## Status

Accepted

## Context

Following a completed 2-player match, players remained bound to the finished `Room` on the Host server. Subsequent commands (such as starting a solo game or hosting another match) were erroneously routed into the finished `Room`, causing the finished game state to be rebroadcast and trapping the Client in the game-over modal unless the browser tab was refreshed.

## Decision

1. **Auto-Detachment on Host**:
   - In both desktop (`main.cpp`) and ESP32 firmware (`main.cpp`), before creating a room or routing input, the Host automatically detaches any Session from previous finished Rooms.
   - Multiplayer input routing explicitly requires `!activeRoom->isFinished()`. Once a game finishes, input is no longer accepted, preventing stale state loops.
   - Returning to the lobby or starting a new game dispatches an explicit `leave` message from Client to Host.

2. **Client-Side State Reset**:
   - The Client resets `lastState = null`, `prevFinished = false`, `prevScore = 0`, and clears particles/screen shake whenever returning to the Lobby or starting any game.

3. **Neo-Arcade Cabinet Polish (Impeccable Bolder/Delight)**:
   - Procedural Web Audio synthesizer expanded with `laser()` and `explosion()` sounds.
   - Screen shake and particle bursts calibrated for tactile collision feedback.
   - Tactile button press feedback and dark arcade aesthetic with 0 external network dependencies.

## Consequences

- Completely eliminates the stale game-over modal loop without requiring client browser reloads.
- Clean session-to-room lifecycle management with robust fallback.
- Enhanced sensory feedback matching the Gamebox neo-arcade identity.
