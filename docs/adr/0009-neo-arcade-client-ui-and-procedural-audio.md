# Neo-Arcade Client UI and Zero-Dependency Procedural Audio

Because the ESP32 Access Point is air-gapped without internet access to fetch external web fonts, component libraries, or audio files, the web client must remain completely self-contained within C++ flash memory (`WebAssets.hpp`).

We decided to build the frontend as a high-polish Neo-Arcade Glassmorphism single-page application using pure HTML5, CSS custom properties, and vanilla JavaScript:
1. **View State Machine**: Distinct, animated view states (`LOBBY_VIEW`, `ROOM_WAITING_VIEW`, `GAMEPLAY_VIEW`, `GAME_OVER_MODAL`) replacing cluttered all-in-one screens.
2. **Context-Aware Controls**: The mobile touch D-pad only renders dynamically on touch devices during Snake gameplay and is hidden during Tic-Tac-Toe and on desktop browsers.
3. **Procedural Web Audio Engine**: Zero-asset audio synthesis generated in browser memory using the Web Audio API (`AudioContext`), providing retro chiptune blips, harmonic score chimes, and defeat chords with a 1-tap persistent mute toggle.
4. **Enhanced Canvas Juice**: Rounded snake bodies with directional eyes, glowing pulsing food, 60 FPS particle bursts on consumption/defeat, screen shake, and smooth stroke animations for Tic-Tac-Toe.
