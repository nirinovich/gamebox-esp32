# Spec: Arcade Lobby Overhaul and Callsign System

Status: closed

## Problem Statement

The Gamebox client lobby suffered from containeritis ("card stew" border fatigue), redundant branding copy, an awkward player bar stuck on "Guest", and a disconnected information architecture where multiplayer room inputs were wedged between category tabs and game entries:

1. **Guest Nickname Bug**: The client initialized to the static string `'Guest'`, never assigning players a unique arcade identity.
2. **Duplicated Wordmarks & Dead Marketing Copy**: The word `GAMEBOX` was duplicated across the top navigation bar and hero card, coupled with brochure text (*"Authoritative Multiplayer Arcade Engine"*) that occupied 80px of prime mobile screen space without gameplay utility.
3. **Card Stew & Border Fatigue**: 7 consecutive nested boxes with identical 1px borders created visual noise and lacked visual hierarchy.
4. **Disjointed Multiplayer Seams**: The Room Code bar and empty Live Match Feed card took up permanent space even when users were playing Solo AI, yet users given a room code need immediate access to enter it.
5. **Passive Game Cards**: Game directory entries looked like static table rows lacking clear Call-to-Action (CTA) triggers.

## Proposed Changes

### 1. Callsign System & Instant Shuffle
- Define a canonical pool of 18 non-numeric arcade callsigns in `gameStore.ts`:
  `['Viper', 'Pilot', 'Neon', 'Rocket', 'Ace', 'Ghost', 'Striker', 'Shadow', 'Blaze', 'Radar', 'Titan', 'Falcon', 'Havoc', 'Apex', 'Phantom', 'Volt', 'Cipher', 'Vortex']`.
- If `localStorage` contains empty string or `'Guest'`, automatically assign the first untaken callsign.
- In `Header.tsx` (the Player nickname row), retain the dedicated player row and add an arcade shuffle button (`↺` / `🎲`) adjacent to the input that cycles to the next untaken callsign in 1 click.

### 2. Action-Driven QuickMatchMarquee
- Replace the static hero banner in `Lobby.tsx` with `QuickMatchMarquee`.
- Feature the animated Launchbox vector emblem alongside a dynamic "QUICK MATCH" headline showing the last-played game (persisted in `gamebox_last_game`, defaulting to `domino_solo` / `Dominoes Solo`).
- Provide a prominent red `[ QUICK MATCH ▶ ]` button that immediately starts the game in 1 click.

### 3. Universal Streamlined Room Code Bar & Collapsible Match Feed
- Redesign the Room Code bar into an ultra-compact 36px terminal input directly beneath the category tabs.
- Collapse the Live Match Feed container completely when 0 rooms are active (`roomDirectory.value.length === 0`), expanding only when active joinable rooms are broadcast by the Host.
- Support universal room joining from any tab.

### 4. Unified CartridgeRack
- Group game items into a single unified container (`.cartridgeRack`) with subtle internal dividers (`--gb-border-subtle`), eliminating disconnected floating cards.
- Each row contains:
  - Game vector emblem box.
  - Game title and concise gameplay description.
  - Player capacity badge (`1P SOLO`, `2P DUEL`, `4P PARTY`).
  - Direct tactile CTA button (`PLAY ▶` for Solo AI, `HOST ⚔️` for Multiplayer).

### 5. Verification & Packaging
- Unit tests covering callsign generation, shuffle behavior, quick match launch, and cartridge rack action triggers.
- Inlined single-file bundle build via `npm run build`.
- Asset packing into `WebAssets.hpp` and automated server verification on `http://localhost:8081`.
