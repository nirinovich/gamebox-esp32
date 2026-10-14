# Pure OOP Domain Contracts, Event-Driven Lifecycle, and Behavioral Sessions

To restore full academic Object-Oriented Programming (OOP / POO) integrity and eliminate procedural anti-patterns, the architecture transitions to compile-time type-safe domain contracts, the GoF Observer Pattern, and rich behavioral sessions without runtime downcasting (`dynamic_cast`).

## Considered Options
1. **Generic Command Downcasting vs Domain-Specific Contracts**: Generic commands forced `dynamic_cast<TGame*>(&game)` RTTI smells in all states and commands. Chosen: sovereign domain-specific command interfaces per game (e.g., `IDominoCommand`, `ILudoCommand`, `IKatroCommand`).
2. **Procedural Polling vs Observer Pattern**: `Room::tick` polled `m_game->isFinished()` every tick. Chosen: `IGameEventListener` where games notify `Room` of game and round completion ("Tell, Don't Ask").
3. **Anemic Session with 3 Host Maps vs Behavioral Session**: `HostEngine` maintained redundant parallel hash maps. Chosen: `Session` encapsulates its active `Room*`, seat index, and network dispatch.
4. **Catalog Scope**: Texas Hold'em Poker, legacy Tic-Tac-Toe, and Connect Four relics are formally excised from builds and interfaces to focus purely on the active catalog: Malagasy Tsipy Dominoes, Ludo, and Malagasy Katro.
