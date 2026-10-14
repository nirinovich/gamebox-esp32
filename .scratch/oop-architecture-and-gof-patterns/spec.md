Status: resolved

# OOP Architectural Reconciliation and GoF Patterns

## Problem Statement

The project is an academic capstone for an Object-Oriented Programming (OOP) course. However, previous premature optimization decisions (notably ADR-0018) prohibited deep OOP class hierarchies and dynamic polymorphism in favor of C-style Plain Old Data (POD) structs and procedural reducers under the assumption of microcontroller memory and cache constraints. 

This resulted in severe architectural anti-patterns that directly contradict the grading criteria of an advanced OOP curriculum:
1. **God Classes**: Game implementations (`DominoGame`, `LudoGame`, `KatroGame`) ballooned into monolithic classes (600 to 1,000+ lines) concurrently managing rules, state layout, bot AI heuristics, turn timers, input string scanning, and manual JSON string stitching in violation of the Single Responsibility Principle (SRP).
2. **Abandoned Strategy Pattern**: Although `PlayerController` was defined in the domain glossary (`CONTEXT.md`), games regressed to procedural `bool isBot` type-tags and monolithic `if-else` branching for move execution.
3. **Procedural Phase Switches**: Multi-phase game progression (e.g. Ludo turn sequences, Domino round lifecycles) was driven by raw `enum class` states and switch-case statements rather than behavioral state modeling.
4. **Ad-hoc Input Parsing**: Player actions were received as raw strings and parsed via ad-hoc substring searches (`input.find(...)`) rather than encapsulated command objects.
5. **Shallow Polymorphism**: Polymorphism across the host engine was restricted to a single abstract base class (`Game`), after which all execution collapsed into procedural routines.

The project requires an architectural overhaul to restore textbook Object-Oriented Design, encapsulate domain concepts into dedicated classes, and implement canonical Gang of Four (GoF) design patterns while maintaining compatibility with the existing headless web client.

## Solution

Formally supersede ADR-0018 via ADR-0022 to re-adopt modern C++17 dynamic polymorphism (`virtual` dispatch, abstract interfaces, and `std::unique_ptr` ownership). Archive Texas Hold'em Poker to eliminate non-academic combinatorial card complexity and focus on three distinct, rich domain models: **Malagasy Tsipy Dominoes**, **Ludo**, and **Malagasy Katro**.

The solution decomposes monolithic game engines into cohesive domain entities obeying the Single Responsibility Principle and introduces three canonical GoF behavioral patterns:
1. **GoF Strategy Pattern (`PlayerController`)**: Eliminate `bool isBot` across all player entities. Each `Player` encapsulates a polymorphic `PlayerController` strategy (`HumanController` consuming network action queues vs. `BotController` executing heuristic search).
2. **GoF State Pattern (`GamePhaseState`)**: Encapsulate discrete game phases and turn lifecycles in dedicated state classes implementing `onEnter()`, `handleCommand()`, `update(dt)`, and `onExit()`. The active state object has authority over command validity and state transitions.
3. **GoF Command Pattern (`ICommand` and `CommandFactory`)**: Deserialize incoming client actions into concrete `ICommand` subclasses via a `CommandFactory`. Commands execute polymorphically via double dispatch against the sovereign active `GamePhaseState`.
4. **Composite State Projection**: Domain entities encapsulate their own state representations, which are aggregated cleanly into the broadcast snapshot.

## User Stories

### Core Architecture & Academic Evaluation
1. As an academic evaluator, I want the codebase to demonstrate rich object decomposition and the Single Responsibility Principle, so that classes have clear boundaries and cohesive responsibilities.
2. As an academic evaluator, I want to see canonical Gang of Four (GoF) design patterns (Strategy, State, Command, Factory) correctly implemented, so that the project satisfies university criteria for advanced OOP design.
3. As an academic evaluator, I want the engine to use dynamic polymorphism and Liskov Substitution Principle (LSP) rather than boolean type flags (`isBot`) and procedural `switch` statements, so that behavioral extensions are open for extension but closed for modification (OCP).
4. As an academic evaluator, I want memory ownership to be strictly managed through modern C++17 RAII idioms (`std::unique_ptr`), so that dynamic polymorphism introduces zero memory leaks.

### Player & Controller Modeling (Strategy Pattern)
5. As a game engine, I want every `Player` entity to encapsulate a `PlayerController` strategy interface, so that I can query for actions without knowing whether a participant is human or AI.
6. As a human player connected via WebSocket, I want my actions to be queued into a `HumanController`, so that the game loop consumes my intent seamlessly through the strategy interface.
7. As a solo player, I want my opponent to be controlled by an autonomous `BotController`, so that heuristic or search calculations execute behind the polymorphic interface without special-casing the game loop.
8. As a developer, I want to eliminate all `bool isBot` flags and associated procedural branching, so that player behavior is entirely governed by strategy polymorphism.
9. As a developer, I want to be able to substitute different bot difficulty strategies (e.g. `RandomBotStrategy` vs. `HeuristicBotStrategy`) at runtime, so that AI capabilities can be swapped without modifying game logic.

### Game Lifecycle & Turn Progression (State Pattern)
10. As a game orchestrator, I want distinct game and turn phases to be modeled as polymorphic `GamePhaseState` objects, so that phase-specific logic is isolated from the main game container.
11. As a game orchestrator, I want the active `GamePhaseState` to control phase transitions, so that illegal state transitions are prevented by construction.
12. As a player in Malagasy Dominoes, I want round progression (dealing, active play, blocked outcome evaluation, round scoring, rematch) to be managed by dedicated phase states, so that scoring rules and tie rollovers (`Rebon`) are encapsulated cleanly.
13. As a player in Ludo, I want turn sequences (waiting for roll, selecting movable pawn, executing extra roll on six, passing turn) to be governed by dedicated turn states, so that players cannot act out of phase.
14. As a player in Katro, I want continuous sowing rounds, capture laps, and turn handoffs to be managed by dedicated phase states, so that complex Malagasy mancala rules are encapsulated.
15. As a developer, I want to add new phase transitions without editing a monolithic 500-line `update()` or `handleInput()` method, so that state machines remain maintainable and extensible.

### Input Ingestion & Execution (Command Pattern)
16. As a game host, I want incoming client messages to be parsed by a `CommandFactory` into concrete `ICommand` instances, so that wire-format JSON parsing is decoupled from game state execution.
17. As a game host, I want the active `GamePhaseState` to validate incoming `ICommand` instances via double dispatch, so that actions incompatible with the current phase are rejected polymorphically.
18. As a developer, I want unit tests to construct and execute `ICommand` objects directly in C++, so that game logic can be verified without synthesizing JSON strings.
19. As a developer, I want all input parsing to avoid raw string substring searching (`input.find(...)`), so that input decoding is robust and type-safe.
20. As a player in Dominoes, I want my move intent to be encapsulated as a `PlayTileCommand` or `PassTurnCommand`, so that tile index and placement side are validated by the domain model.
21. As a player in Ludo, I want my move intent to be encapsulated as a `RollDiceCommand` or `MovePawnCommand`, so that pawn identification and roll validity are cleanly validated.
22. As a player in Katro, I want my move intent to be encapsulated as a `SowPitCommand`, so that pit index and sowing direction are validated against board invariants.

### Domain Entity Decomposition (Single Responsibility)
23. As a Domino game model, I want a `DominoBoard` class that owns the open chain of placed tiles, so that open end values and placement connectivity are encapsulated away from scorekeeping.
24. As a Domino game model, I want a `DominoHand` class that encapsulates a player's private tiles, so that pip summation and matching queries are private to the hand.
25. As a Domino game model, I want a `DominoTileSet` class that manages tile distribution and undealt `Deadwood`, so that tile dealing is isolated.
26. As a Ludo game model, I want a `Dice` class that encapsulates the random number generation and current rolled face, so that dice mechanics are separated from board traversal.
27. As a Ludo game model, I want a `Pawn` class that tracks individual token state (yard, active track, safe square, finished) and calculate track distances, so that token movement rules are encapsulated.
28. As a Ludo game model, I want a `LudoBoard` class that maps track squares, safe star cells, and player home stretches, so that spatial navigation and collision detection are encapsulated.
29. As a Katro game model, I want a `KatroBoard` class that manages the 4-row layout and validates pit accessibility, so that row ownership rules are encapsulated.
30. As a Katro game model, I want a `KatroPit` class that tracks individual pit seed counts and row types (inner vs outer), so that seed transfers obey pit invariants.

### Client Shell Compatibility & Wire Protocol
31. As a web client, I want to receive identical JSON state snapshots from the refactored C++ engine, so that existing Canvas 2D renderers and UI components continue functioning without frontend modifications.
32. As a game host, I want each domain entity to project its own state representation via composite serialization, so that the global snapshot is assembled cleanly without monolithic string stitching.
33. As an operator of the ESP32-S3 host, I want the refactored engine to execute within the memory budget of the microcontroller, so that dynamic polymorphism does not cause heap exhaustion.

## Implementation Decisions

### 1. Formally Supersede ADR-0018 via ADR-0022
- Reject the prohibition of OOP class hierarchies and dynamic polymorphism on the ESP32-S3.
- Allow standard C++17 virtual interfaces, abstract base classes, and `std::unique_ptr` dynamic allocations bounded to `Room` and `Game` lifetimes.
- Maintain zero dynamic allocation only within high-frequency network tick callbacks where necessary, but allow object-oriented allocations during room initialization, turn phase transitions, and command dispatch.

### 2. Catalog Focus: Archive Poker
- Formally archive `TexasHoldemGame` (`games/poker/`).
- Focus all OOP architectural refactoring on the triad of parlor and board games:
  - **Malagasy Dominoes (Tsipy Domino)**
  - **Ludo**
  - **Malagasy Katro**

### 3. GoF Strategy Pattern: PlayerController Hierarchy
- Remove all `bool isBot` struct fields and methods like `executeBotMove()`.
- Define an abstract interface `PlayerController`:
  - `virtual std::unique_ptr<ICommand> decideAction(const Game& game, PlayerId id) = 0;`
  - `virtual bool isAutonomous() const = 0;`
- Concrete implementations:
  - `HumanController`: dequeues commands forwarded from the network session; returns `nullptr` when no input is pending.
  - `BotController`: encapsulates game-specific heuristics/search algorithms and returns computed `ICommand` instances autonomously.
- `Player` entity encapsulates `std::unique_ptr<PlayerController>`.

### 4. GoF State Pattern: GamePhaseState Hierarchy
- Define an abstract interface `IGamePhaseState`:
  - `virtual void onEnter(Game& game) {}`
  - `virtual bool handleCommand(Game& game, PlayerId id, const ICommand& cmd) = 0;`
  - `virtual void update(Game& game, float dt) {}`
  - `virtual void onExit(Game& game) {}`
  - `virtual std::string getPhaseName() const = 0;`
- For each game, implement dedicated state classes:
  - **Domino**: `DominoRoundState`, `DominoPendingChoiceState`, `DominoRoundOverState`, `DominoGameOverState`.
  - **Ludo**: `LudoWaitingRollState`, `LudoSelectPawnState`, `LudoPawnMovingState`, `LudoGameOverState`.
  - **Katro**: `KatroTurnState`, `KatroSowingAnimationState`, `KatroGameOverState`.
- Game contexts maintain a `std::unique_ptr<IGamePhaseState>` and transition via a `transitionTo(std::unique_ptr<IGamePhaseState> nextState)` method.

### 5. GoF Command Pattern & Double Dispatch
- Define an abstract interface `ICommand`:
  - `virtual ~ICommand() = default;`
  - `virtual bool execute(Game& game, PlayerId id) const = 0;`
  - `virtual std::string getCommandType() const = 0;`
- Execution model uses double dispatch: `ICommand::execute(game, id)` calls `game.getActiveState()->handle(game, id, *this)`.
- Introduce `CommandFactory` for each game (or a centralized factory with game-specific builders):
  - Ingests raw JSON command strings and produces `std::unique_ptr<ICommand>`.
  - Replaces all ad-hoc `input.find(...)` occurrences.

### 6. Domain Entity Decomposition
- **Domino**:
  - `DominoTile`: Value object with left/right pips, flip capability, and match validation.
  - `DominoHand`: Manages player's tile collection, pip summation, and matching checks.
  - `DominoBoard`: Manages the open chain deque, valid open ends, and tile placement.
  - `DominoTileSet`: Manages the double-six pool, shuffling, dealing, and `Deadwood`.
- **Ludo**:
  - `Dice`: Encapsulates pseudorandom rolling, roll lock, and current face value.
  - `Pawn`: Encapsulates player index, token index, status (yard, track, safe, home stretch, finished), and distance traveled.
  - `LudoBoard`: Encapsulates the 52-tile main circuit, safe star squares, entry/spawn offsets, and home column stretches.
- **Katro**:
  - `KatroPit`: Encapsulates pit index, seed count, row assignment (inner vs. outer), and player territory.
  - `KatroBoard`: Encapsulates the 4xN grid, sowing step math, capture checks, and board balance evaluation.

### 7. Composite State Projection
- Domain entities provide structured projection methods returning serializable state fragments.
- Game context aggregates these projections to serialize the canonical JSON format required by `ClientShell`.

## Testing Decisions

### Testing Philosophy
- Tests verify external observable behavior (command execution, legal move validation, state transitions, win conditions, and JSON snapshot correctness) rather than private implementation details.
- Unit tests for domain entities and behavioral patterns compile and run on the host desktop test runner via CTest without microcontroller dependencies.

### Primary Testing Seam: The Game Command & State Boundary
The highest testing seam is the **Command & State Boundary**:
`CommandFactory::create(json)` $\rightarrow$ `ICommand::execute(game, playerId)` $\rightarrow$ `Game::update(dt)` $\rightarrow$ `Game::serializeStateForPlayer(playerId)`.

This seam tests:
1. `CommandFactory` deserialization and command instantiation.
2. `IGamePhaseState` input validation, transition triggers, and rejection of out-of-turn/out-of-phase actions.
3. Domain entity state mutations (`DominoBoard`, `LudoBoard`, `KatroBoard`, `Pawn`, `Dice`).
4. `PlayerController` Strategy dispatch (verifying `BotController` produces legal commands that transition states).
5. State serialization output fidelity against existing client schema expectations.

### Secondary Testing Seam: Isolated Domain Entity Unit Tests
- Direct unit tests for:
  - `DominoBoard`: verifying tile connectivity, flipping, and double placement.
  - `LudoBoard` & `Pawn`: verifying roll-out on 6, track wrap-around, safe square collision immunity, and home stretch entry.
  - `KatroBoard`: verifying continuous sowing, lap continuation, and inner-row capture rules.
  - `CommandFactory`: verifying correct parsing of valid actions and graceful rejection of malformed JSON.

### Prior Art
- Existing unit tests in `games/domino/host/test_domino.cpp`, `games/ludo/host/test_ludo.cpp`, and `games/katro/host/test_katro.cpp`.
- Host engine integration tests in `tests/test_host_engine.cpp` and `tests/test_room_manager.cpp`.

## Out of Scope

- Refactoring `TexasHoldemGame` (permanently archived per ADR-0022).
- Modifying the WebSocket protocol wire format or JSON schema (the web client must remain untouched).
- Modifying frontend Preact/TypeScript components or Canvas 2D renderers.
- Re-architecting desktop server networking or ESP32 FreeRTOS tasks (these remain stable infrastructure).

## Further Notes

- All changes adhere to the domain vocabulary defined in `CONTEXT.md` (`Host`, `Client`, `Session`, `Player`, `Room`, `Game`, `PlayerController`, `HumanController`, `BotController`, `GamePhaseState`, `Command`, `CommandFactory`).
- All architectural decisions are backed by ADR-0022 (`docs/adr/0022-oop-architectural-reconciliation-and-gof-patterns.md`).
