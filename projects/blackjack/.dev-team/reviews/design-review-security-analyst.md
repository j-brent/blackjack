<!-- metadata
id: DREV-SEC
type: design-review
status: draft
author: A14-security-analyst
created: 2026-01-30
updated: 2026-01-30
approved-by: pending
dependencies: [ARCH-001, API-001, SPEC-001]
-->

# Design Review: Security Analyst

**Reviewer**: A14-security-analyst
**Artifacts Reviewed**: ARCH-001, API-001, SPEC-001
**Review Date**: 2026-01-30

## Summary

The blackjack game engine presents a minimal attack surface appropriate for a local, single-player game. The design demonstrates sound defensive principles: no I/O, no network exposure, no user data persistence, and a command-query API that returns error codes rather than throwing exceptions. The primary security concerns center on input validation through the API state machine, memory safety in the C++ implementation, and PRNG quality for fair gameplay. No blocking security issues identified; warnings address implementation hygiene and future-phase considerations.

## Findings

### FINDING-001: Incomplete API State Machine Error Handling Specification

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | security |
| **Artifact** | API-001 |
| **Section** | Section 4 (Game API), Section 7 (Error Handling Contract) |

**Issue**: The API contract specifies that invalid commands return `ActionResult` error codes and "never crash, assert, throw, or corrupt state," but does not explicitly document edge cases such as calling methods on a default-constructed `Game` object before `deal()`, or calling query methods like `dealer_hand()` when no round is in progress. While section 7 states that query methods "return empty/default data" in unexpected states, the specification lacks concrete examples of what "empty/default" means for each query method.

**Recommendation**: Add to API-001 section 9 (Memory and Lifetime) explicit guarantees for each query method's behavior in invalid states:
- `player_hands()` returns empty span when state is `WaitingForDeal`
- `dealer_hand()` returns empty hand when no round active
- `dealer_up_card()` returns `std::nullopt` when no round active
- `available_actions()` returns empty vector when state is not `PlayerTurn`

This defensive specification prevents frontends from making assumptions that could lead to undefined behavior if the library implementation changes.

**Rationale**: Clear failure modes prevent frontends from writing code that relies on undefined behavior. While the current design appears sound, explicit documentation reduces implementation risk and prevents future regressions.

---

### FINDING-002: No Bounds Checking Specification for Hand/Deck Sizes

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | security |
| **Artifact** | API-001 |
| **Section** | Section 2 (hand.hpp), Section 3 (deck.hpp) |

**Issue**: The `Hand::add_card()` and `Deck::draw()` methods have no documented maximum bounds. A malicious or buggy frontend could theoretically call `hit()` repeatedly until memory exhaustion, or the deck could be drawn empty causing undefined behavior. SPEC-001 notes "a single-deck blackjack game uses at most ~20 cards," but the API does not enforce this.

**Recommendation**:
1. Add to API-001 section 3 (Deck): Specify that `draw()` on an empty deck returns a sentinel value or that the library guarantees this never happens during valid game flow (since a new deck is created for each `deal()` and max cards needed is bounded).
2. Add to API-001 section 4 (Game): Document that the library enforces maximum hand size internally (e.g., 11 cards max — even with 11 aces, the hand value would be 11). If this limit is hit, `hit()` returns `ActionResult::InvalidAction`.
3. Alternatively, accept that the deck size (52 cards) and game rules naturally bound the maximum hand size and document this assumption in SPEC-001 section 4 NFR-4 (Testability).

**Rationale**: Explicit bounds prevent denial-of-service via memory exhaustion. While the game rules naturally limit this, defensive specification protects against implementation bugs where state machine logic fails to transition correctly.

---

### FINDING-003: PRNG Seed Entropy Source Not Specified

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | security |
| **Artifact** | SPEC-001, ARCH-001 |
| **Section** | SPEC-001 section 3 FR-8, ARCH-001 section 3 |

**Issue**: The default `Game()` constructor uses `std::random_device` to seed `std::mt19937`, but the quality of `std::random_device` is implementation-defined. On some platforms (notably MinGW), `std::random_device` may be deterministic or low-entropy. For a single-player game with no prizes or stakes, this is not a cryptographic concern, but it could allow patterns in shuffling that reduce gameplay fairness or allow card prediction.

**Recommendation**: Add to SPEC-001 section 3 FR-8 a note that the default seed uses `std::random_device` which is platform-dependent, and that on platforms where this is deterministic (e.g., MinGW), frontends should supply their own seed from a higher-quality source (e.g., system time, hardware entropy). Alternatively, the library could mix `std::random_device` with `std::chrono::high_resolution_clock::now()` to ensure non-determinism.

**Rationale**: Players expect fair shuffles. A predictable PRNG undermines this expectation even if no money is at stake. Documenting this allows frontend developers to make informed decisions.

---

### FINDING-004: std::span Lifetime Aliasing Risk

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | security |
| **Artifact** | API-001 |
| **Section** | Section 9 (Memory and Lifetime) |

**Issue**: The API specifies that `std::span<const HandState>` returned by `player_hands()` is invalidated by any command call, but frontends may not enforce this discipline. If a frontend caches the span across a `hit()` or `split()` call and then dereferences it, undefined behavior results (dangling reference). This is a use-after-free risk in the frontend, not the library, but the API design enables this footgun.

**Recommendation**:
1. Emphasize in API-001 section 9 that spans are valid only until the next non-const method call.
2. Consider adding a debug mode (e.g., `#ifdef BLACKJACK_DEBUG`) where the library increments an internal mutation counter on every command, and the span checks this counter on dereference to detect stale access. This would not be enabled in production but would catch bugs during development.
3. For future phases (WASM, Python), prefer returning `std::vector` copies in the bindings rather than exposing spans, trading performance for safety.

**Rationale**: Use-after-free is a common C++ bug class and a security vulnerability if exploited. While this is a local game with no external attackers, defensive design reduces crash risk and improves reliability.

---

### FINDING-005: No Input Sanitization for seed Parameter

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | security |
| **Artifact** | API-001 |
| **Section** | Section 4 (Game constructor) |

**Issue**: The `Game(uint32_t seed)` constructor accepts any 32-bit seed value, including zero. While this is valid for `std::mt19937`, a seed of zero produces a known, predictable sequence. If a frontend inadvertently passes zero (e.g., from uninitialized memory or user input), the game becomes deterministic and unfair.

**Recommendation**: Add to API-001 section 4 a note that seed=0 is valid but not recommended for gameplay (reserve it for testing). Alternatively, the library could reject seed=0 and default to random seeding, or document that seed=0 is explicitly reserved for deterministic testing scenarios.

**Rationale**: Predictable shuffles reduce fairness. While this is a NOTE severity (it's the frontend's responsibility to validate input), explicit documentation prevents accidental misuse.

---

### FINDING-006: No Specification for Thread Safety Beyond "Not Thread-Safe"

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | security |
| **Artifact** | API-001 |
| **Section** | Section 8 (Thread Safety) |

**Issue**: API-001 states the `Game` class is "NOT thread-safe" and requires the caller to serialize access. However, it does not specify whether simultaneous access from multiple threads causes merely incorrect behavior or undefined behavior (e.g., data races leading to memory corruption). For a local game this is low-priority, but future phases (WASM with web workers, Python with threading) expand the risk surface.

**Recommendation**: Add to API-001 section 8 a clarification: "Simultaneous access from multiple threads without external synchronization results in data races and undefined behavior. Use external locking (e.g., `std::mutex`) if multi-threaded access is required."

**Rationale**: Undefined behavior in the presence of data races can escalate to memory corruption. Explicit documentation sets expectations for future binding authors.

---

### FINDING-007: Future Phase WASM/Python Bindings Expand Input Attack Surface

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | security |
| **Artifact** | ARCH-001, API-001 |
| **Section** | ARCH-001 section 7 (Future Phases), API-001 section 10 |

**Issue**: The current API is consumed by trusted C++ frontends, but Phase 2 (WASM) and Phase 3 (Python) will expose the API to JavaScript and Python code, which may include untrusted or malicious input. JavaScript in particular has no type safety, so a malicious actor could call the WASM bindings with unexpected argument types or call sequences. While the library's error-return design mitigates this, the bindings layer must validate all inputs.

**Recommendation**: Add to ARCH-001 section 7 a note that future binding layers (WASM C API wrapper, Python bindings) must validate all inputs at the binding boundary:
- Enum values must be in valid ranges (e.g., `PlayerAction` is 0-2)
- Pointers/references must be non-null (if the C API uses pointers)
- Call sequences must respect state machine invariants (though the library already enforces this via `ActionResult`)

Consider adding a Phase 2 security review specifically for the WASM bindings once they are designed.

**Rationale**: Untrusted input is the primary attack vector. While the library is defensive, bindings introduce a new trust boundary that requires explicit validation.

---

### FINDING-008: No Specification for Error Code Exhaustiveness

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | security |
| **Artifact** | API-001 |
| **Section** | Section 1 (types.hpp, ActionResult enum) |

**Issue**: The `ActionResult` enum defines four error codes (`Success`, `InvalidAction`, `InvalidSplit`, `AlreadySplit`), but it's unclear if this list is exhaustive or if the library may add new error codes in future versions. If frontends switch on `ActionResult` without a default case, adding a new error code could cause undefined behavior.

**Recommendation**: Add to API-001 section 1 a stability guarantee for `ActionResult`: "This enum is exhaustive as of version 1.0. Future versions may add new error codes, so frontends should include a default case in switch statements to handle unknown codes gracefully."

**Rationale**: Enum exhaustiveness is a common source of bugs when libraries evolve. Explicit documentation allows frontends to write defensive code.

---

## Findings Summary

| # | Title | Severity | Category | Artifact | Status |
|---|-------|----------|----------|----------|--------|
| FINDING-001 | Incomplete API State Machine Error Handling Specification | WARNING | security | API-001 | open |
| FINDING-002 | No Bounds Checking Specification for Hand/Deck Sizes | WARNING | security | API-001 | open |
| FINDING-003 | PRNG Seed Entropy Source Not Specified | NOTE | security | SPEC-001, ARCH-001 | open |
| FINDING-004 | std::span Lifetime Aliasing Risk | WARNING | security | API-001 | open |
| FINDING-005 | No Input Sanitization for seed Parameter | NOTE | security | API-001 | open |
| FINDING-006 | No Specification for Thread Safety Beyond "Not Thread-Safe" | NOTE | security | API-001 | open |
| FINDING-007 | Future Phase WASM/Python Bindings Expand Input Attack Surface | NOTE | security | ARCH-001, API-001 | open |
| FINDING-008 | No Specification for Error Code Exhaustiveness | NOTE | security | API-001 | open |

## Overall Assessment

- [ ] **PASS**: No blockers. Ready to proceed.
- [x] **CONDITIONAL PASS**: Warnings exist that should be addressed, but not blocking.
- [ ] **FAIL**: Blockers exist. Must be resolved before proceeding.

**Justification**: The three WARNING-level findings (incomplete error handling spec, missing bounds checking, span lifetime aliasing) are implementation hygiene issues that should be addressed via specification updates before implementation begins. They do not require architectural changes and can be resolved through documentation clarifications. The five NOTE-level findings are forward-looking considerations that do not block Phase 1 development but should inform future phase planning.

The core architectural decisions (no I/O, error codes instead of exceptions, passive state machine, seedable PRNG) demonstrate security-conscious design appropriate for a local game. The threat model is correctly sized: no authentication, no network, no persistence, no external attack surface. The warnings address internal robustness rather than external threats.

**Recommended Next Steps**:
1. Update API-001 to address FINDING-001, FINDING-002, FINDING-004, and FINDING-008 via specification clarifications.
2. Update SPEC-001 to address FINDING-003 with PRNG entropy documentation.
3. Update ARCH-001 section 7 to address FINDING-007 with binding security requirements.
4. Proceed to implementation with the understanding that these specification updates will be incorporated into the API header documentation.

## Sign-off

| Reviewer | Assessment | Date |
|----------|-----------|------|
| A14-security-analyst | CONDITIONAL PASS | 2026-01-30 |
