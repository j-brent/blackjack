<!-- metadata
id: STD-001
type: coding-standards
status: draft
author: A3-tech-lead
created: YYYY-MM-DD
updated: YYYY-MM-DD
approved-by: H2-cto
dependencies: [ARCH-001]
-->

# Coding Standards: [Product Name]

## Language & Runtime

| Property | Value |
|----------|-------|
| Primary Language(s) | |
| Runtime/Framework | |
| Language Version | |
| Package Manager | |

## Project Structure
<!-- Define the directory layout and what goes where. -->

```
project-root/
  src/
    [define structure]
  tests/
    [define structure]
```

**Rules**:
- [e.g., "One component per file", "Group by feature not by type"]

## Naming Conventions

| Element | Convention | Example |
|---------|-----------|---------|
| Files | | |
| Directories | | |
| Functions/Methods | | |
| Variables | | |
| Constants | | |
| Classes/Types | | |
| Interfaces | | |
| Database tables | | |
| API endpoints | | |
| Test files | | |

## Code Style

### Formatting
<!-- Automated formatter config (Prettier, Black, gofmt, etc.) -->

| Rule | Standard |
|------|----------|
| Indentation | |
| Line length | |
| Quotes | |
| Semicolons | |
| Trailing commas | |
| Formatter tool | |
| Linter tool | |

### Imports/Dependencies
<!-- Import ordering, grouping rules. -->

### Comments
<!-- When to comment, when not to. Documentation comment format. -->

- [e.g., "Comment WHY, not WHAT"]
- [e.g., "All public APIs must have doc comments"]

## Error Handling

| Scenario | Pattern |
|----------|---------|
| Expected errors (validation, not found) | |
| Unexpected errors (bugs, infrastructure) | |
| External service failures | |
| User-facing error messages | |

## Logging

| Level | When to Use | Example |
|-------|-------------|---------|
| ERROR | | |
| WARN | | |
| INFO | | |
| DEBUG | | |

**Format**: [structured/unstructured, required fields]

## Testing Standards

| Test Type | Location | Naming | Coverage Target |
|-----------|----------|--------|----------------|
| Unit | | | |
| Integration | | | |
| E2E | | | |

**Rules**:
- [e.g., "Every public function must have at least one test"]
- [e.g., "Tests must be independent and idempotent"]

## Dependency Rules
- [e.g., "No dependencies without Tech Lead approval"]
- [e.g., "Pin exact versions, no ranges"]
- [e.g., "Prefer standard library over third-party when reasonable"]

## Security Practices
- [e.g., "Never log PII or credentials"]
- [e.g., "All user input must be validated at the boundary"]
- [e.g., "Use parameterized queries, never string concatenation for SQL"]

## Code Review Checklist
<!-- What the Code Reviewer will check against. Must be concrete and verifiable. -->

- [ ] Follows naming conventions
- [ ] Error handling follows defined patterns
- [ ] No hardcoded secrets or credentials
- [ ] Tests exist and pass
- [ ] Logging follows standards
- [ ] No unnecessary dependencies added
- [ ] Functions are focused (single responsibility)
- [ ] [Add project-specific checks]
