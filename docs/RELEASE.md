# Release Protocol & Versioning Guide

This document establishes the official branching model, release lifecycle, and automated tagging pipeline for the **ESP32 GameHub** project.

---

## 1. Branching Model (GitHub Flow)

The repository maintains two primary branches:

| Branch | Role | Deployment / Status |
| :--- | :--- | :--- |
| `main` | Production-ready, stable releases. Every commit on `main` is tagged with an annotated release tag (`vX.Y.Z`). | Flashed to production hardware, deployed as stable artifacts. |
| `dev` | Integration trunk for new games, engine enhancements, and refactors. Feature slices branch off `dev` and merge back into `dev`. | Local development, CI test execution, pre-release staging. |

### Golden Rules
1. **Never commit directly to `main`**. All changes arrive on `main` via a non-fast-forward merge (`--no-ff`) from `dev`.
2. **Releases originate from `dev`**. `dev` must be clean, with 100% passing tests (CTest + Vitest), before a release is cut.
3. **Every release is tagged**. Releases on `main` are tracked with annotated git tags: `v<MAJOR>.<MINOR>.<PATCH>`.

---

## 2. Versioning Specification (SemVer)

GameHub strictly follows [Semantic Versioning 2.0.0](https://semver.org/):

$$\text{Version} = \text{MAJOR}.\text{MINOR}.\text{PATCH}$$

- **MAJOR** (`+1.0.0`): Incompatible API or architectural breaking changes (e.g., protocol rewrite, breaking `class Game` interface changes, backward-incompatible hardware abstraction).
- **MINOR** (`0.+1.0`): Backwards-compatible feature additions (e.g., adding a new game slice under `games/<game>`, new rendering engine capabilities, sound engine upgrades).
- **PATCH** (`0.0.+1`): Backwards-compatible bug fixes, performance optimizations, memory tuning, or UI polish.

### The Single Source of Truth: `VERSION`
The root file [`VERSION`](../VERSION) contains the plain-text semantic version string (e.g. `1.0.0`). Both build systems consume this file at configure time:
- **CMake**: Reads `VERSION` and sets CMake project version and `-DGAMEBOX_VERSION="1.0.0"`.
- **PlatformIO / ESP32**: Consumes `GAMEBOX_VERSION` via build flags.
- **Frontend / Vite**: Injected into client bundle for lobby footer and diagnostic display.

---

## 3. Commit Convention (Conventional Commits)

All commits on `dev` and feature branches must adhere to the Conventional Commits specification:

```
<type>(<scope>): <short summary>

[optional body]

[optional footer(s)]
```

### Types
- `feat`: A new feature or game module (e.g., `feat(domino): add boneyard and pip-sum logic`).
- `fix`: A bug fix (e.g., `fix(host): resolve memory leak in session disconnect`).
- `refactor`: Code reorganization with zero functional changes (e.g., `refactor(games): migrate pong to vertical slice`).
- `perf`: Performance improvement (e.g., `perf(snake): eliminate heap allocations during tick`).
- `test`: Adding or refactoring tests (e.g., `test(battleship): verify fog-of-war masking`).
- `docs`: Documentation updates (e.g., `docs(adr): document ADR-0018 modular architecture`).
- `chore`: Maintenance tasks or version bumps (e.g., `chore(release): bump version to 1.1.0`).

---

## 4. Automated Release Pipeline

We provide cross-platform scripts to automate the end-to-end release procedure:

### PowerShell (Windows)
```powershell
# Dry run preview
.\scripts\release.ps1 1.1.0 -DryRun

# Execute release
.\scripts\release.ps1 1.1.0
```

### Bash (macOS / Linux)
```bash
# Dry run preview
./scripts/release.sh 1.1.0 --dry-run

# Execute release
./scripts/release.sh 1.1.0
```

### What the Release Script Does:
1. **Validates Working Directory**: Asserts no untracked or uncommitted changes exist.
2. **Validates Branch**: Asserts execution is taking place on `dev`.
3. **Runs Test Gates**:
   - Executes CTest (15+ native unit and integration tests across Host Engine and game modules).
   - Executes Vitest (24+ client tests across Preact shell and Canvas 2D renderers).
4. **Updates `VERSION`**: Writes the target version string to [`VERSION`](../VERSION) and commits `chore(release): bump version to X.Y.Z`.
5. **Performs Non-Fast-Forward Merge**: Switches to `main` and runs `git merge --no-ff dev -m "release: vX.Y.Z"`.
6. **Creates Annotated Tag**: Runs `git tag -a vX.Y.Z -m "Release vX.Y.Z"`.
7. **Switches Back to `dev`**: Restores the working branch to `dev` for continued development.

---

## 5. Publishing Releases

Once the release script finishes, publish the branch and tags to the remote repository:

```bash
git push origin main dev --tags
```
