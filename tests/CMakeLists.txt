include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(blackjack_tests
    src/test_card.cpp
    src/test_hand.cpp
    src/test_deck.cpp
    src/test_game.cpp
)

target_link_libraries(blackjack_tests PRIVATE blackjack_lib Catch2::Catch2WithMain)

target_compile_features(blackjack_tests PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(blackjack_tests PRIVATE /W4)
else()
    target_compile_options(blackjack_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(CTest)
include(Catch)
catch_discover_tests(blackjack_tests)
