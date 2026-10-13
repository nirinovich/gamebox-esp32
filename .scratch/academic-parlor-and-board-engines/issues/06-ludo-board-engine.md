# 06: Ludo Authoritative Board Engine & Tactical Bot Controller

**What to build:** An authoritative C++ Ludo game engine for 2 to 4 players conforming to the polymorphic `Game` interface. Supports Quick Match (2 pawns) and Classic Match (4 pawns), authoritative dice rolling, pawn yard exits on 6 with bonus rolls, 3-consecutive-6 forfeiture, track traversal across 52 shared cells and 4 color home stretches, collision captures returning pawns to yard with bonus rolls, star SafeSquare immunity, and a tactical AI bot for solo play.

**Blocked by:** 01: Purge Legacy Tutorial Games & Clean Platform Registry

**Status:** resolved

- [x] `LudoGame.hpp` and `LudoGame.cpp` implement authoritative cross-and-circle board rules under `games/ludo/host/`.
- [x] Board model tracks 52 perimeter track tiles, 4 colored starting cells, 4 colored home stretches (5 cells each), and 4 center home goals.
- [x] Pawns remain in PawnYard until an authoritative 6 is rolled.
- [x] Rolling a 6 grants a bonus roll; rolling three consecutive 6s forfeits the bonus and advances the turn.
- [x] Landing on an opponent pawn on a standard cell returns that pawn to its PawnYard and awards a bonus roll.
- [x] Star-marked SafeSquares and starting cells allow multiple pawns to co-exist without capture.
- [x] Room creator can configure Quick Match (2 pawns) vs Classic Match (4 pawns) per player.
- [x] Tactical `BotController` evaluates pawn choices, prioritizing captures, yard rollouts, safe square entries, and home finishes.
- [x] Unit tests in `games/ludo/host/test_ludo.cpp` verify yard rollouts, safe square immunity, captures, home runs, and win triggers.
