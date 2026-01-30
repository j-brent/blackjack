<!-- metadata
id: AUDIT-NNN
type: audit-report
status: draft
author: A16-auditor
created: YYYY-MM-DD
updated: YYYY-MM-DD
approved-by: both
dependencies: [all artifacts]
-->

# Audit Report: [Product Name] — [Version]

**Auditor**: A16 — Software Auditor
**Audit Date**: YYYY-MM-DD
**Scope**: Pre-release audit for version [X.Y.Z]

## Executive Summary
<!-- 3-5 sentences. Overall assessment, key findings, pass/fail determination. -->

## Specification Traceability Matrix

| Spec ID | Feature | All ACs Implemented | All ACs Tested | Test Result | Status |
|---------|---------|-------------------|---------------|------------|--------|
| SPEC-001 | | yes / no / partial | yes / no / partial | pass / fail | PASS / FAIL / PARTIAL |

**Traceability Coverage**: [X of Y specifications fully traced] = [N%]

## Code Quality Assessment

| Metric | Value | Target | Status |
|--------|-------|--------|--------|
| Test coverage (line) | | | PASS / FAIL |
| Test coverage (branch) | | | PASS / FAIL |
| Static analysis issues | | 0 critical/high | PASS / FAIL |
| Code review findings resolved | | 100% | PASS / FAIL |
| Technical debt items | | | INFO |

## Security Compliance

| Check | Status | Reference |
|-------|--------|-----------|
| Security review completed | yes / no | SREV-NNN |
| Critical/High findings resolved | yes / no | |
| Dependency audit completed | yes / no | DAUD-NNN |
| No known critical CVEs | yes / no | |
| Secrets scan passed | yes / no | |

## Regulatory Compliance (if applicable)

### [Regulation Name] (e.g., GDPR, HIPAA, SOC2)

| Requirement | Implementation | Evidence | Status |
|-------------|---------------|----------|--------|
| | | | COMPLIANT / NON-COMPLIANT / N/A |

## Defect Status

| Severity | Total Found | Resolved | Deferred | Open |
|----------|-----------|----------|----------|------|
| CRITICAL | | | 0 (not deferrable) | 0 (blocks release) |
| HIGH | | | | 0 (blocks release) |
| MEDIUM | | | | |
| LOW | | | | |

### Deferred Defects (require explicit acceptance)

| Defect ID | Severity | Reason for Deferral | Accepted By |
|-----------|----------|-------------------|-------------|
| | | | H1 / H2 |

## Documentation Completeness

| Document | Exists | Current | Accurate | Status |
|----------|--------|---------|----------|--------|
| API documentation | yes / no | yes / no | yes / no | PASS / FAIL |
| User documentation | yes / no | yes / no | yes / no | PASS / FAIL |
| Developer documentation | yes / no | yes / no | yes / no | PASS / FAIL |
| Operational runbook | yes / no | yes / no | yes / no | PASS / FAIL |

## Process Compliance

| Stage | Completed | Artifacts Present | Gates Passed |
|-------|----------|------------------|-------------|
| 1 - Ideation | yes / no | vision-statement.md | Visionary approved |
| 2 - Architecture | yes / no | architecture.md, specs, data-model, api-contracts | Visionary + CTO approved |
| 3 - Standards | yes / no | coding-standards.md, git-workflow.md | CTO approved |
| 4 - Design Review | yes / no | design-review-*.md | Zero blockers |
| 5 - Planning | yes / no | project-plan.md | Visionary approved |
| 6 - UX Design | yes / no / skipped | ux/*.md | Visionary approved |
| 7 - Implementation | yes / no | source code, unit tests | Tech Lead confirmed |
| 8 - Testing | yes / no | test-strategy.md, test suites, defect reports | Tests pass |
| 9 - Review & Security | yes / no | code-review.md, security-review.md, dependency-audit.md | All resolved |
| 10 - Documentation | yes / no | docs/* | Accuracy verified |

## Findings

### FINDING-NNN: [Title]

| Property | Value |
|----------|-------|
| **Severity** | CRITICAL / HIGH / MEDIUM / LOW / NOTE |
| **Category** | traceability / quality / security / compliance / documentation / process |

**Issue**: [Description]

**Recommendation**: [What should be done]

**Status**: open / resolved / accepted

---

## Overall Verdict

- [ ] **PASS**: All criteria met. Approved for release.
- [ ] **CONDITIONAL PASS**: Minor issues exist. Visionary + CTO must accept risk.
- [ ] **FAIL**: Critical issues. Cannot release until resolved.

## Sign-off

| Role | Name/ID | Verdict | Date |
|------|---------|---------|------|
| A16 — Auditor | | | |
| H1 — Visionary | | | |
| H2 — CTO | | | |
