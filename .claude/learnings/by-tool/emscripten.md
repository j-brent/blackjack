# Emscripten Learnings

## [2026-01-31] Lesson: Embind enum values are wrapper objects, not raw integers
**ID**: c7d8e9f0
**Category**: tool
**Context**: When exposing C++ enum classes to JavaScript via Emscripten Embind
**Learning**: Embind wraps C++ enum values in JavaScript objects. Accessing an enum property (e.g., `card.rank`) returns `{value: 2}`, not `2`. Always use `.value` to extract the numeric value (e.g., `card.rank.value`). Forgetting `.value` produces `[object Object]` in string contexts and silent failures in numeric comparisons. Document this for frontend consumers when writing bindings.
**Evidence**: Web frontend showed "?" for all card ranks and "[object Object]" for dealer value. Root cause was `game-controller.js` accessing `card.rank` and `card.suit` directly instead of `card.rank.value` and `card.suit.value`. Same issue affected `handState.result`, `state`, and `action` enum accesses.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team

## [2026-01-31] Lesson: Emscripten CMake -s flags must have no space
**ID**: d8e9f0a1
**Category**: tool
**Context**: When using CMake `target_link_options` with Emscripten `-s` flags
**Learning**: Use `-sMODULARIZE=1` (no space), not `-s MODULARIZE=1`. CMake's `target_link_options` treats space-separated values as separate list items, splitting `-s MODULARIZE=1` into `-s` and `MODULARIZE=1` as two independent arguments. This causes `em++: error: MODULARIZE=1: No such file or directory` or similar linker errors. The same applies to all Emscripten `-s` settings: `-sEXPORT_ES6=1`, `-sENVIRONMENT=web`, `-sALLOW_MEMORY_GROWTH=1`, etc.
**Evidence**: Initial CMake build failed with cryptic Emscripten errors. The generated CMakeLists.txt used `-s MODULARIZE=1` syntax which CMake split into separate arguments.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team

## [2026-01-31] Lesson: Separate native source from servable content in WASM projects
**ID**: e9f0a1b2
**Category**: tool
**Context**: When structuring a project that compiles native code (C/C++) to WebAssembly for browser consumption
**Learning**: Never mix native source files (C++, Rust, etc.) with JavaScript/web files in the same directory. Use a clear structure: build config (`CMakeLists.txt`, `package.json`) at project root, native source in a language-specific directory (e.g., `cpp/`), all browser-servable content under a single directory (e.g., `app/`) that becomes the HTTP server root, WASM build output inside the servable directory (e.g., `app/build/`), and tests outside the servable directory. Use `RUNTIME_OUTPUT_DIRECTORY` in CMake `set_target_properties` to direct WASM output into the servable directory.
**Evidence**: Initial project structure placed `bindings.cpp` alongside `.js` files in `src/`, and `index.html`/`style.css` at the project root mixed with `CMakeLists.txt`. Required full restructuring into `cpp/`, `app/`, `app/src/`, `app/build/` layout.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team
