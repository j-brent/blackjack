<!-- metadata
id: PLAN-001
type: project-plan
status: draft
author: A4-pm
created: 2026-01-30
updated: 2026-01-30
approved-by: pending
dependencies: [ARCH-001, SPEC-001, SPEC-002, SPEC-003, API-001, DATA-001, STD-001]
-->

# Project Plan: Blackjack — Phase 1

**Time budget**: 6 hours
**Deliverables**: `libblackjack` (static library), `blackjack-cli` (CLI frontend), `blackjack-tui` (FTXUI frontend), Catch2 unit tests

---

## 1. Epics

| Epic | Name | Description | Est. Hours |
|------|------|-------------|------------|
| E1 | Project Setup | CMake root, directory skeleton, FetchContent for Catch2 and FTXUI | 0.5 |
| E2 | Library Core | `types.hpp/cpp`, `card.hpp/cpp`, `deck.hpp/cpp`, `hand.hpp/cpp` | 1.0 |
| E3 | Game Engine | `game.hpp/cpp` — full state machine, all commands/queries | 1.5 |
| E4 | Library Tests | Catch2 tests for all library classes, covering all SPEC-001 ACs | 1.0 |
| E5 | CLI Frontend | `blackjack-cli` per SPEC-002 | 0.75 |
| E6 | FTXUI Frontend | `blackjack-tui` per SPEC-003 | 1.0 |
| E7 | Integration & Polish | Build verification, cross-platform check, final review | 0.25 |

---

## 2. Task Cards

### Epic E1: Project Setup

| ID | Title | Dependencies | AC References | Description | Deliverables |
|----|-------|-------------|---------------|-------------|-------------|
| TASK-E1-01 | Create directory skeleton | — | ARCH-001 §5 | Create the full monorepo directory structure as defined in the architecture: root `blackjack/` with `lib/include/blackjack/`, `lib/src/`, `cli/src/`, `ftxui/src/`, `tests/src/` subdirectories. Add placeholder `.gitkeep` or initial empty files. | All directories per ARCH-001 §5 |
| TASK-E1-02 | Root CMakeLists.txt | TASK-E1-01 | STD-001 §10 | Create the root `CMakeLists.txt` with `cmake_minimum_required(VERSION 3.20)`, project declaration (`blackjack`), C++20 standard, and `add_subdirectory` calls for `lib`, `cli`, `ftxui`, `tests`. | `blackjack/CMakeLists.txt` |
| TASK-E1-03 | Library CMakeLists.txt | TASK-E1-02 | STD-001 §10 | Create `lib/CMakeLists.txt` defining `blackjack_lib` as a static library. Set `target_include_directories` for `lib/include`, apply warning flags per STD-001 §10, set `cxx_std_20` via `target_compile_features`. | `blackjack/lib/CMakeLists.txt` |
| TASK-E1-04 | Tests CMakeLists.txt with Catch2 | TASK-E1-02 | STD-001 §8, §10 | Create `tests/CMakeLists.txt` with `FetchContent` for Catch2 v3 (pinned tag). Define `blackjack_tests` target linking `blackjack_lib` and Catch2. Register tests with CTest. | `blackjack/tests/CMakeLists.txt` |
| TASK-E1-05 | CLI CMakeLists.txt | TASK-E1-02 | STD-001 §10 | Create `cli/CMakeLists.txt` defining `blackjack_cli` executable linking `blackjack_lib`. Apply warning flags and C++20 standard. | `blackjack/cli/CMakeLists.txt` |
| TASK-E1-06 | FTXUI CMakeLists.txt | TASK-E1-02 | STD-001 §10, SPEC-003 §7 | Create `ftxui/CMakeLists.txt` with `FetchContent` for FTXUI (pinned tag). Define `blackjack_tui` executable linking `blackjack_lib` and FTXUI components (screen, dom, component). | `blackjack/ftxui/CMakeLists.txt` |
| TASK-E1-07 | Verify build skeleton compiles | TASK-E1-03, TASK-E1-04, TASK-E1-05, TASK-E1-06 | — | Add minimal stub files (empty `main()` in frontends, empty source in lib) and verify the entire project configures and builds with CMake. This is a smoke test for the build system. | Successful build output |

### Epic E2: Library Core

| ID | Title | Dependencies | AC References | Description | Deliverables |
|----|-------|-------------|---------------|-------------|-------------|
| TASK-E2-01 | Implement types.hpp and types.cpp | TASK-E1-03 | SPEC-001 FR-1, DATA-001 §1–§2 | Define all enums (`Suit`, `Rank`, `GameState`, `HandResult`, `ActionResult`, `PlayerAction`), the `Card` struct with `operator==`, and the `card_value()` free function. All per API-001 §1 and DATA-001. | `lib/include/blackjack/types.hpp`, `lib/src/types.cpp` |
| TASK-E2-02 | Implement hand.hpp and hand.cpp | TASK-E2-01 | SPEC-001 FR-3, AC-1 through AC-3, AC-13, DATA-001 §4 | Implement the `Hand` class: `add_card()`, `cards()`, `size()`, `value()` (with soft ace algorithm per DATA-001 §4.1), `is_soft()`, `is_bust()`, `is_natural_blackjack()`, `can_split()`. All methods must be `const`-correct and `[[nodiscard]]` where appropriate. | `lib/include/blackjack/hand.hpp`, `lib/src/hand.cpp` |
| TASK-E2-03 | Implement deck.hpp and deck.cpp | TASK-E2-01 | SPEC-001 FR-2, DATA-001 §3 | Implement the `Deck` class: constructor takes `std::mt19937&`, creates 52 cards, shuffles. `draw()` returns top card (last element, O(1)). `remaining()` and `is_empty()` queries. | `lib/include/blackjack/deck.hpp`, `lib/src/deck.cpp` |

### Epic E3: Game Engine

| ID | Title | Dependencies | AC References | Description | Deliverables |
|----|-------|-------------|---------------|-------------|-------------|
| TASK-E3-01 | Implement game.hpp and HandState struct | TASK-E2-02, TASK-E2-03 | API-001 §4, DATA-001 §5–§7 | Define the `Game` class public API in `game.hpp`: both constructors, all command methods (`deal`, `hit`, `stand`, `split`, `play_dealer`), all query methods. Define `HandState` struct. Define private `RoundState` and member variables in the implementation. | `lib/include/blackjack/game.hpp` |
| TASK-E3-02 | Implement deal() and initial state | TASK-E3-01 | SPEC-001 FR-4, FR-5, AC-4, AC-4b, AC-4c | Implement `deal()`: validate state (WaitingForDeal or RoundOver), create deck, shuffle, deal 2 cards each to player and dealer. Detect player natural blackjack and transition to DealerTurn if so, otherwise PlayerTurn. Return InvalidAction if called in wrong state. | `lib/src/game.cpp` (partial) |
| TASK-E3-03 | Implement hit() and bust logic | TASK-E3-02 | SPEC-001 FR-5, AC-5, AC-5b | Implement `hit()`: validate state (PlayerTurn), draw card to active hand. If hand busts, advance to next split hand or DealerTurn. Check `is_split_aces` flag to reject hits on split aces. Return InvalidAction if wrong state. | `lib/src/game.cpp` (partial) |
| TASK-E3-04 | Implement stand() and hand advancement | TASK-E3-02 | SPEC-001 FR-5, AC-6 | Implement `stand()`: validate state (PlayerTurn), mark active hand as stood, advance to next split hand or DealerTurn. Handle single-hand and split-hand cases. | `lib/src/game.cpp` (partial) |
| TASK-E3-05 | Implement split() | TASK-E3-03, TASK-E3-04 | SPEC-001 FR-5, AC-7 | Implement `split()`: validate state (PlayerTurn), check `can_split()` and `!has_split`. Split active hand into two hands, deal one card to each. If split aces, mark both `is_split_aces` and auto-stand both, advancing to DealerTurn. Return InvalidSplit or AlreadySplit on failure. | `lib/src/game.cpp` (partial) |
| TASK-E3-06 | Implement play_dealer() | TASK-E3-04 | SPEC-001 FR-6, AC-8 | Implement `play_dealer()`: validate state (DealerTurn). Dealer hits on soft 17, stands on hard 17+. Draw cards from deck until standing condition met. Transition to RoundOver and call result determination. | `lib/src/game.cpp` (partial) |
| TASK-E3-07 | Implement result determination | TASK-E3-06 | SPEC-001 FR-7, AC-9, AC-12, AC-12b | Implement private `determine_results()`: for each player hand, compare against dealer. Handle bust, natural blackjack, split-ace-21-is-not-blackjack, push, win/lose. Set `HandResult` on each `HandState`. | `lib/src/game.cpp` (partial) |
| TASK-E3-08 | Implement all query methods | TASK-E3-02 | SPEC-001 AC-10, AC-13, API-001 §7.1 | Implement `state()`, `player_hands()`, `active_hand_index()`, `dealer_hand()`, `is_dealer_hole_card_visible()`, `dealer_up_card()`, `available_actions()`. All must be safe in any state per API-001 §7.1 table. | `lib/src/game.cpp` (partial) |

### Epic E4: Library Tests

| ID | Title | Dependencies | AC References | Description | Deliverables |
|----|-------|-------------|---------------|-------------|-------------|
| TASK-E4-01 | Tests for types and card_value | TASK-E2-01, TASK-E1-04 | SPEC-001 FR-1 | Test `card_value()` for all ranks: Ace=1, 2-10=face, J/Q/K=10. Test Card equality operator. | `tests/src/test_card.cpp` |
| TASK-E4-02 | Tests for Hand | TASK-E2-02, TASK-E1-04 | SPEC-001 AC-1, AC-2, AC-3 | Test hand value calculation: simple hands, soft aces, multiple aces, bust detection, natural blackjack detection, `can_split()`. Cover all AC-1 through AC-3 scenarios plus edge cases (empty hand, single card). | `tests/src/test_hand.cpp` |
| TASK-E4-03 | Tests for Deck | TASK-E2-03, TASK-E1-04 | SPEC-001 FR-2 | Test deck has 52 cards, all unique. Test draw reduces count. Test deterministic seed produces same order. Test deck shuffles (different seed gives different order). | `tests/src/test_deck.cpp` |
| TASK-E4-04 | Tests for Game — deal and state transitions | TASK-E3-08, TASK-E1-04 | SPEC-001 AC-4, AC-4b, AC-4c, AC-10, AC-14 | Test `deal()` in valid states, invalid states. Test natural blackjack skips to DealerTurn. Test deterministic seed reproducibility. Test all invalid-action-in-wrong-state scenarios (AC-10). | `tests/src/test_game.cpp` (partial) |
| TASK-E4-05 | Tests for Game — hit, stand, split | TASK-E3-08, TASK-E1-04 | SPEC-001 AC-5, AC-5b, AC-6, AC-7 | Test hit adds card, bust detection, stand transitions. Test split with pairs, split with non-pairs (error), re-split (error), split aces (auto-stand). Test hand advancement after split. | `tests/src/test_game.cpp` (partial) |
| TASK-E4-06 | Tests for Game — dealer and results | TASK-E3-08, TASK-E1-04 | SPEC-001 AC-8, AC-9, AC-11, AC-12, AC-12b | Test dealer soft-17 hit, hard-17 stand. Test all result scenarios: win, lose, push, blackjack, player bust, dealer bust, both natural, split-ace-21-is-win-not-blackjack. Test full round flow (AC-11). | `tests/src/test_game.cpp` (partial) |
| TASK-E4-07 | Tests for Game — available_actions | TASK-E3-08, TASK-E1-04 | SPEC-001 AC-13 | Test `available_actions()` in all states: PlayerTurn with pair (Hit/Stand/Split), PlayerTurn without pair (Hit/Stand), DealerTurn (empty), RoundOver (empty), WaitingForDeal (empty). | `tests/src/test_game.cpp` (partial) |

### Epic E5: CLI Frontend

| ID | Title | Dependencies | AC References | Description | Deliverables |
|----|-------|-------------|---------------|-------------|-------------|
| TASK-E5-01 | Card and hand display formatting | TASK-E3-08, TASK-E1-05 | SPEC-002 §2.1, §2.2, §2.5, AC-CLI-9 | Implement helper functions to format cards as short text (`A♠`, `10♥`), format hands with values and soft indicator, format dealer hand with hidden hole card. Handle Unicode and ASCII fallback. | `cli/src/main.cpp` (display helpers) |
| TASK-E5-02 | Input parsing and command loop | TASK-E5-01 | SPEC-002 §3, AC-CLI-2, AC-CLI-5, AC-CLI-7 | Implement input reading: case-insensitive prefix matching for h/hit, s/stand, p/split, q/quit. Handle invalid input with error message and re-prompt. Handle EOF and bad stream state. | `cli/src/main.cpp` (input handling) |
| TASK-E5-03 | Main game loop and flow | TASK-E5-02 | SPEC-002 §4, AC-CLI-1, AC-CLI-3, AC-CLI-6, AC-CLI-8 | Implement the full game flow: welcome message, deal, display state, player loop, dealer turn with reveal, results display, play-again prompt. Handle natural blackjack shortcut, bust display, split flow with hand transition messages (§4.2). | `cli/src/main.cpp` (main function) |
| TASK-E5-04 | Error handling and edge cases | TASK-E5-03 | SPEC-002 §7 | Handle library API errors (display message, re-prompt). Handle unexpected exceptions (print error, exit non-zero). Handle stdin EOF (exit cleanly). Verify split display labels Hand 1/Hand 2 with active marker. | `cli/src/main.cpp` (error paths) |

### Epic E6: FTXUI Frontend

| ID | Title | Dependencies | AC References | Description | Deliverables |
|----|-------|-------------|---------------|-------------|-------------|
| TASK-E6-01 | Card rendering component | TASK-E3-08, TASK-E1-06 | SPEC-003 §3, AC-TUI-5 | Implement FTXUI Element functions to render face-up cards (7x5 bordered box with rank and suit), face-down cards (filled pattern), and suit coloring (red for Hearts/Diamonds, default for Spades/Clubs). Handle 10 as two-character rank. | `ftxui/src/main.cpp` (card rendering) |
| TASK-E6-02 | Layout panels | TASK-E6-01 | SPEC-003 §2, AC-TUI-1, AC-TUI-8 | Build the four-panel layout: title bar (centered "BLACKJACK"), dealer panel (hand + value, hole card hidden during PlayerTurn), player panel (hand(s) + value with soft indicator), controls panel, status bar. Use FTXUI vbox/hbox/border composition. | `ftxui/src/main.cpp` (layout) |
| TASK-E6-03 | Controls and keyboard input | TASK-E6-02 | SPEC-003 §4, §2.5, AC-TUI-6, AC-TUI-7 | Implement button components with keyboard shortcut labels. Handle H/S/P/N/Q/Esc key bindings. Show only valid actions during PlayerTurn, switch to New Game/Quit during RoundOver. Implement Tab/arrow navigation and Enter activation. | `ftxui/src/main.cpp` (controls) |
| TASK-E6-04 | Game state integration and flow | TASK-E6-03 | SPEC-003 §5, AC-TUI-2, AC-TUI-3, AC-TUI-9 | Wire up the Game object to the FTXUI event loop. On key press, issue commands to Game, trigger screen refresh. Handle auto-deal on launch. Implement dealer turn with ~500ms pause between draws. Handle bust visual (red text or "BUST" in status). | `ftxui/src/main.cpp` (game logic) |
| TASK-E6-05 | Split display and hand transition | TASK-E6-04 | SPEC-003 §5.5, AC-TUI-4 | Implement split hand display: two hands side-by-side (or stacked), active hand highlighted with brighter border. Status bar updates for hand transitions. Play hands sequentially with visual feedback. | `ftxui/src/main.cpp` (split handling) |
| TASK-E6-06 | Terminal size handling and polish | TASK-E6-04 | SPEC-003 §6, §7, AC-TUI-10 | Check terminal dimensions. Show warning if below 40x20. Ensure graceful resize handling. Verify clean terminal restoration on exit. Test on dark/light backgrounds for readability. | `ftxui/src/main.cpp` (polish) |

### Epic E7: Integration & Polish

| ID | Title | Dependencies | AC References | Description | Deliverables |
|----|-------|-------------|---------------|-------------|-------------|
| TASK-E7-01 | Full build verification | All E1–E6 tasks | STD-001 §1, NFR-5 | Build the entire project from clean with CMake. Verify zero warnings on MSVC (`/W4`). Run all tests. Verify both executables launch and function. | Clean build log, all tests passing |
| TASK-E7-02 | Cross-platform spot check | TASK-E7-01 | ARCH-001 §6, STD-001 §1 | If available, verify build on a second platform (Linux/macOS via GCC or Clang). Verify FTXUI renders correctly in at least two terminal emulators. Flag any platform-specific issues. | Build verification notes |
| TASK-E7-03 | Final review and cleanup | TASK-E7-01 | STD-001 §9, GIT-001 | Review all code for naming convention compliance (STD-001 §3), remove any TODO comments without tracking references, ensure include order compliance, verify `[[nodiscard]]` on all query/command methods. | Clean, standards-compliant codebase |

---

## 3. Implementation Order

The recommended implementation sequence respects dependencies, front-loads the critical path (library), and enables parallel work where possible.

### Phase A: Foundation (0:00–0:30)

| Order | Task | Rationale |
|-------|------|-----------|
| 1 | TASK-E1-01 | Directory skeleton — everything depends on this |
| 2 | TASK-E1-02 | Root CMake — needed before any sub-CMake |
| 3 | TASK-E1-03, TASK-E1-04, TASK-E1-05, TASK-E1-06 | Sub-CMake files — can be done in parallel |
| 4 | TASK-E1-07 | Smoke-test the build system |

### Phase B: Library Core (0:30–1:30)

| Order | Task | Rationale |
|-------|------|-----------|
| 5 | TASK-E2-01 | Types — everything in the library depends on this |
| 6 | TASK-E2-02, TASK-E2-03 | Hand and Deck — can be done in parallel, both depend only on types |
| 7 | TASK-E4-01 | Card/types tests — validate foundation immediately |
| 8 | TASK-E4-02, TASK-E4-03 | Hand and Deck tests — can be done in parallel with each other |

### Phase C: Game Engine (1:30–3:00)

| Order | Task | Rationale |
|-------|------|-----------|
| 9 | TASK-E3-01 | Game header and structs — defines the API |
| 10 | TASK-E3-02 | deal() — entry point for all game flow |
| 11 | TASK-E3-08 | Query methods — needed to test anything |
| 12 | TASK-E3-03, TASK-E3-04 | hit() and stand() — can be done in parallel |
| 13 | TASK-E3-05 | split() — depends on hit/stand for hand advancement logic |
| 14 | TASK-E3-06 | play_dealer() |
| 15 | TASK-E3-07 | Result determination |

### Phase D: Game Engine Tests (3:00–4:00)

| Order | Task | Rationale |
|-------|------|-----------|
| 16 | TASK-E4-04 | Deal and state transition tests |
| 17 | TASK-E4-05 | Hit/stand/split tests |
| 18 | TASK-E4-06 | Dealer and result tests |
| 19 | TASK-E4-07 | Available actions tests |

### Phase E: CLI Frontend (4:00–4:45)

| Order | Task | Rationale |
|-------|------|-----------|
| 20 | TASK-E5-01 | Display formatting |
| 21 | TASK-E5-02 | Input parsing |
| 22 | TASK-E5-03 | Main game loop |
| 23 | TASK-E5-04 | Error handling |

### Phase F: FTXUI Frontend (4:45–5:45)

| Order | Task | Rationale |
|-------|------|-----------|
| 24 | TASK-E6-01 | Card rendering component |
| 25 | TASK-E6-02 | Layout panels |
| 26 | TASK-E6-03 | Controls and keyboard input |
| 27 | TASK-E6-04 | Game state integration |
| 28 | TASK-E6-05 | Split display |
| 29 | TASK-E6-06 | Terminal size and polish |

### Phase G: Integration (5:45–6:00)

| Order | Task | Rationale |
|-------|------|-----------|
| 30 | TASK-E7-01 | Full build verification |
| 31 | TASK-E7-02 | Cross-platform spot check |
| 32 | TASK-E7-03 | Final review and cleanup |

### Parallelization Opportunities

The following tasks can be worked in parallel if multiple agents are available:

- **TASK-E1-03 / E1-04 / E1-05 / E1-06** — all sub-CMake files are independent
- **TASK-E2-02 / E2-03** — Hand and Deck only depend on types
- **TASK-E4-02 / E4-03** — Hand tests and Deck tests are independent
- **TASK-E3-03 / E3-04** — hit() and stand() share deal() as prerequisite but are independent of each other
- **TASK-E5-xx / E6-xx** — CLI and FTXUI frontends can be developed in parallel once E3 is complete
- **TASK-E4-04–E4-07** — Game tests can be written in parallel if the Game implementation is complete

---

## 4. Risk-Adjusted Priority

### Critical Path Tasks

These tasks block the most downstream work. Delays here cascade.

| Task | Why Critical |
|------|-------------|
| TASK-E1-01, E1-02, E1-03 | Every subsequent task depends on a working build system |
| TASK-E2-01 (types) | Every library class depends on types |
| TASK-E2-02 (Hand) | Game engine and both frontends depend on Hand |
| TASK-E3-01, E3-02 (Game header, deal) | Entry point for all game logic; frontends cannot start without Game |
| TASK-E3-08 (queries) | Frontends read state via queries; tests verify state via queries |

### High Risk Tasks

These are most likely to surface design issues or require iteration.

| Task | Risk | Mitigation |
|------|------|------------|
| TASK-E3-05 (split) | Split logic is the most complex state management: two hands, hand advancement, split aces auto-stand, re-split rejection | Write split tests (TASK-E4-05) immediately after implementation. Use deterministic seeds to control card deals. |
| TASK-E3-07 (results) | Many edge cases: player bust vs dealer bust, natural vs natural, split-ace-21 | Enumerate all AC-9/AC-12 scenarios as test cases first, then implement. |
| TASK-E6-01 (card rendering) | FTXUI box-drawing + color is new territory; visual bugs are hard to unit test | Prototype rendering in isolation first. Manual visual check before integrating. |
| TASK-E6-04 (FTXUI game integration) | Wiring async dealer turn (500ms pause) into FTXUI's event loop is non-trivial | Use FTXUI's `Loop` or timer mechanisms. If animations prove too complex, degrade to instant dealer play. |
| TASK-E1-06 (FTXUI FetchContent) | FTXUI version pinning and FetchContent can have build issues on MSVC | Test the FTXUI fetch and minimal compile early (TASK-E1-07). |

### Time-Sensitive / Cut Candidates

If the project runs over budget, cut these in order (last = first to cut):

| Priority | Task | What Gets Lost |
|----------|------|----------------|
| Cut last | TASK-E6-06 (terminal size polish) | Graceful small-terminal handling; low-impact |
| Cut 2nd | TASK-E6-05 (split display) | Split in FTXUI; game still works for non-split hands |
| Cut 3rd | TASK-E7-02 (cross-platform check) | Deferred to post-Phase 1; not a deliverable blocker |
| Cut 4th | TASK-E6-04 dealer animation | Dealer plays instantly instead of animated; still functional |
| Never cut | E1–E4, E5 | Library, tests, and CLI are the core deliverables |

---

## 5. Dependency Graph

```
TASK-E1-01  (directory skeleton)
    │
    ▼
TASK-E1-02  (root CMake)
    │
    ├──────────────┬──────────────┬──────────────┐
    ▼              ▼              ▼              ▼
TASK-E1-03     TASK-E1-04     TASK-E1-05     TASK-E1-06
(lib CMake)    (test CMake)   (CLI CMake)    (FTXUI CMake)
    │              │              │              │
    └──────────────┴──────────────┴──────────────┘
                   │
                   ▼
            TASK-E1-07  (build smoke test)
                   │
                   ▼
            TASK-E2-01  (types) ◄──── CRITICAL
                   │
            ┌──────┴──────┐
            ▼             ▼
      TASK-E2-02    TASK-E2-03
       (Hand)        (Deck)
            │             │
            │      ┌──────┘
            ▼      ▼
      TASK-E3-01  (Game header)
            │
            ▼
      TASK-E3-02  (deal) ◄──── CRITICAL
            │
            ├────────────────┐
            ▼                ▼
      TASK-E3-08       TASK-E3-03, E3-04
      (queries)        (hit, stand)
            │                │
            │                ▼
            │          TASK-E3-05  (split) ◄──── HIGH RISK
            │                │
            │                ▼
            │          TASK-E3-06  (play_dealer)
            │                │
            │                ▼
            │          TASK-E3-07  (results) ◄──── HIGH RISK
            │                │
            └────────┬───────┘
                     │
          ┌──────────┼──────────────┐
          ▼          ▼              ▼
    TASK-E4-01   TASK-E5-01    TASK-E6-01
    through      through       through
    TASK-E4-07   TASK-E5-04    TASK-E6-06
    (Tests)      (CLI)         (FTXUI)
          │          │              │
          └──────────┴──────────────┘
                     │
                     ▼
              TASK-E7-01  (full build)
                     │
              ┌──────┴──────┐
              ▼             ▼
        TASK-E7-02    TASK-E7-03
        (xplat)       (review)
```

### Critical Path (longest sequential chain)

```
E1-01 → E1-02 → E1-03 → E2-01 → E2-02 → E3-01 → E3-02 → E3-03 → E3-05 → E3-06 → E3-07 → E4-06 → E7-01
```

This chain is approximately 5 hours of work. The frontends (E5, E6) branch off in parallel after E3 completes but are not on the critical path unless the library finishes early.

---

## 6. Git Branch Strategy

Per GIT-001, the implementation maps to these branches:

| Branch | Tasks | Merge Order |
|--------|-------|-------------|
| `feature/project-setup` | TASK-E1-01 through E1-07 | 1st — merge to main |
| `feature/game-engine` | TASK-E2-01 through E3-08 | 2nd — merge to main |
| `feature/library-tests` | TASK-E4-01 through E4-07 | 3rd — merge to main |
| `feature/cli-frontend` | TASK-E5-01 through E5-04 | 4th — merge to main |
| `feature/ftxui-frontend` | TASK-E6-01 through E6-06 | 5th — merge to main |

Integration tasks (E7) are performed on `main` after all feature branches merge.

---

## 7. Acceptance Criteria Traceability Matrix

Every AC from SPEC-001, SPEC-002, and SPEC-003 must be validated by at least one task.

### SPEC-001 (Library)

| AC | Validated By |
|----|-------------|
| AC-1 (card values) | TASK-E4-02 |
| AC-2 (natural blackjack) | TASK-E4-02 |
| AC-3 (bust detection) | TASK-E4-02 |
| AC-4 (deal normal) | TASK-E4-04 |
| AC-4b (deal natural) | TASK-E4-04 |
| AC-4c (deal invalid state) | TASK-E4-04 |
| AC-5 (hit no bust) | TASK-E4-05 |
| AC-5b (hit bust) | TASK-E4-05 |
| AC-6 (stand) | TASK-E4-05 |
| AC-7 (split) | TASK-E4-05 |
| AC-8 (dealer logic) | TASK-E4-06 |
| AC-9 (result determination) | TASK-E4-06 |
| AC-10 (state integrity) | TASK-E4-04 |
| AC-11 (full round flow) | TASK-E4-06 |
| AC-12 (split aces 21) | TASK-E4-06 |
| AC-12b (dealer natural) | TASK-E4-06 |
| AC-13 (available actions) | TASK-E4-07 |
| AC-14 (deterministic seed) | TASK-E4-04 |

### SPEC-002 (CLI)

| AC | Validated By |
|----|-------------|
| AC-CLI-1 (basic round) | TASK-E5-03 |
| AC-CLI-2 (hit) | TASK-E5-02, TASK-E5-03 |
| AC-CLI-3 (stand/dealer) | TASK-E5-03 |
| AC-CLI-4 (split display) | TASK-E5-03 |
| AC-CLI-5 (invalid input) | TASK-E5-02 |
| AC-CLI-6 (play again) | TASK-E5-03 |
| AC-CLI-7 (quit) | TASK-E5-02 |
| AC-CLI-8 (bust display) | TASK-E5-03 |
| AC-CLI-9 (soft ace) | TASK-E5-01 |

### SPEC-003 (FTXUI)

| AC | Validated By |
|----|-------------|
| AC-TUI-1 (launch/display) | TASK-E6-02, TASK-E6-04 |
| AC-TUI-2 (hit) | TASK-E6-04 |
| AC-TUI-3 (stand/dealer) | TASK-E6-04 |
| AC-TUI-4 (split display) | TASK-E6-05 |
| AC-TUI-5 (card rendering) | TASK-E6-01 |
| AC-TUI-6 (keyboard nav) | TASK-E6-03 |
| AC-TUI-7 (round over controls) | TASK-E6-03 |
| AC-TUI-8 (hole card hidden) | TASK-E6-02 |
| AC-TUI-9 (bust visual) | TASK-E6-04 |
| AC-TUI-10 (terminal size) | TASK-E6-06 |
