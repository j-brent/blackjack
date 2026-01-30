include(FetchContent)

FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG        v5.0.0
)
FetchContent_MakeAvailable(ftxui)

add_executable(blackjack_tui src/main.cpp)

target_link_libraries(blackjack_tui PRIVATE
    blackjack_lib
    ftxui::screen
    ftxui::dom
    ftxui::component
)

target_compile_features(blackjack_tui PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(blackjack_tui PRIVATE /W4)
else()
    target_compile_options(blackjack_tui PRIVATE -Wall -Wextra -Wpedantic)
endif()
