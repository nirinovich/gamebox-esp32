# Issue 01: In-Game Contextual Header and Exit Trigger

Status: closed
Type: task

## Description

In `frontend/src/components/Header/Header.tsx` and `Header.module.css`:
1. When `currentView.value !== 'LOBBY'`, replace the static wordmark with an arcade exit trigger `[ ⏴ LEAVE ]`.
2. When clicked in Solo mode, immediately call `gameSocket.leaveRoom()`.
3. When clicked in Multiplayer mode, request confirmation from `showExitPrompt`.
