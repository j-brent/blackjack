# CSS Learnings

## [2026-01-31] Lesson: Use rem and em instead of px for layout and typography
**ID**: f0a1b2c3
**Category**: language
**Context**: When writing CSS for any web application, especially responsive designs
**Learning**: Use `rem` for spacing, typography, and border-radius. Use `em` for component-internal sizing that should scale with its container (e.g., card dimensions in a game). Reserve `px` only for borders, box-shadows, and outlines. Define all values as CSS custom properties in `:root` — no magic numbers in component styles. This ensures the layout respects user font-size preferences and scales consistently across devices.
**Evidence**: Generated web frontend used hardcoded `px` values for all spacing, font sizes, and card dimensions. Layout broke at different zoom levels and didn't respect user font-size preferences. Required full conversion of all design tokens to rem/em.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team

## [2026-01-31] Lesson: Use dvh not vh for viewport-fitting layouts
**ID**: a1b2c3d4
**Category**: language
**Context**: When building single-screen layouts that must fill exactly the viewport (games, dashboards, full-screen tools)
**Learning**: Use `height: 100dvh` (dynamic viewport height), not `100vh`. On mobile browsers, `100vh` includes the area behind the URL bar, causing content to overflow the visible area. `dvh` accounts for the browser chrome and gives the actual visible height. Combine with `overflow: hidden` on the container and `flex: 1` with `min-height: 0` on flex children — never use fixed `min-height` in pixels on layout sections, as it prevents flexbox from shrinking content to fit.
**Evidence**: Generated layout used `min-height: 100vh` and `min-height: 180px` on dealer/player areas, resulting in ~673px minimum height that overflowed mobile viewports (~660px usable). Required rewrite to `100dvh` + flex-based sizing.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team

## [2026-01-31] Lesson: Budget vertical space on smallest target viewport before writing CSS
**ID**: b2c3d4e5
**Category**: language
**Context**: When designing mobile-first layouts, especially viewport-fitting ones
**Learning**: Before writing CSS, calculate the vertical space budget for the smallest target viewport (typically 360x640 for Android). Sum up all fixed-height elements (header, footer, action bars, padding, gaps) and verify the total fits within ~600px usable height (after browser chrome). Keep action buttons in a horizontal row at all breakpoints unless there are 5+ buttons — vertical stacking wastes scarce mobile viewport height.
**Evidence**: Generated layout stacked buttons vertically and used generous padding/gaps that totaled ~673px minimum, overflowing the 360x640 viewport. Required tightening gaps, switching buttons to horizontal, and removing fixed min-heights to fit within ~492px.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team

## [2026-01-31] Lesson: CSS fill overrides SVG presentation attributes — use specific selectors to restore them
**ID**: e4f5g6h7
**Category**: language
**Context**: When styling SVG elements that use both CSS classes and SVG fill attributes (e.g., pattern fills)
**Learning**: CSS `fill` properties always override SVG `fill` presentation attributes, regardless of specificity. If a class like `.card-bg { fill: white }` is applied to an SVG `<rect fill="url(#pattern)">`, the CSS wins and the pattern is lost. Fix by adding a more specific CSS rule (e.g., `.card.face-down .card-bg { fill: url(#card-back-stripes) }`) rather than removing the shared class. This is a fundamental SVG/CSS interaction: CSS properties > SVG presentation attributes > inherited values.
**Evidence**: Face-down card in blackjack web app had `class="card-bg"` and `fill="url(#card-back-stripes)"` on its rect. CSS `.card-bg { fill: var(--color-card-bg) }` (white) overrode the stripe pattern, making the card appear blank white. Fixed with a `.card.face-down .card-bg` override rule.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team

## [2026-01-31] Lesson: Use SVG with viewBox for playing cards, not styled divs
**ID**: c3d4e5f6
**Category**: language
**Context**: When rendering playing cards or similar visual elements in a web UI
**Learning**: Use inline `<svg>` elements with a `viewBox` (e.g., `0 0 250 350`) instead of styled `<div>` elements. Benefits: (1) resolution-independent rendering at any size, (2) internal proportions scale automatically via viewBox, (3) no em/font-size tricks for internal text sizing — use SVG attributes for font-size (unitless viewBox coordinates), not CSS px. Style with `fill` instead of `color`, `filter: drop-shadow()` instead of `box-shadow`, and SVG `<rect rx>` instead of CSS `border-radius`. Set card width/height via CSS on the `<svg>` element using em units for container-relative scaling. For rotated bottom-right text, rotate around the card center (e.g., `rotate(180, 125, 175)`) using the same coordinates as the top-left text — this keeps text extending inward after rotation, preventing viewBox clipping.
**Evidence**: Rewrote card-renderer.js from div-based to SVG-based. Initially used `rotate(180, 235, 315)` with `text-anchor: end` at x=235 for bottom-right text — after rotation, text extended past the viewBox right edge (x>250) and was clipped. Fixed by using top-left coordinates (x=15, y=62) rotated around card center (125, 175). Font sizes moved from CSS (px) to SVG attributes (unitless) to comply with no-px coding standard.
**Confidence**: high
**Validations**: 2
**Projects**: dev-team

## [2026-01-31] Lesson: Set SVG text font-size as attributes, not CSS, to avoid px in stylesheets
**ID**: g6h7i8j9
**Category**: language
**Context**: When styling text inside SVG elements that use a viewBox, in a project with a no-px CSS convention
**Learning**: Inside an SVG with a viewBox, CSS `font-size: 28px` maps to viewBox user units (not screen pixels) and scales correctly. However, it still puts `px` in the stylesheet, violating no-px conventions. Instead, set `font-size` as an SVG attribute on the `<text>` element (e.g., `el.setAttribute('font-size', '56')`). SVG attributes are unitless and use viewBox coordinates natively. This keeps CSS clean of px while achieving identical rendering. Reserve CSS for properties that genuinely need to be in the stylesheet (font-weight, font-family, fill).
**Evidence**: Blackjack card text had `font-size: 28px` / `80px` / `48px` in CSS for `.card-rank-top`, `.card-suit-center`, `.card-back-text`. Moved all three to SVG attributes in card-renderer.js, eliminating px from the stylesheet entirely.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team

## [2026-01-31] Lesson: Use negative margin fan overlap for card hands, not horizontal scroll
**ID**: d4e5f6g7
**Category**: language
**Context**: When displaying a variable number of playing cards in a horizontal layout that must fit any viewport
**Learning**: Never use `overflow-x: auto` for card containers — horizontal scrollbars break the viewport-fitting layout. Instead, use negative margins on `.card:not(:first-child)` (e.g., `margin-left: -3em`) so cards fan/overlap like a real hand. The overlap amount should leave enough of each card's top-left corner visible to show rank and suit. The last card is always fully visible. This approach scales naturally: 2 cards barely overlap while 7+ cards compress into the available space without scrolling.
**Evidence**: Original layout used `overflow-x: auto` with `flex-shrink: 0`, causing horizontal scrollbar with 5+ cards. Replaced with `-3em` negative margin overlap. At 360px viewport with 7 cards: 80px + 6x32px = 272px, fits comfortably in ~320px available width.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team
