# Mod Creation Guide (Current Build-Ready Scope)

This document describes what is **actually supported now** in the playable build-ready version.

## 1) Mods folder location

Mods are discovered from:

- `./mods/`

(relative to the game working directory, next to runtime files/output)

## 2) Supported mod path right now

- **Primary supported path:** folder-based mods
- Zip support is limited and metadata-oriented; folder mods are the reliable workflow for assets/data experimentation in the current phase.

## 3) Minimum folder structure

Example:

```text
mods/example_mod/
  mod.json
  assets/
  data/
```

## 4) `mod.json` format

Example:

```json
{
  "id": "example_mod",
  "name": "Example Mod",
  "version": "1.0.0",
  "description": "Example mod"
}
```

### Required fields (recommended for reliable discovery)

- `id`
- `name`
- `version`

### Optional/common fields

- `description`
- additional metadata fields may be ignored if not currently handled by phase-1 logic

## 5) `assets/` usage

Use `assets/` for folder-based resource overrides (for supported lookup paths in current runtime).

Typical pattern:

```text
mods/example_mod/assets/minecraft/textures/...
```

The runtime can prefer folder overrides over built-in assets for supported resources.

## 6) `data/` usage

Use `data/` for lightweight phase-1 data discovery paths.

Typical pattern:

```text
mods/example_mod/data/<namespace>/<category>/<file>.json
```

Current phase focuses on discovery/diagnostics and safe integration boundaries, not unrestricted gameplay code injection.

## 7) Working example layout

```text
mods/example_mod/
  mod.json
  assets/
    minecraft/
      textures/
        gui/
          (your replacement textures)
  data/
    example_mod/
      mechanics/
        example_rule.json
```

## 8) What is supported now

- folder mod discovery in `mods/`
- manifest parsing/validation for phase-1 metadata
- mod diagnostics surfaced in Mods UI
- folder-based asset override path (supported resources)
- lightweight data definition discovery under `data/`

## 9) What is NOT supported yet (honest limits)

- no full dynamic gameplay scripting API
- no general-purpose runtime code modding API
- no infinite-world rewrite here
- zip mods are not the main reliable authoring path in this stage
- compatibility expectations should remain phase-1/phase-2/phase-3 incremental

## 10) Practical recommendation

For this build-ready version, create and test mods as **folder-based mods** first, then validate via:

1. launch game
2. open Mods screen
3. press Refresh
4. check diagnostics list for any manifest/path issues
