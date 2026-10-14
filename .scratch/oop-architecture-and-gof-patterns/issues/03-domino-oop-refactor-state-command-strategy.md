# 03: Refonte POO de Malagasy Dominoes (Entités métier, États, Commandes, Stratégie)

**What to build:**
Refactor `DominoGame` from a monolithic God class into clean, cohesive OOP domain components and GoF design patterns:
- Extract domain entities: `DominoBoard` (open ends, matching, chain deque), `DominoHand` (player tiles, pip calculations, matching queries), `DominoTileSet` (dealing and undealt `Deadwood`).
- Implement the GoF State Pattern (`DominoRoundState`, `DominoPendingChoiceState`, `DominoRoundOverState`, `DominoGameOverState`).
- Implement the GoF Command Pattern (`PlayTileCommand`, `PassTurnCommand`, `SelectTileEndCommand`) ingested via `DominoCommandFactory`.
- Implement the GoF Strategy Pattern: replace `bool isBot` with `PlayerController` (`HumanController` vs `DominoBotController` with pip/blocking heuristic).
- Validate that all game rules (Tsipy Dominoes, Rebon rollover, TargetScore, blocked rounds) pass existing unit tests.

**Blocked by:** 01 (Fondations et interfaces POO du noyau)

**Status:** resolved

- [x] Domain entities `DominoBoard`, `DominoHand`, and `DominoTileSet` created and encapsulated
- [x] GoF State Pattern implemented for round and turn lifecycles
- [x] GoF Command Pattern implemented with `DominoCommandFactory` replacing substring search
- [x] `bool isBot` eliminated in favor of `PlayerController` Strategy
- [x] Unit tests in `games/domino/host/test_domino.cpp` updated and passing
