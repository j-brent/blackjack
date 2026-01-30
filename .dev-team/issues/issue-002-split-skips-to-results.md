---
id: ISSUE-002
title: Splitting a non-ace pair skips player turn and jumps to results
severity: critical
status: open
reported: 2026-01-30
resolved:
assigned-to:
---

## Description

When splitting a non-ace pair in the CLI frontend, the game skips the player turn entirely. Instead of allowing the player to hit/stand on each split hand, it immediately shows the dealer's hand and declares a result. The player never gets to play either split hand.

## Steps to Reproduce

1. Run `blackjack_cli.exe`
2. Deal a hand that contains a non-ace pair (e.g., two 8s)
3. Choose split (p)
4. Observe the output

## Expected Behavior

After splitting:
1. Display both split hands with their new second cards
2. Prompt the player to hit/stand on Hand 1
3. After Hand 1 is complete, prompt for Hand 2
4. After both hands are complete, play the dealer turn
5. Display results for both hands

## Actual Behavior

After choosing split, the game skips directly to showing the dealer's hand and declaring a win. No opportunity to play either split hand. It is unclear whether results for both hands were shown.

## Notes

- This does NOT apply to ace splits (aces auto-stand by rule, so skipping is correct for aces).
- The issue is likely in the CLI frontend's game loop — it may not be re-entering the player turn loop after the split command, or the split may be incorrectly transitioning state.
- The library's `Game::split()` for non-aces should leave `state_ == PlayerTurn` and `active_hand_index == 0`, allowing continued play. The bug is likely in how the CLI handles the post-split flow.
