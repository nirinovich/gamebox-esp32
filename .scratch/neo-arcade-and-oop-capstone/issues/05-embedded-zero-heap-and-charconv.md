# Issue 05: Embedded Zero-Heap Grid and Non-Throwing String Conversions

Status: closed

## Description
Flatten `TronGame`'s 30x30 nested vector grid (`std::vector<std::vector<int>>`) into a contiguous static `std::array<int8_t, 900>` to eliminate 31 dynamic heap allocations on the microcontroller. Replace all `try/catch/std::stoi` calls with standard C++17 `std::from_chars` (`<charconv>`) for exception-free, non-allocating string parsing.

## Acceptance Criteria
1. `TronGame` uses flat `std::array<int8_t, 900>` with helper coordinate indexing `x + y * 30`.
2. All occurrences of `std::stoi` in game input handlers replaced with `std::from_chars`.
3. Code compiles cleanly with `-fno-exceptions`.
4. Unit tests pass with identical collision and territory behavior.
