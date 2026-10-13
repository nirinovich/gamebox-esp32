# 02: Host Directory Catalog and InputTemplate Broadcast

**What to build:**
Update `RoomManager` and `HostEngine` to broadcast the registry's game catalogue, metadata, and `InputTemplate` directly over WebSocket. When a client connects or requests the directory feed, the Host sends the list of registered games, their player limits, and required controls, enabling dynamic client rendering without hardcoded client store tables.

**Blocked by:** 01: Host Decentralized GameRegistry and Zero-Heap GameId

**Status:** resolved

- [x] Update `HostEngine` to serialize the registered games catalog with their `InputTemplate` flags into JSON payloads.
- [x] Update `RoomManager` to instantiate games via `GameRegistry` using `GameId`.
- [x] Add integration test verifying catalog serialization and room creation via string `GameId`.
