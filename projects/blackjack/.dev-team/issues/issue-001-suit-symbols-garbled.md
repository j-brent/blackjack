---
id: ISSUE-001
title: Suit symbols render as mojibake in all Windows terminals
severity: high
status: resolved
reported: 2026-01-30
resolved: 2026-01-30 (commit 6fb1716)
assigned-to:
---

## Description

The CLI frontend uses Unicode suit symbols (♠ ♥ ♦ ♣) which render as multi-byte garbled characters (mojibake like â™ sequences) on Windows. The issue reproduces in Git Bash, PowerShell, and cmd.exe using the classic console host (conhost.exe).

## Steps to Reproduce

1. Build `blackjack_cli.exe`
2. Run it in any of: Git Bash, PowerShell (conhost), cmd.exe
3. Deal a hand
4. Observe suit symbols in the card display

## Expected Behavior

Suit symbols display correctly (either Unicode glyphs or ASCII fallbacks).

## Actual Behavior

Suit characters render as multi-byte garbled text (mojibake). The output is unreadable where suits appear.

## Notes

- The classic Windows console (conhost.exe) does not default to UTF-8. The active code page is typically 437 or 1252, not 65001 (UTF-8).
- Possible fixes:
  - Use ASCII fallbacks for suits (S, H, D, C) when Unicode output is not supported
  - Call `SetConsoleOutputCP(CP_UTF8)` on Windows before printing
  - Detect terminal capabilities and choose symbols accordingly
- This is a display-only issue — game logic is unaffected.
