# Mission: 3D Printing ESP32-S3 Enclosures with Prusa i3 MK3S & PrusaSlicer

## Why
Print a clean, dimensionally accurate, robust protective enclosure for the ESP32-S3 microcontroller using an Original Prusa i3 MK3S (0.4mm nozzle) and PrusaSlicer, transitioning from zero 3D-printing experience to confidently slicing, preparing, and operating the printer autonomously.

## Success looks like
- Successfully distinguish and inspect 3D model formats (.STL geometry vs .SRT subtitle typo) and import `ESP32-S3.stl` into PrusaSlicer.
- Orient the enclosure flat-side down using "Place on Face" (`F`), configure optimal 0.20mm layer height, perimeters (3), infill (15-20%), and clean organic supports for port cutouts.
- Export valid G-code to an SD card and execute the print on the i3 MK3S with proper PEI sheet cleaning and first-layer Live Z verification.
- Obtain a finished enclosure where the ESP32-S3 board sits securely with unobstructed USB-C, antenna, and GPIO cutouts.

## Constraints
- Total beginner in 3D printing, slicers, and physical printer maintenance.
- Hardware: Original Prusa i3 MK3S with a standard 0.4mm brass nozzle.
- File asset: Pre-existing model `ESP32-S3.stl` located in `Downloads`.

## Out of scope
- CAD mechanical modeling from scratch in Fusion 360 or FreeCAD.
- Nozzle hardware swaps (0.25mm or 0.60mm/0.80mm nozzles).
- High-temp or abrasive exotic filaments (Nylon, PC, Carbon Fiber) requiring enclosures and hardened nozzles.
