<!-- metadata
id: DAUD-NNN
type: dependency-audit
status: draft
author: A19-dependency-manager
created: YYYY-MM-DD
updated: YYYY-MM-DD
approved-by: H2-cto
dependencies: [ARCH-001]
-->

# Dependency Audit Report

**Reviewer**: A19 — Dependency Manager
**Review Date**: YYYY-MM-DD
**Scope**: [Full project / specific component]

## Summary

| Metric | Count |
|--------|-------|
| Total dependencies (direct) | |
| Total dependencies (transitive) | |
| Known vulnerabilities | |
| License violations | |
| Unmaintained packages | |
| Outdated packages | |

## Vulnerability Assessment

### VULN-NNN: [Package Name]

| Property | Value |
|----------|-------|
| **Package** | [name@version] |
| **Vulnerability** | [CVE ID or advisory] |
| **Severity** | CRITICAL / HIGH / MEDIUM / LOW |
| **Description** | |
| **Affected Versions** | |
| **Fixed Version** | |
| **Direct/Transitive** | |
| **Exploitable in Our Context** | yes / no / uncertain |

**Recommendation**: [Upgrade / replace / accept risk / remove]

---

<!-- Repeat for each vulnerability -->

## License Compliance

### License Inventory

| License | Count | Compatible | Notes |
|---------|-------|-----------|-------|
| MIT | | yes | |
| Apache-2.0 | | yes | |
| GPL-3.0 | | depends on usage | |
| BSD-3-Clause | | yes | |
| [other] | | | |

### License Violations

| Package | License | Issue | Resolution |
|---------|---------|-------|-----------|
| | | | |

## Dependency Health

| Package | Version | Latest | Last Updated | Weekly Downloads | Risk |
|---------|---------|--------|-------------|-----------------|------|
| | | | | | healthy / stale / abandoned |

### Flagged Dependencies

| Package | Issue | Severity | Recommendation |
|---------|-------|----------|---------------|
| | unmaintained / low adoption / no alternatives / deprecated | | |

## Supply Chain Risks

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|-----------|
| Typosquatting | | | |
| Maintainer compromise | | | |
| Dependency confusion | | | |

## Recommendations

| Priority | Action | Package(s) | Rationale |
|----------|--------|-----------|-----------|
| CRITICAL | | | |
| HIGH | | | |

## Overall Verdict

- [ ] **PASS**: No critical vulnerabilities, no license violations.
- [ ] **CONDITIONAL PASS**: Issues exist but mitigated. CTO must accept risk.
- [ ] **FAIL**: Critical vulnerabilities or license violations. Must resolve.

## Sign-off

| Reviewer | Verdict | Date |
|----------|---------|------|
| A19 — Dependency Manager | | |
| H2 — CTO (risk acceptance) | | |
