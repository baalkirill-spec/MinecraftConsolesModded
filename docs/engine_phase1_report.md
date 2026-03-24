# Engine Phase 1 Report

## Visual Studio 2026 Preview / build workflow risks
- The root CMake build is still explicitly Windows-only and depends on prebuilt 4J binaries, Win32 APIs, and DirectX-era headers, so the safe path is still Visual Studio x64 with the existing Windows64 platform definitions.
- Source registration is manual through `Minecraft.Client/cmake/sources/Common.cmake`, so new `.cpp/.h` files must always be added there or they will silently disappear from the VS solution.
- The project relies on legacy `stdafx.h`/PCH conventions and a large set of platform macros; that makes "clean" refactors risky without incremental compile validation.

## FPS / render bottlenecks found
- `LevelRenderer::updateDirtyChunks()` walks the visible chunk set for every active local player when dirty work is pending, which makes wide invalidation bursts expensive.
- `LevelRenderer::setDirty()` was queueing every dirty chunk individually even for large invalidation areas, which can flood the lock-free dirty queue during lighting, chunk-border, and world-change updates.
- Cloud rendering already uses safer geometry than the legacy version, but it is still one of the heaviest visual passes because it renders cube-style cloud geometry instead of a tiny billboard mesh.
- Full debug overlay work in `Gui.cpp` is intentionally expensive because it gathers world, biome, lighting, chunk, and entity stats every frame.

## Incomplete / stubbed systems discovered
- Texture pack repository discovery is still mostly stubbed: `TexturePackRepository::updateList()`, `getIdOrNull()`, and `getWorkDirContents()` are effectively placeholders.
- `FileTexturePack` zip-backed resource loading is stubbed, so archive texture packs are not a complete user-facing system yet.
- `HttpTexture` is still effectively unimplemented, so web-backed texture loading is not a finished path.

## Large features that are not honestly "done"
- Java world import is not a small patch: the save format, chunk serialization, registries, and platform-specific world assumptions differ too much.
- Infinite world support is not a small patch: renderer bounds, save layout, networking, lighting caches, and chunk ownership logic all assume bounded console-style worlds.
- Full Minecraft-like modding for blocks/items/NPC/worldgen needs registries, resource reload hooks, data validation, and content pipeline changes far beyond a single stabilization pass.
