# 02: Console Monolith Design Tokens and Global Theme

**What to build:** A unified console color and elevation system across the web client. Replaces all legacy cyan/pink/navy variables with the authoritative Obsidian (`#14151b`), Gamebox Red (`#ef3938`), and Stark White (`#ffffff`) palette, complete with high-contrast text contrast, tactile active states, and scanline filter harmony.

**Blocked by:** None (can start immediately)

**Status:** resolved

- [x] `frontend/src/theme.css` overhauled with `--gb-bg-canvas: #14151b`, `--gb-accent-red: #ef3938`, `--gb-text-primary: #ffffff`.
- [x] Surface tiers (`#1c1d25`, `#252733`, `#2f3140`) and crisp borders (`rgba(255, 255, 255, 0.12)`) implemented.
- [x] Root body and container padding, typography, and CRT scanline filter tuned for the console monolith aesthetic.
- [x] Elimination of legacy unbranded color variables from core styles.
