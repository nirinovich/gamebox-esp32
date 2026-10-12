# 02: Reactive State, Resilient Socket, and Lobby Room Directory Feed

**What to build:** A reactive application state store using Preact Signals, a resilient WebSocket client with exponential backoff and latency tracking, and the Lobby view. Players can set their nickname, toggle procedural audio mute, and watch the real-time RoomDirectory update as rooms are created on the Host.

**Blocked by:** 01: Preact Scaffold, Inlined Bundler, and End-to-End Asset Generation

**Status:** resolved

- [x] `@preact/signals` state store models connection status, latency, nickname, active room, room directory, and error messages
- [x] `GameSocket` service establishes WebSocket connection to `/ws` with automatic exponential backoff reconnection (1s, 2s, 4s, capped at 10s)
- [x] Header renders the player's nickname (persisted in `localStorage`), ping/latency indicator, connection badge, and audio mute button
- [x] Mode tabs filter games between Solo, 2-Player Duel, and Multiplayer Party modes
- [x] Real-time `RoomDirectory` renders active rooms broadcast from the Host with animated live radar beacons and Join buttons
- [x] Room creation form sends `CREATE_ROOM` WebSocket action to the Host with game type and selected mode
