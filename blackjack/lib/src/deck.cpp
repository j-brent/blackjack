#include "blackjack/deck.hpp"
#include <algorithm>
#include <cassert>

namespace blackjack {

Deck::Deck(std::mt19937& rng) {
    cards_.reserve(52);

    for (int s = 0; s < 4; ++s) {
        for (int r = 1; r <= 13; ++r) {
            cards_.push_back(Card{
                static_cast<Rank>(r),
                static_cast<Suit>(s)
            });
        }
    }

    std::shuffle(cards_.begin(), cards_.end(), rng);
}

Card Deck::draw() {
    assert(!cards_.empty());
    Card top = cards_.back();
    cards_.pop_back();
    return top;
}

std::size_t Deck::remaining() const {
    return cards_.size();
}

bool Deck::is_empty() const {
    return cards_.empty();
}

} // namespace blackjack
