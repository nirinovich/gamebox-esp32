# 06: End-to-End Test Suite and Single-File Flash Packaging Verification

**What to build:** Comprehensive verification of the brand overhaul. Runs unit and component test suites verifying logo path generation, token hierarchy, and DOM rendering, followed by full Vite single-file production compilation confirming zero external network requests and compliance with the ESP32-S3 asset packer.

**Blocked by:** 03 (Branded Header and Live Telemetry Experience), 04 (Lobby Hero Showcase and Terminal Room Code Bar), 05 (Agent Governance, Anti-Slop Charter, and System Architecture)

**Status:** resolved

- [x] Vitest unit test suite `frontend/src/__tests__/branding.test.ts` testing `GameboxLogo` variants and DOM structure.
- [x] TypeScript check and Vitest test suite pass (`npm test`).
- [x] Production build succeeds (`npm run build`) and generates standalone `frontend/dist/index.html`.
- [x] Zero broken imports, zero external font or asset requests.
