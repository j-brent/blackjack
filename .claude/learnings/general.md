# General Learnings

## [2026-01-30] Lesson: One commit per issue fix
**ID**: f9a0b1c2
**Category**: general
**Context**: When resolving tracked issues from `.dev-team/issues/`
**Learning**: Each issue fix must be a single, isolated commit. Do not batch multiple issue fixes into one commit. The commit message should reference the issue ID (e.g., "Fix ISSUE-001: ..."). This makes it easy to revert, review, and trace fixes independently.
**Evidence**: User directive during Phase 17 integration test.
**Confidence**: high
**Validations**: 1
**Projects**: dev-team
