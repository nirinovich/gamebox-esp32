# 01: Build System Multi-Root Support & Single-Source Versioning

**What to build:**
Enable CMake, PlatformIO, and Vite/TypeScript to compile and import code from the new `games/` root directory alongside legacy paths, and consume the single-source `VERSION` file. All existing builds and tests remain green.

**Blocked by:** None (can start immediately)

**Status:** resolved

- [x] CMake reads version from `VERSION` file and defines `GAMEBOX_VERSION`
- [x] CMake includes `games/` in header search path
- [x] Frontend `vite.config.ts` and `tsconfig.json` configure `@games/*` path alias pointing to `../games/*/client`
- [x] Existing test suites (`ctest` and `npm test`) pass completely
