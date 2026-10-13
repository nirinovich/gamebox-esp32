# 07: Ludo Client Renderer & Interactive Track Presentation

**What to build:** A vibrant, touch-friendly Canvas 2D Ludo presentation client. Renders the classic four-color (Red, Green, Yellow, Blue) quadrant board layout, animated 3D-styled dice roll button/cube, pawn stacking indicators, glowing selection rings around legal movable pawns, and smooth path movement animations.

**Blocked by:** 06: Ludo Authoritative Board Engine & Tactical Bot Controller

**Status:** resolved

- [x] `LudoRenderer.ts` under `games/ludo/client/` renders the full cross-and-circle board geometry with colored yards, track spaces, safe stars, and home triangles.
- [x] Central or contextual dice roll component animates the rolled pip value (1 to 6).
- [x] When multiple pawns are eligible to move, glowing rings highlight valid pawns; tapping a pawn dispatches `{ cmd: "move", pawn_id: id }`. If only one pawn is eligible, auto-selection is supported.
- [x] Multiple pawns occupying the same safe square render as clean stacked mini-badges with player counts.
- [x] Unit tests in `LudoRenderer.test.ts` verify pawn selection hit-detection and dice roll action dispatches.
