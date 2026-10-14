# 05: Refonte POO de Malagasy Katro (Entités métier, États, Commandes, Stratégie)

**What to build:**
Refactor `KatroGame` into clean, cohesive OOP domain components and GoF design patterns:
- Extract domain entities: `KatroPit` (inner vs outer row, seed count, ownership), `KatroBoard` (4-row board representation, sowing mechanics, capture evaluation).
- Implement the GoF State Pattern (`KatroTurnState`, `KatroGameOverState`).
- Implement the GoF Command Pattern (`SowPitCommand`) via `KatroCommandFactory`.
- Implement the GoF Strategy Pattern: replace `bool isBot` with `PlayerController` (`HumanController` vs `KatroBotController` with capture gain heuristic).
- Validate that all game rules (continuous multi-lap sowing, captures from inner/outer rows, game over detection) pass existing unit tests.

**Blocked by:** 01 (Fondations et interfaces POO du noyau)

**Status:** resolved

- [x] Domain entities `KatroBoard` and `KatroPit` created and encapsulated
- [x] GoF State Pattern implemented for turn lifecycles
- [x] GoF Command Pattern implemented with `KatroCommandFactory`
- [x] `bool isBot` eliminated in favor of `PlayerController` Strategy
- [x] Unit tests in `games/katro/host/test_katro.cpp` updated and passing
