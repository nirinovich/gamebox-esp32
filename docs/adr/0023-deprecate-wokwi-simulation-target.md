# Deprecate Wokwi Simulation Target in Favor of Native Desktop Runner

## Context
ADR-0007 established a dual-target architecture: a native desktop runner (`src/desktop/`) and an ESP32-S3 firmware build (`src/esp32/`) configured for both physical hardware and the Wokwi virtual simulator (`diagram.json`, `wokwi.toml`). In practice, Wokwi's virtual environment only supports Wi-Fi Station Mode (`WIFI_STA`) outbound connections and cannot emulate the ESP32 Wi-Fi SoftAP (`WiFi.mode(WIFI_AP)`), Port 53 DNS captive portal redirection, or browser-to-firmware WebSocket handshakes required by Gamebox. Consequently, running Gamebox under Wokwi produces only serial log output without any interactive web client or multiplayer capability, creating maintenance confusion and false expectations for contributors.

## Decision
1. **Retire Wokwi Simulation Files**: Delete `diagram.json` and `wokwi.toml` from the repository.
2. **Standardize Dual Targets**: Officially designate the two supported execution targets:
   - **Target 1 (Desktop Runner - CMake)**: Portable C++17 engine running natively on Windows/Linux/macOS on `http://localhost:8080`, supporting instant zero-hardware testing across multiple browser tabs and full debugger attachment.
   - **Target 2 (Physical ESP32-S3 Hardware - PlatformIO)**: Firmware flashing to physical `esp32-s3-devkitc-1` boards broadcasting the offline `Gamebox` SoftAP and captive portal.
3. **Update Documentation & Governance**: Reframe Document 07 (`docs/architecture/07-dual-target-desktop-and-simulation.md`), `README.md`, and the architecture index to present the two genuine execution targets, and mark ADR-0007 as superseded by this record.

## Consequences
- Eliminates non-functional configuration files and developer confusion regarding browser simulation.
- Retains 100% of zero-hardware testing velocity via the high-performance native desktop runner.
- Leaves physical ESP32-S3 firmware compilation and flash workflows untouched.
