# 01: Vendor miniz and Wire CMake Build Target

**What to build:** Vendor the single-file `miniz` library (`miniz.h` and `miniz.c`) under `third_party/miniz/` and configure CMake to expose a `miniz` static/object library target that native executables can link against.

**Blocked by:** None (can start immediately)

**Status:** resolved

- [x] `third_party/miniz/miniz.h` and `third_party/miniz/miniz.c` vendored cleanly from public domain / MIT upstream.
- [x] `CMakeLists.txt` defines a `miniz` target and includes `third_party/miniz` in header search paths.
- [x] Build succeeds on MSVC and GCC without compiler warnings or symbol conflicts.
