# 01: Fondations et interfaces POO du noyau (ICommand, IGamePhaseState, PlayerController)

**What to build:**
Define and establish the foundational abstract GoF behavioral interfaces and base hierarchies in the engine core (`include/gamehub/core/`):
- `ICommand` and `CommandFactory` base abstraction
- `IGamePhaseState` interface with lifecycle hooks (`onEnter`, `handleCommand`, `update`, `onExit`)
- Polymorphic `PlayerController` hierarchy (`HumanController` and `BotController`), fully resolving the domain terms in `CONTEXT.md`
- `Player` entity encapsulating `std::unique_ptr<PlayerController>`
Verify with an isolated core unit test validating command execution, double-dispatch, and strategy behavior.

**Blocked by:** None (can start immediately)

**Status:** resolved

- [x] Abstract `ICommand` and `CommandFactory` base interfaces defined
- [x] Abstract `IGamePhaseState` interface with lifecycle methods defined
- [x] `PlayerController`, `HumanController`, and `BotController` properly defined with dynamic polymorphism
- [x] `Player` entity encapsulates `std::unique_ptr<PlayerController>` without `bool isBot`
- [x] Unit test `test_core_oop_patterns.cpp` added to CMake and passing via CTest
