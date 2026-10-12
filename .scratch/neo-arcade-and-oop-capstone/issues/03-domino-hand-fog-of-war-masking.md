# Issue 03: Dominoes Hand Fog-of-War Network Masking

Status: closed

## Description
Implement player-specific state serialization (`serializeStateForPlayer(PlayerId id)`) for `DominoGame`. Ensure opponent hands are strictly concealed as count numbers only, matching the authoritative Fog-of-War security pattern established in Battleship.

## Acceptance Criteria
1. `DominoGame::serializeStateForPlayer(PlayerId id)` is implemented.
2. The requesting player receives their own actual hand tiles.
3. Other players' hands are serialized as tile counts only (`p2_count: N`), preventing browser DevTools inspection cheating.
4. Unit tests in `test_host_engine.cpp` verify hand masking across multiple connected sessions.
