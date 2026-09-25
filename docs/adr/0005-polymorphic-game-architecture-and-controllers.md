# Polymorphic Game Architecture and Controller Strategy Pattern

We need an extensible, object-oriented design for games and input sources. We decided to structure the engine around an abstract `Game` base class with lifecycle hooks (`init`, `update`, `handleInput`, `serializeState`), a `GameFactory` for dynamic instantiation, and a `PlayerController` Strategy interface with concrete `HumanController` (network stream) and `BotController` (embedded Minimax / AI) implementations. This cleanly decouples game rules from input origin and demonstrates core object-oriented principles.
