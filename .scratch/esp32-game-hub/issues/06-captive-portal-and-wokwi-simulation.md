# 06: Captive Portal DNS Redirection & Wokwi Simulation Setup

**What to build:** The embedded captive portal subsystem on Core 0 of the ESP32-S3. Built-in DNS captures port 53 traffic to trigger automatic captive portal popups on mobile phones connecting to the `ESP32-GameHub` SoftAP. Includes Wokwi simulation files (`wokwi.toml`, `diagram.json`) so the entire ESP32 firmware can be emulated in VS Code or in the browser without physical hardware.

**Blocked by:** 05 (Real-Time Multiplayer Arena Snake)

**Status:** resolved

- [x] Captive portal DNS server running on UDP port 53 answering all queries with Host SoftAP IP (192.168.4.1).
- [x] HTTP captive portal redirect endpoints (`/generate_204`, `/hotspot-detect.html`, `/ncsi.txt`) auto-launch game portal.
- [x] `wokwi.toml` and `diagram.json` configured for ESP32-S3 simulation with virtual Wi-Fi bridge.
- [x] Firmware compiles cleanly under PlatformIO for target `esp32-s3-devkitc-1`.
