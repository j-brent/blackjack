---
id: ISSUE-005
title: C-style (void) casts used to suppress [[nodiscard]] warnings
severity: medium
status: open
reported: 2026-01-30
resolved:
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
