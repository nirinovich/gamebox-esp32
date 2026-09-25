# PlatformIO and Asynchronous Web Stack

We need a modern C++17 build environment and low-latency, non-blocking networking on the ESP32-S3. We decided to use PlatformIO with the ESPAsyncWebServer and AsyncTCP libraries. PlatformIO provides strict C++17 compilation, library dependency management, and structured `src/`/`include/` directory separation suitable for an OOP architecture, while ESPAsyncWebServer processes HTTP requests and WebSocket frames asynchronously without blocking the Core 1 game simulation loop.
