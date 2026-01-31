#pragma once
#include "blackjack/types.hpp"

#include <algorithm>
#include <random>
#include <vector>

namespace blackjack {

class Deck {
public:
    explicit Deck(std::mt19937& rng);

    [[nodiscard]] Card draw() noexcept;
    [[nodiscard]] std::size_t remaining() const noexcept;
    [[nodiscard]] bool is_empty() const noexcept;

private:
    std::vector<Card> cards_;
};

} // namespace blackjack
