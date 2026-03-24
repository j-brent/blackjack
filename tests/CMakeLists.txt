include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(blackjack_tests
    test_card.cpp
    test_hand.cpp
    test_deck.cpp
    test_game.cpp
)

target_link_libraries(blackjack_tests PRIVATE blackjack_lib Catch2::Catch2WithMain)

target_compile_features(blackjack_tests PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(blackjack_tests PRIVATE /W4)
else()
    target_compile_options(blackjack_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

# clang-tidy is intentionally NOT applied to the test target.
# Catch2 macros expand to code that triggers false positives
# (naming conventions, modernize suggestions) in Catch2 internal headers.
# The library target is already analyzed — that's where the checks matter.

include(CTest)
include(Catch)
catch_discover_tests(blackjack_tests)
