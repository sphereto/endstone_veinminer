# Contributing

Thanks for your interest in improving VeinMiner!

## Development Setup

### Windows
- Visual Studio 2019 or newer
- CMake 3.15+

### Linux
- Clang 15+ with libc++
- CMake 3.15+

```bash
git clone https://github.com/sphereto/endstone_veinminer.git
cd endstone_veinminer
cmake -B build
cmake --build build
```

## Making Changes

1. Create a branch for your changes
2. Ensure the project builds on at least one platform
3. Update `CHANGELOG.md` under `## [Unreleased]` if your change is user-facing
4. Open a pull request with a clear description of what changed and why

## Code Style

- One responsibility per file: config, state, service, listener, form, plugin lifecycle each stay in their own header
- No inline comments; keep code self-explanatory through naming
- Use the `.clang-format` configuration if present for consistent formatting

## Reporting Issues

Use [GitHub Issues](https://github.com/sphereto/endstone_veinminer/issues) for bugs and feature requests.
