#include "blackjack/game.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

// Forward declarations
std::string rank_to_string(blackjack::Rank rank);
std::string suit_to_string(blackjack::Suit suit);
std::string card_to_string(const blackjack::Card& card);
std::string hand_to_string(const blackjack::Hand& hand, bool show_soft = true);
void display_game_state(const blackjack::Game& game);
void display_results(const blackjack::Game& game);
void display_dealer_reveal(const blackjack::Game& game);
std::string trim(const std::string& str);
std::string to_lower(const std::string& str);
bool play_round(blackjack::Game& game);

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    blackjack::Game game;

    // Print welcome header
    std::cout << "------------------------------------------------------------\n";
    std::cout << "\n";
    std::cout << "                        BLACKJACK\n";
    std::cout << "\n";
    std::cout << "------------------------------------------------------------\n";

    // Main game loop
    bool play_again = true;
    while (play_again) {
        play_again = play_round(game);
    }

    return 0;
}

std::string rank_to_string(blackjack::Rank rank) {
    switch (rank) {
        case blackjack::Rank::Ace:   return "A";
        case blackjack::Rank::Two:   return "2";
        case blackjack::Rank::Three: return "3";
        case blackjack::Rank::Four:  return "4";
        case blackjack::Rank::Five:  return "5";
        case blackjack::Rank::Six:   return "6";
        case blackjack::Rank::Seven: return "7";
        case blackjack::Rank::Eight: return "8";
        case blackjack::Rank::Nine:  return "9";
        case blackjack::Rank::Ten:   return "10";
        case blackjack::Rank::Jack:  return "J";
        case blackjack::Rank::Queen: return "Q";
        case blackjack::Rank::King:  return "K";
    }
    return "?";
}

std::string suit_to_string(blackjack::Suit suit) {
    switch (suit) {
        case blackjack::Suit::Spades:   return "\u2660";  // ♠
        case blackjack::Suit::Hearts:   return "\u2665";  // ♥
        case blackjack::Suit::Diamonds: return "\u2666";  // ♦
        case blackjack::Suit::Clubs:    return "\u2663";  // ♣
    }
    return "?";
}

std::string card_to_string(const blackjack::Card& card) {
    std::string rank = rank_to_string(card.rank);
    std::string suit = suit_to_string(card.suit);

    if (rank == "10") {
        return "[10" + suit + " ]";
    } else {
        return "[ " + rank + suit + " ]";
    }
}

std::string hand_to_string(const blackjack::Hand& hand, bool show_soft) {
    std::string result;

    for (const auto& card : hand.cards()) {
        if (!result.empty()) {
            result += " ";
        }
        result += card_to_string(card);
    }

    result += "  (" + std::to_string(hand.value());
    if (show_soft && hand.is_soft()) {
        result += ", soft";
    }
    result += ")";

    if (hand.is_bust()) {
        result += " BUST";
    }

    return result;
}

void display_game_state(const blackjack::Game& game) {
    std::cout << "------------------------------------------------------------\n";
    std::cout << "                        BLACKJACK\n";
    std::cout << "------------------------------------------------------------\n";
    std::cout << "\n";

    // Display dealer hand
    std::cout << "Dealer:    ";
    const auto& dealer_hand = game.dealer_hand();
    auto dealer_cards = dealer_hand.cards();

    if (game.is_dealer_hole_card_visible()) {
        // Show all cards
        std::cout << hand_to_string(dealer_hand, false);
    } else {
        // Show first card and hidden card
        if (!dealer_cards.empty()) {
            std::cout << card_to_string(dealer_cards[0]) << " [??]";
        }
    }
    std::cout << "\n";
    std::cout << "\n";

    // Display player hand(s)
    auto player_hands = game.player_hands();
    bool has_split = player_hands.size() > 1;

    if (has_split) {
        for (size_t i = 0; i < player_hands.size(); ++i) {
            const auto& hand_state = player_hands[i];
            bool is_active = (i == game.active_hand_index());

            std::cout << "Hand " << (i + 1) << (is_active ? " *:" : "  :");
            std::cout << "  ";  // Two spaces after label to reach 11 chars total
            std::cout << hand_to_string(hand_state.hand);
            std::cout << "\n";
        }
    } else {
        std::cout << "Your hand: " << hand_to_string(player_hands[0].hand);
        std::cout << "\n";
    }
    std::cout << "\n";
}

void display_dealer_reveal(const blackjack::Game& game) {
    std::cout << "Dealer reveals hole card...\n";
    std::cout << "\n";

    // Show only the first two cards for the reveal
    const auto& dealer_hand = game.dealer_hand();
    auto cards = dealer_hand.cards();

    std::string cards_str;
    for (size_t i = 0; i < std::min<size_t>(2, cards.size()); ++i) {
        if (!cards_str.empty()) {
            cards_str += " ";
        }
        cards_str += card_to_string(cards[i]);
    }

    // Calculate value of first two cards
    blackjack::Hand initial_hand;
    for (size_t i = 0; i < std::min<size_t>(2, cards.size()); ++i) {
        initial_hand.add_card(cards[i]);
    }

    std::cout << "Dealer:    " << cards_str << "  (" << initial_hand.value() << ")\n";
}

void display_results(const blackjack::Game& game) {
    auto player_hands = game.player_hands();
    bool has_split = player_hands.size() > 1;

    std::cout << "------------------------------------------------------------\n";

    if (has_split) {
        std::cout << "Results:\n";
        for (size_t i = 0; i < player_hands.size(); ++i) {
            const auto& hand_state = player_hands[i];
            std::cout << "  Hand " << (i + 1) << ": ";

            if (hand_state.hand.is_bust()) {
                std::cout << "Bust.\n";
            } else {
                switch (hand_state.result) {
                    case blackjack::HandResult::Win:
                    case blackjack::HandResult::Blackjack:
                        std::cout << "Win!\n";
                        break;
                    case blackjack::HandResult::Lose:
                        std::cout << "Lose.\n";
                        break;
                    case blackjack::HandResult::Push:
                        std::cout << "Push.\n";
                        break;
                    default:
                        break;
                }
            }
        }
    } else {
        const auto& hand_state = player_hands[0];
        std::cout << "Result: ";

        if (hand_state.result == blackjack::HandResult::Blackjack) {
            // Check if dealer also has blackjack
            if (game.dealer_hand().is_natural_blackjack()) {
                std::cout << "Push -- dealer also has blackjack.\n";
            } else {
                std::cout << "Blackjack! You win!\n";
            }
        } else {
            switch (hand_state.result) {
                case blackjack::HandResult::Win:
                    std::cout << "You win!\n";
                    break;
                case blackjack::HandResult::Lose:
                    std::cout << "You lose.\n";
                    break;
                case blackjack::HandResult::Push:
                    std::cout << "Push.\n";
                    break;
                default:
                    break;
            }
        }
    }

    std::cout << "------------------------------------------------------------\n";
}

std::string trim(const std::string& str) {
    auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
        return std::isspace(ch);
    });
    auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
        return std::isspace(ch);
    }).base();

    return (start < end) ? std::string(start, end) : std::string();
}

std::string to_lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return result;
}

bool play_round(blackjack::Game& game) {
    // Deal cards
    auto deal_result = game.deal();
    if (deal_result != blackjack::ActionResult::Success) {
        std::cout << "An unexpected error occurred. Exiting.\n";
        return false;
    }

    // Display initial state
    display_game_state(game);

    // Check for natural blackjack
    auto player_hands = game.player_hands();
    if (player_hands[0].hand.is_natural_blackjack()) {
        std::cout << "Blackjack!\n";
        std::cout << "\n";

        // Dealer turn
        auto play_result = game.play_dealer();
        if (play_result != blackjack::ActionResult::Success) {
            std::cout << "An unexpected error occurred. Exiting.\n";
            return false;
        }
        display_dealer_reveal(game);

        // Show final dealer action
        const auto& dealer_hand = game.dealer_hand();
        if (dealer_hand.is_bust()) {
            std::cout << "Dealer busts!\n";
        } else {
            std::cout << "Dealer stands.\n";
        }
        std::cout << "\n";

        display_results(game);

        // Play again prompt
        while (true) {
            std::cout << "\nPlay again? [y/n]: ";
            std::string input;
            if (!std::getline(std::cin, input)) {
                return false;  // EOF
            }

            input = trim(to_lower(input));
            if (input == "y" || input == "yes") {
                return true;
            } else if (input == "n" || input == "no") {
                return false;
            } else {
                std::cout << "Please enter 'y' or 'n'.\n";
            }
        }
    }

    // Player turn loop
    while (game.state() == blackjack::GameState::PlayerTurn) {
        // Get available actions
        auto actions = game.available_actions();
        bool can_split = std::find(actions.begin(), actions.end(),
                                    blackjack::PlayerAction::Split) != actions.end();

        // Display prompt
        std::cout << "Actions: [h]it  [s]tand";
        if (can_split) {
            std::cout << "  s[p]lit";
        }
        std::cout << "  [q]uit\n";
        std::cout << ">";

        // Read input
        std::string input;
        if (!std::getline(std::cin, input)) {
            return false;  // EOF
        }

        input = trim(to_lower(input));
        if (input.empty()) {
            continue;
        }

        // Parse command
        char first_char = input[0];
        blackjack::ActionResult result = blackjack::ActionResult::InvalidAction;

        if (first_char == 'q' || input == "quit") {
            return false;
        } else if (first_char == 'h' || input == "hit") {
            // Capture index before hit (hit may advance to next hand on bust)
            auto pre_hands = game.player_hands();
            bool has_split = pre_hands.size() > 1;
            size_t hit_idx = game.active_hand_index();

            result = game.hit();

            if (result == blackjack::ActionResult::Success) {
                display_game_state(game);

                // Check if the hand we hit busted
                auto current_hands = game.player_hands();
                if (current_hands[hit_idx].hand.is_bust()) {
                    if (has_split && hit_idx == 0) {
                        std::cout << "Hand 1: Bust! --- Now playing Hand 2 ---\n";
                        std::cout << "\n";
                        display_game_state(game);
                    } else {
                        std::cout << "Bust!\n";
                    }
                }
            }
        } else if (first_char == 'p' || input == "split") {
            // Split must be checked before stand — "split" starts with 's'
            // which would otherwise match the stand handler's first_char check
            result = game.split();

            if (result == blackjack::ActionResult::Success) {
                display_game_state(game);
            }
        } else if (first_char == 's' || input == "stand") {
            // Check if this is hand 1 of a split before standing
            auto current_hands = game.player_hands();
            bool has_split = current_hands.size() > 1;
            size_t active_idx = game.active_hand_index();
            bool is_hand1 = has_split && active_idx == 0;

            result = game.stand();

            if (result == blackjack::ActionResult::Success) {
                // Check if we transitioned to hand 2
                if (is_hand1 && game.state() == blackjack::GameState::PlayerTurn) {
                    std::cout << "--- Hand 1 complete. Now playing Hand 2 ---\n";
                    std::cout << "\n";
                    display_game_state(game);
                }
            }
        } else {
            std::cout << "Invalid command. Options: hit (h), stand (s)";
            if (can_split) {
                std::cout << ", split (p)";
            }
            std::cout << "\n";
            continue;
        }

        // Handle action errors
        if (result != blackjack::ActionResult::Success) {
            std::cout << "Error: action not available right now.\n";
        }
    }

    // Dealer turn
    if (game.state() == blackjack::GameState::DealerTurn) {
        // The play_dealer() call happens instantly, so we need to show the progression
        // by checking the dealer's hand state before and after
        auto play_result = game.play_dealer();
        if (play_result != blackjack::ActionResult::Success) {
            std::cout << "An unexpected error occurred. Exiting.\n";
            return false;
        }

        display_dealer_reveal(game);

        const auto& dealer_hand = game.dealer_hand();
        size_t num_cards = dealer_hand.cards().size();

        // Show any additional hits beyond the initial 2 cards
        if (num_cards > 2) {
            for (size_t i = 2; i < num_cards; ++i) {
                std::cout << "Dealer hits...\n";

                // Build hand display up to current card
                std::string cards_str;
                for (size_t j = 0; j <= i; ++j) {
                    if (!cards_str.empty()) {
                        cards_str += " ";
                    }
                    cards_str += card_to_string(dealer_hand.cards()[j]);
                }

                // Calculate value up to current card
                blackjack::Hand partial_hand;
                for (size_t j = 0; j <= i; ++j) {
                    partial_hand.add_card(dealer_hand.cards()[j]);
                }

                std::cout << "Dealer:    " << cards_str << "  (" << partial_hand.value() << ")\n";
            }
        }

        // Show final dealer action
        if (dealer_hand.is_bust()) {
            std::cout << "Dealer busts!\n";
        } else {
            std::cout << "Dealer stands.\n";
        }
        std::cout << "\n";
    }

    // Display results
    display_results(game);

    // Play again prompt
    while (true) {
        std::cout << "\nPlay again? [y/n]: ";
        std::string input;
        if (!std::getline(std::cin, input)) {
            return false;  // EOF
        }

        input = trim(to_lower(input));
        if (input == "y" || input == "yes") {
            return true;
        } else if (input == "n" || input == "no") {
            return false;
        } else {
            std::cout << "Please enter 'y' or 'n'.\n";
        }
    }
}
