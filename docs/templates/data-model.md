<!-- metadata
id: DATA-001
type: data-model
status: draft
author: A2-architect
created: YYYY-MM-DD
updated: YYYY-MM-DD
approved-by: H2-cto
dependencies: [ARCH-001]
-->

# Data Model: [Product Name]

## Overview
<!-- High-level description of the data architecture. Storage strategy, key design principles. -->

## Entity Definitions

### [Entity Name]

| Field | Type | Constraints | Description |
|-------|------|------------|-------------|
| id | | PK, auto-generated | |
| | | required / unique / nullable / FK | |

**Indexes**:
| Name | Fields | Type | Purpose |
|------|--------|------|---------|
| | | unique / composite / partial | |

**Business Rules**:
- [Rule 1]

---

<!-- Repeat for each entity -->

## Entity Relationship Diagram

```
[Describe or embed ERD here]
```

## Relationships

| From Entity | Relationship | To Entity | Cardinality | Cascade Behavior |
|------------|-------------|-----------|-------------|-----------------|
| | has_many / belongs_to / has_one / many_to_many | | 1:1 / 1:N / M:N | |

## Storage Strategy

| Data Category | Storage Type | Technology | Rationale |
|--------------|-------------|------------|-----------|
| Transactional | relational / document / key-value | | |
| Cache | | | |
| Files/Blobs | | | |
| Search | | | |

## Migration Strategy
<!-- How will schema changes be managed over time? -->

## Data Retention & Archival
<!-- How long is data kept? What gets archived? What gets deleted? -->

| Data Category | Retention Period | Archival Strategy | Deletion Policy |
|--------------|-----------------|-------------------|----------------|
| | | | |

## Seed Data
<!-- What initial data is needed for the system to function? -->

## Data Security

| Data Classification | Encryption at Rest | Encryption in Transit | Access Control |
|--------------------|-------------------|----------------------|---------------|
| PII | | | |
| Credentials | | | |
| Business data | | | |
