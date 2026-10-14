# Issue 03: Universal Room Code Bar and Collapsible Match Feed

Status: closed
Type: task

## Description

Streamline multiplayer entry points in `frontend/src/components/Lobby/Lobby.tsx` and `Lobby.module.css`:
1. Redesign Room Code bar as an inline 36px arcade terminal bar positioned directly below the category tabs, accessible universally regardless of active tab.
2. Ensure monospace uppercase input with prompt glyph (`>_`).
3. Auto-collapse the Live Match Feed card when 0 rooms are present (`roomDirectory.value.length === 0`), completely eliminating empty-state visual noise.
4. Auto-expand with smooth transition when active rooms are broadcast.
