#include "blackjack/deck.hpp"

#include <random>
#include <set>
#include <tuple>

#include <catch2/catch_test_macros.hpp>

using namespace blackjack;

TEST_CASE("Deck initialization", "[deck]") {
    SECTION("New deck has 52 cards") {
        std::mt19937 rng(12345);
        Deck deck(rng);
        int count = 0;
        while (!deck.is_empty()) {
            std::ignore = deck.draw();
            count++;
        }
        REQUIRE(count == 52);
    }

    SECTION("All cards are unique") {
        std::mt19937 rng(54321);
        Deck deck(rng);
        std::set<std::pair<int, int>> seen;

        while (!deck.is_empty()) {
            Card card = deck.draw();
            auto key = std::make_pair(static_cast<int>(card.rank), static_cast<int>(card.suit));
            REQUIRE(seen.find(key) == seen.end());
            seen.insert(key);
        }

        REQUIRE(seen.size() == 52);
    }

    SECTION("Deck contains all 52 unique rank-suit combinations") {
        std::mt19937 rng(99999);
        Deck deck(rng);
        std::set<std::pair<int, int>> cards;

        while (!deck.is_empty()) {
            Card card = deck.draw();
            cards.insert(std::make_pair(static_cast<int>(card.rank), static_cast<int>(card.suit)));
        }

        for (int r = 1; r <= 13; r++) {
            for (int s = 0; s < 4; s++) {
                REQUIRE(cards.count(std::make_pair(r, s)) == 1);
            }
        }
    }
}

TEST_CASE("Deck draw operation", "[deck]") {
    SECTION("draw() reduces remaining count") {
        std::mt19937 rng(11111);
        Deck deck(rng);
        REQUIRE(deck.remaining() == 52);

        std::ignore = deck.draw();
        REQUIRE(deck.remaining() == 51);

        std::ignore = deck.draw();
        REQUIRE(deck.remaining() == 50);
    }

    SECTION("remaining reaches 0 after 52 draws") {
        std::mt19937 rng(22222);
        Deck deck(rng);

        for (int i = 0; i < 52; i++) {
            REQUIRE(deck.remaining() == static_cast<std::size_t>(52 - i));
            std::ignore = deck.draw();
        }

        REQUIRE(deck.remaining() == 0);
        REQUIRE(deck.is_empty());
    }
}

TEST_CASE("Deck deterministic behavior", "[deck]") {
    SECTION("Same seed produces same card order") {
        std::mt19937 rng1(42);
        std::mt19937 rng2(42);
        Deck deck1(rng1);
        Deck deck2(rng2);

        for (int i = 0; i < 52; i++) {
            Card card1 = deck1.draw();
            Card card2 = deck2.draw();
            REQUIRE(card1 == card2);
        }
    }

    SECTION("Different seed produces different order") {
        std::mt19937 rng1(12345);
        std::mt19937 rng2(54321);
        Deck deck1(rng1);
        Deck deck2(rng2);

        bool found_difference = false;
        for (int i = 0; i < 10; i++) {
            Card card1 = deck1.draw();
            Card card2 = deck2.draw();
            if (!(card1 == card2)) {
                found_difference = true;
                break;
            }
        }

        REQUIRE(found_difference);
    }
}
