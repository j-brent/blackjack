<!-- metadata
id: GIT-001
type: git-workflow
status: draft
author: A3-tech-lead
created: 2026-01-30
updated: 2026-01-30
approved-by: pending
dependencies: [STD-001]
-->

# Git Workflow: Blackjack C++20 Project

## 1. Branching Strategy

| Branch | Purpose | Rules |
|--------|---------|-------|
| `main` | Stable, always builds, all tests pass | No direct pushes. Merge only via squash merge after review. |
| `feature/<name>` | New features | Branch from `main`. Examples: `feature/game-engine`, `feature/cli-frontend`, `feature/ftxui-frontend` |
| `fix/<name>` | Bug fixes | Branch from `main`. Examples: `fix/soft-ace-calc`, `fix/split-state-transition` |

- **Merge method**: Squash merge to `main` (one commit per feature/fix, clean history).
- **Delete branches** after merge.

## 2. Commit Message Format

```
<type>(<scope>): <description>

[optional body]
```

**Types**:
| Type | Use for |
|------|---------|
| `feat` | New functionality |
| `fix` | Bug fix |
| `test` | Adding or updating tests |
| `build` | CMake, dependencies, CI configuration |
| `refactor` | Code changes that don't alter behavior |
| `docs` | Documentation only |
| `style` | Formatting, whitespace, naming (no logic change) |

**Scopes**: `lib`, `cli`, `ftxui`, `tests`, `build`

**Examples**:
```
feat(lib): implement hand value calculation with soft ace logic
fix(lib): correct bust detection when ace transitions from 11 to 1
test(tests): add edge case tests for split aces
build(build): add FTXUI FetchContent configuration
refactor(lib): extract state transition logic into private methods
```

**Rules**:
- Subject line: imperative mood, lowercase, no period, max 72 characters.
- Body (optional): explain *why*, not *what*. Wrap at 72 characters.

## 3. Code Review

- All merges to `main` require review (handled by the Code Reviewer agent in this pipeline).
- Review checks against STD-001 (coding standards): naming, style, error handling, test coverage, warning-free compilation.
- Reviewer approves or requests changes. No self-merges.

## 4. Build Verification (Pre-Merge)

Before any merge to `main`:

1. **Compiles with zero warnings** on all target compilers (GCC 12+, Clang 15+, MSVC 19.30+).
2. **All tests pass** (`blackjack_tests` runs clean).
3. **No regressions**: existing tests must not break.

## 5. Branch Naming Examples

| Task | Branch Name |
|------|-------------|
| Implement Card, Deck, Hand classes | `feature/game-engine` |
| Implement Game state machine | `feature/game-engine` (same branch, it's one logical unit) |
| Build CLI frontend | `feature/cli-frontend` |
| Build FTXUI frontend | `feature/ftxui-frontend` |
| Set up CMake and project skeleton | `feature/project-setup` |
| Fix a bug in dealer logic | `fix/dealer-soft-17` |
