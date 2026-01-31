# Building Blackjack Web (WASM)

## Prerequisites

- Emscripten SDK (emsdk) installed and activated
- CMake 3.20+

## Build Instructions

```sh
# Configure with Emscripten toolchain
emcmake cmake -B blackjack/web/build -S blackjack/web

# Build
cmake --build blackjack/web/build

# Output files:
# - blackjack/web/build/blackjack.wasm
# - blackjack/web/build/blackjack.js
```

## Testing Locally

WASM modules require HTTP serving (not `file://` URLs).

Option 1: Using emrun (included with Emscripten):
```sh
emrun blackjack/web/index.html
```

Option 2: Using Python:
```sh
python -m http.server -d blackjack/web 8080
# Then open http://localhost:8080
```

## Build Output

- `blackjack.wasm`: Compiled WebAssembly module (~50-150 KB)
- `blackjack.js`: Emscripten-generated glue code (~30-80 KB)

Both files are placed in `blackjack/web/build/` and referenced by `index.html`.
