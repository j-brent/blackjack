# Blackjack Project

## Build & Test

```sh
# Configure (once)
cmake -B build -S . -G Ninja

# Build all targets
cmake --build build

# Run tests
./build/tests/blackjack_tests.exe
```

## Source Layout

- `blackjack/` — Pure logic library (no I/O). Headers and sources together (P1204 style).
- `apps/cli/` — CLI frontend (`blackjack_cli`)
- `apps/ftxui/` — TUI frontend (`blackjack_tui`)
- `apps/electron/` — Desktop app (Electron wrapper of `apps/web/`). Run with `cd apps/electron && npm start`.
- `tests/` — Catch2 test suite (`blackjack_tests`)

## Formatting

Run `clang-format` on all `.cpp` and `.hpp` files before committing. Use the project's `.clang-format`.

A pre-commit hook auto-formats staged C++ files. Configure once per clone:

```sh
git config core.hooksPath .githooks
git config blame.ignoreRevsFile .git-blame-ignore-revs
```
