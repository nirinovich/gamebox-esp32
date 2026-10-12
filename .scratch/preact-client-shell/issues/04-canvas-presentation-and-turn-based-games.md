# 04: Canvas Presentation Layer (`<GameCanvas />`) and Turn-Based Games Migration

**What to build:** The `<GameCanvas />` container component with high-DPI `RetinaViewport` clamped scaling and camera shake matrix, coupled with the modular `IGameRenderer` strategy hierarchy. Migrates the procedural Web Audio synthesizer and the 3 turn-based games: Tic-Tac-Toe, Connect Four, and Battleship.

**Blocked by:** 03: Room Waiting Card, Match HUD, and Rematch Series Lifecycle

**Status:** resolved

- [x] `<GameCanvas />` mounts high-DPI canvas buffer scaled by `devicePixelRatio` (clamped to 2x) with logical coordinate mapping
- [x] Camera shake coordinator translates canvas coordinates by decaying impulse vectors without jittering
- [x] Procedural Web Audio synthesizer (`SoundEngine`) generates retro chiptune sound effects for moves, impacts, and victories
- [x] `TicTacToeRenderer` implements `IGameRenderer` with animated drawn stroke paths and laser overdrive winning line
- [x] `ConnectFourRenderer` implements `IGameRenderer` with falling gravity disc physics, elastic settling bounce, and column targeting beam
- [x] `BattleshipRenderer` implements `IGameRenderer` with 360-degree sonar sweep line, concentric water ripples on misses, and fire/smoke explosions on hits
- [x] Dispatched player clicks/taps emit valid `MOVE` or `INPUT` actions to the Host
