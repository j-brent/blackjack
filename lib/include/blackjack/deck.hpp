#pragma once
#include "blackjack/types.hpp"

#include <algorithm>
#include <random>
#include <vector>

namespace blackjack {

class Deck {
public:
    explicit Deck(std::mt19937& rng);

    [[nodiscard]] Card draw();
    [[nodiscard]] std::size_t remaining() const;
    [[nodiscard]] bool is_empty() const;

private:
    std::vector<Card> cards_;
};

} // namespace blackjack
