---
id: ISSUE-002
title: Splitting a non-ace pair skips player turn and jumps to results
severity: critical
status: resolved
reported: 2026-01-30
resolved: 2026-01-30 (could not reproduce)
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

## Resolution

Could not reproduce. Extensive investigation found no code path where splitting non-aces skips the player turn. Verified through:

1. **Static analysis**: Traced every code path through `Game::split()` (lib/src/game.cpp) and the CLI's `play_round()` while loop (cli/src/main.cpp). For non-aces, `split()` always leaves `state_ == PlayerTurn`, and the CLI while loop always re-enters.

2. **Brute-force testing**: Ran 100,000 seeds through the game engine, found 5,145 non-ace splittable pairs, split all of them, and verified that every one left state as `PlayerTurn` with 2 hands, `active_hand_index == 0`, and neither hand stood. Zero failures.

3. **Manual retest**: Added optional seed argument to CLI (`blackjack_cli.exe <seed>`). User retested with seed 30 (deals a pair of Queens) and confirmed split works correctly — both hands displayed, player prompted for each hand.

The original observation likely occurred because the initial test was done before the ISSUE-001 fix (garbled Unicode suit symbols), making the post-split display difficult to read. The reported behavior also matches what happens when splitting **aces** (both hands auto-stand per blackjack rules, immediately transitioning to dealer turn), which may have been the actual scenario.

### Secondary fix applied

During investigation, a real input-parsing bug was found and fixed: the CLI's if-else chain checked `first_char == 's'` (stand) before `input == "split"`, so typing the full word "split" would trigger stand instead. Fixed by reordering the split handler before the stand handler.

## Notes

- This does NOT apply to ace splits (aces auto-stand by rule, so skipping is correct for aces).
- The library's `Game::split()` for non-aces correctly leaves `state_ == PlayerTurn` and `active_hand_index == 0`.
- The CLI's `display_game_state()` correctly renders two hands after split with "Hand 1 *:" / "Hand 2  :" labels.
