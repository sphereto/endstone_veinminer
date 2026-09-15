# VeinMiner

[![Build](https://github.com/sphereto/endstone_veinminer/actions/workflows/build.yml/badge.svg)](https://github.com/sphereto/endstone_veinminer/actions/workflows/build.yml)

Break one ore or log and the whole connected vein comes with it. Built for
[Endstone](https://github.com/EndstoneMC/endstone), a plugin framework for Minecraft Bedrock
Dedicated Server.

## Features

- Breaks all directly connected blocks of the same type when the origin block is broken
- Three activation modes, chosen server-wide or per player: sneak to activate, always active, or a toggle command
- Server-wide settings and personal settings both live behind a single `/miner config` command, using an in-game form
- Configurable block whitelist (ores and logs by default), max blocks per vein, and max search radius
- Configurable base-use permission default (everyone or operators only)
- Respects tool durability and Unbreaking on the extra blocks in a vein

## Commands

| Command | Description |
|---|---|
| `/miner` | Shows a short status message, or toggles vein mining on/off if your active mode is "Toggle Command" |
| `/miner config` | Opens the settings form. Operators with `veinminer.config` see the full server-wide form; everyone else sees a personal-only form |
| `/miner reload` | Reloads `config.toml` from disk (requires `veinminer.reload`) |

## Permissions

| Permission | Default | Description |
|---|---|---|
| `veinminer.use` | Configurable (everyone or operators) via `/miner config` | Allows breaking connected veins |
| `veinminer.config` | Operators | Allows editing the server-wide configuration |
| `veinminer.reload` | Operators | Allows reloading the configuration from disk |

## Known Limitations

The Endstone C++ SDK does not currently expose block loot tables, so drops for the extra blocks
in a vein are the block's own item form rather than a fully vanilla-accurate drop (for example,
Fortune-scaled raw ore quantities are not modeled). The origin block you actually click still
breaks and drops through the normal game engine, so only the *additional* vein blocks are
affected by this simplification.

## Building

### Prerequisites

**Windows:** [Visual Studio](https://visualstudio.microsoft.com/) 2019 or newer with the
"Desktop development with C++" workload.

**Linux:** Clang 15+ with libc++.

```bash
sudo apt-get install -y cmake ninja-build
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 18
sudo apt-get install -y libc++-18-dev libc++abi-18-dev
```

### Build

```bash
git clone https://github.com/sphereto/endstone_veinminer.git
cd endstone_veinminer
```

**Windows:**
```bash
cmake -B build
cmake --build build --config Release
```

**Linux:**
```bash
CC=clang-18 CXX=clang++-18 cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Install on a Server

Copy the output binary to your server's `plugins/` folder:
- **Windows:** `build/Release/endstone_veinminer.dll`
- **Linux:** `build/endstone_veinminer.so`

Restart the server to load the plugin.

## Releasing

1. Add your changes under `## [Unreleased]` in `CHANGELOG.md`
2. Go to **Actions > Release > Run workflow**
3. Enter the version (e.g. `1.0.1`) and run

The workflow validates the version, updates the changelog, creates a git tag and GitHub release,
builds for Windows and Linux, and attaches `endstone_veinminer-vX.Y.Z.dll`/`.so` to the release.

## License

[MIT License](LICENSE)
