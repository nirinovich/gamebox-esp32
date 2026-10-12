# 05: Real-Time 60 FPS Games Migration and Touch Ergonomics

**What to build:** The remaining 60 FPS tick-based games and complex tabletop game (Snake, Pong, Tron, Dominoes), paired with mobile ergonomic touch controls: 60px Virtual D-pad, 1:1 vertical paddle drag in Pong, and swipe gestures in Tron and Snake.

**Blocked by:** 04: Canvas Presentation Layer (`<GameCanvas />`) and Turn-Based Games Migration

**Status:** resolved

- [x] `SnakeRenderer` implements `IGameRenderer` with hexagonal heads, directional robotic eyes, and radiant glowing food with drifting embers
- [x] `PongRenderer` implements `IGameRenderer` with phosphor comet trails and paddle contact collision sparks
- [x] `TronRenderer` implements `IGameRenderer` with luminous neon bloom light cycle trails and collision shockwaves
- [x] `DominoRenderer` implements `IGameRenderer` with emerald billiard felt table, 3D ivory tiles, legal move pulsing halos, Boneyard drawing drawer, and opponent hand masking
- [x] Virtual D-pad renders an ergonomic 60px button grid with the down button placed below left/right
- [x] Direct canvas pointer dragging enables 1:1 vertical paddle control in Pong
- [x] Canvas swipe detection enables cardinal steering in Tron and Snake
