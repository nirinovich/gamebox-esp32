# 02: Core Polymorphic Game Architecture & Solo Snake

**What to build:** The abstract `Game` base class and a complete, playable Solo Snake game. The web menu allows selecting "Play Solo Snake". The Host runs a fixed 15 Hz tick loop simulating snake kinematics, apple spawning, boundary/self-collision, and score tracking. The state is serialized to JSON and rendered on the client canvas, supporting both keyboard (WASD/arrows) and virtual on-screen touch D-pad controls with restart functionality.

**Blocked by:** 01 (Project Scaffold & End-to-End WebSocket Tracer Bullet)

**Status:** resolved

- [x] Abstract base class `Game` defines virtual lifecycle methods (`init`, `update`, `handleInput`, `serializeState`, `isFinished`, `getWinner`).
- [x] `SnakeGame` implements grid kinematics, apple consumption, tail growth, boundary checks, and self-collision.
- [x] Fixed tick loop updates game simulation at 15 Hz and broadcasts JSON state to connected Session.
- [x] Web client renders snake body, food apple, current score, and game over screen on HTML5 canvas.
- [x] Desktop keyboard (WASD/arrows) and mobile on-screen D-pad buttons dispatch direction commands upstream.
- [x] Automated tests at the Room seam verify snake movement, apple consumption, growth, and collision death.
