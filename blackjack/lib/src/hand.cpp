#include "blackjack/hand.hpp"

namespace blackjack {

void Hand::add_card(Card card) {
    cards_.push_back(card);
}

std::span<const Card> Hand::cards() const {
    return cards_;
}

std::size_t Hand::size() const {
    return cards_.size();
}

int Hand::value() const {
    int total = 0;
    int aces = 0;

    for (const auto& card : cards_) {
        total += card_value(card.rank);
        if (card.rank == Rank::Ace) {
            ++aces;
        }
    }

    while (aces > 0 && total + 10 <= 21) {
        total += 10;
        --aces;
    }

    return total;
}

bool Hand::is_soft() const {
    int total = 0;
    int aces = 0;

    for (const auto& card : cards_) {
        total += card_value(card.rank);
        if (card.rank == Rank::Ace) {
            ++aces;
        }
    }

    // A hand is soft if at least one ace can be counted as 11 without busting.
    // We mirror value()'s promotion logic: promote aces greedily, then check
    // if any promotion was applied.
    bool promoted = false;
    while (aces > 0 && total + 10 <= 21) {
        total += 10;
        --aces;
        promoted = true;
    }

    return promoted;
}

bool Hand::is_bust() const {
    return value() > 21;
}

bool Hand::is_natural_blackjack() const {
    return cards_.size() == 2 && value() == 21;
}

bool Hand::can_split() const {
    return cards_.size() == 2 && cards_[0].rank == cards_[1].rank;
}

} // namespace blackjack
