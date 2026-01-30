<!-- metadata
id: BLOCKER-RESOLUTION-001
type: resolution-log
status: complete
author: A2-architect
created: 2026-01-30
-->

# Blocker Resolution Log

Summary of changes made to resolve all 7 BLOCKER findings and additional high-priority warnings from the Stage 4 design reviews.

## BLOCKER Resolutions

### BLOCKER 1 — DREV-TEST FINDING-001: AC-4 Untestable Parenthetical Exception

**Problem:** AC-4 contained "(unless natural blackjack)" which is untestable as a single AC.

**Resolution:** Split AC-4 in SPEC-001 into three separate ACs:
- **AC-4** (Normal): deal() when no natural blackjack -> PLAYER_TURN
- **AC-4b** (Natural Blackjack): deal() when player has natural -> DEALER_TURN
- **AC-4c** (Invalid State): deal() in wrong state -> error

**File:** `spec-001-blackjack-engine.md`

---

### BLOCKER 2 — DREV-TEST FINDING-002: AC-5 Non-Deterministic Card Reference

**Problem:** AC-5 referenced "the drawn card is a 9" without specifying a seed, making the test non-deterministic.

**Resolution:** Rewrote AC-5 to be seed-agnostic:
- **AC-5** (No Bust): Asserts hand has one more card and value increased by drawn card's value
- **AC-5b** (Bust): Asserts hand is bust and state advances, without specifying a particular card

**File:** `spec-001-blackjack-engine.md`

---

### BLOCKER 3 — DREV-TEST FINDING-003: Ambiguous "Automatic" Dealer Turn Language

**Problem:** SPEC-001 said "dealer plays automatically" but API-001 requires frontend to call `play_dealer()`.

**Resolution:**
- SPEC-001 FR-4 state diagram: Changed "(automatic -- dealer draws per rules)" to "(frontend calls play_dealer() to advance)"
- SPEC-001 FR-6: Replaced "Executes automatically" with explicit statement that frontend must call `play_dealer()`
- SPEC-001 AC-11: Replaced "dealer plays automatically" with explicit `play_dealer()` call sequence
- API-001 section 6.1: Removed "auto-advances" language, stated frontend **must** call `play_dealer()`
- API-001 `play_dealer()` comment: Added IMPORTANT note that engine does not auto-play

**Files:** `spec-001-blackjack-engine.md`, `api-contracts.md`

---

### BLOCKER 4 — DREV-TEST FINDING-010: SPEC-003 No Testability Guidance

**Problem:** All SPEC-003 ACs are visual with no testing strategy defined.

**Resolution:** Added section 9 "Testing Strategy" to SPEC-003 covering:
- 9.1: Rule correctness tested at library level (not duplicated in frontend)
- 9.2: Compile and launch smoke test (automated)
- 9.3: Manual visual testing checklist
- 9.4: Future automated UI testing boundary note

**File:** `spec-003-ftxui-frontend.md`

---

### BLOCKER 5 — DREV-UX FINDING-001: Missing Error State Coverage

**Problem:** SPEC-002 and SPEC-003 had no error handling sections.

**Resolution:**
- SPEC-002: Added section 7 "Error Handling" with subsections for library API errors, unexpected/internal errors, and input stream errors
- SPEC-003: Added section 10 "Error Handling" with subsections for library API errors (status bar message), unexpected errors (terminal cleanup), and terminal resize during play

**Files:** `spec-002-cli-frontend.md`, `spec-003-ftxui-frontend.md`

---

### BLOCKER 6 — DREV-UX FINDING-002: Split Flow Transition Unclear

**Problem:** When Hand 1 busts/stands, transition to Hand 2 had no defined notification mechanism.

**Resolution:**
- SPEC-002 section 4.2: Added hand transition messages (`"--- Hand 1 complete. Now playing Hand 2 ---"` on stand, `"Hand 1: Bust! --- Now playing Hand 2 ---"` on bust)
- SPEC-002 game flow steps 4e/4f: Added transition message reference
- SPEC-003 section 5.5: Added status bar transition messages (`"Hand 1 complete. Playing Hand 2..."` etc.) and active hand highlight movement

**Files:** `spec-002-cli-frontend.md`, `spec-003-ftxui-frontend.md`

---

### BLOCKER 7 — DREV-UX FINDING-003: Soft Ace Indicator Not Defined

**Problem:** No specification for when "(soft)" appears or how soft-to-hard transitions display.

**Resolution:**
- SPEC-002 section 2.5: Added "Soft Ace Display" with rules and four concrete examples (soft, still soft, demoted, multiple aces)
- SPEC-002 AC-CLI-9: Added acceptance criteria for soft ace display and demotion
- SPEC-003 section 2.6: Added equivalent "Soft Ace Display" with same rules and examples
- Both sections reference `Hand::is_soft()` as the source of truth

**Files:** `spec-002-cli-frontend.md`, `spec-003-ftxui-frontend.md`

---

## WARNING Resolutions

### WARNING FINDING-004 (Test): Split Aces + 10-value = Win, Not Blackjack
Added **AC-12** to SPEC-001 specifying that a split hand reaching 21 is a Win, not a Blackjack, because natural blackjack requires the original 2-card deal.

### WARNING FINDING-005 (Test): Dealer Natural vs Player Non-Natural
Added **AC-12b** to SPEC-001 specifying that when dealer has natural blackjack and player does not, the result is Lose.

### WARNING FINDING-012 (Test): Query Method Behavior in Invalid States
Added **section 7.1** to API-001 with a table documenting return values for all query methods across all four game states.

### WARNING FINDING-014 (Test): No AC for `available_actions()`
Added **AC-13** to SPEC-001 with three scenarios: non-pair hand (Hit/Stand only), splittable pair (Hit/Stand/Split), and non-PlayerTurn states (empty list).

### WARNING FINDING-017 (Test): Deterministic Seed Documentation Missing
Added **AC-14** to SPEC-001 specifying the reproducibility guarantee. Added **section 7.2** to API-001 with the deterministic seed contract (reproducibility-only, implementation-defined card order, no other non-determinism sources).

### WARNING FINDING-005 (UX): Reveal Dealer Hand After Player Bust
- SPEC-002 AC-CLI-8: Added "dealer's hole card is revealed and final dealer hand is shown" on bust
- SPEC-002 game flow step 5: Added note that dealer hand is always revealed, even on bust
- SPEC-003 AC-TUI-9: Added "dealer's hole card is revealed and full dealer hand is displayed for transparency"

### WARNING FINDING-007 (UX): Show Keyboard Shortcuts in FTXUI Button Labels
- SPEC-003 layout diagram: Changed `[ Hit ]` to `[ (H)it ]` etc.
- SPEC-003 section 2.4: Updated button label format to include shortcut hints: `[ (H)it ]`, `[ (S)tand ]`, `[ S(p)lit ]`, `[ (N)ew Game ]`, `[ (Q)uit ]`
