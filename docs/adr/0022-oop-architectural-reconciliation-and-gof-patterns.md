# OOP Architectural Reconciliation, GoF Design Patterns, and Domain Decomposition

## Context
ADR-0018 prematurely prohibited deep OOP class hierarchies and dynamic polymorphism in favor of C-style POD structs and procedural reducers, citing micro-benchmarks on ESP32-S3 cache and heap. In an academic Object-Oriented Programming (OOP) course, this compromise created severe anti-patterns:
1. **God Classes**: Game implementations (`PokerGame`, `DominoGame`, `LudoGame`, `KatroGame`) swelled to 600–1,000+ lines, managing rules, hardware timers, bot AI, ad-hoc string searching, and JSON string stitching in single classes.
2. **Abandoned Strategy Pattern**: Despite `PlayerController` being defined in `CONTEXT.md`, games regressed to `bool isBot` type-tags and procedural `if-else` execution.
3. **Procedural Phase Switches**: Game progression was driven by raw enums and monolithic switch statements rather than behavioral state modeling.
4. **Ad-hoc Input Parsing**: Inputs were parsed using raw substring queries (`input.find(...)`) rather than encapsulated command objects.

The ESP32-S3 has 512 KB internal SRAM and 8 MB PSRAM. Managing a few dozen polymorphic objects (< 10 KB) via C++17 RAII (`std::unique_ptr`) bounded to `Room` lifecycles poses zero risk of memory leaks or heap exhaustion while restoring full OOP educational integrity.

## Decision
1. **Supersede ADR-0018 Anti-OOP Mandate**: Formally re-adopt C++17 dynamic polymorphism, abstract interfaces, and RAII-managed dynamic allocations (`std::unique_ptr`) for all domain models.
2. **Catalog Focus (Archive Poker)**: Archive Texas Hold'em Poker to eliminate non-academic combinatorial complexity. The active curriculum roster focuses on three distinct, rich domain models: **Malagasy Tsipy Dominoes**, **Ludo**, and **Malagasy Katro**.
3. **GoF State Pattern (`GamePhaseState`)**: Model game phases and turn sequences using polymorphic state classes (`IGamePhaseState`). The active state handles input validation, entry/exit hooks, and phase transitions.
4. **GoF Strategy Pattern (`PlayerController`)**: Eliminate `bool isBot` flags and procedural bot methods. Every `Player` entity encapsulates a `std::unique_ptr<PlayerController>`, with `HumanController` consuming session action queues and `BotController` executing autonomous search heuristics.
5. **GoF Command Pattern (`ICommand` and `CommandFactory`)**: Ingest client actions through a `CommandFactory` yielding concrete `ICommand` instances. Commands execute via double dispatch against the sovereign active `GamePhaseState`.
6. **Domain Decomposition (Single Responsibility)**: Decompose monolithic game classes into dedicated domain entities:
   - **Domino**: `DominoTile`, `DominoHand`, `DominoBoard` (chain connectivity & open ends), `DominoTileSet`.
   - **Ludo**: `Dice`, `Pawn` (yard/track/safe/home status), `LudoBoard` (track navigation & safe squares).
   - **Katro**: `KatroBoard`, `KatroPit` (inner/outer row ownership & seed accounting).
7. **Composite State Projection**: Domain entities provide state-projection methods aggregated cleanly into the broadcast snapshot.

## Consequences
- Directly satisfies academic evaluation criteria for OOP, design patterns (GoF State, Strategy, Command, Factory), and SOLID principles.
- Decouples wire-format serialization and networking from domain entities.
- Drastically simplifies testing: states, commands, and domain entities can be unit-tested in complete isolation without network or JSON simulation.
