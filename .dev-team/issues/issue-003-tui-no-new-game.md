---
id: ISSUE-003
title: FTXUI frontend has no way to start a new game after round ends
severity: high
status: resolved
reported: 2026-01-30
resolved: 2026-01-30
assigned-to:
---

## Description

After completing a round in the FTXUI frontend (`blackjack_tui.exe`), there is no button or prompt to start a new game. The round ends, results are displayed, and the user is stuck with no available actions.

## Steps to Reproduce

1. Run `blackjack_tui.exe`
2. Play a round to completion (hit/stand until results are shown)
3. Look for a way to start a new round

## Expected Behavior

After a round ends, a "Deal" or "New Game" button should appear, allowing the player to start another round.

## Actual Behavior

No button is shown after the round ends. The player is stuck and must quit the application to play again.

## Notes

- The library supports consecutive rounds — `Game::deal()` is valid when `state_ == RoundOver`.
- The `build_controls()` function is only called once at startup, so the button set is never rebuilt to reflect the RoundOver state.
- Fix should add a "Deal" button that appears when `state() == GameState::RoundOver` (or `WaitingForDeal`).
