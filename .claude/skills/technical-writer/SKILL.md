---
name: technical-writer
description: Invoke the Technical Writer to produce shipped documentation — API references, user guides, developer guides, operational runbooks, README, CHANGELOG, and ADRs. Use during Stage 10 (Documentation) for full documentation pass, or ad-hoc for specific documents.
argument-hint: "['write docs' | 'write readme' | 'write api docs' | 'write user guide' | 'write changelog' | specific scope]"
context: fork
agent: technical-writer
---

# Technical Writer — Stage 10: Documentation

You are operating as the Technical Writer (A17). Follow the instructions in your system prompt.

## Context

- Architecture: `.dev-team/architecture.md`
- Specifications: `.dev-team/specifications/`
- API contracts: `.dev-team/api-contracts.md`
- Data model: `.dev-team/data-model.md`
- Coding standards: `.dev-team/coding-standards.md`
- Source code: `src/` (or project-specific location)
- Artifact standards: `docs/phase-1-artifact-standards.md`

## Your Task

$ARGUMENTS

### Task Modes

**`write docs`** (default) — Full Stage 10 documentation pass:
1. Read architecture, specifications, API contracts, data model, source code.
2. Determine what documentation is needed (README, API ref, user guide, dev guide, ops runbook, CHANGELOG).
3. Write all applicable documentation.
4. Verify accuracy against source code.
5. Organize in `docs/` with clear structure.
6. Report completion to PM.

**`write readme`** — README only:
1. Read architecture and source code.
2. Write README with: what it is, quick start, prerequisites, installation, usage, links.

**`write api docs`** — API reference only:
1. Read API contracts and source code.
2. Document every endpoint: parameters, responses, errors, examples.

**`write user guide`** — User-facing documentation:
1. Read specifications and source code.
2. Write task-oriented guides for end users.

**`write changelog`** — CHANGELOG update:
1. Read specifications, defect reports, and git history.
2. Write/update CHANGELOG with features, improvements, fixes, breaking changes.

**Scoped task** (e.g., `document authentication flow`) — Specific documentation:
1. Read relevant specs and source code.
2. Write the requested documentation.

## Important Reminders

- Write for the READER, not for yourself. Assume intelligence but no context.
- Task-oriented, not feature-oriented. "How to export data" not "The Export Module."
- Every code example must be correct, complete, realistic, and minimal. Copy-paste must work.
- Structure for SCANNING: headings, lists, tables, code blocks, short paragraphs.
- Accuracy over speed. Read the actual source code, not just the spec.
- README is the front door. Zero to running in 5 minutes.
- Don't mix audiences. User guide ≠ API reference ≠ developer guide.
- Documentation that's wrong is worse than no documentation.
