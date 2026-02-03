# Blackjack

C++20 blackjack game engine with four frontends, built as a shared-library architecture exercise.

## Architecture

The library (`lib/`) is a passive state machine with no I/O, no exceptions, and no callbacks. It exposes commands (`deal`, `hit`, `stand`, `split`, `play_dealer`) that return `ActionResult` enums and query methods that return current state. Frontends drive the game loop.

```
blackjack/
├── lib/        Pure logic library — public headers in lib/include/blackjack/
├── cli/        Text-based CLI frontend
├── ftxui/      Terminal UI frontend (FTXUI)
├── web/        Browser frontend (Emscripten/WASM + vanilla JS)
├── electron/   Desktop app (Electron wrapper of web/)
└── tests/      Catch2 test suite (353 assertions, 22 test cases)
```

## Build

Requires CMake 3.20+, a C++20 compiler (GCC 12+, Clang 15+, or MSVC 2022), and Ninja.

```sh
cmake -B build -S . -G Ninja
cmake --build build
./build/tests/blackjack_tests.exe
```

### Frontends

The web and Electron frontends build and run separately from the main CMake project. See [docs/frontends.md](docs/frontends.md) for build instructions, source layouts, and architecture notes for all four frontends.

## API

All types live in the `blackjack` namespace. The primary interface is the `Game` class.

```cpp
blackjack::Game game;       // random seed
blackjack::Game game(42);   // deterministic seed (for testing)

game.deal();

while (game.state() == blackjack::GameState::PlayerTurn) {
    auto actions = game.available_actions();
    // ... render, get player choice ...
    game.hit();   // or game.stand(), game.split()
}

game.play_dealer();

for (const auto& hs : game.player_hands()) {
    // hs.result is Win, Lose, Push, or Blackjack
}
```

Commands return `ActionResult::Success` or an error variant (`InvalidAction`, `InvalidSplit`, `AlreadySplit`). Query methods are always safe to call in any state.

## Error handling

The library is compiled with `-fno-exceptions` on GCC/Clang. Functions that perform no heap allocation are marked `noexcept`. See the [API contracts](.dev-team/api-contracts.md#7-error-handling-contract) for the full `noexcept` contract and allocation failure behavior.

## Formatting

A pre-commit hook auto-formats staged C++ files. Configure once per clone:

```sh
git config core.hooksPath projects/blackjack/.githooks
git config blame.ignoreRevsFile projects/blackjack/.git-blame-ignore-revs
```

## Static analysis

clang-tidy is opt-in:

```sh
cmake -B build -S . -G Ninja -DBLACKJACK_CLANG_TIDY=ON
cmake --build build
```
