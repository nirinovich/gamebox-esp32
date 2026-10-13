# Issue 02: Authoritative Rematch Protocol and Series Score Tracking

Status: closed

## Description
Implement the `cmd: "rematch"` protocol in `HostEngine`, `Room`, and `WebAssets.hpp`. When both players click rematch at the game-over screen, reset the game state in the same room, increment the persistent Series Score (`p1Wins`, `p2Wins`), and transition `RoomState` back to `ActiveRoomState` without returning players to the lobby.

## Acceptance Criteria
1. `HostEngine` and `Room` handle `rematch` action in `FinishedRoomState`.
2. When all room participants have requested rematch, the game reinitializes and room returns to `ActiveRoomState`.
3. Series match counter and win tally are tracked in `Room` and broadcast in state payloads.
4. Client UI displays "Rematch Requested (1/2)" and updates seamlessly without navigating back to the lobby.
