# Gamebox Brand Identity and Agent-Ready UI Design System

## Context
The client shell previously featured unbranded generic placeholder icons, legacy cyan/pink/navy color tokens (`#090d16`, `#38bdf8`, `#ec4899`), and lacked an authoritative brand showcase. Furthermore, AI agents lacked machine-readable design specifications, risking design drift and the introduction of generic AI UI slop (purple gradients, blurry halos, and low-contrast text).

## Decision
1. **Canonical Brand Assets & Vector Engine**: Ingest the official Gamebox logo marks (Isometric Launchbox + Gamepad Rocket) into `frontend/public/` and author an inline, scalable vector component (`GameboxLogo.tsx`) supporting `full`, `glyph`, and `box` variants with zero external HTTP requests.
2. **Console Monolith Palette**: Replace legacy palettes with the strict brand triumvirate: Obsidian (`#14151b`), Gamebox Red (`#ef3938`), and Pure White (`#ffffff`), supported by matte elevation tiers (`#1c1d25`, `#252733`, `#2f3140`) and crisp structural borders.
3. **Neo-Arcade Client Showcase**: Rebuild the navigation Header with the official Rocket mark and industrial typography ("GAMEBOX"), build a dedicated Lobby Hero Showcase banner, and style the 4-letter Room Code input as an arcade terminal.
4. **Agent Governance & Anti-Slop Charter**: Codify design tokens, typography, and an explicit Anti-Slop Charter in `DESIGN.md` and `PRODUCT.md` at root. Update `AGENTS.md` to mandate that all agents follow `DESIGN.md` for UI changes.

## Consequences
- The client shell immediately presents an unmistakable, high-craft arcade console identity to connecting players over the captive portal.
- All brand marks and styles inline into the single-file embedded bundle (`WebAssets.hpp`), preserving the zero-HTTP and flash-headroom constraints of the ESP32-S3.
- Automated AI contributions are governed by strict contracts, preventing visual regressions and generic UI slop.
