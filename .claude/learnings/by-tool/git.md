# Git Learnings

## [2026-01-31] Lesson: Place git worktrees outside the repo as siblings, not inside it
**ID**: a1b2c3d4
**Category**: tool
**Context**: When creating git worktrees for isolated parallel work on the same codebase
**Learning**: Always create worktrees as sibling directories outside the repo (`git worktree add ../repo-name-feature -b branch-name base`), never inside the repo (`git worktree add worktrees/feature`). Placing worktrees inside the repo creates an orphan directory that requires `.gitignore` entries and can confuse other tools. Sibling placement is standard practice and avoids polluting the repo.
**Evidence**: Created `worktrees/no-exceptions/` inside the repo during no-exceptions enforcement work. After cleanup, an empty `worktrees/` directory remained and raised the question of whether it should be in `.gitignore` — a sign it was in the wrong location.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team
