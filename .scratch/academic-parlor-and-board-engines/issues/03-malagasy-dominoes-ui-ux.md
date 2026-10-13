# 03: Malagasy Dominoes Tactile UI/UX & Mobile Hand Overhaul

**What to build:** An intuitive, tactile Canvas 2D client for Malagasy Dominoes. When a player taps a playable tile in their hand, the board displays glowing target anchors directly over valid open ends (Left vs Right). If ambiguous, tapping either board anchor commits the tile to that side. Replaces the squished tile row with a clean horizontal paged or scrollable tray that keeps pips and borders crisp on mobile screens, displaying round scores, cumulative TargetScore progress, and Rebon jackpot indicators.

**Blocked by:** 02: Malagasy Dominoes Authoritative Engine & Cumulative Match Scoring

**Status:** ready-for-agent

- [ ] Tapping a tile with a single valid play commits or highlights the single open end.
- [ ] Tapping a tile that matches both open ends activates glowing tactile target brackets over both board ends; tapping either anchor dispatches `{ cmd: "play", index: i, end: "left" | "right" }`.
- [ ] Player hand renders with fixed, legible tile dimensions in a mobile-friendly horizontal tray without vertical or horizontal squishing when holding 7+ tiles.
- [ ] Opponent hand counts, pass alerts, cumulative match scores toward TargetScore, and active Rebon pool are clearly rendered on the table HUD.
- [ ] Unit tests in `DominoRenderer.test.ts` verify dual-choice anchor click detection and action dispatching.
