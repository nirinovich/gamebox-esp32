# Issue 01: Callsign System and Instant Shuffle

Status: closed
Type: task

## Description

Implement the curated arcade callsign system in `frontend/src/state/gameStore.ts` and `frontend/src/components/Header/Header.tsx`:
1. Pool of 18 curated retro callsigns: `['Viper', 'Pilot', 'Neon', 'Rocket', 'Ace', 'Ghost', 'Striker', 'Shadow', 'Blaze', 'Radar', 'Titan', 'Falcon', 'Havoc', 'Apex', 'Phantom', 'Volt', 'Cipher', 'Vortex']`.
2. Replace `'Guest'` default with automatic assignment of an untaken callsign on initialization.
3. Add a tactile shuffle button (`↺` / `🎲`) next to the input in `Header.tsx` / `Header.module.css` that cycles through available callsigns in 1 tap.
4. Keep the input editable for custom handles.
