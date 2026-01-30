<!-- metadata
id: UI-001
type: ui-design
status: draft
author: A5-ux-designer
created: 2026-01-30
updated: 2026-01-30
approved-by: pending
dependencies: [SPEC-002, SPEC-003, VISION-001]
-->

# UI Design Document: Blackjack

## 1. Design Principles

### P1: Clarity Over Decoration
Every visual element must serve a functional purpose. No ornamental borders, no decorative ASCII art, no filler characters. If a design element does not help the player understand the game state, remove it.

### P2: Instant Comprehension
A player glancing at the screen should understand three things within one second: whose turn it is, what the hands look like, and what actions are available. Layout, typography, and color exist to serve this goal.

### P3: Consistent Visual Language
The same information is represented the same way everywhere. Card format, value display, soft-ace notation, and result messages use identical patterns across both frontends. A player switching between CLI and FTXUI should feel no cognitive friction.

### P4: Graceful Restraint
Use the minimum effective amount of color, spacing, and emphasis. One accent color is better than three. One blank line of separation is better than a decorative border. White space is a design tool, not wasted space.

### P5: Accessible by Default
All information is conveyed through text, not solely through color. Color is additive emphasis, never the only signal. Keyboard-only operation is fully supported in both frontends.

---

## 2. CLI Design Specification

### 2.1 Typography and Spacing

**Line width**: All output fits within 60 characters. No line exceeds this width.

**Divider**: A simple em-dash line used to separate game states:

```
------------------------------------------------------------
```

That is 60 hyphens (`-`). Used once at the top of each game state refresh.

**Blank line usage**:
- One blank line between the divider and the title
- One blank line between the title and dealer hand
- One blank line between dealer hand and player hand(s)
- One blank line between player hand(s) and the prompt
- No trailing blank lines after the prompt

**Card text format**: Rank immediately followed by suit symbol, wrapped in square brackets with single space padding.

```
[A~] [10~] [K~] [7~]
```

Where `~` represents the suit symbol. The bracket contents are: space, rank, suit, space -- except for `10` which omits the leading space: `[10~]`.

Exact formats per rank:
```
[ A~ ]   [ 2~ ]   [ 3~ ]   [ 4~ ]   [ 5~ ]
[ 6~ ]   [ 7~ ]   [ 8~ ]   [ 9~ ]   [10~ ]
[ J~ ]   [ Q~ ]   [ K~ ]
```

Where `~` is the suit symbol. Each card token is separated by a single space in a hand display.

**Hidden card format**: `[??]`

**Hand display format**:

Normal hand:
```
Your hand: [ A~ ] [ 7~ ]  (18, soft)
```

Hard hand (no soft ace):
```
Your hand: [ K~ ] [ 7~ ]  (17)
```

Bust hand:
```
Your hand: [ K~ ] [ 9~ ] [ 5~ ]  (24) BUST
```

Dealer hidden:
```
Dealer:    [ K~ ] [??]
```

Dealer revealed:
```
Dealer:    [ K~ ] [ 7~ ]  (17)
```

Split hands (Hand 1 active):
```
Hand 1 *:  [ A~ ] [ 5~ ]  (16, soft)
Hand 2  :  [ A~ ] [ 8~ ]  (19, soft)
```

Split hands (Hand 2 active):
```
Hand 1  :  [ A~ ] [ 5~ ] [ 6~ ]  (12)
Hand 2 *:  [ A~ ] [ 8~ ]  (19, soft)
```

**Label alignment**: The labels `Your hand:`, `Dealer:`, `Hand 1 *:`, `Hand 2  :` are all right-padded to 11 characters so that card displays align vertically.

### 2.2 Character Set

**Unicode characters (preferred)**:

| Symbol | Unicode | Description |
|--------|---------|-------------|
| `~`    | `U+2660` | Black Spade Suit |
| `~`    | `U+2665` | Black Heart Suit |
| `~`    | `U+2666` | Black Diamond Suit |
| `~`    | `U+2663` | Black Club Suit |

Rendered: `\u2660` = `~`, `\u2665` = `~`, `\u2666` = `~`, `\u2663` = `~`

**ASCII fallback** (when Unicode detection fails):

| Unicode | Fallback |
|---------|----------|
| `\u2660` (spade)   | `S` |
| `\u2665` (heart)   | `H` |
| `\u2666` (diamond) | `D` |
| `\u2663` (club)    | `C` |

**Separators**: Only ASCII hyphens (`-`) are used for dividers. No box-drawing characters in CLI mode.

### 2.3 Game Flow Screens

Each screen below is the exact text output. Lines are shown literally. `~` in card tokens represents a suit symbol.

---

**Welcome Screen**

```
------------------------------------------------------------

                        BLACKJACK

------------------------------------------------------------
```

This is printed once on startup, immediately followed by the first deal.

---

**Normal Deal (Player Turn)**

```
------------------------------------------------------------
                        BLACKJACK
------------------------------------------------------------

Dealer:    [ K\u2666 ] [??]

Your hand: [ A\u2660 ] [ 7\u2663 ]  (18, soft)

Actions: [h]it  [s]tand  [q]uit
>
```

When split is available, the prompt changes to:
```
Actions: [h]it  [s]tand  s[p]lit  [q]uit
>
```

---

**After Hit (No Bust)**

```
------------------------------------------------------------
                        BLACKJACK
------------------------------------------------------------

Dealer:    [ K\u2666 ] [??]

Your hand: [ A\u2660 ] [ 7\u2663 ] [ 3\u2665 ]  (21)

Actions: [h]it  [s]tand  [q]uit
>
```

Note: Once the soft ace is consumed (total would bust with ace as 11), the `(soft)` indicator disappears. In this example, A+7+3 = 21 with ace as 11, so it would still show `(21, soft)` if the ace is still counted as 11. The engine's `is_soft()` return value is authoritative.

---

**After Hit (Bust)**

```
------------------------------------------------------------
                        BLACKJACK
------------------------------------------------------------

Dealer:    [ K\u2666 ] [??]

Your hand: [ K\u2660 ] [ 9\u2663 ] [ 5\u2665 ]  (24) BUST

Bust!
```

The game then proceeds automatically to dealer reveal (see below).

---

**Dealer Reveal and Play**

```
Dealer reveals hole card...

Dealer:    [ K\u2666 ] [ 7\u2663 ]  (17)
Dealer stands.

```

If dealer draws:
```
Dealer reveals hole card...

Dealer:    [ K\u2666 ] [ 3\u2663 ]  (13)
Dealer hits...
Dealer:    [ K\u2666 ] [ 3\u2663 ] [ 5\u2665 ]  (18)
Dealer stands.

```

If dealer busts:
```
Dealer reveals hole card...

Dealer:    [ K\u2666 ] [ 3\u2663 ]  (13)
Dealer hits...
Dealer:    [ K\u2666 ] [ 3\u2663 ] [ Q\u2665 ]  (23)
Dealer busts!

```

---

**Round Result (Single Hand)**

Win:
```
------------------------------------------------------------
Result: You win!
------------------------------------------------------------

Play again? [y/n]:
```

Lose:
```
------------------------------------------------------------
Result: You lose.
------------------------------------------------------------

Play again? [y/n]:
```

Push:
```
------------------------------------------------------------
Result: Push.
------------------------------------------------------------

Play again? [y/n]:
```

Blackjack:
```
------------------------------------------------------------
Result: Blackjack! You win!
------------------------------------------------------------

Play again? [y/n]:
```

Push with both blackjacks:
```
------------------------------------------------------------
Result: Push -- dealer also has blackjack.
------------------------------------------------------------

Play again? [y/n]:
```

---

**Round Result (Split Hands)**

```
------------------------------------------------------------
Results:
  Hand 1: Win!
  Hand 2: Lose.
------------------------------------------------------------

Play again? [y/n]:
```

Possible per-hand results: `Win!`, `Lose.`, `Push.`, `Bust.`

---

**Split Flow**

After player enters `p`:

```
------------------------------------------------------------
                        BLACKJACK
------------------------------------------------------------

Dealer:    [ K\u2666 ] [??]

Hand 1 *:  [ 8\u2660 ] [ 3\u2665 ]  (11)
Hand 2  :  [ 8\u2663 ] [ 6\u2666 ]  (14)

Actions: [h]it  [s]tand  [q]uit
>
```

After Hand 1 stands:
```
--- Hand 1 complete. Now playing Hand 2 ---

------------------------------------------------------------
                        BLACKJACK
------------------------------------------------------------

Dealer:    [ K\u2666 ] [??]

Hand 1  :  [ 8\u2660 ] [ 3\u2665 ] [ 9\u2663 ]  (20)
Hand 2 *:  [ 8\u2663 ] [ 6\u2666 ]  (14)

Actions: [h]it  [s]tand  [q]uit
>
```

After Hand 1 busts:
```
Hand 1: Bust! --- Now playing Hand 2 ---

------------------------------------------------------------
                        BLACKJACK
------------------------------------------------------------

Dealer:    [ K\u2666 ] [??]

Hand 1  :  [ 8\u2660 ] [ 3\u2665 ] [ K\u2663 ] [ Q\u2666 ]  (31) BUST
Hand 2 *:  [ 8\u2663 ] [ 6\u2666 ]  (14)

Actions: [h]it  [s]tand  [q]uit
>
```

---

**Play Again Prompt**

```
Play again? [y/n]:
```

If invalid response:
```
Please enter 'y' or 'n'.
Play again? [y/n]:
```

---

**Error Messages**

Invalid command during play:
```
Invalid command. Options: hit (h), stand (s), split (p)
>
```

(Omit `split (p)` when split is not available.)

Invalid action from engine:
```
Error: action not available right now.
>
```

Fatal error:
```
An unexpected error occurred. Exiting.
```

---

## 3. FTXUI Design Specification

### 3.1 Color Palette

All colors use FTXUI's `ftxui::Color` enum values. The palette is minimal: white/default for structure, red for hearts/diamonds, and green for positive outcomes.

| UI Element | Foreground | Background | FTXUI Value |
|---|---|---|---|
| **Title bar text** | White, Bold | Default | `color(Color::White) \| bold` |
| **Title bar border** | GrayDark | Default | `color(Color::GrayDark)` |
| **Dealer panel label** | White | Default | `color(Color::White)` |
| **Dealer panel border** | GrayDark | Default | `color(Color::GrayDark)` |
| **Dealer value text** | GrayLight | Default | `color(Color::GrayLight)` |
| **Player panel label** | White | Default | `color(Color::White)` |
| **Player panel border** | GrayDark | Default | `color(Color::GrayDark)` |
| **Player panel border (active, split)** | White | Default | `color(Color::White)` |
| **Player value text** | GrayLight | Default | `color(Color::GrayLight)` |
| **Card rank -- Spades** | White | Default | `color(Color::White)` |
| **Card rank -- Clubs** | White | Default | `color(Color::White)` |
| **Card rank -- Hearts** | Red | Default | `color(Color::Red)` |
| **Card rank -- Diamonds** | Red | Default | `color(Color::Red)` |
| **Card suit -- Spades (\u2660)** | White | Default | `color(Color::White)` |
| **Card suit -- Clubs (\u2663)** | White | Default | `color(Color::White)` |
| **Card suit -- Hearts (\u2665)** | Red | Default | `color(Color::Red)` |
| **Card suit -- Diamonds (\u2666)** | Red | Default | `color(Color::Red)` |
| **Card back fill** | GrayDark | Default | `color(Color::GrayDark)` |
| **Card border** | GrayDark | Default | `color(Color::GrayDark)` |
| **Button text (normal)** | White | Default | `color(Color::White)` |
| **Button border (normal)** | GrayDark | Default | `color(Color::GrayDark)` |
| **Button border (focused)** | White | Default | `color(Color::White)` |
| **Button text (disabled)** | GrayDark | Default | `color(Color::GrayDark)` |
| **Button border (disabled)** | GrayDark | Default | `color(Color::GrayDark)` (dimmed via `dim`) |
| **Status bar -- normal** | GrayLight | Default | `color(Color::GrayLight)` |
| **Status bar -- "You win!"** | Green | Default | `color(Color::Green)` |
| **Status bar -- "You lose."** | Red | Default | `color(Color::Red)` |
| **Status bar -- "Bust!"** | Red | Default | `color(Color::Red)` |
| **Status bar -- "Push."** | Yellow | Default | `color(Color::Yellow)` |
| **Status bar -- "Blackjack!"** | Green, Bold | Default | `color(Color::Green) \| bold` |
| **Hand value (bust)** | Red | Default | `color(Color::Red)` |

### 3.2 Card Design

Cards are 7 characters wide and 5 lines tall, using light box-drawing characters.

**Face-up card -- single-character rank (A, 2-9, J, Q, K):**

```
\u250c\u2500\u2500\u2500\u2500\u2500\u2510
\u2502 R     \u2502
\u2502   S   \u2502
\u2502     R \u2502
\u2514\u2500\u2500\u2500\u2500\u2500\u2518
```

Where `R` = rank character, `S` = suit symbol.

Character-by-character for Ace of Spades:
```
Col:  1234567
      \u250c\u2500\u2500\u2500\u2500\u2500\u2510
      \u2502 A   \u2502
      \u2502  \u2660  \u2502
      \u2502   A \u2502
      \u2514\u2500\u2500\u2500\u2500\u2500\u2518
```

Positions (1-indexed within the border interior, 5 columns wide):
- Row 1: rank at col 1, spaces at cols 2-5
- Row 2: suit at col 3 (centered), spaces elsewhere
- Row 3: spaces at cols 1-3, rank at col 4, space at col 5

Correction -- let me be precise with a 5-wide interior:

```
\u250c\u2500\u2500\u2500\u2500\u2500\u2510
\u2502RssssS\u2502
\u2502ssSSSS\u2502
\u2502ssssRS\u2502
\u2514\u2500\u2500\u2500\u2500\u2500\u2518
```

Wait -- let me define this unambiguously with exact character positions. The card is 7 columns wide total (border + 5 interior + border).

**Card grid (7 wide x 5 tall):**

```
Col: 0123456
R0:  \u250c\u2500\u2500\u2500\u2500\u2500\u2510
R1:  \u2502xxxxx\u2502
R2:  \u2502xxxxx\u2502
R3:  \u2502xxxxx\u2502
R4:  \u2514\u2500\u2500\u2500\u2500\u2500\u2518
```

Interior cells are columns 1-5, rows 1-3.

**Single-character rank (A, 2-9, J, Q, K):**

| Position | Content |
|---|---|
| R1 C1 | rank char |
| R1 C2-C5 | space |
| R2 C1-C2 | space |
| R2 C3 | suit symbol |
| R2 C4-C5 | space |
| R3 C1-C3 | space |
| R3 C4 | rank char |
| R3 C5 | space |

Example -- King of Hearts:
```
\u250c\u2500\u2500\u2500\u2500\u2500\u2510
\u2502K    \u2502
\u2502  \u2665  \u2502
\u2502   K \u2502
\u2514\u2500\u2500\u2500\u2500\u2500\u2518
```

**Two-character rank (10):**

| Position | Content |
|---|---|
| R1 C1 | `1` |
| R1 C2 | `0` |
| R1 C3-C5 | space |
| R2 C1-C2 | space |
| R2 C3 | suit symbol |
| R2 C4-C5 | space |
| R3 C1-C2 | space |
| R3 C3 | `1` |
| R3 C4 | `0` |
| R3 C5 | space |

Example -- 10 of Spades:
```
\u250c\u2500\u2500\u2500\u2500\u2500\u2510
\u250210   \u2502
\u2502  \u2660  \u2502
\u2502  10 \u2502
\u2514\u2500\u2500\u2500\u2500\u2500\u2518
```

**Face-down card (dealer hole card):**

```
\u250c\u2500\u2500\u2500\u2500\u2500\u2510
\u2502\u2591\u2591\u2591\u2591\u2591\u2502
\u2502\u2591\u2591\u2591\u2591\u2591\u2502
\u2502\u2591\u2591\u2591\u2591\u2591\u2502
\u2514\u2500\u2500\u2500\u2500\u2500\u2518
```

Fill character: `\u2591` (Light Shade block). Rendered in `Color::GrayDark`.

**Spacing between cards**: 1 space between adjacent cards in a hand.

**Card overlap for width constraints**: When the total card width (cards * 7 + gaps) exceeds the available panel width, overlap cards so that only the leftmost 3 columns of each card (except the rightmost) are visible:

```
\u250c\u2500\u2500\u250c\u2500\u2500\u250c\u2500\u2500\u250c\u2500\u2500\u2500\u2500\u2500\u2510
\u2502K \u2502A \u25029 \u2502 7   \u2502
\u2502  \u2502  \u2502  \u2502  \u2663  \u2502
\u2502  \u2502  \u2502  \u2502   7 \u2502
\u2514\u2500\u2500\u2514\u2500\u2500\u2514\u2500\u2500\u2514\u2500\u2500\u2500\u2500\u2500\u2518
```

Overlap kicks in when card count exceeds `floor((panel_width - 2) / 8)` cards (where 8 = card width 7 + 1 gap). In overlap mode, each hidden card contributes 3 columns (left border + 2 interior chars), and the rightmost card renders fully (7 columns).

### 3.3 Layout Specifications

**Panel border style**: `ftxui::borderLight` (uses light box-drawing characters: `\u250c \u2500 \u2510 \u2502 \u2514 \u2518`). This matches the card borders for visual consistency.

**Overall structure** (vertical stack using `ftxui::vbox`):

```
\u250c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2510
\u2502                 BLACKJACK                 \u2502  <- Title bar
\u251c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2524
\u2502                                           \u2502
\u2502  Dealer's Hand                            \u2502  <- Dealer panel
\u2502  \u250c\u2500\u2500\u2500\u2500\u2500\u2510 \u250c\u2500\u2500\u2500\u2500\u2500\u2510                        \u2502
\u2502  \u2502K    \u2502 \u2502\u2591\u2591\u2591\u2591\u2591\u2502                        \u2502
\u2502  \u2502  \u2666  \u2502 \u2502\u2591\u2591\u2591\u2591\u2591\u2502                        \u2502
\u2502  \u2502   K \u2502 \u2502\u2591\u2591\u2591\u2591\u2591\u2502                        \u2502
\u2502  \u2514\u2500\u2500\u2500\u2500\u2500\u2518 \u2514\u2500\u2500\u2500\u2500\u2500\u2518                        \u2502
\u2502  Value: 10                                \u2502
\u2502                                           \u2502
\u251c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2524
\u2502                                           \u2502
\u2502  Your Hand                                \u2502  <- Player panel
\u2502  \u250c\u2500\u2500\u2500\u2500\u2500\u2510 \u250c\u2500\u2500\u2500\u2500\u2500\u2510                        \u2502
\u2502  \u2502A    \u2502 \u25027    \u2502                        \u2502
\u2502  \u2502  \u2660  \u2502 \u2502  \u2663  \u2502                        \u2502
\u2502  \u2502   A \u2502 \u2502   7 \u2502                        \u2502
\u2502  \u2514\u2500\u2500\u2500\u2500\u2500\u2518 \u2514\u2500\u2500\u2500\u2500\u2500\u2518                        \u2502
\u2502  Value: 18 (soft)                         \u2502
\u2502                                           \u2502
\u251c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2524
\u2502  [ (H)it ]  [ (S)tand ]                   \u2502  <- Controls
\u251c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2524
\u2502  Your turn                                \u2502  <- Status bar
\u2514\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2518
```

**Panel inner padding**: 2 spaces on left and right within each panel. 1 blank line above and below content within dealer and player panels. No vertical padding in title, controls, or status panels.

**Minimum width**: 40 columns. Below this, display a centered warning message:
```
Please resize terminal
     (40x20 minimum)
```

**Maximum width**: No maximum. Content is left-aligned within panels; panels stretch to fill terminal width via `ftxui::flex`.

**Minimum height**: 20 rows.

**Vertical spacing**:
- Title bar: 1 row of text content (plus borders)
- Dealer panel: label (1 row) + blank (1 row) + cards (5 rows) + value (1 row) + blank (1 row) = 9 rows content
- Player panel: same structure as dealer panel
- Controls panel: 1 row of buttons (plus borders)
- Status bar: 1 row of text (plus borders)

**Title centering**: Use `ftxui::center` to horizontally center the title text within the title bar panel.

**Flex behavior on resize**: The dealer and player panels use `ftxui::flex` to share available vertical space equally. Title, controls, and status bars have fixed heights. Cards are always drawn at their fixed 5x7 size; they do not scale.

### 3.4 Button Design

**Button label format**: Shortcut letter wrapped in parentheses within the label text, surrounded by spaces. The entire button is rendered inside `ftxui::border`.

```
 (H)it     (S)tand     S(p)lit     (N)ew Game     (Q)uit
```

The shortcut letter within parentheses is capitalized. The rest of the label is lowercase.

**FTXUI implementation**: Each button is a `ftxui::Button` component with a `ButtonOption` using `ButtonOption::Ascii()` style for clean borders.

**Button states:**

| State | Border | Text Color | FTXUI Decorators |
|---|---|---|---|
| Normal | `color(Color::GrayDark)` | `color(Color::White)` | `borderLight` |
| Focused | `color(Color::White)` | `color(Color::White) \| bold` | `borderLight \| inverted` or brighter border |
| Disabled | `color(Color::GrayDark)` | `color(Color::GrayDark)` | `borderLight \| dim` |

For focused state, use `ftxui::inverted` on the entire button element to provide a clear visual signal of which button has keyboard focus.

**Spacing between buttons**: 2 spaces between each button.

**Horizontal alignment**: Buttons are centered horizontally within the controls panel using `ftxui::hbox` wrapped in `ftxui::center`.

**Button sets by game state:**

| Game State | Visible Buttons |
|---|---|
| Player turn (no split available) | `(H)it`, `(S)tand` |
| Player turn (split available) | `(H)it`, `(S)tand`, `S(p)lit` |
| Round over | `(N)ew Game`, `(Q)uit` |

Buttons not in the active set are removed from the layout entirely (not shown disabled). This keeps the interface clean.

### 3.5 Status Bar Messages

The status bar is a single line of left-aligned text with 2 spaces of left padding.

| Game State | Message Text | Color |
|---|---|---|
| Player turn (single hand) | `Your turn` | `Color::GrayLight` |
| Player turn (split, hand 1) | `Your turn -- Hand 1` | `Color::GrayLight` |
| Player turn (split, hand 2) | `Your turn -- Hand 2` | `Color::GrayLight` |
| Hand 1 complete (stand), transitioning | `Hand 1 complete. Playing Hand 2...` | `Color::GrayLight` |
| Hand 1 complete (bust), transitioning | `Hand 1: Bust! Playing Hand 2...` | `Color::Red` |
| Dealer turn | `Dealer's turn...` | `Color::GrayLight` |
| Player wins | `You win!` | `Color::Green` |
| Player loses | `You lose.` | `Color::Red` |
| Push | `Push.` | `Color::Yellow` |
| Blackjack | `Blackjack!` | `Color::Green` + `bold` |
| Bust | `Bust!` | `Color::Red` |
| Split result | `Hand 1: Win \| Hand 2: Lose` | per-hand color (see below) |
| Invalid action | `Invalid action` | `Color::Red` |
| Terminal too small | `Please resize terminal (40x20 minimum)` | `Color::Yellow` |

**Split result color**: Each segment of the split result uses its own color. "Win" segments use `Color::Green`, "Lose" and "Bust" segments use `Color::Red`, "Push" segments use `Color::Yellow`. The pipe separator `|` uses `Color::GrayLight`.

### 3.6 Split Hand Layout

**Layout decision**: Side-by-side when terminal width >= 60 columns. Stacked (vertically) when terminal width is 40-59 columns.

**Side-by-side layout:**

```
\u251c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u252c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2524
\u2502                    \u2502                    \u2502
\u2502  Hand 1            \u2502  Hand 2            \u2502
\u2502  \u250c\u2500\u2500\u2500\u2500\u2500\u2510 \u250c\u2500\u2500\u2500\u2500\u2500\u2510  \u2502  \u250c\u2500\u2500\u2500\u2500\u2500\u2510 \u250c\u2500\u2500\u2500\u2500\u2500\u2510  \u2502
\u2502  \u25028    \u2502 \u25023    \u2502  \u2502  \u25028    \u2502 \u25026    \u2502  \u2502
\u2502  \u2502  \u2660  \u2502 \u2502  \u2665  \u2502  \u2502  \u2502  \u2663  \u2502 \u2502  \u2666  \u2502  \u2502
\u2502  \u2502   8 \u2502 \u2502   3 \u2502  \u2502  \u2502   8 \u2502 \u2502   6 \u2502  \u2502
\u2502  \u2514\u2500\u2500\u2500\u2500\u2500\u2518 \u2514\u2500\u2500\u2500\u2500\u2500\u2518  \u2502  \u2514\u2500\u2500\u2500\u2500\u2500\u2518 \u2514\u2500\u2500\u2500\u2500\u2500\u2518  \u2502
\u2502  Value: 11         \u2502  Value: 14         \u2502
\u2502                    \u2502                    \u2502
\u2514\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2518
```

Use `ftxui::hbox` with two equally-sized `ftxui::vbox` panels separated by a `ftxui::separator`.

**Stacked layout (narrow terminal):**

```
\u251c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2524
\u2502  Hand 1                                \u2502
\u2502  \u250c\u2500\u2500\u2500\u2500\u2500\u2510 \u250c\u2500\u2500\u2500\u2500\u2500\u2510                    \u2502
\u2502  ...                                   \u2502
\u2502  Value: 11                             \u2502
\u251c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2524
\u2502  Hand 2                                \u2502
\u2502  \u250c\u2500\u2500\u2500\u2500\u2500\u2510 \u250c\u2500\u2500\u2500\u2500\u2500\u2510                    \u2502
\u2502  ...                                   \u2502
\u2502  Value: 14                             \u2502
\u2514\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2518
```

**Active hand indicator**: The active hand's sub-panel uses a brighter border color (`Color::White`) while the inactive hand uses the default dim border (`Color::GrayDark`). The active hand's label is also rendered in bold.

Implementation: Apply `color(Color::White) | bold` to the active hand's border and label. Apply `color(Color::GrayDark) | dim` to the inactive hand's border and label.

**Hand labels**: `Hand 1` and `Hand 2`, placed as the first line inside each sub-panel, left-aligned with the 2-space left padding.

### 3.7 Animation and Timing

**Dealer card draw delay**: 500ms pause between each dealer card draw. Implemented via `std::this_thread::sleep_for(std::chrono::milliseconds(500))` between screen updates within the dealer-play sequence.

During the dealer turn:
1. Hole card flip: Replace face-down card graphic with face-up card. Render. Wait 500ms.
2. Each subsequent draw: Add card to hand display. Render. Wait 500ms.
3. Final state: Show dealer standing or busting.

**Card deal on new round**: No animation delay for the initial deal. All four cards (2 dealer, 2 player) appear simultaneously on the first render.

**Hand transition (split)**: When transitioning from Hand 1 to Hand 2, the active highlight moves immediately (no delay). The status bar message updates simultaneously.

**No other transitions or effects**. No fade-in, no slide, no blinking. Clean instant updates except for the dealer draw sequence.

---

## 4. Accessibility Notes

### 4.1 Contrast
- All text uses high-contrast pairings: white or bright colors on the default terminal background (assumed dark). The design avoids medium-gray text for any critical information.
- The `Color::GrayDark` usage is limited to borders and disabled elements -- never for hand values, card content, or status messages.
- Red text (`Color::Red`) for hearts/diamonds is distinguishable from white text on both dark and light terminal backgrounds.

### 4.2 Color Is Not the Only Signal
- Game outcomes are conveyed through text ("You win!", "You lose.", "Push.", "Bust!") in addition to color coding. A player who cannot perceive color differences can still read the result.
- Card suits are displayed as Unicode symbols (the shape differs), not only by color.
- The active hand in split mode is indicated by both a brighter border AND a bold label -- two independent visual signals.
- Bust is indicated by the word "BUST" (CLI) or "Bust!" in the status bar (FTXUI) in addition to red coloring.

### 4.3 Screen Reader Compatibility
- The CLI frontend outputs plain text to stdout, which is inherently screen-reader compatible. No special accommodations are needed.
- The FTXUI frontend is a TUI application that renders to a terminal buffer. Screen reader compatibility with FTXUI is limited by the library and terminal emulator. No special screen reader integration is in scope for Phase 1. The CLI frontend serves as the accessible alternative.

### 4.4 Keyboard-Only Navigation
- **CLI**: Fully keyboard-operated by design. All input is text commands typed at a prompt.
- **FTXUI**: Fully keyboard-operated. All actions are accessible via single-key shortcuts (H, S, P, N, Q, Esc). Tab/Arrow navigation between buttons is also supported. No mouse interaction is required or implemented.
- **Focus visibility**: The focused button in FTXUI uses `inverted` rendering, providing a clear visual indicator of the current keyboard focus position.
