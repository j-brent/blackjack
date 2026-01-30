#include "blackjack/game.hpp"

namespace blackjack {

Game::Game()
    : rng_(std::random_device{}()) {}

Game::Game(uint32_t seed)
    : rng_(seed) {}

// --- Commands ---

ActionResult Game::deal() {
    if (state_ != GameState::WaitingForDeal && state_ != GameState::RoundOver) {
        return ActionResult::InvalidAction;
    }

    deck_.emplace(rng_);
    round_.emplace();

    auto& r = *round_;
    r.player_hands.push_back(HandState{});
    auto& player = r.player_hands[0].hand;

    // Deal: player, dealer, player, dealer
    player.add_card(deck_->draw());
    r.dealer_hand.add_card(deck_->draw());
    player.add_card(deck_->draw());
    r.dealer_hand.add_card(deck_->draw());

    // Check for naturals — either side ends the round immediately
    if (player.is_natural_blackjack() || r.dealer_hand.is_natural_blackjack()) {
        state_ = GameState::DealerTurn;
    } else {
        state_ = GameState::PlayerTurn;
    }

    return ActionResult::Success;
}

ActionResult Game::hit() {
    if (state_ != GameState::PlayerTurn) {
        return ActionResult::InvalidAction;
    }

    auto& r = *round_;
    auto& hs = r.player_hands[r.active_hand_index];

    // Split aces cannot be hit
    if (hs.is_split_aces) {
        return ActionResult::InvalidAction;
    }

    hs.hand.add_card(deck_->draw()); // NOLINT(bugprone-unchecked-optional-access) — deck_ is always populated during active play

    if (hs.hand.is_bust()) {
        hs.is_stood = true;
        advance_hand();
    }

    return ActionResult::Success;
}

ActionResult Game::stand() {
    if (state_ != GameState::PlayerTurn) {
        return ActionResult::InvalidAction;
    }

    auto& r = *round_;
    r.player_hands[r.active_hand_index].is_stood = true;
    advance_hand();

    return ActionResult::Success;
}

ActionResult Game::split() {
    if (state_ != GameState::PlayerTurn) {
        return ActionResult::InvalidAction;
    }

    auto& r = *round_;

    if (r.has_split) {
        return ActionResult::AlreadySplit;
    }

    auto& active = r.player_hands[r.active_hand_index];
    if (!active.hand.can_split()) {
        return ActionResult::InvalidSplit;
    }

    r.has_split = true;
    auto cards = active.hand.cards();
    Card card1 = cards[0];
    Card card2 = cards[1];
    bool splitting_aces = (card1.rank == Rank::Ace);

    // Replace active hand with first card
    active = HandState{};
    active.hand.add_card(card1);
    active.hand.add_card(deck_->draw()); // NOLINT(bugprone-unchecked-optional-access) — deck_ is always populated during active play
    active.is_from_split = true;
    active.is_split_aces = splitting_aces;

    // Create second hand — push_back may reallocate, invalidating `active`
    HandState second{};
    second.hand.add_card(card2);
    second.hand.add_card(deck_->draw()); // NOLINT(bugprone-unchecked-optional-access) — deck_ is always populated during active play
    second.is_from_split = true;
    second.is_split_aces = splitting_aces;
    r.player_hands.push_back(std::move(second));

    if (splitting_aces) {
        // Re-acquire references after push_back (may have reallocated)
        r.player_hands[0].is_stood = true;
        r.player_hands[1].is_stood = true;
        state_ = GameState::DealerTurn;
    }
    // Otherwise stay in PlayerTurn on first hand

    return ActionResult::Success;
}

ActionResult Game::play_dealer() {
    if (state_ != GameState::DealerTurn) {
        return ActionResult::InvalidAction;
    }

    auto& r = *round_;

    // Check if all player hands busted — dealer doesn't need to draw
    bool all_bust = true;
    for (const auto& hs : r.player_hands) {
        if (!hs.hand.is_bust()) {
            all_bust = false;
            break;
        }
    }

    if (!all_bust) {
        // Dealer draws: hit on soft 17, stand on hard 17+
        while (r.dealer_hand.value() < dealer_stand_threshold ||
               (r.dealer_hand.value() == dealer_stand_threshold && r.dealer_hand.is_soft())) {
            r.dealer_hand.add_card(deck_->draw()); // NOLINT(bugprone-unchecked-optional-access) — deck_ is always populated during active play
        }
    }

    determine_results();
    state_ = GameState::RoundOver;

    return ActionResult::Success;
}

// --- Queries ---

GameState Game::state() const {
    return state_;
}

std::span<const HandState> Game::player_hands() const {
    if (!round_) return {};
    return round_->player_hands;
}

std::size_t Game::active_hand_index() const {
    if (!round_) return 0;
    return round_->active_hand_index;
}

const Hand& Game::dealer_hand() const {
    if (!round_) return empty_hand_;
    return round_->dealer_hand;
}

bool Game::is_dealer_hole_card_visible() const {
    return state_ == GameState::DealerTurn || state_ == GameState::RoundOver;
}

std::optional<Card> Game::dealer_up_card() const {
    if (!round_ || round_->dealer_hand.size() == 0) return std::nullopt;
    return round_->dealer_hand.cards()[0];
}

std::vector<PlayerAction> Game::available_actions() const {
    if (state_ != GameState::PlayerTurn) return {};
    if (!round_) return {};

    std::vector<PlayerAction> actions;
    actions.push_back(PlayerAction::Hit);
    actions.push_back(PlayerAction::Stand);

    const auto& active = round_->player_hands[round_->active_hand_index];
    if (active.hand.can_split() && !round_->has_split) {
        actions.push_back(PlayerAction::Split);
    }

    return actions;
}

// --- Private ---

void Game::determine_results() {
    auto& r = *round_;
    bool dealer_natural = r.dealer_hand.is_natural_blackjack();
    bool dealer_bust = r.dealer_hand.is_bust();
    int dealer_val = r.dealer_hand.value();

    for (auto& hs : r.player_hands) {
        // NOLINTNEXTLINE(bugprone-branch-clone) — bust and dealer-natural both yield Lose, but are logically distinct conditions
        if (hs.hand.is_bust()) {
            hs.result = HandResult::Lose;
        } else if (!hs.is_from_split && hs.hand.is_natural_blackjack()) {
            // Natural blackjack only counts from original deal, not split
            if (dealer_natural) {
                hs.result = HandResult::Push;
            } else {
                hs.result = HandResult::Blackjack;
            }
        } else if (dealer_natural) {
            hs.result = HandResult::Lose;
        } else if (dealer_bust) {
            hs.result = HandResult::Win;
        } else {
            int player_val = hs.hand.value();
            if (player_val > dealer_val) {
                hs.result = HandResult::Win;
            } else if (player_val < dealer_val) {
                hs.result = HandResult::Lose;
            } else {
                hs.result = HandResult::Push;
            }
        }
    }
}

void Game::advance_hand() {
    auto& r = *round_;
    // Try to move to the next unplayed hand
    if (r.active_hand_index + 1 < r.player_hands.size()) {
        ++r.active_hand_index;
        // If next hand is already stood (split aces), keep advancing
        if (r.player_hands[r.active_hand_index].is_stood) {
            advance_hand();
        }
    } else {
        // All hands done
        state_ = GameState::DealerTurn;
    }
}

} // namespace blackjack
