<!-- metadata
id: ARCH-001
type: architecture
status: draft
author: A2-architect
created: YYYY-MM-DD
updated: YYYY-MM-DD
approved-by: both
dependencies: [VISION-001]
-->

# Architecture: [Product Name]

## Overview
<!-- High-level description of the system architecture. 2-3 paragraphs max. -->

## System Context Diagram
<!-- How does this system fit into the broader environment? What external systems does it interact with? -->

```
[Describe or embed diagram here]
```

## Component Architecture
<!-- What are the major components/services and how do they relate? -->

```
[Describe or embed component diagram here]
```

### Component Descriptions

| Component | Responsibility | Technology | Owner Agent |
|-----------|---------------|------------|-------------|
| | | | A6/A7/A8/A9/A10 |

## Technology Stack

### Chosen Technologies

| Layer | Technology | Version | Rationale |
|-------|-----------|---------|-----------|
| Frontend | | | |
| Backend | | | |
| Database | | | |
| Infrastructure | | | |
| CI/CD | | | |

### Technologies Considered but Rejected

| Technology | Reason for Rejection |
|-----------|---------------------|
| | |

## Data Flow
<!-- How does data move through the system? Key flows only. -->

### Flow: [Name]
```
[Source] → [Step 1] → [Step 2] → [Destination]
```

## Integration Points
<!-- External APIs, services, or systems this product connects to. -->

| Integration | Direction | Protocol | Auth Method | Failure Mode |
|-------------|-----------|----------|-------------|-------------|
| | inbound / outbound / bidirectional | | | |

## Non-Functional Requirements

### Performance
| Metric | Target | Measurement Method |
|--------|--------|-------------------|
| | | |

### Scalability
<!-- How does the system scale? What are the bottlenecks? -->

### Availability
<!-- Uptime target, redundancy strategy, disaster recovery approach. -->

### Security Architecture
<!-- Authentication, authorization, encryption, data protection approach. High-level only — details in specs. -->

## Deployment Architecture
<!-- How is this system deployed? Environments, infrastructure topology. -->

```
[Describe or embed deployment diagram here]
```

| Environment | Purpose | Infrastructure |
|------------|---------|---------------|
| Development | | |
| Staging | | |
| Production | | |

## Architectural Decisions

> **DECISION [YYYY-MM-DD]**: [Description]
> **Options considered**: [List]
> **Rationale**: [Why]
> **Decided by**: [H1/H2/agent]

## Constraints
<!-- Technical, business, or regulatory constraints that shaped this architecture. -->

## References
<!-- Links to relevant specifications, external docs, prior art. -->
