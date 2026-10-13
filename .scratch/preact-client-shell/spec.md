# Spec: Preact Single-File Client Shell and Asset Pipeline

Status: ready-for-agent

## Problem Statement

Gamebox's client interface currently resides as an uncompiled, 2,700-line monolithic JavaScript and CSS string literal embedded directly inside a C++ header file (`WebAssets.hpp`) split across 16 fragmented raw string chunks. Both players and developers encounter critical technical and operational problems:

1. **Brittle Developer Experience & High Bug Surface**: Frontend code inside raw C++ string literals lacks syntax highlighting, code completion, ESLint linting, and static TypeScript type checking. A single syntax error or typo in a WebSocket message handler is only caught at runtime in the browser console after recompiling C++ and relaunching the server.
2. **State Synchronization & DOM Desynchronization Bugs**: UI updates rely on manual, imperative DOM manipulation (`document.getElementById`, `.innerHTML = ...`, `.classList.toggle(...)`). This creates frequent UI desync issues, such as stale room codes, ghost action buttons, stuck loading spinners, and unhandled race conditions when WebSocket state packets arrive rapidly.
3. **Monolithic Spaghetti Architecture**: The presentation layer tightly couples the procedural Web Audio synthesizer, touch gesture handlers, 7 different game renderers, and lobby navigation inside one global script. Modifying one game risks breaking unrelated games or lobby views.
4. **Slow Local Iteration Cycles**: Frontend styling and behavioral tweaks currently require recompiling the C++ desktop binary (`cmake --build`) or re-flashing the ESP32-S3 microcontroller (`pio run`), eliminating modern instant hot-module-reloading (HMR) workflows.
5. **Mobile Captive Portal Memory & Network Fragility**: When mobile devices connect to the local Wi-Fi Access Point, backgrounding the browser or locking the screen abruptly suspends WebSocket connections. The current client lacks exponential backoff and session re-synchronization, forcing players to refresh the page and forfeit their active room session.

## Solution

A decoupled, modern frontend architecture powered by **Preact**, **TypeScript**, and **Vite**, compiled into a single-file inlined offline bundle and automatically embedded into C++ flash memory:

1. **Declarative Preact ClientShell**: Rebuild all UI views (Header, Lobby, RoomDirectory, Profile bar, Game Selector, HUD overlays, SeriesScore tallies, Rematch dialogs, and touch controls) into clean Preact components using **Preact Signals** (`@preact/signals`) for zero-overhead, fine-grained leaf-level reactivity.
2. **Preserved 60 FPS Canvas VisualEngine**: Retain the high-performance HTML5 Canvas 2D presentation layer and high-DPI `RetinaViewport` inside a dedicated `<GameCanvas />` container component. Game rendering runs on dedicated animation frames without Virtual DOM diffing overhead.
3. **Polymorphic Game Renderer Strategy Hierarchy**: Decompose game rendering into an `IGameRenderer` interface (`init`, `resize`, `render`, `handleInput`, `destroy`). Each of the 7 games (Snake, Pong, Tron, Dominoes, TicTacToe, Battleship, ConnectFour) lives in its own typed, isolated module.
4. **Resilient GameSocket Client**: A typed WebSocket manager providing exponential backoff reconnection, heartbeat latency calculation, and automatic session and room recovery when mobile devices wake up.
5. **Single-File Inlined Bundle & Automated C++ Generator**: Use `vite-plugin-singlefile` to bundle all JavaScript, CSS, and SVG assets into a single, self-contained `dist/index.html` file (<45 KB minified). A Python build script automatically slices this file into ~8 KB raw string literal chunks in `WebAssets.hpp`, preserving the existing C++ API and single-command builds (ADR 0004).
6. **Sub-50ms Developer Experience**: Vite development server configured to proxy WebSocket traffic (`ws://localhost:5173/ws` to `ws://localhost:8080/ws`), enabling instant Hot Module Replacement during UI design against the live C++ authoritative game engine.

## User Stories

1. As a player on a smartphone connecting to the Gamebox Wi-Fi captive portal, I want the web application to load completely offline within 200ms, so that I can start playing immediately without internet connectivity.
2. As a player in the Lobby, I want the RoomDirectory feed to update in real time with smooth animations and live radar beacons, so that I can see open joinable games as soon as they are created.
3. As a player entering my nickname in the profile bar, I want my name to persist in local storage and synchronize across rooms, so that I do not have to retype it between sessions.
4. As a player selecting a game mode, I want clear tabs for Solo, 2-Player Duel, and Multiplayer Party modes, so that I can quickly browse available games.
5. As a player waiting in a private Room, I want a prominent room code card with an animated radar pulse and a quick copy-to-clipboard action, so that I can easily invite a friend to join my match.
6. As a player joining a Room, I want instantaneous feedback and clear validation if the room code is invalid or the room is full, so that I am never stuck on an indefinite loading screen.
7. As a mobile player in Pong, I want to drag my finger vertically on the canvas to move my paddle, so that I have fluid 1:1 analog paddle control.
8. As a mobile player in Snake and Tron, I want to swipe in any cardinal direction on the canvas, so that steering feels natural on touchscreens without looking at buttons.
9. As a mobile player preferring virtual buttons, I want an ergonomic directional pad with large touch targets (60px) and active tactile feedback, so that I can make precise turns without misclicks.
10. As a Connect Four player, I want column touch targets with luminous highlight beams, so that I always drop my chip into the intended column.
11. As a Battleship player, I want an interactive targeting grid that clearly distinguishes my fleet placement from active naval radar targeting, so that tactical moves are easy to execute.
12. As a Dominoes player, I want my legal playable tiles and open matching ends on the table to pulse with glowing halos, so that I can easily spot valid moves.
13. As a Dominoes player, I want the Boneyard draw button to be prominently highlighted when I have no legal moves, so that I never get confused about my options.
14. As a player in an active match, I want a responsive HUD displaying player handles, color badges, current scores, and connection status, so that game status is visible at a glance.
15. As a player finishing a match, I want a polished Game Over modal with animated confetti, victory banners, and an instant Rematch button, so that competitive rivalries can continue seamlessly.
16. As rival players playing consecutive rematches, I want the in-room SeriesScore tally to track our head-to-head match wins, so that we know who wins the overall set.
17. As a player whose mobile phone temporarily sleeps or switches apps, I want the client to automatically reconnect and recover my room session with exponential backoff, so that I do not get kicked out of active games.
18. As a player with audio enabled, I want procedural retro chiptune sound effects for moves, impacts, and victories, so that the arcade experience feels immersive.
19. As a player in a quiet room, I want an easily accessible audio mute toggle in the header that persists across browser reloads, so that sound preferences are respected.
20. As a player who enjoys retro visuals, I want to toggle a simulated CRT scanline overlay, so that I can enjoy an authentic 1980s arcade feel.
21. As a player sensitive to screen shake or motion, I want the interface and game canvas to respect system `prefers-reduced-motion` settings, so that animations remain accessible.
22. As a frontend engineer developing the UI, I want full TypeScript type definitions matching C++ WebSocket domain packets, so that protocol errors are caught during compilation.
23. As a frontend engineer modifying components, I want Vite Hot Module Replacement (HMR) running against the native C++ desktop runner, so that I see UI updates in under 50ms without restarting the C++ server.
24. As a frontend engineer debugging games, I want each game's rendering and input handling encapsulated in a dedicated module implementing `IGameRenderer`, so that I can debug and test game logic in isolation.
25. As a frontend engineer styling components, I want CSS Modules with centralized Neo-Arcade custom property design tokens, so that styles are scoped without class name collisions or bloated runtime libraries.
26. As a C++ firmware developer, I want all frontend assets compiled into a single C++ header file committed to git, so that I can build and flash the ESP32 firmware with `pio run` without having Node.js or npm installed.
27. As a C++ desktop developer, I want `WebAssets.hpp` to retain its existing public API (`getParts()`, `getIndexHtml()`), so that `DesktopServer.cpp` and `main.cpp` require zero modifications.
28. As an embedded developer flashing firmware, I want the compiled frontend bundle to remain under 45 KB minified, so that flash memory utilization and captive portal transmission latency are minimized.
29. As a QA engineer testing the client, I want automated unit tests verifying that WebSocket messages correctly transition Preact Signals and UI views, so that regressions are caught before deployment.
30. As an automated CI build runner, I want asset generation to run deterministically and verify chunk size limits, so that compiler string literal limits are never exceeded.

## Implementation Decisions

### 1. Presentation Layer Architecture & Boundaries
- The application will be partitioned into two distinct presentation layers:
  - **ClientShell (Preact DOM)**: Governs all high-level DOM views, overlays, and modals: Header, Connection badge, Profile bar, Game selection tabs, RoomDirectory feed, Room creation forms, Waiting room card, Game HUD, SeriesScore tally, Game Over modal, Virtual D-pad, and Audio controls.
  - **VisualEngine (HTML5 Canvas 2D)**: Governs real-time 60 FPS presentation via an uninhibited canvas buffer mounted inside a `<GameCanvas />` container component. It handles high-DPI clamped Retina scaling, the universal particle object pool, camera impulse screen-shake, and frame interpolation.
- Turn-based and real-time game presentation will implement an `IGameRenderer` strategy interface:
  ```ts
  interface IGameRenderer {
    init(canvas: HTMLCanvasElement, ctx: CanvasRenderingContext2D): void;
    resize(width: number, height: number, scale: number): void;
    render(state: GameStatePayload, juice: JuiceCoordinator): void;
    handlePointerDown(x: number, y: number): void;
    handlePointerMove(x: number, y: number): void;
    handlePointerUp(x: number, y: number): void;
    destroy(): void;
  }
  ```
  Each of the 7 games (Snake, Pong, Tron, Dominoes, TicTacToe, Battleship, ConnectFour) will be implemented as an isolated class implementing this interface.

### 2. Reactive State Management & Protocol Layer
- State will be modeled using `@preact/signals` for fine-grained leaf-level reactivity without virtual DOM cascade re-renders:
  - `connectionStatus`: `'DISCONNECTED' | 'CONNECTING' | 'CONNECTED'`
  - `latencyMs`: Round-trip heartbeat duration
  - `currentView`: `'LOBBY' | 'WAITING' | 'GAME'`
  - `playerName`: Stored in and hydrated from `localStorage`
  - `activeRoom`: Nullable snapshot containing `roomCode`, `gameType`, `players`, `seriesScore`, `roomLifecycle`
  - `roomDirectory`: Array of active public rooms broadcast from the Host
  - `gameState`: Real-time polymorphic game state payload received from the Host tick loop
- A singleton `GameSocket` service will manage WebSocket connectivity:
  - Exponential backoff retry loop (1s, 2s, 4s, capped at 10s).
  - Outgoing message typing: `CREATE_ROOM`, `JOIN_ROOM`, `LEAVE_ROOM`, `MOVE`, `INPUT`, `REMATCH`.
  - Heartbeat ping/pong messages to detect dropped TCP connections on mobile sleep.
  - Automatic session re-query on reconnection to restore active room status.

### 3. Styling & Neo-Arcade Aesthetics
- Styling will use CSS Modules (`*.module.css`) combined with CSS custom properties (`--bg`, `--surface`, `--p1-cyan`, `--p2-pink`, `--bot-amber`, `--border`, `--radius-lg`, `--ease`).
- Preserves frosted-glass backdrop blur (`backdrop-filter: blur(16px)`), responsive mobile centering (max-width 440px), and glowing neon accent highlights.
- Zero external CSS libraries or runtime styling engines to maintain strict bundle size limits.

### 4. Build Pipeline & C++ Integration
- The frontend will be isolated in a `frontend/` directory with its own `package.json`, `tsconfig.json`, and `vite.config.ts`.
- `vite-plugin-singlefile` will bundle all JS, CSS, and inlined SVGs into a single self-contained `frontend/dist/index.html`.
- A Python generator script (`scripts/generate_web_assets.py`) will read `frontend/dist/index.html` and write `include/gamehub/web/WebAssets.hpp`:
  - Output is formatted as an array of `const char*` raw string literals (`R"rawliteral(...)rawliteral"`), strictly chunked at <= 8 KB boundaries to prevent MSVC compiler limits.
  - Generates the standard C++ API methods `WebAssets::getParts()` and `WebAssets::getIndexHtml()`, requiring zero changes to `DesktopServer.cpp` and `main.cpp`.
  - The generated header will be committed to version control so C++ compilation via CMake and PlatformIO remains completely standalone with no Node.js requirement.

### 5. Local Development Proxying
- Vite dev server will configure a local WebSocket proxy:
  - Requests to `ws://localhost:5173/ws` will proxy directly to `ws://localhost:8080/ws`.
  - Allows full Hot Module Replacement while interacting with the running C++ `gamehub_desktop` runner.

## Testing Decisions

### What Makes a Good Test
Tests must verify external behavioral contracts and observable state transitions, not internal component implementation details. Unit tests must execute quickly without opening real network ports or requiring physical display hardware.

### Modules Tested
1. **Protocol Ingestion & State Transitions**: Verify that injecting synthetic WebSocket messages (`ROOM_LIST`, `ROOM_STATE`, `TICK`, `GAME_OVER`) into the `GameSocket` correctly updates the reactive Preact Signals and transitions the active view (`Lobby` -> `Waiting` -> `Game`).
2. **Action Dispatch Verification**: Verify that user actions (creating a room, moving a piece, requesting a rematch) emit the exact JSON payload schema required by the C++ engine.
3. **Game Renderer Lifecycle**: Verify that `IGameRenderer` instances initialize, handle coordinate scaling, process inputs, and cleanly destroy/release resources when transitioning out of a room.
4. **Single-File Asset Generator**: Verify that `scripts/generate_web_assets.py` parses `dist/index.html`, chunks output strictly under 8 KB per chunk, produces valid C++ syntax, and matches the expected public API.
5. **C++ Compilation Seam**: Verify that the generated `WebAssets.hpp` compiles and passes all native desktop unit tests via `ctest --test-dir build`.

### Prior Art
- C++ HostEngine and RoomManager unit tests in `tests/test_host_engine.cpp` and `tests/test_room_manager.cpp`.

## Out of Scope
- Rewriting C++ game domain logic, bot strategies, or HostEngine state machines.
- Rewriting Canvas 2D game rendering into DOM or SVG elements.
- Storing web assets in LittleFS/SPIFFS filesystem partitions (ADR 0004 remains strictly enforced).
- External network requests or CDN dependencies of any kind.
- WebGL or 3D graphics rendering.

## Further Notes
- Respects [ADR 0001](file:///c:/Users/Nikita/Documents/C++/docs/adr/0001-authoritative-cpp-engine-thin-web-client.md) (Authoritative C++ Engine on ESP32 with Thin Web Client).
- Respects [ADR 0004](file:///c:/Users/Nikita/Documents/C++/docs/adr/0004-embedded-flash-assets.md) (Embedded Flash String Literals for Web Assets).
- Implements [ADR 0014](file:///c:/Users/Nikita/Documents/C++/docs/adr/0014-preact-single-file-inlined-client-shell.md) (Preact Single-File Inlined Client Shell).
- Aligns with the domain vocabulary in [CONTEXT.md](file:///c:/Users/Nikita/Documents/C++/CONTEXT.md) including `Host`, `Client`, `ClientShell`, `VisualEngine`, `RetinaViewport`, `RoomDirectory`, and `SeriesScore`.
