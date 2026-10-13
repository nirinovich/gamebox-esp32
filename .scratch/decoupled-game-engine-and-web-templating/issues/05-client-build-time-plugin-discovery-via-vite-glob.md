# 05: Client Build-Time Plugin Discovery via Vite Glob

**What to build:**
Enable automatic discovery of client game modules (`ClientPlugin`) using Vite's `import.meta.glob('@games/*/client/index.ts')`. Standardize the exported interface across all client games so that adding a game folder automatically registers its renderer into the bundle without editing a central dictionary.

**Blocked by:** 01: Host Decentralized GameRegistry and Zero-Heap GameId

**Status:** resolved

- [x] Define standard `ClientPlugin` TypeScript interface in `renderers/IGameRenderer.ts`.
- [x] Replace static imports and dictionary in `gameRegistry.ts` with `import.meta.glob`.
- [x] Ensure all existing game client packages export the standard `ClientPlugin` shape.
- [x] Add unit test verifying that `getGameRenderer` resolves all registered games via glob discovery.
