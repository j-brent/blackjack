# Blackjack Web Frontend

Single-page vanilla JS app using the C++ blackjack library compiled to WebAssembly via Emscripten.

## Source Layout

- `cpp/` — C++ Embind bindings (`bindings.cpp`)
- `app/` — Servable content (HTTP server root)
  - `index.html` — HTML shell
  - `style.css` — Stylesheet (rem/em units, 100dvh layout)
  - `src/` — ES module JS files
    - `main.js` — Entry point, event wiring, game loop
    - `game-controller.js` — Wraps WASM Game instance, converts Embind types to plain JS
    - `ui-renderer.js` — DOM rendering (dealer area, player area, buttons, status bar)
    - `card-renderer.js` — Card element generation, rank/suit display mapping
  - `build/` — WASM output (generated, gitignored)
- `tests/` — Playwright acceptance tests
- `CMakeLists.txt` — Emscripten CMake config (standalone, not part of root CMakeLists.txt)
- `package.json` — npm config for Playwright dev dependency

## Prerequisites

- Emscripten SDK (emsdk) installed and activated
- Node.js (for Playwright tests)
- Ninja build system

## Build WASM

```sh
# From blackjack/web/
emcmake cmake -B build -S . -G Ninja
cmake --build build
```

Output lands in `app/build/` (blackjack.js + blackjack.wasm).

On Windows with Git Bash where `emcmake` isn't on PATH:

```sh
/c/dev/emsdk/python/3.13.3_64bit/python.exe /c/dev/emsdk/upstream/emscripten/emcmake.py cmake -B build -S . -G Ninja
cmake --build build
```

## Serve

```sh
python -m http.server -d app 8080
```

Open http://localhost:8080 in a browser.

## Run Tests

```sh
# Install dependencies (once)
npm install
npx playwright install chromium

# Start server in background, then run tests
python -m http.server -d app 8080 &
npm test
```

Tests require the app to be served at http://localhost:8080.

### Acceptance Tests (tests/layout.test.js)

- AT-1 through AT-3: No scroll required at 360x640, 390x844, 414x896
- AT-4: All game elements visible within viewport at 360x640
- AT-5: No px units in CSS design token custom properties
- AT-6: No horizontal overflow at 200% zoom
- AT-7: Split hands fit at 360x640

## Architecture Notes

- **No frameworks** — Vanilla JS with ES modules, no bundler
- **Embind enums return objects** — Always access `.value` to get the numeric value (e.g., `card.rank.value`, not `card.rank`)
- **Frontend drives the game loop** — C++ library is a passive state machine. JS calls `deal()`, `hit()`, `stand()`, `split()`, `play_dealer()` and polls `getState()`
- **CSS units** — Spacing/typography use `rem`, card sizes use `em` (scale with container font-size). No `px` in design tokens.
- **Layout** — `#game-container` uses `height: 100dvh` with flexbox. Dealer/player areas share remaining space via `flex: 1`.
- **Keyboard shortcuts** — H (hit), S (stand), P (split), N (new game)
