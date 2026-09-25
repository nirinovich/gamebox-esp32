Status: resolved

# ESP32 Game Hub Specification

## Problem Statement

Students and developers learning object-oriented C++ often build simple command-line or desktop programs that feel disconnected from real-world distributed or embedded systems. Conversely, embedded IoT projects often neglect rich object-oriented software engineering principles in favor of procedural C code. Users want a compelling, self-contained project that demonstrates rigorous C++ OOP architecture (polymorphism, design patterns, RAII, concurrency) running on embedded hardware, while offering a frictionless experience where anyone with a smartphone can join and play solo or multiplayer games without installing an application.

## Solution

An authoritative, headless game engine implemented in modern C++17 running on an ESP32-S3 microcontroller configured as a Wi-Fi Access Point (`ESP32-GameHub`). When a Client connects to the Wi-Fi network, a captive portal automatically presents a web-based game panel served directly from Host flash memory. The browser serves strictly as a thin display surface rendering game state and forwarding touch/keyboard inputs over WebSockets. The Host executes all domain logic, concurrency management, and game simulation across two isolated FreeRTOS cores, offering both turn-based and real-time games (Tic-Tac-Toe and Snake) in solo and multiplayer modes.

## User Stories

1. As a player, I want to connect to the "ESP32-GameHub" Wi-Fi network, so that I can access the game panel instantly without installing any app.
2. As a player, I want the game portal to pop up automatically upon connecting to the Wi-Fi network, so that I do not need to manually discover or type an IP address.
3. As a player, I want to view a clear main menu upon opening the portal, so that I can choose between Solo play and Multiplayer play.
4. As a solo player, I want to select Snake from the Solo menu, so that I can start an immediate single-player game on a private Room.
5. As a solo player playing Snake, I want to control my snake using on-screen virtual directional buttons or keyboard arrow keys, so that I can guide the snake toward food.
6. As a solo player playing Snake, I want my score to increment and the snake body to grow whenever I consume an apple, so that I can track my survival progress.
7. As a solo player playing Snake, I want the game to terminate when I collide with a wall or my own body, so that I receive immediate feedback on my performance.
8. As a solo player, I want to select Tic-Tac-Toe from the Solo menu, so that I can play against an algorithmic bot opponent.
9. As a solo player playing Tic-Tac-Toe, I want to tap an empty cell on the 3x3 board, so that my marker is placed on the board.
10. As a solo player playing Tic-Tac-Toe, I want the Host's bot opponent to respond with optimal moves, so that I face an engaging challenge.
11. As a solo player playing Tic-Tac-Toe, I want the game to detect three-in-a-row or a draw, so that the round concludes with an unambiguous winner declaration.
12. As a multiplayer host, I want to create a new multiplayer Room and choose a game mode, so that I receive a unique 4-character RoomCode to share with an opponent.
13. As a multiplayer opponent, I want to enter a 4-character RoomCode on my device, so that I can join the existing Room hosted by my peer.
14. As a multiplayer player, I want the Room to display a "Waiting for opponent" status until the second player connects, so that I know the game is pending readiness.
15. As a multiplayer player, I want the game to begin automatically once both players are connected to the Room, so that gameplay starts without manual coordination.
16. As a multiplayer player in 2-player Snake, I want both snakes to share the same arena grid, so that we can compete for food and attempt to trap each other.
17. As a multiplayer player in 2-player Snake, I want collision with the boundary, my own body, or the opponent's body to result in a loss, so that the last surviving snake is declared the winner.
18. As a multiplayer player in 2-player Tic-Tac-Toe, I want turns to alternate strictly between Player 1 (X) and Player 2 (O), so that neither player can act out of sequence.
19. As a multiplayer player, I want both Clients to receive identical, synchronized game state updates in real time, so that the game state remains authoritative and consistent.
20. As a player, I want responsive touch D-pad controls on mobile screens and keyboard WASD/arrow key support on desktops, so that control input is comfortable on any client form factor.
21. As a player, I want to return to the main menu after a game finishes, so that I can start another match or change games.
22. As a player, I want the Room to cleanly terminate if my opponent disconnects, so that the match does not stall indefinitely.
23. As an evaluator or developer, I want all game logic, room management, and state machines to be implemented in C++ OOP on the Host, so that the academic requirements of object-oriented programming are thoroughly satisfied.
24. As an evaluator or developer, I want to compile and run the engine locally as a desktop executable, so that I can test solo and multiplayer matches in local browser tabs without needing physical ESP32 hardware.

## Implementation Decisions

- **Host Platform & Concurrency (ADR-0002, ADR-0006)**:
  - ESP32-S3 dual-core microcontroller running FreeRTOS.
  - Core 0 runs Wi-Fi SoftAP, DNS server (captive portal on UDP 53), HTTP web server, and asynchronous WebSocket server (`ESPAsyncWebServer` / `AsyncTCP`).
  - Core 1 runs a dedicated `GameEngineTask` executing a fixed 20 Hz tick loop for real-time simulation.
  - Core 0 and Core 1 exchange player inputs and state broadcasts via thread-safe queues protected by modern C++ RAII mutex wrappers.

- **Dual-Target Desktop Runner & Wokwi Simulation (ADR-0007)**:
  - Clean architecture separation: all core domain logic (`Game`, `Room`, `RoomManager`, `PlayerController`) resides in a portable core library with zero microcontroller dependencies.
  - Native Desktop Target: compiles as a standard desktop application serving `http://localhost:8080` via a lightweight C++ HTTP/WebSocket server, enabling complete local development, debugging, and grading in browser tabs without hardware.
  - ESP32 Firmware Target: compiles for ESP32-S3 using PlatformIO, executable on physical hardware or virtually through the Wokwi simulator.

- **Polymorphic Game Architecture (ADR-0005)**:
  - Abstract base class `Game` providing pure virtual lifecycle interfaces: `init()`, `update(float dt)`, `handleInput(PlayerId id, const InputCommand& cmd)`, `serializeState(JsonDocument& doc) const`, `isFinished()`, and `getWinner()`.
  - Concrete derived classes `SnakeGame` (real-time tick loop with grid coordinate tracking) and `TicTacToeGame` (turn-based discrete state transitions).
  - `GameFactory` encapsulating dynamic polymorphic game instantiation based on selected game mode.

- **Player Controller Strategy Pattern (ADR-0005)**:
  - Abstract strategy interface `PlayerController` defining `nextAction()`.
  - `HumanController` consumes input commands routed from a client Session.
  - `BotController` executes automated heuristic/Minimax move calculation directly on Core 1 of the Host without external network dependency.

- **Room and Session Lifecycle**:
  - `RoomManager` owns and manages up to 4 concurrent `Room` instances using `std::unique_ptr<Room>`.
  - Each `Room` manages its own `Game` instance, player session mappings, and unique 4-character `RoomCode`.
  - Disconnect handling: if a Session disconnects during an active game, the remaining player is awarded a forfeit victory, and the Room is immediately cleaned up to prevent memory leaks in Host RAM.

- **Client Surface & Asset Delivery (ADR-0001, ADR-0004)**:
  - Single-page application (HTML5 Canvas + responsive CSS + minimal JavaScript WebSocket driver) compiled into C++ flash memory (`PROGMEM`) as a raw string literal.
  - Client acts solely as a dumb terminal: streams raw user input events upstream, renders received state dictionaries downstream.
  - Responsive canvas layout with virtual on-screen D-pad buttons for mobile touch devices and keyboard event listeners for desktop browsers.

- **State Replication Wire Protocol (ADR-0003)**:
  - State replication serialized as JSON text payloads over WebSockets via `ArduinoJson`.
  - Inputs sent from Client to Host as compact JSON command objects (`{"cmd":"dir","val":"UP"}` or `{"cmd":"place","x":1,"y":2}`).

## Testing Decisions

- **Testing Philosophy**:
  - Tests verify external observable behavior (input processing, game state changes, win/loss evaluation, JSON output) rather than internal implementation details.
  - All game simulation logic and domain models must be decoupled from physical microcontroller hardware and FreeRTOS APIs so they can compile and execute in a standard desktop C++ native test runner (via PlatformIO native testing or standard CMake/CTest).

- **Primary Testing Seam**:
  - The highest seam is the **`Room` Message Boundary**:
    `Room::handleInput(PlayerId, JsonCommand)` $\rightarrow$ `Room::tick(dt)` $\rightarrow$ `Room::serializeState(JsonDocument&)`.
  - This seam tests:
    1. Input command validation and turn ordering.
    2. Polymorphic `Game` state transitions across both `SnakeGame` and `TicTacToeGame`.
    3. `PlayerController` strategy execution (verifying `BotController` generates valid winning/blocking moves).
    4. Win, loss, draw, and collision conditions.
    5. Correct JSON serialization of game state for client rendering.

- **Secondary Testing Seam**:
  - **`RoomManager` Lifecycle**:
    Creating rooms, joining with valid `RoomCode`, rejecting invalid codes, enforcing the 4-room capacity limit, and cleaning up rooms on player disconnect.

## Out of Scope

- Internet connectivity, remote cloud multiplayer, or external MQTT brokers (system is strictly a local Wi-Fi offline appliance).
- Persistent user accounts, authentication, or cross-session global leaderboards (all sessions and scores reset when rooms close).
- Complex 3D graphics or high-bandwidth audio streaming over WebSockets.
- File system partitions (SPIFFS / LittleFS); all assets are flash-embedded.
- Bluetooth connectivity or physical on-board screen/button peripheral controls.

## Further Notes

- The project adheres to domain terms defined in `CONTEXT.md` (`Host`, `Client`, `Session`, `Player`, `Room`, `Game`, `Tick`, `RoomCode`, `PlayerController`, `HumanController`, `BotController`).
- All architectural decisions are backed by ADRs under `docs/adr/` (ADR-0001 through ADR-0007).
