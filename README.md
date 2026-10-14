# ESP32 Game Hub 🎮

An authoritative, headless game engine implemented in modern C++17 for the ESP32-S3 microcontroller. It hosts solo and real-time multiplayer games (Malagasy Tsipy Dominoes, Ludo, and Malagasy Katro) served over a local Wi-Fi Access Point and captive portal via WebSockets.

Clients connect with any standard mobile or desktop web browser—no app installation required.

## 📥 Getting the Code

Clone the repository to your local machine:
```bash
git clone https://github.com/nirinovich/gamebox-esp32.git
cd gamebox-esp32
```

---

## 🚀 How to Launch

You can run this project in two ways:
1. **Desktop Mode** (recommended for quick testing, development, and grading without hardware)
2. **ESP32 Hardware Mode** (flashing directly to an ESP32-S3 board)

> **Note on Web Assets:** All ClientShell assets (HTML/JS/CSS) are compressed with RFC 1952 Gzip and embedded directly into the binary as C++ byte arrays using the native C++ asset packer (`tools/asset_packer.cpp`). No Python scripts, external package managers, or separate SPIFFS/LittleFS filesystem partitions are needed.

---

### Option 1: Desktop Runner (No Hardware Required)

The core game engine is platform-agnostic and includes a native desktop server with an embedded web panel. No third-party package managers or external libraries are required—standard system libraries (`ws2_32` on Windows, `pthread` on Linux/macOS) are linked automatically.

#### Requirements
- CMake 3.16+
- C++17 compatible compiler (MSVC on Windows, GCC, or Clang)

#### 1. Build
```bash
cmake -B build
cmake --build build --config Release
```

#### 2. Run Tests (Optional)
```bash
ctest --test-dir build --output-on-failure
```

#### 3. Launch
- **Windows**:
  ```powershell
  .\build\Release\gamehub_desktop.exe
  ```
- **Linux / macOS**:
  ```bash
  ./build/gamehub_desktop
  ```

#### 4. Play
Open your browser and navigate to:
```
http://localhost:8080
```
> **Tip for Multiplayer:** Open two separate browser tabs (or an incognito window). Create a Room in Tab 1, note the 4-letter Room Code, and join with Tab 2!

---

### Option 2: ESP32 Hardware (PlatformIO)

#### Requirements
- [PlatformIO](https://platformio.org/) (CLI or VS Code extension)
- ESP32-S3 board (e.g., `esp32-s3-devkitc-1`) connected via USB

#### 1. Build and Flash
```bash
# Build firmware
pio run

# Upload to connected ESP32-S3
pio run -t upload

# Open serial monitor
pio device monitor
```

#### 2. Connect and Play
1. On your phone or laptop, search for Wi-Fi networks and connect to:
   - **SSID:** `ESP32-GameHub` (Open / No password)
2. The captive portal will pop up automatically.
   - If it doesn't open automatically, open your browser and navigate to `http://192.168.4.1`.
3. Choose a game and start playing!

---

## 📁 Project Structure

```
├── include/              # Public headers (Core engine, games, room management)
├── src/
│   ├── core/             # Portable C++17 domain logic (Session, Room, HostEngine, RoomState)
│   ├── desktop/          # Native desktop server (HTTP & WebSocket for PC testing)
│   └── esp32/            # ESP32 FreeRTOS tasks, Wi-Fi SoftAP, DNS captive portal
├── games/                # Co-located games (host simulation + client renderers)
│   ├── domino/           # Malagasy Tsipy Dominoes
│   ├── katro/            # Traditional Malagasy Katro Mancala
│   └── ludo/             # Ludo cross-and-circle board game
├── frontend/             # Single-page ClientShell (Preact/React, Canvas 2D)
├── docs/                 # Documentation suite & ADRs
│   ├── architecture/     # Complete architectural deep dives & lifecycle guides
│   ├── adr/              # Architectural Decision Records (ADRs)
│   └── index.html        # Interactive browser for architecture documentation
├── tools/                # Native C++ host build utilities (asset_packer)
├── tests/                # Unit and integration tests
├── platformio.ini        # PlatformIO configuration for ESP32-S3
└── CMakeLists.txt        # CMake build script for desktop and tests
```

