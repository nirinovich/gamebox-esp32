# 06: Contract Legacy Paths & Enforce Engine Boundaries

**What to build:**
Delete legacy scattered game files from `include/gamehub/core/`, `src/core/`, and `frontend/src/renderers/`. Ensure `include/gamehub/core/` and `src/core/` only contain core platform engine components. Ensure clean builds across Desktop, PlatformIO, and Vite.

**Blocked by:** 03: Classic Real-Time Games Migration, 04: Turn-Based & Grid Games Migration, 05: Complex Tile Game Migration

**Status:** resolved

- [x] Remove legacy game headers from `include/gamehub/core/`
- [x] Remove legacy game implementations from `src/core/`
- [x] Remove legacy renderers from `frontend/src/renderers/`
- [x] Verify `ctest` runs 100% green
- [x] Verify `npm test` runs 100% green
