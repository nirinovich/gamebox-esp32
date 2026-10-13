# 05: Texas Hold'em Client Renderer & Mobile Betting Interface

**What to build:** A responsive Canvas 2D Texas Hold'em presentation client. Features an authentic green felt poker table with 2-to-4 seat layouts, chip stack badges, community card animations across Flop/Turn/River, private HoleCard display for the local player, active turn countdown ring, and a mobile-optimized betting bar (Fold, Check/Call, Bet/Raise slider, All-In).

**Blocked by:** 04: Texas Hold'em Authoritative Poker Engine & 7-Card Evaluator

**Status:** ready-for-agent

- [ ] `PokerRenderer.ts` under `games/poker/client/` renders a high-DPI Canvas 2D felt table, dealer button, pot total, and community cards.
- [ ] Active player positions display chip amounts, current bets, and turn indicator with a pulsing 20-second countdown ring.
- [ ] Local player's HoleCards render face-up with clean suit and rank graphics; opponents render face-down card backs until showdown.
- [ ] Touch/mouse controls provide quick buttons for Fold, Check, Call, and a dynamic slider/stepper for Raise/All-In, dispatching action payloads.
- [ ] Unit tests in `PokerRenderer.test.ts` verify betting action dispatch and rendering state transitions.
