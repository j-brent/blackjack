<!-- metadata
id: DREV-DEP
type: design-review
status: draft
author: A19-dependency-manager
created: 2026-01-30
updated: 2026-01-30
approved-by: pending
dependencies: [ARCH-001, STD-001]
-->

# Design Review: Dependency Manager

**Reviewer**: A19-dependency-manager
**Artifacts Reviewed**: ARCH-001 (Architecture), STD-001 (Coding Standards)
**Review Date**: 2026-01-30

## Summary

The project's dependency strategy is sound: libblackjack has zero external dependencies, FTXUI and Catch2 v3 are both actively maintained, permissively licensed, and appropriate for their respective roles. The use of CMake FetchContent provides reproducibility but has trade-offs compared to modern package managers like vcpkg. Overall the dependency choices are low-risk, but several recommendations would improve version control, supply chain security, and build performance.

## Findings

### FINDING-DEP-001: FetchContent Strategy vs. Package Manager

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | dependency |
| **Artifact** | ARCH-001, STD-001 |
| **Section** | Technology Stack, CMake Standards |

**Issue**: The architecture specifies using CMake FetchContent for FTXUI and Catch2. While this provides reproducibility and simplicity, it forces every developer to build these dependencies from source on every clean build, increasing compile times. Modern C++ package managers like vcpkg or Conan provide pre-built binaries while maintaining reproducibility through manifest files.

**Recommendation**: Consider adopting vcpkg as the primary dependency manager for this project. vcpkg is industry-standard (Microsoft-backed), provides centralized dependency declaration via a `vcpkg.json` manifest file, offers pre-compiled binaries for common platforms (significantly faster than FetchContent's source builds), and maintains a curated registry with thousands of packages including both FTXUI and Catch2 v3. FetchContent can be retained as a fallback for custom or internal dependencies.

**Rationale**: For production C++ projects in 2025-2026, the industry consensus favors package managers like vcpkg or Conan over pure FetchContent. They combine the speed of pre-built libraries with the reproducibility of versioned manifests. This is especially important as the project grows and CI/CD pipelines run more frequently. FetchContent's strength is self-contained builds, but vcpkg provides that plus faster iteration times. For a learning/exploration project, FetchContent is acceptable, but the architecture should acknowledge this trade-off and consider migration to vcpkg for Phase 2+ if build times become problematic.

---

### FINDING-DEP-002: Missing Specific Version Pins in Standards

| Property | Value |
|----------|-------|
| **Severity** | WARNING |
| **Category** | dependency |
| **Artifact** | STD-001 |
| **Section** | CMake Standards (Section 10) |

**Issue**: STD-001 states "Pin to specific git tags" for Catch2 (v3.x release tag) and FTXUI (specific release tag) but does not specify which exact versions to use. Without concrete version specifications, different developers may fetch different versions, breaking reproducibility.

**Recommendation**: Specify exact version tags in STD-001 and later in the actual CMakeLists.txt files. For example:
- **FTXUI**: Pin to `v6.1.9` (latest stable release as of May 2025, actively maintained through January 2026)
- **Catch2**: Pin to `v3.8.0` (released January 2025, with ongoing performance improvements)

Additionally, document a versioning policy: dependencies should only be upgraded after explicit review and testing, with version changes tracked in commit messages and release notes.

**Rationale**: "Pin to a v3.x release tag" is too vague. Reproducible builds require exact SHA or tag references. FTXUI v6.1.9 is the current stable release with confirmed active maintenance. Catch2 v3.8.0 is the latest v3 release with performance optimizations for debug builds. Pinning to these specific versions ensures everyone builds against the same code. Future upgrades should be intentional decisions, not accidental drifts.

---

### FINDING-DEP-003: FTXUI and Catch2 Health Assessment — PASS

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | dependency |
| **Artifact** | ARCH-001 |
| **Section** | Technology Stack (Section 3) |

**Issue**: N/A — This is a positive finding.

**Recommendation**: Both FTXUI and Catch2 v3 are healthy, actively maintained dependencies suitable for this project:

**FTXUI**:
- **Latest release**: v6.1.9 (May 7, 2025)
- **Active maintenance**: Issues filed through January 2026, frequent v6.1.x releases throughout 2025
- **License**: MIT (highly permissive, compatible with all common licenses including GPL, Apache, proprietary)
- **GitHub activity**: 9.1k stars, active community
- **Security**: No known CVEs or vulnerabilities as of January 2026
- **Cross-platform**: Linux, macOS, Windows, WebAssembly support
- **Zero dependencies**: Reduces transitive dependency risk
- **Documentation**: Comprehensive docs at arthursonzogni.github.io/FTXUI/

**Catch2 v3**:
- **Latest release**: v3.8.0 (January 6, 2025)
- **Active maintenance**: Ongoing performance improvements, release notes show 20% debug build speedup in v3.8.0
- **License**: Boost Software License 1.0 (BSL-1.0) — very permissive, OSI-approved, compatible with proprietary and GPL
- **GitHub activity**: Maintained by catchorg, widely adopted in industry
- **Security**: No known CVEs. Official security policy acknowledges low attack surface (unit test framework doesn't interact with untrusted input). PGP-verified releases available.
- **C++ standard**: Supports C++14, C++17, C++20 (v2.x supports C++11, v1.x supports C++03)
- **IDE support**: Native support in CLion, Visual Studio, etc.

Both dependencies are low-risk choices with strong track records.

**Rationale**: Due diligence requires verifying that third-party dependencies are healthy, maintained, and secure. Both FTXUI and Catch2 v3 exceed baseline expectations: active development in 2025-2026, permissive licenses with no compatibility concerns, no security incidents, and strong community adoption. The architecture's choices are validated.

---

### FINDING-DEP-004: License Compatibility Confirmed

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | dependency |
| **Artifact** | ARCH-001 |
| **Section** | Technology Stack (Section 3) |

**Issue**: N/A — This is a positive finding.

**Recommendation**: All dependency licenses are compatible with each other and with typical project goals (open-source or proprietary):

- **libblackjack**: Pure C++ standard library — no third-party license obligations
- **FTXUI**: MIT License — permits use in proprietary software, compatible with GPL, Apache, etc.
- **Catch2**: Boost Software License 1.0 (BSL-1.0) — permits use in proprietary software, no copyleft restrictions, compatible with GPL, Apache, etc.

Both MIT and BSL-1.0 allow:
- Commercial use
- Modification
- Distribution
- Private use
- Mixing with proprietary code

Neither license requires:
- Source disclosure (unlike GPL)
- Patent grants (unlike Apache 2.0, though this is rarely an issue)
- Copyleft propagation

If the project later adopts a GPL license, both MIT and BSL-1.0 are GPL-compatible. If the project remains proprietary or uses Apache 2.0, MIT and BSL-1.0 are also compatible.

**Rationale**: License incompatibility can block future project directions (e.g., cannot mix GPL-incompatible libraries with GPL code, cannot use copyleft libraries in proprietary products). Confirming upfront that all dependencies use permissive, broadly compatible licenses avoids future legal or architectural roadblocks. This project is in a strong position to pivot to any licensing model in future phases.

---

### FINDING-DEP-005: Supply Chain Security Considerations

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | dependency |
| **Artifact** | STD-001 |
| **Section** | CMake Standards (Section 10) |

**Issue**: The project fetches dependencies from GitHub repositories via FetchContent. The 2025-2026 threat landscape shows a 73% increase in malicious open-source packages and supply chain attacks targeting CI/CD pipelines (e.g., CVE-2025-30066 affecting 23,000+ repositories). While FTXUI and Catch2 are low-risk (both have clean security records, reputable maintainers, and active communities), supply chain hygiene best practices should be documented.

**Recommendation**: Document and implement the following supply chain security practices in STD-001 or a dedicated security policy:

1. **Pin exact commit SHAs**, not just tags. Tags can theoretically be moved (though rare on well-maintained projects). Example:
   ```cmake
   FetchContent_Declare(
       ftxui
       GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
       GIT_TAG        v6.1.9
       GIT_SHALLOW    TRUE
       GIT_PROGRESS   TRUE
   )
   ```
   Better yet, specify `GIT_TAG` as the full commit SHA corresponding to `v6.1.9`.

2. **Verify GPG signatures** on release tags where available. Catch2 releases are signed; document how to verify them.

3. **Use HTTPS URLs**, never `git://` (already implied by `https://github.com` but worth stating explicitly).

4. **Submodule/vendoring fallback**: For critical production deployments, consider vendoring dependencies (copying source into the repo) or using Git submodules as an alternative to runtime fetching. This ensures builds work even if GitHub is unavailable and provides an audit trail of exactly which code is being built.

5. **Dependency review on updates**: Any PR that updates a dependency version should require explicit review of the upstream changelog and diff to ensure no malicious code was introduced.

6. **CI/CD pipeline hardening**: Ensure CI runners use pinned dependencies and cannot be compromised by upstream changes. Consider using a private vcpkg registry or mirror for production.

7. **SBOM (Software Bill of Materials)**: For Phase 2+, consider generating an SBOM (e.g., using SPDX or CycloneDX formats) to track all dependencies and their versions for compliance and security audits.

**Rationale**: FTXUI and Catch2 are trustworthy, but supply chain attacks are an industry-wide concern in 2026. Defense-in-depth means not relying solely on the reputation of upstream maintainers. Version pinning, signature verification, and vendoring options provide layers of protection. These practices are especially important if this project becomes a dependency for others or if future phases add more complex dependencies (e.g., cryptography libraries, network libraries). Documenting these practices now establishes a security-first culture.

---

### FINDING-DEP-006: Alternatives Analysis — FTXUI is the Right Choice

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | dependency |
| **Artifact** | ARCH-001 |
| **Section** | Technology Stack (Section 3) |

**Issue**: N/A — This is a validation finding.

**Recommendation**: FTXUI is the appropriate choice for a modern C++ terminal UI library in this project. Alternatives considered:

| Library | Pros | Cons | Verdict |
|---------|------|------|---------|
| **ncurses** | Ubiquitous, stable, C API | C API (not modern C++), complex, platform quirks | Too low-level |
| **cpp-terminal** | Lightweight ncurses replacement | Less feature-rich, smaller community | Adequate but less capable |
| **FINAL CUT** | Widget toolkit, full-featured | Heavier, more complex API | Overkill for this project |
| **Notcurses** | Modern, vibrant rendering | C library (C++ headers exist), more complex | Overkill, steeper learning curve |
| **ImTui** | Immediate mode (Dear ImGui style) | Less mature, smaller ecosystem | Interesting but unproven |
| **termbox/termbox2** | Minimal, simple API | C API, less feature-rich | Too minimal for rich UI |
| **PDCurses** | Cross-platform curses | C API, older design | Same issues as ncurses |

**Why FTXUI wins**:
- **Modern C++ API**: Uses C++17, aligns with project's C++20 standard
- **Functional/reactive style**: Inspired by React, fits modern UI paradigms
- **Zero dependencies**: Matches libblackjack's philosophy
- **CMake FetchContent support**: Documented in FTXUI repo
- **Active maintenance**: 9.1k stars, releases through 2025
- **Cross-platform**: Linux, macOS, Windows, WASM (matches project's future phases)
- **UTF-8 and Unicode support**: Critical for card suit symbols (♠♥♦♣)
- **Mouse and keyboard navigation**: Rich interaction model
- **Right complexity level**: More capable than termbox, less complex than FINAL CUT

For a blackjack TUI that needs to render card suits, layout dealer/player panels, and provide keyboard-driven controls, FTXUI hits the sweet spot of capability and simplicity.

**Rationale**: Validating the choice against alternatives confirms the architecture made an informed decision. ncurses is the "default" choice for C/C++ TUIs but is a poor fit for modern C++ (C API, manual memory management, platform differences). FTXUI is purpose-built for modern C++ and aligns with the project's goals. If FTXUI becomes unmaintained in the future, cpp-terminal or Notcurses would be viable fallbacks, but as of 2026, FTXUI is the clear winner.

---

### FINDING-DEP-007: Alternatives Analysis — Catch2 v3 vs. GoogleTest vs. doctest

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | dependency |
| **Artifact** | STD-001 |
| **Section** | Testing Standards (Section 8) |

**Issue**: N/A — This is a validation finding.

**Recommendation**: Catch2 v3 is a strong choice for this project, though GoogleTest and doctest are also viable. Comparison:

| Framework | Pros | Cons | Verdict for this Project |
|-----------|------|------|--------------------------|
| **Catch2 v3** | Free-form test names, BDD macros, sections for setup/teardown, actively maintained (v3.8.0 in Jan 2025) | Moderate compile times (better in v3 than v2) | **Recommended** — Good balance |
| **Google Test** | Industry standard, extensive features, GMock for mocking, wide IDE support | Test names must be valid C++ identifiers, overkill for simple projects, more complex API | Acceptable alternative |
| **doctest** | Fastest compile times, single-header, minimal overhead, can be disabled for production builds | Fewer advanced features, smaller community | Good for embedded/fast iteration |
| **Boost.Test** | Comprehensive, hierarchical test organization | Requires Boost, complex API, slow compile times | Not recommended (no Boost in project) |

**Why Catch2 v3 fits this project**:
- **Free-form test names**: `TEST_CASE("Hand value with soft ace counts ace as 11")` is more readable than GoogleTest's `TEST(HandTest, SoftAceValue)`
- **Sections**: ARCH-001's game logic has clear phases (deal, player turn, dealer turn, resolution). Catch2's sections allow shared setup code inline with tests, which fits this workflow better than GoogleTest's fixtures.
- **BDD macros**: If tests want to use `GIVEN/WHEN/THEN` style, Catch2 supports it natively.
- **Active maintenance**: v3.8.0 released Jan 2025 with 20% faster debug builds — shows ongoing optimization.
- **STD-001 already chose it**: Changing now would require justification.

**When to use alternatives**:
- **GoogleTest**: If the team is already using Google's ecosystem (Bazel, Abseil, etc.), or if extensive mocking is needed (GMock is mature and well-integrated).
- **doctest**: If compile times become a bottleneck (unlikely for this project's scale), or if the library needs embedded testing (e.g., tests compiled into the production binary for field diagnostics).

**Rationale**: STD-001 specifies Catch2 v3, and this review validates that choice. All three frameworks (Catch2, GoogleTest, doctest) are production-ready; the differences are stylistic and philosophical. Catch2's free-form naming and section-based setup align well with the blackjack domain (test names like "Player busts with three face cards" are clearer than `TEST(PlayerTest, BustWithThreeFaceCards)`). Since the architecture doesn't require Boost and doesn't need advanced mocking (game logic is deterministic with seeded RNG), Catch2 is the right fit.

---

### FINDING-DEP-008: No Transitive Dependency Risk

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | dependency |
| **Artifact** | ARCH-001 |
| **Section** | Technology Stack (Section 3) |

**Issue**: N/A — This is a positive finding.

**Recommendation**: The project has minimal transitive dependency risk:

- **libblackjack**: Zero dependencies (pure C++ stdlib) → Zero transitive dependencies
- **FTXUI**: Zero dependencies (stated in project docs) → Zero transitive dependencies
- **Catch2 v3**: Minimal dependencies (may depend on standard system libraries for threading/time, but no third-party libraries) → Effectively zero transitive dependencies

**Dependency tree**:
```
blackjack/
├── lib/ (libblackjack)
│   └── dependencies: none
├── cli/
│   └── dependencies: libblackjack
├── ftxui/
│   └── dependencies: libblackjack, FTXUI
│       └── FTXUI dependencies: none
└── tests/
    └── dependencies: libblackjack, Catch2
        └── Catch2 dependencies: none
```

This is the ideal dependency graph: flat, minimal, no deep transitive chains.

**Contrast with high-risk scenarios**:
- Projects using Boost (50+ sub-libraries, transitive dependencies on system libraries)
- Projects using Qt (depends on dozens of modules, platform-specific backends)
- Projects using modern web frameworks (npm-style dependency hell with hundreds of transitive packages)

This project has **three direct dependencies** (libblackjack, FTXUI, Catch2) and **zero transitive dependencies**. This dramatically reduces:
- Supply chain attack surface (fewer packages to audit)
- License compliance burden (only need to track MIT, BSL-1.0, and C++ stdlib)
- Build complexity (no dependency resolution conflicts)
- Update risk (changing FTXUI version doesn't cascade into 20 other updates)

**Rationale**: Transitive dependencies are a major source of supply chain risk, license conflicts, and build fragility. The 2025 increase in malicious packages often exploits deep transitive dependency chains (e.g., developer depends on A, which depends on B, which depends on C, and C gets compromised). This project's zero-transitive-dependency architecture is a significant security and maintainability advantage. Document this as a design principle to preserve in future phases.

---

### FINDING-DEP-009: Version Pinning Strategy for Future Phases

| Property | Value |
|----------|-------|
| **Severity** | NOTE |
| **Category** | dependency |
| **Artifact** | ARCH-001 |
| **Section** | Future Phase Considerations (Section 7) |

**Issue**: ARCH-001 outlines future phases (WASM, Python, Qt, Mobile) but does not address how dependency management will evolve as new platforms and languages are added.

**Recommendation**: Document a forward-looking dependency strategy for multi-language/multi-platform phases:

**Phase 2: WASM (Emscripten)**
- libblackjack compiles to WASM unchanged (pure C++20, no platform-specific code).
- FTXUI may not be needed (browser has its own UI). If a terminal emulator in browser is desired, FTXUI supports WASM but verify Emscripten compatibility for the pinned version.
- Catch2 tests should run natively (pre-compilation) and via Node.js or browser test runners for WASM builds.
- **New dependency risk**: Emscripten SDK and its toolchain. Pin Emscripten version explicitly.

**Phase 3: Python (pybind11/nanobind)**
- libblackjack compiles unchanged.
- **New dependencies**: pybind11 or nanobind (choose one, pin version).
- **License check**: pybind11 is BSD-3-Clause (compatible). nanobind is BSD-3-Clause (compatible).
- **Transitive risk**: Python ecosystem (pip packages). If Python frontend needs dependencies (e.g., pytest for testing, rich for terminal UI), those introduce a new dependency domain with different norms (frequent updates, larger transitive trees). Consider vendoring or using Poetry/pipenv for reproducible Python environments.

**Phase 4: Qt**
- libblackjack compiles unchanged.
- **New dependency**: Qt framework (massive dependency, LGPL/commercial dual-license).
- **License implications**: LGPL requires dynamic linking or source disclosure of modifications. If proprietary distribution is a goal, Qt commercial license is needed. This is a significant decision.
- **Version pinning**: Qt LTS versions (e.g., Qt 6.5 LTS). Qt has a different release cadence than small libraries like FTXUI.

**Phase 5: Mobile (iOS/Android)**
- Likely uses Qt for Mobile or native SDKs (UIKit/SwiftUI for iOS, Jetpack Compose/Java for Android).
- **New dependencies**: Xcode toolchain (iOS), Android SDK/NDK (Android).
- libblackjack may need a C API wrapper for easier JNI/Swift interop.

**General principle for future phases**: Each phase should add a new subdirectory (e.g., `wasm/`, `python/`, `qt/`) with its own `CMakeLists.txt` or build configuration (e.g., `setup.py` for Python, `qmake` or `CMakeLists.txt` for Qt). **Dependencies for that phase should be isolated to that subdirectory**. Do not let Qt or Emscripten dependencies leak into the core library or other frontends. The library's zero-dependency guarantee must be preserved.

**Rationale**: ARCH-001 correctly designs the library to be agnostic to frontends, but dependency management becomes more complex when multiple ecosystems are involved (C++, JavaScript, Python, Qt/QML). Establishing a policy now—each frontend manages its own dependencies, library stays pure C++ stdlib—prevents future architectural drift where the library becomes coupled to a specific frontend's dependencies. This is critical for the vision's goal of "explore as many frontends as possible."

---

## Findings Summary

| # | Title | Severity | Category | Artifact | Status |
|---|-------|----------|----------|----------|--------|
| DEP-001 | FetchContent vs. vcpkg package manager | WARNING | dependency | ARCH-001, STD-001 | open |
| DEP-002 | Missing specific version pins in standards | WARNING | dependency | STD-001 | open |
| DEP-003 | FTXUI and Catch2 health assessment — PASS | NOTE | dependency | ARCH-001 | n/a |
| DEP-004 | License compatibility confirmed | NOTE | dependency | ARCH-001 | n/a |
| DEP-005 | Supply chain security considerations | NOTE | dependency | STD-001 | open |
| DEP-006 | Alternatives analysis — FTXUI is right choice | NOTE | dependency | ARCH-001 | n/a |
| DEP-007 | Alternatives analysis — Catch2 v3 validation | NOTE | dependency | STD-001 | n/a |
| DEP-008 | No transitive dependency risk | NOTE | dependency | ARCH-001 | n/a |
| DEP-009 | Version pinning strategy for future phases | NOTE | dependency | ARCH-001 | open |

## Overall Assessment

- [x] **CONDITIONAL PASS**: Warnings exist that should be addressed, but not blocking.
- [ ] **PASS**: No blockers. Ready to proceed.
- [ ] **FAIL**: Blockers exist. Must be resolved before proceeding.

**Summary**: The dependency choices (FTXUI, Catch2 v3) are excellent: actively maintained, permissively licensed, secure, and appropriate for the project's needs. The zero-transitive-dependency architecture is a major strength. Two warnings should be addressed before finalizing the design:

1. **DEP-001**: Consider migrating from FetchContent to vcpkg for better build performance and ecosystem alignment, especially if the project scales or moves to CI/CD.
2. **DEP-002**: Specify exact version pins (FTXUI v6.1.9, Catch2 v3.8.0) in STD-001 to ensure reproducible builds.

These are not blockers—the project can proceed with FetchContent and later refine version pins in the actual CMakeLists.txt files. The notes (DEP-003 through DEP-009) provide valuable context and forward-looking recommendations but do not require immediate action.

## Sign-off

| Reviewer | Assessment | Date |
|----------|-----------|------|
| A19-dependency-manager | CONDITIONAL PASS | 2026-01-30 |

---

## Sources

Research for this review included the following sources:

### FTXUI
- [FTXUI GitHub Repository](https://github.com/ArthurSonzogni/FTXUI)
- [FTXUI Documentation](https://arthursonzogni.github.io/FTXUI/)
- [FTXUI Releases](https://github.com/ArthurSonzogni/FTXUI/releases)
- [FTXUI vcpkg Package](https://vcpkg.link/ports/ftxui)
- [Arch Linux FTXUI Package](https://aur.archlinux.org/packages/ftxui-git)
- [MacPorts FTXUI](https://ports.macports.org/port/FTXUI/details/)
- [FreeBSD FTXUI Port](https://www.freshports.org/devel/ftxui)

### Catch2
- [Catch2 GitHub Repository](https://github.com/catchorg/Catch2)
- [Catch2 Official Website](https://catch2.org/)
- [Catch2 Releases](https://github.com/catchorg/catch2/releases)
- [Catch2 Documentation - Release Notes](https://catch2-temp.readthedocs.io/en/latest/release-notes.html)
- [Catch2 License (BSL-1.0)](https://github.com/catchorg/Catch2/blob/devel/LICENSE.txt)
- [Catch2 Security Policy](https://github.com/catchorg/Catch2/blob/devel/SECURITY.md)
- [Boost Software License](https://choosealicense.com/licenses/bsl-1.0/)

### Dependency Management
- [CMake Discourse: FetchContent vs vcpkg/Conan](https://discourse.cmake.org/t/fetchcontent-vs-vcpkg-conan/6578)
- [CMake and vcpkg Tutorial](https://www.studyplan.dev/cmake/cmake-package-managers-integration-vcpkg)
- [CMake FetchContent vs ExternalProject Guide](https://www.studyplan.dev/cmake/fetching-external-dependencies)
- [GitHub: vcpkg and FetchContent Integration](https://github.com/microsoft/vcpkg/issues/30572)
- [CPM.cmake - FetchContent Wrapper](https://dev.to/pgradot/cpmcmake-to-make-cmakes-fetchcontent-easier-5dm)

### Security and Supply Chain
- [ReversingLabs 2026 Software Supply Chain Report](https://www.globenewswire.com/news-release/2026/01/27/3226752/0/en/ReversingLabs-2026-Software-Supply-Chain-Security-Report-Identifies-73-Increase-in-Malicious-Open-Source-Packages.html)
- [Five Key Flaws in 2025 Supply Chain Incidents](https://www.infosecurity-magazine.com/news-features/five-flaws-exploited-2025-software/)
- [Supply Chain Attacks 2025 Summary](https://www.silobreaker.com/blog/cyber-threats/supply-chain-attacks-in-2025-a-month-by-month-summary/)
- [GitHub Advisory Database](https://github.com/advisories)
- [CISA Known Exploited Vulnerabilities](https://www.cisa.gov/known-exploited-vulnerabilities-catalog)

### Alternative Libraries
- [cpp-terminal GitHub](https://github.com/jupyter-xeus/cpp-terminal)
- [Awesome TUIs List](https://github.com/rothgar/awesome-tuis)
- [termbox Alternatives](https://alternativeto.net/software/termbox/)
- [Lobsters: TUI Libraries Discussion](https://lobste.rs/s/b4jhn0/do_you_know_any_good_tui_libraries)
- [Awesome C++ List](https://github.com/fffaraz/awesome-cpp)

### Test Framework Comparisons
- [C++ Test Framework Selection Guide](https://yurigeronimus.medium.com/guide-for-choosing-a-test-framework-for-your-c-project-2a7741b53317)
- [Catch2 vs Google Test Comparison](https://snorristurluson.github.io/Catch2/)
- [Comparison of C++ Unit Test Frameworks](https://linuxhint.com/unit-test-frameworks-cpp/)
- [C++ Testing Frameworks in 2023](https://hackingcpp.com/cpp/tools/testing_frameworks.html)
- [doctest - Fastest C++ Unit Testing](https://blog.jetbrains.com/rscpp/2019/07/10/better-ways-testing-with-doctest/)
- [Feature Comparison: Catch, doctest, lest](https://github.com/martinmoene/catch-lest-other-comparison)
- [CLion Unit Testing Tutorial](https://www.jetbrains.com/help/clion/unit-testing-tutorial.html)
