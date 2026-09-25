# 10. Craft-Floor UI Deslop and Multi-Game Engine Expansion

Date: 2026-09-25

## Status

Accepted

## Context

The initial UI used informal patterns such as UTF-8 emojis for icons, decorative backdrop blur glassmorphism, same-size container card grids, and text/button gradients. Additionally, the platform only supported Snake and Tic-Tac-Toe.

## Decision

1. **Craft-Floor Deslop**:
   - Strictly ban emojis as icons; author dedicated inline SVGs with unified 2px stroke, round caps, and round joins.
   - Replace decorative glassmorphism with solid surface styling (`--surface: #111827`, `--bg: #090d16`).
   - Replace the 2-column card grid with an intentional tabbed layout (Solo vs. Multiplayer) and structured list rows.
   - Remove gradient text and button gradients, grounding actions in solid accent colors.

2. **Core Game Engine Expansion**:
   - Implement **ConnectFourGame**: 7×6 column-major grid with alpha-beta depth-5 minimax AI for solo play and 2P turn-based PvP.
   - Implement **PongGame**: Real-time 2P paddle physics with 15Hz authoritative server simulation.
   - Implement **TronGame**: Real-time 30×30 grid light-trail arena with 90-degree turning and collision checks.
   - Implement **BattleshipGame**: 10×10 naval battle with 1-tap "Deploy Fleet" randomized arrangement, fog-of-war enemy view, and coordinate firing.

## Consequences

- Completely clean, responsive, professional web client with zero external asset or network dependencies.
- Total of 6 distinct games (9 modes: 3 solo, 6 multiplayer) running on a unified polymorphic `Game` interface.
- 100% CTest automated test coverage across all game engines.
