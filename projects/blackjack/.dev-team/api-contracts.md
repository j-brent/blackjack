<!-- metadata
id: API-001
type: api-contract
status: draft
author: A2-architect
created: 2026-01-30
updated: 2026-01-30
approved-by: pending
dependencies: [ARCH-001, DATA-001]
-->

# API Contracts: Blackjack Game Engine (`libblackjack`)

This document defines the public C++ API that all frontends code against. The API is designed around two principles:

1. **Command/Query separation**: Commands change state and return a result code. Queries read state and never change it.
2. **Fail-safe**: Invalid commands return an error enum. They never crash, assert, throw, or corrupt state.

## 1. Header: `blackjack/types.hpp`

All enums and small structs used across the API.

```cpp
#pragma once
#include <cstdint>

namespace blackjack {

enum class Suit : uint8_t {
    Clubs,
    Diamonds,
    Hearts,
    Spades
};

enum class Rank : uint8_t {
    Ace = 1,
    Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten,
    Jack, Queen, King
};

enum class GameState : uint8_t {
    WaitingForDeal,
    PlayerTurn,
    DealerTurn,
    RoundOver
};

enum class HandResult : uint8_t {
    None,
    Win,
    Lose,
    Push,
    Blackjack
};

enum class ActionResult : uint8_t {
    Success,
    InvalidAction,
    InvalidSplit,
    AlreadySplit
};

enum class PlayerAction : uint8_t {
    Hit,
    Stand,
    Split
};

struct Card {
    Rank rank;
    Suit suit;

    bool operator==(const Card& other) const = default;
};

// Returns the blackjack point value of a rank.
// Ace=1, 2-10=face, J/Q/K=10.
// Soft ace logic is NOT applied here — that's Hand's responsibility.
int card_value(Rank rank);

} // namespace blackjack
```

## 2. Header: `blackjack/hand.hpp`

```cpp
#pragma once
#include "blackjack/types.hpp"
#include <vector>
#include <span>

namespace blackjack {

class Hand {
public:
    Hand() = default;

    void add_card(Card card);

    std::span<const Card> cards() const;
    std::size_t size() const;

    // Best hand value with soft ace logic applied.
    int value() const;

    // True if an ace is counted as 11 in the current value.
    bool is_soft() const;

    // True if value() > 21.
    bool is_bust() const;

    // True if exactly 2 cards and value == 21.
    bool is_natural_blackjack() const;

    // True if exactly 2 cards with the same rank.
    bool can_split() const;

private:
    std::vector<Card> cards_;
};

} // namespace blackjack
```

## 3. Header: `blackjack/deck.hpp`

```cpp
#pragma once
#include "blackjack/types.hpp"
#include <vector>
#include <random>

namespace blackjack {

class Deck {
public:
    explicit Deck(std::mt19937& rng);

    Card draw();
    std::size_t remaining() const;
    bool is_empty() const;

private:
    std::vector<Card> cards_;
};

} // namespace blackjack
```

## 4. Header: `blackjack/game.hpp`

This is the primary API frontends interact with.

```cpp
#pragma once
#include "blackjack/types.hpp"
#include "blackjack/hand.hpp"
#include <vector>
#include <span>
#include <optional>
#include <cstdint>
#include <random>

namespace blackjack {

struct HandState {
    Hand hand;
    HandResult result = HandResult::None;
    bool is_stood = false;
    bool is_from_split = false;
    bool is_split_aces = false;
};

class Game {
public:
    // Construct with a random seed.
    Game();

    // Construct with a deterministic seed (for testing).
    explicit Game(uint32_t seed);

    // --- Commands ---
    // Each returns ActionResult::Success on success,
    // or an error variant if the action is invalid.

    // Start a new round. Valid in: WaitingForDeal, RoundOver.
    ActionResult deal();

    // Draw a card for the active player hand. Valid in: PlayerTurn.
    ActionResult hit();

    // End the active player hand's turn. Valid in: PlayerTurn.
    ActionResult stand();

    // Split the active player hand. Valid in: PlayerTurn,
    // only if hand can_split() and no prior split this round.
    ActionResult split();

    // Execute the dealer's turn (draw until rules say stop).
    // Valid in: DealerTurn. Transitions to RoundOver.
    // IMPORTANT: The engine does NOT auto-play the dealer. The frontend
    // must explicitly call this method when state() == DealerTurn.
    ActionResult play_dealer();

    // --- Queries (const, never modify state) ---

    // Current game state.
    GameState state() const;

    // All player hands in the current round.
    // Returns empty span if no round in progress.
    std::span<const HandState> player_hands() const;

    // Index of the currently active player hand (0 or 1).
    // Meaningful only during PlayerTurn.
    std::size_t active_hand_index() const;

    // The dealer's hand. Returns empty hand if no round in progress.
    const Hand& dealer_hand() const;

    // Whether the dealer's hole card should be visible to the player.
    // False during PlayerTurn, true during DealerTurn and RoundOver.
    bool is_dealer_hole_card_visible() const;

    // The dealer's face-up card (first card dealt to dealer).
    // Only valid when a round is in progress.
    std::optional<Card> dealer_up_card() const;

    // List of actions the player can currently take.
    // Empty if not in PlayerTurn state.
    std::vector<PlayerAction> available_actions() const;

private:
    // Implementation details hidden from consumers.
    // See data-model.md for internal structure.
};

} // namespace blackjack
```

## 5. API Usage Patterns

### 5.1 Basic Game Loop (CLI-style)

```cpp
blackjack::Game game;

// Start a round
game.deal();

// Player turn
while (game.state() == blackjack::GameState::PlayerTurn) {
    // Query state for rendering
    auto hands = game.player_hands();
    auto active = game.active_hand_index();
    auto dealer_up = game.dealer_up_card();
    auto actions = game.available_actions();

    // ... render hands, prompt user ...

    // Execute player's chosen action
    blackjack::ActionResult result;
    if (player_chose_hit) {
        result = game.hit();
    } else if (player_chose_stand) {
        result = game.stand();
    } else if (player_chose_split) {
        result = game.split();
    }

    // result == ActionResult::Success on valid action
    // result == ActionResult::InvalidAction etc. on invalid action
}

// Dealer turn
if (game.state() == blackjack::GameState::DealerTurn) {
    game.play_dealer();
}

// Round over — read results
auto hands = game.player_hands();
for (const auto& hs : hands) {
    // hs.result is Win, Lose, Push, or Blackjack
}
```

### 5.2 Event-Loop Frontend (FTXUI-style)

In an event-loop frontend, the Game object is held in the application state. UI events trigger commands, and the render function queries state:

```cpp
blackjack::Game game;
game.deal();

// In event handler:
void on_hit_pressed() {
    if (game.state() == blackjack::GameState::PlayerTurn) {
        game.hit();
        if (game.state() == blackjack::GameState::DealerTurn) {
            game.play_dealer();  // or defer to animate
        }
    }
}

// In render function (called by FTXUI on each frame):
ftxui::Element render() {
    auto state = game.state();
    auto hands = game.player_hands();
    auto dealer = game.dealer_hand();
    bool show_hole = game.is_dealer_hole_card_visible();
    // ... build FTXUI elements from this data ...
}
```

### 5.3 Deterministic Testing

```cpp
blackjack::Game game(42);  // Fixed seed — always same shuffle
game.deal();

// Now we know exactly which cards were dealt.
// We can assert on hand contents and values.
auto hands = game.player_hands();
REQUIRE(hands[0].hand.size() == 2);
// ... assert specific cards if seed behavior is documented ...
```

## 6. State Transition Table

| Current State    | Action         | Precondition                        | Next State (success)                   | On Failure           |
|------------------|----------------|--------------------------------------|----------------------------------------|----------------------|
| WaitingForDeal   | `deal()`       | —                                    | PlayerTurn (or DealerTurn if natural)  | —                    |
| RoundOver        | `deal()`       | —                                    | PlayerTurn (or DealerTurn if natural)  | —                    |
| PlayerTurn       | `hit()`        | —                                    | PlayerTurn, or DealerTurn (if bust/done)| —                   |
| PlayerTurn       | `stand()`      | —                                    | PlayerTurn (next hand) or DealerTurn   | —                    |
| PlayerTurn       | `split()`      | `can_split()` && `!has_split`        | PlayerTurn                             | InvalidSplit / AlreadySplit |
| DealerTurn       | `play_dealer()` | —                                   | RoundOver                              | —                    |
| Any other        | Any command    | —                                    | —                                      | InvalidAction        |

### 6.1 Natural Blackjack Special Cases

After `deal()`:
- If the player has a natural blackjack, the state transitions to `DealerTurn` (skipping player input).
- The frontend **must** check `state()` after calling `deal()`. If the state is `DealerTurn`, the frontend must explicitly call `play_dealer()` to execute the dealer's turn. The engine does NOT auto-advance — the frontend drives all state transitions via explicit command calls.

After `play_dealer()`:
- Results are computed for all hands.
- If both player and dealer have naturals, result is Push.
- If only player has natural, result is Blackjack.
- If only dealer has natural, result is Lose.

## 7. Error Handling Contract

- No exceptions are thrown intentionally by any public method. The library contains no `throw`, `try`, or `catch` statements and is compiled with `-fno-exceptions` on GCC/Clang. On MSVC, the no-exceptions policy is enforced by clang-tidy and code review (the MSVC STL does not support `-fno-exceptions` safely).
- **Caveat — implicit `std::bad_alloc`**: Some functions call standard library operations that allocate (`std::vector::push_back`, `std::vector::reserve`, `std::optional::emplace`). These can theoretically throw `std::bad_alloc` on allocation failure. In practice this is unrealistic — the library's peak allocation is a 52-element vector of 2-byte structs. Under `-fno-exceptions` (GCC/Clang, Emscripten), allocation failure calls `std::terminate()` instead of throwing. On MSVC (where `-fno-exceptions` is not set), `std::bad_alloc` could technically propagate to the caller. Functions that perform no allocation are marked `noexcept`; functions that do allocate are deliberately left without `noexcept` to make this distinction visible.
- Commands return `ActionResult`. The caller checks the return value.
- Query methods always return valid data. If called in an unexpected state (e.g., `dealer_hand()` when no round is in progress), they return empty/default data — never crash.
- The library never enters an inconsistent state. Any sequence of calls is safe (though may return `InvalidAction`).

### 7.1 `noexcept` Contract

Functions marked `noexcept` perform no heap allocation and no system calls. They are safe to call from any context, including `noexcept` callers and WASM.

Functions NOT marked `noexcept` may allocate memory internally. Under `-fno-exceptions` (GCC/Clang), allocation failure calls `std::terminate()`. Under normal compilation (MSVC, or consumers building the library with exceptions enabled), allocation failure would throw `std::bad_alloc`.

| Category | Marked `noexcept` | Examples |
|----------|-------------------|----------|
| Pure arithmetic / accessors | Yes | `card_value()`, `value()`, `is_bust()`, `state()`, `remaining()` |
| Constructors that allocate | No | `Deck(rng)`, `Game()` |
| Commands that allocate | No | `deal()`, `hit()`, `split()`, `play_dealer()` |
| Queries returning containers | No | `available_actions()` |

`Game(uint32_t seed)` is the one constructor marked `noexcept` — it constructs only an `mt19937` from a seed, which is deterministic arithmetic with no allocation. The default constructor `Game()` is NOT `noexcept` because it uses `std::random_device{}()`, which may fail if the system entropy source is unavailable.

### 7.2 Query Method Behavior in Non-Applicable States

Query methods are always safe to call. The following table documents return values when called outside their primary intended state:

| Query Method                  | WaitingForDeal          | PlayerTurn            | DealerTurn             | RoundOver               |
|-------------------------------|-------------------------|-----------------------|------------------------|-------------------------|
| `state()`                     | WaitingForDeal          | PlayerTurn            | DealerTurn             | RoundOver               |
| `player_hands()`             | empty span              | current hands         | current hands          | final hands w/ results  |
| `active_hand_index()`        | 0 (meaningless)         | valid index (0 or 1)  | 0 (meaningless)        | 0 (meaningless)         |
| `dealer_hand()`              | empty hand              | partial (2 cards)     | full hand              | full hand               |
| `is_dealer_hole_card_visible()` | false                | false                 | true                   | true                    |
| `dealer_up_card()`           | `std::nullopt`          | first dealer card     | first dealer card      | first dealer card       |
| `available_actions()`        | empty vector            | valid actions list    | empty vector           | empty vector            |

### 7.3 Deterministic Seed Contract

When a `Game` is constructed with a `uint32_t` seed:
- Two `Game` instances with the same seed, given the same sequence of commands, produce identical game states, card deals, and results.
- The specific cards dealt for any given seed value are **implementation-defined**. Tests must not hardcode expected cards for a given seed; instead they should use seeds to ensure reproducibility across test runs.
- The seed controls the `std::mt19937` PRNG used for deck shuffling. No other source of non-determinism exists in the engine.

## 8. Thread Safety

- The `Game` class is NOT thread-safe. The caller must not call methods concurrently from multiple threads.
- This is fine — no frontend in scope requires concurrent access to the game engine.

## 9. Memory and Lifetime

- `Game` is a value type. It can be stack-allocated, heap-allocated, or stored as a member.
- `std::span<const HandState>` returned by `player_hands()` borrows from the `Game` object. The span is invalidated if the `Game` is mutated (any command call). Frontends must not store spans across command calls.
- `const Hand&` returned by `dealer_hand()` has the same lifetime constraint.

## 10. Future-Proofing Notes

- The API uses C++ types (`std::vector`, `std::span`, `std::optional`). For WASM (Phase 2), a thin C wrapper or Embind bindings will translate these to JS-compatible types. This wrapper is a Phase 2 concern, not a Phase 1 concern.
- For Python (Phase 3), pybind11 handles `std::vector` and `std::optional` transparently. `std::span` may need a helper — or the Python bindings can call a method that returns a `std::vector` copy.
- For Qt (Phase 4), the C++ API is used directly. No translation needed.
- The key invariant: the library API surface is small (~10 methods). Writing a wrapper or binding for it is straightforward regardless of target.
