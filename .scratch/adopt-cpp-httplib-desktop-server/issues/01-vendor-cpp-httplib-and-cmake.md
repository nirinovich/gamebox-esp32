# 01: Vendor cpp-httplib and Wire CMake Build Targets

**What to build:** Vendor the single-header `httplib.h` library under `third_party/httplib/` and update `CMakeLists.txt` to add `third_party/httplib` to include directories for `gamehub_desktop` and `test_desktop_server`.

**Blocked by:** None (can start immediately)

**Status:** resolved

- [x] `third_party/httplib/httplib.h` vendored into repository (MIT license).
- [x] `CMakeLists.txt` includes `third_party/httplib` for desktop executable and test runner targets.
- [x] Project compiles cleanly with `httplib.h` accessible.
