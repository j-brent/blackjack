# Blackjack Web Frontend

See [../docs/frontends.md](../docs/frontends.md#web-emscriptenwasm) for build instructions, source layout, architecture notes, and known gotchas.

See [../README.md](../README.md) for the library API and project-wide conventions.

## Quick reference

```sh
# Build WASM
emcmake cmake -B build -S . -G Ninja
cmake --build build

# Serve
python -m http.server -d app 8080

# Test
npm test
```
