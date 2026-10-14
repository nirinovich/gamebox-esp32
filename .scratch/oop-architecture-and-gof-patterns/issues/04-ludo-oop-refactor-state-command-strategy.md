# 04: Refonte POO de Ludo (Entités métier, États, Commandes, Stratégie)

**What to build:**
Refactor `LudoGame` into clean, cohesive OOP domain components and GoF design patterns:
- Extract domain entities: `Dice` (random generation and current rolled value), `Pawn` (yard, track, safe, home status, movement), `LudoBoard` (track path, entry points, star safe squares).
- Implement the GoF State Pattern (`LudoWaitingRollState`, `LudoSelectPawnState`, `LudoGameOverState`).
- Implement the GoF Command Pattern (`RollDiceCommand`, `MovePawnCommand`) via `LudoCommandFactory`.
- Implement the GoF Strategy Pattern: replace `bool isBot` with `PlayerController` (`HumanController` vs `LudoBotController` with tactical score heuristic).
- Validate that all game rules (yard rollout on 6, extra roll on 6, consecutive six penalty, safe squares, collisions) pass existing unit tests.

**Blocked by:** 01 (Fondations et interfaces POO du noyau)

**Status:** resolved

- [x] Domain entities `Dice`, `Pawn`, and `LudoBoard` created and encapsulated
- [x] GoF State Pattern implemented for turn phases (roll, select, game over)
- [x] GoF Command Pattern implemented with `LudoCommandFactory`
- [x] `bool isBot` eliminated in favor of `PlayerController` Strategy
- [x] Unit tests in `games/ludo/host/test_ludo.cpp` updated and passing
