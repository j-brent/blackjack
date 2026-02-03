<!-- metadata
id: SPEC-001
type: feature-spec
status: draft
author: A2-architect
created: 2026-01-30
updated: 2026-01-30
approved-by: pending
dependencies: [ARCH-001]
-->

# Feature Specification: Blackjack Game Engine (`libblackjack`)

## 1. Overview

The blackjack game engine is a C++ static library implementing standard blackjack rules for a single human player versus a dealer. It manages all game state and rules. It performs no I/O and has no UI awareness. Frontends consume it via a query/command API.

## 2. Game Rules Summary

- Standard 52-card deck, reshuffled each game
- Player is dealt 2 cards face-up; dealer is dealt 2 cards (one face-up, one face-down)
- Player may **hit** (receive another card), **stand** (end turn), or **split** (if holding a pair)
- Aces count as 11 (soft) or 1 (hard); a hand with an ace counted as 11 is a "soft hand"
- **Natural blackjack**: an ace + 10-value card as the initial 2-card hand (value 21)
- **Bust**: hand value exceeds 21
- **Dealer rules**: dealer reveals hole card after player finishes, then hits on soft 17, stands on hard 17+
- **Split rules**: if the player's initial 2 cards have the same rank, the player may split into two separate hands. Each hand receives one additional card and is played independently. Only one level of splitting (no re-splitting). Split aces receive exactly one card each (no further hits allowed).
- **Results per hand**: win, lose, push (tie), or blackjack (natural 21, pays nothing special since no betting — just a distinct result)

## 3. Functional Requirements

### FR-1: Card Representation
- Each card has a suit (Clubs, Diamonds, Hearts, Spades) and a rank (Ace, 2–10, Jack, Queen, King).
- Cards have a numeric value: Ace = 1 or 11, 2–10 = face value, Jack/Queen/King = 10.

### FR-2: Deck Management
- A deck contains 52 cards (one of each suit-rank combination).
- Shuffling uses a seedable pseudo-random number generator.
- Drawing removes and returns the top card. Drawing from an empty deck is a programming error (the library must ensure this never happens during normal play — a single-deck blackjack game uses at most ~20 cards).

### FR-3: Hand Evaluation
- A hand's value is the sum of card values, with aces adjusted to avoid busting.
- If a hand contains one or more aces and counting one ace as 11 keeps the total at 21 or under, one ace counts as 11 (soft hand). Otherwise all aces count as 1.
- A hand is "soft" if an ace is currently counted as 11.
- A hand is "bust" if its value exceeds 21.
- A hand is a "natural blackjack" if it contains exactly 2 cards totaling 21.

### FR-4: Game State Machine
The game progresses through these states:

```
  WAITING_FOR_DEAL
       │
       ▼ deal()
  PLAYER_TURN  ◄──────────┐
       │                   │
       ├─ hit()  ──► card dealt, if not bust → PLAYER_TURN
       │                   if bust → next hand or DEALER_TURN
       ├─ stand() ──► next hand or DEALER_TURN
       ├─ split() ──► hands split, PLAYER_TURN (first split hand)
       │
       ▼ (all player hands resolved)
  DEALER_TURN  (frontend calls play_dealer() to advance)
       │
       ▼
  ROUND_OVER
       │
       ▼ deal() starts new round
  WAITING_FOR_DEAL
```

Special cases:
- If the player has a natural blackjack, skip to DEALER_TURN (dealer checks for natural too).
- If the dealer has a natural blackjack (revealed after player naturals or after player stands), go directly to ROUND_OVER.
- After a split, the player plays each hand sequentially. The game tracks which hand is active.

### FR-5: Player Actions
- **deal()**: Start a new round. Valid only in WAITING_FOR_DEAL. Shuffles and deals 2 cards each to player and dealer.
- **hit()**: Draw one card for the active player hand. Valid only in PLAYER_TURN. If the hand busts, automatically advance to the next hand or DEALER_TURN.
- **stand()**: End the active player hand's turn. Valid only in PLAYER_TURN. Advance to the next hand or DEALER_TURN.
- **split()**: Split the active player hand into two. Valid only in PLAYER_TURN when the hand has exactly 2 cards of the same rank and no prior split has occurred this round. Each new hand receives one card. If split aces, both hands are immediately stood (no further hits).

### FR-6: Dealer Logic
- The dealer does NOT play automatically. When the game enters DEALER_TURN, the frontend must explicitly call `play_dealer()` to execute the dealer's logic. This gives frontends the opportunity to animate the reveal and card draws.
- Dealer reveals hole card.
- Dealer hits while hand value < 17, or hand value == 17 and hand is soft.
- Dealer stands on hard 17 or any 18+.

### FR-7: Result Determination
- For each player hand, compare against dealer:
  - Player busts → **Lose** (regardless of dealer)
  - Dealer busts and player didn't → **Win**
  - Player natural blackjack and dealer doesn't have natural → **Blackjack** (win variant)
  - Both have natural blackjack → **Push**
  - Neither busted: higher value wins, equal value is **Push**
- Results are stored per hand (relevant when splits occur).

### FR-8: Randomness
- The game engine uses `std::mt19937` seeded by `std::random_device` by default.
- A constructor overload accepts a `uint32_t` seed for deterministic testing.

## 4. Non-Functional Requirements

### NFR-1: No I/O
The library must not include `<iostream>`, `<fstream>`, `<cstdio>`, or any I/O headers. It must not read from stdin, write to stdout, or access the filesystem.

### NFR-2: No External Dependencies
The library depends only on the C++ standard library. No Boost, no third-party libs.

### NFR-3: Modern C++ Style
- Use `enum class` for all enumerations
- Use `std::vector`, `std::array`, `std::optional` as appropriate
- RAII for resource management (though there are minimal resources)
- Value semantics — cards, hands are copyable value types
- No raw `new`/`delete`, no raw pointers in the public API
- `const` correctness on all query methods

### NFR-4: Testability
- All game logic must be testable without any UI
- Deterministic mode via seed enables reproducible test scenarios
- The public API must allow inspecting all game state needed to verify correctness

### NFR-5: Compilation
- Must compile cleanly with `-Wall -Wextra -Wpedantic` (GCC/Clang) or `/W4` (MSVC)
- No compiler warnings in the library code

## 5. Acceptance Criteria

### AC-1: Card Values

```
Given a hand with cards [5 of Hearts, 3 of Clubs]
When I evaluate the hand value
Then the value is 8 and the hand is not soft
```

```
Given a hand with cards [Ace of Spades, 7 of Diamonds]
When I evaluate the hand value
Then the value is 18 and the hand is soft
```

```
Given a hand with cards [Ace of Spades, Ace of Hearts, 9 of Clubs]
When I evaluate the hand value
Then the value is 21 and the hand is soft
```

```
Given a hand with cards [Ace of Spades, Ace of Hearts, Ace of Clubs]
When I evaluate the hand value
Then the value is 13 and the hand is soft
```

```
Given a hand with cards [King of Hearts, Queen of Spades]
When I evaluate the hand value
Then the value is 20 and the hand is not soft
```

### AC-2: Natural Blackjack Detection

```
Given a hand with cards [Ace of Hearts, King of Spades]
When I check for natural blackjack
Then the result is true
```

```
Given a hand with cards [Ace of Hearts, 5 of Clubs, 5 of Spades]
When I check for natural blackjack
Then the result is false (3 cards, not a natural)
```

### AC-3: Bust Detection

```
Given a hand with cards [King of Hearts, 7 of Clubs, 8 of Diamonds]
When I evaluate the hand
Then the hand is bust (value 25)
```

```
Given a hand with cards [Ace of Hearts, King of Spades, 5 of Clubs]
When I evaluate the hand
Then the hand is not bust (value 16, ace demoted to 1)
```

### AC-4: Dealing (Normal)

```
Given a game in WAITING_FOR_DEAL state
When I call deal()
And the player does NOT have a natural blackjack
Then the player has 1 hand with 2 cards
And the dealer has 2 cards
And the game state is PLAYER_TURN
And the action result is success
```

### AC-4b: Dealing (Natural Blackjack)

```
Given a game in WAITING_FOR_DEAL state
When I call deal()
And the player has a natural blackjack
Then the player has 1 hand with 2 cards
And the dealer has 2 cards
And the game state is DEALER_TURN (player input is skipped)
And the action result is success
```

### AC-4c: Dealing (Invalid State)

```
Given a game in PLAYER_TURN state
When I call deal()
Then the action result is an error (invalid action for state)
And the game state is unchanged
```

### AC-5: Hit (No Bust)

```
Given a game in PLAYER_TURN
When I call hit()
And the new hand value <= 21
Then the active player hand has one more card than before
And the hand value has increased by the drawn card's value (accounting for soft ace logic)
And the state remains PLAYER_TURN
```

### AC-5b: Hit (Bust)

```
Given a game in PLAYER_TURN
When I call hit()
And the new hand value > 21
Then the active player hand is bust
And the state advances to the next split hand (if any) or DEALER_TURN
```

### AC-6: Stand

```
Given a game in PLAYER_TURN with one player hand
When I call stand()
Then the state transitions to DEALER_TURN
```

```
Given a game in PLAYER_TURN with two hands (split) and the first hand is active
When I call stand()
Then the active hand advances to the second hand
And the state remains PLAYER_TURN
```

### AC-7: Split

```
Given a game in PLAYER_TURN with player hand [8 of Hearts, 8 of Clubs]
When I call split()
Then the player now has 2 hands
And each hand starts with one 8 and one newly dealt card
And the active hand is the first split hand
And the state is PLAYER_TURN
```

```
Given a game in PLAYER_TURN with player hand [Ace of Hearts, Ace of Clubs]
When I call split()
Then the player has 2 hands
And each hand has one Ace plus one dealt card
And both hands are immediately stood (no further hits)
And the state advances to DEALER_TURN
```

```
Given a game in PLAYER_TURN with player hand [8 of Hearts, 5 of Clubs]
When I call split()
Then the action result is an error (cards are not a pair)
```

```
Given a game where a split has already occurred this round
When I call split() on a new pair in a split hand
Then the action result is an error (re-split not allowed)
```

### AC-8: Dealer Logic

```
Given the dealer's hand is [Ace of Hearts, 6 of Clubs] (soft 17)
When the dealer plays
Then the dealer hits (because soft 17)
```

```
Given the dealer's hand is [10 of Spades, 7 of Diamonds] (hard 17)
When the dealer plays
Then the dealer stands
```

```
Given the dealer's hand is [5 of Hearts, 6 of Clubs, 4 of Diamonds] (hard 15)
When the dealer plays
Then the dealer hits
```

### AC-9: Result Determination

```
Given the player hand value is 20 and the dealer hand value is 18
When results are determined
Then the player's result is Win
```

```
Given the player hand value is 18 and the dealer hand value is 18
When results are determined
Then the player's result is Push
```

```
Given the player busted (value 25) and the dealer busted (value 23)
When results are determined
Then the player's result is Lose (player bust always loses)
```

```
Given the player has natural blackjack and the dealer does not
When results are determined
Then the player's result is Blackjack
```

```
Given both player and dealer have natural blackjack
When results are determined
Then the player's result is Push
```

```
Given the player has value 19 and the dealer busted
When results are determined
Then the player's result is Win
```

### AC-10: State Integrity

```
Given a game in ROUND_OVER state
When I call hit()
Then the action result is an error
And the game state remains ROUND_OVER
```

```
Given a game in DEALER_TURN state
When I call split()
Then the action result is an error
```

### AC-11: Full Round Flow

```
Given a new game
When I call deal(), then stand()
Then the game state transitions to DEALER_TURN
When the frontend calls play_dealer()
Then the dealer draws cards per the dealer rules
And results are determined
And the game state is ROUND_OVER
And I can call deal() to start a new round
```

### AC-12: Split Aces 21 Is Win, Not Blackjack

```
Given the player split aces
And one split hand receives a 10-value card (total 21)
When results are determined
Then the result for that hand is Win (not Blackjack)
Because a natural blackjack requires the original 2-card deal, not a split hand
```

### AC-12b: Dealer Natural vs Player Non-Natural

```
Given the player does not have a natural blackjack
And the player has not busted
When the dealer reveals a natural blackjack
Then the player's result is Lose
```

### AC-13: Available Actions

```
Given a game in PLAYER_TURN with no split and a non-pair hand
When I call available_actions()
Then the result contains Hit and Stand (but not Split)
```

```
Given a game in PLAYER_TURN with a splittable pair and no prior split
When I call available_actions()
Then the result contains Hit, Stand, and Split
```

```
Given a game in DEALER_TURN, ROUND_OVER, or WAITING_FOR_DEAL state
When I call available_actions()
Then the result is an empty list
```

### AC-14: Deterministic Seed Guarantee

```
Given two Game instances constructed with the same uint32_t seed
When the same sequence of commands is issued to both
Then both produce identical game states, hands, and results
Note: The specific cards dealt for a given seed are implementation-defined.
      Seeds guarantee reproducibility only — not a specific documented card order.
```
