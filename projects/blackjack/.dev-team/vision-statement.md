<!-- metadata
id: VISION-001
type: vision-statement
status: approved
author: H1-visionary
created: 2026-01-30
updated: 2026-01-30
approved-by: H1-visionary
dependencies: []
-->

# Vision Statement: Blackjack — C++ Frontend Exploration

## Product Vision

A blackjack game engine written in modern C++ with multiple frontend implementations. The project exists to evaluate C++ frontend integration strategies, establish guidelines for separating backend logic from frontend presentation, and expose unseen challenges that emerge only when the same backend is consumed by fundamentally different UI technologies. Every frontend should be simple, beautiful, and elegant — positive user experience is a first-class requirement, not an afterthought.

## Problem Statement

When building a C++ application that needs a user interface, developers face a fragmented landscape of options — native GUI frameworks, terminal UIs, web via WASM, and foreign-language bindings. Each has different build complexity, interop friction, and developer experience. There is no hands-on, apples-to-apples comparison built against a single backend. This project creates that comparison.

## Target Audience

| Audience Segment | Description | Primary Need |
|-----------------|-------------|--------------|
| The Visionary/CTO | The project author, learning by building | Hands-on understanding of C++ frontend tradeoffs and integration patterns |
| Future project developers | Team members choosing a UI strategy for a C++ project | A documented comparison with real code and written findings |

## Value Proposition

Instead of reading blog posts and documentation about UI frameworks, build the same application in each and compare directly. The blackjack domain is simple enough that framework evaluation isn't obscured by domain complexity.

## Success Criteria

| Criterion | Metric | Target |
|-----------|--------|--------|
| Backend library complete | Standalone C++ library with clean API, compilable and testable independently | Yes |
| Each frontend plays blackjack | A single human user can play repeated games against a dealer | All implemented frontends |
| UX quality | Each frontend is simple, beautiful, and elegant — sleek design appropriate to its platform | All implemented frontends |
| Findings documented | Written comparison of each frontend on named dimensions (build complexity, interop friction, developer experience, binary size, platform support) | One findings document per phase |
| Separation guidelines produced | A document describing the backend/frontend separation pattern that emerged | Yes |
| Time budget respected | Phase 1 effort (backend + CLI + FTXUI) | 6 hours maximum |

## Scope: What This Product IS

- A C++ backend library implementing blackjack rules: hit, stand, split, soft aces, dealer hits soft 17. No betting.
- **Phase 1**: CLI frontend + FTXUI terminal GUI frontend (6-hour budget)
- **Phase 2**: Vanilla JS web frontend via Emscripten/WASM
- **Phase 3**: Python frontend via pybind11/nanobind bindings + Python UI framework
- **Phase 4**: Qt/QML desktop GUI frontend
- **Phase 5**: Mobile app frontend
- A comparison document evaluating each frontend
- A guidelines document on backend/frontend separation for C++ projects

## Scope: What This Product IS NOT

- Not a casino or gambling application (no real money, no multiplayer)
- Not a full casino game (no betting, no doubling down, no insurance, no side bets)
- Not a production application (no persistence, no user accounts, no networking)
- Later phases are not guaranteed — expand only after earlier phases are complete and documented

## Key Assumptions

- Modern C++ (C++17 or later) is the baseline for the backend library
- FTXUI is cross-platform (Linux, Windows, macOS) and suitable for a terminal GUI
- Emscripten can compile the backend library to WASM without major architectural changes
- pybind11 or nanobind can expose the backend API to Python cleanly
- The backend API can be designed once and consumed by all frontends without modification

## Open Questions

| # | Question | Status | Resolution |
|---|----------|--------|------------|
| 1 | Which Python UI framework to use? (Tkinter, Dear PyGui, Pygame, Textual) | open | Decide during Phase 3 |
| 2 | Should the backend expose a C API for maximum interop, or a C++ API with bindings per frontend? | open | Architect to decide in Stage 2 |
| 3 | Which mobile framework for Phase 5? (Qt for mobile, React Native + WASM, native iOS/Android with C++ core) | open | Decide after Phase 4 |
| 4 | Should Phase 4 (Qt/QML) reuse the same C++ API directly, or go through a Qt-specific adapter? | open | Architect to decide in Stage 2 |
