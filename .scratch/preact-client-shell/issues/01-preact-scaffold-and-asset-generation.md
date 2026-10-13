# 01: Preact Scaffold, Inlined Bundler, and End-to-End Asset Generation

**What to build:** A standalone Preact + TypeScript frontend project bundled with Vite and `vite-plugin-singlefile`, paired with an automated Python asset generator script that converts the single-file inlined HTML bundle into chunked C++ raw string literals in `WebAssets.hpp`. When built, the C++ desktop runner serves the compiled Preact bundle over HTTP on localhost:8080.

**Blocked by:** None (can start immediately)

**Status:** resolved

- [x] `frontend/` directory is configured with Preact, TypeScript, Vite, and `vite-plugin-singlefile`
- [x] `npm run build` generates a single, fully inlined `dist/index.html` with zero external network or CDN references
- [x] `scripts/generate_web_assets.py` parses `frontend/dist/index.html` and writes `include/gamehub/web/WebAssets.hpp`
- [x] String literals in the generated `WebAssets.hpp` are chunked at <= 8 KB boundaries to prevent compiler limit errors
- [x] The generated `WebAssets.hpp` maintains the public C++ API (`getParts()`, `getIndexHtml()`)
- [x] Building C++ with CMake succeeds, and running `gamehub_desktop.exe` serves the Preact page at `http://localhost:8080` (or `8081`)
