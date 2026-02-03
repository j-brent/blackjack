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
    # MSVC: -fno-exceptions has no safe equivalent. The MSVC STL uses exception
    # machinery internally (<random>, <vector>, <optional>). No-exceptions policy
    # is enforced by clang-tidy and code review on MSVC builds.
else()
    target_compile_options(blackjack_lib PRIVATE -Wall -Wextra -Wpedantic -fno-exceptions)
endif()

if(BLACKJACK_CLANG_TIDY_COMMAND)
    set_target_properties(blackjack_lib PROPERTIES CXX_CLANG_TIDY "${BLACKJACK_CLANG_TIDY_COMMAND}")
endif()
