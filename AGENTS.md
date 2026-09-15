# VeinMiner Development Guide

This file helps contributors and AI coding agents understand this Endstone C++ plugin.

Endstone docs: https://endstone.dev/latest/

## Structure

Each header owns exactly one responsibility:

- `include/vein_miner_config.h` — `GlobalConfig` data and its load/save to `config.toml`
- `include/vein_miner_state.h` — per-player runtime overrides, held only in memory
- `include/vein_miner_service.h` — the flood-fill vein algorithm and durability handling
- `include/vein_miner_listener.h` — `BlockBreakEvent` handling and activation-mode gating
- `include/vein_miner_form_response.h` — parses a `ModalForm` submission string into typed values
- `include/vein_miner_form.h` — builds the global and personal settings forms, using the parser above
- `include/vein_miner_plugin.h` — plugin lifecycle and `/miner` command dispatch only
- `src/vein_miner_plugin.cpp` — `ENDSTONE_PLUGIN` metadata: commands and permissions

Do not merge responsibilities across these files. If a change needs both config storage and
form UI, touch both files rather than moving logic into one.

## Conventions

- C++20, header-only implementations except `vein_miner_plugin.cpp`
- No inline comments; keep names descriptive instead
- `.clang-format` at the repo root defines formatting

## Building

```bash
cmake -B build
cmake --build build
```

See `README.md` for full platform prerequisites.
