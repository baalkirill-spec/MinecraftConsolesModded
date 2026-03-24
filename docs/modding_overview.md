# Modding Overview

This project currently supports **phase-1/phase-2 style folder mods** as the primary runtime path.

It does **not** currently provide a full Minecraft-Java-style registry/mod-loader stack, and it does **not** yet support complete Forge/Fabric-like gameplay injection.

## Supported right now

### 1. Folder-based mods

Create a folder under `mods/`:

```text
mods/
  example_mod/
    mod.json
    assets/
      minecraft/
        textures/
          gui/
            icons.png
    data/
      example_mod/
        items/
          copper_hammer.json
        mechanics/
          stamina_rules.json
```

Folder mods are the main runtime-supported format.

### 2. `mod.json` metadata

The loader currently recognizes these fields:

* `id` - recommended unique mod id. Lowercase letters, numbers, `_`, `-`, and `.` are supported.
* `name` - display name.
* `version` - display version string.
* `description` - short description.
* `author` - single author string.
* `authors` - array of author strings.

If `id` is missing, the loader falls back to the folder/zip file name and logs a warning.

### 3. Asset overrides

Folder mods can override existing client assets by placing files in one of these practical layouts:

```text
mods/example_mod/textures/...
mods/example_mod/minecraft/textures/...
mods/example_mod/assets/minecraft/...
mods/example_mod/assets/minecraft/textures/...
```

At runtime, folder mods are used for **builtin asset fallback overrides**.

Active texture packs still take precedence first. After that, folder mod overrides are checked, and then builtin/default resources are used as fallback.

### 4. Data-definition discovery foundation

Folder mods can now ship simple JSON definition files under:

```text
mods/<mod>/data/<namespace>/blocks/*.json
mods/<mod>/data/<namespace>/items/*.json
mods/<mod>/data/<namespace>/entities/*.json
mods/<mod>/data/<namespace>/mechanics/*.json
mods/<mod>/data/<namespace>/world_rules/*.json
```

These definitions are:

* discovered during mod scan
* validated structurally
* assigned canonical ids from their path
* stored in the mod manager for future runtime systems

This is **real groundwork**, not a claim that custom blocks/items/entities are fully implemented yet.

## Supported vs unsupported

### Supported

* folder mod discovery
* `mod.json` metadata loading
* folder-based asset overrides
* custom skins via `skins/`
* JSON data-definition discovery for future gameplay/data systems
* diagnostics for invalid or partially broken folder mods

### Experimental / limited

* zip mods are metadata-only right now
* zip runtime asset mounting is not a stable supported path yet
* data-definition files are discovered and validated, but they are **not** yet automatically wired into world save formats, rendering registries, item ids, placement logic, spawning logic, or AI behavior

### Not supported yet

* full registry-based modding
* custom blocks that fully place/render/save/network correctly
* custom items with complete runtime ids and inventory integration
* full NPC/entity behavior injection
* Java world import
* infinite world support

## Runtime loading notes

* Folder mods remain the priority path for runtime use.
* Zip mods should be treated as discovery/metadata-only unless runtime mounting is deliberately expanded in later work.
* Mod ids must be unique; duplicate ids are skipped to avoid undefined asset/data conflicts.
* Invalid data definition files are skipped with diagnostics instead of crashing the loader.

## Future extension points

The current data foundation is intentionally scoped so later work can attach real systems to:

* `blocks`
* `items`
* `entities`
* `mechanics`
* `world_rules`

Those hooks are **not** a finished registry system yet, but the on-disk structure is now defined and validated.
