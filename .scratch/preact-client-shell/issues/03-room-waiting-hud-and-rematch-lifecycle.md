# 03: Room Waiting Card, Match HUD, and Rematch Series Lifecycle

**What to build:** The complete room orchestration flow from waiting room to match HUD, Game Over screen, and instant Rematch with SeriesScore tracking across multiple matches.

**Blocked by:** 02: Reactive State, Resilient Socket, and Lobby Room Directory Feed

**Status:** resolved

- [x] Waiting Room card displays the 4-letter RoomCode with animated radar pulse, one-click clipboard copy, and cancel button
- [x] Match HUD displays player handles, cyan/pink color tags, live scores, and bot indicators
- [x] Game Over modal appears upon match conclusion with victory confetti effects, winner announcement, and match result details
- [x] Instant Rematch action button transmits `REMATCH` WebSocket command to the Host
- [x] Room retains and renders the cumulative `SeriesScore` across consecutive rematches without disconnecting sessions
