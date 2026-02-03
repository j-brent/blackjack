#include "blackjack/deck.hpp"
#include "blackjack/game.hpp"
#include "blackjack/hand.hpp"
#include "blackjack/types.hpp"

#include <vector>

#include <emscripten/bind.h>

namespace blackjack {

// Wrapper functions to convert std::span to std::vector for Embind compatibility
std::vector<Card> hand_cards_wrapper(const Hand& hand) {
    auto cards_span = hand.cards();
    return std::vector<Card>(cards_span.begin(), cards_span.end());
}

std::vector<HandState> player_hands_wrapper(const Game& game) {
    auto hands_span = game.player_hands();
    return std::vector<HandState>(hands_span.begin(), hands_span.end());
}

// Wrapper for std::optional<Card> from dealer_up_card()
// Returns a Card with sentinel value (rank=0, suit=0) if no card is present
struct OptionalCard {
    Card card;
    bool has_value;
};

OptionalCard dealer_up_card_wrapper(const Game& game) {
    auto opt_card = game.dealer_up_card();
    if (opt_card.has_value()) {
        return OptionalCard{opt_card.value(), true};
    }
    return OptionalCard{Card{Rank::Ace, Suit::Clubs}, false};
}

} // namespace blackjack

EMSCRIPTEN_BINDINGS(blackjack_module) {
    using namespace emscripten;
    using namespace blackjack;

    // Enum: Suit
    enum_<Suit>("Suit")
        .value("Clubs", Suit::Clubs)
        .value("Diamonds", Suit::Diamonds)
        .value("Hearts", Suit::Hearts)
        .value("Spades", Suit::Spades);

    // Enum: Rank
    enum_<Rank>("Rank")
        .value("Ace", Rank::Ace)
        .value("Two", Rank::Two)
        .value("Three", Rank::Three)
        .value("Four", Rank::Four)
        .value("Five", Rank::Five)
        .value("Six", Rank::Six)
        .value("Seven", Rank::Seven)
        .value("Eight", Rank::Eight)
        .value("Nine", Rank::Nine)
        .value("Ten", Rank::Ten)
        .value("Jack", Rank::Jack)
        .value("Queen", Rank::Queen)
        .value("King", Rank::King);

    // Enum: GameState
    enum_<GameState>("GameState")
        .value("WaitingForDeal", GameState::WaitingForDeal)
        .value("PlayerTurn", GameState::PlayerTurn)
        .value("DealerTurn", GameState::DealerTurn)
        .value("RoundOver", GameState::RoundOver);

    // Enum: HandResult
    enum_<HandResult>("HandResult")
        .value("None", HandResult::None)
        .value("Win", HandResult::Win)
        .value("Lose", HandResult::Lose)
        .value("Push", HandResult::Push)
        .value("Blackjack", HandResult::Blackjack);

    // Enum: ActionResult
    enum_<ActionResult>("ActionResult")
        .value("Success", ActionResult::Success)
        .value("InvalidAction", ActionResult::InvalidAction)
        .value("InvalidSplit", ActionResult::InvalidSplit)
        .value("AlreadySplit", ActionResult::AlreadySplit);

    // Enum: PlayerAction
    enum_<PlayerAction>("PlayerAction")
        .value("Hit", PlayerAction::Hit)
        .value("Stand", PlayerAction::Stand)
        .value("Split", PlayerAction::Split);

    // Struct: Card
    value_object<Card>("Card").field("rank", &Card::rank).field("suit", &Card::suit);

    // Struct: OptionalCard
    value_object<OptionalCard>("OptionalCard")
        .field("card", &OptionalCard::card)
        .field("has_value", &OptionalCard::has_value);

    // Class: Hand
    class_<Hand>("Hand")
        .constructor<>()
        .function("cards", &hand_cards_wrapper)
        .function("value", &Hand::value)
        .function("size", &Hand::size)
        .function("is_soft", &Hand::is_soft)
        .function("is_bust", &Hand::is_bust)
        .function("is_natural_blackjack", &Hand::is_natural_blackjack)
        .function("can_split", &Hand::can_split);

    // Struct: HandState
    value_object<HandState>("HandState")
        .field("hand", &HandState::hand)
        .field("result", &HandState::result)
        .field("is_stood", &HandState::is_stood)
        .field("is_from_split", &HandState::is_from_split)
        .field("is_split_aces", &HandState::is_split_aces);

    // Class: Game
    class_<Game>("Game")
        .constructor<>()
        .constructor<uint32_t>()
        .function("deal", &Game::deal)
        .function("hit", &Game::hit)
        .function("stand", &Game::stand)
        .function("split", &Game::split)
        .function("play_dealer", &Game::play_dealer)
        .function("state", &Game::state)
        .function("player_hands", &player_hands_wrapper)
        .function("active_hand_index", &Game::active_hand_index)
        .function("dealer_hand", &Game::dealer_hand)
        .function("is_dealer_hole_card_visible", &Game::is_dealer_hole_card_visible)
        .function("dealer_up_card", &dealer_up_card_wrapper)
        .function("available_actions", &Game::available_actions);

    // Register vector types for Embind
    register_vector<Card>("VectorCard");
    register_vector<HandState>("VectorHandState");
    register_vector<PlayerAction>("VectorPlayerAction");
}
