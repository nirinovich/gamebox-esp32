# ADR 0024: Arcade Lobby Overhaul and Callsign System

Status: accepted

## Context & Decision

The Gamebox client lobby previously suffered from containeritis ("card stew" border fatigue), redundant branding copy, a disconnected player bar stuck on "Guest", and a fragmented information architecture where multiplayer room inputs were wedged between category tabs and game entries.

We decided to overhaul the client lobby interface along five key architectural axes:
1. **Curated Callsign System**: Eliminate the static "Guest" default in favor of curated retro arcade callsigns (`Viper`, `Pilot`, `Neon`, `Rocket`, `Ace`, `Ghost`, `Striker`, etc.) without numerical suffixes, selected from an untaken pool for the 10-player ceiling. The dedicated player row is retained with an instant tactile re-roll button (`🎲` / `↺`) alongside manual editing.
2. **Action-Driven QuickMatchMarquee**: Replace the redundant marketing hero card with an interactive Launchbox marquee featuring single-tap execution (`QUICK MATCH ▶`) of the last-played or signature game.
3. **Unified CartridgeRack**: Consolidate individual game cards into a single structural container with internal hairline dividers (`--gb-border-subtle`), player count badges (`1P SOLO`, `2P DUEL`, `4P PARTY`), and explicit action triggers (`PLAY ▶` and `HOST ⚔️`).
4. **Universal Streamlined Room Code Entry**: Keep the 4-character Room Code input permanently accessible across all category views via a compact 36px terminal bar directly above the catalogue, ensuring players given a room code can immediately join without tab guessing.
5. **Auto-Collapsing Match Feed**: Collapse the Live Match Feed when zero rooms are active, expanding into live match cards only when joinable multiplayer rooms exist on the local network.

## Consequences

- Solves the "Guest" nickname bug and establishes authentic arcade identity immediately upon first connection.
- Reduces vertical scrolling clutter by eliminating 3 redundant card containers.
- Retains instant joinability for multiplayer matches from any tab while keeping Solo AI mode clean and immediate.
