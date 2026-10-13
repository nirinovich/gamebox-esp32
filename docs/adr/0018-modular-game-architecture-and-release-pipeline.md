# Modular Game Architecture, Functional State Core, and Release Pipeline

## Context
1. **Scattered Game Artifacts**: Adding or modifying a game previously required editing disconnected files across `include/gamehub/core/`, `src/core/`, `frontend/src/renderers/`, `frontend/src/types/protocol.ts`, `GameFactory.hpp`, `GameCanvas.tsx`, and `tests/`. This high coupling hindered adding new games cleanly.
2. **Ambiguity Around OOP in Game Design**: Classical deep OOP entity hierarchies (`Entity -> Actor -> Pawn` with virtual update/draw) cause heap fragmentation and cache misses on microcontrollers like the ESP32-S3. A clear boundary was required separating engine polymorphism from internal game state logic.
3. **Lack of Formal Release & Versioning Pipeline**: No single source of truth existed for project versions across C++ firmware, Desktop server, and Frontend, and merge conventions from `dev` to `main` were unspecified.

## Decision
1. **Feature-Sliced Game Modules (`/games/<game>/`)**:
   - Every game is self-contained under `games/<game_name>/`:
     - `games/<game>/host/`: C++ rules, state models, and game-specific unit tests (`test_<game>.cpp`).
     - `games/<game>/client/`: Canvas 2D renderer (`<Game>Renderer.ts`), custom HUD/controls, and client tests.
   - Core engine infrastructure remains isolated in `include/gamehub/` and `src/` (HostEngine, RoomManager, Room, Session, network adapters).
   - Client shell remains isolated in `frontend/src/` (Lobby, Navigation, GameCanvas, SoundEngine, GameSocket).

2. **Polymorphic Boundary with Functional State Core**:
   - **Host Boundary**: The Host engine interacts with games polymorphically via the virtual `class Game` interface (`init()`, `update(dt)`, `handleInput(id, json)`, `serializeStateForPlayer(id)`, `isFinished()`, `getWinner()`).
   - **Internal Game Core**: Inside each game, avoid deep OOP class hierarchies. Game state is modeled as Plain Old Data (POD) structs with fixed-size arrays and zero heap allocations during tick execution. Game transitions are pure deterministic reducers or finite state machines over POD state.

3. **Authoritative State Snapshot Replication**:
   - The C++ Host is the sole authority. It serializes state into JSON snapshots broadcast over WebSocket on tick intervals or state mutations.
   - The Client is a thin visual terminal: it renders authoritative snapshots at 60 FPS using Canvas 2D with procedural juice (particle emitters, camera shake), and dispatches player intent actions back to the Host.

4. **Explicit Compile-Time Registries**:
   - Host uses `GameRegistry.hpp` mapping `GameType` to concrete factory constructors, guaranteeing embedded linker safety without dynamic macro loading.
   - Client uses `gameRegistry.ts` mapping `GameType` to renderer instances and game metadata.

5. **Single Source-of-Truth Versioning & GitHub Flow**:
   - A root `VERSION` file (SemVer `MAJOR.MINOR.PATCH`) is the single source of truth consumed by CMake (`GAMEBOX_VERSION`), Vite, and PlatformIO.
   - Active development occurs on `dev` using Conventional Commits (`feat:`, `fix:`, `refactor:`, `chore:`).
   - Releases are cut by merging `dev` into `main` with a non-fast-forward merge commit (`Merge dev into main for vX.Y.Z`) and creating an annotated git tag `vX.Y.Z`.

6. **Co-located Unit Tests**:
   - Game-specific logic tests live in `games/<game>/host/test_<game>.cpp`.
   - Engine-wide integration tests (room lifecycle, host engine ticks, networking) remain in `tests/`.
