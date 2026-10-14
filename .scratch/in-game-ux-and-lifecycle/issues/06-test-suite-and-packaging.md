# Issue 06: Test Suite and Packaging

Status: closed
Type: task

## Description

1. Write unit tests in `frontend/src/__tests__/ingame_ux.test.ts` covering:
   - In-game header displaying `[ ⏴ LEAVE ]` when view is `GAME` or `WAITING`.
   - Profile bar unmounting when view is `GAME` or `WAITING`.
   - Exit action calling `leaveRoom()` directly in Solo mode.
   - Exit action requesting forfeit confirmation modal in Multiplayer mode.
   - GameHUD rendering canonical tokens and turn guidance.
   - GameHUD room code copying.
2. Build single-file bundle via `npm run build`.
3. Pack web assets via `asset_packer.exe`.
4. Run CTest suite.
