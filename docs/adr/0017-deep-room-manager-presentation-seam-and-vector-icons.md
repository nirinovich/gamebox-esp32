# Deep RoomManager, Presentation Seam Decoupling, and Vector Iconography

## Context
1. **Shallow Room & Session Partitioning**: Solo games were previously instantiated and owned privately inside `Session`, while multiplayer games were managed in `RoomManager`. This produced dual ticking loops in `HostEngine`, duplicated player-to-room mappings in three separate places, and leaked room lifecycle logic across network session code.
2. **Leaky Presentation Seam in Web Client**: Canvas renderers directly imported the `GameSocket` singleton to fire network commands and read mutable `window.__lastGameState` globals, preventing isolated component testing and violating presentation layer boundaries.
3. **Hypothetical Bot Strategy Seams**: Several `IBotStrategy` interfaces existed with zero concrete adapters, while games like `ConnectFourGame` embedded AI heuristics directly in their game classes.
4. **Emoji Iconography**: Raw UTF-8 emojis were used in the game catalog and hall of fame, violating ADR 0010 craft-floor standards and rendering inconsistently across mobile captive portal operating systems.

## Decision
1. **Consolidate Room Management in `RoomManager`**:
   - Deepen `RoomManager` to manage both multiplayer and solo rooms via `createSoloRoom()`.
   - Collapse `Session` into a lightweight connection token with non-owning room references.
   - Unify `HostEngine::tick(float dt)` to drive all active rooms in a single authoritative tick loop.
2. **Decouple the `VisualEngine` Presentation Seam**:
   - Introduce `ActionDispatcher` to `IGameRenderer` (`(action: any) => void`).
   - Remove global `window` state lookups; renderers cache internal frame state and dispatch actions via the seam adapter.
3. **Realize Concrete Bot Strategies**:
   - Implement `ConnectFourMinimaxStrategy` as a concrete adapter satisfying `IConnectFourStrategy`, injectable into `ConnectFourGame`.
4. **Vector Iconography via `lucide-preact`**:
   - Strictly remove UTF-8 emojis from state stores.
   - Introduce `<Icon />` vector component wrapping tree-shakeable Lucide icons with uniform 2px stroke width matching ADR 0010 aesthetic.
