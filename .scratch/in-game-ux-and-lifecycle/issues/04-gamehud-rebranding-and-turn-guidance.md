# Issue 04: GameHUD Rebranding and Turn Guidance

Status: closed
Type: task

## Description

In `frontend/src/components/GameHud/GameHud.tsx` and `GameHud.module.css`:
1. Remove all unbranded colors (`--p1-cyan`, `--p2-pink`). Re-theme using canonical Gamebox tokens (`--gb-white`, `--gb-red`, `--gb-surface-2`).
2. Implement dynamic turn guidance:
   - For Solo AI: "YOUR TURN" (pulsing red pip) or "AI THINKING...".
   - For Multiplayer: "YOUR TURN" (when `state.turn === room.playerNum`) or "OPPONENT'S TURN".
   - When finished: "MATCH OVER".
