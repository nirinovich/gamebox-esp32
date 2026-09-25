# ESP32 Game Hub 🎮

An authoritative, headless game engine implemented in modern C++17 for the ESP32-S3 microcontroller. It hosts solo and real-time multiplayer games (Snake and Tic-Tac-Toe) served over a local Wi-Fi Access Point and captive portal via WebSockets.

Clients connect with any standard mobile or desktop web browser—no app installation required.

---

## 🚀 How to Launch

You can run this project in two ways:
1. **Desktop Mode** (recommended for quick testing, development, and grading without hardware)
2. **ESP32 Hardware Mode** (flashing directly to an ESP32-S3 board)
3. **Wokwi Simulator** (virtual simulation in VS Code or CLI)

---

### Option 1: Desktop Runner (No Hardware Required)

The core game engine is platform-agnostic and includes a native desktop server with an embedded web panel.

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

### Option 3: Wokwi Simulator

You can simulate the ESP32 firmware directly inside VS Code:
1. Install the **Wokwi Simulator** extension in VS Code.
2. Build the firmware with `pio run`.
3. Open `diagram.json` and click **Start Simulation** (or press `F1` and select `Wokwi: Start Simulator`).

---

## 📁 Project Structure

```
├── include/              # Public headers (Core engine, games, room management)
├── src/
│   ├── core/             # Portable C++17 domain logic (Session, Room, Snake, TicTacToe)
│   ├── desktop/          # Native desktop server (HTTP & WebSocket for PC testing)
│   └── esp32/            # ESP32 FreeRTOS tasks, Wi-Fi SoftAP, DNS captive portal
├── tests/                # Unit and integration tests
├── platformio.ini        # PlatformIO configuration for ESP32-S3
├── CMakeLists.txt        # CMake build script for desktop and tests
├── diagram.json          # Wokwi simulation hardware definition
└── wokwi.toml            # Wokwi simulation firmware paths
```
