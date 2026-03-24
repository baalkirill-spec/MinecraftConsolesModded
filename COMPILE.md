# Build Guide (Windows / Visual Studio / CMake)

This repository is designed to be built on **Windows x64**.

## 1) Requirements

- **Visual Studio 2022** (Desktop C++ workload)
- **CMake** (version compatible with presets; CMake 3.25+ recommended)
- **Windows SDK** (installed via Visual Studio)
- A normal clone of this repository with subfolders intact

> Recommended shell: **Developer PowerShell for VS 2022**.

---

## 2) Configure the project (CMake preset)

From repo root:

```powershell
cmake --preset windows64
```

This creates build files under:

- `build/windows64/`

---

## 3) Build targets

### Client (Debug)

```powershell
cmake --build --preset windows64-debug --target Minecraft.Client
```

### Client (Release)

```powershell
cmake --build --preset windows64-release --target Minecraft.Client
```

### Dedicated Server (optional)

```powershell
cmake --build --preset windows64-debug --target Minecraft.Server
cmake --build --preset windows64-release --target Minecraft.Server
```

---

## 4) Visual Studio workflow

1. Open repo folder in Visual Studio (CMake project mode).
2. Wait for CMake configure to finish.
3. Select preset/configuration:
   - `Windows64 - Debug` or
   - `Windows64 - Release`
4. Select startup target:
   - `Minecraft.Client`
5. Build/Run:
   - `Build > Build Solution`
   - `Debug > Start Debugging (F5)`

---

## 5) Output binaries

Typical output paths:

- Client Debug: `build/windows64/Minecraft.Client/Debug/Minecraft.Client.exe`
- Client Release: `build/windows64/Minecraft.Client/Release/Minecraft.Client.exe`
- Server Debug: `build/windows64/Minecraft.Server/Debug/Minecraft.Server.exe`

Run from the output directory so relative asset paths resolve correctly.

---

## 6) Run and quick validation checklist

After launching `Minecraft.Client.exe`, verify:

1. **Launcher screen appears first**.
2. Enter valid player name (3-16, letters/numbers/underscore), press **Start Game**.
3. Exit and relaunch: player name should be loaded from `options.txt`.
4. Main menu opens and buttons work (Singleplayer/Multiplayer/Options/Quit/Mods).
5. FPS overlay can be toggled in video/settings and appears in-game/UI.
6. `mods/` folder exists and Mods screen can refresh/discover folder mods.
7. `skins/` folder exists and custom skin path can be used.

---

## 7) Notes and scope

- This build path targets **Windows64** for playable validation.
- Launcher is currently an **integrated launcher screen** (inside `Minecraft.Client`), not a separate launcher EXE.
- Folder-based mods are the primary supported mod path (see modding docs).
