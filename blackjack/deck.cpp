#include "blackjack/deck.hpp"

#include <algorithm>
#include <cassert>

namespace blackjack {

Deck::Deck(std::mt19937& rng) {
    cards_.reserve(cards_per_deck);

    for (int s = 0; s < suits_per_deck; ++s) {
        for (int r = 1; r <= ranks_per_suit; ++r) {
            cards_.push_back(Card{static_cast<Rank>(r), static_cast<Suit>(s)});
        }
    }

    std::shuffle(cards_.begin(), cards_.end(), rng);
}

Card Deck::draw() noexcept {
    assert(!cards_.empty());
    Card top = cards_.back();
    cards_.pop_back();
    return top;
}

std::size_t Deck::remaining() const noexcept {
    return cards_.size();
}

bool Deck::is_empty() const noexcept {
    return cards_.empty();
}

} // namespace blackjack
