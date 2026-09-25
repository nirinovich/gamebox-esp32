# Dual-Core FreeRTOS Task Separation

We need consistent real-time game simulation ticks without stutter caused by bursty Wi-Fi/HTTP traffic. We decided to pin network I/O, captive portal DNS, and WebSocket handling to Core 0, while running a dedicated `GameEngineTask` on Core 1 executing a fixed-frequency tick loop. State and input commands are exchanged asynchronously across cores via thread-safe queues and mutex-protected memory.
