# ADR 0016: Unified Zero-Allocation GameplayCore and CanvasGridLayer

## Context
Each authoritative game in the C++ core currently implements its own bespoke 2D coordinate structs, board arrays, and manual string concatenation for JSON state serialization. Similarly, board game renderers in the web client independently calculate cell dimensions, line borders, and pointer coordinate hit-testing. We need to introduce shared engine utilities without compromising the ESP32-S3 microcontroller constraints (512 KB SRAM, FreeRTOS Core 0 cooperative loop) or the 60 FPS headless client presentation architecture.

## Decision
1. **Host (C++17)**: Introduce `GameplayCore` consisting of:
   - A fixed-capacity, compile-time allocated 2D grid template `Grid2D<T, Width, Height>` built on `std::array<T, Width * Height>` to guarantee zero heap fragmentation.
   - Standard value-type spatial primitives: `Vec2i` and `Rect2i`.
   - A zero-dynamic-allocation `JsonBuffer<Capacity>` streaming serializer to standardize authoritative state generation without repetitive string allocations.
2. **Client (TypeScript / Canvas 2D)**: Introduce `CanvasGridLayer` as a shared presentation primitive:
   - Provides reusable cell geometry calculation, neon border rendering, glow passes, and pointer coordinate to cell coordinate translation (`hitTestCell(px, py)`).
   - Reused across grid-based games (`TicTacToe`, `ConnectFour`, `Battleship`).
   - Keeps all game interaction strictly on the Canvas while Preact remains responsible for the outer shell HUD and modal dialogs.

## Consequences
- **Positive**: Eliminates duplicated coordinate math and JSON formatting across games.
- **Positive**: Zero additional heap allocation or fragmentation on the ESP32-S3 host.
- **Positive**: High visual consistency across all client board games with unified neon rendering passes and responsive hit-testing.
