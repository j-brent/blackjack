<!-- metadata
id: DREV-UX
type: design-review
status: draft
author: A5-ux-designer
created: 2026-01-30
updated: 2026-01-30
approved-by: pending
dependencies: [SPEC-002, SPEC-003]
-->

# Design Review: UX Designer

**Reviewer**: A5-ux-designer (UX Designer)
**Artifacts Reviewed**: SPEC-002 (CLI Frontend), SPEC-003 (FTXUI Frontend)
**Review Date**: 2026-01-30

## Summary

Both specifications demonstrate strong attention to user experience with clear flow documentation and thoughtful state handling. However, critical UX gaps exist around error states, edge cases, soft ace handling clarity, and split flow completion. The FTXUI spec shows promise for achieving the Visionary's standard of "simple, beautiful, and elegant" design, but several areas need refinement to fully deliver on that vision. The CLI spec is solid as a baseline but lacks error handling detail.

## Findings

### FINDING-001: Missing Error State Coverage

| Property | Value |
|----------|-------|
| **Severity** | BLOCKER |
| **Category** | UX |
| **Artifact** | SPEC-002, SPEC-003 |
| **Section** | Game Flow, Display |

**Issue**: Neither spec addresses critical error states: what happens if the game engine throws an exception, if library initialization fails, or if the state becomes corrupted? Users need clear, actionable error messages, not crashes or undefined behavior.

**Recommendation**: Add section "Error Handling" to both specs covering:
- Library initialization failure: show clear error message explaining what failed and exit gracefully
- Unexpected game state errors: display user-friendly message (not raw exception text), offer option to restart
- Invalid state transitions: catch and handle with clear messaging
- For FTXUI: modal error dialog with "Restart Game" and "Exit" options

**Rationale**: Error states are user states. When things go wrong, the user must understand what happened and have a clear path forward. This is fundamental to positive UX and meets the Visionary's requirement for elegant design.

---

### FINDING-002: Split Flow Completion States Unclear

| Property | Value |
|----------|-------|
| **Severity** | BLOCKER |
| **Category** | UX |
| **Artifact** | SPEC-002, SPEC-003 |
| **Section** | 4.2 (SPEC-002), 5.5 (SPEC-003) |

**Issue**: Split flow describes playing "Hand 1 fully, then Hand 2" but doesn't specify:
- What happens when Hand 1 busts — does it immediately switch to Hand 2 or wait for user acknowledgment?
- When Hand 1 stands, is there a visual transition/confirmation before Hand 2 becomes active?
- After Hand 2 completes, is there a brief pause showing both final hands before dealer turn?
- Can the user review Hand 1's final state while playing Hand 2?

**Recommendation**:
- CLI: After each hand completes (bust or stand), print "Hand 1 complete. Moving to Hand 2..." with hand state visible
- FTXUI: Keep both hands visible, smoothly transition highlight from Hand 1 to Hand 2, show "Hand 1: [final value] - Now playing Hand 2" in status bar

**Rationale**: Split is the most complex flow. Users need clear signaling of transitions between hands to maintain mental model. Abrupt switches cause confusion.

---

### FINDING-003: Soft Ace Indicator Implementation Not Defined

| Property | Value |
|----------|-------|
| **Severity** | BLOCKER |
| **Category** | UX |
| **Artifact** | SPEC-002, SPEC-003 |
| **Section** | 2.2 (SPEC-002), 2.3 (SPEC-003) |

**Issue**: Both specs mention showing soft hands with "(soft)" indicator, but critical details missing:
- CLI spec example shows `Your hand: [A♠] [10♥] (21)` — is this soft or hard 21? Should it show `(21 soft)` or just `(21)`?
- When does the soft indicator appear vs disappear? (e.g., A+6 = "17 (soft)", then hit 5 = "12" — soft indicator removed?)
- What about hands like A+A+9 (soft 21 vs hard 12)? How is this displayed?

**Recommendation**:
- CLI: Always show hand value as "X" or "X (soft)". Examples: "18 (soft)" for A+7, "21" for A+10 (natural), "12" for A+A+10 (hard)
- FTXUI: Same convention, visually distinguish soft value (e.g., different color or "(s)" suffix)
- Add explicit examples to specs showing soft-to-hard transitions

**Rationale**: Soft vs hard aces is fundamental blackjack strategy. Users must clearly understand hand composition. Ambiguity here breaks the game's pedagogical value and creates frustration.

---

### FINDING-004: Natural Blackjack Flow Missing User Control

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | UX |
| **Artifact** | SPEC-002, SPEC-003 |
| **Section** | 4.1 (SPEC-002), 5.2 (SPEC-003) |

**Issue**: When player has natural blackjack, specs say "skip player input — display 'Blackjack!' and proceed to dealer turn." This removes user agency and feels abrupt, especially in FTXUI where the user expects to see the result and acknowledge it before dealer turn.

**Recommendation**:
- CLI: Display "Blackjack!" and "Press Enter to reveal dealer's hand"
- FTXUI: Display "Blackjack!" in status bar, change controls to single "[ Reveal Dealer ]" button, proceed on user input

**Rationale**: Even in automatic flows, users appreciate moment of control and acknowledgment. This makes the win feel earned rather than rushed past. Improves satisfaction and engagement.

---

### FINDING-005: Dealer Turn Visibility Gap During Player Bust

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | UX |
| **Artifact** | SPEC-002, SPEC-003 |
| **Section** | 4 (SPEC-002), 5.2/5.3 (SPEC-003) |

**Issue**: When player busts, the specs imply immediate round end without dealer turn. However, users may wonder: "Did the dealer also have a bad hand? What was the hole card?" Not showing dealer's hand after player bust is technically correct but reduces learning value.

**Recommendation**:
- After player bust, still reveal dealer's hole card and show final dealer hand
- Display result as "You bust. Dealer had [value]."
- This is optional flavor but improves transparency

**Rationale**: Seeing dealer's hand even in losing scenarios enhances learning and reduces feeling of "black box" game logic. Builds trust.

---

### FINDING-006: FTXUI Card Overlap Behavior Undefined

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | UX |
| **Artifact** | SPEC-003 |
| **Section** | 3.4 |

**Issue**: Spec says "If many cards would overflow the terminal width, overlap cards (show only left edge of earlier cards)" but doesn't specify:
- How much of each card remains visible when overlapped?
- Can user still distinguish suits/ranks of overlapped cards?
- At what point does this trigger (e.g., 7 cards at 7 chars each = 49 chars, threshold for 80-col terminal)?

**Recommendation**:
- Define overlap: show left 2-3 characters of each card when overlapping
- Ensure suit symbol remains visible in overlap
- Specify trigger: start overlapping when cards would exceed terminal width minus 10-char margin
- Add diagram showing 6-card hand in both normal and overlapped display

**Rationale**: Overlap done poorly makes cards unreadable. Users must be able to distinguish all cards in hand even in degraded layout. Clear spec prevents implementation guesswork.

---

### FINDING-007: Keyboard Shortcut Consistency Issue

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | UX |
| **Artifact** | SPEC-003 |
| **Section** | 4.1 |

**Issue**: Keyboard shortcuts use `H`, `S`, `P` for actions but `N` for New Game and `Q` for Quit. In the controls panel, buttons are `[ Hit ]`, `[ Stand ]`, `[ Split ]`, `[ New Game ]`, `[ Quit ]`. The shortcut letters should be visually indicated in button labels for discoverability.

**Recommendation**:
- Display buttons as `[ (H)it ]`, `[ (S)tand ]`, `[ (P)lit ]`, `[ (N)ew Game ]`, `[ (Q)uit ]`
- Or use underlined letter: `[ Hit ]` with 'H' underlined
- Ensure visual treatment is consistent across all buttons

**Rationale**: Keyboard shortcuts are only useful if discoverable. Without visual indication in the UI, users must guess or read docs. Standard UI pattern is to show shortcuts inline with labels.

---

### FINDING-008: Terminal Size Degradation Strategy Incomplete

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | UX |
| **Artifact** | SPEC-003 |
| **Section** | 6, 7 |

**Issue**: Spec says "degrade gracefully (smaller cards, abbreviate labels) rather than crash or garble" but doesn't specify degradation steps. What exactly happens at 50 cols? 45? 42? AC-TUI-10 only covers below 40x20 (show warning), but doesn't define the warning or behavior.

**Recommendation**:
- Define specific breakpoints:
  - 80x24+: Full layout with 7-char cards
  - 60-79 cols: Narrower cards (5 chars) or abbreviated suit symbols
  - 40-59 cols: Minimal cards (3 chars: rank + suit), compressed spacing
  - Below 40x20: Display centered message "Terminal too small. Please resize to at least 40x20." and pause game state
- Spec the exact warning message and whether app blocks input or allows play anyway

**Rationale**: "Degrade gracefully" without specifics leads to inconsistent implementation. Users with smaller terminals need predictable, functional experience. Clear breakpoints ensure elegant degradation, not guesswork.

---

### FINDING-009: Missing Empty State (Post-Quit Scenario)

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | UX |
| **Artifact** | SPEC-002, SPEC-003 |
| **Section** | 4 (SPEC-002), 5.4 (SPEC-003) |

**Issue**: Both specs describe quit flow (user presses Q or selects Quit) but don't specify if a confirmation prompt is shown or if exit is immediate. For FTXUI especially, immediate exit on 'Q' during active play could lead to accidental quits.

**Recommendation**:
- CLI: immediate exit on 'q' is fine (matches CLI conventions)
- FTXUI: during active play (not round-over), pressing Q should show confirmation modal: "Quit game? (Y/N)"
- FTXUI: during round-over state, Q/selecting Quit button can exit immediately

**Rationale**: Prevents accidental exits during active gameplay. FTXUI is a richer UI where users expect some guardrails. CLI users expect immediate response to typed commands. Different UX standards apply.

---

### FINDING-010: FTXUI Animation Timing Not Specified

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | UX |
| **Artifact** | SPEC-003 |
| **Section** | 5.2, 5.3 |

**Issue**: Spec mentions "brief visual effect", "brief pause of ~500ms between draws" but these are the only timing references. Missing:
- How long should card deal animation take (if implemented)?
- How long should hole card flip animation take?
- Should these be configurable or hardcoded?
- Is there a minimum time to ensure readability, or can user skip animations?

**Recommendation**:
- Define all animation timings explicitly:
  - Card deal: 150-200ms per card (optional)
  - Hole card flip: 300ms
  - Dealer draws: 500ms pause between each card (as stated)
  - Bust indication flash: 500ms
- State whether animations are skippable (e.g., press any key to skip to result)
- Keep animations short to avoid feeling sluggish

**Rationale**: Animations enhance elegance but poorly timed animations frustrate users. Explicit timings ensure consistent, polished feel. Option to skip respects user time, especially during repeated play.

---

### FINDING-011: Dealer Hand Value Display During Player Turn

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | UX |
| **Artifact** | SPEC-003 |
| **Section** | 2.2 |

**Issue**: Spec says dealer's hand value is "hidden during player turn, or show only the visible card's value". This is ambiguous. Showing visible card value (e.g., "Value: 10") is helpful for user strategy, but "hidden" suggests nothing shown. Needs clarification.

**Recommendation**:
- Always show the visible card's value during player turn: "Dealer: [K♦] [?] — Value: 10"
- After reveal, show full hand value: "Dealer: [K♦] [7♣] — Value: 17"
- Be explicit in spec which approach is used

**Rationale**: Showing visible dealer card value aids player strategy (basic blackjack play depends on dealer's up-card). Hiding it entirely is poor UX. Consistency with CLI spec (which shows `Dealer: [K♦] [??]` without value) should be reconsidered.

---

### FINDING-012: Play Again Prompt in CLI Too Abrupt

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | UX |
| **Artifact** | SPEC-002 |
| **Section** | 3.2 |

**Issue**: After displaying results, immediately prompting "Play again? (y/n):" doesn't give user a moment to absorb outcome, especially after a complex split hand. Feels rushed.

**Recommendation**:
- Add brief pause (500ms-1s) or wait for any key press before showing play-again prompt
- Or change prompt to "Press any key to continue..." then show "Play again? (y/n):"

**Rationale**: Pacing matters. A moment of pause after results lets users process outcome and feel satisfaction/disappointment before moving to next decision. Improves perceived quality.

---

### FINDING-013: Invalid Input Handling in FTXUI Not Specified

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | UX |
| **Artifact** | SPEC-003 |
| **Section** | 4 |

**Issue**: FTXUI spec defines keyboard shortcuts but doesn't say what happens if user presses an invalid key during gameplay (e.g., presses 'X'). Should there be feedback, or silent ignore?

**Recommendation**:
- Invalid keys during gameplay: silently ignore (no beep, no message)
- Invalid keys are only those not mapped to any action — all mapped keys (H/S/P/Q/N/Esc/Arrow/Tab/Enter) have defined behavior
- No need for error message clutter in a GUI

**Rationale**: In a GUI, unmapped keys should do nothing. This is standard behavior. Error messages for random keypresses would be annoying. Spec should state this explicitly to avoid implementation divergence.

---

### FINDING-014: Split Results Display Formatting Not Detailed

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | UX |
| **Artifact** | SPEC-002, SPEC-003 |
| **Section** | 2.4 (SPEC-002), 2.5 (SPEC-003) |

**Issue**: Both specs show split results as "Hand 1: Win! | Hand 2: Lose." but don't specify:
- Should final hand values be shown in results? (e.g., "Hand 1: 20 - Win | Hand 2: 19 - Lose")
- For CLI, where is this displayed relative to final hands display?
- For FTXUI, does this appear in status bar, or overlaid on hands?

**Recommendation**:
- CLI: After final hands are displayed with values, show per-hand results: "Hand 1 (20): Win! | Hand 2 (19): Lose."
- FTXUI: Show in status bar as specified, but also visually mark each hand panel (green border for win, red for loss, yellow for push)
- Add example outputs to specs

**Rationale**: Users need to connect results to hand values to understand outcome. Visual reinforcement (color coding) enhances clarity and elegance in FTXUI.

---

### FINDING-015: FTXUI Elegance Meets Vision Standard, CLI is Baseline Only

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | UX |
| **Artifact** | SPEC-002, SPEC-003 |
| **Section** | All |

**Issue**: Vision statement requires "simple, beautiful, and elegant" design with "positive user experience is a first-class requirement." SPEC-002 (CLI) is explicitly a "minimal" and "proof-of-concept" frontend. This is acceptable as stated, but expectations must align: CLI will not meet the elegance bar, and that's intentional.

SPEC-003 (FTXUI) shows strong design thinking: card rendering with suit colors, bordered panels, smooth transitions, keyboard shortcuts, visual state indicators. This has potential to achieve the elegance standard, but depends on execution quality (e.g., animations, color choices, spacing).

**Recommendation**:
- Acknowledge in SPEC-002 that CLI is intentionally basic and serves as functional baseline, not elegant UX
- For SPEC-003, add explicit design review checkpoint: after implementation, conduct visual QA against "simple, beautiful, elegant" standard before considering it complete
- Consider adding screenshot/mockup examples to SPEC-003 for alignment on visual aesthetics

**Rationale**: The Visionary's high standard for elegance is clear. FTXUI is the first frontend attempting to meet it. Setting explicit quality gates ensures the vision is realized, not approximated.

---

## Findings Summary

| # | Title | Severity | Category | Artifact | Status |
|---|-------|----------|----------|----------|--------|
| FINDING-001 | Missing Error State Coverage | BLOCKER | UX | SPEC-002, SPEC-003 | open |
| FINDING-002 | Split Flow Completion States Unclear | BLOCKER | UX | SPEC-002, SPEC-003 | open |
| FINDING-003 | Soft Ace Indicator Implementation Not Defined | BLOCKER | UX | SPEC-002, SPEC-003 | open |
| FINDING-004 | Natural Blackjack Flow Missing User Control | WARNING | UX | SPEC-002, SPEC-003 | open |
| FINDING-005 | Dealer Turn Visibility Gap During Player Bust | WARNING | UX | SPEC-002, SPEC-003 | open |
| FINDING-006 | FTXUI Card Overlap Behavior Undefined | WARNING | UX | SPEC-003 | open |
| FINDING-007 | Keyboard Shortcut Consistency Issue | WARNING | UX | SPEC-003 | open |
| FINDING-008 | Terminal Size Degradation Strategy Incomplete | WARNING | UX | SPEC-003 | open |
| FINDING-009 | Missing Empty State (Post-Quit Scenario) | NOTE | UX | SPEC-002, SPEC-003 | open |
| FINDING-010 | FTXUI Animation Timing Not Specified | NOTE | UX | SPEC-003 | open |
| FINDING-011 | Dealer Hand Value Display During Player Turn | NOTE | UX | SPEC-003 | open |
| FINDING-012 | Play Again Prompt in CLI Too Abrupt | NOTE | UX | SPEC-002 | open |
| FINDING-013 | Invalid Input Handling in FTXUI Not Specified | NOTE | UX | SPEC-003 | open |
| FINDING-014 | Split Results Display Formatting Not Detailed | NOTE | UX | SPEC-002, SPEC-003 | open |
| FINDING-015 | FTXUI Elegance Meets Vision Standard, CLI is Baseline Only | NOTE | UX | SPEC-002, SPEC-003 | open |

## Overall Assessment

- [ ] **PASS**: No blockers. Ready to proceed.
- [ ] **CONDITIONAL PASS**: Warnings exist that should be addressed, but not blocking.
- [x] **FAIL**: Blockers exist. Must be resolved before proceeding.

**Rationale**: Three BLOCKER-level findings exist that would result in poor user experience and incomplete user flows if left unaddressed:
1. No error handling strategy leaves users stranded when things go wrong
2. Split flow transitions are undefined, causing confusion in the most complex scenario
3. Soft ace display ambiguity breaks fundamental game feedback

These must be resolved before implementation. The WARNING-level findings significantly impact quality and elegance, and should be addressed to meet the Visionary's stated standard. The NOTE-level findings are polish items that enhance but don't block.

The FTXUI spec shows strong UX thinking and has the foundation for elegant design. The CLI spec is appropriately minimal. Both need refinement to deliver complete, polished user experiences.

## Sign-off

| Reviewer | Assessment | Date |
|----------|-----------|------|
| A5-ux-designer | FAIL | 2026-01-30 |
