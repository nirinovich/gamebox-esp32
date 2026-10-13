# Academic Roster Pivot: Parlor and Board Engines Replacing Tutorial Games

## Context
Academic feedback required the complete excision of ubiquitous, low-complexity tutorial games (Snake, Pong, Tic-Tac-Toe, Tron, Battleship, and Connect Four) to satisfy university course rigor. The platform required non-trivial game state machines, hidden information handling, and academic credibility while retaining responsive multiplayer and solo play over an ESP32-S3 Wi-Fi captive portal.

## Decision
We permanently deprecate and purge the six legacy arcade games, establishing an authoritative triad of sophisticated parlor and tabletop games:
1. **Malagasy Dominoes (Tsipy Domino)**: Block-style double-six dominoes with no Boneyard drawing, passing on block, Rebon rollover on ties, and multi-round cumulative scoring toward a host-defined TargetScore (60 or 100 points).
2. **Texas Hold'em Poker**: 2–4 player No-Limit poker engine with authoritative HoleCards masking, CommunityCards, Pot accounting, 20-second turn timeouts, and a Host-side 7-card hand rank evaluator.
3. **Ludo**: 2–4 player cross-and-circle board game featuring spatial track navigation (52 track tiles + home stretches), pawn collisions and yard returns, safe star zones, and configurable token counts (Quick 2-token vs Classic 4-token).

Solo play across all three titles is serviced through autonomous `BotController` implementations rather than standalone arcade puzzles.

## Consequences
- Eliminates clichéd beginner projects, significantly improving academic rigor and evaluation.
- Consolidates network protocol around turn-based, hidden-information parlor models.
- Requires building authoritative bot heuristics and tactile Canvas 2D renderers for complex board and card states.
