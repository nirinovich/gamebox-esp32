# ADR 0030: Adoption of Physical Parlor Visual Engine and CameraJuice for Katro

Status: accepted

## Context & Decision

Katro client presentation originally rendered an abstract neon synthwave grid (`#080c14`, `#10b98166`) inherited from early retro-arcade prototypes. However, Malagasy parlor games (Fanorona and Katro) benefit from tangible cultural fidelity and physical readability. Furthermore, complex multi-relay sowing and captures in Katro were difficult for human players to follow when played at high speed without spatial feedback.

We adopted Manantsoa's physical parlor redesign (commit `4834f47`):
1. **Physical Carved-Wood & Stone Rendering**:
   - Board canvas uses traditional wood/parchment tones (`#ede8dd`, `#ded5c4`, `#cfc4af`) embedded into the base Obsidian (`#14151b`) viewport.
   - Sowing seeds are rendered as 3D radial spherical stones matching Fanorona: Slate blue (`#64748b` -> `#0f172a`) for South (P1) and Terracotta coral (`#fb923c` -> `#7c2d12`) for North (P2).
2. **Kinetic Feedback & Particle Bursts**:
   - Integrated `CameraJuice` for physical impulse shake upon captures.
   - Added `KatroSpark` particle cascades and directional capture beams across the central demarcation line.
3. **Pacing & Pipelining**:
   - Raised Host `BOT_DELAY_SEC` from `0.6f` to `1.5f` so human players can track multi-step sequential relay animations before the bot moves again.
   - Tied `gamehub_desktop` build target to `generate_assets` in `CMakeLists.txt` to eliminate stale web asset anomalies during simulation.

## Consequences

- Both traditional Malagasy games (Fanorona and Katro) share a consistent physical artifact aesthetic anchored in the Gamebox Design System.
- Neon cyber borders in Katro are deprecated in favor of tactile carved pits.
- Bot turns in Solo Katro have deliberate pacing (1.5s) to allow sequential sowing buffers to finish cleanly.
