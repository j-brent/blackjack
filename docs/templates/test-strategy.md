<!-- metadata
id: TEST-001
type: test-strategy
status: draft
author: A11-test-engineer
created: YYYY-MM-DD
updated: YYYY-MM-DD
approved-by: H2-cto
dependencies: [ARCH-001, all SPEC-NNN, STD-001]
-->

# Test Strategy: [Product Name]

## Overview
<!-- What is the overall approach to testing this product? -->

## Test Scope

### In Scope

| Specification | Features Covered | Test Types |
|--------------|-----------------|-----------|
| SPEC-NNN | | unit / integration / e2e / security / performance |

### Out of Scope
<!-- What is explicitly not being tested and why? -->

## Test Pyramid

| Layer | Scope | Quantity | Speed | Owner |
|-------|-------|----------|-------|-------|
| Unit | Individual functions/methods | High | Fast | Developers (during Stage 7) |
| Integration | Component interactions, API contracts | Medium | Medium | Test Engineer |
| End-to-End | Full user workflows | Low | Slow | Test Engineer |
| Behavioral/Exploratory | UX, edge cases, real usage | Varies | Manual | QA Tester |

## Coverage Targets

| Type | Target | Measurement |
|------|--------|-------------|
| Line coverage | | |
| Branch coverage | | |
| Specification coverage | 100% of acceptance criteria | Traceability matrix |

## Test Environment

| Environment | Purpose | Data | Infrastructure |
|------------|---------|------|---------------|
| Unit test | Isolated tests | Mocked | Local / CI |
| Integration test | Service interaction | Test fixtures | |
| E2E test | Full system | Seeded test data | |
| Performance test | Load/stress | Realistic volume | |

## Test Data Strategy
<!-- How is test data created, managed, and cleaned up? -->

- **Unit tests**: [approach]
- **Integration tests**: [approach]
- **E2E tests**: [approach]

## Specification Traceability Matrix

| Spec | Acceptance Criteria | Test ID(s) | Status |
|------|-------------------|-----------|--------|
| SPEC-NNN | AC-1 | TEST-NNN | not started / in progress / pass / fail |
| SPEC-NNN | AC-2 | TEST-NNN | |

## Test Categories

### Functional Tests
<!-- Tests that verify the system does what the specs say. -->

| Test ID | Spec Reference | Description | Type | Priority |
|---------|---------------|-------------|------|----------|
| TEST-001 | SPEC-NNN, AC-1 | | unit / integration / e2e | CRITICAL / HIGH / MEDIUM / LOW |

### Non-Functional Tests

| Test ID | Category | Description | Target | Type |
|---------|----------|-------------|--------|------|
| | performance / security / accessibility / reliability | | | |

### Negative Tests
<!-- Tests that verify the system handles bad input and error conditions correctly. -->

| Test ID | Scenario | Expected Behavior |
|---------|----------|------------------|
| | | |

### Edge Case Tests

| Test ID | Edge Case | Expected Behavior |
|---------|-----------|------------------|
| | | |

## Defect Management
<!-- How are defects reported, triaged, and tracked? -->

- Defect reports use the `defect-report.md` template.
- Severity follows the standard scale (CRITICAL through NOTE).
- CRITICAL and HIGH defects block release.
- MEDIUM defects require explicit acceptance by Visionary or CTO to defer.

## CI/CD Integration

| Trigger | Tests Run | Blocking |
|---------|----------|----------|
| PR created/updated | | yes / no |
| Merge to develop | | yes / no |
| Merge to main | | yes / no |
| Nightly | | no |
