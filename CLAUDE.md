# Dev Team

This repository is a multi-agent software development team. The agents are the primary product — they learn and improve by building real projects. Working software is the vehicle for learning; accumulated knowledge is the real output.

## Repository Structure

```
.claude/
  agents/           19 agent definitions (architect, frontend-developer, etc.)
  skills/           21 callable skills (invoke agents for specific tasks)

.dev-team/
  CLAUDE.md             Project-level instructions (issue workflow, commit conventions)
  coding-standards.md   Coding standards (STD-001 C++, STD-002 Web)
  git-workflow.md       Branching, commit, and review conventions
  architecture.md       System architecture (ARCH-001)
  api-contracts.md      Public API contracts (API-001)
  specifications/       Feature specifications
  issues/               Issue tracker (one file per issue)
  ux/                   UX specifications

blackjack/              First project — C++ game engine + multiple frontends
  lib/                  Pure logic library (no I/O)
  cli/                  CLI frontend
  ftxui/                TUI frontend (FTXUI)
  web/                  Browser frontend (Emscripten/WASM)
  tests/                Catch2 test suite
```

## Mandatory: Capture Learnings Before Committing

After completing any significant task (bug fix, feature, refactor, investigation), you MUST capture learnings BEFORE committing. This applies to all agents and to direct Claude Code sessions. Skip only for trivial changes (typo fixes, formatting-only commits).

**Before every non-trivial commit:**

1. **Reflect**: What worked? What failed? What was surprising? What knowledge would have prevented this issue or saved time?
2. **Classify**: Is the insight project-specific or generalizable?
   - Project-specific → update the relevant `CLAUDE.md` (e.g., `blackjack/web/CLAUDE.md`)
   - Generalizable → write an entry to `~/.claude/learnings/by-{language,tool,domain}/*.md`
3. **Write the entry** using the format defined in `.claude/skills/continuous-learning/SKILL.md`
4. **Flag structural improvements**: If a coding standard, agent definition, or skill should be updated to prevent recurrence, note it for the user — don't silently move on.
5. **Then commit**.

The learning files live outside this repo (`~/.claude/learnings/`), so they won't appear in the commit diff — but the reflection and capture must still happen.

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
| `~/.claude/learnings/` | Observation | New insight, unvalidated |
| `~/.claude/skills/` | Technique | High confidence, 5+ validations, 2+ projects |
| `~/.claude/CLAUDE.md` | Directive | 10+ validations, 3+ projects, user-approved |

See `.claude/skills/continuous-learning/SKILL.md` for the full system, entry format, and promotion rules.

## Current Projects

### Blackjack (`blackjack/`)
C++20 game engine with a clean state-machine API. Frontends drive the game loop; the library is passive (no I/O, no exceptions, returns `ActionResult` enums). Three frontends exist:
- **CLI** — Text-based (`blackjack_cli`)
- **TUI** — Terminal GUI via FTXUI (`blackjack_tui`)
- **Web** — Vanilla JS + Emscripten WASM (`blackjack/web/`)

See `blackjack/CLAUDE.md` for build instructions and C++ conventions.
See `blackjack/web/CLAUDE.md` for web frontend specifics.
