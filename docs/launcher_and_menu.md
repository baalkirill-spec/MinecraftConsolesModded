# Launcher and Main Menu (Integrated)

## Why this is an integrated launcher

For this run, the safest approach was an **in-game launcher screen** that appears before the main menu.
This keeps the existing Visual Studio + CMake startup path unchanged (single executable) while still adding a real launcher flow with persistent player-name configuration.

## How it works

1. Game starts normally via `Minecraft.Client`.
2. Startup now opens `LauncherScreen` first.
3. Player enters a name and presses **Start Game**.
4. The launcher validates the name and stores it in options.
5. The game proceeds to the redesigned `TitleScreen`.

## Player name storage

Player name is stored in `options.txt` as:

- `playerName:<value>`

Validation rules:

- non-empty (enforced by length)
- 3 to 16 characters
- allowed characters: `A-Z`, `a-z`, `0-9`, `_`

Invalid or unsafe values are normalized to `Player` when loading/saving.

## How to start the game

Use the same current workflow:

- Configure/build with CMake for Windows64 (Visual Studio generator).
- Run `Minecraft.Client`.
- Enter a name in the launcher and press **Start Game**.

## Main menu structure (Java-era inspired)

The title menu keeps this project's existing navigation architecture, but updates layout to feel closer to the classic Java-era flow:

- centered button stack
- top-level primary actions:
  - Singleplayer
  - Multiplayer
  - Options
  - Quit
- Mods button remains available
- cleaner spacing and panelized composition

## Limitations

- This is **inspired by** Minecraft Java-era menu style, not an exact byte-perfect 1.13.5 recreation.
- Folder-open shortcuts (open game/mods/skins folders) were deferred to keep this pass low-risk and cross-platform safe.
