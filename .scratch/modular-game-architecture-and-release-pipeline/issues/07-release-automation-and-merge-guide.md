# 07: Release Automation & Branch Merge Guide Documentation

**What to build:**
Create release automation script and runbook documenting the GitHub Flow merge procedure from `dev` to `main`, validating Conventional Commits, reading from `VERSION`, and cutting an annotated git tag.

**Blocked by:** 06: Contract Legacy Paths & Enforce Engine Boundaries

**Status:** resolved

- [x] Provide `scripts/release.ps1` (or shell script) that verifies clean git status, updates `VERSION`, performs `--no-ff` merge to `main`, and creates annotated tag
- [x] Document release process in `docs/RELEASE.md`
