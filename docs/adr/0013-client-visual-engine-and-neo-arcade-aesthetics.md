# ADR 0013: Client-Side Neo-Arcade Visual Engine and Distributed MVC Architecture

## Status
Accepted

## Context
The ESP32-S3 host is a resource-constrained microcontroller (240 MHz dual-core, 512 KB SRAM).
The authoritative domain simulation executes at a fixed 15 Hz tick rate to guarantee deterministic FreeRTOS execution, minimal Wi-Fi packet contention, and zero heap fragmentation.
However, modern smartphone screens refresh at 60 Hz or 120 Hz with high pixel densities (Retina, AMOLED 3x). Rendering raw 15 Hz state packets produces visible motion stepping. Furthermore, static graphical assets (PNG/MP3) would exhaust onboard SPI Flash storage.

## Decision
1. Implement a strict Distributed Model-View-Controller (MVC) architecture.
2. The C++ core (`HostEngine`, `Room`, `Game`) remains 100% headless, publishing state at 15 Hz with zero visual logic.
3. The client presentation layer in `WebAssets.hpp` implements:
   - Clamped 2x Retina high-DPI scaling (`window.devicePixelRatio` capped to 2.0).
   - Linear temporal state interpolation running in `requestAnimationFrame` for continuous motion games.
   - A pre-allocated procedural particle pool (`ParticleEngine`) and camera shake matrix (`CameraJuice`).
   - Procedural Canvas 2D vector drawing and glow compositing for all 7 games.
   - Pure algorithmic Web Audio API oscillator synthesis.
4. Keep all embedded C++ string literals in `WebAssets.hpp` partitioned below 12,000 bytes to strictly satisfy MSVC Error C2026.

## Consequences
- **Positive**: ESP32-S3 incurs 0% extra CPU load and 0 bytes additional RAM.
- **Positive**: Zero static media assets required; firmware binary remains compact.
- **Positive**: Client achieves 60/120 FPS visual fluidity and arcade-grade tactile feedback.
- **Positive**: Proves clean separation of domain logic from presentation logic during capstone defense.
