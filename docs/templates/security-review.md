<!-- metadata
id: SREV-NNN
type: security-review
status: draft
author: A14-security-analyst
created: YYYY-MM-DD
updated: YYYY-MM-DD
approved-by: H2-cto
dependencies: [ARCH-001, SPEC-NNN]
-->

# Security Review Report

**Reviewer**: A14 — Security Analyst
**Review Date**: YYYY-MM-DD
**Scope**: [What was reviewed — architecture, codebase, specific feature]

## Threat Model Summary
<!-- Brief overview of the threat landscape for this product. -->

### Assets
| Asset | Sensitivity | Protection Required |
|-------|-----------|-------------------|
| | HIGH / MEDIUM / LOW | |

### Threat Actors
| Actor | Motivation | Capability |
|-------|-----------|-----------|
| | | |

### Attack Surface
| Surface | Exposure | Risk Level |
|---------|----------|-----------|
| | public / internal / authenticated | |

## OWASP Top 10 Assessment

| # | Category | Status | Notes |
|---|----------|--------|-------|
| A01 | Broken Access Control | PASS / FAIL / N/A | |
| A02 | Cryptographic Failures | PASS / FAIL / N/A | |
| A03 | Injection | PASS / FAIL / N/A | |
| A04 | Insecure Design | PASS / FAIL / N/A | |
| A05 | Security Misconfiguration | PASS / FAIL / N/A | |
| A06 | Vulnerable Components | PASS / FAIL / N/A | |
| A07 | Auth Failures | PASS / FAIL / N/A | |
| A08 | Software/Data Integrity Failures | PASS / FAIL / N/A | |
| A09 | Logging/Monitoring Failures | PASS / FAIL / N/A | |
| A10 | Server-Side Request Forgery | PASS / FAIL / N/A | |

## Authentication & Authorization Review

| Check | Status | Notes |
|-------|--------|-------|
| Auth mechanism is secure | | |
| Sessions managed properly | | |
| Password policy enforced | | |
| Token handling secure | | |
| Role-based access enforced | | |
| Privilege escalation prevented | | |

## Data Protection Review

| Check | Status | Notes |
|-------|--------|-------|
| PII identified and classified | | |
| Encryption at rest | | |
| Encryption in transit | | |
| Secrets management | | |
| No credentials in code | | |
| Data sanitization on output | | |
| Input validation at boundaries | | |

## Findings

### FINDING-NNN: [Title]

| Property | Value |
|----------|-------|
| **Severity** | CRITICAL / HIGH / MEDIUM / LOW |
| **OWASP Category** | A01-A10 / other |
| **CWE** | [CWE ID if applicable] |
| **File(s)** | [file path(s)] |
| **Exploitability** | Easy / Moderate / Difficult |
| **Impact** | Data breach / Unauthorized access / Service disruption / Information disclosure |

**Vulnerability**: [Description of the vulnerability]

**Proof of Concept**: [How this could be exploited — conceptual, not weaponized]

**Recommendation**: [Specific remediation steps]

**Status**: open / resolved

---

<!-- Repeat for each finding -->

## Findings Summary

| # | Title | Severity | OWASP | Exploitability | Status |
|---|-------|----------|-------|---------------|--------|
| FINDING-001 | | | | | |

## Overall Verdict

- [ ] **PASS**: No critical or high security findings.
- [ ] **CONDITIONAL PASS**: Medium findings exist. CTO must accept risk.
- [ ] **FAIL**: Critical or high findings. Must be resolved before release.

## Sign-off

| Reviewer | Verdict | Date |
|----------|---------|------|
| A14 — Security Analyst | | |
| H2 — CTO (risk acceptance) | | |
