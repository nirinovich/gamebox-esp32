# 01: Master Brand Assets and Vector Logo Engine

**What to build:** The official Gamebox logo marks (Isometric Launchbox + Gamepad Rocket) are persisted in the project and rendered via a high-craft inline SVG component system (`full`, `glyph`, `box` variants) and browser favicon. Connects from raw assets to scalable in-DOM rendering and favicon display, verifiable via unit test.

**Blocked by:** None (can start immediately)

**Status:** resolved

- [x] Master brand assets stored in `frontend/public/` (`logo-dark.png`, `logo-glyph.png`, `logo-light.png`).
- [x] High-craft inline SVG component `GameboxLogo.tsx` implemented supporting `variant="full"`, `variant="glyph"`, and `variant="box"`.
- [x] Vector SVG favicon created and linked in `frontend/index.html`.
- [x] Brand assets render without external HTTP fetches, compatible with single-file packaging.
