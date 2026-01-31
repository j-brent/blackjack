# C++ Learnings

## [2026-01-30] Directive: Follow the C++ Core Guidelines
**ID**: e5f6g7h8
**Category**: language
**Context**: All C++ code written by any agent
**Learning**: All C++ code must conform to the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) by Bjarne Stroustrup and Herb Sutter by default. Key areas: resource management (RAII, no raw new/delete), type safety (no C-style casts, use static_cast etc.), bounds safety (prefer span/array_view over raw pointers), lifetime safety (no dangling references), interface design (make interfaces easy to use correctly and hard to use incorrectly), and modern idioms (prefer value semantics, use constexpr, default comparisons, structured bindings). Any decision to deviate from the Core Guidelines must be explicitly justified and documented (e.g., in a code comment or design artifact explaining why the guideline does not apply). Reference the official guidelines at https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines when uncertain.
**Evidence**: User directive during Phase 17 integration test. The C++ Core Guidelines are the industry-standard baseline for modern C++ (C++11 onward) and should be the default for all C++ work.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team

## [2026-01-30] Lesson: Use std::ignore instead of (void) cast to discard [[nodiscard]] values
**ID**: a1b2c3d4
**Category**: language
**Context**: C++ codebases using [[nodiscard]] on function return values where the caller intentionally ignores the result
**Learning**: Never use `(void)expr` to suppress [[nodiscard]] warnings — it is a C-style cast. Use `std::ignore = expr` (requires `#include <tuple>`) instead. This applies to all C++ code: library, tests, and frontends.
**Evidence**: User-reported coding standard violation during Phase 17 integration test. Agents introduced `(void)game.deal()` casts throughout test and frontend code to suppress [[nodiscard]] warnings, violating the project's no-C-style-casts rule.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team

## [2026-01-30] Directive: Prefer std::ranges algorithms over iterator-pair overloads
**ID**: b3c4d5e6
**Category**: language
**Context**: All C++ code using standard algorithms (find, sort, transform, etc.)
**Learning**: Always prefer `std::ranges::` algorithm overloads (e.g., `std::ranges::find(container, value)`) over the classic iterator-pair versions (e.g., `std::find(c.begin(), c.end(), value)`). The ranges versions are cleaner, safer (no mismatched iterators), and available in C++20. This applies to all algorithms in `<algorithm>` that have a `std::ranges::` counterpart.
**Evidence**: User directive during Phase 17 integration test.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team
