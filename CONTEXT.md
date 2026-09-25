# ESP32 Game Hub

An authoritative headless game engine running on an ESP32-S3 that hosts multiplayer and solo games over a local Wi-Fi captive portal.

## Language

**Host**:
The physical ESP32-S3 microcontroller running the authoritative C++ game engine, HTTP server, and WebSocket transport.
_Avoid_: Board, device, hardware

**Client**:
A connected mobile or desktop web browser rendering game state received via WebSocket and streaming player inputs back to the Host.
_Avoid_: User, player, browser

**Session**:
An active, stateful network connection associated with a single connected Client.
_Avoid_: Connection, socket

**Player**:
A participant entity inside a Game, controlled either by a Session or by an AI agent.
_Avoid_: User, account

**Room**:
An isolated execution environment that pairs one or more Sessions to a single Game instance.
_Avoid_: Lobby, channel, match

**Game**:
A polymorphic stateful domain model encapsulating rules, entities, input handling, and state replication for a specific game mode.
_Avoid_: Engine, minigame, match

**Tick**:
A single fixed-interval execution step of the real-time game loop on the Host.
_Avoid_: Frame, cycle, step

**RoomCode**:
A unique 4-character alphanumeric identifier used by Clients to join a specific Room.
_Avoid_: Pin, key, password

**PlayerController**:
An abstract strategy interface providing decision-making or input ingestion for a Player.
_Avoid_: InputHandler, Agent, Driver

**HumanController**:
A PlayerController implementation that consumes inputs queued from a connected Client Session.
_Avoid_: RemotePlayer, NetworkUser

**BotController**:
A PlayerController implementation that autonomously calculates moves on the Host using game-specific heuristics or algorithms.
_Avoid_: AI, CPU, Computer
