#include "blackjack/game.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include <vector>

using namespace ftxui;
using namespace blackjack;

// ========================= Helper Functions =========================

// Convert rank to string
std::string rank_to_string(Rank rank) {
    switch (rank) {
        case Rank::Ace: return "A";
        case Rank::Two: return "2";
        case Rank::Three: return "3";
        case Rank::Four: return "4";
        case Rank::Five: return "5";
        case Rank::Six: return "6";
        case Rank::Seven: return "7";
        case Rank::Eight: return "8";
        case Rank::Nine: return "9";
        case Rank::Ten: return "10";
        case Rank::Jack: return "J";
        case Rank::Queen: return "Q";
        case Rank::King: return "K";
    }
    return "";
}

// Get suit symbol
std::string suit_to_symbol(Suit suit) {
    switch (suit) {
        case Suit::Spades: return "\u2660";
        case Suit::Hearts: return "\u2665";
        case Suit::Diamonds: return "\u2666";
        case Suit::Clubs: return "\u2663";
    }
    return "";
}

// Determine if suit should be red
bool is_red_suit(Suit suit) {
    return suit == Suit::Hearts || suit == Suit::Diamonds;
}

// Render a face-up card as a 7x5 element
Element render_card(const Card& card) {
    std::string rank_str = rank_to_string(card.rank);
    std::string suit_str = suit_to_symbol(card.suit);
    Color suit_color = is_red_suit(card.suit) ? Color::Red : Color::White;

    std::vector<Element> lines;

    // Top border
    lines.push_back(text("\u250c\u2500\u2500\u2500\u2500\u2500\u2510") | color(Color::GrayDark));

    // Rank row (top-left)
    if (rank_str == "10") {
        lines.push_back(
            hbox({
                text("\u2502") | color(Color::GrayDark),
                text("10   ") | color(suit_color),
                text("\u2502") | color(Color::GrayDark)
            })
        );
    } else {
        lines.push_back(
            hbox({
                text("\u2502") | color(Color::GrayDark),
                text(rank_str + "    ") | color(suit_color),
                text("\u2502") | color(Color::GrayDark)
            })
        );
    }

    // Suit row (centered)
    lines.push_back(
        hbox({
            text("\u2502") | color(Color::GrayDark),
            text("  " + suit_str + "  ") | color(suit_color),
            text("\u2502") | color(Color::GrayDark)
        })
    );

    // Rank row (bottom-right)
    if (rank_str == "10") {
        lines.push_back(
            hbox({
                text("\u2502") | color(Color::GrayDark),
                text("  10 ") | color(suit_color),
                text("\u2502") | color(Color::GrayDark)
            })
        );
    } else {
        lines.push_back(
            hbox({
                text("\u2502") | color(Color::GrayDark),
                text("   " + rank_str + " ") | color(suit_color),
                text("\u2502") | color(Color::GrayDark)
            })
        );
    }

    // Bottom border
    lines.push_back(text("\u2514\u2500\u2500\u2500\u2500\u2500\u2518") | color(Color::GrayDark));

    return vbox(lines);
}

// Render a face-down card (dealer hole card)
Element render_face_down_card() {
    std::vector<Element> lines;

    lines.push_back(text("\u250c\u2500\u2500\u2500\u2500\u2500\u2510") | color(Color::GrayDark));
    lines.push_back(
        hbox({
            text("\u2502") | color(Color::GrayDark),
            text("\u2591\u2591\u2591\u2591\u2591") | color(Color::GrayDark),
            text("\u2502") | color(Color::GrayDark)
        })
    );
    lines.push_back(
        hbox({
            text("\u2502") | color(Color::GrayDark),
            text("\u2591\u2591\u2591\u2591\u2591") | color(Color::GrayDark),
            text("\u2502") | color(Color::GrayDark)
        })
    );
    lines.push_back(
        hbox({
            text("\u2502") | color(Color::GrayDark),
            text("\u2591\u2591\u2591\u2591\u2591") | color(Color::GrayDark),
            text("\u2502") | color(Color::GrayDark)
        })
    );
    lines.push_back(text("\u2514\u2500\u2500\u2500\u2500\u2500\u2518") | color(Color::GrayDark));

    return vbox(lines);
}

// Render a hand of cards
Element render_hand(std::span<const Card> cards, bool show_all = true, bool hole_card_visible = false) {
    if (cards.empty()) {
        return text("");
    }

    std::vector<Element> card_elements;
    for (size_t i = 0; i < cards.size(); ++i) {
        if (i == 1 && !show_all && !hole_card_visible) {
            // Dealer's hole card
            card_elements.push_back(render_face_down_card());
        } else {
            card_elements.push_back(render_card(cards[i]));
        }

        // Add spacing between cards
        if (i < cards.size() - 1) {
            card_elements.push_back(text(" "));
        }
    }

    return hbox(card_elements);
}

// Build dealer panel
Element build_dealer_panel(const Game& game) {
    auto dealer_hand = game.dealer_hand();
    auto cards = dealer_hand.cards();
    bool hole_visible = game.is_dealer_hole_card_visible();

    std::vector<Element> content;

    // Label
    content.push_back(text("  Dealer's Hand") | color(Color::White));
    content.push_back(text(""));

    // Cards
    content.push_back(hbox({text("  "), render_hand(cards, hole_visible, hole_visible)}));

    // Value
    std::string value_text;
    if (hole_visible) {
        value_text = "  Value: " + std::to_string(dealer_hand.value());
    } else if (!cards.empty()) {
        // Show only visible card value during player turn
        int visible_value = card_value(cards[0].rank);
        value_text = "  Value: " + std::to_string(visible_value);
    }

    if (!value_text.empty()) {
        content.push_back(text(""));
        content.push_back(text(value_text) | color(Color::GrayLight));
    } else {
        content.push_back(text(""));
    }

    content.push_back(text(""));

    return vbox(content) | border | color(Color::GrayDark);
}

// Build player panel for single hand
Element build_player_panel_single(const Game& game) {
    auto player_hands = game.player_hands();
    if (player_hands.empty()) {
        return text("");
    }

    const HandState& hand_state = player_hands[0];

    std::vector<Element> content;

    // Label
    content.push_back(text("  Your Hand") | color(Color::White));
    content.push_back(text(""));

    // Cards
    content.push_back(hbox({text("  "), render_hand(hand_state.hand.cards())}));

    // Value
    std::string value_text = "  Value: " + std::to_string(hand_state.hand.value());
    if (hand_state.hand.is_soft()) {
        value_text += " (soft)";
    }
    if (hand_state.hand.is_bust()) {
        value_text += " BUST";
    }

    Color value_color = hand_state.hand.is_bust() ? Color::Red : Color::GrayLight;

    content.push_back(text(""));
    content.push_back(text(value_text) | color(value_color));
    content.push_back(text(""));

    return vbox(content) | border | color(Color::GrayDark);
}

// Build player panel for split hands
Element build_player_panel_split(const Game& game, int terminal_width) {
    auto player_hands = game.player_hands();
    if (player_hands.size() < 2) {
        return build_player_panel_single(game);
    }

    size_t active_index = game.active_hand_index();

    // Helper to build a single hand sub-panel
    auto build_hand_subpanel = [&](size_t index) -> Element {
        const HandState& hand_state = player_hands[index];
        bool is_active = (index == active_index);

        std::vector<Element> content;

        // Label
        std::string label = "  Hand " + std::to_string(index + 1);
        content.push_back(text(label) | (is_active ? (color(Color::White) | bold) : color(Color::GrayLight)));
        content.push_back(text(""));

        // Cards
        content.push_back(hbox({text("  "), render_hand(hand_state.hand.cards())}));

        // Value
        std::string value_text = "  Value: " + std::to_string(hand_state.hand.value());
        if (hand_state.hand.is_soft()) {
            value_text += " (soft)";
        }
        if (hand_state.hand.is_bust()) {
            value_text += " BUST";
        }

        Color value_color = hand_state.hand.is_bust() ? Color::Red : Color::GrayLight;

        content.push_back(text(""));
        content.push_back(text(value_text) | color(value_color));
        content.push_back(text(""));

        Color border_color = is_active ? Color::White : Color::GrayDark;
        return vbox(content) | border | color(border_color);
    };

    // Side-by-side if width >= 60, otherwise stacked
    if (terminal_width >= 60) {
        return hbox({
            build_hand_subpanel(0) | flex,
            separator(),
            build_hand_subpanel(1) | flex
        });
    } else {
        return vbox({
            build_hand_subpanel(0),
            separator(),
            build_hand_subpanel(1)
        });
    }
}

// Build status bar text with color
Element build_status_bar(const Game& game) {
    GameState state = game.state();
    std::string message;
    Color message_color = Color::GrayLight;

    switch (state) {
        case GameState::WaitingForDeal:
            message = "  Press (N)ew Game to start";
            break;

        case GameState::PlayerTurn: {
            auto hands = game.player_hands();
            if (hands.size() > 1) {
                // Split hands
                size_t active = game.active_hand_index();
                message = "  Your turn -- Hand " + std::to_string(active + 1);
            } else {
                message = "  Your turn";
            }
            break;
        }

        case GameState::DealerTurn:
            message = "  Dealer's turn...";
            break;

        case GameState::RoundOver: {
            auto hands = game.player_hands();
            if (hands.size() > 1) {
                // Split hands - show results for both
                std::string result1, result2;
                Color color1 = Color::GrayLight, color2 = Color::GrayLight;

                switch (hands[0].result) {
                    case HandResult::Win:
                    case HandResult::Blackjack:
                        result1 = "Win";
                        color1 = Color::Green;
                        break;
                    case HandResult::Lose:
                        result1 = "Lose";
                        color1 = Color::Red;
                        break;
                    case HandResult::Push:
                        result1 = "Push";
                        color1 = Color::Yellow;
                        break;
                    default:
                        if (hands[0].hand.is_bust()) {
                            result1 = "Bust";
                            color1 = Color::Red;
                        }
                        break;
                }

                switch (hands[1].result) {
                    case HandResult::Win:
                    case HandResult::Blackjack:
                        result2 = "Win";
                        color2 = Color::Green;
                        break;
                    case HandResult::Lose:
                        result2 = "Lose";
                        color2 = Color::Red;
                        break;
                    case HandResult::Push:
                        result2 = "Push";
                        color2 = Color::Yellow;
                        break;
                    default:
                        if (hands[1].hand.is_bust()) {
                            result2 = "Bust";
                            color2 = Color::Red;
                        }
                        break;
                }

                // For split, we use a simple combined message for now
                message = "  Hand 1: " + result1 + " | Hand 2: " + result2;
                // Use the first hand's color (simplified)
                message_color = color1;
            } else if (!hands.empty()) {
                // Single hand result
                const HandState& hand = hands[0];

                if (hand.hand.is_bust()) {
                    message = "  Bust!";
                    message_color = Color::Red;
                } else {
                    switch (hand.result) {
                        case HandResult::Blackjack:
                            message = "  Blackjack!";
                            message_color = Color::Green;
                            break;
                        case HandResult::Win:
                            message = "  You win!";
                            message_color = Color::Green;
                            break;
                        case HandResult::Lose:
                            message = "  You lose.";
                            message_color = Color::Red;
                            break;
                        case HandResult::Push:
                            message = "  Push.";
                            message_color = Color::Yellow;
                            break;
                        default:
                            message = "  Round over";
                            break;
                    }
                }
            }
            break;
        }
    }

    return text(message) | color(message_color);
}

// ========================= Main Application =========================

int main() {
    // Create game
    Game game;

    // Auto-deal on launch
    if (game.deal() != ActionResult::Success) {
        return 1;
    }

    // Create screen
    auto screen = ScreenInteractive::Fullscreen();

    // Track terminal dimensions
    int terminal_width = 80;
    int terminal_height = 24;

    // Helper: play dealer if state transitioned to DealerTurn.
    // The guard guarantees play_dealer() succeeds (single-threaded,
    // state verified immediately before call).
    auto play_dealer_if_needed = [&]() {
        if (game.state() == GameState::DealerTurn) {
            auto result = game.play_dealer();
            assert(result == ActionResult::Success);
        }
    };

    // Handle natural blackjack on initial deal
    play_dealer_if_needed();

    // --- Dynamic controls (shown/hidden via Maybe) ---

    auto hit_btn = Button(" (H)it ", [&]() {
        if (game.hit() == ActionResult::Success) {
            play_dealer_if_needed();
        }
    }, ButtonOption::Ascii());

    auto stand_btn = Button(" (S)tand ", [&]() {
        if (game.stand() == ActionResult::Success) {
            play_dealer_if_needed();
        }
    }, ButtonOption::Ascii());

    auto split_btn = Button(" S(p)lit ", [&]() {
        if (game.split() == ActionResult::Success) {
            play_dealer_if_needed();
        }
    }, ButtonOption::Ascii());

    auto deal_btn = Button(" (N)ew Game ", [&]() {
        if (game.deal() == ActionResult::Success) {
            play_dealer_if_needed();
        }
    }, ButtonOption::Ascii());

    auto quit_btn = Button(" (Q)uit ", [&]() {
        screen.ExitLoopClosure()();
    }, ButtonOption::Ascii());

    auto hit_maybe = Maybe(hit_btn, [&]() {
        auto actions = game.available_actions();
        return std::ranges::find(actions, PlayerAction::Hit) != actions.end();
    });
    auto stand_maybe = Maybe(stand_btn, [&]() {
        auto actions = game.available_actions();
        return std::ranges::find(actions, PlayerAction::Stand) != actions.end();
    });
    auto split_maybe = Maybe(split_btn, [&]() {
        auto actions = game.available_actions();
        return std::ranges::find(actions, PlayerAction::Split) != actions.end();
    });
    auto deal_maybe = Maybe(deal_btn, [&]() {
        auto s = game.state();
        return s == GameState::RoundOver || s == GameState::WaitingForDeal;
    });

    auto controls_container = Container::Horizontal({
        hit_maybe, stand_maybe, split_maybe, deal_maybe, quit_btn
    });

    auto ui_renderer = Renderer(controls_container, [&] {
        // Get terminal dimensions
        auto dimensions = Terminal::Size();
        terminal_width = dimensions.dimx;
        terminal_height = dimensions.dimy;

        // Check minimum size
        if (terminal_width < 40 || terminal_height < 20) {
            return vbox({
                text(""),
                text("  Please resize terminal") | center,
                text("  (40x20 minimum)") | center,
                text("")
            }) | border | color(Color::Yellow);
        }

        // Build panels
        auto title = text("BLACKJACK") | center | bold | color(Color::White);
        auto title_panel = vbox({title}) | border | color(Color::GrayDark);

        auto dealer_panel = build_dealer_panel(game);

        Element player_panel;
        auto hands = game.player_hands();
        if (hands.size() > 1) {
            player_panel = build_player_panel_split(game, terminal_width);
        } else {
            player_panel = build_player_panel_single(game);
        }

        // Controls panel - render the interactive component
        auto controls_element = controls_container->Render();
        auto controls_panel = vbox({
            hbox({text("  "), controls_element})
        }) | border | color(Color::GrayDark);

        // Status bar
        auto status_element = build_status_bar(game);
        auto status_panel = vbox({status_element}) | border | color(Color::GrayDark);

        // Combine all panels
        return vbox({
            title_panel,
            dealer_panel | flex,
            player_panel | flex,
            controls_panel,
            status_panel
        });
    });

    // Handle keyboard shortcuts
    auto event_handler = CatchEvent(ui_renderer, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Character('Q') || event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        }

        GameState state = game.state();

        if (state == GameState::RoundOver || state == GameState::WaitingForDeal) {
            if (event == Event::Character('n') || event == Event::Character('N')) {
                if (game.deal() == ActionResult::Success) {
                    play_dealer_if_needed();
                    return true;
                }
            }
        }

        if (state == GameState::PlayerTurn) {
            auto available = game.available_actions();

            if (event == Event::Character('h') || event == Event::Character('H')) {
                if (std::ranges::find(available, PlayerAction::Hit) != available.end()) {
                    if (game.hit() == ActionResult::Success) {
                        play_dealer_if_needed();
                        return true;
                    }
                }
            }

            if (event == Event::Character('s') || event == Event::Character('S')) {
                if (std::ranges::find(available, PlayerAction::Stand) != available.end()) {
                    if (game.stand() == ActionResult::Success) {
                        play_dealer_if_needed();
                        return true;
                    }
                }
            }

            if (event == Event::Character('p') || event == Event::Character('P')) {
                if (std::ranges::find(available, PlayerAction::Split) != available.end()) {
                    if (game.split() == ActionResult::Success) {
                        play_dealer_if_needed();
                        return true;
                    }
                }
            }
        }

        return false;
    });

    // Run the screen loop
    screen.Loop(event_handler);

    return 0;
}
