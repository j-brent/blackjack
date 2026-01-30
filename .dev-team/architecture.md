<!-- metadata
id: ARCH-001
type: architecture
status: draft
author: A2-architect
created: 2026-01-30
updated: 2026-01-30
approved-by: pending
dependencies: [VISION-001]
-->

# Architecture: Blackjack — C++ Frontend Exploration

## 1. System Context

The system is a monorepo containing one shared backend library and multiple independent frontend executables. Each frontend links against the same library and drives its own game loop. The library has zero knowledge of any frontend.

```
┌─────────────────────────────────────────────────────┐
│                    Monorepo                          │
│                                                     │
│  ┌──────────────────────┐                           │
│  │  libblackjack        │  Static library (.a/.lib) │
│  │  (Game Engine)       │  Pure C++ — no I/O        │
│  │                      │                           │
│  │  - Card, Deck, Hand  │                           │
│  │  - Game state machine│                           │
│  │  - Rules evaluation  │                           │
│  └──────────┬───────────┘                           │
│             │ links against                         │
│       ┌─────┴──────┐                                │
│       │            │                                │
│  ┌────▼────┐ ┌─────▼──────┐                         │
│  │  CLI    │ │   FTXUI    │    (Future: WASM,       │
│  │ Frontend│ │  Frontend  │     Python, Qt, Mobile) │
│  │  (exe)  │ │   (exe)    │                         │
│  └─────────┘ └────────────┘                         │
│                                                     │
└─────────────────────────────────────────────────────┘
```

The library exposes a C++ API. Frontends are consumers — they create a game, query state, issue commands, and render based on what they read. The library never calls back into the frontend. There are no callbacks, no observers, no event systems. The frontend polls state after each command.

## 2. Component Design

### 2.1 Backend Library: `libblackjack`

**Purpose**: Implement all blackjack game rules as a stateful, deterministic (given a deck order) game engine.

**Responsibilities**:
- Card, deck, and hand representation
- Deck shuffling (using a caller-provided or default random engine)
- Hand value calculation with soft ace logic
- Game state machine: betting phase is skipped; flow is deal → player turns → dealer turn → resolution
- Split logic: splitting a pair into two hands, each played independently
- Dealer logic: hit on soft 17, stand on hard 17+
- Win/loss/push/natural blackjack determination per hand
- Exposing all game state for read-only inspection by frontends

**Constraints**:
- No `#include <iostream>`, no `stdin`/`stdout`, no file I/O
- No UI logic, no string formatting for display
- No external dependencies — pure C++ standard library only
- Thread-safe if the caller serializes calls (no internal threading)
- Header-only public API or minimal headers + static lib

**Design principle**: The library is a state machine. The frontend sends commands (`deal`, `hit`, `stand`, `split`) and reads state (`get_player_hands`, `get_dealer_hand`, `get_game_state`, `get_results`). Invalid commands for the current state return an error — they do not crash, assert, or throw.

### 2.2 CLI Frontend

**Purpose**: Minimal text-based frontend proving the library API works.

**Responsibilities**:
- Print hands and values to stdout
- Read player commands from stdin
- Drive the game loop: deal → player actions → dealer reveal → result display → play again
- Hide dealer's hole card during player's turn

**Technology**: Pure C++ with standard I/O. No dependencies beyond the library.

### 2.3 FTXUI Frontend

**Purpose**: Rich terminal GUI frontend demonstrating the library works with a real UI framework.

**Responsibilities**:
- Render cards using Unicode box-drawing characters
- Color-code suits (red for hearts/diamonds, white/default for spades/clubs)
- Structured layout: dealer panel (top), player panel(s) (middle), controls (bottom), status bar
- Keyboard-driven interaction with labeled buttons for Hit, Stand, Split
- Animate or visually distinguish card reveals
- Show hand values, game results, play-again prompt

**Technology**: C++ with FTXUI library. FTXUI is fetched via CMake's FetchContent.

## 3. Technology Stack

| Component       | Technology                        | Rationale                                   |
|-----------------|-----------------------------------|---------------------------------------------|
| Language        | C++20                             | `std::span`, `std::format` (where available), concepts, designated initializers |
| Build system    | CMake 3.20+                       | Industry standard for C++, good FTXUI support |
| Backend library | Pure C++ standard library         | Zero dependencies = trivial to embed anywhere |
| CLI frontend    | C++ standard I/O                  | Simplest possible consumer                  |
| FTXUI frontend  | FTXUI (FetchContent)              | Cross-platform terminal GUI, active project |
| Testing         | Catch2 or GoogleTest (FetchContent)| Backend library tested independently        |
| Random          | `<random>` (std::mt19937)         | Seedable for deterministic testing          |

**C++20 rationale**: C++20 is well-supported by GCC 10+, Clang 10+, and MSVC 19.29+. It provides `std::span` for non-owning views, `enum class` with `using enum` declarations, and designated initializers — all useful for a clean API. If a compiler lacks `std::format`, we fall back to manual formatting in frontends (the library doesn't format anything).

## 4. Key Architectural Decisions

### ADR-1: C++ API, not C API

**Decision**: The library exposes a C++ API (classes, enums, `std::vector`, `std::optional`).

**Rationale**: Phase 1 consumers are C++. A C API would add indirection and complexity now for uncertain future benefit. If Phase 2 (WASM/Emscripten) or Phase 3 (Python bindings) need a C API or a flattened API, we can add a thin C wrapper around the C++ API at that time without changing the library internals. The C++ API is the source of truth; a C API would be a facade.

**Risk**: If the C++ API uses types that don't cross language boundaries well (e.g., `std::vector`, `std::string`), bindings may need adapters. Mitigated by keeping the API surface small and using simple types at the boundary.

### ADR-2: Frontend drives the loop, library is passive

**Decision**: The library has no game loop, no main loop, no update/tick function. The frontend calls commands and reads state.

**Rationale**: Different frontends have radically different event models. CLI uses blocking stdin reads. FTXUI uses an event loop. WASM uses browser requestAnimationFrame. Embedding a loop in the library would force every frontend to adapt to it. Instead, the library is passive — the frontend is in control.

**Implication**: The library's `Game` class is a pure state machine. Calling `hit()` transitions state and returns. The frontend then queries state and renders.

### ADR-3: Error handling via return values, not exceptions

**Decision**: Commands return a result type (success or error enum). No exceptions thrown from the public API.

**Rationale**: Exceptions complicate WASM compilation (Emscripten exception support is costly), Python bindings (exception translation is fragile), and make the API harder to reason about. A simple enum error return is universal.

### ADR-4: Randomness is injectable

**Decision**: The `Game` class accepts an optional random seed. By default it uses a random device seed. Tests can pass a fixed seed for deterministic behavior.

**Rationale**: Game logic must be testable. Non-deterministic shuffling makes tests flaky. A seedable RNG solves this.

### ADR-5: Single-deck, reshuffle each game

**Decision**: Each new game uses a freshly shuffled single 52-card deck.

**Rationale**: Simplicity. Card counting and multi-deck shoe management are out of scope per the vision statement.

## 5. Build Structure

```
blackjack/
├── CMakeLists.txt              # Root CMake — defines project, adds subdirectories
├── lib/
│   ├── CMakeLists.txt          # Builds libblackjack as a static library
│   ├── include/
│   │   └── blackjack/
│   │       ├── card.hpp        # Card, Suit, Rank
│   │       ├── deck.hpp        # Deck
│   │       ├── hand.hpp        # Hand, value calculation
│   │       ├── game.hpp        # Game state machine (public API)
│   │       └── types.hpp       # Enums, result types, shared types
│   └── src/
│       ├── card.cpp
│       ├── deck.cpp
│       ├── hand.cpp
│       └── game.cpp
├── cli/
│   ├── CMakeLists.txt          # Builds blackjack-cli executable
│   └── src/
│       └── main.cpp
├── ftxui/
│   ├── CMakeLists.txt          # Builds blackjack-tui executable, fetches FTXUI
│   └── src/
│       └── main.cpp
└── tests/
    ├── CMakeLists.txt          # Builds and runs tests for libblackjack
    └── src/
        ├── test_card.cpp
        ├── test_deck.cpp
        ├── test_hand.cpp
        └── test_game.cpp
```

**Build targets**:
- `blackjack_lib` — static library, no dependencies
- `blackjack_cli` — executable, links `blackjack_lib`
- `blackjack_tui` — executable, links `blackjack_lib` + FTXUI
- `blackjack_tests` — test executable, links `blackjack_lib` + test framework

**FTXUI dependency**: Fetched via `FetchContent` in `ftxui/CMakeLists.txt`. This avoids requiring FTXUI to be system-installed and pins the version.

## 6. Deployment

Each frontend compiles to a standalone executable. No installers, no packaging. Run from the build directory or copy the binary.

- `blackjack-cli` — works anywhere with a terminal
- `blackjack-tui` — works anywhere with a terminal that supports ANSI/Unicode (most modern terminals)

## 7. Future Phase Considerations

The architecture is designed so future phases add new directories and CMake targets without touching the library:

| Phase | Addition | Integration Strategy |
|-------|----------|---------------------|
| Phase 2: WASM | `wasm/` directory | Emscripten compiles `blackjack_lib` to WASM, JS frontend calls exported C functions via a thin C wrapper |
| Phase 3: Python | `python/` directory | pybind11/nanobind wraps the C++ API, Python frontend imports the module |
| Phase 4: Qt | `qt/` directory | Qt app links `blackjack_lib` directly (both are C++) |
| Phase 5: Mobile | TBD | Likely wraps via C API or uses Qt for mobile |

The library's zero-dependency, no-I/O, passive design makes all of these integrations feasible without library changes.
