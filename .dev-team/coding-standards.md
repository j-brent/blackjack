<!-- metadata
id: STD-001
type: coding-standards
status: draft
author: A3-tech-lead
created: 2026-01-30
updated: 2026-01-30
approved-by: pending
dependencies: [ARCH-001]
-->

# Coding Standards: Blackjack C++20 Project

## 1. Language & Compiler

- **Standard**: C++20.
- **Target compilers**: GCC 12+, Clang 15+, MSVC 19.30+ (Visual Studio 2022).
- **Warning flags**:
  - GCC/Clang: `-Wall -Wextra -Wpedantic`
  - MSVC: `/W4`
- **Zero warnings** in library code (`lib/`). Frontends should strive for zero warnings but are not blocked by third-party header warnings (e.g., FTXUI).
- If `std::format` is unavailable on a target compiler, frontends fall back to streams or `snprintf`. The library does not format strings.

## 2. Project Structure

Follow the directory layout defined in ARCH-001:

```
blackjack/
├── CMakeLists.txt
├── lib/
│   ├── CMakeLists.txt
│   ├── include/blackjack/   ← public headers
│   └── src/                 ← implementation files
├── cli/
│   ├── CMakeLists.txt
│   └── src/
├── ftxui/
│   ├── CMakeLists.txt
│   └── src/
└── tests/
    ├── CMakeLists.txt
    └── src/
```

- **One class per header/source pair**: `hand.hpp` / `hand.cpp`, `deck.hpp` / `deck.cpp`, etc.
- **Shared types** (enums, small structs, free functions like `card_value`) go in `types.hpp` / `types.cpp`.
- Public headers live in `lib/include/blackjack/` and use `#pragma once`.
- Include paths use the `blackjack/` prefix: `#include "blackjack/hand.hpp"`.

## 3. Naming Conventions

| Element | Convention | Example |
|---------|-----------|---------|
| Namespace | `lowercase` | `blackjack` |
| Class / Struct / Enum | `PascalCase` | `GameState`, `HandResult`, `Card` |
| Enum values | `PascalCase` | `Suit::Hearts`, `GameState::PlayerTurn` |
| Functions / Methods | `snake_case` | `add_card()`, `is_bust()`, `play_dealer()` |
| Member variables | `snake_case_` (trailing underscore) | `cards_`, `state_`, `rng_` |
| Local variables / Parameters | `snake_case` | `active_index`, `card` |
| Constants | `k_snake_case` or `constexpr snake_case` | `k_max_hand_value`, `constexpr int blackjack_value = 21;` |
| Files | `snake_case` | `hand.hpp`, `game.cpp`, `test_hand.cpp` |

## 4. Code Style

- **Brace style**: K&R (opening brace on same line).
  ```cpp
  if (hand.is_bust()) {
      advance_to_next_hand();
  } else {
      // continue
  }
  ```
- **Indentation**: 4 spaces. No tabs.
- **Max line length**: 100 characters.
- **`auto` usage**: Use `auto` when the type is obvious from the initializer (e.g., `auto it = vec.begin();`). Spell out the type when it aids readability or the type is non-obvious.
- **`const` / `constexpr`**: Use wherever possible. Mark all query methods `const`. Prefer `constexpr` for compile-time constants.
- **`[[nodiscard]]`**: Apply to query methods that return values callers should not ignore (e.g., `value()`, `is_bust()`, `state()`, command methods returning `ActionResult`).
- **Structured bindings**: Use where they improve readability (e.g., iterating over pairs/tuples).
- **No `using namespace std;`**: Fully qualify standard library types or use targeted `using` declarations within function scope only.
- **Include order**:
  1. Corresponding header (in `.cpp` files)
  2. Project headers (`blackjack/...`)
  3. Standard library headers
  4. Third-party headers (frontends only)

  Separate each group with a blank line.

## 5. Modern C++ Practices

- **`enum class`** for all enumerations. No unscoped enums.
- **`std::optional`** for values that may not exist (e.g., `dealer_up_card()`).
- **`std::span`** for non-owning views of contiguous data (e.g., `cards()`, `player_hands()`).
- **`std::vector`** for owning collections.
- **Value semantics**: Prefer value types. Pass by `const&` for types larger than a pointer. Return by value.
- **RAII**: No manual resource management. Smart pointers if heap allocation is needed (unlikely in this project).
- **No raw `new`/`delete`**. No raw owning pointers.
- **No C-style casts**: Use `static_cast`, `const_cast`, `reinterpret_cast` as needed. This includes `(void)expr` — use `std::ignore = expr` (from `<tuple>`) to discard `[[nodiscard]]` return values.
- **No `#define` for constants**: Use `constexpr`.
- **Default comparisons**: Use `= default` for `operator==` where applicable (as in `Card`).

## 6. Error Handling

- **Library (`lib/`)**: Commands return `ActionResult` enum. No exceptions thrown from any public method. Invalid operations return an error variant — they never crash, assert, throw, or corrupt state.
- **Frontends**: May use exceptions for their own I/O errors if appropriate. Must check `ActionResult` from every library command call.
- **Never silently swallow errors**: Every `ActionResult` must be checked or explicitly discarded with a comment explaining why.

## 7. Logging & I/O Constraints

- **Library (`lib/`)**: No `#include <iostream>`, no `stdout`, no `stderr`, no file I/O, no logging. It is a pure logic library.
- **CLI frontend**: Uses `<iostream>` for standard I/O.
- **FTXUI frontend**: Uses FTXUI rendering. May use `stderr` for fatal errors.

## 8. Testing Standards

- **Framework**: Catch2 v3, fetched via `FetchContent`.
- **File naming**: `test_<unit>.cpp` (e.g., `test_hand.cpp`, `test_game.cpp`).
- **One test file per library source file**.
- **Test names**: Descriptive scenario strings.
  ```cpp
  TEST_CASE("Hand value with soft ace counts ace as 11") { ... }
  TEST_CASE("Hit on bust hand returns InvalidAction") { ... }
  ```
- **Deterministic seeds**: All game-level tests use `Game(seed)` with a fixed seed. Document the seed and expected deck order in a comment if tests depend on specific cards.
- **Edge cases to cover**:
  - Empty hand (value = 0)
  - Single card hand
  - Soft ace transitions (ace counted as 11 drops to 1 when bust)
  - Bust boundary (value = 21 vs 22)
  - Natural blackjack (two-card 21)
  - Split aces
  - Split non-aces
  - Invalid actions in each game state
  - Full game loop: deal through round over
- **All tests must pass on all target platforms** (GCC, Clang, MSVC).

## 9. Comments & Documentation

- **Public API headers** (`lib/include/blackjack/`): Brief doc comment above each class and each public method using `///`.
  ```cpp
  /// Returns the best hand value with soft ace logic applied.
  /// Aces count as 11 if the total does not exceed 21, otherwise 1.
  [[nodiscard]] int value() const;
  ```
- **Implementation files**: Comment non-obvious logic only. Do not comment the obvious.
- **No TODO comments** in committed code without a tracking reference (issue number or equivalent).

## 10. CMake Standards

- **Minimum version**: CMake 3.20.
- **Target-scoped commands only**: Use `target_include_directories`, `target_link_libraries`, `target_compile_features`, etc. Never use directory-level `include_directories()` or `link_libraries()`.
- **C++ standard**: Set via `target_compile_features(<target> PUBLIC cxx_std_20)` on the library, `PRIVATE` on executables.
- **External dependencies**: Fetched via `FetchContent`. Pin to specific git tags.
  - Catch2: pin to a v3.x release tag
  - FTXUI: pin to a specific release tag
- **Target names**:
  - `blackjack_lib` — static library
  - `blackjack_cli` — CLI executable
  - `blackjack_tui` — FTXUI executable
  - `blackjack_tests` — test executable
- **Warning flags**: Applied via `target_compile_options` on each target, using generator expressions for compiler-specific flags:
  ```cmake
  target_compile_options(blackjack_lib PRIVATE
      $<$<CXX_COMPILER_ID:MSVC>:/W4>
      $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
  )
  ```
