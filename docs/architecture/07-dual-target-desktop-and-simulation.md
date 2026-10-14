# 07. Dual-Target: Desktop Runner & Physical ESP32 Hardware

This document explains Gamebox's dual-target architecture, enabling rapid local testing on desktop operating systems (Windows, Linux, macOS) without microcontroller hardware, alongside production deployment to physical ESP32-S3 microcontrollers.

---

## 🎯 1. Why Dual-Target Development?

Embedded microcontroller firmware development typically suffers from slow feedback loops:
1. Flashing over USB takes 15–45 seconds per iteration.
2. Debugging crashes on hardware often requires JTAG probes or serial print debugging.
3. Automated CI/CD pipelines in GitHub Actions do not have physical microcontrollers attached.

To solve this, Gamebox decouples all core game logic into platform-agnostic C++17 modules (`src/core/`), allowing developers to compile and test on desktop machines in seconds.

```mermaid
graph TD
    Core["src/core/ (HostEngine, RoomManager, Game Logic)"]
    
    subgraph DesktopTarget ["1. Native Desktop Runner (CMake)"]
        DeskNet["src/desktop/ (Winsock / POSIX Sockets)"]
        DeskBin["gamehub_desktop (.exe)"]
        Browser["Local Browser (http://localhost:8080)"]
    end
    
    subgraph HardwareTarget ["2. Physical Hardware Target (PlatformIO)"]
        ESPNet["src/esp32/ (ESPAsyncWebServer, Wi-Fi SoftAP)"]
        Firmware["firmware.bin"]
        Hardware["ESP32-S3 Microcontroller"]
    end

    Core --> DeskNet --> DeskBin --> Browser
    Core --> ESPNet --> Firmware
    Firmware --> Hardware
```

---

## 💻 2. Option 1: Native Desktop Runner (CMake)

The desktop server implementation (`src/desktop/main.cpp`) implements an HTTP and WebSocket server using native platform socket APIs:
- **Windows**: Links standard Windows Sockets (`ws2_32`).
- **Linux / macOS**: Uses standard POSIX sockets and pthreads.

### Building and Running:
```bash
# Configure and build
cmake -B build
cmake --build build --config Release

# Launch desktop server
./build/Release/gamehub_desktop.exe   # Windows
./build/gamehub_desktop               # Linux / macOS
```

The server opens on `http://localhost:8080`. Developers can open multiple browser tabs or incognito windows to test multiplayer games locally without any hardware.

---

## 🔌 3. Option 2: ESP32-S3 Hardware Deployment (PlatformIO)

The microcontroller firmware is configured via [`platformio.ini`](file:///c:/Users/Nikita/Documents/C++/platformio.ini):

```ini
[env:esp32-s3-devkitc-1]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
monitor_speed = 115200

build_flags = 
    -std=gnu++17
    -Iinclude
    -D ARDUINO_USB_CDC_ON_BOOT=1
    -D ARDUINO_USB_MODE=1

build_src_filter =
    +<core/*>
    +<esp32/*>
    -<desktop/*>
```

### Commands:
```bash
# Build firmware binary
pio run

# Flash to connected ESP32-S3 board
pio run -t upload

# Monitor serial debug output
pio device monitor
```
