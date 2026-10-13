# Gamebox Design System & Anti-Slop Charter

This document defines the authoritative design tokens, visual architecture, component conventions, and quality standards for the Gamebox client shell. All AI agents and human contributors modifying or generating UI code MUST strictly comply with this document.

---

## 1. Brand Palette & Color Tokens

Gamebox uses a disciplined, high-contrast console palette anchored by three primary colors:
- **Obsidian**: `#14151b` (Base viewport canvas, deep matte backing)
- **Gamebox Red**: `#ef3938` (Hero brand accent, rocket propulsion, primary CTAs, active highlights)
- **Pure White**: `#ffffff` (High-contrast typography, structural outlines on dark surfaces)

### Core Token Reference

| Token Variable | Hex / Value | Semantic Role |
| :--- | :--- | :--- |
| `--gb-black` | `#14151b` | Absolute canvas base |
| `--gb-bg-canvas` | `#14151b` | Viewport background |
| `--gb-surface-1` | `#1c1d25` | Primary panels, navigation header, hero cards |
| `--gb-surface-2` | `#252733` | Elevated tiles, input backings, item containers |
| `--gb-surface-3` | `#2f3140` | Hover states, active pressed states |
| `--gb-red` | `#ef3938` | Primary brand accent, hero CTA background |
| `--gb-red-hover` | `#f15251` | Hover state for red action elements |
| `--gb-red-dim` | `rgba(239, 57, 56, 0.16)` | Translucent badge backing, radar glow backing |
| `--gb-red-glow` | `rgba(239, 57, 56, 0.40)` | Focused control halo, active radar pulse |
| `--gb-white` | `#ffffff` | Primary headings, active tab text, box lines |
| `--gb-text-primary` | `#ffffff` | High-contrast readable copy |
| `--gb-text-secondary` | `#9e9fa8` | Metadata, subtitles, inactive tabs |
| `--gb-text-tertiary` | `#656775` | Captions, subtle hints, disabled indicators |
| `--gb-border-subtle` | `rgba(255, 255, 255, 0.08)` | Internal dividers, subtle tile borders |
| `--gb-border-crisp` | `rgba(255, 255, 255, 0.16)` | Primary card borders, container boundaries |
| `--gb-border-highlight` | `rgba(255, 255, 255, 0.32)` | Hover borders, elevated focus |
| `--gb-border-red` | `#ef3938` | Selected state border, active input border |

---

## 2. Typography & Hierarchy

Gamebox typography balances industrial legibility with crisp arcade precision:

- **Primary Sans**: `-apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif`
  - High-DPI optimized, zero font download overhead.
- **Console Monospace**: `ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, monospace`
  - Mandated for Room Codes (`ROOM CODE`), telemetry ping (`32 ms`), timers, scores, and status pips.

### Scale & Weight Rules
- **Brand Title / Hero**: `1.25rem` (900 weight, `letter-spacing: 0.12em`, uppercase).
- **Section Headers**: `0.78rem – 0.85rem` (800 weight, `letter-spacing: 0.08em`, uppercase).
- **Body / Game Titles**: `0.88rem – 0.95rem` (700–800 weight).
- **Descriptions / Subtitles**: `0.72rem – 0.76rem` (500 weight, `var(--gb-text-secondary)`).
- **Badges / Telemetry**: `0.65rem – 0.70rem` (800 weight, monospace, uppercase).

---

## 3. Brand Assets & Logo Usage

The official Gamebox logo features an open isometric launchbox in crisp geometric lines with a vibrant red gamepad controller launching upward with three propulsion exhaust bars.

Components must use `<GameboxLogo />` (`src/components/common/GameboxLogo.tsx`):
- `variant="full"`: Complete launchbox + launching rocket controller. Use on the Lobby Hero banner and promotional screens.
- `variant="glyph"`: Isolated red gamepad rocket with exhaust trails. Use on the navigation Header, favicons, loading spinners, and compact badges.
- `variant="box"`: Standalone geometric isometric launchbox outline. Use for container framing and structural stamps.

---

## 4. Component Anatomy Standards

### Buttons
- **Primary CTA**: Background `var(--gb-red)`, text `var(--gb-white)`, font-weight `900`, `letter-spacing: 0.05em`, border-radius `var(--radius-sm)` (4px). Active scale `0.96`.
- **Secondary Action**: Background `transparent` or `var(--gb-surface-2)`, border `1px solid var(--gb-border-crisp)`, text `var(--gb-text-secondary)`. Hover border `var(--gb-border-highlight)`.
- **Destructive Action**: Background `var(--gb-red-dim)`, border `1px solid rgba(239, 57, 56, 0.3)`, text `var(--gb-red)`.

### Input Bars (Room Code & Nickname)
- Background `var(--gb-surface-2)`, border `1px solid var(--gb-border-subtle)`, text `var(--gb-white)`, border-radius `var(--radius-sm)` (4px).
- Focus state: `border-color: var(--gb-red); background: var(--gb-surface-3);`.
- Room Code input must enforce 4 uppercase characters in monospace font with generous letter-spacing (`3px`).

### Tiles & Cards (Room Directory & Games)
- Background `var(--gb-surface-1)`, border `1px solid var(--gb-border-crisp)`, border-radius `var(--radius-md)` (8px).
- Hover: border `var(--gb-border-highlight)`, background `var(--gb-surface-2)`.
- Active: border `var(--gb-red)`, transform scale `0.985`.

---

## 5. Anti-Slop Charter (Strict Bans)

To ensure Gamebox maintains an award-winning, console-grade interface and never degrades into generic AI-generated slop, the following patterns are **BANNED ENTIRELY**:

1. 🚫 **NO AI Gradient Soup**: Do NOT use generic purple/magenta/blue multi-stop gradients (`linear-gradient(135deg, #667eea, #764ba2)` or neon violet nebulas). Surfaces must be solid matte obsidian or subtle dark slate tiers.
2. 🚫 **NO Blurry Pastel Dropshadows**: Do NOT use diffuse colorful box-shadows or glowing halos around random cards. Shadows must be dark and grounding (`rgba(0, 0, 0, 0.4)`).
3. 🚫 **NO Low-Contrast Disabled-Looking Grey Text**: All primary text must be stark white (`#ffffff`). Secondary text must have verified WCAG AA contrast against `#14151b`. Never use washed-out `#4b5563` on black backgrounds.
4. 🚫 **NO Bubblegum Pill Rounding**: Do NOT apply oversized border-radiuses (`24px` or `9999px`) to rectangular cards, inputs, or standard buttons. Gamebox uses precise, sharp geometric corners (`4px` to `8px`). Only circular icon pips and status pills may use full rounding.
5. 🚫 **NO Emojis as UI Icons**: Do NOT use system emojis (🎮, 🎲, 🏆) as primary UI icons or buttons. Use crisp vector SVGs or the project's `<Icon />` and `<GameboxLogo />` components.
6. 🚫 **NO Unbranded Accent Colors**: Do NOT introduce random cyan, magenta, emerald, or indigo accent colors into layout shells or navigation. The sole brand accent is Gamebox Red (`#ef3938`). (Secondary colors are permitted solely inside individual game canvas grids for piece differentiation).
7. 🚫 **NO External Resource Dependencies**: Do NOT import Google Fonts, external CDN scripts, or remote image URLs. All code and assets must be self-contained within the single-file client build for offline ESP32 captive portal deployment.
