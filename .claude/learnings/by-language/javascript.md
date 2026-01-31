# JavaScript Learnings

## [2026-01-31] Lesson: Embind enum values require .value to extract the integer
**ID**: g1h2i3j4
**Category**: language
**Context**: When consuming C++ enums exposed to JavaScript via Emscripten Embind
**Learning**: Embind wraps C++ `enum class` values in JavaScript objects. Accessing an enum property returns `{value: N}`, not the raw integer `N`. Always use `.value` to extract the numeric value (e.g., `card.rank.value`, `state.value`, `action.value`). Without `.value`, the object coerces to `[object Object]` in string contexts and fails silently in numeric comparisons or map lookups. When writing a JS wrapper around an Embind-generated API, convert all enum properties to plain integers in one place (e.g., a `getState()` method) so the rest of the application never touches Embind objects directly.
**Evidence**: Web frontend game-controller.js accessed `card.rank` and `card.suit` directly instead of `card.rank.value` and `card.suit.value`. Cards displayed "?" for rank/suit and dealer value showed "[object Object]". Fixed by adding `.value` to all 7 enum access points in the getState() method.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team

## [2026-01-31] Lesson: Pre-render UI elements and toggle visibility instead of DOM recreation
**ID**: h2i3j4k5
**Category**: language
**Context**: When building interactive UIs with buttons/controls that appear/disappear based on state
**Learning**: Pre-render all UI elements (buttons, panels, controls) in the HTML and toggle the `hidden` attribute based on state. Never clear `innerHTML` and recreate elements on each render. Benefits: (1) no layout shifts from DOM insertion/removal, (2) event delegation works consistently since elements always exist, (3) simpler code — toggle a boolean instead of createElement/appendChild. When checking if a button is interactive, verify both `!button.hidden` and `!button.disabled` since these are orthogonal concerns.
**Evidence**: Blackjack web UI originally cleared `#actions` innerHTML and recreated buttons on every render, causing buttons to jump around. Refactored to 4 static buttons in HTML with `hidden` attribute toggled by renderActions(). Eliminated layout shifts and simplified the rendering code.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team
