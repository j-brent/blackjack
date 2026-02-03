#include "blackjack/types.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace blackjack;

TEST_CASE("card_value returns correct values for all ranks", "[card]") {
    SECTION("Ace has value 1") {
        REQUIRE(card_value(Rank::Ace) == 1);
    }

    SECTION("Two through Ten have face values") {
        REQUIRE(card_value(Rank::Two) == 2);
        REQUIRE(card_value(Rank::Three) == 3);
        REQUIRE(card_value(Rank::Four) == 4);
        REQUIRE(card_value(Rank::Five) == 5);
        REQUIRE(card_value(Rank::Six) == 6);
        REQUIRE(card_value(Rank::Seven) == 7);
        REQUIRE(card_value(Rank::Eight) == 8);
        REQUIRE(card_value(Rank::Nine) == 9);
        REQUIRE(card_value(Rank::Ten) == 10);
    }

    SECTION("Face cards have value 10") {
        REQUIRE(card_value(Rank::Jack) == 10);
        REQUIRE(card_value(Rank::Queen) == 10);
        REQUIRE(card_value(Rank::King) == 10);
    }
}

TEST_CASE("Card equality operator works correctly", "[card]") {
    SECTION("Same rank and suit are equal") {
        Card card1{Rank::Ace, Suit::Hearts};
        Card card2{Rank::Ace, Suit::Hearts};
        REQUIRE(card1 == card2);
    }

    SECTION("Different rank are not equal") {
        Card card1{Rank::Ace, Suit::Hearts};
        Card card2{Rank::King, Suit::Hearts};
        REQUIRE_FALSE(card1 == card2);
    }

    SECTION("Different suit are not equal") {
        Card card1{Rank::Ace, Suit::Hearts};
        Card card2{Rank::Ace, Suit::Spades};
        REQUIRE_FALSE(card1 == card2);
    }

    SECTION("Different rank and suit are not equal") {
        Card card1{Rank::King, Suit::Hearts};
        Card card2{Rank::Queen, Suit::Diamonds};
        REQUIRE_FALSE(card1 == card2);
    }
}
