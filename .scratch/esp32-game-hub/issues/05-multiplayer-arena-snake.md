# 05: Real-Time Multiplayer Arena Snake (2-Player Duel)

**What to build:** 2-player real-time arena Snake running over RoomCodes. Two players join a shared Snake room; each controls a distinct colored snake simultaneously on the same grid. Both players compete for apples, and collision with walls, own tail, or opponent's body awards victory to the surviving player.

**Blocked by:** 04 (Room Manager & 2-Player Multiplayer Tic-Tac-Toe)

**Status:** resolved

- [x] `SnakeGame` supports 2 simultaneous player snakes with distinct starting positions, orientations, and colors.
- [x] Real-time tick loop synchronizes inputs from both Sessions concurrently.
- [x] Mutual collision rules: crashing into opponent's body or head-to-head collision triggers definitive win/draw outcomes.
- [x] Client canvas renders both snakes, scores, and real-time victory screen for multiplayer matches.
- [x] Tests at Room seam verify 2-player input handling, simultaneous movement, and duel collision resolution.
