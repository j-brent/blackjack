# Blackjack Web Frontend

Single-page vanilla JavaScript application for playing blackjack in a web browser. The C++ game engine is compiled to WebAssembly via Emscripten.

## Build Instructions

### Prerequisites

- Emscripten SDK installed and activated
- CMake 3.20 or higher
- A modern web browser (Chrome 57+, Firefox 53+, Safari 11+, Edge 79+)

### Build Steps

```sh
# Configure with Emscripten toolchain
emcmake cmake -B build -S .

# Build WASM module
cmake --build build

# Output files: build/blackjack.wasm and build/blackjack.js
```

## Running Locally

WASM modules require HTTP serving (cannot be loaded from `file://` URLs).

### Option 1: Using emrun (included with Emscripten)

```sh
emrun --browser chrome index.html
```

### Option 2: Using Python's built-in HTTP server

```sh
# From the blackjack/web directory
python -m http.server 8080

# Open http://localhost:8080 in your browser
```

### Option 3: Using any other local server

Any static file server will work. Just serve from the `blackjack/web` directory.

## Project Structure

```
web/
├── index.html              # HTML shell
├── style.css               # All styles, design tokens as CSS variables
├── src/
│   ├── main.js             # Entry point, WASM loading, event wiring
│   ├── game-controller.js  # Game state manager, WASM API wrapper
│   ├── ui-renderer.js      # DOM manipulation, state → UI
│   ├── card-renderer.js    # Card HTML generation
│   └── bindings.cpp        # Emscripten Embind bindings (C++)
├── build/                  # Build output (gitignored)
│   ├── blackjack.wasm      # Compiled WASM module
│   └── blackjack.js        # Emscripten glue code
└── README.md               # This file
```

## Features

- Pure vanilla JavaScript (no frameworks, no bundler)
- ES6 modules with native browser support
- Responsive design (360px mobile to 1920px+ desktop)
- WCAG 2.1 AA accessibility compliance
- Keyboard shortcuts (H, S, P, N)
- Offline-ready after initial load (no network calls during gameplay)

## Controls

- **H** - Hit (draw a card)
- **S** - Stand (end turn)
- **P** - Split (split pairs, when available)
- **N** - New Game (start new round)

All controls also work via mouse/touch on the UI buttons.

## Browser Compatibility

| Browser | Minimum Version |
|---------|----------------|
| Chrome  | 57+            |
| Firefox | 53+            |
| Safari  | 11+            |
| Edge    | 79+ (Chromium) |

No support for Internet Explorer.

## Development

The web frontend follows the coding standards defined in `.dev-team/coding-standards.md` (for C++ bindings) and the UX specification in `.dev-team/ux/blackjack-web-app.md`.

### JavaScript Style

- ES6 modules
- JSDoc comments for documentation
- No TypeScript (vanilla JS only)
- Consistent naming: camelCase for functions/variables, PascalCase for classes

### CSS Style

- CSS custom properties (CSS variables) for design tokens
- Mobile-first responsive design
- BEM-like naming for component classes

## Architecture

The web frontend uses a clean separation of concerns:

1. **bindings.cpp** - Exposes C++ Game API to JavaScript via Embind
2. **main.js** - Application entry point, event handling, game loop
3. **game-controller.js** - Wraps WASM Game instance, provides JS-friendly state snapshots
4. **ui-renderer.js** - Reads state and updates DOM
5. **card-renderer.js** - Generates card HTML elements

The game state flows one direction: WASM → GameController → UIRenderer → DOM.

## Testing

No automated tests for the frontend in this phase. Manual testing checklist:

- [ ] Game loads without errors
- [ ] Initial deal works
- [ ] Hit/Stand actions work
- [ ] Split works for pairs
- [ ] Dealer turn plays automatically
- [ ] Results display correctly
- [ ] New Game button works
- [ ] Keyboard shortcuts work
- [ ] Responsive layout works on mobile/tablet/desktop
- [ ] Accessibility: keyboard navigation works
- [ ] Accessibility: screen reader announces state changes

## Known Limitations (MVP)

- No card animations (instant card appearance, no slide/fade)
- No dealer draw animation (dealer cards appear instantly after play_dealer call)
- No session persistence (no localStorage, game resets on page reload)
- No multiplayer
- No betting system
