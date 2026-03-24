# Example Mod Layout

## Minimal metadata-only mod

```text
mods/
  hello_metadata/
    mod.json
```

Example `mod.json`:

```json
{
  "id": "hello_metadata",
  "name": "Hello Metadata",
  "version": "0.1.0",
  "description": "A simple example mod folder.",
  "author": "Example Creator"
}
```

## Texture override mod

```text
mods/
  brighter_gui/
    mod.json
    assets/
      minecraft/
        textures/
          gui/
            icons.png
```

This kind of mod can override builtin textures after active texture-pack lookup and before builtin fallback resources.

## Data-foundation example mod

```text
mods/
  gameplay_foundation/
    mod.json
    data/
      gameplay_foundation/
        items/
          copper_hammer.json
        mechanics/
          stamina_rules.json
        world_rules/
          harsher_nights.json
```

Example `copper_hammer.json`:

```json
{
  "id": "gameplay_foundation:copper_hammer",
  "display_name": "Copper Hammer",
  "tier": 1
}
```

Example `stamina_rules.json`:

```json
{
  "id": "gameplay_foundation:stamina_rules",
  "enabled": true,
  "regen_delay_ticks": 60
}
```

## Current reality check

These files are currently:

* discovered
* validated as small JSON objects
* assigned canonical ids from their `data/<namespace>/...` path
* stored for future gameplay/runtime systems

They are **not yet** automatically turned into fully working new blocks, items, mobs, or game rules by themselves.

## Recommended practices for mod creators

* Prefer folder mods over zip mods for now.
* Keep ids lowercase.
* Use one unique namespace per mod.
* Match any optional `"id"` field to the canonical path-based id.
* Put gameplay/data experiments under `data/<namespace>/...`.
* Put runtime texture replacements under `assets/minecraft/...`.
