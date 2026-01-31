# Frontends

All frontends link against the same `blackjack_lib` static library and drive the game loop themselves. The library is passive — frontends call commands, poll state, and render.

## CLI

Text-based terminal frontend. Reads player input via `stdin`, renders game state to `stdout`.

```sh
./build/cli/blackjack_cli.exe
```

No external dependencies beyond the C++ standard library.

## TUI (FTXUI)

Interactive terminal UI built with [FTXUI](https://github.com/ArthurSonzogni/FTXUI) v5.0.0. Renders card graphics, handles keyboard input via FTXUI's event loop.

```sh
./build/ftxui/blackjack_tui.exe
```

FTXUI is fetched automatically via CMake `FetchContent`.

## Web (Emscripten/WASM)

Single-page vanilla JS app using the C++ library compiled to WebAssembly via Emscripten Embind.

### Source layout

```
web/
├── cpp/            C++ Embind bindings (bindings.cpp)
├── app/            Servable content (HTTP server root)
│   ├── index.html
│   ├── style.css   rem/em units, 100dvh layout
│   ├── src/        ES module JS files
│   │   ├── main.js             Entry point, event wiring
│   │   ├── game-controller.js  Wraps WASM Game, converts Embind types to plain JS
│   │   ├── ui-renderer.js      DOM rendering
│   │   └── card-renderer.js    Card element generation
│   └── build/      WASM output (generated, gitignored)
├── tests/          Playwright acceptance tests
├── CMakeLists.txt  Emscripten CMake config (standalone, not part of root CMake)
└── package.json    npm config for Playwright
```

### Prerequisites

- Emscripten SDK (emsdk) installed and activated
- Node.js (for Playwright tests)
- Ninja build system

### Build

```sh
cd web
emcmake cmake -B build -S . -G Ninja
cmake --build build
```

Output lands in `app/build/` (blackjack.js + blackjack.wasm).

On Windows with Git Bash where `emcmake` isn't on PATH:

```sh
/c/dev/emsdk/python/3.13.3_64bit/python.exe /c/dev/emsdk/upstream/emscripten/emcmake.py cmake -B build -S . -G Ninja
cmake --build build
```

### Serve

```sh
python -m http.server -d app 8080
```

Open http://localhost:8080.

### Tests

```sh
npm install
npx playwright install chromium
python -m http.server -d app 8080 &
npm test
```

Acceptance tests (in `tests/layout.test.js`):
- AT-1 through AT-3: No scroll required at 360x640, 390x844, 414x896
- AT-4: All game elements visible within viewport at 360x640
- AT-5: No px units in CSS design token custom properties
- AT-6: No horizontal overflow at 200% zoom
- AT-7: Split hands fit at 360x640

### Architecture notes

- No frameworks — vanilla JS with ES modules, no bundler
- Embind enums return objects — always access `.value` (e.g., `card.rank.value`, not `card.rank`)
- CSS units: `rem` for spacing/typography, `em` for card sizes, no `px` in design tokens
- Layout: `#game-container` uses `height: 100dvh` with flexbox
- Keyboard shortcuts: H (hit), S (stand), P (split), N (new game)

### Known gotchas

- **Embind enums are objects, not integers.** Use `.value` to get the numeric value. Forgetting `.value` produces `[object Object]` in string contexts.
- **Emscripten CMake `-s` flags must have no space.** Use `-sMODULARIZE=1`, not `-s MODULARIZE=1`. CMake splits on spaces.
- **WASM output directory.** Use `RUNTIME_OUTPUT_DIRECTORY` in `set_target_properties` to place output into `app/build/`.

## Electron

Desktop wrapper around the web frontend using [Electron](https://www.electronjs.org/).

```sh
cd electron
npm install
npm start
```

Loads the web frontend's `app/` directory in a native window. Requires the web frontend to be built first (`web/app/build/` must contain the WASM output).
