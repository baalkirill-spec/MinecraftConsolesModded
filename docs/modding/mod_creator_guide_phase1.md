# Mod Creator Guide (Phase 1)

This guide documents **what works today** in the current C++ runtime mod path.

## 1) Where mods go

Place mods inside:

- `./mods/<your-mod-folder>/...`

The loader will create `mods/` automatically if needed.

## 2) Minimum working folder layout

Use folder mods as the primary supported runtime path:

```text
mods/
  your_mod/
    mod.json
    assets/
    data/
```

Legacy texture-only layouts are still recognized:

- `mods/your_mod/textures/...`
- `mods/your_mod/minecraft/...`

## 3) mod.json (recommended fields)

Recommended fields:

- `id` (lowercase, `[a-z0-9_.-]`)
- `name`
- `version`
- `description` (optional)
- `author` or `authors` (recommended)

If fields are missing, the loader falls back to folder/file-name defaults and reports warnings.

## 4) Asset overrides

Supported phase-1 pattern:

- Put override assets under `assets/minecraft/...` (recommended).
- The runtime also accepts common query variants and normalizes `res/`, `assets/`, and `minecraft/` prefixes.

## 5) Data definitions

Supported discovery root:

- `data/<namespace>/<category>/.../*.json`

Supported categories:

- `blocks`
- `items`
- `entities`
- `mechanics`
- `world_rules`

Only lightweight discovery/validation is currently guaranteed; this is not a full gameplay registry system yet.

## 6) Debugging invalid mods

Open the in-game Mods screen and press **Refresh Mods**.

The screen now shows recent scan diagnostics (warnings/errors), and each loaded mod reports warning/error counts.

## 6.1) Duplicate mod ids and precedence

- Keep `id` unique across all mods.
- If a zip metadata mod and a folder mod share the same id, the folder mod is preferred (folder-first runtime path).
- For duplicate data definition ids, the first discovered definition is kept for deterministic phase-1 behaviour, and later duplicates are reported as warnings.

## 7) Honest limitations

Not currently supported:

- runtime zip asset mounting
- full dynamic block/entity/gameplay registry parity
- automatic full API compatibility promises
