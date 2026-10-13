# 04: Texas Hold'em Authoritative Poker Engine & 7-Card Evaluator

**What to build:** An authoritative C++ No-Limit Texas Hold'em engine for 2 to 4 players conforming to the polymorphic `Game` interface. Manages a 52-card shuffled deck, deals 2 private HoleCards per player (strictly masked in network state serialization until showdown), deals 5 shared CommunityCards across Flop, Turn, and River streets, tracks player chip stacks (1,000 default), small/big blinds (10/20), pot and side-pots, runs an authoritative 20-second turn countdown (auto-check/fold), and evaluates 7-card poker hands with zero dynamic heap allocation during ticks.

**Blocked by:** 01: Purge Legacy Tutorial Games & Clean Platform Registry

**Status:** ready-for-agent

- [ ] `PokerGame.hpp` and `PokerGame.cpp` implement a 2-to-4 player No-Limit Texas Hold'em finite state machine under `games/poker/host/`.
- [ ] 52-card deck is shuffled deterministically and deals 2 private HoleCards to each seat.
- [ ] `serializeStateForPlayer(id)` strictly conceals opponents' HoleCards prior to showdown while exposing visible chip counts, current bets, and CommunityCards.
- [ ] Multi-street betting loop properly processes `fold`, `check`, `call`, `raise`, and `all-in` commands, balancing pot and side-pots.
- [ ] Server-side 20-second action timer triggers auto-check/fold in `update(dt)` if an active player fails to respond.
- [ ] Zero-heap 7-card hand rank evaluator correctly determines winning hands (High Card up to Royal Flush) and resolves split pots.
- [ ] Autonomous `BotController` logic makes heuristic betting decisions for solo and partially filled tables.
- [ ] Unit tests in `games/poker/host/test_poker.cpp` verify dealing, card masking, side-pot distribution, and 7-card evaluations.
