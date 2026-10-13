# Cooperative Core 0 Tick Loop and WebSocket Thread Safety

## Context
ADR 0002 originally partitioned execution across ESP32 cores by running a dedicated FreeRTOS `GameEngineTask` on Core 1 while `ESPAsyncWebServer` and `AsyncTCP` ran on Core 0. Under physical hardware testing on the ESP32-S3, `AsyncWebSocket::text(...)` calls dispatched directly from Core 1 concurrently with Core 0 LwIP packet handling produced heap queue corruption, socket queue stalls, packet drops, and sudden burst-flushing (erratic freezing followed by rapid speed-ups).

## Decision
Supersede the dual-core FreeRTOS task partition in ADR 0002 with a unified, cooperative 15 Hz tick loop running directly in `loop()` on Core 0 alongside `ws.cleanupClients()` and `dnsServer.processNextRequest()` using a non-blocking `millis()` interval check (`dt = 0.066s`).

Because authoritative game logic for all 7 games executes in under 50 microseconds per tick (<0.1% CPU of the 240 MHz Tensilica LX7 core), running simulation synchronously on Core 0 incurs zero perceptible latency while guaranteeing 100% thread safety for `AsyncWebSocket` packet serialization, eliminates cross-core race conditions, avoids task stack RAM overhead, and prevents Wi-Fi socket buffer congestion.
