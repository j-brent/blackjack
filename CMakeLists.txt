cmake_minimum_required(VERSION 3.20)
project(blackjack LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

option(BLACKJACK_CLANG_TIDY "Enable clang-tidy static analysis during build" OFF)
if(BLACKJACK_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        set(BLACKJACK_CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" CACHE STRING "")
        message(STATUS "clang-tidy enabled: ${CLANG_TIDY_EXE}")
    else()
        message(FATAL_ERROR "BLACKJACK_CLANG_TIDY is ON but clang-tidy was not found")
    endif()
endif()

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

add_subdirectory(blackjack)
add_subdirectory(apps/cli)
add_subdirectory(apps/ftxui)
add_subdirectory(tests)
