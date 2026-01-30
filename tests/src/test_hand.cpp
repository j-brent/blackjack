#include <catch2/catch_test_macros.hpp>
#include "blackjack/hand.hpp"

using namespace blackjack;

TEST_CASE("Hand value calculation - AC-1", "[hand]") {
    SECTION("Simple hand without aces: [5H, 3C] = 8, not soft") {
        Hand hand;
        hand.add_card(Card{Rank::Five, Suit::Hearts});
        hand.add_card(Card{Rank::Three, Suit::Clubs});

        REQUIRE(hand.value() == 8);
        REQUIRE_FALSE(hand.is_soft());
    }

    SECTION("Soft hand: [AS, 7D] = 18, soft") {
        Hand hand;
        hand.add_card(Card{Rank::Ace, Suit::Spades});
        hand.add_card(Card{Rank::Seven, Suit::Diamonds});

        REQUIRE(hand.value() == 18);
        REQUIRE(hand.is_soft());
    }

    SECTION("Multiple aces soft: [AS, AH, 9C] = 21, soft") {
        Hand hand;
        hand.add_card(Card{Rank::Ace, Suit::Spades});
        hand.add_card(Card{Rank::Ace, Suit::Hearts});
        hand.add_card(Card{Rank::Nine, Suit::Clubs});

        REQUIRE(hand.value() == 21);
        REQUIRE(hand.is_soft());
    }

    SECTION("Three aces: [AS, AH, AC] = 13, soft") {
        Hand hand;
        hand.add_card(Card{Rank::Ace, Suit::Spades});
        hand.add_card(Card{Rank::Ace, Suit::Hearts});
        hand.add_card(Card{Rank::Ace, Suit::Clubs});

        REQUIRE(hand.value() == 13);
        REQUIRE(hand.is_soft());
    }

    SECTION("Two face cards: [KH, QS] = 20, not soft") {
        Hand hand;
        hand.add_card(Card{Rank::King, Suit::Hearts});
        hand.add_card(Card{Rank::Queen, Suit::Spades});

        REQUIRE(hand.value() == 20);
        REQUIRE_FALSE(hand.is_soft());
    }

    SECTION("Ace demoted from 11 to 1: [AH, KS, 5C] = 16, not soft") {
        Hand hand;
        hand.add_card(Card{Rank::Ace, Suit::Hearts});
        hand.add_card(Card{Rank::King, Suit::Spades});
        hand.add_card(Card{Rank::Five, Suit::Clubs});

        REQUIRE(hand.value() == 16);
        REQUIRE_FALSE(hand.is_soft());
    }
}

TEST_CASE("Hand natural blackjack detection - AC-2", "[hand]") {
    SECTION("Natural blackjack: [AH, KS] = true") {
        Hand hand;
        hand.add_card(Card{Rank::Ace, Suit::Hearts});
        hand.add_card(Card{Rank::King, Suit::Spades});

        REQUIRE(hand.is_natural_blackjack());
        REQUIRE(hand.value() == 21);
    }

    SECTION("Natural blackjack with Jack: [AS, JH] = true") {
        Hand hand;
        hand.add_card(Card{Rank::Ace, Suit::Spades});
        hand.add_card(Card{Rank::Jack, Suit::Hearts});

        REQUIRE(hand.is_natural_blackjack());
    }

    SECTION("Not natural with 3 cards: [AH, 5C, 5S] = false") {
        Hand hand;
        hand.add_card(Card{Rank::Ace, Suit::Hearts});
        hand.add_card(Card{Rank::Five, Suit::Clubs});
        hand.add_card(Card{Rank::Five, Suit::Spades});

        REQUIRE_FALSE(hand.is_natural_blackjack());
        REQUIRE(hand.value() == 21);
    }

    SECTION("Not natural without ace: [KH, QS] = false") {
        Hand hand;
        hand.add_card(Card{Rank::King, Suit::Hearts});
        hand.add_card(Card{Rank::Queen, Suit::Spades});

        REQUIRE_FALSE(hand.is_natural_blackjack());
    }

    SECTION("Not natural with ace but not 21: [AS, 5H] = false") {
        Hand hand;
        hand.add_card(Card{Rank::Ace, Suit::Spades});
        hand.add_card(Card{Rank::Five, Suit::Hearts});

        REQUIRE_FALSE(hand.is_natural_blackjack());
    }
}

TEST_CASE("Hand bust detection - AC-3", "[hand]") {
    SECTION("Bust hand: [KH, 7C, 8D] = bust (25)") {
        Hand hand;
        hand.add_card(Card{Rank::King, Suit::Hearts});
        hand.add_card(Card{Rank::Seven, Suit::Clubs});
        hand.add_card(Card{Rank::Eight, Suit::Diamonds});

        REQUIRE(hand.is_bust());
        REQUIRE(hand.value() == 25);
    }

    SECTION("Not bust with ace demotion: [AH, KS, 5C] = 16, not bust") {
        Hand hand;
        hand.add_card(Card{Rank::Ace, Suit::Hearts});
        hand.add_card(Card{Rank::King, Suit::Spades});
        hand.add_card(Card{Rank::Five, Suit::Clubs});

        REQUIRE_FALSE(hand.is_bust());
        REQUIRE(hand.value() == 16);
    }

    SECTION("Not bust at 21: [KH, QS, AS] = 21, not bust") {
        Hand hand;
        hand.add_card(Card{Rank::King, Suit::Hearts});
        hand.add_card(Card{Rank::Queen, Suit::Spades});
        hand.add_card(Card{Rank::Ace, Suit::Spades});

        REQUIRE_FALSE(hand.is_bust());
        REQUIRE(hand.value() == 21);
    }

    SECTION("Not bust under 21: [9H, 8C] = 17, not bust") {
        Hand hand;
        hand.add_card(Card{Rank::Nine, Suit::Hearts});
        hand.add_card(Card{Rank::Eight, Suit::Clubs});

        REQUIRE_FALSE(hand.is_bust());
        REQUIRE(hand.value() == 17);
    }
}

TEST_CASE("Hand can_split detection", "[hand]") {
    SECTION("Can split matching rank: [8H, 8C] = true") {
        Hand hand;
        hand.add_card(Card{Rank::Eight, Suit::Hearts});
        hand.add_card(Card{Rank::Eight, Suit::Clubs});

        REQUIRE(hand.can_split());
    }

    SECTION("Cannot split different rank: [8H, 5C] = false") {
        Hand hand;
        hand.add_card(Card{Rank::Eight, Suit::Hearts});
        hand.add_card(Card{Rank::Five, Suit::Clubs});

        REQUIRE_FALSE(hand.can_split());
    }

    SECTION("Cannot split different rank even with same value: [KH, QS] = false") {
        Hand hand;
        hand.add_card(Card{Rank::King, Suit::Hearts});
        hand.add_card(Card{Rank::Queen, Suit::Spades});

        REQUIRE_FALSE(hand.can_split());
    }

    SECTION("Can split aces: [AH, AS] = true") {
        Hand hand;
        hand.add_card(Card{Rank::Ace, Suit::Hearts});
        hand.add_card(Card{Rank::Ace, Suit::Spades});

        REQUIRE(hand.can_split());
    }

    SECTION("Cannot split with more than 2 cards: [8H, 8C, 8D] = false") {
        Hand hand;
        hand.add_card(Card{Rank::Eight, Suit::Hearts});
        hand.add_card(Card{Rank::Eight, Suit::Clubs});
        hand.add_card(Card{Rank::Eight, Suit::Diamonds});

        REQUIRE_FALSE(hand.can_split());
    }
}
