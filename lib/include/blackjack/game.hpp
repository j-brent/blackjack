#pragma once
#include "blackjack/types.hpp"
#include "blackjack/hand.hpp"
#include "blackjack/deck.hpp"
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
    Game();
    explicit Game(uint32_t seed);

    // --- Commands ---
    [[nodiscard]] ActionResult deal();
    [[nodiscard]] ActionResult hit();
    [[nodiscard]] ActionResult stand();
    [[nodiscard]] ActionResult split();
    [[nodiscard]] ActionResult play_dealer();

    // --- Queries ---
    [[nodiscard]] GameState state() const;
    [[nodiscard]] std::span<const HandState> player_hands() const;
    [[nodiscard]] std::size_t active_hand_index() const;
    [[nodiscard]] const Hand& dealer_hand() const;
    [[nodiscard]] bool is_dealer_hole_card_visible() const;
    [[nodiscard]] std::optional<Card> dealer_up_card() const;
    [[nodiscard]] std::vector<PlayerAction> available_actions() const;

private:
    void determine_results();
    void advance_hand();

    GameState state_ = GameState::WaitingForDeal;
    std::mt19937 rng_;

    struct RoundState {
        std::vector<HandState> player_hands;
        Hand dealer_hand;
        std::size_t active_hand_index = 0;
        bool has_split = false;
    };

    std::optional<Deck> deck_;
    std::optional<RoundState> round_;
    Hand empty_hand_;
};

} // namespace blackjack
