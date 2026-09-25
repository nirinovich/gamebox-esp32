# Embedded Flash String Literals for Web Assets

We need to serve the frontend single-page application (HTML/CSS/JS) from the ESP32-S3 without complicating project builds. We decided to store web assets directly in C++ flash memory (`PROGMEM`) as raw string literals in a header file rather than creating a separate LittleFS/SPIFFS filesystem partition. This allows the entire project to build and flash in a single command without external filesystem image generation tools or risk of filesystem partition table corruption.
