# Issue 03: Unmount Profile Bar In-Game

Status: closed
Type: task

## Description

In `frontend/src/components/Header/Header.tsx`:
1. Conditionally render `.profileBar` ONLY when `currentView.value === 'LOBBY'`.
2. In `GAME` and `WAITING` views, completely unmount the bar to lock player nicknames and reclaim 50px of vertical screen space for the canvas.
