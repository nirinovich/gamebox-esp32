# Spec: Modular Game Architecture, Functional State Core, and Release Pipeline

Status: resolved

## Problem Statement

As Gamebox expands from a prototype collection of games into a full multi-game arcade platform running on an ESP32-S3 Host and browser Client, the existing repository structure and architectural patterns suffer from three compounding issues:

1. **Scattered Game Artifacts & High Friction for Adding Games**:
   Currently, adding or modifying a single game requires editing and maintaining disconnected files scattered across at least 8 directories: C++ header in `include/gamehub/core/`, C++ implementation in `src/core/`, TypeScript renderer in `frontend/src/renderers/`, protocol definitions in `frontend/src/types/protocol.ts`, factory mappings in `GameFactory.hpp`, Canvas dispatch in `GameCanvas.tsx`, catalog entries in `Lobby.tsx`, and tests in `tests/`. There is no self-contained boundary for a game module, making it tedious and error-prone to add, test, or remove a game.

2. **Ambiguity Around OOP in Embedded Real-Time Game Design**:
   Traditional "schoolbook" object-oriented programming (deep inheritance trees such as `Entity -> Actor -> Pawn`, multiple virtual methods per entity, and frequent dynamic allocations with `new`) causes heap fragmentation and poor cache locality on microcontrollers like the ESP32-S3. There was ambiguity about how OOP should be applied—whether the game simulation should be an object hierarchy or whether game logic should be separated from presentation.

3. **Undefined Release Cadence & Branch Versioning**:
   The workflow for merging active work from `dev` into `main` lacked a formal naming convention, tagging strategy, and single source-of-truth version file. The C++ firmware, desktop runner, and frontend bundle lacked unified semantic version synchronization.

## Solution

A feature-sliced modular game architecture, functional state core design, and standard GitHub Flow release pipeline:

1. **Feature-Sliced Game Modules (`/games/<game_name>/`)**:
   Colocate all assets for each game inside its own self-contained directory:
   - `games/<game>/host/`: Authoritative C++ game rules, state definitions, and unit tests (`test_<game>.cpp`).
   - `games/<game>/client/`: Canvas 2D renderer (`<Game>Renderer.ts`), custom controls/HUD, tests, and metadata entrypoint.
   - Core engine infrastructure remains isolated in `include/gamehub/core/` and `src/core/` (HostEngine, RoomManager, Room, Session, network adapters).
   - The ClientShell remains isolated in `frontend/src/` (Lobby, Navigation, GameCanvas, SoundEngine, GameSocket).

2. **Polymorphic Boundary with Functional State Core**:
   - **Engine Boundary (OOP Polymorphism)**: The Host engine interacts with games solely through the abstract `Game` base class interface (`init()`, `update(dt)`, `handleInput(id, json)`, `serializeStateForPlayer(id)`, `isFinished()`, `getWinner()`).
   - **Internal Game Core (Value Semantics / Functional State)**: Inside each game module, game state is modeled as Plain Old Data (POD) structs with fixed-size arrays and zero heap allocations during the tick loop. Transitions are pure deterministic reducers or finite state machines over this POD state.

3. **Authoritative State Snapshot Replication**:
   - The C++ Host simulation is the sole source of truth. It serializes state into JSON snapshots broadcast over WebSocket on tick intervals or state mutations.
   - The Client is a thin visual terminal: it renders authoritative snapshots at 60 FPS using Canvas 2D with procedural juice (particles, screen shake) and sends pure player intent actions (`{ type: "ACTION", ... }`) back to the Host.

4. **Compile-Time Registries**:
   - Host uses an explicit `GameRegistry.hpp` mapping `GameType` to concrete factory constructors, guaranteeing linker safety on embedded targets without fragile static reflection macros.
   - Client uses `gameRegistry.ts` mapping `GameType` to renderers and catalog metadata.

5. **Single Source-of-Truth Versioning & GitHub Flow Pipeline**:
   - A root `VERSION` file (SemVer `MAJOR.MINOR.PATCH`, e.g., `1.0.0`) drives CMake project definitions, C++ compiler defines (`GAMEBOX_VERSION`), and client build assets.
   - Active development occurs on `dev` using Conventional Commits.
   - Releases are cut by merging `dev` into `main` with a non-fast-forward merge commit (`Merge dev into main for vX.Y.Z`) and creating an annotated git tag `vX.Y.Z`.

## User Stories

1. As a game developer, I want all files for a game (C++ rules, unit tests, frontend renderer, and custom UI) located inside a single `games/<game>/` directory, so that I can develop, maintain, and delete games without hunting across the entire codebase.
2. As a game developer, I want to implement a game using a clean `Game` interface in C++, so that the Host engine treats my game polymorphically without needing game-specific branching in `Room` or `HostEngine`.
3. As an embedded firmware developer, I want the internal state of games to use Plain Old Data (POD) structs and zero heap allocations during ticks, so that memory fragmentation and cache misses on the ESP32-S3 are prevented.
4. As a game developer, I want game rules to run as deterministic state reducers over plain state structs, so that I can easily write unit tests that verify state transitions with given inputs.
5. As a frontend developer, I want to write game renderers that receive authoritative JSON state snapshots and draw to Canvas 2D, so that game rendering is completely decoupled from microcontroller hardware.
6. As a frontend developer, I want a central `gameRegistry.ts` that maps `GameType` to renderer implementations, so that new games can be registered cleanly in the client shell.
7. As a backend developer, I want an explicit `GameRegistry.hpp` mapping `GameType` to game factory functions, so that embedded linkers never strip game implementations due to static initialization quirks.
8. As a release engineer, I want a single root `VERSION` file, so that the current version of the platform is visible in one place and consumed consistently by CMake and Vite.
9. As a developer preparing a release, I want to merge `dev` into `main` using a non-fast-forward merge commit and an annotated `vMAJOR.MINOR.PATCH` tag, so that release commits and change history are traceable in git.
10. As a tester, I want game unit tests to live directly alongside the game source code in `games/<game>/host/test_<game>.cpp`, so that test locality matches feature locality.
11. As a CI/CD pipeline, I want CMake to discover and build all game host libraries and tests, so that every commit is validated by automated test suites.
12. As a player on a mobile device, I want game controls to dispatch lightweight action packets to the Host, so that low-latency input streaming is maintained over captive portal Wi-Fi.
13. As a player in a room, I want games with hidden information (like Dominoes or Battleship) to deliver player-specific state snapshots, so that opponents cannot inspect private hands.

## Implementation Decisions

### 1. Directory Structure Topology
- Create root `games/` directory with subdirectories for all 7 current games:
  - `games/snake/`
  - `games/tictactoe/`
  - `games/pong/`
  - `games/tron/`
  - `games/connectfour/`
  - `games/battleship/`
  - `games/domino/`
- Each game directory contains:
  - `host/`: `<GameName>Game.hpp`, `<GameName>Game.cpp`, `test_<gamename>.cpp`
  - `client/`: `<GameName>Renderer.ts`, `<GameName>Renderer.test.ts`, `index.ts`
- Core engine infrastructure remains in `include/gamehub/core/` and `src/core/`:
  - `HostEngine`, `RoomManager`, `Room`, `Session`, `RoomState`, `Game.hpp`, `GameTypes.hpp`, `GameRegistry.hpp`, `INetworkAdapter.hpp`.
- Frontend client shell remains in `frontend/src/`:
  - `App.tsx`, `components/GameCanvas`, `components/Lobby`, `components/Header`, `services/GameSocket.ts`, `audio/SoundEngine.ts`.
  - Introduce `frontend/src/registry/gameRegistry.ts` mapping each `GameType` to its respective renderer imported from `@games/*`.

### 2. Build Tooling Configurations
- **CMake (`CMakeLists.txt`)**:
  - Read version from `VERSION` file: `file(STRINGS VERSION GAMEBOX_VERSION_STR)`.
  - Define `add_library(gamehub_games STATIC ...)` compiling all `games/*/host/*.cpp`.
  - Link `gamehub_games` to `gamehub_core`.
  - Register individual game test executables: `add_executable(test_snake games/snake/host/test_snake.cpp)`.
- **PlatformIO (`platformio.ini`)**:
  - Update `build_src_filter` to include `+<../games/*/host/*.cpp>`.
  - Pass `-Igames` to `build_flags`.
- **Vite & TypeScript (`frontend/vite.config.ts`, `frontend/tsconfig.json`)**:
  - Add path alias `@games/*` resolving to `../games/*/client/*`.
  - Enable Vite to import renderer modules across the workspace boundary.

### 3. Functional State Core Contract
Inside each game module, the state is defined as Plain Old Data:
```cpp
// Prototype shape for Game state model
struct SnakeState {
    Point body[100];
    int length = 0;
    Direction dir = Direction::RIGHT;
    Point food;
    int score = 0;
    bool alive = true;
};
```
The game class implements the virtual methods of `Game`:
```cpp
class SnakeGame : public Game {
public:
    void init() override;
    void update(float dt) override;
    void handleInput(PlayerId id, const std::string& commandJson) override;
    std::string serializeState() const override;
    std::string serializeStateForPlayer(PlayerId id) const override;
    bool isFinished() const override;
    int getWinner() const override;
private:
    SnakeState m_state;
    void stepSimulation();
    void applyAction(PlayerId id, Direction newDir);
};
```

### 4. Release & Merge Protocol
- Single version file: `VERSION` containing exact SemVer string (e.g. `1.0.0`).
- Merge dev to main:
  ```bash
  git checkout main
  git merge --no-ff dev -m "Merge dev into main for vX.Y.Z"
  git tag -a vX.Y.Z -m "Release vX.Y.Z"
  git push origin main --tags
  ```

## Testing Decisions

### Test Seams
1. **Host Game Polymorphic Boundary Seam (`class Game`)**:
   - The primary seam for game verification. Tests instantiate the game, invoke `init()`, `handleInput()`, and `update(dt)`, and assert that `serializeStateForPlayer()` returns the expected JSON structure.
   - Behavior-focused: Tests never inspect private internal state directly; they verify state transformations via public inputs and serialized outputs.
   - Colocated at `games/<game>/host/test_<game>.cpp`.
2. **Client Visual Presentation Seam (`IGameRenderer`)**:
   - The primary seam for visual verification in TypeScript. Tests instantiate the renderer, supply mock canvas contexts and `StateSnapshot` fixtures, and assert that expected render routines and `ActionDispatcher` callbacks fire.
   - Colocated at `games/<game>/client/<Game>Renderer.test.ts`.
3. **Engine Lifecycle Integration Seam (`RoomManager` & `HostEngine`)**:
   - System integration tests at `tests/test_host_engine.cpp` and `tests/test_room_manager.cpp` ensuring multi-session room lifecycle, solo/multiplayer room isolation, and auto-detachment function properly across all registered games.

## Out of Scope

1. Implementing dynamic shared-library runtime plugins (`.dll`/`.so` or runtime `.wasm` loading). All games are compiled statically into the Host firmware for embedded flash efficiency and zero overhead.
2. Rewriting existing game renderers from Canvas 2D to WebGL/Three.js.
3. Adding new games beyond the existing 7 catalog games during the architectural migration.

## Further Notes

- This specification adheres to ADR 0001 (Authoritative C++ Engine & Thin Web Client), ADR 0005 (Polymorphic Game Architecture), ADR 0016 (Gameplay Core and Canvas Grid Layer), ADR 0017 (Deep RoomManager and Presentation Seam), and ADR 0018 (Modular Game Architecture and Release Pipeline).
- Domain language is strictly enforced according to `CONTEXT.md` (`Host`, `Client`, `Session`, `Room`, `Game`, `GameModule`, `StateSnapshot`, `FunctionalStateCore`, `ReleaseTag`, `GameRegistry`, `VersionFile`).
