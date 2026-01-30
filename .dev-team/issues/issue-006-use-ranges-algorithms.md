---
id: ISSUE-006
title: Replace iterator-pair std algorithms with std::ranges equivalents
severity: low
status: open
reported: 2026-01-30
resolved:
assigned-to:
---

## Description

The codebase uses classic iterator-pair algorithm calls (e.g., `std::find(c.begin(), c.end(), val)`) in several places. These should be replaced with `std::ranges::` equivalents (e.g., `std::ranges::find(c, val)`), which are cleaner and eliminate the risk of mismatched iterators.

## Scope

All files under `blackjack/` — library, CLI frontend, FTXUI frontend, and tests.

Algorithms to look for:
- `std::find` → `std::ranges::find`
- `std::find_if` / `std::find_if_not` → `std::ranges::find_if` / `std::ranges::find_if_not`
- `std::transform` → `std::ranges::transform`
- `std::any_of` / `std::all_of` / `std::none_of` → `std::ranges::any_of` / etc.
- Any other `std::` algorithm with a `std::ranges::` counterpart

## Notes

- The project targets C++20, so all `std::ranges::` algorithms in `<algorithm>` are available.
- `std::ranges::contains` (C++23) is NOT available — continue using `std::ranges::find(c, val) != c.end()`.
- The FTXUI frontend's new code (ISSUE-003/004 fix) already uses `std::ranges::find`. This issue covers the remaining legacy uses.
