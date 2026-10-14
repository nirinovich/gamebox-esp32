# Issue 04: Unified Cartridge Rack

Status: closed
Type: task

## Description

Unify the fragmented game cards in `frontend/src/components/Lobby/Lobby.tsx` and `Lobby.module.css` into a cohesive Cartridge Rack:
1. Replace individual card stew with a single container (`.cartridgeRack`) featuring subtle internal hairline dividers (`--gb-border-subtle`).
2. Each cartridge row displays:
   - Retro vector game glyph / emblem box.
   - Game title and brief gameplay description.
   - Player capacity badge (`1P SOLO`, `2P DUEL`, `4P PARTY`).
   - Tactile CTA trigger (`PLAY ▶` for Solo AI, `HOST ⚔️` for Multiplayer).
3. Ensure hover and focus states conform to Impeccable Craft Floor (no glow halos, no ghost borders, clean red accent highlight).
