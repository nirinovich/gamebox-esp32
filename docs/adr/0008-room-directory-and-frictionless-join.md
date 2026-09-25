# Room Directory and Frictionless Multiplayer Matchmaking

In a portable hardware gamebox operating as an isolated Wi-Fi Access Point, all players are physically co-located within the Access Point's signal radius. Requiring players to manually read, type, or copy-paste 4-character RoomCodes creates unnecessary friction on mobile captive portals.

We decided to implement a real-time reactive `RoomDirectory` on the Host. When a Client creates a multiplayer Room, the Host automatically broadcasts the open Room metadata (internal code, host player nickname, game mode, and current player count) to all Clients in the Lobby via WebSockets. Connected players see active rooms appear immediately on an "Active Games" feed and can join with a single tap. The internal 4-character RoomCode remains the transport identifier between Host and Room, but is abstracted entirely away from the user interface.
