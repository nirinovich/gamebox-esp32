# Issue 06: Universal Particle Engine, Clamped Retina Scaling, and Camera Juice

Status: closed

## Description
Implement a universal multi-emitter `ParticleEngine` class in `WebAssets.hpp` running an on-demand 60 FPS animation loop across all 7 games. Add clamped 2x Retina canvas scaling and a 3-tier decaying `CameraJuice` screen shake system with `prefers-reduced-motion` support.

## Acceptance Criteria
1. Canvas initializes with clamped `dpr = Math.min(window.devicePixelRatio || 1, 2)` and maps pointer events accurately to 400x400 space.
2. `ParticleEngine` supports confetti, collision sparks, water ripples, and fire/smoke.
3. Victory celebrations trigger animated confetti on all games (not just Snake).
4. `CameraJuice` applies decaying screen shake matrix offsets to the canvas, bypassed when motion reduction is preferred.
