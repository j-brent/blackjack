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

- `lib/` — Pure logic library (no I/O). Public headers in `lib/include/blackjack/`.
- `cli/` — CLI frontend (`blackjack_cli`)
- `ftxui/` — TUI frontend (`blackjack_tui`)
- `tests/` — Catch2 test suite (`blackjack_tests`)

## Formatting

Run `clang-format` on all `.cpp` and `.hpp` files before committing. Use the project's `.clang-format` if present, otherwise: 4-space indent, K&R braces (consistent with `.dev-team/coding-standards.md`).
