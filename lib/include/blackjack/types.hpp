#pragma once
#include <cstdint>

namespace blackjack {

enum class Suit : uint8_t { Clubs, Diamonds, Hearts, Spades };

enum class Rank : uint8_t {
    Ace = 1,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Ten,
    Jack,
    Queen,
    King
};

enum class GameState : uint8_t { WaitingForDeal, PlayerTurn, DealerTurn, RoundOver };

enum class HandResult : uint8_t { None, Win, Lose, Push, Blackjack };

enum class ActionResult : uint8_t { Success, InvalidAction, InvalidSplit, AlreadySplit };

enum class PlayerAction : uint8_t { Hit, Stand, Split };

struct Card {
    Rank rank;
    Suit suit;

    bool operator==(const Card& other) const = default;
};

// Standard blackjack rules constants
constexpr int face_card_value = 10;
constexpr int ace_promotion_bonus = 10;
constexpr int blackjack_value = 21;
constexpr int dealer_stand_threshold = 17;
constexpr int suits_per_deck = 4;
constexpr int ranks_per_suit = 13;
constexpr int cards_per_deck = suits_per_deck * ranks_per_suit;

[[nodiscard]] int card_value(Rank rank);

} // namespace blackjack
