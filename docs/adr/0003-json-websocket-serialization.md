# JSON State Serialization over WebSockets

We need a communication format between the C++ engine on the ESP32-S3 and the thin web client. We decided to transmit structured JSON payloads over WebSockets (using ArduinoJson) rather than custom packed binary structs. While binary encoding is smaller, JSON drastically simplifies browser client parsing, debugging, and schema iteration, while the low concurrency (max 4 rooms) and local Wi-Fi bandwidth make the serialization overhead negligible.
