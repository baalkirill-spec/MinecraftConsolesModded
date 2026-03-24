# Modding Phase 3 (Narrow Scope) Changelog

Date: 2026-03-24

## Summary of changes

- Improved folder-mod diagnostics during `mods/` scanning.
- Added stricter `mod.json` validation warnings for missing `name`, `version`, and author fields.
- Improved folder override path normalization for `assets/...`, `assets/minecraft/...`, and `minecraft/...` query variants.
- Added example phase-1 mod structure under `examples/mods/phase1-example`.

## Regression checklist

- [ ] Game still starts with an empty `mods/` folder.
- [ ] `mods/` folder is auto-created if missing.
- [ ] Folder mods with `mod.json` still load.
- [ ] Folder mods using `assets/minecraft/...` texture overrides still resolve.
- [ ] `data/<namespace>/<category>/.../*.json` definitions still scan and validate.
- [ ] Zip mods remain metadata-only (no runtime zip asset mounting).
- [ ] Mods screen can refresh without crashing.

## Supported vs unsupported (honest status)

### Supported in phase-1/phase-2 runtime path

- Folder-based mods in `mods/`.
- `mod.json` metadata parsing and diagnostics.
- Folder asset override lookup for textures/files resolved through the existing texture loader path.
- Basic discovery/validation of JSON definitions under `data/<namespace>/<category>/...`.
- Mods screen listing discovered mods and scan diagnostics.

### Explicitly not supported yet

- Runtime zip asset mounting.
- Full gameplay registries (new blocks/items/entities with runtime behavior wiring).
- Java world import.
- Infinite world migration work.
- "Full mod API" claims.
