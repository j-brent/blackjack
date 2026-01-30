---
name: learn
description: Enables agents to learn from experiences across projects. Use after completing significant tasks to capture lessons, and periodically to validate, promote, and prune accumulated knowledge.
---

# Continuous Learning Skill

**Version**: 1.0.0  
**Last Updated**: 2025-01-30

---

## Overview

This skill enables agents to learn from their experiences across projects, accumulating validated knowledge over time. Agents capture observations, validate them through repeated application, and promote proven insights into stable skills and core directives.

---

## Knowledge Hierarchy

```
~/.claude/
  CLAUDE.md                 # Core directives (proven, universal, high-signal)
  skills/                   # Stable techniques (validated, actionable)
    debugging.md
    testing.md
    refactoring.md
    code-review.md
    <domain>.md
  learnings/                # Observations (unvalidated, evolving)
    by-language/
    by-domain/
    by-tool/
    general.md
    _archive.md
    _maintenance.log
```

### Maturity Levels

| Location | Maturity | Criteria |
|----------|----------|----------|
| `learnings/` | Observation | New insight, unvalidated |
| `skills/` | Technique | High confidence, 5+ validations, 2+ projects |
| `CLAUDE.md` | Directive | 10+ validations, 3+ projects, user-approved |

---

## Destination Rules

### Where to Write New Knowledge

| Destination | When | Examples |
|-------------|------|----------|
| `<project>/.claude/notes.md` | Insight is specific to this project | "This repo uses nonstandard X", "Team prefers Y" |
| `learnings/by-language/<lang>.md` | Language-specific gotcha or idiom | "Python's `is` vs `==` for None", "C++ iterator invalidation" |
| `learnings/by-domain/<domain>.md` | Domain knowledge | "DICOM files require X", "REST APIs should Y" |
| `learnings/by-tool/<tool>.md` | Tool-specific behavior | "Git rebase vs merge in this case", "CMake quirk" |
| `learnings/general.md` | Cross-cutting observation | "Smaller PRs get reviewed faster" |

### Decision Flow

```
Observation made
      │
      ▼
Is this project-specific? ──YES──► <project>/.claude/notes.md (STOP)
      │
      NO
      ▼
Does a similar learning exist? ──YES──► Validate or contradict existing entry
      │
      NO
      ▼
Create new entry in learnings/<category>/*.md
      │
      ▼
[Validated over time...]
      │
      ▼
High confidence + 5 validations + 2 projects? ──YES──► Promote to skills/
      │
      ▼
10+ validations + 3 projects? ──YES──► Propose promotion to CLAUDE.md (requires user approval)
```

---

## Procedures

### 1. Capture

**Trigger**: After completing any significant task (bug fix, feature, refactor, investigation).

**Steps**:
1. Reflect: What worked? What failed? What was surprising?
2. Determine if insight is project-specific or generalizable
3. If generalizable, select appropriate category
4. Append entry in this format:

```markdown
## [YYYY-MM-DD] Lesson: <descriptive title>
**ID**: <8-char-hash>
**Category**: language|domain|tool|general
**Context**: <specific conditions when this applies>
**Learning**: <the insight, actionable if possible>
**Evidence**: <what happened that taught you this>
**Confidence**: low
**Validations**: 0
**Projects**: <project-name>
```

---

### 2. Validate

**Trigger**: When you encounter a situation where a past learning is relevant.

**If the learning helps (it works):**
```markdown
**Validations**: <increment>
**Projects**: <add project if new>
**Validated**: [YYYY-MM-DD] <brief context>
```

Update confidence:
- low → medium: 2+ validations
- medium → high: 5+ validations

**If the learning fails (contradicted):**
```markdown
**Contradicted**: [YYYY-MM-DD] <what actually happened>
**Status**: disputed  # if contradictions ≥ validations
```

Downgrade confidence if repeated contradictions.

---

### 3. Promote

#### Learning → Skill

**Trigger**: Entry reaches high confidence + 5 validations + 2 different projects.

**Steps**:
1. Identify the appropriate skill file by activity:
   - `skills/debugging.md` — Diagnosis strategies
   - `skills/testing.md` — Effective test patterns
   - `skills/refactoring.md` — Safe transformation patterns
   - `skills/code-review.md` — Review techniques
   - `skills/architecture.md` — Design patterns
   - `skills/communication.md` — Explaining, clarifying
   - `skills/<domain>.md` — Domain-specific techniques

2. Rewrite as actionable technique:
```markdown
### <Technique Name>

**When to use**: <conditions>

**How to apply**: <concrete steps>

**Why it works**: <brief rationale>

---
*Promoted: YYYY-MM-DD from learning <ID> | Validations: N | Projects: list*
```

3. Mark original learning:
```markdown
**Status**: promoted to skills/<file>.md
```

#### Skill → CLAUDE.md

**Trigger**: Skill reaches 10+ validations + 3+ different projects + no recent contradictions.

**Steps**:
1. **Request user approval** — Present evidence summary:
   ```
   Proposing promotion to CLAUDE.md:
   
   Skill: <name>
   Validations: N across M projects
   Last contradicted: <date or "never">
   
   Proposed directive: "<concise actionable statement>"
   
   Approve? (y/n)
   ```

2. If approved, add to CLAUDE.md:
   ```markdown
   - <Directive> *(promoted: YYYY-MM-DD, N validations)*
   ```

3. Mark skill:
   ```markdown
   **Status**: promoted to CLAUDE.md
   ```

---

### 4. Curate

**Trigger**: File exceeds 50 entries OR 30 days since last curation.

**Actions**:
- **Merge** overlapping lessons into consolidated entries
  - Combine validation counts
  - Union project lists
  - Note: `**Merged from**: <ID1>, <ID2>`
- **Group** related lessons under descriptive headers
- **Clarify** vague entries using accumulated evidence

---

### 5. Prune

**Trigger**: During curation.

**Archive to `learnings/_archive.md` if**:
- Confidence: low AND validations: 0 AND age: 30+ days
- Contradictions > validations
- Superseded by a promoted skill

**Archive format**:
```markdown
## [Archived: YYYY-MM-DD] <original title>
**Reason**: <stale|disputed|superseded>
**Original ID**: <id>
<original content>
```

---

### 6. Periodic Review

**Trigger**: Start of session after 7+ days since last session.

**Quick scan**:
1. Identify promotion candidates
2. Identify prune candidates
3. Check for files needing curation
4. Log and report summary

---

## Companion Skill: /review-knowledge


---

## Safeguards

- **Never modify CLAUDE.md without explicit user approval**
- **Never delete**—archive to `_archive.md` instead
- **Log all maintenance actions** to `_maintenance.log`
- **When uncertain**: downgrade confidence rather than prune
- **Project-specific knowledge stays local**—never pollute global learnings
- **Validate before trusting**—even "high confidence" entries can be wrong in new contexts

---

## Initial Setup

If the directory structure doesn't exist, create it:

```bash
mkdir -p ~/.claude/skills
mkdir -p ~/.claude/learnings/by-language
mkdir -p ~/.claude/learnings/by-domain
mkdir -p ~/.claude/learnings/by-tool
mkdir -p ~/.claude/commands
touch ~/.claude/learnings/general.md
touch ~/.claude/learnings/_archive.md
touch ~/.claude/learnings/_maintenance.log
```

---

## Changelog

### 1.0.0 (2025-01-30)
- Initial version
- Knowledge hierarchy: learnings → skills → CLAUDE.md
- Capture, validate, promote, curate, prune procedures
- Companion /review-knowledge skill
- Safeguards and destination rules
