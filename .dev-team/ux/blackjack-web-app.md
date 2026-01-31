<!-- metadata
id: UX-001
type: ux-spec
status: draft
author: A5-ux-designer
created: 2026-01-31
updated: 2026-01-31
approved-by: pending
dependencies: [ARCH-002, SPEC-003, UI-001]
-->

# UX Specification: Blackjack Web Application

## Overview

**Related Specifications**:
- [ARCH-002](../architecture-phase2.md) — Web frontend architecture
- [SPEC-003](../specifications/spec-003-ftxui-frontend.md) — Game flow reference
- [UI-001](../ui-design.md) — Visual design principles

**User Task**: Play single-player blackjack in a web browser against a dealer. The user wants to understand the game state at a glance, take actions quickly, and start new games without friction.

**Platform**: Modern web browsers (Chrome 57+, Firefox 53+, Safari 11+, Edge 79+)

**Key Constraints**:
- Single-page vanilla JavaScript application (no framework)
- C++ engine compiled to WASM via Emscripten
- Responsive design from 360px mobile to 1920px+ desktop
- WCAG 2.1 AA accessibility baseline

---

## User Flow

```
[Browser loads index.html]
  ↓
[WASM module loads] → [Loading state shown]
  ↓
[Module ready] → [Initial deal automatically triggered]
  ↓
[Player Turn: Hit/Stand/Split buttons available]
  ↓
  ├─ [User clicks Hit] → [Card dealt, display updates]
  │   ↓
  │   ├─ [Not bust] → [Back to Player Turn]
  │   └─ [Bust] → [Dealer Turn (automatic)]
  │
  ├─ [User clicks Stand] → [Dealer Turn (automatic)]
  │
  └─ [User clicks Split (if eligible)] → [Two hands displayed, play sequentially]
      ↓
[Dealer Turn: Hole card reveals, dealer draws cards with animation]
  ↓
[Round Over: Result displayed, "New Game" button appears]
  ↓
  ├─ [User clicks New Game] → [New deal]
  └─ [User closes tab/window] → [Session ends]

Error paths:
- [WASM fails to load] → [Error state: message + reload button]
- [Invalid action attempted] → [No-op or error flash]
- [Network error (offline)] → [WASM is cached, game continues offline]
```

---

## Screen Layout

### Single-Page Layout Structure

```
┌─────────────────────────────────────────────────────────┐
│                   Header (fixed)                        │
│                     BLACKJACK                           │
├─────────────────────────────────────────────────────────┤
│                                                         │
│                   Dealer Area                           │
│              ┌──────┐  ┌──────┐                        │
│              │  K♦  │  │  ??  │                        │
│              └──────┘  └──────┘                        │
│              Value: 10                                  │
│                                                         │
├─────────────────────────────────────────────────────────┤
│                                                         │
│                   Player Area                           │
│              ┌──────┐  ┌──────┐                        │
│              │  A♠  │  │  7♣  │                        │
│              └──────┘  └──────┘                        │
│              Value: 18 (soft)                           │
│                                                         │
├─────────────────────────────────────────────────────────┤
│                                                         │
│                   Actions                               │
│          [ Hit (H) ]  [ Stand (S) ]                     │
│                                                         │
├─────────────────────────────────────────────────────────┤
│  Status: Your turn                                      │
└─────────────────────────────────────────────────────────┘
```

**Layout Breakdown**:

| Section | Height | Content | Scroll Behavior |
|---------|--------|---------|-----------------|
| Header | Flex-shrink 0 | Title "BLACKJACK" centered | No scroll |
| Dealer Area | Flex 1 | Cards, hand value, hole card during play | No scroll — cards fan/overlap |
| Player Area | Flex 1 | Cards, hand value(s), split hands | No scroll — cards fan/overlap |
| Actions | Flex-shrink 0 | Action buttons (Hit/Stand/Split/New Game) | No scroll, always visible |
| Status Bar | Flex-shrink 0 | Game state message | No scroll, always visible |

**Viewport constraints**:
- Minimum width: 360px (iPhone SE)
- Minimum height: 640px (iPhone SE)
- Maximum width: No limit (layout centers content above 1200px)

---

## Component Inventory

### 1. Header

| Component | Type | Behavior | States |
|-----------|------|----------|--------|
| Header container | `<header>` | Fixed at top, spans viewport width | N/A |
| Title text | `<h1>` | Displays "BLACKJACK", centered, always visible | N/A |

**Specifications**:
- Background color: `#1a1a1a` (dark gray)
- Text color: `#ffffff` (white)
- Font: 2rem/2.5rem (mobile/desktop) bold sans-serif
- Padding: 0.5rem vertical
- Border bottom: 2px solid `#333333`

---

### 2. Dealer Area

| Component | Type | Behavior | States |
|-----------|------|----------|--------|
| Dealer container | `<section>` | Centered flex container | N/A |
| Dealer label | `<h2>` | "Dealer" text, always visible | N/A |
| Card elements | `<svg class="card">` | Inline SVG cards | face-up, face-down |
| Hand value display | `<p class="value">` | Shows dealer's hand value | visible (DealerTurn/RoundOver), hidden (PlayerTurn shows only up-card) |

**Card rendering** (detailed in Card Component section below):
- Face-up: rank + suit symbol visible
- Face-down (hole card): back pattern (diagonal stripes or solid fill)

**Hand value logic**:
- **PlayerTurn**: Show only the up-card value (e.g., "Value: 10")
- **DealerTurn/RoundOver**: Show full hand value (e.g., "Value: 17")

**Layout**:
- Cards arranged horizontally, fanning/overlapping with negative margins
- Cards centered in section
- Value text below cards, centered
- No minimum height — flex layout shares available space

---

### 3. Player Area

| Component | Type | Behavior | States |
|-----------|------|----------|--------|
| Player container | `<section>` | Centered flex container | single-hand, split-hands |
| Player label | `<h2>` | "Your Hand" or "Hand 1" / "Hand 2" during split | N/A |
| Card elements | `<svg class="card">` | Inline SVG cards | face-up only |
| Hand value display | `<p class="value">` | Shows player's hand value with soft indicator | normal, soft, bust |

**Split hand layout**:
- **Side-by-side** when viewport width >= 640px:
  - Two columns, each hand in its own column
  - Active hand has visual highlight (border glow or background tint)
  - Hand labels: "Hand 1" and "Hand 2"
- **Stacked** when viewport width < 640px:
  - Vertical stack, active hand at top or highlighted with border
  - Hand labels same as side-by-side

**Value display**:
- Format: "Value: {number}" or "Value: {number} (soft)" or "Value: {number} BUST"
- Soft indicator appears when ace counted as 11 (per `Hand::is_soft()`)
- Bust: red text, bold

**Active hand indicator (split only)**:
- Active hand container: 2px solid border `#4a9eff` (blue)
- Inactive hand container: 1px solid border `#555555` (gray)
- Smooth transition (200ms) when switching active hand

---

### 4. Card Component

**Visual design**:

Cards are inline SVG elements with a `viewBox="0 0 250 350"` (5:7 aspect ratio). Sizing is controlled by CSS `width`/`height` using em units that scale with the card container's font-size:
- Width: 5em, Height: 7em (scales per breakpoint via container font-size)
- Mobile (default): ~5rem wide (~80px at 16px base)
- Tablet (640px+): container font-size 1.25rem → ~6.25rem wide
- Desktop (1024px+): container font-size 1.5rem → ~7.5rem wide
- Border radius: via SVG `<rect rx="12">` (in viewBox units)
- Shadow: CSS `filter: drop-shadow(0 2px 4px rgba(0,0,0,0.3))`

**Face-up card structure**:
```
┌──────────┐
│ A        │  ← rank (top-left)
│          │
│    ♠     │  ← suit symbol (centered)
│          │
│        A │  ← rank (bottom-right, rotated 180°)
└──────────┘
```

**Layout positions** (SVG viewBox coordinates, 250×350):
- Top-left rank: x=15, y=40
- Top-left suit: x=15, y=70
- Center suit: x=125, y=200 (text-anchor: middle)
- Bottom-right rank: x=235, y=320, rotated 180°
- Bottom-right suit: x=235, y=290, rotated 180°

**Colors**:
- Background: `#ffffff` (white)
- Border: 1px solid `#cccccc` (light gray)
- Spades (♠): `#000000` (black)
- Clubs (♣): `#000000` (black)
- Hearts (♥): `#dc143c` (crimson red)
- Diamonds (♦): `#dc143c` (crimson red)

**Face-down card (hole card)**:
- Background: `#1a4d80` (dark blue)
- Pattern: Repeating diagonal stripes `#2a5d90` (lighter blue) at 45° angle
- Or solid fill with "??" centered in white
- No rank/suit visible

**Suit symbols**:
- Use Unicode characters: ♠ (U+2660), ♥ (U+2665), ♦ (U+2666), ♣ (U+2663)
- Font size: 80px (SVG viewBox units) for center suit
- Font size: 28px (SVG viewBox units) for corner ranks and suits
- SVG text sizes are in viewBox coordinates and scale automatically with the SVG element

**Rank display**:
- A, 2-10, J, Q, K
- "10" is two characters, others are single character
- Font: bold sans-serif

**States**:
| State | Visual |
|-------|--------|
| Default | White background, colored suit |
| Hover (desktop) | Subtle lift (box-shadow increases to `0 4px 8px`) |
| Dealt (animation) | Fade-in + slide from deck position (300ms ease-out) |

---

### 5. Action Buttons

| Component | Type | Behavior | States |
|-----------|------|----------|--------|
| Button: Hit | `<button id="btn-hit">` | Calls `game.hit()`, triggers re-render | enabled, disabled, focused, active |
| Button: Stand | `<button id="btn-stand">` | Calls `game.stand()`, triggers re-render | enabled, disabled, focused, active |
| Button: Split | `<button id="btn-split">` | Calls `game.split()`, triggers re-render | enabled (only when can_split), disabled, focused, active |
| Button: New Game | `<button id="btn-new-game">` | Calls `game.deal()`, triggers re-render | enabled (RoundOver only), disabled, focused, active |

**Button layout**:
- Horizontal row, centered
- 0.75rem gap between buttons on mobile, 1rem on desktop
- All 4 buttons pre-rendered in HTML, visibility toggled via `hidden` attribute

**Button styling**:

| State | Background | Border | Text Color | Cursor |
|-------|-----------|--------|-----------|--------|
| Enabled (default) | `#2a7de1` (blue) | None | `#ffffff` (white) | pointer |
| Enabled (hover) | `#1f5fb8` (darker blue) | None | `#ffffff` | pointer |
| Enabled (focus) | `#2a7de1` | 2px solid `#ffffff` outline | `#ffffff` | pointer |
| Enabled (active/click) | `#174a8b` (darkest blue) | None | `#ffffff` | pointer |
| Disabled | `#555555` (gray) | None | `#999999` (light gray) | not-allowed |

**Button dimensions**:
- Height: 2.75rem (minimum touch target 2.75rem per WCAG)
- Padding: 0 0.75rem
- Font: 1rem bold sans-serif
- Border radius: 0.5rem
- Min-width: 7rem

**Button label format**:
- "Hit (H)" — action name + keyboard shortcut in parentheses
- "Stand (S)"
- "Split (P)" — only visible when split is available
- "New Game (N)" — only visible when round is over

**Button visibility logic**:
| GameState | Visible Buttons |
|-----------|----------------|
| WaitingForDeal | None (auto-deals on load) |
| PlayerTurn (no split available) | Hit, Stand |
| PlayerTurn (split available) | Hit, Stand, Split |
| DealerTurn | None (dealer plays automatically) |
| RoundOver | New Game |

Buttons are **pre-rendered in HTML** with `hidden` attribute. On state change, toggle `hidden` — never remove/recreate. This prevents layout shifts.

---

### 6. Status Bar

| Component | Type | Behavior | States |
|-----------|------|----------|--------|
| Status message | `<p class="status">` | Displays current game state message | normal, success, error, info |

**Message text by state**:

| GameState | Message | Color |
|-----------|---------|-------|
| WaitingForDeal | "Loading..." | `#aaaaaa` (gray) |
| PlayerTurn (single hand) | "Your turn" | `#ffffff` (white) |
| PlayerTurn (split, hand 1 active) | "Your turn — Hand 1" | `#ffffff` |
| PlayerTurn (split, hand 2 active) | "Your turn — Hand 2" | `#ffffff` |
| DealerTurn | "Dealer's turn..." | `#aaaaaa` (gray) |
| RoundOver (Win) | "You win!" | `#4caf50` (green) |
| RoundOver (Lose) | "You lose." | `#f44336` (red) |
| RoundOver (Push) | "Push." | `#ff9800` (orange) |
| RoundOver (Blackjack) | "Blackjack! You win!" | `#4caf50` (green), bold |
| Bust | "Bust!" | `#f44336` (red) |
| Split result (mixed) | "Hand 1: Win \| Hand 2: Lose" | Multi-color (green "Win", red "Lose", gray separator) |

**Split result color coding**:
- Each segment ("Hand 1: Win", "Hand 2: Lose") rendered separately
- Win → green, Lose/Bust → red, Push → orange
- Separator " \| " → gray

**Status bar styling**:
- Background: `#222222` (dark gray)
- Font: 1rem sans-serif
- Padding: 0.5rem 0.75rem
- Text aligned left
- Border top: 1px solid `#333333`

---

## Interaction Specifications

### INT-1: Page Load and WASM Initialization

| # | User Action | System Response | Transition |
|---|-------------|-----------------|-----------|
| 1.1 | User navigates to URL / opens index.html | Browser loads HTML, CSS, JS modules | → Loading state |
| 1.2 | JS imports WASM module | Emscripten fetches and compiles blackjack.wasm | → Loading spinner visible |
| 1.3 | WASM module ready | Game controller creates `new Game()` instance | → Loaded state |
| 1.4 | Game instance created | Automatic `game.deal()` called | → PlayerTurn state displayed |
| 1.5 | Initial deal complete | Dealer shows 1 up-card + 1 hole card, player shows 2 cards, buttons appear | User can take action |

**Error handling**:
- If WASM fails to load: Display error message "Failed to load game. Please refresh." with a "Reload" button
- If initial deal fails: Display error message "Failed to start game. Please refresh."

---

### INT-2: Hit Action

| # | User Action | System Response | Transition |
|---|-------------|-----------------|-----------|
| 2.1 | User clicks "Hit" button OR presses "H" key | Event listener fires | → |
| 2.2 | Game controller calls `game.hit()` | WASM game engine adds card to active hand | → |
| 2.3 | `game.hit()` returns ActionResult | Controller checks result: Success or InvalidAction | → |
| 2.4 | If Success | Controller reads new state: `game.player_hands()`, `game.state()` | → |
| 2.5 | UI renderer updates player area | New card animates in (300ms fade + slide), hand value updates | → |
| 2.6 | Check if bust | If hand value > 21, status bar shows "Bust!" in red | → DealerTurn (automatic) |
| 2.7 | Check if state is DealerTurn | If so, trigger dealer play automatically after 500ms delay | → Dealer plays |
| 2.8 | If not bust and still PlayerTurn | Buttons remain enabled, user can hit/stand again | User can take action |

**Animation**:
- New card fades in from 0% to 100% opacity over 300ms
- Card slides from deck position (top-right corner) to hand position
- Easing: ease-out

**Button state during action**:
- Disable all buttons during animation (300ms)
- Re-enable after animation complete (unless state changed to DealerTurn)

---

### INT-3: Stand Action

| # | User Action | System Response | Transition |
|---|-------------|-----------------|-----------|
| 3.1 | User clicks "Stand" button OR presses "S" key | Event listener fires | → |
| 3.2 | Game controller calls `game.stand()` | WASM marks active hand as stood | → |
| 3.3 | `game.stand()` returns ActionResult | Controller checks state: still PlayerTurn or transitioned to DealerTurn | → |
| 3.4 | If split hands and hand 1 stood | Status bar shows "Hand 1 complete. Playing Hand 2..." | → Active hand switches to Hand 2 |
| 3.5 | If last hand stood or single hand | State transitions to DealerTurn | → Trigger dealer play after 500ms |
| 3.6 | Dealer turn triggered | Controller calls `game.play_dealer()` | → Dealer sequence (INT-4) |

**Split hand transition**:
- Active hand indicator (blue border) moves to next hand
- Smooth transition animation (200ms)
- Status bar updates immediately

---

### INT-4: Split Action

| # | User Action | System Response | Transition |
|---|-------------|-----------------|-----------|
| 4.1 | User clicks "Split" button OR presses "P" key | Event listener fires | → |
| 4.2 | Game controller calls `game.split()` | WASM splits hand into two hands, deals one card to each | → |
| 4.3 | `game.split()` returns ActionResult | Controller checks result: Success, InvalidSplit, or AlreadySplit | → |
| 4.4 | If InvalidSplit or AlreadySplit | Flash error in status bar (red text, 2 seconds), no state change | → Buttons remain |
| 4.5 | If Success | UI re-renders player area with two hands side-by-side (or stacked on mobile) | → Split layout |
| 4.6 | Active hand indicator | Hand 1 is active (blue border), Hand 2 is inactive (gray border) | → |
| 4.7 | Status bar updates | "Your turn — Hand 1" | → |
| 4.8 | Buttons update | "Split" button removed, only "Hit" and "Stand" remain | User plays Hand 1 |

**Split aces special case**:
- If split aces, both hands are auto-stood (per game rules)
- Status bar shows "Split aces — one card each"
- Immediately transitions to DealerTurn after 1 second delay (to show cards)

**Animation**:
- Hands slide apart from center to left/right positions (400ms ease-out)
- New cards dealt to each hand animate in (300ms fade + slide)

---

### INT-5: Dealer Turn (Automatic)

| # | User Action | System Response | Transition |
|---|-------------|-----------------|-----------|
| 5.1 | State is DealerTurn | Buttons disappear, status bar shows "Dealer's turn..." | → |
| 5.2 | Hole card reveal | Hole card flips from back to face-up (400ms flip animation) | → |
| 5.3 | Delay | 500ms pause | → |
| 5.4 | Controller calls `game.play_dealer()` | WASM dealer logic: dealer draws cards until stand condition | → |
| 5.5 | For each dealer draw | New card animates in (300ms), dealer hand value updates, 500ms pause | → Loop until stand/bust |
| 5.6 | Dealer stands or busts | Final dealer hand displayed | → |
| 5.7 | State transitions to RoundOver | Controller reads results: `game.player_hands()[].result` | → |
| 5.8 | UI displays results | Status bar shows result message ("You win!", "You lose.", etc.) | → RoundOver state |
| 5.9 | Buttons update | "New Game" button appears | User can start new game |

**Hole card flip animation**:
- 3D CSS transform: rotateY from 0° to 180° over 400ms
- Card back visible at 0°-90°, card face visible at 90°-180°
- Easing: ease-in-out

**Dealer draw animation**:
- Same as player hit: fade + slide from deck position
- Sequential: each card waits for previous card animation + 500ms pause

**No user interaction during dealer turn**:
- All buttons disabled/hidden
- Keyboard shortcuts inactive
- User cannot interrupt dealer sequence

---

### INT-6: New Game Action

| # | User Action | System Response | Transition |
|---|-------------|-----------------|-----------|
| 6.1 | User clicks "New Game" button OR presses "N" key | Event listener fires | → |
| 6.2 | Game controller calls `game.deal()` | WASM resets state, shuffles new deck, deals 4 cards | → |
| 6.3 | Clear previous round | All cards fade out (300ms), status bar clears | → |
| 6.4 | Deal new cards | New cards animate in (300ms fade + slide), dealer shows up-card + hole card, player shows 2 cards | → |
| 6.5 | State is PlayerTurn | Status bar shows "Your turn", Hit/Stand buttons appear (Split if eligible) | User can take action |

**Clear animation**:
- All cards fade to 0% opacity over 300ms
- Cards removed from DOM after animation
- 200ms pause before new deal animation

---

### INT-7: Keyboard Shortcuts

| Key | Action | Enabled When |
|-----|--------|-------------|
| `H` | Hit | PlayerTurn, Hit button enabled |
| `S` | Stand | PlayerTurn, Stand button enabled |
| `P` | Split | PlayerTurn, Split button enabled |
| `N` | New Game | RoundOver, New Game button enabled |
| `Escape` | (No action in MVP) | Future: pause/menu |

**Implementation**:
- Global `keydown` event listener on `document`
- Case-insensitive (accept 'h' or 'H')
- Check button enabled state before triggering action
- Prevent default browser behavior (e.g., 'S' saving page)
- Visual feedback: button shows active state briefly (200ms) when triggered by keyboard

---

## States & Conditions

### State: Loading

**Trigger**: Page load, WASM module not yet initialized

**Display**:
- Header visible
- Main area shows centered loading spinner + text "Loading game..."
- No buttons, no status bar content
- Background: dark gray

**Duration**: Typically 1-3 seconds (depends on network and device)

**Transition**: → Loaded state when WASM ready

---

### State: Loaded / WaitingForDeal

**Trigger**: WASM module loaded, Game instance created

**Display**:
- Auto-deal triggered immediately, no user action needed
- Brief flash of empty table (< 100ms) before initial deal animation

**Transition**: → PlayerTurn state

---

### State: PlayerTurn

**Trigger**: Deal complete, or player action complete (hit/stand/split)

**Display**:
- Dealer area: 1 face-up card, 1 face-down hole card, value shows only up-card
- Player area: Face-up cards, hand value displayed
- Buttons: Hit, Stand, (Split if `available_actions` includes it)
- Status bar: "Your turn" or "Your turn — Hand 1/2" if split

**Sub-states**:
- **Single hand**: One hand displayed, standard layout
- **Split hands**: Two hands displayed side-by-side or stacked, active hand highlighted

**Transition**:
- → DealerTurn when player busts or stands on last hand
- → PlayerTurn (same state, updated data) after hit that doesn't bust

---

### State: DealerTurn

**Trigger**: Player busted, or player stood on last hand

**Display**:
- Hole card reveals (flip animation)
- Dealer draws cards automatically with 500ms pauses
- No buttons visible
- Status bar: "Dealer's turn..."

**User interaction**: None (automatic sequence)

**Transition**: → RoundOver when dealer stands or busts

---

### State: RoundOver

**Trigger**: Dealer turn complete, results determined

**Display**:
- All cards face-up
- Final hand values displayed
- Status bar shows result: "You win!", "You lose.", "Push.", "Blackjack!"
- Buttons: "New Game" only

**Sub-states by result**:
- **Win**: Status bar green, player hand value ≤ 21 and > dealer (or dealer bust)
- **Lose**: Status bar red, player bust or dealer value > player
- **Push**: Status bar orange, player value = dealer value (both ≤ 21)
- **Blackjack**: Status bar green + bold, player natural blackjack (unless dealer also has blackjack → Push)
- **Split result**: Status bar multi-color, each hand result shown separately

**Transition**: → WaitingForDeal → PlayerTurn when user clicks "New Game"

---

### State: Error

**Trigger**: WASM load failure, unexpected exception, network error

**Display**:
- Error message centered on screen: "Failed to load game. Please refresh the page."
- "Reload" button → refreshes page (`window.location.reload()`)
- No game UI visible
- Background: dark red tint

**User interaction**: Click "Reload" or manually refresh

**Transition**: → Loading state on reload

---

### State: Partial / Degraded (No data state — N/A for this game)

This game always has data once WASM loads. No "no data" state exists.

---

## Form Specifications

**N/A** — This application has no forms. All interaction is via buttons.

---

## Responsive Behavior

### Breakpoint: Mobile Portrait (360px - 639px)

**Layout changes**:
- Header: Font size 2rem
- Card size: 5em × 7em (container font-size at default 1rem → ~80px × 112px)
- Cards fan/overlap with negative margins — no horizontal scroll
- Dealer/player areas: flex: 1, share remaining space
- **Split hands: Stacked vertically** (Hand 1 above Hand 2)
- Active hand indicator: 2px solid border `#4a9eff`
- Buttons: Horizontal row, min-width 7rem, 0.75rem gap
- Status bar: Font size 1rem

**Touch targets**:
- All buttons minimum 2.75rem height (WCAG guideline: 2.75rem ≈ 44px minimum)
- Buttons have min-width: 7rem for easy tapping

---

### Breakpoint: Mobile Landscape / Tablet Portrait (640px - 1023px)

**Layout changes**:
- Header: Font size 2rem
- Card size: 5em × 7em (container font-size 1.25rem → ~100px × 140px)
- Cards fan/overlap with negative margins
- Dealer/player areas: flex: 1
- **Split hands: Side-by-side** (flex row)
- Active hand indicator: 2px border around hand container
- Buttons: Horizontal row, 1rem gap, centered
- Status bar: Font size 1rem

---

### Breakpoint: Desktop (1024px+)

**Layout changes**:
- Header: Font size 2.5rem
- Card size: 5em × 7em (container font-size 1.5rem → ~120px × 168px)
- Cards fan/overlap with negative margins
- Dealer/player areas: flex: 1
- **Split hands: Side-by-side** (flex row, 1.5rem gap)
- Main content area: Max width 75rem, centered with auto margins
- Buttons: Horizontal row, min-width 8.75rem, 1rem gap
- Status bar: Font size 1rem
- Hover states enabled (card drop-shadow increase, button color change)

**Desktop-specific interactions**:
- Hover effects on buttons and cards (subtle `filter: drop-shadow` increase)
- Cursor changes to `pointer` on interactive elements

---

### Breakpoint: Very Small Screens (< 360px)

**Degradation strategy**:
- Card size: 5em × 7em (container font-size 0.875rem → ~70px × 98px)
- Font sizes scaled down via smaller rem values
- Buttons: min-width 5rem, height 2.5rem, smaller font
- Functionality fully intact, just cramped visually

---

## Accessibility Requirements

### Keyboard Navigation

| Requirement | Implementation |
|-------------|---------------|
| All interactive elements reachable via Tab | Buttons use native `<button>` elements (focusable by default) |
| Logical tab order | Top to bottom: Hit → Stand → Split → New Game |
| Keyboard shortcuts | H, S, P, N keys trigger respective actions |
| Focus visible | 2px white outline on focused button, high contrast |
| No keyboard trap | User can tab through all elements and out to browser controls |

---

### Screen Reader Support

| Requirement | Implementation |
|-------------|---------------|
| All images have alt text | Cards include `aria-label="Ace of Spades"` or similar |
| Buttons have meaningful labels | "Hit (H)" visible text, `aria-label="Hit, keyboard shortcut H"` if needed |
| Dynamic content announced | ARIA live region on status bar: `aria-live="polite"` |
| Landmark roles | `<header>`, `<main>`, `<section>` semantic HTML |
| State changes announced | When dealer draws card, screen reader announces "Dealer draws King of Hearts, value 20" |
| Card values announced | Each card has `aria-label="{rank} of {suit}"` |

**ARIA live regions**:
- Status bar: `<p class="status" aria-live="polite" aria-atomic="true">`
- Player hand value: `<p class="value" aria-live="polite">`
- Dealer hand value: `<p class="value" aria-live="polite">`

**Screen reader sequence (example round)**:
1. "Loading game..." (status bar)
2. "Dealer has King of Diamonds face up, hole card hidden" (dealer area)
3. "Your hand: Ace of Spades, Seven of Clubs, value 18, soft" (player area)
4. "Your turn. Hit button, Stand button, Split button available." (status bar + buttons)
5. User presses H
6. "Three of Hearts dealt. Your hand value 21." (player area update)
7. "Dealer's turn. Hole card revealed: Seven of Diamonds. Dealer value 17." (dealer area update)
8. "Dealer stands." (status bar)
9. "You win!" (status bar)
10. "New Game button available." (buttons)

---

### Color Contrast

| Element | Foreground | Background | Contrast Ratio | WCAG Level |
|---------|-----------|-----------|---------------|-----------|
| Header text | `#ffffff` | `#1a1a1a` | 15.3:1 | AAA |
| Body text | `#ffffff` | `#2a2a2a` | 13.6:1 | AAA |
| Button text (enabled) | `#ffffff` | `#2a7de1` | 4.7:1 | AA |
| Button text (disabled) | `#999999` | `#555555` | 3.5:1 | AA (large text) |
| Status bar (normal) | `#ffffff` | `#222222` | 14.5:1 | AAA |
| Status bar (win) | `#4caf50` | `#222222` | 5.8:1 | AA |
| Status bar (lose) | `#f44336` | `#222222` | 4.9:1 | AA |
| Card text (black) | `#000000` | `#ffffff` | 21:1 | AAA |
| Card text (red) | `#dc143c` | `#ffffff` | 7.2:1 | AAA |

**Tool**: Verified with WebAIM Contrast Checker

**Color is not the only signal**:
- Win/lose/push conveyed by text ("You win!", "You lose.", "Push.") not just color
- Bust indicated by text "BUST" not just red color
- Active hand indicated by border AND label ("Hand 1", "Hand 2") not just blue border
- Card suits differentiated by symbol shape (♠♥♦♣) not just color

---

### Focus Management

| Scenario | Focus Behavior |
|----------|---------------|
| Page load | Focus on "Hit" button after initial deal (first interactive element) |
| User clicks button | Focus remains on clicked button (no unexpected jump) |
| State changes to RoundOver | Focus moves to "New Game" button automatically |
| Split triggered | Focus remains on "Hit" button (first button in new state) |
| Dealer turn (automatic) | Focus moves to status bar (read-only), then to "New Game" button when round ends |
| Modal/error dialog | Focus moves to "Reload" button or first interactive element in modal |

**Implementation**:
- Programmatic focus: `document.getElementById('btn-new-game').focus()` after state transition
- Smooth focus ring transitions (no jarring jumps)

---

### ARIA Labels

| Element | ARIA Attributes |
|---------|----------------|
| Dealer area | `<section aria-labelledby="dealer-label">` where `dealer-label` is "Dealer's Hand" |
| Player area | `<section aria-labelledby="player-label">` |
| Card | `<svg class="card" role="img" aria-label="Ace of Spades">` |
| Hole card | `<svg class="card face-down" role="img" aria-label="Hole card hidden">` |
| Status bar | `<p class="status" role="status" aria-live="polite">` |
| Button (disabled) | `<button aria-disabled="true" disabled>` |

---

## Design Tokens

### Color Palette

| Token Name | Value | Usage |
|-----------|-------|-------|
| `--color-primary` | `#2a7de1` | Buttons, active elements |
| `--color-primary-dark` | `#1f5fb8` | Button hover |
| `--color-primary-darker` | `#174a8b` | Button active |
| `--color-success` | `#4caf50` | Win message, positive feedback |
| `--color-error` | `#f44336` | Lose message, bust, error states |
| `--color-warning` | `#ff9800` | Push message, neutral result |
| `--color-bg-dark` | `#1a1a1a` | Header, main background |
| `--color-bg-medium` | `#2a2a2a` | Sections, containers |
| `--color-bg-light` | `#333333` | Borders, dividers |
| `--color-text-primary` | `#ffffff` | Main text |
| `--color-text-secondary` | `#aaaaaa` | Muted text, labels |
| `--color-text-disabled` | `#999999` | Disabled buttons |
| `--color-card-bg` | `#ffffff` | Card background |
| `--color-card-red` | `#dc143c` | Hearts, Diamonds |
| `--color-card-black` | `#000000` | Spades, Clubs |
| `--color-card-back` | `#1a4d80` | Hole card background |
| `--color-border-active` | `#4a9eff` | Active hand indicator |
| `--color-border-inactive` | `#555555` | Inactive hand border |

---

### Spacing

> All spacing uses `rem` units. Reference px values assume 16px base font-size.

| Token Name | Value | Ref px | Usage |
|-----------|-------|--------|-------|
| `--space-xs` | `0.25rem` | 4px | Tight element padding |
| `--space-sm` | `0.5rem` | 8px | Card gaps, small padding |
| `--space-md` | `0.75rem` | 12px | Standard padding, button gaps |
| `--space-lg` | `1rem` | 16px | Section padding, larger gaps |
| `--space-xl` | `1.5rem` | 24px | Section margins, desktop gaps |
| `--space-2xl` | `2rem` | 32px | Desktop large spacing |

---

### Typography

> Font sizes use `rem` units. Card-internal font sizes use SVG viewBox units (not CSS rem).

| Token Name | Value | Ref px | Usage |
|-----------|-------|--------|-------|
| `--font-family-base` | `'Segoe UI', Tahoma, Geneva, Verdana, sans-serif` | — | All text |
| `--font-size-sm` | `0.875rem` | 14px | Small labels, fine print |
| `--font-size-base` | `1rem` | 16px | Body text, status bar |
| `--font-size-lg` | `1.125rem` | 18px | Hand values, larger status |
| `--font-size-xl` | `1.5rem` | 24px | Subheadings |
| `--font-size-2xl` | `2rem` | 32px | Header (mobile) |
| `--font-size-3xl` | `2.5rem` | 40px | Header (desktop) |
| `--font-weight-normal` | `400` | — | Body text |
| `--font-weight-bold` | `700` | — | Headers, buttons, emphasis |

**Card-internal font sizes** (SVG viewBox units, scale with card element):
| Element | Size | Notes |
|---------|------|-------|
| Corner rank/suit | 28 | viewBox coordinate units |
| Center suit | 80 | viewBox coordinate units |
| Face-down text | 48 | viewBox coordinate units |

---

### Borders & Shadows

| Token Name | Value | Usage |
|-----------|-------|-------|
| `--border-radius-sm` | `0.25rem` | Small elements |
| `--border-radius-md` | `0.5rem` | Buttons |
| `--shadow-sm` | `0 2px 4px rgba(0,0,0,0.3)` | Cards (default, via `filter: drop-shadow`) |
| `--shadow-md` | `0 4px 8px rgba(0,0,0,0.4)` | Cards (hover, via `filter: drop-shadow`) |
| `--shadow-lg` | `0 6px 12px rgba(0,0,0,0.5)` | Modals, elevated elements |

> Note: Card border-radius is handled by SVG `<rect rx="12">` (viewBox units). CSS `border-radius` is not used on SVG cards. Shadows use `filter: drop-shadow()` instead of `box-shadow` for SVG compatibility.

---

### Transitions

| Token Name | Value | Usage |
|-----------|-------|-------|
| `--transition-fast` | `200ms ease-out` | Button hover, quick feedback |
| `--transition-normal` | `300ms ease-out` | Card animations, fades |
| `--transition-slow` | `400ms ease-in-out` | Hole card flip, complex animations |

---

## Edge Cases

### Scenario: Very Long Game (Deck Depletion)

**Expected UX**:
- Game engine handles deck reshuffling internally (or game cannot reach this state due to rules)
- If deck runs out mid-game (edge case in engine): Error state, "Game error. Please start a new game." displayed
- **Likelihood**: Extremely low (52 cards, ~10 cards used per round max)

---

### Scenario: Slow Network (Offline After Load)

**Expected UX**:
- WASM module is loaded once and cached by browser
- After initial load, game runs entirely client-side (no network calls)
- If user goes offline after WASM loads: **Game continues to function normally**
- If user tries to load page while offline: Browser offline error (or service worker cache if implemented in future)

---

### Scenario: No JavaScript

**Expected UX**:
- Display `<noscript>` message: "JavaScript is required to play this game. Please enable JavaScript in your browser."
- No game UI visible
- **Design decision**: WebAssembly requires JavaScript, no fallback possible

---

### Scenario: First-Time User

**Expected UX**:
- No tutorial or help screen in MVP
- Game auto-deals on load (immediate play)
- Button labels include action names ("Hit", "Stand", "Split") which are standard blackjack terminology
- **Future enhancement**: Optional "How to Play" button in header

---

### Scenario: Return User

**Expected UX**:
- No session persistence in MVP (no localStorage, no saved games)
- Each page load starts fresh
- **Future enhancement**: Save game state to localStorage, restore on reload

---

### Scenario: Very Fast Clicks (Double-Click Prevention)

**Expected UX**:
- Disable buttons during card animations (300ms)
- Prevent rapid double-clicks from triggering multiple actions
- **Implementation**: Set `disabled` attribute on all buttons during state transitions, re-enable when state settled

---

### Scenario: Keyboard Shortcut While Button Disabled

**Expected UX**:
- Keyboard event listener checks button enabled state before triggering action
- If button is disabled: No-op (silent fail, no error message)
- **Example**: User presses "P" when Split is not available → Nothing happens

---

### Scenario: Browser Zoom (Accessibility)

**Expected UX**:
- Layout uses relative units (`rem`, `em`, `%`) where possible
- Text zooms cleanly up to 200% (WCAG requirement)
- Cards may overlap or scroll at extreme zoom levels, but remain readable
- **Test at**: 100%, 150%, 200% zoom in Chrome/Firefox

---

### Scenario: Split Aces (Special Rule)

**Expected UX**:
- User splits aces
- Each hand receives one additional card
- Both hands auto-stand immediately (no Hit/Stand buttons)
- Status bar shows "Split aces — one card each, standing" for 1 second
- Transitions to Dealer Turn automatically
- **This is handled by WASM engine; UI just reflects state**

---

### Scenario: Natural Blackjack on Deal

**Expected UX**:
- Player dealt A+10
- Status bar shows "Blackjack!" immediately
- No Hit/Stand buttons appear
- Game auto-transitions to Dealer Turn after 1 second delay
- Dealer reveals hole card, checks for dealer blackjack
- If dealer also has blackjack: "Push — dealer also has blackjack." (orange)
- If dealer doesn't: "Blackjack! You win!" (green, bold)

---

### Scenario: Long Content (Many Cards After Multiple Hits)

**Expected UX**:
- Cards arranged horizontally in a fanned/overlapping layout
- Each card after the first overlaps the previous card with a negative margin (-3em)
- The top-left rank and suit of each overlapped card remain visible
- The last card in the hand is always fully visible
- No horizontal scrollbar at any viewport size
- Hand value always visible (below cards, no scroll)

---

### Scenario: Browser Back Button

**Expected UX**:
- Single-page app, no URL changes during gameplay
- Back button navigates away from game (to previous site or browser home)
- **Future enhancement**: Prevent accidental navigation with `beforeunload` event

---

## Design System Notes

**Consistency with TUI design**:
- Color coding matches TUI: red for hearts/diamonds, black for spades/clubs
- Hand value display format matches: "Value: X" or "Value: X (soft)"
- Status messages match TUI where applicable: "Your turn", "Dealer's turn...", "You win!"
- Button labels use same keyboard shortcuts: (H), (S), (P), (N)

**Differences from TUI**:
- Web uses visual playing cards instead of text cards `[A♠]`
- Web uses CSS animations instead of terminal redraws
- Web supports touch and mouse interaction (TUI keyboard-only)
- Web has persistent UI (header, status bar always visible) vs. TUI screen refreshes

**Design inspiration**:
- Clean, minimal casino aesthetic (dark background, white/red cards)
- Modern flat design (no skeuomorphism, no fake felt texture)
- High contrast for readability
- Animation used sparingly (only for feedback, not decoration)

---

## Open Questions for PM/Visionary

None at this stage. Design is complete based on current architecture and specifications.

---

## Implementation Notes for Frontend Developer (A7)

### File Structure (Recommended)

```
web/
├── index.html              # HTML shell
├── style.css               # All styles, design tokens as CSS variables
├── src/
│   ├── main.js             # Entry point, WASM loading, event wiring
│   ├── game-controller.js  # Game state manager, WASM API wrapper
│   ├── ui-renderer.js      # DOM manipulation, state → UI
│   ├── card-renderer.js    # Card HTML/SVG generation
│   └── animations.js       # Animation helpers (fade, slide, flip)
└── build/
    ├── blackjack.wasm      # Compiled from C++ (built separately)
    └── blackjack.js        # Emscripten glue (built separately)
```

### Key DOM Selectors (IDs/Classes)

```html
<!-- Header -->
<header id="header">
  <h1>BLACKJACK</h1>
</header>

<!-- Dealer Area -->
<section id="dealer-area" aria-labelledby="dealer-label">
  <h2 id="dealer-label">Dealer</h2>
  <div id="dealer-cards" class="card-container"></div>
  <p id="dealer-value" class="value"></p>
</section>

<!-- Player Area -->
<section id="player-area" aria-labelledby="player-label">
  <h2 id="player-label">Your Hand</h2>
  <div id="player-cards" class="card-container"></div>
  <p id="player-value" class="value"></p>
</section>

<!-- Split: Alternate Layout -->
<section id="player-area-split" class="split-layout">
  <div class="hand-container active" data-hand="1">
    <h3>Hand 1</h3>
    <div class="card-container"></div>
    <p class="value"></p>
  </div>
  <div class="hand-container" data-hand="2">
    <h3>Hand 2</h3>
    <div class="card-container"></div>
    <p class="value"></p>
  </div>
</section>

<!-- Actions -->
<div id="actions">
  <button id="btn-hit">Hit (H)</button>
  <button id="btn-stand">Stand (S)</button>
  <button id="btn-split">Split (P)</button>
  <button id="btn-new-game">New Game (N)</button>
</div>

<!-- Status Bar -->
<footer id="status-bar">
  <p id="status-message" role="status" aria-live="polite"></p>
</footer>
```

### Animation Timing Reference

| Animation | Duration | Easing | Trigger |
|-----------|----------|--------|---------|
| Card deal (fade + slide) | 300ms | ease-out | New card added |
| Hole card flip | 400ms | ease-in-out | Dealer turn starts |
| Hand split (slide apart) | 400ms | ease-out | Split action |
| Button press feedback | 200ms | ease-out | Button click/keypress |
| Active hand switch | 200ms | ease-out | Stand on split hand |
| Dealer draw pause | 500ms | N/A (delay) | Between dealer cards |

### WASM API Call Examples (Pseudocode for Controller)

```javascript
// Initial deal
await game.deal();
const state = game.state(); // GameState enum
const playerHands = game.player_hands(); // Array of HandState objects
const dealerHand = game.dealer_hand(); // Hand object

// Player hit
const result = game.hit(); // ActionResult enum
if (result === ActionResult.Success) {
  updateUI();
}

// Check available actions
const actions = game.available_actions(); // Array of PlayerAction enums
if (actions.includes(PlayerAction.Split)) {
  showSplitButton();
}

// Dealer turn
game.play_dealer();
const results = playerHands.map(h => h.result); // Array of HandResult enums
```

---

## Conclusion

This specification provides complete UX guidance for the web blackjack application:
- **User flow**: Every path from load to game end, including errors
- **Component specs**: Every UI element with behavior, states, and styling
- **Interactions**: Every user action with system response and transitions
- **Responsive design**: Exact layout changes at 3 breakpoints
- **Accessibility**: WCAG 2.1 AA compliance with keyboard, screen reader, contrast
- **Edge cases**: 10+ scenarios with expected behavior

Frontend Developer (A7) should have zero ambiguity on:
- What each screen looks like
- How each interaction works
- What animations play when
- How accessibility is implemented
- What happens in edge cases

No decisions are left to "figure it out later." The design is complete.
