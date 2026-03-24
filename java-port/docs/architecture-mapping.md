# Architecture Mapping: C++ -> Java Port (Phase 1)

## Цель

Не переводить C++ синтаксис в Java 1:1, а перенести архитектурные роли системы.

## Mapping

### Core
- C++: `Minecraft.cpp`, `Minecraft.h`
- Java: `org.minecraftport.core.MinecraftPortGame`

### UI / Screens
- C++: `Screen.*`, `TitleScreen.*`, `OptionsScreen.*`, `VideoSettingsScreen.*`, `ModsScreen.*`
- Java: `org.minecraftport.ui.*`

### Options
- C++: `Options.*`
- Java: `org.minecraftport.options.CoreOptions`, `WindowPreset`

### Mods
- C++: `ModManager.*`, `ModInfo.*`, `SimpleZipReader.*`
- Java: `org.minecraftport.mods.*`

### Rendering helpers
- C++: `Gui.cpp`, FPS overlay paths in `Minecraft.cpp`
- Java: `org.minecraftport.rendering.FpsOverlay`, `org.minecraftport.util.BackgroundRenderer`

## Phase 1 scope

Переносим только:
- shell/game loop
- настройки
- FPS overlay
- главное меню
- ModManager phase-1

## Next phases

1. Extract gameplay state container
2. Port asset registry / texture lookup abstraction
3. Port input mapping layer
4. Port basic world renderer
5. Port gameplay systems incrementally
