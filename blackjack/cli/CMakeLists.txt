add_executable(blackjack_cli src/main.cpp)

target_link_libraries(blackjack_cli PRIVATE blackjack_lib)

target_compile_features(blackjack_cli PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(blackjack_cli PRIVATE /W4)
else()
    target_compile_options(blackjack_cli PRIVATE -Wall -Wextra -Wpedantic)
endif()
