# Spec: Decoupled Game Engine and Schema-Driven Web Templating

Status: resolved

## Problem Statement

Gamebox currently presents itself as a multi-game platform, but architecturally it operates as a tightly coupled firmware application rather than a true game engine. Adding or maintaining a game requires developers to make coordinated edits across centralized type enumerations, factory switch blocks, global client catalogues, and monolithic canvas presentation components.

Specifically, developers face the following friction:
1. **Centralized Host Bottlenecks**: Adding a game in C++ forces developers to modify centralized enum definitions and factory switch statements, preventing games from being truly self-contained modules.
2. **Leaky Client Presentation**: The canvas and HUD components in the web shell contain hardcoded conditionals, manual button injections, and game-specific branches for games like Dominoes, Battleship, Snake, Pong, and Tron.
3. **Manual Client Registration & Catalogue Duplication**: Game metadata (titles, categories, player limits, and control schemes) is duplicated manually in the client store, and every client renderer must be manually imported into a static registry dictionary.
4. **Lack of Dynamic Web Templating**: The client shell has no generic mechanism to render interactive game controls, choice modals, or HUD elements based on authoritative game state, forcing each game's interaction requirements to be hardcoded into the core client shell.

## Solution

A fully decoupled, schema-driven game engine architecture where the Host and ClientShell act as generic runtimes, and games act as completely pluggable, self-registering modules:

1. **Host-Side Decentralized Game Registry**:
   - The centralized game type enumeration and factory switch are replaced with a static registration mechanism keyed by string or fixed-capacity string identifiers.
   - Core host subsystems (engine loop, session management, room management) interact strictly with an abstract game interface and never reference concrete game implementations.
   - Identifiers use fixed-capacity storage or string views to maintain the zero-heap allocation guarantee across the host simulation loop.

2. **Host-Advertised Input and Metadata Schemas**:
   - Each game module declares its metadata (title, category, player limits) and required input capabilities (such as virtual directional pads, grid pointers, or drag sliders) upon registration.
   - The host advertises this catalogue directly to connecting clients over the network transport, allowing the client lobby and input controls to configure themselves dynamically without hardcoded client catalogue files.

3. **Authoritative Action Stream and Dynamic Action Toolbar**:
   - Game state snapshots broadcast by the host include an authoritative collection of currently legal discrete player actions and optional choice prompts.
   - The client shell renders these legal actions through a generic Action Toolbar and Action Modal system, dispatching standardized commands back to the host.
   - Hardcoded game-specific buttons and modals are removed from the canvas component.

4. **Build-Time Client Plugin Auto-Discovery**:
   - The client build pipeline automatically discovers all client game modules matching a standard entrypoint pattern using build-time globbing.
   - Adding a client renderer requires no edits to central client registry files.

5. **Pure Behavioral Seams and Zero-Leak Presentation**:
   - The canvas presentation layer is responsible solely for rendering visual state and mapping spatial pointer interactions, while the generic action system handles discrete turn and lifecycle decisions.

## User Stories

1. As a game developer, I want to register a new game on the Host using a self-registration macro, so that I can add a game without modifying central engine files or factory classes.
2. As a game developer, I want my game to declare its input requirements (such as D-Pad, touch pointer, or drag slider) in its registration descriptor, so that the client shell automatically activates the correct input peripherals.
3. As a game developer, I want my game to broadcast available actions in its state snapshot, so that the client shell automatically renders action buttons without needing custom HTML or JSX.
4. As a game developer, I want to request branching player choices (such as Domino placement ends or pawn promotions) via an authoritative prompt schema, so that the client shell displays a choice modal without game-specific UI code.
5. As an embedded firmware developer, I want room identifiers and game identifiers to use fixed-capacity memory without heap allocations, so that the ESP32-S3 FreeRTOS heap is protected from fragmentation.
6. As an embedded firmware developer, I want core room and engine loops to interact with games purely through polymorphic interfaces, so that compiler dead-code elimination and linker safety are maintained.
7. As a frontend developer, I want the client lobby to populate games dynamically from the Host's directory broadcast, so that adding a game to the Host automatically exposes it in the web UI.
8. As a frontend developer, I want client game renderers to be automatically discovered by the build system via directory globbing, so that I do not have to maintain a static registry dictionary.
9. As a frontend developer, I want the canvas component to have zero game-specific conditional branches, so that presentation code remains clean and maintainable.
10. As a frontend developer, I want the game HUD to display scores and player names from normalized state fields, so that custom scoring logic does not leak into global header components.
11. As a player on a smartphone, I want interactive buttons (like Draw, Pass, or Deploy Fleet) to appear and enable only when they are legal moves, so that I never execute invalid actions.
12. As a player on a smartphone, I want directional controls to appear only for games that require directional navigation, so that turn-based board games retain maximum screen space.
13. As a player in a multiplayer match, I want game outcome modals and rematch options to follow a unified interface, so that every game feels cohesive and responsive.
14. As a test author, I want to verify game registration and room creation through the top-level Host engine network adapter seam, so that tests validate external behavior rather than internal registry storage.
15. As a test author, I want to verify dynamic action toolbar rendering by feeding mock state snapshots to the client action component, so that action dispatch behavior is verified independently of network latency.

## Implementation Decisions

### 1. Host Game Registry and Identifiers
- Replace the central game type enum with a canonical string identifier (`GameId`).
- Internally, `GameId` will be represented as a fixed-capacity character buffer (24 bytes) in room states and a string view during registration, avoiding dynamic heap allocations.
- Implement a decentralized registry where each game module registers a factory function and a game descriptor struct at static initialization time.
- The descriptor struct includes human-readable title, category, minimum/maximum player counts, and an input capability flag set (`InputTemplate`).

### 2. Network Protocol and Catalog Advertisement
- The initial handshake and room directory payloads will include the registered game descriptors and their declared `InputTemplate`.
- State broadcast frames will support two standardized extension arrays:
  - `available_actions`: An array of objects specifying `{ id: string, label: string, enabled: boolean, style?: string }`.
  - `prompt`: An optional object specifying `{ title: string, choices: [{ cmd: string, label: string, params?: object }] }` for transient decision gates.
- Client action dispatch will use a standardized command schema: `{ cmd: string, ...params }`.

### 3. Client Shell and Generic Action Toolbar
- Implement a generic `ActionToolbar` component that subscribes to the active state's `available_actions` and renders retro-arcade buttons.
- Implement an `ActionModal` overlay component that renders when an authoritative `prompt` is present in the state snapshot.
- Refactor `GameCanvas` to remove all game-specific branches (including Domino placement modals, Domino draw/pass buttons, Battleship fleet deployment buttons, and hardcoded D-Pad checks).
- Drive the activation of the virtual directional pad purely from the active game's declared `InputTemplate`.
- Refactor `GameHud` to read player scores and identifiers from standardized state arrays rather than game-specific field fallbacks.

### 4. Build-Time Discovery and Asset Compression
- Use Vite's `import.meta.glob` to scan `@games/*/client/index.ts`, collecting all client game modules into a runtime renderer lookup table at build time.
- Maintain the single-file inlined HTML bundling strategy via `asset_packer` and RFC 1952 Gzip compression embedded into C++ flash headers.

## Testing Decisions

### Good Test Principles
- Tests must interact exclusively through public behavioral seams (sending messages into the HostEngine or simulating state snapshots in the client components) rather than asserting against private registry data structures or internal DOM class names.
- Tests must verify that adding or registering a mock game produces expected lobby listings, valid state snapshots, and correct command responses.

### Modules Tested
1. **HostEngine and RoomManager (Host Integration Seam)**:
   - Registering a test game module and verifying room creation via room codes.
   - Verifying that directory broadcasts include the test game's declared metadata and `InputTemplate`.
   - Ingesting generic command packets and verifying state transitions.
2. **ActionToolbar and ActionModal (Client Presentation Seam)**:
   - Rendering buttons from an `available_actions` state stream.
   - Disconnecting/disabling buttons when `enabled` is false.
   - Rendering modal prompt dialogs when `prompt` is populated and asserting command dispatch upon button press.
3. **GameCanvas and Client Registry (Client Shell Seam)**:
   - Verifying that renderer selection resolves correctly via glob discovery.
   - Verifying that `GameCanvas` mounts without errors for all registered game types without game-specific branching.

### Prior Art
- `tests/test_host_engine.cpp`: Prior art for testing host lifecycle and network message dispatch via `MockNetworkAdapter`.
- `tests/test_room_manager.cpp`: Prior art for testing room creation, joining, and lifecycle transitions.
- `frontend/src/__tests__/renderers.test.ts`: Prior art for testing client renderer initialization, resizing, and teardown.

## Out of Scope

- Rewriting existing game simulations into an Entity-Component-System (ECS) architecture.
- Dynamic runtime loading of compiled shared libraries (`.so` / `.wasm`) on the ESP32-S3 hardware.
- Multi-file chunked asset streaming over Wi-Fi (retaining the single-file inlined Gzip payload).
- Implementing new games beyond standardizing the existing seven games (Snake, Tic-Tac-Toe, Connect Four, Pong, Tron, Battleship, Dominoes).

## Further Notes

- This specification fulfills the architectural decisions recorded in ADR 0019 (`docs/adr/0019-decoupled-game-engine-and-schema-driven-web-templating.md`).
- All terminology conforms to the project domain model in `CONTEXT.md`.
