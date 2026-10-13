# 03: Authoritative ActionStream and State Prompt Protocol

**What to build:**
Extend the authoritative state replication protocol to support `available_actions` (array of legal discrete player commands) and optional `prompt` objects (branching modal decisions). Migrate the reference implementation (Dominoes) to emit legal actions (`draw`, `pass`) and end-choice prompts (`place_left`, `place_right`) through this stream.

**Blocked by:** 02: Host Directory Catalog and InputTemplate Broadcast

**Status:** resolved

- [x] Define standardized JSON schema for `available_actions` and `prompt`.
- [x] Update `Game` interface or state serialization to emit `available_actions` and `prompt`.
- [x] Migrate `DominoGame` host implementation to populate `available_actions` and `prompt` dynamically based on turn and legal moves.
- [x] Add unit test verifying that `DominoGame` emits correct legal actions and prompt states when moves are blocked or require end choice.
