# Testing Learnings

## [2026-01-31] Lesson: Write visual containment acceptance tests for layout-sensitive UIs
**ID**: f5g6h7i8
**Category**: domain
**Context**: When building viewport-fitting layouts where elements must stay within parent bounds (games, dashboards, single-screen apps)
**Learning**: Layout acceptance tests should include a general containment check that walks parent-child pairs and asserts each child's bounding rect is within its parent's. Key pairs: cards within card-containers, containers within areas, areas within main, main within viewport. Use `getBoundingClientRect()` with a small tolerance (~2px) for sub-pixel rounding. Run the containment check in every distinct layout state (initial deal, split hands, game over). This catches overflow bugs that scroll-based tests miss — a child can overflow its parent without triggering a scrollbar if the parent's ancestor has `overflow: hidden`. For SVG elements, also check that `getBBox()` of text elements stays within the viewBox bounds to catch rotation/transform clipping.
**Evidence**: Blackjack web app had cards overflowing their hand-container borders (visible in screenshots) but all existing tests passed — they only checked for scrollbar absence and element visibility. Added AT-9 (SVG text within viewBox) and AT-10 (visual containment at 360x640) which would have caught both the card clipping bug and the split hand overflow.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team
