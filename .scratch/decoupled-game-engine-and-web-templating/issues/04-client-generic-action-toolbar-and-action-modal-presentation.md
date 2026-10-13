# 04: Client Generic ActionToolbar and ActionModal Presentation

**What to build:**
Create generic Preact `ActionToolbar` and `ActionModal` components in `ClientShell` that render interactive buttons and modal decision prompts directly from `available_actions` and `prompt` state streams. Purge `GameCanvas` of all hardcoded game-specific buttons and modals.

**Blocked by:** 03: Authoritative ActionStream and State Prompt Protocol

**Status:** resolved

- [x] Implement `ActionToolbar.tsx` rendering buttons dynamically from `available_actions` with click dispatch.
- [x] Implement `ActionModal.tsx` rendering modal choice overlays from `prompt` with choice dispatch.
- [x] Strip hardcoded Domino buttons, Battleship fleet buttons, and modal overlays from `GameCanvas.tsx`.
- [x] Add component tests asserting dynamic button and modal rendering from mock state snapshots.
