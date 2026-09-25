# 04: Room Manager & 2-Player Multiplayer Tic-Tac-Toe (Room Codes & Matchmaking)

**What to build:** The `RoomManager` handling up to 4 concurrent rooms and 4-character `RoomCode` generation. Player 1 creates a Tic-Tac-Toe room and receives a RoomCode; Player 2 enters the code from a second browser tab. Both Sessions are paired into the Room, assigned X and O markers, enforce strict turn alternation, detect wins/draws, and immediately clean up the room if a player disconnects.

**Blocked by:** 03 (Turn-Based Game & Bot Controller Strategy)

**Status:** resolved

- [x] `RoomManager` manages dynamically allocated `Room` instances using `std::unique_ptr<Room>` up to a 4-room cap.
- [x] 4-character alphanumeric `RoomCode` generator guarantees collision-free active room identification.
- [x] Lobby UI supports "Create Room" (displaying code and "Waiting for opponent") and "Join Room" (inputting code).
- [x] Two distinct browser tabs pair into the same Room and play 1v1 multiplayer with synchronized state.
- [x] Disconnecting one tab terminates the room, awards victory to the remaining player, and frees memory.
- [x] Tests at RoomManager seam verify creation, join routing, duplicate code prevention, and disconnect cleanup.
