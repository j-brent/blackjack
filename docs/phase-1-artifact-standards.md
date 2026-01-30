# Phase 1: Artifact Standards

## Purpose

This document defines the format, structure, and conventions for every artifact produced by the agent team. All agents MUST use these standards. Consistency across artifacts is what allows agents to read each other's outputs without ambiguity.

## General Conventions

### File Format
- All artifacts are **Markdown** (`.md`) files.
- Use GitHub-Flavored Markdown (GFM) for tables, code blocks, and task lists.

### Naming
- All filenames use **lowercase-kebab-case**: `vision-statement.md`, `feature-001-user-login.md`.
- Feature specs are numbered sequentially: `feature-NNN-short-name.md`.
- Defect reports are numbered sequentially: `defect-NNN-short-name.md`.
- Design reviews include the agent name: `design-review-security-analyst.md`.
- Release notes include the version: `v1.0.0.md`.

### Metadata Block
Every artifact begins with a YAML-style metadata block inside an HTML comment. This is machine-readable for the PM and other agents to parse pipeline state.

```markdown
<!-- metadata
id: ARTIFACT-ID
type: vision-statement | architecture | specification | data-model | api-contract |
      technical-risks | coding-standards | git-workflow | design-review | project-plan |
      ux-spec | test-strategy | defect | code-review | security-review | dependency-audit |
      audit-report | release-notes
status: draft | in-review | approved | superseded
author: agent-id (e.g., A2-architect, A3-tech-lead, H1-visionary)
created: YYYY-MM-DD
updated: YYYY-MM-DD
approved-by: H1-visionary | H2-cto | both
dependencies: [list of artifact IDs this depends on]
-->
```

### Status Lifecycle
All artifacts follow this lifecycle:

```
draft → in-review → approved
                  → superseded (replaced by a newer version)
```

- **draft**: Being written. Not yet ready for review.
- **in-review**: Complete and submitted for review/approval.
- **approved**: Accepted by the required human(s). This is the authoritative version.
- **superseded**: Replaced by a newer version. The artifact body must link to its replacement.

### Cross-References
When referencing another artifact, use this format:
```
[SPEC-001](../specifications/feature-001-user-login.md)
```
Always use relative paths from the `.dev-team/` directory root.

### Severity/Priority Scale
Used consistently across defects, review findings, and risk assessments:

| Level | Label | Meaning |
|-------|-------|---------|
| 1 | **CRITICAL** | Blocks release. Must fix before proceeding. |
| 2 | **HIGH** | Significant issue. Must fix before release. |
| 3 | **MEDIUM** | Should fix. Can defer with explicit acceptance. |
| 4 | **LOW** | Minor issue. Fix when convenient. |
| 5 | **NOTE** | Informational. No action required. |

### Decision Records
When a decision is made during any stage, it is recorded inline using this format:

```markdown
> **DECISION [YYYY-MM-DD]**: [Description of what was decided]
> **Options considered**: [Brief list]
> **Rationale**: [Why this option was chosen]
> **Decided by**: [H1-visionary | H2-cto | agent-id]
```

---

## Artifact Templates

Each template below defines the required structure. Sections marked **(required)** must always be present. Sections marked **(if applicable)** can be omitted when not relevant.

Templates are stored in `docs/templates/` and referenced by agents when creating new artifacts.

### Template Index

| # | Artifact | Template File | Produced By | Consumed By | Stage |
|---|----------|--------------|-------------|-------------|-------|
| T1 | Vision Statement | `vision-statement.md` | Strategic Advisor + Visionary | Architect, all agents | 1 |
| T2 | Architecture | `architecture.md` | Architect | All agents | 2 |
| T3 | Feature Specification | `feature-spec.md` | Architect | Developers, Test Engineer, Reviewers | 2 |
| T4 | Data Model | `data-model.md` | Architect | Database Developer, Backend Developer | 2 |
| T5 | API Contract | `api-contract.md` | Architect | Frontend/Backend/Mobile Developers, Test Engineer | 2 |
| T6 | Technical Risks | `technical-risks.md` | Architect | PM, all agents | 2 |
| T7 | Coding Standards | `coding-standards.md` | Tech Lead | Developers, Code Reviewer | 3 |
| T8 | Git Workflow | `git-workflow.md` | Tech Lead | Developers, Code Reviewer, DevOps | 3 |
| T9 | Design Review | `design-review.md` | Any reviewing agent | Architect, PM | 4 |
| T10 | Project Plan | `project-plan.md` | Project Manager | All agents | 5 |
| T11 | UX Specification | `ux-spec.md` | UX Designer | Frontend/Mobile Developers | 6 |
| T12 | Test Strategy | `test-strategy.md` | Test Engineer | QA Tester, Developers, PM | 8 |
| T13 | Defect Report | `defect-report.md` | Test Engineer, QA Tester | Developers, PM | 8 |
| T14 | Code Review | `code-review.md` | Code Reviewer | Developers, Tech Lead | 9 |
| T15 | Security Review | `security-review.md` | Security Analyst | Developers, CTO | 9 |
| T16 | Dependency Audit | `dependency-audit.md` | Dependency Manager | Developers, CTO | 9 |
| T17 | Audit Report | `audit-report.md` | Auditor | Visionary, CTO | 11 |
| T18 | Release Notes | `release-notes.md` | Release Manager | Visionary, users | 12 |
