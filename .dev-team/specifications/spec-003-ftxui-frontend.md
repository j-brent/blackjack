<!-- metadata
id: SPEC-003
type: feature-spec
status: draft
author: A2-architect
created: 2026-01-30
updated: 2026-01-30
approved-by: pending
dependencies: [ARCH-001, SPEC-001]
-->

# Feature Specification: FTXUI Terminal GUI Frontend

## 1. Overview

A rich terminal GUI frontend using the FTXUI library. Provides a visually structured, keyboard-driven interface for playing blackjack. Must look sleek and elegant within terminal constraints — this is not just a prettified CLI, but a proper terminal application with panels, styled cards, and smooth interaction.

## 2. Layout

The terminal screen is divided into vertically stacked panels:

```
┌─────────────────────────────────────────┐
│              BLACKJACK                  │  Title bar
├─────────────────────────────────────────┤
│                                         │
│           Dealer's Hand                 │  Dealer panel
│        ┌───┐ ┌───┐                      │
│        │ K │ │ ? │                      │
│        │ ♦ │ │   │                      │
│        └───┘ └───┘                      │
│           Value: 10                     │
│                                         │
├─────────────────────────────────────────┤
│                                         │
│           Your Hand                     │  Player panel
│        ┌───┐ ┌───┐                      │
│        │ A │ │ 7 │                      │
│        │ ♠ │ │ ♣ │                      │
│        └───┘ └───┘                      │
│           Value: 18 (soft)              │
│                                         │
├─────────────────────────────────────────┤
│  [ (H)it ]  [ (S)tand ]  [ S(p)lit ]    │  Controls
├─────────────────────────────────────────┤
│  Status: Your turn                      │  Status bar
└─────────────────────────────────────────┘
```

### 2.1 Title Bar
- Centered text: "BLACKJACK"
- Visually separated from the rest (border or color)

### 2.2 Dealer Panel
- Label: "Dealer's Hand"
- Card rendering (see section 3)
- During player turn: hole card shown as `[?]` or a face-down card graphic
- After reveal: all cards shown face-up
- Hand value displayed below cards (hidden during player turn, or show only the visible card's value)

### 2.3 Player Panel
- Label: "Your Hand" (or "Hand 1" / "Hand 2" during split)
- Card rendering with all cards face-up
- Hand value displayed below cards, with "(soft)" indicator when applicable (see section 2.4)
- During split: both hands visible, active hand highlighted (brighter border or marker)

### 2.4 Soft Ace Display

The "(soft)" indicator is shown below the hand value whenever an ace is currently counted as 11.

**Rules:**
- Display `Value: N (soft)` whenever `Hand::is_soft()` returns true
- When an ace is demoted from 11 to 1, the "(soft)" indicator is removed

**Examples:**
- `[A♠] [6♥]` shows `Value: 17 (soft)`
- `[A♠] [6♥] [3♣]` shows `Value: 20 (soft)`
- `[A♠] [6♥] [8♣]` shows `Value: 15` — ace demoted to 1, no longer soft
- `[A♠] [A♥] [9♦]` shows `Value: 21 (soft)` — one ace is 11, one is 1

### 2.5 Controls Panel
- Horizontal row of buttons
- Buttons include keyboard shortcut hints in labels: `[ (H)it ]`, `[ (S)tand ]`, `[ S(p)lit ]`
- During ROUND_OVER: controls change to `[ (N)ew Game ]` and `[ (Q)uit ]`
- Only valid actions are enabled/visible. Invalid actions are either hidden or grayed out.
- Keyboard navigation: arrow keys or Tab to move between buttons, Enter to select
- Shortcut keys: `H` for Hit, `S` for Stand, `P` for Split

### 2.6 Status Bar
- Single line at the bottom
- Shows current game status:
  - `"Your turn"` — during player turn
  - `"Your turn - Hand 1"` / `"Your turn - Hand 2"` — during split
  - `"Dealer's turn..."` — during dealer play
  - `"You win!"`, `"You lose."`, `"Push."`, `"Blackjack!"` — after round
  - During split, show per-hand results: `"Hand 1: Win | Hand 2: Lose"`

## 3. Card Rendering

Cards are rendered as small bordered boxes using Unicode box-drawing characters:

### 3.1 Face-Up Card
```
┌─────┐
│ A   │
│  ♠  │
│   A │
└─────┘
```
- Top-left: rank (left-aligned)
- Center: suit symbol
- Bottom-right: rank (right-aligned, inverted conceptually)
- Width: 7 characters, Height: 5 lines

For 10:
```
┌─────┐
│10   │
│  ♠  │
│  10 │
└─────┘
```

### 3.2 Face-Down Card (Dealer Hole Card)
```
┌─────┐
│░░░░░│
│░░░░░│
│░░░░░│
└─────┘
```

### 3.3 Suit Colors
- Hearts (♥) and Diamonds (♦): **Red** text
- Spades (♠) and Clubs (♣): **White** or default terminal color
- Card background: default terminal background
- Card border: dim/gray

### 3.4 Card Arrangement
- Cards in a hand are displayed horizontally, separated by 1 space
- If many cards would overflow the terminal width, overlap cards (show only left edge of earlier cards)

## 4. Interaction

### 4.1 Keyboard Controls
| Key       | Action                                    |
|-----------|-------------------------------------------|
| `H`       | Hit                                       |
| `S`       | Stand                                     |
| `P`       | Split (when eligible)                     |
| `N`       | New Game (when round is over)             |
| `Q`       | Quit                                      |
| Arrow/Tab | Navigate between buttons                  |
| Enter     | Activate selected button                  |
| `Esc`     | Quit                                      |

### 4.2 Button States
- **Active**: available for selection, normal color
- **Disabled/Hidden**: not valid for current state, either hidden or shown in dim/gray and non-selectable
- **Focused**: currently selected via keyboard, highlighted border or color

## 5. Game Flow

### 5.1 Startup
- Application launches, displays title and empty table
- Automatically deals the first hand (no menu needed)

### 5.2 Player Turn
- Cards animate in (appear one by one with a brief visual effect if feasible, otherwise just appear)
- Player selects actions via keyboard
- After each action, the display updates immediately
- Bust is shown via status bar and a brief visual indicator (e.g., hand value turns red)

### 5.3 Dealer Turn
- Hole card flips (face-down graphic replaced with face-up card)
- Dealer draws are shown one at a time (with a brief pause of ~500ms between draws for visual effect)
- Dealer bust indicated visually

### 5.4 Round Over
- Result displayed in status bar
- Controls switch to `[ (N)ew Game ]` and `[ (Q)uit ]`
- Player presses `N` to deal again or `Q`/`Esc` to exit

### 5.5 Split Flow
- When split occurs, the player panel expands to show two hands
- Active hand is highlighted with a brighter border or a `>>>` marker
- Hands are arranged side-by-side if terminal width allows, otherwise stacked
- Each hand is played in sequence
- **Hand transition**: When Hand 1 is completed (by bust or stand), the status bar updates to indicate the transition:
  - On stand: `"Hand 1 complete. Playing Hand 2..."`
  - On bust: `"Hand 1: Bust! Playing Hand 2..."`
- The active hand highlight moves to Hand 2 immediately

## 6. Visual Design Guidelines

- **Minimalist**: no unnecessary decoration. Clean borders, consistent spacing.
- **High contrast**: text easily readable on both dark and light terminals (prefer white text, red for hearts/diamonds)
- **Responsive**: if terminal is too narrow, degrade gracefully (smaller cards, abbreviate labels) rather than crash or garble
- **No flicker**: FTXUI's screen diffing handles this, but ensure no unnecessary full redraws

## 7. Non-Functional Requirements

- Depends on: `libblackjack` (static lib) and FTXUI (fetched via CMake FetchContent)
- Must compile and run on Windows (Windows Terminal, ConEmu), Linux (most terminal emulators), macOS (Terminal.app, iTerm2)
- Terminal must support Unicode and 16-color ANSI (virtually all modern terminals do)
- Minimum terminal size: 40 columns x 20 rows. If smaller, show a warning.

## 8. Acceptance Criteria

### AC-TUI-1: Launch and Display

```
Given the TUI application is launched
When the first hand is dealt
Then the dealer panel shows one face-up card and one face-down card
And the player panel shows two face-up cards with the hand value
And the controls panel shows active Hit and Stand buttons
And the status bar shows "Your turn"
```

### AC-TUI-2: Hit

```
Given the player presses H
When a card is added to the player's hand
Then the player panel updates to show the new card
And the hand value updates
And the controls remain active (unless bust)
```

### AC-TUI-3: Stand and Dealer Reveal

```
Given the player presses S
Then the dealer's hole card is revealed (face-down becomes face-up)
And the dealer draws cards one at a time with a brief visual pause
And the status bar updates to show the result
And the controls switch to New Game and Quit
```

### AC-TUI-4: Split Display

```
Given the player has a pair and presses P
Then the player panel shows two hands side-by-side
And the active hand is visually highlighted
And the player plays each hand in sequence
```

### AC-TUI-5: Card Rendering

```
Given a card (e.g., Ace of Spades)
When it is rendered in the player panel
Then it appears as a bordered box with rank and suit
And spades are displayed in white/default color
```

```
Given a card (e.g., 10 of Hearts)
When it is rendered
Then the hearts suit symbol is displayed in red
```

### AC-TUI-6: Keyboard Navigation

```
Given the controls panel shows Hit and Stand buttons
When the player presses Tab
Then focus moves between the buttons
And the focused button is visually highlighted
```

### AC-TUI-7: Round Over Controls

```
Given a round has ended
When the result is displayed
Then the controls show [New Game] and [Quit]
And pressing N starts a new round
And pressing Q exits the application
```

### AC-TUI-8: Dealer Hole Card Hidden

```
Given it is the player's turn
Then the dealer's second card is shown as a face-down card (filled box)
And the dealer's displayed hand value only reflects the visible card
```

### AC-TUI-9: Bust Visual

```
Given the player busts
Then the hand value is displayed in a visually distinct way (e.g., red text or "BUST")
And the status bar shows "Bust!"
And the dealer's hole card is revealed and full dealer hand is displayed for transparency
```

### AC-TUI-10: Graceful Terminal Size

```
Given the terminal is at least 40x20
Then the full layout is rendered correctly
Given the terminal is smaller than 40x20
Then a message indicates the terminal should be resized
```

## 9. Testing Strategy

The FTXUI frontend is primarily a visual presentation layer. Its testing strategy differs from the engine library.

### 9.1 Rule Correctness
All game rule logic (hand evaluation, dealer behavior, result determination, split rules) is tested at the `libblackjack` library level via automated unit tests. The FTXUI frontend does NOT duplicate rule-correctness testing.

### 9.2 Compile and Launch Smoke Test
- The FTXUI frontend must compile cleanly with no warnings (same compiler flags as the library).
- A smoke test verifies the application launches without crashing and renders the initial deal screen. This can be automated as a build-system test that launches the process, waits briefly, and checks for a zero or clean exit code when sent a quit signal.

### 9.3 Manual Visual Testing
The following are verified by manual visual inspection during development:
- Card rendering (correct rank, suit, colors, box-drawing characters)
- Layout panels (title bar, dealer panel, player panel, controls, status bar)
- Button states (enabled, disabled, focused)
- Split hand display (side-by-side, active hand highlighting)
- Soft ace indicator display
- Dealer reveal animation timing
- Graceful terminal size handling
- Keyboard shortcut labels in buttons

### 9.4 Automated Testing Boundary
If future phases require automated UI testing, FTXUI's component model allows injecting synthetic events. This is out of scope for Phase 1 but the architecture does not preclude it.

## 10. Error Handling

### 10.1 Library API Errors
- If a command method returns an error `ActionResult` (e.g., `InvalidAction`), the status bar displays an error message: `"Invalid action"`. The UI remains responsive and the game state is unchanged.

### 10.2 Unexpected/Internal Errors
- If an unexpected exception or unrecoverable error occurs, the application should clean up the terminal (restore normal mode via FTXUI's cleanup) and print an error message to stderr before exiting with a non-zero status code.
- The application must never leave the terminal in a broken state (raw mode, hidden cursor, etc.).

### 10.3 Terminal Resize During Play
- FTXUI handles terminal resize events natively. The layout re-renders on resize.
- If the terminal is resized below the minimum (40x20) during play, display the resize warning. When the terminal is restored to a valid size, gameplay resumes normally.
