#Requires -Version 5.1
<#
.SYNOPSIS
    Automated release script for ESP32 GameHub.
.DESCRIPTION
    Verifies working tree cleanliness, validates semantic versioning, updates the VERSION file,
    executes CTest and Vitest test suites, performs a non-fast-forward merge from dev to main,
    and cuts an annotated Git release tag.
.PARAMETER NewVersion
    The semantic version string to release (e.g., 1.1.0).
.PARAMETER DryRun
    Simulate the release workflow without modifying git branches or tags.
.PARAMETER SkipTests
    Skip automated test execution before merging.
#>

[CmdletBinding()]
param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$NewVersion,

    [switch]$DryRun,
    [switch]$SkipTests
)

$ErrorActionPreference = "Stop"

# Ensure we are at project root
$rootDir = (Resolve-Path "$PSScriptRoot/..").Path
Set-Location $rootDir

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "  ESP32 GameHub Release Automation Pipeline" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "Target Version: $NewVersion" -ForegroundColor Yellow

# 1. Semantic Version Validation
if ($NewVersion -notmatch '^\d+\.\d+\.\d+(-[0-9A-Za-z.-]+)?$') {
    Write-Error "Invalid semantic version format: '$NewVersion'. Must be MAJOR.MINOR.PATCH (e.g. 1.1.0 or 1.2.0-rc1)."
}

# 2. Check current Git branch
$currentBranch = (git rev-parse --abbrev-ref HEAD).Trim()
if ($currentBranch -ne "dev") {
    Write-Error "Releases must originate from the 'dev' branch. Currently on '$currentBranch'."
}

# 3. Check for uncommitted changes (excluding VERSION if already edited)
$gitStatus = (git status --porcelain).Trim()
if ($gitStatus) {
    Write-Error "Working tree has uncommitted changes. Please stash or commit before running release:`n$gitStatus"
}

# 4. Run automated test suites
if (-not $SkipTests) {
    Write-Host "`n--> [1/5] Running CTest test suite..." -ForegroundColor Green
    if (Test-Path "$rootDir/build") {
        $cmd = Get-Command ctest -ErrorAction SilentlyContinue
        $ctestExe = if ($cmd) { $cmd.Source } else { $null }
        if (-not $ctestExe) {
            $ctestExe = "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\ctest.exe"
        }
        if (Test-Path $ctestExe) {
            & $ctestExe --test-dir "$rootDir/build" -C Debug --output-on-failure
            if ($LASTEXITCODE -ne 0) {
                Write-Error "CTest test suite failed. Aborting release."
            }
        } else {
            Write-Warning "ctest executable not found. Skipping CTest execution."
        }
    }

    Write-Host "`n--> [2/5] Running Vitest test suite..." -ForegroundColor Green
    npm --prefix "$rootDir/frontend" test
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Vitest test suite failed. Aborting release."
    }
} else {
    Write-Host "`n--> Skipping automated test execution (-SkipTests enabled)." -ForegroundColor Yellow
}

if ($DryRun) {
    Write-Host "`n[DRY RUN] Would update VERSION to $NewVersion, merge dev to main, and tag v$NewVersion." -ForegroundColor Magenta
    exit 0
}

# 5. Update VERSION file
Write-Host "`n--> [3/5] Updating VERSION file to $NewVersion..." -ForegroundColor Green
Set-Content -Path "$rootDir/VERSION" -Value $NewVersion -NoNewline
git add "$rootDir/VERSION"
git commit -m "chore(release): bump version to $NewVersion"

# 6. Checkout main and merge dev with --no-ff
Write-Host "`n--> [4/5] Merging dev into main (--no-ff)..." -ForegroundColor Green
git checkout main
try {
    git merge --no-ff dev -m "release: v$NewVersion"
} catch {
    Write-Error "Merge conflict occurred while merging dev into main. Please resolve manually."
}

# 7. Create annotated Git tag
$tagName = "v$NewVersion"
Write-Host "`n--> [5/5] Creating annotated Git tag '$tagName'..." -ForegroundColor Green
git tag -a $tagName -m "Release $tagName"

# 8. Switch back to dev
git checkout dev

Write-Host "`n==========================================" -ForegroundColor Green
Write-Host "  Release $tagName Successfully Cut!" -ForegroundColor Green
Write-Host "==========================================" -ForegroundColor Green
Write-Host "Next step to publish:" -ForegroundColor Yellow
Write-Host "  git push origin main dev --tags" -ForegroundColor White
