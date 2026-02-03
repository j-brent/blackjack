<!-- metadata
id: RISK-001
type: technical-risks
status: draft
author: A2-architect
created: 2026-01-30
updated: 2026-01-30
approved-by: pending
dependencies: [ARCH-001]
-->

# Technical Risks

## Phase 1 Risks

### RISK-P1-01: FTXUI Learning Curve

**Risk**: The team has no prior experience with FTXUI. The library's component model (functional reactive style) differs from typical imperative UI code. Misunderstanding the render/event loop could lead to flickering, unresponsive UI, or architectural mistakes that require rework.

**Likelihood**: Medium
**Impact**: Medium (delays, suboptimal UI code)

**Mitigation**:
- Spend 30-60 minutes with FTXUI examples before starting the frontend.
- Start with a minimal FTXUI app (static text, one button) and incrementally add features.
- Use FTXUI's `ScreenInteractive::FitComponent()` or `FullscreenAlternateScreen()` — choose early and stick with it.

### RISK-P1-02: FTXUI Cross-Platform Unicode/Color Rendering

**Risk**: FTXUI renders Unicode box-drawing characters and ANSI colors. These may render differently across terminals: Windows Terminal vs. ConEmu vs. cmd.exe, macOS Terminal.app vs. iTerm2, Linux xterm vs. gnome-terminal. Card visuals designed on one platform may look broken on another.

**Likelihood**: Medium
**Impact**: Low-Medium (visual glitches, not functional breakage)

**Mitigation**:
- Test on at least two terminals (Windows Terminal + one Linux terminal).
- Use only widely supported Unicode (box-drawing: U+2500 block, card suits: U+2660 block). Avoid exotic characters.
- Have an ASCII fallback in mind if suit symbols don't render.

### RISK-P1-03: CMake + FetchContent Complexity

**Risk**: FetchContent for FTXUI can be slow (downloads and builds FTXUI from source on first configure). If FTXUI's CMake configuration is incompatible with the project's settings (C++ standard, compiler flags), debugging CMake errors can burn significant time.

**Likelihood**: Medium
**Impact**: Medium (build system debugging is high-friction, time-consuming)

**Mitigation**:
- Pin FTXUI to a specific release tag (e.g., `v5.0.0`), not `main`.
- Set the C++ standard at the root CMakeLists.txt level before FetchContent.
- Keep FTXUI FetchContent in the `ftxui/CMakeLists.txt` subdirectory, isolated from the library build.
- Test the build on a clean checkout early.

### RISK-P1-04: API Design Blind Spots

**Risk**: The library API is designed in Phase 1 for CLI and FTXUI consumers. These are both C++ and both have synchronous access patterns. The API might accidentally bake in assumptions that break for async consumers (WASM with JS callbacks) or non-C++ consumers (Python).

**Likelihood**: Low-Medium
**Impact**: High (API changes in later phases ripple through all existing frontends)

**Mitigation**:
- The API is deliberately simple: ~10 methods, command/query separation, no callbacks, no I/O. This limits the surface area for bad assumptions.
- `play_dealer()` is a separate call (not automatic inside `stand()`), giving frontends control over timing — important for WASM where you'd want to yield between dealer draws.
- Review the API against WASM and Python integration patterns before finalizing (mental exercise, not code).

### RISK-P1-05: Scope Creep Within Phase 1

**Risk**: The 6-hour budget is tight for backend library + tests + CLI + FTXUI frontend. Feature creep (adding double-down, insurance, fancy animations, comprehensive test coverage) could blow the budget.

**Likelihood**: Medium
**Impact**: High (missed deadline, half-finished deliverables)

**Mitigation**:
- The spec explicitly excludes double-down, insurance, and betting. Enforce this.
- Prioritize: library + tests first, CLI second, FTXUI third. If time runs short, FTXUI can be minimal.
- "Split" is the most complex feature. If it threatens the budget, implement it after the basic flow works.

### RISK-P1-06: Soft 17 and Split Aces Edge Cases

**Risk**: Soft ace logic and split aces are the trickiest rules. Bugs here are subtle — a hand might be evaluated as soft when it shouldn't be, or split aces might allow further hits when they shouldn't. These bugs are easy to write and hard to spot without targeted tests.

**Likelihood**: Medium
**Impact**: Medium (incorrect game behavior, subtle)

**Mitigation**:
- Write acceptance tests for every edge case in SPEC-001 before implementing.
- Use deterministic seeding to create specific card sequences that exercise soft ace transitions (ace goes from 11 to 1 when a card pushes past 21).
- Test split aces specifically: ensure exactly one card dealt, no further hits allowed.

---

## Future Phase Risks (Flagged Now for Awareness)

### RISK-F-01: WASM Compilation Constraints (Phase 2)

**Risk**: Emscripten compiles C++ to WASM but imposes constraints: no threads (without SharedArrayBuffer), no exceptions (unless enabled at cost), limited filesystem, no blocking I/O. If the library uses features Emscripten struggles with (e.g., `<random>` might need entropy source configuration, exceptions for error handling), the WASM build may fail or produce oversized output.

**Likelihood**: Medium
**Impact**: High (may require library changes)

**Mitigation (Phase 1 decisions that help)**:
- No exceptions in the public API (using `ActionResult` enum instead).
- No I/O, no filesystem, no threading in the library.
- `std::mt19937` works under Emscripten, but seeding via `std::random_device` may need a JS-provided seed. The seedable constructor already handles this.

### RISK-F-02: Python Binding Complexity (Phase 3)

**Risk**: pybind11/nanobind need to wrap C++ types. `std::span` is not natively supported by pybind11 — it would need conversion to a list. `enum class` requires explicit binding. If the API has too many types or complex signatures, the binding layer becomes a maintenance burden.

**Likelihood**: Low-Medium
**Impact**: Medium

**Mitigation (Phase 1 decisions that help)**:
- Small API surface (~10 methods, ~6 enums, ~4 types). Binding is mechanical.
- Avoid `std::span` in the Python-facing layer — provide a method that returns `std::vector` copy. This can be a separate binding function that delegates to the `std::span` method internally.
- Keep types simple: POD structs, enum classes, no templates in the public API.

### RISK-F-03: Qt Integration Patterns (Phase 4)

**Risk**: Qt has its own type system (QString, QVector, QObject, signals/slots). Integrating a pure C++ library requires translation between Qt types and standard C++ types. If the library's API returns `std::vector<HandState>`, the Qt frontend needs to iterate and convert for display. This is fine but tedious. The bigger risk is whether the Qt frontend should use QML (declarative, needs Q_PROPERTY) or Qt Widgets (imperative, simpler integration).

**Likelihood**: Low
**Impact**: Medium

**Mitigation**: Defer decision to Phase 4. The library API works with any C++ consumer regardless of Qt's type system.

### RISK-F-04: Mobile Platform Uncertainty (Phase 5)

**Risk**: Phase 5 ("mobile frontend") is undefined. Options include Qt for mobile, React Native + WASM, or native iOS/Android with C++ core via JNI/Objective-C++. Each has radically different integration patterns. Choosing wrong could waste an entire phase.

**Likelihood**: High (because it's fully undefined)
**Impact**: High

**Mitigation**: Do not plan for Phase 5 until Phases 2-4 are complete. Learnings from those phases will inform the mobile strategy. The library's zero-dependency, no-I/O design at least ensures it can compile on mobile platforms.

### RISK-F-05: API Stability Across Phases

**Risk**: As new frontends are added, they may need API methods that don't exist yet. Adding methods is fine; changing existing method signatures or semantics breaks all existing frontends. If Phase 1's API is too minimal, later phases may need breaking changes.

**Likelihood**: Low-Medium
**Impact**: High (breaking changes cascade)

**Mitigation**:
- Design the Phase 1 API to expose ALL game state, even if CLI/FTXUI don't use every method. Better to have `dealer_up_card()` available and unused than to add it later.
- Use the state transition table (API-001 Section 6) as the contract. New methods can be added, but existing transitions must not change.
- Semantic versioning on the library, even within the monorepo, signals when changes are breaking.
