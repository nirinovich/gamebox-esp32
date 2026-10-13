# Decoupled Game Engine and Schema-Driven Web Templating

## Context
Adding or modifying a game previously required modifying centralized enums and switch-cases in `GameTypes.hpp` and `GameFactory.hpp`, hardcoding UI buttons in `GameCanvas.tsx`, and maintaining a hardcoded catalogue in `gameStore.ts`. This created high coupling between core engine infrastructure and specific game implementations.

## Decision
1. **Decoupled Host GameRegistry**: Replace centralized `enum class GameType` with zero-heap fixed string identifiers (`GameId`). Games register themselves statically via `GameRegistry` without core engine headers referencing concrete games.
2. **Host-Advertised InputTemplates**: Games declare their input capabilities (`InputTemplate` such as `DPAD`, `GRID_POINTER`, or `DRAG_SLIDER`) and room capacities at registration. The Host broadcasts this catalogue to the ClientShell, dynamically configuring the Lobby and controls.
3. **Authoritative ActionStream & Generic ActionToolbar**: Legal player actions (`available_actions`) and branching choices (`ActionModal`) are streamed in authoritative state snapshots. `GameCanvas.tsx` renders these generically with zero game-specific JSX.
4. **Build-Time Client Discovery**: The ClientShell discovers client game packages (`ClientPlugin`) via Vite `import.meta.glob('@games/*/client/index.ts')`, eliminating manual entries in `gameRegistry.ts`.
5. **Monolithic Inlined Flash Assets**: Maintain the single-file RFC 1952 Gzip flash asset pipeline (`WebAssets.hpp`) for sub-second, zero-roundtrip Wi-Fi captive portal loading on the ESP32-S3.

## Consequences
- Adding a new game requires only adding files in `games/<game>/` and registering it in the host build, touching zero engine core or client shell files.
- The ClientShell becomes a true generic game engine runtime terminal.
