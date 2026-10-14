# ADR 0026: In-Game Navigation Lifecycle and HUD Turn Guidance

Status: accepted

## Context & Decision

During active gameplay (`currentView === 'GAME'`), the client shell exhibited three critical UI/UX shortcomings:
1. **Trapped Gameplay**: No in-game exit or go-back control existed; players were unable to return to the lobby or abort a match without reloading the page or concluding the game.
2. **Unwanted Mid-Game Nickname Mutation**: The player profile bar remained globally mounted and editable in `Header.tsx`, allowing players to edit or re-roll callsigns while bound to an active room on the Host, while consuming ~50px of vital vertical screen space needed for the canvas.
3. **Turn Ambiguity & Anti-Slop Palette Smells**: `GameHud.tsx` used non-canonical cyan/pink accent classes (`var(--p1-cyan)` and `var(--p2-pink)`), lacked explicit turn guidance (leaving players uncertain whose turn was active), and lost visibility of the Room Code once a match started.

We decided to resolve these issues across the client shell:
1. **Contextual In-Game Navigation Header**: When `currentView !== 'LOBBY'`, the top Header replaces the static brand wordmark with an arcade exit trigger `[ ⏴ LEAVE ]`.
   - **Solo AI Matches**: Immediate 1-tap exit and room detachment returning to the lobby instantaneously.
   - **Multiplayer Matches**: Defensive 2-tap confirmation (`MatchExitPrompt`) warning the player of forfeit before sending `{ cmd: 'leave' }`.
2. **Strict Profile Bar Lifecycle**: The `.profileBar` is rendered **strictly** when `currentView === 'LOBBY'`. Inside `WAITING` or `GAME`, it is completely unmounted, recovering 50px of vertical viewport space to comfortably frame the 400px canvas on mobile screens without scrolling.
3. **Canonical HUD Re-Branding & Active Turn Guidance**:
   - Eliminated all cyan/pink CSS references in `GameHud.module.css`, converting player handles to stark white (`--gb-white`) on obsidian surface backings.
   - Implemented dynamic turn guidance with a pulsing Gamebox Red pip and high-contrast text (`"YOUR TURN"` vs `"OPPONENT'S TURN"` / `"AI THINKING..."`).
   - Integrated an inline clickable Room Code pill in the HUD (e.g., `[ WXYZ 📋 ]`) that copies the active room code to the clipboard for frictionless spectator/friend sharing.

## Consequences

- Players have an intuitive, arcade-standard navigation path out of any active match at all times.
- Mobile screens display the entire game canvas without vertical scroll clipping.
- Nickname desynchronization during active matches is completely prevented.
- Turn ambiguity is resolved across all turn-based parlor games.
- Layout shell adheres 100% to the Impeccable Anti-Slop Charter and Gamebox design tokens.
