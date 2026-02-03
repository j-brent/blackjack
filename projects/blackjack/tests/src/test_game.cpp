#include "blackjack/game.hpp"

#include <tuple>

#include <catch2/catch_test_macros.hpp>

using namespace blackjack;

// Helper: find a seed where player does NOT get natural blackjack
static Game make_normal_game() {
    for (uint32_t seed = 1; seed < 100000; seed++) {
        Game game(seed);
        std::ignore = game.deal();
        if (game.state() == GameState::PlayerTurn) {
            return Game(seed);
        }
    }
    return Game(1);
}

TEST_CASE("Game deal operation - AC-4", "[game]") {
    SECTION("deal() in WaitingForDeal succeeds") {
        Game game(12345);
        REQUIRE(game.state() == GameState::WaitingForDeal);

        auto result = game.deal();
        REQUIRE(result == ActionResult::Success);

        auto hands = game.player_hands();
        REQUIRE(hands.size() == 1);
        REQUIRE(hands[0].hand.size() == 2);
        REQUIRE(game.dealer_hand().size() == 2);
        REQUIRE(game.state() != GameState::WaitingForDeal);
    }

    SECTION("deal() in PlayerTurn returns InvalidAction - AC-4c") {
        auto game = make_normal_game();
        std::ignore = game.deal();
        REQUIRE(game.state() == GameState::PlayerTurn);

        auto result = game.deal();
        REQUIRE(result == ActionResult::InvalidAction);
        REQUIRE(game.state() == GameState::PlayerTurn);
    }
}

TEST_CASE("Natural blackjack on deal - AC-4b", "[game]") {
    SECTION("Natural blackjack transitions to DealerTurn") {
        bool found = false;
        for (uint32_t seed = 1; seed < 100000; seed++) {
            Game game(seed);
            std::ignore = game.deal();

            auto hands = game.player_hands();
            if (!hands.empty() && hands[0].hand.is_natural_blackjack()) {
                REQUIRE(game.state() == GameState::DealerTurn);
                found = true;
                break;
            }
        }
        REQUIRE(found);
    }
}

TEST_CASE("Hit operation - AC-5", "[game]") {
    SECTION("hit() adds card and stays in PlayerTurn") {
        bool found = false;
        for (uint32_t seed = 1; seed < 10000; seed++) {
            Game game(seed);
            std::ignore = game.deal();
            if (game.state() != GameState::PlayerTurn)
                continue;

            auto before = game.player_hands()[0].hand.size();
            auto result = game.hit();
            REQUIRE(result == ActionResult::Success);

            auto hands = game.player_hands();
            REQUIRE(hands[0].hand.size() == before + 1);

            if (!hands[0].hand.is_bust()) {
                REQUIRE(game.state() == GameState::PlayerTurn);
                found = true;
                break;
            }
        }
        REQUIRE(found);
    }

    SECTION("hit() causing bust advances state - AC-5b") {
        bool found = false;
        for (uint32_t seed = 1; seed < 100000; seed++) {
            Game game(seed);
            std::ignore = game.deal();
            if (game.state() != GameState::PlayerTurn)
                continue;

            for (int i = 0; i < 10; i++) {
                auto result = game.hit();
                if (result != ActionResult::Success)
                    break;

                if (game.player_hands()[0].hand.is_bust()) {
                    REQUIRE(game.state() != GameState::PlayerTurn);
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
        REQUIRE(found);
    }
}

TEST_CASE("Stand operation - AC-6", "[game]") {
    SECTION("stand() with single hand transitions to DealerTurn") {
        auto game = make_normal_game();
        std::ignore = game.deal();
        REQUIRE(game.state() == GameState::PlayerTurn);

        auto result = game.stand();
        REQUIRE(result == ActionResult::Success);
        REQUIRE(game.state() == GameState::DealerTurn);
    }

    SECTION("stand() with split advances to second hand") {
        bool found = false;
        for (uint32_t seed = 1; seed < 100000; seed++) {
            Game game(seed);
            std::ignore = game.deal();
            if (game.state() != GameState::PlayerTurn)
                continue;

            auto hands = game.player_hands();
            if (hands.empty() || !hands[0].hand.can_split())
                continue;
            if (hands[0].hand.cards()[0].rank == Rank::Ace)
                continue; // skip ace splits

            auto sr = game.split();
            if (sr != ActionResult::Success)
                continue;

            REQUIRE(game.player_hands().size() == 2);
            REQUIRE(game.active_hand_index() == 0);

            std::ignore = game.stand();
            REQUIRE(game.active_hand_index() == 1);
            REQUIRE(game.state() == GameState::PlayerTurn);
            found = true;
            break;
        }
        REQUIRE(found);
    }
}

TEST_CASE("Split operation - AC-7", "[game]") {
    SECTION("Split pair creates two hands") {
        bool found = false;
        for (uint32_t seed = 1; seed < 100000; seed++) {
            Game game(seed);
            std::ignore = game.deal();
            if (game.state() != GameState::PlayerTurn)
                continue;
            if (!game.player_hands()[0].hand.can_split())
                continue;

            auto result = game.split();
            REQUIRE(result == ActionResult::Success);
            REQUIRE(game.player_hands().size() == 2);
            REQUIRE(game.player_hands()[0].hand.size() == 2);
            REQUIRE(game.player_hands()[1].hand.size() == 2);
            found = true;
            break;
        }
        REQUIRE(found);
    }

    SECTION("Split aces auto-stands to DealerTurn") {
        bool found = false;
        for (uint32_t seed = 1; seed < 100000; seed++) {
            Game game(seed);
            std::ignore = game.deal();
            if (game.state() != GameState::PlayerTurn)
                continue;

            auto hands = game.player_hands();
            if (!hands[0].hand.can_split())
                continue;
            if (hands[0].hand.cards()[0].rank != Rank::Ace)
                continue;

            auto result = game.split();
            REQUIRE(result == ActionResult::Success);
            REQUIRE(game.state() == GameState::DealerTurn);
            found = true;
            break;
        }
        REQUIRE(found);
    }

    SECTION("Split non-pair returns InvalidSplit") {
        bool found = false;
        for (uint32_t seed = 1; seed < 10000; seed++) {
            Game game(seed);
            std::ignore = game.deal();
            if (game.state() != GameState::PlayerTurn)
                continue;
            if (game.player_hands()[0].hand.can_split())
                continue;

            auto result = game.split();
            REQUIRE(result == ActionResult::InvalidSplit);
            REQUIRE(game.player_hands().size() == 1);
            found = true;
            break;
        }
        REQUIRE(found);
    }

    SECTION("Re-split returns AlreadySplit") {
        bool found = false;
        for (uint32_t seed = 1; seed < 100000; seed++) {
            Game game(seed);
            std::ignore = game.deal();
            if (game.state() != GameState::PlayerTurn)
                continue;
            if (!game.player_hands()[0].hand.can_split())
                continue;
            if (game.player_hands()[0].hand.cards()[0].rank == Rank::Ace)
                continue;

            auto sr = game.split();
            if (sr != ActionResult::Success)
                continue;

            auto result = game.split();
            REQUIRE(result == ActionResult::AlreadySplit);
            found = true;
            break;
        }
        REQUIRE(found);
    }
}

TEST_CASE("Dealer logic - AC-8", "[game]") {
    SECTION("Dealer follows hit-soft-17 rule") {
        bool tested = false;
        for (uint32_t seed = 1; seed < 10000; seed++) {
            Game game(seed);
            std::ignore = game.deal();
            if (game.state() != GameState::PlayerTurn)
                continue;

            std::ignore = game.stand();
            REQUIRE(game.state() == GameState::DealerTurn);

            auto result = game.play_dealer();
            REQUIRE(result == ActionResult::Success);

            const auto& dealer = game.dealer_hand();
            bool valid = dealer.is_bust() || (dealer.value() >= 17 && !dealer.is_soft()) ||
                         (dealer.value() >= 18);
            REQUIRE(valid);
            tested = true;
            break;
        }
        REQUIRE(tested);
    }
}

TEST_CASE("Game results - AC-9", "[game]") {
    SECTION("Win with higher value") {
        bool found = false;
        for (uint32_t seed = 1; seed < 100000; seed++) {
            Game game(seed);
            std::ignore = game.deal();
            if (game.state() != GameState::PlayerTurn)
                continue;

            std::ignore = game.stand();
            std::ignore = game.play_dealer();

            if (game.state() != GameState::RoundOver)
                continue;

            auto hands = game.player_hands();
            if (hands[0].result == HandResult::Win && !hands[0].hand.is_bust() &&
                !game.dealer_hand().is_bust()) {
                REQUIRE(hands[0].hand.value() > game.dealer_hand().value());
                found = true;
                break;
            }
        }
        REQUIRE(found);
    }

    SECTION("Lose with lower value") {
        bool found = false;
        for (uint32_t seed = 1; seed < 100000; seed++) {
            Game game(seed);
            std::ignore = game.deal();
            if (game.state() != GameState::PlayerTurn)
                continue;

            std::ignore = game.stand();
            std::ignore = game.play_dealer();
            if (game.state() != GameState::RoundOver)
                continue;

            auto hands = game.player_hands();
            if (hands[0].result == HandResult::Lose && !hands[0].hand.is_bust() &&
                !game.dealer_hand().is_bust()) {
                REQUIRE(hands[0].hand.value() < game.dealer_hand().value());
                found = true;
                break;
            }
        }
        REQUIRE(found);
    }

    SECTION("Push with equal values") {
        bool found = false;
        for (uint32_t seed = 1; seed < 100000; seed++) {
            Game game(seed);
            std::ignore = game.deal();
            if (game.state() != GameState::PlayerTurn)
                continue;

            std::ignore = game.stand();
            std::ignore = game.play_dealer();
            if (game.state() != GameState::RoundOver)
                continue;

            auto hands = game.player_hands();
            if (hands[0].result == HandResult::Push && !hands[0].hand.is_bust() &&
                !game.dealer_hand().is_bust()) {
                REQUIRE(hands[0].hand.value() == game.dealer_hand().value());
                found = true;
                break;
            }
        }
        REQUIRE(found);
    }

    SECTION("Natural blackjack result") {
        bool found = false;
        for (uint32_t seed = 1; seed < 100000; seed++) {
            Game game(seed);
            std::ignore = game.deal();

            auto hands = game.player_hands();
            if (!hands.empty() && hands[0].hand.is_natural_blackjack() &&
                !game.dealer_hand().is_natural_blackjack()) {
                std::ignore = game.play_dealer();
                hands = game.player_hands();
                REQUIRE(hands[0].result == HandResult::Blackjack);
                found = true;
                break;
            }
        }
        REQUIRE(found);
    }

    SECTION("Player bust results in loss") {
        bool found = false;
        for (uint32_t seed = 1; seed < 100000; seed++) {
            Game game(seed);
            std::ignore = game.deal();
            if (game.state() != GameState::PlayerTurn)
                continue;

            for (int i = 0; i < 10; i++) {
                std::ignore = game.hit();
                auto hands = game.player_hands();
                if (hands[0].hand.is_bust()) {
                    // Play dealer to get results
                    if (game.state() == GameState::DealerTurn) {
                        std::ignore = game.play_dealer();
                    }
                    hands = game.player_hands();
                    REQUIRE(hands[0].result == HandResult::Lose);
                    found = true;
                    break;
                }
                if (game.state() != GameState::PlayerTurn)
                    break;
            }
            if (found)
                break;
        }
        REQUIRE(found);
    }

    SECTION("Dealer bust means player wins") {
        bool found = false;
        for (uint32_t seed = 1; seed < 100000; seed++) {
            Game game(seed);
            std::ignore = game.deal();
            if (game.state() != GameState::PlayerTurn)
                continue;

            std::ignore = game.stand();
            std::ignore = game.play_dealer();
            if (game.state() != GameState::RoundOver)
                continue;

            auto hands = game.player_hands();
            if (game.dealer_hand().is_bust() && !hands[0].hand.is_bust()) {
                REQUIRE(hands[0].result == HandResult::Win);
                found = true;
                break;
            }
        }
        REQUIRE(found);
    }
}

TEST_CASE("State integrity - AC-10", "[game]") {
    SECTION("hit() in RoundOver returns error") {
        auto game = make_normal_game();
        std::ignore = game.deal();
        std::ignore = game.stand();
        std::ignore = game.play_dealer();
        REQUIRE(game.state() == GameState::RoundOver);

        auto result = game.hit();
        REQUIRE(result == ActionResult::InvalidAction);
    }

    SECTION("split() in DealerTurn returns error") {
        auto game = make_normal_game();
        std::ignore = game.deal();
        std::ignore = game.stand();
        REQUIRE(game.state() == GameState::DealerTurn);

        auto result = game.split();
        REQUIRE(result == ActionResult::InvalidAction);
    }

    SECTION("Actions in WaitingForDeal return error") {
        Game game(12345);
        REQUIRE(game.state() == GameState::WaitingForDeal);
        REQUIRE(game.hit() == ActionResult::InvalidAction);
        REQUIRE(game.stand() == ActionResult::InvalidAction);
        REQUIRE(game.split() == ActionResult::InvalidAction);
    }
}

TEST_CASE("Full round flow - AC-11", "[game]") {
    SECTION("deal -> stand -> play_dealer -> deal again") {
        auto game = make_normal_game();
        std::ignore = game.deal();

        if (game.state() == GameState::PlayerTurn) {
            std::ignore = game.stand();
        }
        if (game.state() == GameState::DealerTurn) {
            std::ignore = game.play_dealer();
        }
        REQUIRE(game.state() == GameState::RoundOver);

        auto result = game.deal();
        REQUIRE(result == ActionResult::Success);
    }
}

TEST_CASE("Split aces 21 is Win not Blackjack - AC-12", "[game]") {
    bool found = false;
    for (uint32_t seed = 1; seed < 100000; seed++) {
        Game game(seed);
        std::ignore = game.deal();
        if (game.state() != GameState::PlayerTurn)
            continue;

        auto hands = game.player_hands();
        if (!hands[0].hand.can_split())
            continue;
        if (hands[0].hand.cards()[0].rank != Rank::Ace)
            continue;

        std::ignore = game.split();
        if (game.state() != GameState::DealerTurn)
            continue;

        std::ignore = game.play_dealer();
        hands = game.player_hands();

        for (const auto& hs : hands) {
            if (hs.hand.value() == 21) {
                REQUIRE(hs.result != HandResult::Blackjack);
                found = true;
                break;
            }
        }
        if (found)
            break;
    }
    REQUIRE(found);
}

TEST_CASE("Dealer natural vs non-natural - AC-12b", "[game]") {
    bool found = false;
    for (uint32_t seed = 1; seed < 100000; seed++) {
        Game game(seed);
        std::ignore = game.deal();

        auto hands = game.player_hands();
        if (hands.empty())
            continue;

        if (game.dealer_hand().is_natural_blackjack() && !hands[0].hand.is_natural_blackjack()) {
            // Natural blackjack on dealer should have advanced past PlayerTurn
            if (game.state() == GameState::DealerTurn) {
                std::ignore = game.play_dealer();
            }
            hands = game.player_hands();
            REQUIRE(hands[0].result == HandResult::Lose);
            found = true;
            break;
        }
    }
    REQUIRE(found);
}

TEST_CASE("Available actions - AC-13", "[game]") {
    SECTION("PlayerTurn with pair offers Hit/Stand/Split") {
        bool found = false;
        for (uint32_t seed = 1; seed < 100000; seed++) {
            Game game(seed);
            std::ignore = game.deal();
            if (game.state() != GameState::PlayerTurn)
                continue;
            if (!game.player_hands()[0].hand.can_split())
                continue;

            auto actions = game.available_actions();
            bool has_hit = false, has_stand = false, has_split = false;
            for (auto a : actions) {
                if (a == PlayerAction::Hit)
                    has_hit = true;
                if (a == PlayerAction::Stand)
                    has_stand = true;
                if (a == PlayerAction::Split)
                    has_split = true;
            }
            REQUIRE(has_hit);
            REQUIRE(has_stand);
            REQUIRE(has_split);
            found = true;
            break;
        }
        REQUIRE(found);
    }

    SECTION("PlayerTurn without pair offers Hit/Stand only") {
        bool found = false;
        for (uint32_t seed = 1; seed < 10000; seed++) {
            Game game(seed);
            std::ignore = game.deal();
            if (game.state() != GameState::PlayerTurn)
                continue;
            if (game.player_hands()[0].hand.can_split())
                continue;

            auto actions = game.available_actions();
            bool has_hit = false, has_stand = false, has_split = false;
            for (auto a : actions) {
                if (a == PlayerAction::Hit)
                    has_hit = true;
                if (a == PlayerAction::Stand)
                    has_stand = true;
                if (a == PlayerAction::Split)
                    has_split = true;
            }
            REQUIRE(has_hit);
            REQUIRE(has_stand);
            REQUIRE_FALSE(has_split);
            found = true;
            break;
        }
        REQUIRE(found);
    }

    SECTION("Non-PlayerTurn states have empty actions") {
        Game game(12345);
        REQUIRE(game.available_actions().empty());

        std::ignore = game.deal();
        if (game.state() == GameState::PlayerTurn) {
            std::ignore = game.stand();
            REQUIRE(game.available_actions().empty());
        }
    }
}

TEST_CASE("Deterministic seed - AC-14", "[game]") {
    SECTION("Same seed and same actions produce identical results") {
        Game game1(42);
        Game game2(42);

        std::ignore = game1.deal();
        std::ignore = game2.deal();

        auto h1 = game1.player_hands();
        auto h2 = game2.player_hands();
        REQUIRE(h1[0].hand.size() == h2[0].hand.size());
        for (std::size_t i = 0; i < h1[0].hand.size(); i++) {
            REQUIRE(h1[0].hand.cards()[i] == h2[0].hand.cards()[i]);
        }

        REQUIRE(game1.dealer_hand().size() == game2.dealer_hand().size());
        REQUIRE(game1.state() == game2.state());

        if (game1.state() == GameState::PlayerTurn) {
            std::ignore = game1.hit();
            std::ignore = game2.hit();
            h1 = game1.player_hands();
            h2 = game2.player_hands();
            REQUIRE(h1[0].hand.size() == h2[0].hand.size());
            REQUIRE(game1.state() == game2.state());
        }
    }

    SECTION("Multiple rounds with same seed are reproducible") {
        Game game1(999);
        Game game2(999);

        std::ignore = game1.deal();
        std::ignore = game2.deal();

        if (game1.state() == GameState::PlayerTurn) {
            std::ignore = game1.stand();
            std::ignore = game2.stand();
        }
        if (game1.state() == GameState::DealerTurn) {
            std::ignore = game1.play_dealer();
            std::ignore = game2.play_dealer();
        }

        REQUIRE(game1.state() == game2.state());
        REQUIRE(game1.player_hands()[0].result == game2.player_hands()[0].result);

        std::ignore = game1.deal();
        std::ignore = game2.deal();
        REQUIRE(game1.state() == game2.state());
    }
}
