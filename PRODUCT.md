# PRODUCT: Gamebox

## 1. Vision & Purpose
Gamebox is an authoritative, headless game engine running on an ESP32-S3 microcontroller that transforms any physical room into an instant local multiplayer gaming parlor over an offline Wi-Fi captive portal. 

Players connect to the "Gamebox" Wi-Fi access point on their smartphones or laptops without installing apps, creating accounts, or requiring an internet connection. The Host microcontroller serves a single-file, zero-dependency web client directly from flash memory and orchestrates real-time authoritative game state replication over low-latency WebSockets.

## 2. Target Constraints & Hardware Realities
1. **Zero Internet & Captive Portal Native**: The client shell must load reliably in restrictive captive portal browser viewports (iOS Captive Network Assistant, Android CaptivePortalLogin).
2. **Embedded Single-File Asset Bundle**: The entire web application (HTML, CSS, Preact runtime, game renderers, and vector assets) is bundled into a single standalone `index.html`, compressed via gzip, and stored in ESP32-S3 PROGMEM flash (`WebAssets.hpp`). Zero external fonts, zero external CDN scripts, zero external HTTP image requests.
3. **Low-Latency Microcontroller Concurrency**: The ESP32-S3 dual-core CPU dedicates Core 0 to network I/O and captive portal serving, and Core 1 to the authoritative fixed-tick simulation loop. The web client must remain lightweight, snappy, and reactive without CPU-burning animations.
4. **Mobile Ergonomics First**: Most players interact via touchscreens in portrait orientation. Touch targets must be generous, tactile, and responsive.

## 3. Brand Personality: The Neo-Arcade Console
Gamebox is not a generic enterprise dashboard or a toy app. It is a dedicated micro-console:
- **Aesthetic**: Matte obsidian hardware, crisp structural seams, high-contrast industrial typography, and electric Gamebox Red propulsion accents.
- **Experience**: Fast, tactile, deterministic, and nostalgic without being clunky.
- **Philosophy**: Zero friction to play. Open box, controller launches, game begins.
