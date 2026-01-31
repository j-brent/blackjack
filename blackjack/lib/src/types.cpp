#include "blackjack/types.hpp"

namespace blackjack {

int card_value(Rank rank) noexcept {
    auto val = static_cast<int>(rank);
    if (val >= face_card_value)
        return face_card_value;
    return val;
}

} // namespace blackjack
