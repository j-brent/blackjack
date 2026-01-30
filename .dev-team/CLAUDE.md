# Dev Team Project Instructions

## Project Structure

- `.dev-team/coding-standards.md` — Coding standards
- `.dev-team/issues/` — Issue tracker (one markdown file per issue)
- `.dev-team/` — Architecture and design documents (ARCH-*.md)
- `.claude/agents/` — Agent definitions
- `.claude/skills/` — Shared skill definitions

## Issue Workflow

- Issues are tracked in `.dev-team/issues/`.
- When committing a fix for an issue, **always update the issue file in the same commit**: set `status: resolved`, fill in the `resolved:` date, and note the commit hash if available.
- Never commit a fix without also resolving the corresponding issue file.
- An issue stays open until the user explicitly confirms the fix works. Do not mark resolved before user approval.

## Commit Conventions

- **One commit per issue.** Do not batch multiple issue fixes into one commit. This makes fixes easy to revert, review, and trace independently.
- Commit messages must reference the issue ID (e.g., `Fix ISSUE-001: ...`).
- Multiple issues may share a commit only when explicitly requested by the user.

## Pre-Commit Requirements

- **All tests must pass.** Run the project's test suite and verify zero failures before committing.
- **Build must be warning-free.**
- **Auto-format all files that support it.** Run the appropriate formatter (e.g., `clang-format` for C++, `prettier` for JS/TS) before committing. Use the project's formatter configuration if present.
