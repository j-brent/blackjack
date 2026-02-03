<!-- metadata
id: SPEC-002
type: feature-spec
status: draft
author: A2-architect
created: 2026-01-30
updated: 2026-01-30
approved-by: pending
dependencies: [ARCH-001, SPEC-001]
-->

# Feature Specification: CLI Frontend

## 1. Overview

A minimal text-based frontend for the blackjack game engine. Reads commands from stdin, prints game state to stdout. Exists primarily as a proof-of-concept that the library API works and as a baseline for comparison against richer frontends.

## 2. Display

### 2.1 Card Display
Cards are shown as short text: rank + suit symbol.
- Ranks: `A`, `2`–`10`, `J`, `Q`, `K`
- Suits: `♠`, `♥`, `♦`, `♣` (or ASCII fallback: `S`, `H`, `D`, `C` if Unicode is unavailable)
- Example: `A♠`, `10♥`, `K♦`

### 2.2 Hand Display
Each hand is shown as a list of cards followed by the hand value.
- Player: `Your hand: [A♠] [10♥]  (21)`
- Player (soft hand): `Your hand: [A♠] [7♣]  (18, soft)` — see section 2.5 for soft ace rules
- Dealer (during player turn): `Dealer: [K♦] [??]` — hole card hidden
- Dealer (after reveal): `Dealer: [K♦] [7♣]  (17)`

### 2.5 Soft Ace Display

The "(soft)" indicator is shown whenever an ace in the hand is currently counted as 11.

**Rules:**
- Display "(soft)" after the hand value whenever `Hand::is_soft()` returns true
- When an ace is demoted from 11 to 1 (due to additional cards pushing the total over 21), the "(soft)" indicator is removed

**Examples:**
- `[A♠] [6♥]` displays as `(17, soft)` — ace counted as 11
- `[A♠] [6♥] [3♣]` displays as `(20, soft)` — ace still counted as 11
- `[A♠] [6♥] [8♣]` displays as `(15)` — ace demoted to 1 to avoid bust; no longer soft
- `[A♠] [A♥] [9♦]` displays as `(21, soft)` — one ace is 11, one is 1

### 2.3 Game State Display
After each action, print:
1. A clear divider or header (e.g., `--- Blackjack ---`)
2. Dealer's hand (with hole card hidden or revealed as appropriate)
3. Player's hand(s) — if split, label as `Hand 1` and `Hand 2`, mark the active hand with `*`
4. Available actions as a prompt

### 2.4 Results Display
After a round, print each hand's result:
- `You win!`, `You lose.`, `Push.`, `Blackjack!`
- If split, show per-hand: `Hand 1: Win! | Hand 2: Lose.`

## 3. Input

### 3.1 Player Commands
Text input, case-insensitive, matched by prefix:
- `h` or `hit` — hit
- `s` or `stand` — stand
- `p` or `split` — split (only shown/accepted when eligible)
- `q` or `quit` — exit the program

### 3.2 Play Again Prompt
After a round ends:
```
Play again? (y/n):
```
- `y` or `yes` — deal a new round
- `n` or `no` — exit

### 3.3 Invalid Input
If the player types an unrecognized command, print:
```
Invalid command. Options: hit (h), stand (s), split (p)
```
and re-prompt. Do not change game state.

## 4. Game Flow

```
1. Print welcome message
2. Deal (automatic on first run)
3. Display dealer hand (hole card hidden) and player hand
4. LOOP (player turn):
   a. Prompt for action (show only valid actions)
   b. Read input
   c. Execute action via library API
   d. Display updated hands
   e. If bust, print "Bust!" and exit loop (or move to next split hand with transition message)
   f. If stood, exit loop (or move to next split hand with transition message)
5. Dealer turn — print "Dealer reveals..." then show each dealer draw
   Note: The dealer's full hand is always revealed at the end, even if the player busted, for transparency.
6. Display final hands and values
7. Display results
8. Prompt for play again
9. If yes, go to step 2. If no, exit.
```

### 4.1 Natural Blackjack Handling
- If player has natural blackjack, skip player input — display "Blackjack!" and proceed to dealer turn.
- If dealer also has natural blackjack, display "Push — dealer also has blackjack."

### 4.2 Split Flow
- After a split, label hands `Hand 1` and `Hand 2`
- Mark the active hand with `*` or `>>>`
- Play Hand 1 fully, then Hand 2
- **Hand transition**: When Hand 1 is completed (by bust or stand), print a clear transition message before moving to Hand 2:
  - On stand: `--- Hand 1 complete. Now playing Hand 2 ---`
  - On bust: `Hand 1: Bust! --- Now playing Hand 2 ---`
- After the transition message, display the updated game state showing Hand 2 as the active hand
- Display results for each hand separately

## 5. Non-Functional Requirements

- No dependencies beyond the blackjack library and C++ standard library
- Must compile and run on Windows, Linux, and macOS
- Clean exit on `Ctrl+C` (no crash, no resource leaks — though there are minimal resources)

## 6. Acceptance Criteria

### AC-CLI-1: Basic Round

```
Given the CLI is launched
When a round is dealt
Then the dealer's hand shows one face-up card and one hidden card
And the player's hand shows two face-up cards with the hand value
And the user is prompted for an action
```

### AC-CLI-2: Hit

```
Given the player is prompted for an action
When the player types "h"
Then a new card is added to the player's hand
And the updated hand and value are displayed
And the player is prompted again (unless bust)
```

### AC-CLI-3: Stand and Dealer Play

```
Given the player is prompted for an action
When the player types "s"
Then the dealer's hole card is revealed
And the dealer draws cards per rules (hit soft 17)
And the final hands and result are displayed
```

### AC-CLI-4: Split Display

```
Given the player has a pair and types "p"
Then the display shows "Hand 1" and "Hand 2"
And the active hand is marked
And the player plays each hand sequentially
```

### AC-CLI-5: Invalid Input

```
Given the player is prompted for an action
When the player types "x"
Then an error message is shown with valid options
And the player is prompted again
And the game state is unchanged
```

### AC-CLI-6: Play Again

```
Given a round has ended and results are displayed
When the player is prompted "Play again? (y/n)"
And the player types "y"
Then a new round begins with a fresh deal
```

### AC-CLI-7: Quit

```
Given the player is prompted for an action
When the player types "q"
Then the program exits cleanly
```

### AC-CLI-8: Bust Display

```
Given the player hits and the hand value exceeds 21
Then "Bust!" is displayed
And the dealer's hole card is revealed and final dealer hand is shown
And the round proceeds to resolution without further player input
```

### AC-CLI-9: Soft Ace Display

```
Given the player's hand contains an ace counted as 11
When the hand is displayed
Then the hand value includes "(soft)" — e.g., "Your hand: [A♠] [7♣]  (18, soft)"
```

```
Given the player's hand previously had a soft ace
When a new card causes the ace to be demoted to 1
Then the "(soft)" indicator is no longer shown
```

## 7. Error Handling

### 7.1 Library API Errors
- If a command method (hit, stand, split) returns `ActionResult::InvalidAction` or another error variant, print an error message:
  `"Error: action not available right now."`
- Re-prompt the player for input. Do not change or corrupt game state.

### 7.2 Unexpected/Internal Errors
- If an unexpected exception or unrecoverable error occurs (e.g., library bug), print:
  `"An unexpected error occurred. Exiting."`
- Exit with a non-zero status code.

### 7.3 Input Stream Errors
- If stdin reaches EOF (e.g., piped input ends), exit cleanly with status 0.
- If stdin is in a bad state, exit cleanly.
