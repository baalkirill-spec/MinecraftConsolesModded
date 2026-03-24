# Java Port (Phase 1)

Это не автоматический перевод C++ в Java.

Это отдельная, поэтапная Java-основа проекта, которая сохраняет ключевые идеи текущей архитектуры:
- game loop
- options/settings
- FPS overlay
- screens/UI
- modding layout (`mods/`, `mod.json`, `assets/`, `data/`)

## Технология

Используется **LibGDX (LWJGL3 desktop backend)**.

Почему так:
- проще быстро поднять рабочее окно и game loop
- есть готовые Java-экосистемные инструменты для UI, rendering и input
- не нужно тащить C++-style memory/pointer patterns в Java

## Что уже работает

- desktop launcher
- базовое окно и LibGDX game loop
- title menu screen
- options screen
- FPS overlay
- сохранение/загрузка `options.properties`
- Java `ModManager` для phase-1 folder mods
- моды в `runtime/mods`
- screen для просмотра и refresh модов
- headless-check режим для CI/терминальной проверки без окна

## Как запустить

```bash
cd java-port
gradle run
```

## Как проверить без GUI

```bash
cd java-port
gradle run --args='--headless-check'
```

## Структура

- `org.minecraftport.core` — основной game class
- `org.minecraftport.rendering` — FPS overlay и рендер-вспомогалки
- `org.minecraftport.ui` — экраны/menu UI
- `org.minecraftport.options` — настройки и window presets
- `org.minecraftport.mods` — Java phase-1 mod manager
- `runtime/` — локальные runtime-файлы Java-port (`options.properties`, `mods/`)

## Чего пока нет

- реального world/gameplay port
- полноценного renderer parity с C++ клиентом
- input parity с текущим C++ кодом
- network/multiplayer port
- полного mod API
- автоматического использования существующих C++ assets/render backends
