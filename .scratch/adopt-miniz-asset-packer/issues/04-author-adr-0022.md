# 04: Author ADR 0022 and Update Documentation

**What to build:** Record Architecture Decision Record 0022 documenting the transition from hand-rolled compression to `miniz`, superseding ADR 0018. Update repository documentation and domain glossary where applicable.

**Blocked by:** 03: Excise Hand-Rolled GzipCompressor and Update Native Tests

**Status:** resolved

- [x] `docs/adr/0022-replace-hand-rolled-compression-with-miniz.md` authored with complete context, decision, and consequences.
- [x] `docs/adr/0018-cxx-asset-packer-and-gzip-compression.md` marked as superseded by ADR 0022.
- [x] `CONTEXT.md` updated to reflect `miniz` usage for asset packaging.
