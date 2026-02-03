#pragma once
#include "blackjack/deck.hpp"
#include "blackjack/hand.hpp"
#include "blackjack/types.hpp"

#include <cstdint>
#include <optional>
#include <random>
#include <span>
#include <vector>

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
    explicit Game(uint32_t seed) noexcept;

    // --- Commands ---
    [[nodiscard]] ActionResult deal();
    [[nodiscard]] ActionResult hit();
    [[nodiscard]] ActionResult stand();
    [[nodiscard]] ActionResult split();
    [[nodiscard]] ActionResult play_dealer();

    // --- Queries ---
    [[nodiscard]] GameState state() const noexcept;
    [[nodiscard]] std::span<const HandState> player_hands() const noexcept;
    [[nodiscard]] std::size_t active_hand_index() const noexcept;
    [[nodiscard]] const Hand& dealer_hand() const noexcept;
    [[nodiscard]] bool is_dealer_hole_card_visible() const noexcept;
    [[nodiscard]] std::optional<Card> dealer_up_card() const noexcept;
    [[nodiscard]] std::vector<PlayerAction> available_actions() const;

private:
    void determine_results() noexcept;
    void advance_hand() noexcept;

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
