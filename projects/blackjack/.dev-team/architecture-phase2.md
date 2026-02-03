<!-- metadata
id: ARCH-002
type: architecture
status: in-review
author: A2-architect
created: 2026-01-31
updated: 2026-01-31
approved-by: pending
dependencies: [VISION-001, ARCH-001, API-001, DATA-001]
-->

# Architecture: Blackjack Web Frontend (Phase 2)

## 1. Overview

Phase 2 adds a web browser frontend to the blackjack project. The existing C++ library (`blackjack_lib`) is compiled to WebAssembly using Emscripten, exposed to JavaScript via Embind, and driven by a vanilla JavaScript single-page application. The web frontend follows the same architectural pattern as CLI and FTXUI: the frontend drives the game loop, the library is passive, and all state transitions are explicit.

The web frontend lives in `web/` and produces a self-contained set of static files (HTML, JS, WASM, CSS) that can be opened directly in a browser or served from any static file server.

## 2. System Context

```
+-------------------------------------------------------+
|                     Monorepo                           |
|                                                        |
|  +------------------------+                            |
|  |  libblackjack          |  Static library            |
|  |  (Game Engine)         |  Pure C++ -- no I/O        |
|  +----------+-------------+                            |
|             | compiled by                              |
|       +-----+------+-----+-------+                     |
|       |            |     |       |                     |
|  +----v----+ +-----v---+ +------v------+               |
|  |  CLI    | |  FTXUI  | |    Web      |               |
|  | (exe)   | |  (exe)  | | (WASM+JS)  |               |
|  +---------+ +---------+ +------+------+               |
|                                  |                     |
|                          +-------v--------+            |
|                          | Browser Runtime |            |
|                          | (HTML/CSS/JS)  |            |
|                          +----------------+            |
+-------------------------------------------------------+
```

The browser loads `index.html`, which loads the Emscripten-generated JS glue code, which instantiates the WASM module. JavaScript creates a `Game` instance through Embind bindings, calls commands, reads state, and updates the DOM.

## 3. Component Architecture

### 3.1 C++ Binding Layer (`web/src/bindings.cpp`)

A single C++ file that uses Emscripten Embind to expose the Game class and all supporting types to JavaScript. This layer does NOT add any logic -- it is a pure translation layer.

**Responsibilities**:
- Bind all `enum class` types (Suit, Rank, GameState, HandResult, ActionResult, PlayerAction) as Embind enums
- Bind the `Card` struct with `rank` and `suit` properties
- Bind the `Hand` class with `cards()`, `value()`, `is_soft()`, `is_bust()`, `is_natural_blackjack()`, `can_split()`, `size()`
- Bind the `HandState` struct with `hand`, `result`, `is_stood`, `is_from_split`, `is_split_aces`
- Bind the `Game` class with all commands and queries
- Convert `std::span` and `std::vector` returns to Embind-compatible types (Embind `register_vector` for `std::vector`, and wrapper functions that return `std::vector` copies for `std::span`-returning methods)

**Key design choice**: `std::span<const HandState>` from `player_hands()` and `std::span<const Card>` from `Hand::cards()` cannot be directly bound via Embind. The binding layer provides wrapper functions that copy the span into a `std::vector` and return that. This is a negligible performance cost for the small data sizes involved (at most 2 hands, at most ~10 cards per hand).

### 3.2 Emscripten Build (`web/CMakeLists.txt`)

A CMake target that compiles `blackjack_lib` source files plus `bindings.cpp` using `emcmake`/`em++` to produce:
- `blackjack.wasm` -- the compiled WebAssembly module
- `blackjack.js` -- Emscripten-generated glue code that loads and instantiates the WASM module

**Build approach**: The web target is built using a separate Emscripten CMake toolchain invocation, NOT as a subdirectory of the main CMakeLists.txt. This is because Emscripten requires its own toolchain file and compiler, which is incompatible with the native compiler used for CLI/FTXUI/tests.

### 3.3 JavaScript Application (`web/src/`)

Vanilla JavaScript (no frameworks, no bundler, no transpiler) structured as ES modules.

| Module | Responsibility |
|--------|---------------|
| `game-controller.js` | Owns the Game WASM instance. Translates WASM API calls into JavaScript-friendly objects. Handles the game state machine. |
| `ui-renderer.js` | Reads game state from the controller and updates the DOM. Renders cards, hands, values, buttons, status messages. |
| `card-renderer.js` | Generates SVG or HTML elements for individual playing cards. Handles suit colors, face-down cards. |
| `main.js` | Entry point. Waits for WASM module to load, creates game controller, wires up event listeners, starts first deal. |

### 3.4 HTML/CSS (`web/`)

| File | Purpose |
|------|---------|
| `index.html` | Single-page shell. Contains the layout structure (dealer area, player area, controls, status bar). Loads the JS modules. |
| `style.css` | All styling. Responsive layout using CSS Grid or Flexbox. Card visual design. Color palette. |

### Component Descriptions

| Component | Responsibility | Technology | Owner Agent |
|-----------|---------------|------------|-------------|
| Embind bindings | Expose C++ Game API to JS | C++ / Emscripten Embind | A6 (C++ dev) |
| CMake build | Compile to WASM, produce output files | CMake + Emscripten toolchain | A6 (C++ dev) |
| Game controller | JS-side game state management | Vanilla JS (ES modules) | A7 (JS dev) |
| UI renderer | DOM manipulation, layout updates | Vanilla JS + DOM API | A7 (JS dev) |
| Card renderer | Card visual generation | HTML/CSS or SVG | A7 (JS dev) |
| HTML shell | Page structure | HTML5 | A7 (JS dev) |
| CSS styling | Visual design, responsive layout | CSS3 | A7 (JS dev) |

## 4. Technology Stack

### 4.1 Chosen Technologies

| Layer | Technology | Version | Rationale |
|-------|-----------|---------|-----------|
| C++ compilation to WASM | Emscripten | 3.1.x (latest stable) | The only mature C++-to-WASM compiler. Active development, wide adoption. |
| C++/JS binding | Embind | (part of Emscripten) | Type-safe, supports classes/enums/structs directly. Less boilerplate than extern "C". |
| JavaScript | Vanilla ES modules | ES2020+ | No framework required -- the UI is a single page with ~6 interactive elements. Adding React/Vue would be overengineering. |
| Styling | Plain CSS3 | N/A | Flexbox/Grid for layout. No preprocessor needed for this scope. |
| Build system | CMake + Emscripten toolchain | CMake 3.20+ | Consistent with the rest of the project. Emscripten provides a CMake toolchain file. |
| Dev server (optional) | Python `http.server` or `emrun` | N/A | WASM requires HTTP serving (not `file://`). `emrun` is included with Emscripten. |

### 4.2 Technologies Considered but Rejected

| Technology | Reason for Rejection |
|-----------|---------------------|
| React / Vue / Svelte | Vision statement specifies vanilla JS. The UI complexity does not justify a framework -- there are ~6 interactive elements and a simple state machine. |
| `extern "C"` wrapper instead of Embind | Would require manually flattening all C++ types to C primitives (int, char*), then re-wrapping in JS. Embind handles classes, enums, vectors, and strings natively. The cost is a slightly larger WASM binary (~10-20KB overhead), which is acceptable. |
| wasm-bindgen (Rust ecosystem tool) | Not applicable -- this is a C++ project. |
| WebAssembly System Interface (WASI) | WASI is for server-side/headless WASM. We need browser integration (DOM access from JS, not from WASM). Emscripten is the correct choice for browser targets. |
| TypeScript | Would require a build step (tsc or bundler). The project scope is small enough that vanilla JS with JSDoc comments provides sufficient type documentation without a transpilation step. |
| Webpack / Vite / esbuild | No bundler is needed. ES modules with `<script type="module">` work natively in all modern browsers. The JS codebase is ~4-5 files. |

## 5. Data Flow

### 5.1 WASM Module Loading

```
Browser loads index.html
  -> index.html loads main.js (ES module)
    -> main.js imports Emscripten glue (blackjack.js)
      -> Emscripten glue fetches blackjack.wasm
        -> WASM module is compiled and instantiated
          -> Embind bindings register JS constructors for Game, Card, Hand, etc.
            -> main.js receives the Module object
              -> Game controller creates a new Game() instance
                -> First deal is called, UI renders initial state
```

### 5.2 Player Action Flow

```
User clicks "Hit" button (or presses 'H' key)
  -> Event listener in main.js fires
    -> game-controller.js calls game.hit()
      -> Embind translates JS call to C++ Game::hit()
        -> C++ game engine processes hit, returns ActionResult
      -> Embind translates return value to JS enum
    -> game-controller.js reads new state: game.state(), game.player_hands(), etc.
      -> Embind translates C++ types to JS objects
    -> game-controller.js checks if state is DealerTurn, calls play_dealer() if so
    -> ui-renderer.js receives state snapshot, updates DOM
      -> card-renderer.js generates card HTML/SVG for changed cards
      -> DOM is updated (dealer area, player area, status bar, button visibility)
```

### 5.3 State Query Flow

```
game-controller.js calls game.state()
  -> Returns GameState enum value (JS number)
game-controller.js calls game.player_hands()
  -> Embind wrapper copies std::span<HandState> to std::vector<HandState>
  -> Embind converts vector to JS array of HandState objects
  -> Each HandState contains a Hand object (with cards() returning Card array)
  -> Each Card has .rank and .suit as enum values
game-controller.js calls game.dealer_hand()
  -> Returns Hand object with cards(), value(), is_soft(), etc.
```

## 6. Build Architecture

### 6.1 Directory Structure

```
blackjack/
+-- CMakeLists.txt              # Root -- does NOT add web/ subdirectory
+-- lib/                        # Unchanged
+-- cli/                        # Unchanged
+-- ftxui/                      # Unchanged
+-- tests/                      # Unchanged
+-- web/
    +-- CMakeLists.txt          # Emscripten build for WASM + bindings
    +-- src/
    |   +-- bindings.cpp        # Embind binding definitions
    |   +-- main.js             # JS entry point
    |   +-- game-controller.js  # Game state management
    |   +-- ui-renderer.js      # DOM rendering
    |   +-- card-renderer.js    # Card HTML/SVG generation
    +-- index.html              # HTML shell
    +-- style.css               # Stylesheet
    +-- build/                  # Build output (gitignored)
        +-- blackjack.wasm      # Compiled WASM module
        +-- blackjack.js        # Emscripten glue code
```

### 6.2 Build Commands

The web target uses a separate CMake invocation with the Emscripten toolchain:

```sh
# One-time: ensure Emscripten SDK is installed and activated
# (emsdk install latest && emsdk activate latest)

# Configure (from projects/blackjack/ directory)
emcmake cmake -B web/build -S web

# Build
cmake --build web/build

# Output: web/build/blackjack.wasm, web/build/blackjack.js

# Serve for testing (WASM requires HTTP, not file://)
emrun web/index.html
# OR
python -m http.server -d web 8080
```

### 6.3 Build Output Placement

The CMake build places `blackjack.wasm` and `blackjack.js` into `web/build/`. The `index.html` references these via relative paths. For development, serve from `web/`. For distribution, copy `index.html`, `style.css`, `src/*.js`, and `build/blackjack.{js,wasm}` as a flat set of static files.

### 6.4 Why web/ Is NOT a Subdirectory in Root CMakeLists.txt

The root `CMakeLists.txt` uses the system's native C++ compiler (GCC/Clang/MSVC). Emscripten requires its own toolchain file (`$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake`). Mixing native and Emscripten targets in a single CMake invocation is not supported by CMake. Therefore, `web/` has its own standalone CMakeLists.txt that is configured independently via `emcmake cmake`.

The root `CMakeLists.txt` does NOT `add_subdirectory(web)`. This is by design, not an oversight.

## 7. Emscripten Configuration

### 7.1 Compiler/Linker Flags

| Flag | Purpose |
|------|---------|
| `-s MODULARIZE=1` | Wraps output in a module factory function instead of polluting global scope |
| `-s EXPORT_ES6=1` | Generates an ES6 module (compatible with `import` syntax) |
| `-s ENVIRONMENT=web` | Target browser environment only (not Node.js) |
| `-s NO_EXIT_RUNTIME=1` | Keep runtime alive after main() (needed for persistent Game objects) |
| `-s ALLOW_MEMORY_GROWTH=1` | Allow heap to grow (std::vector allocations) |
| `--bind` | Enable Embind |
| `-s DISABLE_EXCEPTION_CATCHING=1` | Disable C++ exception support (per ADR-3, no exceptions in API) |
| `-O2` | Optimization level for release builds |
| `-s WASM=1` | Output WASM (default, but explicit for clarity) |
| `-s EXPORTED_RUNTIME_METHODS=['UTF8ToString']` | Export minimal runtime methods needed |

### 7.2 Expected Output Sizes

Based on the library size (~500 lines of C++) and Embind overhead:
- `blackjack.wasm`: estimated 50-150 KB (with -O2 and no exceptions)
- `blackjack.js` (glue): estimated 30-80 KB

These are acceptable for a web application. Total payload under 250 KB uncompressed, under 80 KB gzipped.

### 7.3 `std::random_device` Under Emscripten

Emscripten's `std::random_device` uses JavaScript's `crypto.getRandomValues()` as the entropy source. The default `Game()` constructor (which seeds from `std::random_device`) works correctly under Emscripten without modification. No special handling needed.

## 8. Key Architectural Decisions

> **DECISION [2026-01-31]**: Use Embind, not extern "C" wrappers
> **Options considered**: (1) Embind, (2) extern "C" flat API, (3) WebIDL binder
> **Rationale**: Embind directly supports C++ classes, enum classes, std::vector, and std::string. It generates JS-friendly APIs with minimal boilerplate. An extern "C" approach would require flattening every type to C primitives, manually managing memory for arrays, and writing JS wrapper code to reconstruct objects. WebIDL binder is less flexible and less documented than Embind. The ~15KB binary size overhead of Embind is negligible.
> **Decided by**: A2-architect

> **DECISION [2026-01-31]**: Separate CMake invocation for web/, not add_subdirectory
> **Options considered**: (1) Separate invocation with emcmake, (2) ExternalProject_Add from root, (3) add_subdirectory with toolchain detection
> **Rationale**: CMake does not support mixing toolchains in a single invocation. ExternalProject_Add would work but adds complexity for a single extra target. A separate `emcmake cmake` invocation is the simplest, most explicit approach and matches Emscripten's official documentation.
> **Decided by**: A2-architect

> **DECISION [2026-01-31]**: Vanilla JS with ES modules, no framework, no bundler
> **Options considered**: (1) Vanilla JS, (2) Vanilla JS + Vite for dev server, (3) Lit (minimal web components), (4) Preact (tiny React alternative)
> **Rationale**: The vision statement explicitly requires vanilla JavaScript. The UI consists of ~6 interactive elements (hit, stand, split, new game, quit/reset buttons + card display). This does not warrant a framework. ES modules (`<script type="module">`) are natively supported in all browsers that support WebAssembly. No build step for JS means faster iteration and simpler toolchain.
> **Decided by**: A2-architect (product scope confirmed by VISION-001)

> **DECISION [2026-01-31]**: Copy std::span to std::vector in Embind wrappers
> **Options considered**: (1) Copy to vector, (2) Expose raw pointer + length, (3) Custom Embind type converter
> **Rationale**: Embind does not natively support std::span. Copying to std::vector is trivially correct, and the data is tiny (at most ~10 cards). A custom converter would be premature optimization for no measurable benefit. Raw pointer + length would require manual memory management from JS.
> **Decided by**: A2-architect

> **DECISION [2026-01-31]**: Use MODULARIZE=1 and EXPORT_ES6=1
> **Options considered**: (1) Modular ES6, (2) Global script, (3) CommonJS module
> **Rationale**: ES6 module output allows clean `import` in main.js. Non-modular output pollutes the global scope and requires careful script loading order. CommonJS is for Node.js.
> **Decided by**: A2-architect

## 9. Non-Functional Requirements

### 9.1 Performance

| Metric | Target | Measurement Method |
|--------|--------|-------------------|
| WASM load time | Under 2 seconds on 3G connection | Chrome DevTools network throttling |
| Action response time | Under 50ms from click to DOM update | Chrome DevTools Performance panel |
| Total payload size | Under 300 KB uncompressed, under 100 KB gzipped | `ls -la` on build output, `gzip -9` measurement |

### 9.2 Browser Compatibility

| Browser | Minimum Version | Rationale |
|---------|----------------|-----------|
| Chrome | 57+ | WebAssembly + ES modules support |
| Firefox | 53+ | WebAssembly + ES modules support |
| Safari | 11+ | WebAssembly + ES modules support |
| Edge | 79+ (Chromium) | WebAssembly + ES modules support |

No support for Internet Explorer. No polyfills.

### 9.3 Accessibility

- All buttons have visible text labels and keyboard shortcuts
- Card information is conveyed through text (rank + suit name), not just visual appearance
- Status messages use text, not just color
- Keyboard navigation: all actions accessible via keyboard shortcuts (H, S, P, N, Escape)
- ARIA attributes on interactive elements for screen reader compatibility
- Color contrast ratio of at least 4.5:1 for all text against background (WCAG AA)

### 9.4 Responsiveness

- Minimum viewport: 360px wide (mobile portrait)
- Cards scale down on narrow viewports
- Button layout stacks vertically below 480px width
- No horizontal scrolling at any supported viewport size

## 10. Deployment

The web frontend is a set of static files. No server-side logic.

| Environment | Method | Notes |
|------------|--------|-------|
| Development | `emrun` or `python -m http.server` from `web/` | WASM requires HTTP serving |
| Distribution | Copy static files to any web server or CDN | Zero server configuration needed |
| Local file | Not supported | Browsers block WASM loading from `file://` URLs |

## 11. Constraints

- **Emscripten SDK required**: The web build requires the Emscripten SDK installed and activated. This is a development-time dependency, not a runtime dependency.
- **No file:// support**: WebAssembly modules cannot be loaded from local file paths in most browsers due to security restrictions. A local HTTP server is required for development.
- **No exceptions**: Per ADR-3, the C++ library does not throw exceptions. The Emscripten build disables exception support entirely (`DISABLE_EXCEPTION_CATCHING=1`), reducing binary size.
- **No threads**: The library is single-threaded. The web frontend is single-threaded (main thread). No SharedArrayBuffer or Web Workers needed.
- **No C++ I/O**: The library has no `#include <iostream>`. Emscripten does not need to provide filesystem or I/O shims.

## 12. Code Quality Tooling

### 12.1 C++ (bindings.cpp)

- **clang-format**: Same configuration as the rest of the project (`.clang-format` at repo root)
- **clang-tidy**: Same configuration, but some Emscripten-specific macros may require suppression comments

### 12.2 JavaScript

- **Formatter**: Prettier (config file in `web/` or project root)
- **Linter**: ESLint with a minimal config (no-unused-vars, no-undef, consistent style)
- **No TypeScript**: JSDoc type annotations in comments for documentation purposes

### 12.3 CSS

- **Formatter**: Prettier (handles CSS formatting)

### 12.4 HTML

- **Validation**: `html-validate` (npm) or `vnu-jar` (W3C Nu HTML Checker CLI), run as part of the pre-commit or build check

## 13. References

- [ARCH-001](architecture.md) -- Phase 1 architecture (ADR-1 through ADR-5 apply)
- [API-001](api-contracts.md) -- C++ library API contract
- [DATA-001](data-model.md) -- C++ library data model
- [VISION-001](vision-statement.md) -- Product vision (Phase 2 scope)
- [Emscripten Embind documentation](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html)
- [Emscripten CMake documentation](https://emscripten.org/docs/compiling/Building-Projects.html#using-libraries)
