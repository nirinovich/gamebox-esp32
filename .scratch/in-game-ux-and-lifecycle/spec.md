# Spec: In-Game UX, Navigation Lifecycle, and HUD Turn Guidance

Status: closed

## Problem Statement

1. **Trapped In-Game**: Once in a match (`currentView === 'GAME'`), no exit button exists, preventing players from leaving or forfeiting without browser refresh.
2. **Global Nickname Editing**: The Player nickname input and shuffle button are rendered in `Header.tsx` at all times, permitting handle changes mid-game and occupying ~50px of vertical space above the canvas.
3. **Turn Ambiguity**: Turn-based games show static text (`"Solo Run"` or `"Duel Active"`), leaving players uncertain whose turn is active.
4. **Anti-Slop Color Smells**: `GameHud.module.css` uses `--p1-cyan` and `--p2-pink`, violating Rule 10 of the Anti-Slop Charter.
5. **Missing In-Game Room Code**: Once a multiplayer match begins, the 4-letter Room Code disappears from view, preventing sharing.

## Proposed Changes

### 1. In-Game Contextual Header & Exit Trigger
- In `Header.tsx`, check `currentView.value !== 'LOBBY'`.
- When in `GAME` or `WAITING`, replace the static logo text with a tactile arcade button `[ ⏴ LEAVE ]`.
- For Solo games (`room.code === 'SOLO'`), immediately call `gameSocket.leaveRoom()`.
- For Multiplayer games, open `MatchExitPrompt` (confirmation dialog).

### 2. MatchExitPrompt Confirmation Modal
- Render a crisp confirmation dialog when leaving a multiplayer match:
  - Heading: "FORFEIT MATCH?"
  - Subtitle: "Leaving will concede victory to your opponent."
  - Primary button: "FORFEIT & LEAVE" (red danger button).
  - Secondary button: "CANCEL" (resume play).

### 3. Profile Bar Lifecycle
- In `Header.tsx`, render `.profileBar` **only** when `currentView.value === 'LOBBY'`.
- In `GAME` and `WAITING`, unmount completely, saving ~50px of vertical height.

### 4. GameHUD Re-Branding & Active Turn Guidance
- Remove `--p1-cyan` and `--p2-pink`. Use `--gb-white` for player callsigns on `--gb-surface-2` score backings.
- Add dynamic turn guidance:
  - If `isSolo`: when player's turn, pulsing red pip + "YOUR TURN"; when bot's turn, "AI THINKING...".
  - If `Duel`: when `state.turn === room.playerNum`, "YOUR TURN"; else "OPPONENT'S TURN".

### 5. In-Game Room Code Badge
- In `GameHud.tsx`, if not solo and `room.code` is present, display a monospace pill with copy icon: `[ WXYZ 📋 ]`.
- Clicking copies code to clipboard with temporary "COPIED" feedback.

### 6. Verification
- Vitest suite in `frontend/src/__tests__/ingame_ux.test.ts`.
- Single-file bundle build + Miniz packing into `WebAssets.hpp`.
- CTest suite (11/11 tests).
