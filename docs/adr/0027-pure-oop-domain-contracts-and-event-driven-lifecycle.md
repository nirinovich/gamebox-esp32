# Pure OOP Domain Contracts, Event-Driven Lifecycle, and Behavioral Sessions

## Context
While ADR-0022 reintroduced GoF design patterns (State, Strategy, Command, Factory), several structural anti-patterns remained in the implementation:
1. **Smell of Runtime Downcasting (`dynamic_cast`)**: The generic `ICommand::execute(Game&, PlayerId)` and `IGamePhaseState::handleCommand(Game&, PlayerId, const ICommand&)` forced domain games to repeatedly perform RTTI downcasts (`dynamic_cast<DominoGame*>(&game)`, `dynamic_cast<const PlayTileCommand*>(&cmd)`). This violated the Liskov Substitution Principle and created runtime type checking overhead.
2. **Procedural Polling ("Ask, Don't Tell")**: `Room::tick` and `Room::handleInput` polled `m_game->isFinished()` every tick cycle to detect game completion rather than reacting to domain events.
3. **God Class & Anemic Sessions in Orchestration**: `HostEngine` maintained three parallel procedural hash maps (`m_sessions`, `m_sessionPlayerMap`, `m_roomPlayers`) to track player numbers and room memberships, while `Session` acted as an anemic data holder with minimal behavior.
4. **Dead Code Relics**: Archived files and deprecated interfaces (`IBotStrategy.hpp`, `IPlayerController.hpp`, stray root directories) lingered in the project tree.

## Decision
1. **Compile-Time Type-Safe Domain Contracts**:
   - Introduce sovereign typed command and state interfaces per game:
     - **Domino**: `IDominoCommand` and `IDominoState`
     - **Ludo**: `ILudoCommand` and `ILudoState`
     - **Katro**: `IKatroCommand` and `IKatroState`
   - Implement type-safe double dispatch (`executeOnDomino`, `executeOnLudo`, `executeOnKatro`) eliminating 100% of runtime `dynamic_cast` downcasts in the domain state machines.
2. **GoF Observer Pattern (`GameEventListener`)**:
   - Introduce `GameEventListener` with `onGameFinished`, `onRoundFinished`, and `onStateInvalidated`.
   - `Room` implements `GameEventListener` and registers itself with `m_game->setEventListener(this)`.
   - Domain games push lifecycle events to their observer immediately upon game or round completion ("Tell, Don't Ask"), completely eliminating polling from `Room::tick`.
3. **Rich Behavioral `Session` & Eliminating Procedural Maps**:
   - Enrich `Session` to encapsulate its connection adapter (`INetworkAdapter*`), active `Room*`, seat index, and message dispatch (`send`, `joinRoom`, `leaveRoom`, `handleMessage`).
   - `Room` provides `getPlayerSeat(PlayerId id)` for direct seat calculation from its player collection.
   - Eliminate `m_sessionPlayerMap` and `m_roomPlayers` from `HostEngine`. All room multicasting delegates directly to `Room::getPlayers()` and `Session::send()`.
4. **Dead Code & Legacy Pruning**:
   - Delete orphaned interfaces `IBotStrategy.hpp` and `IPlayerController.hpp`.
   - Excise Poker dependencies from `CMakeLists.txt` and update `README.md` to accurately represent the active trio: Domino, Ludo, and Katro.

## Consequences
- **Pure OOP / SOLID Compliance**: Full compile-time type safety across commands and states; zero RTTI downcasting smells in domain logic.
- **Single Source of Truth**: Eliminates state desynchronization bugs previously possible between `HostEngine`'s parallel maps and `RoomManager`/`Room`.
- **Event-Driven Room Lifecycle**: `Room` acts as a true observer, handling winner transitions and series score updates reacting to game events.
- **100% Test Integrity**: All unit and integration test suites pass across Debug and Release configurations.
