<!-- metadata
id: DATA-001
type: data-model
status: draft
author: A2-architect
created: 2026-01-30
updated: 2026-01-30
approved-by: pending
dependencies: [ARCH-001, SPEC-001]
-->

# Data Model: Blackjack Game Engine

All types live in the `blackjack` namespace. All enums are `enum class`. All types use value semantics (copyable, no heap allocation in the public API).

## 1. Enums

### 1.1 Suit

```cpp
enum class Suit : uint8_t {
    Clubs,
    Diamonds,
    Hearts,
    Spades
};
```

4 values. Hearts and Diamonds are red suits; Clubs and Spades are black. The library does not encode color — that is a frontend concern.

### 1.2 Rank

```cpp
enum class Rank : uint8_t {
    Ace = 1,
    Two = 2,
    Three = 3,
    Four = 4,
    Five = 5,
    Six = 6,
    Seven = 7,
    Eight = 8,
    Nine = 9,
    Ten = 10,
    Jack = 11,
    Queen = 12,
    King = 13
};
```

Underlying value is NOT the card's blackjack value (Jack/Queen/King are 10 in blackjack but 11/12/13 as enum values). The `card_value()` function maps Rank to blackjack value.

### 1.3 GameState

```cpp
enum class GameState : uint8_t {
    WaitingForDeal,   // No round in progress. Call deal() to start.
    PlayerTurn,       // Player is making decisions (hit/stand/split).
    DealerTurn,       // Dealer is drawing cards. Frontend should call play_dealer().
    RoundOver         // Round is complete. Results available. Call deal() for a new round.
};
```

### 1.4 HandResult

```cpp
enum class HandResult : uint8_t {
    None,       // Result not yet determined (round in progress)
    Win,        // Player hand beats dealer
    Lose,       // Dealer beats player hand, or player busted
    Push,       // Tie
    Blackjack   // Player has natural blackjack, dealer does not
};
```

### 1.5 ActionResult

```cpp
enum class ActionResult : uint8_t {
    Success,            // Action executed successfully
    InvalidAction,      // Action not valid in current game state
    InvalidSplit,       // Split attempted but hand is not a splittable pair
    AlreadySplit        // Split attempted but a split already occurred this round
};
```

### 1.6 PlayerAction

```cpp
enum class PlayerAction : uint8_t {
    Hit,
    Stand,
    Split
};
```

Used by frontends to enumerate what actions are currently valid (via `get_available_actions()`).

## 2. Card

```cpp
struct Card {
    Rank rank;
    Suit suit;
};
```

- POD-like struct. Trivially copyable.
- Equality: two cards are equal if both rank and suit match.
- No methods on Card itself — value calculation is done at the Hand level.

**Helper function** (free function or static):

```cpp
// Returns the blackjack point value of a rank.
// Ace -> 1 (soft/hard logic is at the Hand level)
// 2-10 -> face value
// Jack, Queen, King -> 10
int card_value(Rank rank);
```

Note: `card_value` returns 1 for Ace. The Hand class handles the soft ace logic (whether to count an ace as 11).

## 3. Deck

```cpp
class Deck {
public:
    // Construct a standard 52-card deck, shuffled with the given RNG.
    explicit Deck(std::mt19937& rng);

    // Draw the top card. Precondition: !is_empty().
    Card draw();

    // Number of cards remaining.
    std::size_t remaining() const;

    // Whether the deck is empty.
    bool is_empty() const;

private:
    std::vector<Card> cards_;  // cards_[0] is the bottom, cards_.back() is the top
};
```

- The constructor creates all 52 cards and shuffles them using the provided RNG.
- `draw()` removes and returns the last element (top of deck). This is O(1).
- The Deck does not reshuffle itself. A new Deck is created for each round.

## 4. Hand

```cpp
class Hand {
public:
    Hand() = default;

    // Add a card to the hand.
    void add_card(Card card);

    // Get all cards in the hand (read-only view).
    std::span<const Card> cards() const;

    // Number of cards.
    std::size_t size() const;

    // Calculate the best hand value (soft ace logic applied).
    int value() const;

    // Whether the hand is soft (an ace is counted as 11).
    bool is_soft() const;

    // Whether the hand value exceeds 21.
    bool is_bust() const;

    // Whether this is a natural blackjack (exactly 2 cards, value == 21).
    bool is_natural_blackjack() const;

    // Whether this hand can be split (exactly 2 cards of the same rank).
    bool can_split() const;

private:
    std::vector<Card> cards_;
};
```

### 4.1 Value Calculation Algorithm

```
total = sum of card_value(rank) for each card  (Ace counted as 1)
aces = count of Aces in hand
while (aces > 0 && total + 10 <= 21):
    total += 10   // promote one ace from 1 to 11
    aces -= 1
return total
```

The hand is "soft" if at least one ace was promoted (i.e., the final value includes an ace counted as 11).

### 4.2 Split Eligibility

`can_split()` returns true if and only if:
- The hand has exactly 2 cards
- Both cards have the same `Rank`

Note: some blackjack variants allow splitting any two 10-value cards (e.g., Jack + Queen). This implementation requires same rank — a Jack and Queen cannot be split.

## 5. HandState (per-hand tracking during a round)

```cpp
struct HandState {
    Hand hand;
    HandResult result = HandResult::None;
    bool is_stood = false;         // Player has stood on this hand
    bool is_from_split = false;    // This hand was created by splitting
    bool is_split_aces = false;    // This hand is from splitting aces (no further hits)
};
```

This struct tracks a single player hand during a round, including its result and whether the player has finished acting on it. The `is_split_aces` flag prevents further hits on split aces.

## 6. RoundState (internal to Game, but documented for clarity)

```cpp
struct RoundState {
    std::vector<HandState> player_hands;  // 1 hand normally, 2 after split
    Hand dealer_hand;
    std::size_t active_hand_index = 0;    // Which player hand is currently being played
    bool has_split = false;               // Whether a split occurred this round
};
```

This is not part of the public API — it's internal to the `Game` class. Frontends access it through query methods on `Game`.

## 7. Game (top-level state, public API outlined here, detailed in API Contracts)

```cpp
class Game {
public:
    Game();                          // Random seed
    explicit Game(uint32_t seed);    // Deterministic seed

    // Commands
    ActionResult deal();
    ActionResult hit();
    ActionResult stand();
    ActionResult split();
    ActionResult play_dealer();      // Executes dealer's turn

    // Queries
    GameState state() const;
    std::span<const HandState> player_hands() const;
    std::size_t active_hand_index() const;
    const Hand& dealer_hand() const;
    bool is_dealer_hole_card_visible() const;
    Card dealer_visible_card() const;
    std::vector<PlayerAction> available_actions() const;

private:
    GameState state_;
    std::mt19937 rng_;
    std::optional<Deck> deck_;
    std::optional<RoundState> round_;
};
```

The `Game` class owns the RNG and round state. Query methods provide read-only access. Command methods return `ActionResult` indicating success or failure.

## 8. Type Relationships Diagram

```
Game
 ├── GameState (enum)
 ├── mt19937 (RNG)
 ├── Deck (optional, exists during a round)
 │    └── vector<Card>
 └── RoundState (optional, exists during a round)
      ├── vector<HandState>
      │    ├── Hand
      │    │    └── vector<Card>
      │    │         ├── Rank (enum)
      │    │         └── Suit (enum)
      │    ├── HandResult (enum)
      │    └── flags (is_stood, is_from_split, is_split_aces)
      ├── Hand (dealer)
      └── active_hand_index
```

## 9. Design Notes

1. **No pointers in the public API.** All data is returned by value, reference, or `std::span`. Frontends never manage memory.

2. **`std::span<const Card>` vs `const std::vector<Card>&`**: `std::span` is preferred because it doesn't expose the container type. If the internal representation changes, the API doesn't break.

3. **`std::optional<Deck>` and `std::optional<RoundState>`**: These exist only while a round is in progress. Using `optional` makes it explicit that they can be absent (in `WaitingForDeal` state before the first deal).

4. **`HandState` is a struct, not a class**: It's a data bundle with no invariants to protect. Making it a struct with public fields is appropriate.

5. **Card is a struct, not a class**: It has no behavior, just data. Two fields, trivially copyable.

6. **`card_value()` is a free function**: It maps Rank to int. It doesn't belong to Card because Card doesn't know about blackjack rules — it's just a playing card. The value function is game-rule logic.
