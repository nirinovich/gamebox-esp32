# 02: Malagasy Dominoes Authoritative Engine & Cumulative Match Scoring

**What to build:** An authoritative C++ Malagasy Dominoes (*Tsipy Domino*) engine conforming to the polymorphic `Game` interface. Supports 2 to 4 players with 7 tiles each, undealt tiles locked as Deadwood, opening lead determined by highest double, strict turn-passing when blocked (no drawing), round scoring awarding sum of opponents' remaining pips, lowest PipSum resolution on blocked rounds, Rebon jackpot rollover on ties, and cumulative match scoring toward a host-configured TargetScore (60 or 100 points).

**Blocked by:** 01: Purge Legacy Tutorial Games & Clean Platform Registry

**Status:** ready-for-agent

- [ ] `DominoGame.hpp` and `DominoGame.cpp` implement block-style *Tsipy Domino* rules for 2 to 4 players without Boneyard drawing.
- [ ] In 2- and 3-player matches, exactly 7 tiles are dealt per player and undealt tiles remain inaccessible Deadwood. In 4-player matches, all 28 tiles are dealt (7 each).
- [ ] Opening round requires the player holding the highest double tile (e.g. `[6|6]`) to make the first play.
- [ ] Passing is strictly permitted only when a player has no legal plays on open board ends.
- [ ] Round finishes when a player empties their hand, scoring the cumulative pip sum of all opponents' unplayed tiles.
- [ ] When all players pass consecutively (game blocked / *mihiboka*), the player with the lowest PipSum wins the opponents' unplayed pips.
- [ ] Ties on lowest PipSum result in 0 round points awarded and the pip sum rolling into a cumulative Rebon jackpot pool awarded to the next round winner.
- [ ] Room creator can configure TargetScore (default 100, configurable 60); cumulative series score increments across rounds until a player reaches the TargetScore, concluding the match.
- [ ] Comprehensive unit tests in `games/domino/host/test_domino.cpp` verify opening leads, block resolutions, Rebon rollovers, and TargetScore victories.
