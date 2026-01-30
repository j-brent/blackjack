#include "blackjack/types.hpp"

namespace blackjack {

int card_value(Rank rank) {
    auto val = static_cast<int>(rank);
    if (val >= 10) return 10;
    return val;
}

} // namespace blackjack
