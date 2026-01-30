---
id: ISSUE-005
title: C-style (void) casts used to suppress [[nodiscard]] warnings
severity: medium
status: resolved
reported: 2026-01-30
resolved: 2026-01-30
assigned-to:
---

## Description

The codebase uses C-style `(void)` casts to suppress `[[nodiscard]]` compiler warnings. This violates the project's coding standard which prohibits C-style casts. All instances should be replaced with `std::ignore` assignments.

## Scope

All `(void)` casts across the entire codebase — tests, CLI frontend, and FTXUI frontend.

## Expected Pattern

Replace:
```cpp
(void)game.deal();
```

With:
```cpp
std::ignore = game.deal();
```

Requires `#include <tuple>` where not already present.

## Actual Pattern

C-style `(void)` casts are used in:
- `tests/src/test_deck.cpp`
- `tests/src/test_game.cpp`
- `cli/src/main.cpp`
- `ftxui/src/main.cpp`

## Notes

- The project coding standards prohibit C-style casts. No exceptions.
- `std::ignore` is the idiomatic C++ way to explicitly discard a `[[nodiscard]]` return value.
- The backend developer agent's defaults should be updated to conform to this rule so future code does not reintroduce C-style casts.

## Resolution

All C-style `(void)` casts removed across 4 files:

- **`tests/src/test_deck.cpp`**: 4 casts replaced with `std::ignore =`
- **`tests/src/test_game.cpp`**: 50+ casts replaced with `std::ignore =`
- **`cli/src/main.cpp`**: 1 cast replaced with proper `ActionResult` check, matching the existing error-handling pattern in the same file
- **`ftxui/src/main.cpp`**: 11 casts replaced with `ActionResult` checks — button callbacks only refresh on success, keyboard handlers only consume events on success, auto-deal exits on failure

The CLI and FTXUI apps were silently discarding `[[nodiscard]]` return values rather than handling errors. These now properly check results instead of suppressing the warning with a cast.
