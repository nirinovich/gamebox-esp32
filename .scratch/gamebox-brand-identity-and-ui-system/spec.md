# Spec: Gamebox Brand Identity and Agent-Ready UI Design System

Status: resolved

## Problem Statement

The Gamebox platform has a functional client shell and headless host engine, but lacks a cohesive, authoritative brand identity. Currently, the client user interface exhibits visual inconsistency, unbranded placeholders, and generic styling that does not reflect the Gamebox arcade console experience:

1. **Unbranded Visual Identity**: The client shell displays a generic controller rectangle SVG icon and default sans-serif text, with no connection to the official Gamebox brand marks (the isometric Launchbox and Rocket Gamepad).
2. **Palette Disconnect and Visual Slop**: The existing CSS theme relies on legacy cyan/pink/navy tones (`#090d16`, `#38bdf8`, `#ec4899`) instead of the canonical Gamebox palette (`#14151b`, `#ef3938`, `#ffffff`). This leads to a generic, unfocused visual presentation that risks degrading into generic AI-generated interface patterns (low-contrast grey typography, muddy pastel glow effects, inconsistent component borders).
3. **Absence of Agent Design Governance**: AI agents contributing code to the frontend have no canonical design contract (`DESIGN.md` and `PRODUCT.md`) to guide them. Without strict, machine-readable design constraints and an enforced Anti-Slop Charter, automated contributions will continually introduce disparate styles, mismatched tokens, and off-brand visual elements.
4. **Lack of Arcade Console Showcase**: The Lobby and Client Shell currently present as a generic web utility rather than a high-performance neo-arcade console hub. The Room Code entry, Room Directory feed, and Game Directory cards lack tactile, high-contrast arcade affordances.

## Solution

Implement the Gamebox visual identity and design system from A to Z across the client shell and establish durable governance so that all AI agents build within strict, high-craft brand boundaries:

1. **Canonical Asset Integration and Scalable Vector Engine**:
   - Integrate the authentic Gamebox logo marks: the full isometric Launchbox with launching Gamepad Rocket, the isolated Red Gamepad Rocket glyph, and the inverted light variant.
   - Author a pixel-perfect, scalable inline SVG component system capable of rendering the full mark, compact glyph, and standalone launchbox at arbitrary resolutions with zero external network requests, preserving the ESP32 single-file embedded bundle budget.
   - Establish high-resolution vector favicons, touch icons, and application metadata for captive portal browsers.

2. **Console Monolith Design System (`#14151b`, `#ef3938`, `#ffffff`)**:
   - Establish Obsidian (`#14151b`) as the root viewport canvas, layered with matte console surface tiers (`#1c1d25`, `#262833`) and stark white structural borders.
   - Position Gamebox Red (`#ef3938`) as the exclusive high-energy brand accent for primary calls-to-action, active peripheral states, and radar indicators.
   - Enforce stark, high-contrast typography in Pure White (`#ffffff`) and muted steel (`#9e9fa8`) with crisp geometric hierarchy and monospace treatment for codes and telemetry.

3. **Neo-Arcade Client Shell Showcase**:
   - Redesign the client Header with the authentic Gamebox Rocket mark, industrial brand wordmark, and streamlined host connection badge.
   - Build a dedicated Gamebox Hero Showcase in the Lobby featuring the rocket launch emblem, high-contrast mode filter tabs, an arcade-terminal 4-character Room Code input bar, and live Room Directory feed tiles.
   - Overhaul game directory cards, session controls, and action buttons with tactile, zero-slop console feedback.

4. **Agent-Ready Governance & Anti-Slop Charter**:
   - Publish `DESIGN.md` defining exact tokens, typography scales, layout grids, component states, and explicit banned patterns (barring AI purple gradients, fuzzy pastel dropshadows, cartoonish rounded corners, and low-contrast text).
   - Publish `PRODUCT.md` articulating the headless engine mission, captive portal constraints, and console-grade UX standards.
   - Update `AGENTS.md` to establish mandatory compliance with `DESIGN.md` before any UI edits.
   - Record architectural trade-offs in ADR `0021-gamebox-brand-identity-and-agent-ui-design-system.md`.

## User Stories

1. As a player connecting to the captive portal, I want to see the authentic Gamebox rocket logo immediately upon loading, so that I recognize I am connected to a dedicated gaming console.
2. As a player on a smartphone, I want the web client to load with a bespoke vector favicon and dark console theme, so that the experience feels like an installed native app rather than a generic website.
3. As a player browsing the Lobby, I want to see an arcade-grade hero banner showcasing the Gamebox emblem, so that the platform feels energetic, polished, and purposeful.
4. As a player entering a 4-character Room Code, I want a high-contrast arcade terminal input with Gamebox Red focus highlights, so that joining a private match feels crisp, tactile, and effortless.
5. As a player inspecting the live Room Directory, I want active match cards to display high-contrast host names, player counts, and pulse beacons without blurry shadows, so that open games are instantly scanable.
6. As a player selecting a game from the Game Directory, I want interactive game tiles to feature clear category badges and crisp geometric hover/active states, so that navigating the catalogue is satisfying.
7. As a player in an active match, I want the Game HUD and action buttons to utilize the unified Gamebox color tokens, so that game controls feel like an integrated extension of the console hardware.
8. As a player monitoring network quality, I want the connection indicator to clearly display live ping telemetry against the obsidian header, so that I know my input stream is responsive.
9. As a frontend developer, I want the Gamebox logo to render via an inline SVG component with flexible sizing and variant props, so that brand marks can be embedded anywhere in the UI without extra HTTP fetches.
10. As a firmware developer, I want all brand assets and SVG vectors to inline into the single-file HTML bundle, so that the host can serve the entire branded client from embedded flash with zero filesystem overhead.
11. As a contributing AI agent, I want a comprehensive `DESIGN.md` file specifying canonical tokens and rules, so that I can implement new UI features without guessing styling values or introducing design drift.
12. As a contributing AI agent, I want an explicit Anti-Slop Charter listing banned design patterns, so that I never generate generic AI gradient soup, muddy cards, or illegible color combinations.
13. As a project maintainer, I want `AGENTS.md` to enforce `DESIGN.md` as a required precondition for frontend modifications, so that multi-agent workflows maintain continuous design consistency.
14. As a project maintainer, I want ADR 0021 in the repository, so that future contributors understand the architectural rationale behind the console monolith aesthetic and embedded vector asset model.
15. As a test author, I want to verify that the brand tokens and logo variants render properly across all client views, so that visual regressions are caught automatically during testing.

## Implementation Decisions

### 1. Vector Logo Component Architecture
- Implement a centralized vector logo component supporting three canonical presentation variants:
  - `full`: Isometric Launchbox in structural outline with Gamepad Rocket launching upward in Gamebox Red.
  - `glyph`: Standalone Gamepad Rocket with vertical exhaust trails for compact headers, favicons, and telemetry badges.
  - `box`: Geometric Launchbox outline for structural badges or container framing.
- The component must accept dynamic dimensions, class names, and color overrides while defaulting to official brand values (`#14151b`, `#ef3938`, `#ffffff`).
- Store master PNG assets in the public asset directory as reference artifacts and provide an SVG favicon linked in the client HTML shell.

### 2. Design Token System & Palette Architecture
- Refactor the global style tokens to eliminate legacy cyan/pink hues from root variables:
  - `--gb-bg-canvas`: Obsidian (`#14151b`) root application background.
  - `--gb-surface-1`: Primary surface panel (`#1c1d25`).
  - `--gb-surface-2`: Elevated component/card surface (`#252733`).
  - `--gb-surface-3`: Active/hover state surface (`#2f3140`).
  - `--gb-accent-red`: Gamebox Red (`#ef3938`) primary brand accent.
  - `--gb-accent-red-hover`: Highlighted red (`#f15251`).
  - `--gb-accent-red-dim`: Translucent red badge backing (`rgba(239, 57, 56, 0.14)`).
  - `--gb-text-primary`: Stark White (`#ffffff`).
  - `--gb-text-secondary`: Industrial Steel (`#9e9fa8`).
  - `--gb-text-muted`: Deep Slate (`#626473`).
  - `--gb-border-subtle`: Dark seam border (`rgba(255, 255, 255, 0.08)`).
  - `--gb-border-crisp`: Defined structural border (`rgba(255, 255, 255, 0.16)`).
  - `--gb-border-accent`: High-energy focus border (`#ef3938`).
- Maintain functional player tokens strictly mapped to high-contrast identifiers (Player 1 White/Red, Player 2 Red/Steel) for in-game turn clarity.

### 3. Client Shell Showcase Redesign
- **Header**:
  - Integrate the Gamebox Rocket glyph into the logo container with bold geometric typography (`letter-spacing: 0.08em`).
  - Re-theme connection status badge, sound toggle, scanline toggle, and trophy drawer triggers into unified matte console buttons.
  - Streamline the Player profile nickname badge into a high-contrast obsidian input block.
- **Lobby**:
  - Hero showcase banner at the top of the lobby highlighting the Gamebox Launchbox insignia, system operational tag, and instant action cues.
  - High-contrast category filter tabs (`Solo AI`, `2P Duel`, `Parlor Table`) defaulting to Solo AI with crisp red active underlines and zero fuzzy glow.
  - Redesigned 4-letter Room Code submission bar featuring an arcade-terminal aesthetic and tactile join trigger.
  - Live Room Directory list with clean status pips, clear player counts, and solid join buttons.
  - Game Directory catalogue displaying high-contrast game cards with distinctive category badges.

### 4. Agent Governance & Anti-Slop Charter
- Create `DESIGN.md` at repository root defining:
  - Complete Token Map and Semantic Roles.
  - Typography Scale and Monospace Pairing.
  - Elevation and Border Principles (stark borders over diffuse drop shadows).
  - Anti-Slop Charter with explicit bans:
    - Banned: Multi-stop purple/magenta gradient backgrounds.
    - Banned: Low-contrast grey text that violates WCAG AA standards against obsidian backgrounds.
    - Banned: Blurry glowing boxes or floating colorful halos without functional meaning.
    - Banned: Rounded pill buttons exceeding 8px radius on rectangular cards (no bubblegum styling).
    - Banned: Non-standard icons or generic emojis in place of clean geometric vectors.
- Create `PRODUCT.md` at repository root defining the product vision, core customer journey (frictionless Wi-Fi connection -> instant play), and craft expectations.
- Update `AGENTS.md` to mandate that agents inspect `DESIGN.md` before generating or modifying any UI code.
- Record architectural decisions in `docs/adr/0021-gamebox-brand-identity-and-agent-ui-design-system.md`.

## Testing Decisions

### Seam Strategy
- **Primary Seam**: Reactive State & DOM Presentation Seam.
  - Test external component rendering, token propagation, and interaction handlers using Vitest and `@preact/signals` state harnesses.
  - Verify that the Header, Lobby hero banner, Room Code bar, and Game Directory cards render expected DOM structures, accessibility attributes, and brand elements given deterministic store state.
  - Verify that `GameboxLogo` renders the correct vector paths and viewBox coordinates for `full`, `glyph`, and `box` variants without runtime exceptions.
- **Secondary Seam**: Build & Single-File Packaging Seam.
  - Verify that `npm run build` compiles TypeScript, bundles all assets via `vite-plugin-singlefile`, and emits a valid standalone `frontend/dist/index.html` with zero external URL references.

### Prior Art
- Existing reactive tests in `frontend/src/__tests__/protocol.test.ts` and `frontend/src/__tests__/action_stream.test.ts` demonstrate testing state mutation, DOM element generation, and signal subscription without spinning up full headless browser instances.

## Out of Scope

- Rewriting host-side C++ game logic or FreeRTOS task schedules.
- Altering the WebSocket JSON protocol message schema (already formalized in ADR 0020).
- Modifying canvas-specific drawing routines inside individual game renderers (e.g., Domino canvas layout or Texas Hold'em card sprites), except where global CSS background variables affect canvas containment.

## Further Notes

- The single-file HTML client is embedded into the host firmware via `asset_packer.cpp` and gzipped into PROGMEM. Keeping SVGs optimized and avoiding oversized external font imports preserves flash memory headroom on the ESP32-S3.
