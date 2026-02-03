# Dev Team

This repository is a multi-agent software development team. The agents are the primary product — they learn and improve by building real projects. Working software is the vehicle for learning; accumulated knowledge is the real output.

## Repository Structure

```
.claude/
  agents/           19 agent definitions (architect, frontend-developer, etc.)
  skills/           21 callable skills (invoke agents for specific tasks)
  learnings/        Accumulated knowledge (by-language, by-domain, by-tool)

projects/
  blackjack/        First project — C++ game engine + multiple frontends
    .dev-team/      Project docs (architecture, specs, issues, coding standards)
    lib/            Pure logic library (no I/O)
    cli/            CLI frontend
    ftxui/          TUI frontend (FTXUI)
    web/            Browser frontend (Emscripten/WASM)
    electron/       Desktop frontend (Electron wrapper of web/)
    tests/          Catch2 test suite
```

## Issue Workflow

- Issues are tracked in each project's `.dev-team/issues/` directory.
- When committing a fix for an issue, **always update the issue file in the same commit**: set `status: resolved`, fill in the `resolved:` date, and note the commit hash if available.
- Never commit a fix without also resolving the corresponding issue file.
- An issue stays open until the user explicitly confirms the fix works. Do not mark resolved before user approval.

## Commit Conventions

- **One commit per issue.** Do not batch multiple issue fixes into one commit. This makes fixes easy to revert, review, and trace independently.
- Commit messages must reference the issue ID (e.g., `Fix ISSUE-001: ...`).
- Multiple issues may share a commit only when explicitly requested by the user.

## Git Worktrees

When creating git worktrees for isolated work, place them **outside** the repo as siblings — never inside it. This avoids `.gitignore` pollution and follows standard practice.

```
dev/
├── dev-team/                  # main worktree
├── dev-team-no-exceptions/    # additional worktree (sibling)
└── dev-team-hotfix/           # another worktree (sibling)
```

Example: `git worktree add ../dev-team-no-exceptions -b build/no-exceptions master`

## Pre-Commit Requirements

- **All tests must pass.** Run the project's test suite and verify zero failures before committing.
- **Build must be warning-free.**
- **Auto-format all files that support it.** Run the appropriate formatter (e.g., `clang-format` for C++, `prettier` for JS/TS) before committing. Use the project's formatter configuration if present.

## Mandatory: Capture Learnings Before Committing

After completing any significant task (bug fix, feature, refactor, investigation), you MUST capture learnings BEFORE committing. This applies to all agents and to direct Claude Code sessions. Skip only for trivial changes (typo fixes, formatting-only commits).

**Before every non-trivial commit:**

1. **Reflect**: What worked? What failed? What was surprising? What knowledge would have prevented this issue or saved time?
2. **Classify**: Is the insight project-specific or generalizable?
   - Project-specific → update the relevant `CLAUDE.md` (e.g., `projects/blackjack/web/CLAUDE.md`)
   - Generalizable → write an entry to `.claude/learnings/by-{language,tool,domain}/*.md`
3. **Write the entry** using the format defined in `.claude/skills/continuous-learning/SKILL.md`
4. **Flag structural improvements**: If a coding standard, agent definition, or skill should be updated to prevent recurrence, note it for the user — don't silently move on.
5. **Then commit**.

The learning files live in `.claude/learnings/` and will appear in the commit diff alongside the code changes they document.

### What counts as a learning

- A bug caused by a gap in agent knowledge (e.g., "Embind enums are objects, not integers")
- A tool behavior that was surprising or underdocumented (e.g., "CMake splits `-s FLAG=VALUE` into two args")
- A pattern that worked well and should be reused (e.g., "dvh not vh for mobile viewports")
- A process improvement (e.g., "write layout acceptance tests alongside CSS, not after")

### What does NOT need a learning

- Typos, formatting fixes, trivial renames
- Well-known language features (don't capture "use const in JavaScript")
- Anything already documented in existing learnings (validate the existing entry instead)

## Learning System

Knowledge matures through three levels:

| Location | Maturity | Promotion criteria |
|----------|----------|--------------------|
| `.claude/learnings/` | Observation | New insight, unvalidated |
| `.claude/skills/` | Technique | High confidence, 5+ validations, 2+ projects |
| `CLAUDE.md` | Directive | 10+ validations, 3+ projects, user-approved |

See `.claude/skills/continuous-learning/SKILL.md` for the full system, entry format, and promotion rules.

## Current Projects

### Blackjack (`projects/blackjack/`)
C++20 game engine with a clean state-machine API. Frontends drive the game loop; the library is passive (no I/O, no exceptions, returns `ActionResult` enums). Four frontends exist:
- **CLI** — Text-based (`blackjack_cli`)
- **TUI** — Terminal GUI via FTXUI (`blackjack_tui`)
- **Web** — Vanilla JS + Emscripten WASM (`projects/blackjack/web/`)
- **Electron** — Desktop app wrapping the web frontend (`projects/blackjack/electron/`)

See `projects/blackjack/CLAUDE.md` for build instructions and C++ conventions.
See `projects/blackjack/web/CLAUDE.md` for web frontend specifics.
