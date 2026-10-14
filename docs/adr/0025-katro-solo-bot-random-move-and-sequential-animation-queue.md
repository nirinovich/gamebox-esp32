# ADR 0025: Katro Solo Bot Random Move and Sequential Animation Queue

Status: accepted

## Context & Decision

Prior to this decision, the solo AI opponent for Katro used a deterministic greedy simulation heuristic that picked the move maximizing seed gain, causing predictable repetition in solo matches. Furthermore, rapid server ticks could deliver game state updates faster than the browser canvas could animate seed relay sowing, risking visual teleportation or skipped animation frames.

In commit `25fb47b` on `origin/dev`, Manantsoa Hari proposed:
1. Selecting random valid moves for the bot among available pits in North's territory (`computeBotMove()`).
2. Buffering incoming server states into a sequential queue on the client (`pendingStateQueue` in `KatroRenderer.ts`).
3. Throttling WebSocket replication to occur only when `m_moveSeq` advances.

We decided to reconcile and merge these changes into our Gamebox brand system and GoF architecture:
1. **Random Move Selection with KatroBoard Entity**: Ported the random available move selection using `std::mt19937` and `std::uniform_int_distribution` directly into `KatroGame::computeBotMove()`, operating cleanly on the decoupled `KatroBoard` entity without compromising GoF Strategy encapsulation in `KatroBotController`.
2. **Sequential Animation Buffer**: Adopted `pendingStateQueue` in `KatroRenderer.ts`, guaranteeing that multi-step sowing and relay animations play out to completion in order before the next board state is applied.
3. **Universal Move Sequence Gating**: Replaced hardcoded game type branching in `HostEngine::tick` with polymorphic sequence gating (`newSeq > prevSeq || newSeq == 0`), preventing 15 Hz WebSocket spamming during idle turns across all turn-based parlor games on the ESP32.
4. **Preserved Gamebox Branding & Miniz Asset Compression**: Retained canonical Gamebox brand tokens (`#14151b`, `#ef3938`, `--gb-surface-*`) and single-file inlined bundle packing via `asset_packer.exe` with Miniz RFC 1952 gzip compression into `WebAssets.hpp`.

## Consequences

- Solo Katro matches are dynamic, varied, and non-repetitive while maintaining a near-zero CPU calculation footprint on the ESP32.
- Complex sowing and capture animations render smoothly and sequentially without seed snapping or teleportation.
- Wireless network bandwidth over the ESP32 captive portal is conserved by only broadcasting state when game sequence numbers advance.
- The single-file captive portal bundle remains fully intact with 70.6% flash reduction.
