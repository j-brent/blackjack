<!-- metadata
id: GIT-001
type: git-workflow
status: draft
author: A3-tech-lead
created: YYYY-MM-DD
updated: YYYY-MM-DD
approved-by: H2-cto
dependencies: [STD-001]
-->

# Git Workflow: [Product Name]

## Branching Strategy

| Branch | Purpose | Created From | Merges Into | Protected |
|--------|---------|-------------|-------------|-----------|
| main | | | | yes / no |
| develop | | | | yes / no |
| feature/* | | | | no |
| bugfix/* | | | | no |
| hotfix/* | | | | no |
| release/* | | | | yes / no |

### Branch Naming Convention
```
type/TICKET-ID-short-description
```
Examples:
- `feature/SPEC-001-user-login`
- `bugfix/DEF-042-null-pointer-dashboard`
- `hotfix/DEF-099-auth-bypass`

## Commit Message Format

```
type(scope): subject

body (optional)

footer (optional)
```

### Types
| Type | When to Use |
|------|-------------|
| feat | New feature |
| fix | Bug fix |
| refactor | Code change that neither fixes a bug nor adds a feature |
| test | Adding or updating tests |
| docs | Documentation changes |
| chore | Build process, dependencies, tooling |
| perf | Performance improvement |
| security | Security fix |

### Rules
- Subject line: imperative mood, lowercase, no period, max 72 characters.
- Body: explain WHAT and WHY, not HOW.
- Footer: reference spec or defect IDs (e.g., `Refs: SPEC-001` or `Fixes: DEF-042`).

## Pull Request Process

### PR Title Format
```
type(scope): subject [SPEC-NNN]
```

### PR Description Template
```markdown
## Summary
[1-3 sentences describing the change]

## Specifications Addressed
- [SPEC-NNN](link)

## Changes Made
- [Bulleted list of changes]

## Testing Done
- [ ] Unit tests added/updated
- [ ] Integration tests added/updated
- [ ] Manual testing completed

## Checklist
- [ ] Code follows coding standards
- [ ] Self-reviewed
- [ ] No secrets or credentials in code
- [ ] Documentation updated (if applicable)
```

### Review Requirements

| Branch Target | Required Reviewers | Auto-merge |
|--------------|-------------------|------------|
| main | | no |
| develop | | |
| release/* | | no |

## Merge Strategy
<!-- squash / merge commit / rebase — and when to use each -->

| Scenario | Strategy |
|----------|----------|
| Feature → develop | |
| Develop → main | |
| Hotfix → main | |

## Tag Convention
```
vMAJOR.MINOR.PATCH
```
- Follow [Semantic Versioning](https://semver.org/).

## CI/CD Integration
<!-- What runs on PR? What runs on merge to main? -->

| Trigger | Actions |
|---------|---------|
| PR opened/updated | |
| Merge to develop | |
| Merge to main | |
| Tag created | |
