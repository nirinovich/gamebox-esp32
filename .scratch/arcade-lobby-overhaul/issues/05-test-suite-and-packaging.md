# Issue 05: Test Suite and Packaging

Status: closed
Type: task

## Description

Verify and package all changes:
1. Write frontend unit tests covering:
   - Untaken callsign assignment on default/empty/Guest.
   - 1-tap callsign shuffle cycling through pool.
   - Quick Match button launching solo game.
   - Cartridge rack action triggering play/host.
   - Room code input functionality across tabs.
   - Match feed collapsing when directory is empty.
2. Build single-file bundle via `npm run build` in `frontend/`.
3. Package bundle using `.\build\Debug\asset_packer.exe frontend\dist\index.html include\gamehub\web\WebAssets.hpp`.
4. Run C++ test suite via `ctest --test-dir build --output-on-failure`.
5. Verify live server responsiveness on `http://localhost:8081`.
