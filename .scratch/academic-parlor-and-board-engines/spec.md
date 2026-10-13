# Spec: Academic Parlor and Board Engines (Purge Clichés, Malagasy Dominoes, Texas Hold'em, Ludo)

Status: ready-for-agent

## Problem Statement

The platform currently features a suite of introductory arcade minigames—Snake, Ping Pong, Tic-Tac-Toe, and Tron—along with early implementations of Battleship and Connect Four. University academic feedback explicitly condemned popular, low-complexity games as unsuitable for a computer science capstone evaluation. Specifically:
1. **Tutorial Clichés Degrade Academic Evaluation**: Popular arcade projects like Snake and Pong represent clichéd beginner tutorials with trivial state machines. Even with visual polish, their inclusion undermines the perception of algorithmic depth and technical rigor.
2. **Domino Engine Ruleset and Interaction Deficiencies**: The existing domino module implements an arbitrary single-round draw mechanic rather than an authentic cultural variant. It lacks cumulative match scoring toward a target, forces player 1 to lead regardless of tiles held, and suffers from a fractured client interaction model where players cannot touch target open ends to place ambiguous tiles.
3. **Absence of High-Complexity Multiplayer Parlor & Board Games**: The platform lacks games showcasing advanced computer science challenges: authoritative hidden-state masking (fog of war), multi-street betting state machines, 7-card poker hand evaluation, and complex spatial board mechanics (token collisions, safe zones, and path navigation).

## Solution

A complete academic curriculum pivot that permanently purges all six legacy tutorial games (Snake, Pong, Tic-Tac-Toe, Tron, Battleship, and Connect Four) and replaces them with an authoritative triad of high-complexity parlor and tabletop engines:
1. **Clean Academic Purge**: Excising all legacy arcade source, client renderers, tests, build configurations, and lobby catalogue entries.
2. **Malagasy Dominoes (Tsipy Domino / Domino Gasy)**:
   - Authentic block-style Double-Six domino rules for 2 to 4 Players.
   - Initial 7-tile deal with undealt tiles locked as inaccessible Deadwood.
   - Highest double in hand leads the opening round.
   - No Boneyard drawing: players unable to legally place a tile pass their turn.
   - Round victory awards the winner the sum of all opponents' unplayed pips.
   - Blocked games (*mihiboka*) award points to the lowest PipSum holder, with tied lowest PipSums rolling over into the Rebon jackpot pool.
   - Cumulative series scoring toward a host-configured TargetScore (e.g. 60 or 100 points).
   - Tactile board anchor interaction: tapping a playable tile highlights glowing target brackets on open ends, allowing direct one-tap placement, supported by a non-squishing hand tray.
3. **Texas Hold'em Poker**:
   - 2 to 4 Player No-Limit Texas Hold'em engine.
   - Authoritative private HoleCards masking until showdown.
   - Shared CommunityCards across Pre-Flop, Flop, Turn, and River streets.
   - Pot and split/side pot accounting.
   - 1,000 starting chip stacks with 10/20 blinds.
   - Authoritative 20-second server action countdown (auto-check/fold on timeout).
   - High-performance zero-heap 7-card rank evaluator on the Host.
4. **Ludo**:
   - 2 to 4 Player cross-and-circle spatial board game.
   - Configurable Quick Match (2 tokens) vs Classic Match (4 tokens) per player.
   - Authoritative dice rolling; rolling a 6 exits the PawnYard and grants a bonus roll (three consecutive 6s forfeits bonus).
   - Collision captures send opponent pawns to their yard and award a bonus roll.
   - Star positions and start cells serve as immune SafeSquares.
5. **Universal Solo Play via Autonomous BotControllers**:
   - Solo mode for all three titles is powered by game-specific heuristic bots running on the Host, eliminating the need for standalone casual puzzle games.

## User Stories

1. As a computer science student, I want all tutorial arcade clichés (Snake, Pong, Tic-Tac-Toe, Tron, Battleship, Connect Four) removed from my repository, so that my instructor evaluates my project on authentic algorithmic and architectural merit.
2. As a room creator, I want to create a Malagasy Dominoes room and specify a TargetScore of 60 or 100 points, so that players compete across multiple strategic rounds.
3. As a domino player, I want the player holding the highest double tile to automatically lead the opening round, so that game initiation strictly conforms to authentic parlor rules.
4. As a domino player, I want my turn to pass when I have no legal plays instead of drawing from a boneyard, so that blocking strategies and hand deductions remain valid.
5. As a domino player, I want to win the sum of all opponents' unplayed pips when I empty my hand, so that aggressive tile shedding is rewarded with cumulative points.
6. As a domino player in a blocked round, I want the player with the lowest PipSum to be awarded the sum of opponents' pips, so that low-value tile preservation remains a valid defensive tactic.
7. As a domino player in a blocked round that ends in a PipSum tie, I want the round points to roll over into a Rebon jackpot pool for the next round, so that stakes dynamically increase after deadlocks.
8. As a mobile domino player, I want to tap a tile and see glowing target brackets on valid open ends of the board, so that placing tiles on touchscreens is intuitive and error-free.
9. As a mobile domino player holding 7 or more tiles, I want my hand displayed in a scrollable, non-squishing tray, so that pip values remain legible on compact smartphone screens.
10. As a poker player, I want to join a Texas Hold'em room with 2 to 4 players, so that I can play authentic poker over local captive portal Wi-Fi.
11. As a poker player, I want my two HoleCards visible exclusively to my client session and concealed from all opponents until showdown, so that bluffing and hidden information are authoritatively enforced.
12. As a poker player, I want to view CommunityCards dealt incrementally across Pre-Flop, Flop, Turn, and River betting rounds, so that I can assess board texture and hand strength in real time.
13. As a poker player, I want to execute Check, Bet, Call, Raise, All-In, and Fold actions with accurate chip and pot tracking, so that standard No-Limit betting rules are respected.
14. As a poker player on a flaky mobile connection, I want a 20-second authoritative turn countdown that automatically Checks or Folds if I disconnect, so that the table never freezes indefinitely.
15. As a poker player reaching showdown, I want the Host engine to evaluate 7-card combinations (HoleCards + CommunityCards) and declare the winner using standard poker hand hierarchies, so that pot distribution is mathematically authoritative.
16. As a poker player going all-in against unequal stacks, I want the engine to correctly calculate main and side pots, so that players only win what they have legitimately called.
17. As a board game player, I want to create a Ludo room and choose between Quick Match (2 tokens) and Classic Match (4 tokens), so that match length can be tuned for fast mobile breaks or longer sessions.
18. As a Ludo player, I want an authoritative dice roll on my turn, so that move calculations cannot be spoofed by client manipulation.
19. As a Ludo player rolling a 6, I want to move a pawn out of my PawnYard onto the starting track and receive a bonus roll, so that rollout turns feel rewarding.
20. As a Ludo player rolling three consecutive 6s, I want my turn bonus forfeited, so that runaway turns are prevented.
21. As a Ludo player landing on an opposing pawn on a non-safe cell, I want the opponent's pawn returned to their PawnYard and my turn awarded a bonus roll, so that tactical collisions drive board tension.
22. As a Ludo player resting on a star-marked SafeSquare, I want my pawn protected from capture collisions, so that I can safely await favorable rolls.
23. As a solo player without nearby peers, I want to launch Malagasy Dominoes, Texas Hold'em, or Ludo in Solo mode against autonomous Host bots, so that all games are fully playable offline against capable AI opponents.
24. As an arcade platform visitor, I want the lobby game catalogue to display only Malagasy Dominoes, Texas Hold'em, and Ludo with distinct badges and tags, so that the platform presents a cohesive tabletop and parlor identity.

## Implementation Decisions

### Module Pruning Decisions
- Permanently delete game directories under `games/snake/`, `games/pong/`, `games/tictactoe/`, `games/tron/`, `games/battleship/`, and `games/connectfour/`.
- Remove legacy game enum values from the core game type system and string mappings.
- Clean up legacy game constructors from the host game factory and compilation registries.
- Remove legacy game renderers, assets, and icons from client registry, client shell, and unit test suites.
- Clean up CMake build lists to stop compiling legacy game host libraries and legacy test targets.

### Malagasy Dominoes Engine
- Refactor the existing domino host module to implement pure block-style rules (*Tsipy Domino*).
- Eliminate boneyard draw routines; implement strict turn-passing when no legal placement exists.
- Add round lifecycle tracking: hand deals, opening lead detection (highest double search), round victory, and block evaluation with lowest PipSum calculation.
- Add persistent match state: cumulative player scores, host-configured TargetScore (default 100, configurable 60), and Rebon rollover pool.
- Update client renderer with tactile board-end targeting anchors: tapping an ambiguous tile activates interactive target zones on the board ends; tapping an anchor commits the move.
- Introduce horizontal paging or scroll tray for the player's tile hand to maintain full tile dimensions regardless of count.

### Texas Hold'em Poker Engine
- Implement a new authoritative poker game module conforming to the polymorphic `Game` interface.
- Deck generation: standard 52-card deck shuffled deterministically using host hardware/pseudo-randomness.
- State machine: `DEALING_HOLE` $\to$ `PRE_FLOP` $\to$ `FLOP` $\to$ `TURN` $\to$ `RIVER` $\to$ `SHOWDOWN`.
- Authoritative state serialization: mask private HoleCards for all players except the recipient session; reveal hole cards only upon showdown or fold-to-one victory.
- Chip accounting: player stacks, small blind, big blind, current bet, pot, and side pots.
- Server turn timer: 20-second tick countdown trigger in `update(dt)` that automatically executes `check` or `fold` on expiration.
- Hand rank evaluator: zero-heap 7-card evaluator computing hand ranks (High Card, Pair, Two Pair, Three of a Kind, Straight, Flush, Full House, Four of a Kind, Straight Flush, Royal Flush) with rank and kicker comparisons.
- Client renderer: Green felt poker table with player seats, chip stacks, community card tray, hole card reveal animations, and betting slider/action buttons.

### Ludo Board Game Engine
- Implement a new authoritative Ludo game module conforming to the polymorphic `Game` interface.
- Board coordinate system: 52 common perimeter track spaces, 4 player-specific starting positions, 4 colored home stretches (5 spaces each), and 4 center home goals.
- Pawn state: `IN_YARD`, `ON_TRACK(index)`, `IN_HOME_STRETCH(index)`, `FINISHED`.
- Turn logic: roll dice, calculate legal movable pawns, await player pawn selection (or auto-move if only 1 pawn is legal).
- Consecutive roll rule: rolling 6 grants a reroll; third consecutive 6 ends turn immediately.
- SafeSquare validation: star positions (`★`) prevent token captures.
- Token mode setting: room configuration for Quick (2 pawns) or Classic (4 pawns).
- Client renderer: Canvas 2D Ludo board showing traditional four-color quadrant layout, token stacks, animated dice roll cube, and tap-to-move highlights.

### Heuristic Bot Controllers
- **Domino Bot**: Heuristic bot playing highest-pip playable tiles to minimize potential block penalties, passing when unable.
- **Poker Bot**: Heuristic bot assessing pre-flop hand tiers and pot odds, executing probabilistic fold/check/call/raise behaviors.
- **Ludo Bot**: Tactical bot prioritizing capturing opponent tokens $>$ exiting yard on 6 $>$ entering safe squares $>$ advancing leading pawns.

## Testing Decisions

### What Makes a Good Test
- Tests must exclusively evaluate observable external behavior at module boundaries: input command ingestion and output serialized state snapshots.
- No tests should assert private internal state or implementation variables.
- All game rules must be verified through deterministic state transitions across complete round lifecycles.

### Modules to Test
1. **Malagasy Dominoes Engine**:
   - Verify opening lead requires highest double tile.
   - Verify turn passing when no legal tile exists in hand.
   - Verify empty-hand win awards sum of all opponents' pips.
   - Verify blocked game awards points to lowest PipSum holder.
   - Verify PipSum tie triggers Rebon rollover to subsequent round.
   - Verify match finishes when a player's cumulative score reaches TargetScore.
   - Verify client interaction dispatches correct end-targeted play commands.
2. **Texas Hold'em Engine**:
   - Verify deck dealing yields exactly 2 hole cards per player and 5 community cards over streets.
   - Verify player-specific state serialization strictly masks opponents' hole cards prior to showdown.
   - Verify betting rounds advance only when all active players have matched the current bet or folded.
   - Verify 20-second timeout automatically resolves with check or fold.
   - Verify 7-card evaluator correctly ranks hands and declares winners, including split pots.
3. **Ludo Engine**:
   - Verify pawns remain in PawnYard until a 6 is rolled.
   - Verify landing on an opponent's pawn on a standard track cell sends it back to the yard and grants a bonus roll.
   - Verify landing on a SafeSquare star cell shares space without capture.
   - Verify pawns require exact rolls to enter the home goal.
   - Verify Quick (2 tokens) and Classic (4 tokens) victory conditions.
4. **Host Engine Integration & Regression**:
   - Verify legacy game commands are rejected or cleanly omitted.
   - Verify room creation, join, solo bot startup, and player detachment across the three new games.

### Prior Art
- Existing authoritative engine tests: `tests/test_host_engine.cpp` using `MockNetworkAdapter` and simulated client message streams.
- Existing game unit tests: `games/domino/host/test_domino.cpp` for state initialization and input execution.

## Out of Scope
- Real-money payment or persistent database user accounts (games use ephemeral room-scoped chips/points).
- Native mobile app wrapper (presentation remains 100% web client over Canvas 2D via captive portal).
- Audio synthesis overhauls beyond reusing existing SoundEngine clicks, wins, and alert chimes.
- 5-player or higher table seating (all games strictly capped at 2 to 4 participants for embedded memory stability).

## Further Notes
- Adheres to ADR 0019 (Academic Roster Pivot).
- Preserves zero-heap allocation principles during the real-time simulation tick on the ESP32-S3 Host.
