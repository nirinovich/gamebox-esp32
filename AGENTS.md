## Agent skills

### Issue tracker

Issues and specs live as local markdown files under `.scratch/`. See `docs/agents/issue-tracker.md`.

### Triage labels

Five canonical roles: `needs-triage`, `needs-info`, `ready-for-agent`, `ready-for-human`, `wontfix`. See `docs/agents/triage-labels.md`.

### Domain docs

Single-context (`CONTEXT.md` and `docs/adr/` at repo root). See `docs/agents/domain.md`.

### Design System & Anti-Slop Charter

All frontend, styling, and UI changes MUST read and strictly adhere to `DESIGN.md` and `PRODUCT.md`. All UI generation and modification must pass Impeccable's Craft Floor:
- Use strictly canonical Gamebox tokens (`#14151b`, `#ef3938`, `#ffffff`).
- 🚫 Zero tolerance for generic AI slop: no purple/magenta gradients, no zero-offset blurry halos, no ghost cards (1px border + soft shadow), no kickers/eyebrows above headings, no colored `border-left` on cards, and no unbranded components or system emojis.
- Mandate themed browser surfaces: selection highlight, caret color, custom console scrollbars, `:focus-visible` rings, and monospace tabular numerals for metrics and timers.

