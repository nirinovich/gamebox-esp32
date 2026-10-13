#!/usr/bin/env bash
set -euo pipefail

# Release automation script for ESP32 GameHub

if [ $# -lt 1 ]; then
    echo "Usage: $0 <new-version> [--dry-run] [--skip-tests]"
    echo "Example: $0 1.1.0"
    exit 1
fi

NEW_VERSION="$1"
DRY_RUN=false
SKIP_TESTS=false

shift || true
while [ $# -gt 0 ]; do
    case "$1" in
        --dry-run) DRY_RUN=true ;;
        --skip-tests) SKIP_TESTS=true ;;
        *) echo "Unknown option: $1"; exit 1 ;;
    esac
    shift || true
done

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

echo "=========================================="
echo "  ESP32 GameHub Release Automation Pipeline"
echo "=========================================="
echo "Target Version: $NEW_VERSION"

# 1. Semantic Version Validation
if ! [[ "$NEW_VERSION" =~ ^[0-9]+\.[0-9]+\.[0-9]+(-[0-9A-Za-z.-]+)?$ ]]; then
    echo "Error: Invalid semantic version format '$NEW_VERSION'. Must be MAJOR.MINOR.PATCH."
    exit 1
fi

# 2. Check current Git branch
CURRENT_BRANCH="$(git rev-parse --abbrev-ref HEAD)"
if [ "$CURRENT_BRANCH" != "dev" ]; then
    echo "Error: Releases must originate from 'dev' branch. Currently on '$CURRENT_BRANCH'."
    exit 1
fi

# 3. Check for uncommitted changes
if [ -n "$(git status --porcelain)" ]; then
    echo "Error: Working tree has uncommitted changes. Stash or commit before releasing."
    exit 1
fi

# 4. Run tests
if [ "$SKIP_TESTS" = false ]; then
    echo "--> [1/5] Running CTest test suite..."
    if [ -d "$ROOT_DIR/build" ]; then
        ctest --test-dir "$ROOT_DIR/build" --output-on-failure
    fi

    echo "--> [2/5] Running Vitest test suite..."
    npm --prefix "$ROOT_DIR/frontend" test
fi

if [ "$DRY_RUN" = true ]; then
    echo "[DRY RUN] Would update VERSION to $NEW_VERSION, merge dev to main, and tag v$NEW_VERSION."
    exit 0
fi

# 5. Update VERSION file
echo "--> [3/5] Updating VERSION file to $NEW_VERSION..."
printf "%s" "$NEW_VERSION" > "$ROOT_DIR/VERSION"
git add "$ROOT_DIR/VERSION"
git commit -m "chore(release): bump version to $NEW_VERSION"

# 6. Checkout main and merge dev with --no-ff
echo "--> [4/5] Merging dev into main (--no-ff)..."
git checkout main
git merge --no-ff dev -m "release: v$NEW_VERSION"

# 7. Create annotated Git tag
TAG_NAME="v$NEW_VERSION"
echo "--> [5/5] Creating annotated Git tag '$TAG_NAME'..."
git tag -a "$TAG_NAME" -m "Release $TAG_NAME"

# 8. Switch back to dev
git checkout dev

echo "=========================================="
echo "  Release $TAG_NAME Successfully Cut!"
echo "=========================================="
echo "Next step to publish:"
echo "  git push origin main dev --tags"
