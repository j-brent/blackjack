#pragma once
#include "blackjack/types.hpp"
#include <vector>
#include <span>

namespace blackjack {

class Hand {
public:
    Hand() = default;

    void add_card(Card card);

    [[nodiscard]] std::span<const Card> cards() const;
    [[nodiscard]] std::size_t size() const;

    [[nodiscard]] int value() const;
    [[nodiscard]] bool is_soft() const;
    [[nodiscard]] bool is_bust() const;
    [[nodiscard]] bool is_natural_blackjack() const;
    [[nodiscard]] bool can_split() const;

private:
    std::vector<Card> cards_;
};

} // namespace blackjack
