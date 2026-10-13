# 01: Standalone C++ Asset Packer with In-Tree Gzip Compression

**What to build:** A standalone native C++17 command-line utility (`tools/asset_packer.cpp`) with an in-tree single-header Deflate/Gzip RFC 1952 compressor. It reads `frontend/dist/index.html`, compresses it in memory into a standard Gzip stream, and formats it as a contiguous C++ binary array in `include/gamehub/web/WebAssets.hpp` with size accessors and `isGzipped() = true`.

**Blocked by:** None (can start immediately)

**Status:** resolved

- [x] Tool compiles cleanly with MSVC, GCC, and Clang using standard C++17 with zero third-party package dependencies.
- [x] Gzip output stream begins with RFC 1952 magic bytes `0x1F, 0x8B`.
- [x] The generated C++ header is valid C++17, defining `getIndexHtmlData()`, `getIndexHtmlSize()`, `getIndexHtml()`, `getIndexHtmlView()`, and `isGzipped()`.
- [x] The tool exits with code 1 and a diagnostic error message if the input HTML file is missing.
