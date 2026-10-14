# Issue 02: Quick Match Marquee

Status: closed
Type: task

## Description

Replace static hero branding in `frontend/src/components/Lobby/Lobby.tsx` and `Lobby.module.css` with an interactive Quick Match Marquee:
1. Feature the animated Launchbox vector emblem (`GameboxLogo`).
2. Display dynamic headline "QUICK MATCH" with featured game subtitle (persisted in `localStorage` under `gamebox_last_game`, defaulting to `domino_solo`).
3. Provide high-contrast primary CTA button `[ QUICK MATCH ▶ ]` triggering immediate `startSolo(lastGameId)`.
4. Remove redundant "GAMEBOX" wordmark and marketing brochure copy from hero section.
