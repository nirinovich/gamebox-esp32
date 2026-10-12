# Issue 08: Frosted Glass Neo-Arcade Shell, CRT Toggle, and Bottom-Sheet Trophy Drawer

Status: closed

## Description
Upgrade the web client navigation, lobby cards, and modals with frosted glass styling (`backdrop-filter: blur(16px)`). Add a live green radar beacon to active room directory entries. Refactor `#stats-drawer` to an animated mobile bottom-sheet with metallic tier badges (Bronze, Silver, Gold, Master), win/loss ratios, and two-step reset confirmation. Add a header CRT scanline filter toggle.

## Acceptance Criteria
1. Header and game cards feature frosted glass styling and subtle neon accent borders.
2. Active rooms display a live pulsing radar beacon.
3. Hall of Fame operates as a bottom-sheet sliding drawer with drag handle, displaying win/loss ratios, metallic achievement badges, and two-step reset confirmation.
4. Header includes a CRT scanline raster overlay toggle button.
5. All `INDEX_HTML_PART_X` string literals in `WebAssets.hpp` strictly remain beneath 12 KB to prevent MSVC error C2026.
