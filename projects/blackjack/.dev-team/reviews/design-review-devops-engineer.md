<!-- metadata
id: DREV-DEVOPS
type: design-review
status: draft
author: A15-devops-engineer
created: 2026-01-30
updated: 2026-01-30
approved-by: pending
dependencies: [ARCH-001, STD-001, GIT-001]
-->

# Design Review: DevOps Engineer

**Reviewer**: A15-devops-engineer
**Artifacts Reviewed**: ARCH-001, STD-001, GIT-001
**Review Date**: 2026-01-30

## Summary

The build and deployment strategy is sound for a local C++ project with multiple frontends. CMake with FetchContent provides clean dependency management. Cross-platform build considerations are addressed. The git workflow is appropriate for the team size. Minor warnings around build configuration completeness and CI automation planning.

## Findings

### FINDING-001: Missing CMake configuration for sanitizers and static analysis

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | deployability |
| **Artifact** | STD-001 |
| **Section** | 10. CMake Standards |

**Issue**: The coding standards specify warning flags but do not mention build configurations for AddressSanitizer, UndefinedBehaviorSanitizer, or static analysis tools (clang-tidy, cppcheck). These are critical for catching memory errors and undefined behavior in C++ projects.

**Recommendation**: Add CMake options for enabling sanitizers:
```cmake
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
```
Document in STD-001 that developers should periodically run tests with sanitizers enabled, especially before merging to `main`.

**Rationale**: Blackjack involves manual memory management of card vectors and state transitions. Sanitizers catch memory leaks, use-after-free, buffer overflows, and undefined behavior that tests alone may miss. This is standard practice for production C++ and costs nothing to configure upfront.

---

### FINDING-002: FetchContent dependency pinning strategy not fully specified

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | deployability |
| **Artifact** | STD-001 |
| **Section** | 10. CMake Standards |

**Issue**: STD-001 states "pin to specific git tags" for Catch2 and FTXUI but does not specify which versions or a policy for updating them.

**Recommendation**: Document specific pinned versions in STD-001 or create a separate dependency manifest:
- Catch2: v3.5.1 (or current stable v3.x)
- FTXUI: v5.0.0 (or current stable release)

Establish a policy: "Dependencies are updated only when a specific feature is needed or a security/bug fix is required. Updates are tested on all target platforms before merge."

**Rationale**: Prevents surprise breakage from upstream changes. FetchContent fetches at configure time; unpinned or moving tags can cause non-reproducible builds. Explicit versioning is a DevOps best practice.

---

### FINDING-003: No Debug/Release configuration guidance

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | deployability |
| **Artifact** | STD-001 |
| **Section** | 10. CMake Standards |

**Issue**: The standards do not specify how Debug and Release configurations should be used. C++ projects have vastly different performance and debugging characteristics between `-O0 -g` (Debug) and `-O3 -DNDEBUG` (Release).

**Recommendation**: Add to STD-001:
- **Development**: Use `CMAKE_BUILD_TYPE=Debug` for local testing and debugging.
- **CI**: Run tests in both Debug (for assertions and sanitizers) and Release (for performance validation).
- **Distribution**: Users should build in Release mode for playable binaries.

Document that the library's `assert()` statements (if any) are for internal invariants only and compiled out in Release.

**Rationale**: Debug builds with optimizations disabled can be 10x slower. Players running the TUI in Debug mode may have a poor experience. Conversely, testing only in Release may hide bugs that only trigger with `-DNDEBUG`.

---

### FINDING-004: Cross-platform build matrix not tested yet

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | deployability |
| **Artifact** | ARCH-001, STD-001 |
| **Section** | 3. Technology Stack (ARCH-001), 1. Language & Compiler (STD-001) |

**Issue**: The specs target GCC 12+, Clang 15+, and MSVC 19.30+ across Linux, macOS, and Windows. FTXUI and Catch2 are cross-platform, but until the CMake files are written and tested on all three platforms, there is risk of platform-specific build failures (e.g., MSVC missing `<format>`, macOS requiring specific SDK flags, Windows path separators in FetchContent).

**Recommendation**:
1. First implementer should test on at least two platforms (e.g., Windows + Linux or Windows + WSL).
2. Document any platform-specific quirks in a `BUILD.md` file (e.g., "On macOS, ensure Xcode Command Line Tools are installed").
3. If CI is added later, use GitHub Actions matrix builds for all three OS/compiler combinations.

**Rationale**: Cross-platform C++ builds have many footguns. Early validation prevents painful debugging later. This is a WARNING not a BLOCKER because the architecture is sound—it's an execution risk, not a design flaw.

---

### FINDING-005: No CI/CD configuration specified for Phase 1

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | deployability |
| **Artifact** | GIT-001 |
| **Section** | 4. Build Verification (Pre-Merge) |

**Issue**: GIT-001 states that all merges to `main` require zero warnings and passing tests, but does not specify how this is enforced. Manual verification is error-prone.

**Recommendation**: Add a `.github/workflows/ci.yml` GitHub Actions workflow that runs on every pull request:
```yaml
jobs:
  build-and-test:
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest, windows-latest]
        compiler: [gcc, clang, msvc]
    steps:
      - configure CMake with warnings-as-errors
      - build all targets
      - run blackjack_tests
      - fail if warnings or test failures
```

Alternatively, document that verification is manual for Phase 1 and CI is deferred to Phase 2.

**Rationale**: Automated checks prevent broken builds from merging. However, for a small team or solo project, CI may be overkill initially. This is a NOTE because the specs acknowledge "local C++ project, not a cloud service"—manual verification is acceptable if documented.

---

### FINDING-006: Git workflow squash merge strategy is appropriate

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | deployability |
| **Artifact** | GIT-001 |
| **Section** | 1. Branching Strategy |

**Issue**: None. This is a positive finding.

**Recommendation**: Continue with squash merges. The linear history on `main` will simplify bisecting, cherry-picking, and release tagging if the project grows.

**Rationale**: For a project of this size and structure, squash merges to `main` with descriptive commit messages provide clean history without the noise of "fix typo" and "wip" commits. This is a DevOps best practice for small-to-medium teams.

---

### FINDING-007: FetchContent is appropriate for FTXUI and Catch2

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | deployability |
| **Artifact** | ARCH-001, STD-001 |
| **Section** | 3. Technology Stack (ARCH-001), 10. CMake Standards (STD-001) |

**Issue**: None. This is a positive finding.

**Recommendation**: Continue using FetchContent. Both FTXUI and Catch2 are header-only or have clean CMake integration. FetchContent avoids the "install FTXUI system-wide" problem and ensures reproducible builds.

**Rationale**: FetchContent is the modern CMake approach for C++ dependencies. It works well for projects like FTXUI that have active CMake support. The alternative (git submodules or system packages) is more fragile. The specs got this right.

---

### FINDING-008: No packaging or installation targets specified

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | deployability |
| **Artifact** | ARCH-001 |
| **Section** | 6. Deployment |

**Issue**: ARCH-001 states "no installers, no packaging. Run from the build directory or copy the binary." This is fine for Phase 1 but limits distribution.

**Recommendation**: For Phase 1, accept this limitation. For Phase 2+, consider adding a CMake `install()` target that places binaries in `bin/`, libraries in `lib/`, and headers in `include/`. This is standard CMake practice and enables `make install` or `cmake --install`.

**Rationale**: Users expect `cmake --install` to work. However, for a learning/exploration project, manual binary copying is acceptable. This is a NOTE, not a warning, because the specs explicitly scope this out for Phase 1.

---

### FINDING-009: Dependency graph is clean and acyclic

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | deployability |
| **Artifact** | ARCH-001 |
| **Section** | 5. Build Structure |

**Issue**: None. This is a positive finding.

**Recommendation**: The dependency graph is:
```
blackjack_lib (no deps)
    ↑
    ├─ blackjack_cli
    ├─ blackjack_tui (+ FTXUI)
    └─ blackjack_tests (+ Catch2)
```
This is ideal. The library is a pure static library with zero dependencies. Frontends depend only on the library and their UI framework. Tests depend on the library and Catch2. This is a textbook clean build structure.

**Rationale**: Clean dependency graphs build fast, cache well, and are easy to reason about. The architecture avoids circular dependencies and "everything depends on everything" anti-patterns. This is exactly what a DevOps engineer wants to see.

---

### FINDING-010: Missing build-time vs runtime dependency clarity

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | deployability |
| **Artifact** | ARCH-001 |
| **Section** | 5. Build Structure |

**Issue**: All executables are statically linked (static library + static FTXUI). This is fine, but STD-001 does not explicitly state "static linking only, no shared libraries."

**Recommendation**: Add to STD-001 or ARCH-001: "All targets use static linking. `blackjack_lib` is a static library. FTXUI is fetched and linked statically. Executables are standalone with no runtime dependencies beyond system libc/libc++."

**Rationale**: Static linking simplifies deployment (no DLL/SO search path issues) but increases binary size. For a terminal app, this is the right tradeoff. Documenting it prevents future confusion about why there's no shared library target.

---

## Findings Summary

| # | Title | Severity | Category | Artifact | Status |
|---|-------|----------|----------|----------|--------|
| FINDING-001 | Missing sanitizer configuration | WARNING | deployability | STD-001 | open |
| FINDING-002 | FetchContent pinning not specified | NOTE | deployability | STD-001 | open |
| FINDING-003 | No Debug/Release guidance | WARNING | deployability | STD-001 | open |
| FINDING-004 | Cross-platform build matrix untested | WARNING | deployability | ARCH-001, STD-001 | open |
| FINDING-005 | No CI/CD for Phase 1 | NOTE | deployability | GIT-001 | open |
| FINDING-006 | Squash merge strategy (positive) | NOTE | deployability | GIT-001 | open |
| FINDING-007 | FetchContent strategy (positive) | NOTE | deployability | ARCH-001, STD-001 | open |
| FINDING-008 | No install target for Phase 1 | NOTE | deployability | ARCH-001 | open |
| FINDING-009 | Clean dependency graph (positive) | NOTE | deployability | ARCH-001 | open |
| FINDING-010 | Static linking not documented | NOTE | deployability | ARCH-001, STD-001 | open |

## Overall Assessment

- [ ] **PASS**: No blockers. Ready to proceed.
- [x] **CONDITIONAL PASS**: Warnings exist that should be addressed, but not blocking.
- [ ] **FAIL**: Blockers exist. Must be resolved before proceeding.

**Verdict**: The architecture and build strategy are solid. The warnings (sanitizers, Debug/Release, cross-platform testing) are standard DevOps best practices that should be addressed during implementation, not design blockers. The git workflow is appropriate. The CMake structure is clean. FetchContent is the right choice. No containers or cloud deployment are needed for Phase 1, which is correct for a local C++ project.

**Action items before starting implementation**:
1. Add sanitizer CMake options to STD-001.
2. Document Debug/Release build mode usage.
3. Pin exact Catch2 and FTXUI versions.
4. Test CMake configuration on at least two platforms early.

## Sign-off

| Reviewer | Assessment | Date |
|----------|-----------|------|
| A15-devops-engineer | CONDITIONAL PASS | 2026-01-30 |
