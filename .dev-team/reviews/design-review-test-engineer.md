<!-- metadata
id: DREV-TEST
type: design-review
status: draft
author: A11-test-engineer
created: 2026-01-30
updated: 2026-01-30
approved-by: pending
dependencies: [ARCH-001, SPEC-001, SPEC-002, SPEC-003, API-001, DATA-001]
-->

# Design Review: Test Engineer

**Reviewer**: A11-test-engineer
**Artifacts Reviewed**: ARCH-001, SPEC-001, SPEC-002, SPEC-003, API-001, DATA-001
**Review Date**: 2026-01-30

## Summary

The architecture and specifications demonstrate strong testability fundamentals with seedable RNG, clear state machines, and separation of concerns. However, several acceptance criteria are ambiguous or untestable as written, and key edge cases are missing. The CLI frontend is testable with integration tests, but FTXUI frontend lacks testability guidance. Several blockers must be resolved before implementation.

## Findings

### FINDING-001: Missing Quantifiable Assertions in AC-4 (Dealing)

| Property | Value |
|----------|-------|
| **Severity** | BLOCKER |
| **Category** | testability |
| **Artifact** | SPEC-001 |
| **Section** | AC-4 |

**Issue**: AC-4 states "And the game state is PLAYER_TURN (unless natural blackjack)" but does not specify the acceptance criteria for the natural blackjack case. How do we test the "unless" branch?

**Recommendation**: Split AC-4 into two separate scenarios:
1. `Given a game deals non-natural hands, Then state is PLAYER_TURN`
2. `Given a game deals a natural blackjack to the player, Then state is DEALER_TURN and player hand is natural`

**Rationale**: Parenthetical exceptions are not testable. Each state transition path must have an explicit acceptance criterion with a deterministic setup (using seed) or clear pass/fail condition.

---

### FINDING-002: Non-Deterministic AC-5 (Hit with specific card)

| Property | Value |
|----------|-------|
| **Severity** | BLOCKER |
| **Category** | testability |
| **Artifact** | SPEC-001 |
| **Section** | AC-5 |

**Issue**: AC-5 states "When I call hit() and the drawn card is a 9". This is only testable with a deterministic seed, but the AC does not specify a seed or deck order. Without a specified seed, this test will be flaky.

**Recommendation**: Either:
- Specify a seed value and document the expected deck order for this test, or
- Rewrite as: "Given a game with a seeded deck where the next card is a 9, When I call hit(), Then the hand value increases by 9 and the hand is bust if the value exceeds 21."

**Rationale**: All acceptance criteria must be reproducible. If a test depends on a specific card being drawn, the seed and deck order must be specified or the criteria must be written to work with any drawn card.

---

### FINDING-003: Ambiguous "Automatic Dealer Turn" in AC-5 and AC-11

| Property | Value |
|----------|-------|
| **Severity** | BLOCKER |
| **Category** | testability |
| **Artifact** | SPEC-001 |
| **Section** | AC-5, AC-11, API-001 Section 5.1 |

**Issue**: AC-5 says "And the state advances (to next hand or DEALER_TURN)" and AC-11 says "Then the dealer plays automatically". However, API-001 Section 5.1 shows the frontend must explicitly call `play_dealer()`. The word "automatically" is ambiguous.

**Recommendation**: Clarify in SPEC-001:
- If the state transitions to DEALER_TURN, does `deal()` or `stand()` internally call dealer logic, or must the frontend call `play_dealer()`?
- If the frontend must call it, update all ACs to say "And the state transitions to DEALER_TURN" (remove "automatically plays").
- Add an AC: "Given state is DEALER_TURN, When the frontend calls play_dealer(), Then dealer draws cards per rules and state transitions to ROUND_OVER."

**Rationale**: The state machine contract must be unambiguous for testing. If dealer play is automatic, we need a hook to observe intermediate dealer draws for testing. If it requires an explicit call, all ACs must reflect this.

---

### FINDING-004: Missing Edge Case - Split Aces Receiving Blackjack

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | testability |
| **Artifact** | SPEC-001 |
| **Section** | AC-7, AC-9 |

**Issue**: AC-7 states that split aces receive one card each and are immediately stood. If that card is a 10-value, the hand totals 21. Is this considered a natural blackjack (HandResult::Blackjack) or a regular win (HandResult::Win)?

**Recommendation**: Add an AC:
```
Given a player splits aces and one hand receives a 10-value card (21 total)
When results are determined
Then the result is Win (not Blackjack, because it was achieved via split, not initial deal)
```

**Rationale**: The definition of "natural blackjack" is "exactly 2 cards totaling 21 as initial deal". A split hand is not an initial deal, so logic must differentiate. This edge case is missing and will cause ambiguity during implementation.

---

### FINDING-005: Missing Edge Case - Dealer Natural vs Player Non-Natural

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | testability |
| **Artifact** | SPEC-001 |
| **Section** | AC-9 |

**Issue**: AC-9 covers player natural vs dealer natural, and player natural vs dealer non-natural. It does NOT cover dealer natural vs player non-natural (player has 20, dealer has natural blackjack).

**Recommendation**: Add AC:
```
Given the player has value 20 (non-natural) and the dealer has natural blackjack
When results are determined
Then the player's result is Lose
```

**Rationale**: Result determination must cover all combinations of player/dealer naturals and non-naturals. Missing test case.

---

### FINDING-006: Missing Edge Case - Empty Deck Scenario

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | testability |
| **Artifact** | SPEC-001, DATA-001 |
| **Section** | FR-2, Deck class |

**Issue**: SPEC-001 FR-2 states "Drawing from an empty deck is a programming error (the library must ensure this never happens)". However, no AC validates that the library handles this gracefully. If a player hits 20 times in a row (or splits and hits many times), the deck could run out.

**Recommendation**: Add an AC or design clarification:
- Either: guarantee that a single-deck game cannot exhaust 52 cards (max 2 initial + max ~10 hits per hand × 2 hands + dealer draws ~10 = ~34 cards max), or
- Add defensive logic: if deck is exhausted, the game transitions to ROUND_OVER with a special error state, or reshuffles.

Include a test: "Given a game where many hits occur, When the deck has < 5 cards remaining, Then the library does not crash and either reshuffles or ends the round gracefully."

**Rationale**: Absence of error handling for edge cases is a testability and robustness gap. The spec assumes this "never happens" but does not prove it or handle it.

---

### FINDING-007: Missing Edge Case - Player Busts Both Hands in Split

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | testability |
| **Artifact** | SPEC-001 |
| **Section** | AC-9 |

**Issue**: AC-9 does not cover the scenario where a player splits, busts both hands, and the dealer also busts. The rule "player bust always loses" suggests both hands lose, but there is no explicit AC for split-hand bust scenarios.

**Recommendation**: Add AC:
```
Given the player splits into two hands, and both hands bust
When results are determined
Then both hands have result Lose (regardless of dealer outcome)
```

**Rationale**: Completeness. Split hands must have explicit test coverage for all result scenarios.

---

### FINDING-008: Untestable AC-CLI-3 "Dealer Draws Per Rules"

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | testability |
| **Artifact** | SPEC-002 |
| **Section** | AC-CLI-3 |

**Issue**: AC-CLI-3 states "And the dealer draws cards per rules (hit soft 17)" but does not specify how to verify this in a CLI integration test. The CLI output is text on stdout. How do we assert "drew cards per rules" vs "drew wrong cards"?

**Recommendation**: Make AC-CLI-3 more specific:
```
Given the player stands with hand value 18
And the dealer has [Ace of Hearts, 6 of Clubs] (soft 17)
When the dealer plays
Then the CLI output shows the dealer drawing at least one card
And the final dealer hand value is >= 17
And the result is determined correctly
```

Alternatively, state that AC-CLI-3 relies on SPEC-001 AC-8 being tested at the library level (unit tests), and the CLI integration test only verifies that `play_dealer()` is called and results are displayed.

**Rationale**: Integration tests must have observable outputs. Saying "per rules" is not an assertion. Either specify exact expected CLI output (fragile) or state that rule correctness is tested at the library layer.

---

### FINDING-009: CLI Invalid Input Does Not Specify Re-Prompting Behavior

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | testability |
| **Artifact** | SPEC-002 |
| **Section** | AC-CLI-5 |

**Issue**: AC-CLI-5 says "And the player is prompted again". How many times? If the user types invalid input 1000 times, does the CLI loop forever? This is an infinite loop risk and untestable without a clear loop exit condition.

**Recommendation**: Add to SPEC-002 Section 3.3:
- After 3 consecutive invalid inputs, print "Too many invalid attempts. Exiting." and quit, OR
- Loop indefinitely (acceptable for a CLI, but should be documented), OR
- Document that Ctrl+C is the only exit from invalid input loop (already mentioned in NFR, so this is likely acceptable).

Update AC-CLI-5: "And the player is prompted again (loop continues until valid input or Ctrl+C)."

**Rationale**: Infinite loops without documented exit conditions are not testable. Clarify the intended behavior.

---

### FINDING-010: FTXUI Frontend Lacks Testability Guidance

| Property | Value |
|----------|-------|
| **Severity** | BLOCKER |
| **Category** | testability |
| **Artifact** | SPEC-003 |
| **Section** | Entire spec, Section 8 (Acceptance Criteria) |

**Issue**: All FTXUI acceptance criteria (AC-TUI-1 through AC-TUI-10) are visual/UI assertions: "the dealer panel shows", "the controls panel shows active Hit and Stand buttons", "the card appears as a bordered box". These are not automatable with standard test frameworks. The spec does not address how these will be tested.

**Recommendation**: Add to SPEC-003:
- **Section 9: Testing Strategy**
  - "The FTXUI frontend will be tested manually against the acceptance criteria. Automated testing is limited to verifying that the frontend compiles and links against libblackjack without errors."
  - "A smoke test will verify the application launches and responds to one keyboard input without crashing."
  - "Visual ACs (card rendering, layout, colors) will be verified through manual review."

Alternatively, if automated testing is required:
- Use FTXUI's programmatic event injection (if available) to simulate keypresses
- Capture rendered screen buffer as text and use snapshot testing
- Add AC: "Given the TUI application in state X, When rendered, Then the screen buffer matches the expected snapshot."

**Rationale**: The spec implies full test coverage, but UI testing is fundamentally different from API/CLI testing. Without a testing strategy, implementers will waste time trying to automate the untestable. Clarify manual vs automated scope.

---

### FINDING-011: Missing Acceptance Criteria for Split Re-Split Validation

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | testability |
| **Artifact** | SPEC-001 |
| **Section** | AC-7 |

**Issue**: AC-7 includes a scenario: "Given a game where a split has already occurred this round, When I call split() on a new pair in a split hand, Then the action result is an error (re-split not allowed)." However, this assumes the split hand is dealt a pair. The AC does not specify how to set up this scenario deterministically.

**Recommendation**: Clarify the AC:
```
Given a player splits [8♠, 8♣] into two hands
And the first split hand is dealt [8♦] (now has [8♠, 8♦], a pair)
When the player attempts to split the first hand again
Then the action result is AlreadySplit
And the game state is unchanged
```

Specify a seed that produces this scenario, or use a mock deck for the test.

**Rationale**: The scenario is valid but requires deterministic setup. Acceptance criteria must be executable.

---

### FINDING-012: API Query Methods Do Not Specify Behavior in Invalid States

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | testability |
| **Artifact** | API-001 |
| **Section** | Section 7 (Error Handling Contract) |

**Issue**: API-001 Section 7 states "Query methods always return valid data. If called in an unexpected state, they return empty/default data — never crash." However, the API contract does not specify what "empty/default data" means for each query method.

Examples:
- What does `player_hands()` return in WaitingForDeal state? Empty span, or span with 1 default HandState?
- What does `dealer_hand()` return when no round is in progress? An empty Hand (0 cards), or undefined behavior?
- What does `active_hand_index()` return when not in PlayerTurn? 0, or SIZE_MAX?

**Recommendation**: Add to API-001 a table:

| Method | State | Return Value |
|--------|-------|--------------|
| `player_hands()` | WaitingForDeal / RoundOver | Empty span |
| `dealer_hand()` | WaitingForDeal / RoundOver | Empty Hand (size() == 0) |
| `active_hand_index()` | Not PlayerTurn | 0 (or undefined; document if undefined) |
| `available_actions()` | Not PlayerTurn | Empty vector |
| `dealer_up_card()` | No round in progress | std::nullopt |

**Rationale**: Testability requires unambiguous contracts. If a query method's behavior is "valid but unspecified" in certain states, tests cannot assert correctness. Specify the contract explicitly.

---

### FINDING-013: Missing Test Coverage for `is_dealer_hole_card_visible()`

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | testability |
| **Artifact** | SPEC-001, API-001 |
| **Section** | N/A (missing AC) |

**Issue**: The API exposes `is_dealer_hole_card_visible()` which is critical for correct rendering, but SPEC-001 has no acceptance criteria testing this method. It is implied by frontend specs (CLI AC-CLI-3, TUI AC-TUI-8) but never tested at the library API level.

**Recommendation**: Add to SPEC-001 AC-10 or create a new AC section:
```
AC-12: Dealer Hole Card Visibility

Given a game in PLAYER_TURN state
When I query is_dealer_hole_card_visible()
Then the result is false

Given a game in DEALER_TURN state
When I query is_dealer_hole_card_visible()
Then the result is true

Given a game in ROUND_OVER state
When I query is_dealer_hole_card_visible()
Then the result is true
```

**Rationale**: All public API methods must have acceptance criteria. Missing test coverage for a boolean flag could cause frontend bugs.

---

### FINDING-014: No Acceptance Criteria for `available_actions()`

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | testability |
| **Artifact** | SPEC-001, API-001 |
| **Section** | N/A (missing AC) |

**Issue**: `available_actions()` is a key API method used by frontends to determine which buttons/options to show. It is not tested in SPEC-001. The logic for this method is non-trivial:
- PlayerTurn with no split: [Hit, Stand, Split (if can_split)]
- PlayerTurn after split: [Hit, Stand] (no Split)
- PlayerTurn with split aces: [] (auto-stood)
- Other states: []

**Recommendation**: Add AC-13 to SPEC-001:
```
AC-13: Available Actions Query

Given a game in PLAYER_TURN with a splittable hand [8♠, 8♣]
When I query available_actions()
Then the result includes Hit, Stand, and Split

Given a game in PLAYER_TURN after a split has occurred
And the active hand is not split aces
When I query available_actions()
Then the result includes Hit and Stand (but not Split)

Given a game in DEALER_TURN
When I query available_actions()
Then the result is an empty vector
```

**Rationale**: This method drives frontend UI. Incorrect logic will cause buttons to appear/disappear incorrectly. Must have explicit test coverage.

---

### FINDING-015: CLI and FTXUI Quit Behavior Not Testable

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | testability |
| **Artifact** | SPEC-002, SPEC-003 |
| **Section** | AC-CLI-7, AC-TUI-7 |

**Issue**: AC-CLI-7 says "When the player types 'q', Then the program exits cleanly." AC-TUI-7 says "pressing Q exits the application." How do we test "exits cleanly" in an automated test?

**Recommendation**: Clarify acceptance criteria:
- Define "exits cleanly" as: process terminates with exit code 0, no error messages on stderr, no resource leaks (as verified by valgrind or sanitizers).
- Update AC: "Then the program exits with code 0 and no errors are logged."

For integration tests, use a shell script or test harness:
```bash
echo "q" | ./blackjack-cli
echo $?  # Should be 0
```

**Rationale**: "Exits cleanly" is subjective without a definition. Specify exit code and absence of errors.

---

### FINDING-016: No Regression Test Suite Specified

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | testability |
| **Artifact** | ARCH-001 |
| **Section** | Section 3 (Technology Stack), Section 5 (Build Structure) |

**Issue**: ARCH-001 specifies "Testing: Catch2 or GoogleTest (FetchContent)" and a `tests/` directory, but does not specify:
- What percentage of code must be covered by tests?
- What categories of tests are required (unit, integration, smoke)?
- Are there CI/CD gates (all tests must pass before merge)?

**Recommendation**: Add to ARCH-001 Section 3 or create a new Testing Strategy document:
```
## Testing Requirements
- **Unit tests**: All library classes (Card, Deck, Hand, Game) must have unit tests covering all public methods and all acceptance criteria in SPEC-001.
- **Integration tests**: CLI frontend must have integration tests verifying end-to-end flows (AC-CLI-1 through AC-CLI-8). Tests use scripted stdin input and assert on stdout output.
- **Manual tests**: FTXUI frontend is tested manually against AC-TUI-1 through AC-TUI-10.
- **Coverage target**: 80% line coverage for libblackjack (measured with gcov or llvm-cov).
- **CI/CD**: All tests must pass on GCC, Clang, and MSVC on Linux, macOS, and Windows before merging.
```

**Rationale**: Without a defined testing standard, coverage will be inconsistent. Specify the bar.

---

### FINDING-017: Deterministic Seed Documentation Missing

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | testability |
| **Artifact** | SPEC-001, API-001 |
| **Section** | FR-8, Section 5.3 (Deterministic Testing) |

**Issue**: SPEC-001 FR-8 states "A constructor overload accepts a `uint32_t` seed for deterministic testing." API-001 Section 5.3 shows `Game game(42);`. However, nowhere is the expected deck order or behavior for a given seed documented.

**Recommendation**: Either:
- Document a reference seed (e.g., seed 42) and the resulting deck order in the test suite or a test documentation file, OR
- State that seed behavior is implementation-defined and tests should only verify reproducibility (same seed = same results on multiple runs), not specific card orders.

Add a note to API-001:
```
## 5.3 Deterministic Testing (Updated)
When constructed with a fixed seed, the Game produces a reproducible shuffle. The exact deck order is implementation-defined and may vary with std::mt19937 implementation or compiler. Tests should verify reproducibility (two Game instances with the same seed produce identical game flows) rather than assert specific card values.
```

**Rationale**: If tests hard-code expected cards for a seed, they will break on different platforms/compilers. Clarify what is guaranteed.

---

## Findings Summary

| # | Title | Severity | Category | Artifact | Status |
|---|-------|----------|----------|----------|--------|
| FINDING-001 | Missing Quantifiable Assertions in AC-4 | BLOCKER | testability | SPEC-001 | open |
| FINDING-002 | Non-Deterministic AC-5 | BLOCKER | testability | SPEC-001 | open |
| FINDING-003 | Ambiguous "Automatic Dealer Turn" | BLOCKER | testability | SPEC-001, API-001 | open |
| FINDING-004 | Missing Edge Case - Split Aces Blackjack | WARNING | testability | SPEC-001 | open |
| FINDING-005 | Missing Edge Case - Dealer Natural vs Player Non-Natural | WARNING | testability | SPEC-001 | open |
| FINDING-006 | Missing Edge Case - Empty Deck Scenario | WARNING | testability | SPEC-001, DATA-001 | open |
| FINDING-007 | Missing Edge Case - Player Busts Both Split Hands | NOTE | testability | SPEC-001 | open |
| FINDING-008 | Untestable AC-CLI-3 "Dealer Draws Per Rules" | WARNING | testability | SPEC-002 | open |
| FINDING-009 | CLI Invalid Input Loop Exit Condition | NOTE | testability | SPEC-002 | open |
| FINDING-010 | FTXUI Frontend Lacks Testability Guidance | BLOCKER | testability | SPEC-003 | open |
| FINDING-011 | Missing AC for Split Re-Split Validation | NOTE | testability | SPEC-001 | open |
| FINDING-012 | API Query Methods Behavior in Invalid States | WARNING | testability | API-001 | open |
| FINDING-013 | Missing Test Coverage for is_dealer_hole_card_visible() | NOTE | testability | SPEC-001, API-001 | open |
| FINDING-014 | No Acceptance Criteria for available_actions() | WARNING | testability | SPEC-001, API-001 | open |
| FINDING-015 | CLI and FTXUI Quit Behavior Not Testable | NOTE | testability | SPEC-002, SPEC-003 | open |
| FINDING-016 | No Regression Test Suite Specified | WARNING | testability | ARCH-001 | open |
| FINDING-017 | Deterministic Seed Documentation Missing | WARNING | testability | SPEC-001, API-001 | open |

## Overall Assessment

- [ ] **PASS**: No blockers. Ready to proceed.
- [ ] **CONDITIONAL PASS**: Warnings exist that should be addressed, but not blocking.
- [X] **FAIL**: Blockers exist. Must be resolved before proceeding.

**Blockers (4)**:
1. FINDING-001: AC-4 has untestable parenthetical exceptions
2. FINDING-002: AC-5 is non-deterministic without seed specification
3. FINDING-003: State machine behavior for dealer turn is ambiguous
4. FINDING-010: FTXUI frontend has no testability strategy (all ACs are visual/manual)

**Warnings (7)**: Edge cases missing (FINDING-004, 005, 006, 008, 012, 014, 016, 017). These should be addressed to ensure robust test coverage.

**Notes (6)**: Minor gaps in testability (FINDING-007, 009, 011, 013, 015). Recommended for completeness but not blocking.

## Sign-off

| Reviewer | Assessment | Date |
|----------|-----------|------|
| A11-test-engineer | FAIL | 2026-01-30 |

**Recommendation**: Resolve all BLOCKER findings before implementation begins. Address WARNING findings during or before implementation to ensure comprehensive test coverage. NOTE findings can be addressed during implementation or deferred to post-launch refinement.
