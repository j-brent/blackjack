---
id: ISSUE-004
title: FTXUI frontend quit shortcut (q) is not displayed
severity: medium
status: resolved
reported: 2026-01-30
resolved: 2026-01-30
assigned-to:
---

## Description

The FTXUI frontend (`blackjack_tui.exe`) supports pressing `q` to quit, but this shortcut is never shown to the user. It should be listed alongside the other available actions.

## Steps to Reproduce

1. Run `blackjack_tui.exe`
2. Look at the displayed controls/buttons
3. Note that quit is not listed

## Expected Behavior

A "[Q]uit" option should be visible in the controls area at all times, so the user knows how to exit the application.

## Actual Behavior

No quit option is displayed. The shortcut only works if the user guesses to press `q`.

## Notes

- The `q` keypress handler exists in the event handler code and works correctly.
- This is a discoverability issue only — the functionality works, it just needs to be shown.
- Could be a persistent label in the footer/status bar area rather than a button.
