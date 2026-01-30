add_library(blackjack_lib STATIC
    src/types.cpp
    src/hand.cpp
    src/deck.cpp
    src/game.cpp
)

target_include_directories(blackjack_lib PUBLIC include)

target_compile_features(blackjack_lib PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(blackjack_lib PRIVATE /W4)
else()
    target_compile_options(blackjack_lib PRIVATE -Wall -Wextra -Wpedantic)
endif()
