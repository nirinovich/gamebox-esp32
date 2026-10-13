# 01: Host Decentralized GameRegistry and Zero-Heap GameId

**What to build:**
A decentralized static game registration subsystem on the Host. Games self-register using a macro that binds a canonical string identifier (`GameId`), factory constructor, and metadata descriptor (`GameDescriptor`). Identifiers are stored using fixed-capacity character arrays (`char[24]`) or string views to guarantee zero heap allocations across the host simulation loop.

**Blocked by:** None (can start immediately)

**Status:** resolved

- [x] Implement `GameRegistry` singleton with static registration macro `REGISTER_GAME`.
- [x] Support zero-heap `GameId` type with fixed 24-byte capacity and `std::string_view` comparisons.
- [x] Define `GameDescriptor` struct containing title, category, player capacity, and `InputTemplate` bitmask/flags.
- [x] Add unit test verifying that mock games can be registered and retrieved without heap allocations.
