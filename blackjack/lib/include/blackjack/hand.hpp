#pragma once
#include "blackjack/types.hpp"

#include <span>
#include <vector>

namespace blackjack {

class Hand {
public:
    Hand() = default;

    void add_card(Card card);

    [[nodiscard]] std::span<const Card> cards() const noexcept;
    [[nodiscard]] std::size_t size() const noexcept;

    [[nodiscard]] int value() const noexcept;
    [[nodiscard]] bool is_soft() const noexcept;
    [[nodiscard]] bool is_bust() const noexcept;
    [[nodiscard]] bool is_natural_blackjack() const noexcept;
    [[nodiscard]] bool can_split() const noexcept;

private:
    std::vector<Card> cards_;
};

} // namespace blackjack
