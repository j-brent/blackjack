cmake_minimum_required(VERSION 3.20)
project(blackjack_web)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library source files (included directly since this is a separate CMake invocation)
set(LIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/src/types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/src/hand.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/src/deck.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/src/game.cpp
)

# Bindings source file
set(BINDINGS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bindings.cpp
)

# Create the WASM target
add_executable(blackjack_web ${LIB_SOURCES} ${BINDINGS_SOURCES})

# Include directories
target_include_directories(blackjack_web PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/include
)

# Emscripten-specific link flags
set(EMSCRIPTEN_LINK_FLAGS
    --bind
    -s MODULARIZE=1
    -s EXPORT_ES6=1
    -s ENVIRONMENT=web
    -s NO_EXIT_RUNTIME=1
    -s ALLOW_MEMORY_GROWTH=1
    -s DISABLE_EXCEPTION_CATCHING=1
    -O2
    -s WASM=1
)

# Apply link flags
target_link_options(blackjack_web PRIVATE ${EMSCRIPTEN_LINK_FLAGS})

# Set output name (generates blackjack.js and blackjack.wasm)
set_target_properties(blackjack_web PROPERTIES OUTPUT_NAME "blackjack")
