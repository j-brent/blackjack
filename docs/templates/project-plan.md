<!-- metadata
id: PLAN-001
type: project-plan
status: draft
author: A4-project-manager
created: YYYY-MM-DD
updated: YYYY-MM-DD
approved-by: H1-visionary
dependencies: [ARCH-001, all SPEC-NNN, all DREV-NNN]
-->

# Project Plan: [Product Name]

## Scope Summary
<!-- Brief recap of what is being built, referencing the vision and specs. -->

## Active Agents

| Agent | Role | Needed for This Project | Justification |
|-------|------|------------------------|---------------|
| A6 - Frontend Developer | Frontend | yes / no | |
| A7 - Backend Developer | Backend | yes / no | |
| A8 - Database Developer | Database | yes / no | |
| A9 - Infrastructure Developer | Infrastructure | yes / no | |
| A10 - Mobile Developer | Mobile | yes / no | |
| A11 - Test Engineer | Testing | yes | Always required |
| A12 - Code Reviewer | Review | yes | Always required |
| A13 - QA Tester | QA | yes | Always required |
| A14 - Security Analyst | Security | yes | Always required |
| A15 - DevOps Engineer | DevOps | yes | Always required |

## Work Breakdown Structure

### Epic: [Epic Name] — [SPEC-NNN]

| Task ID | Task | Assigned To | Dependencies | Status |
|---------|------|-------------|-------------|--------|
| TASK-001 | | A6/A7/A8/A9/A10 | none / TASK-NNN | pending / in-progress / done / blocked |

**Acceptance Criteria Reference**: [SPEC-NNN, AC-1, AC-2, ...]

---

<!-- Repeat for each epic -->

## Dependency Graph
<!-- Which tasks block which other tasks? -->

```
TASK-001 ──→ TASK-003
TASK-002 ──→ TASK-003
TASK-003 ──→ TASK-005
TASK-004 ──→ TASK-005 (parallel with TASK-003)
```

## Execution Order

### Phase 1: [Name]
- TASK-001 (A8) — [Description]
- TASK-002 (A7) — [Description] — parallel with TASK-001

### Phase 2: [Name]
- TASK-003 (A7) — [Description] — depends on TASK-001, TASK-002

<!-- Continue phases -->

## Parallelization Opportunities
<!-- Which tasks can be worked on simultaneously? -->

| Parallel Group | Tasks | Agents |
|---------------|-------|--------|
| | | |

## Risk Items from Design Review
<!-- Carry forward any unresolved WARNINGs that affect the plan. -->

| Risk | Impact on Plan | Mitigation |
|------|---------------|------------|
| | | |

## Progress Tracking

| Metric | Current | Target |
|--------|---------|--------|
| Tasks completed | 0 / N | N / N |
| Blockers open | 0 | 0 |
| Specs fully implemented | 0 / N | N / N |

## Change Log

| Date | Change | Reason | Approved By |
|------|--------|--------|-------------|
| | | | |
