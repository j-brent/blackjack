# Phase 0: Agent Team Workflow & Interaction Model

## 1. Roster

### Human Roles

| # | Role | Level | Core Responsibility |
|---|------|-------|-------------------|
| H1 | **Visionary** | Strategic | Product owner. Has the idea, makes final product decisions. |
| H2 | **CTO** | Technical Authority | Approves engineering standards, technical strategy, architecture sign-off. |

> **Note**: H1 and H2 may be the same person. The system is designed to support two distinct humans in these roles. When a gate requires Visionary approval, it's a product decision. When it requires CTO approval, it's an engineering decision. Some gates require both.

### Agent Roles

| # | Agent | Level | Core Responsibility |
|---|-------|-------|-------------------|
| A1 | **Strategic Advisor** | Strategic | Non-technical critic. Challenges ideas from product/market/user perspective. |
| A2 | **Software Architect** | Technical Leadership | Translates vision into system design, specifications, and technical blueprints. |
| A3 | **Tech Lead** | Technical Leadership | Owns coding standards, day-to-day technical decisions, bridges Architect and Developers. |
| A4 | **Project Manager** | Coordination | Orchestrates all agent activities, tracks progress, manages dependencies. Autonomous invocation authority. |
| A5 | **UX Designer** | Design | Information architecture, interaction design, interface specifications. |
| A6 | **Frontend Developer** | Implementation | Client-side code: UI components, browser/client logic, styling, accessibility. |
| A7 | **Backend Developer** | Implementation | Server-side code: APIs, business logic, services, integrations. |
| A8 | **Database Developer** | Implementation | Schemas, queries, migrations, data pipelines, storage optimization. |
| A9 | **Infrastructure Developer** | Implementation | Infrastructure-as-code, cloud configuration, platform tooling. |
| A10 | **Mobile Developer** | Implementation | iOS, Android, or cross-platform mobile applications. |
| A11 | **Test Engineer** | Quality | Designs test strategy AND implements automated test code. |
| A12 | **Code Reviewer** | Quality Gate | Reviews code for standards compliance, correctness, design quality, test coverage. |
| A13 | **QA Tester** | Quality | Behavioral/exploratory testing of the running product. |
| A14 | **Security Analyst** | Security | Threat modeling, vulnerability analysis, security review. |
| A15 | **DevOps Engineer** | Infrastructure | CI/CD pipelines, build systems, deployment automation. |
| A16 | **Auditor** | Compliance | Code quality, safety, regulatory and standards compliance. |
| A17 | **Technical Writer** | Documentation | API docs, user guides, architecture decision records. |
| A18 | **Release Manager** | Release | Versioning, changelogs, release notes, migration guides. |
| A19 | **Dependency Manager** | Supply Chain | Third-party dependency vetting, license compliance, vulnerability monitoring. |

**Total**: 2 human roles + 19 agent roles = 21 roles

---

## 2. Authority Model

Decisions fall into two domains with clear ownership:

### Product Decisions (Visionary)
- What the product does and doesn't do
- Target audience and market positioning
- Feature priorities and scope
- UX direction and user experience trade-offs
- Release timing and go/no-go

### Engineering Decisions (CTO)
- Coding standards and guidelines (per product)
- Technology stack choices
- Architecture approval (jointly with Visionary for product impact)
- Engineering process and quality bar
- Security policy and risk acceptance

### Shared Decisions (Visionary + CTO)
- Architecture sign-off (product alignment + technical soundness)
- Trade-offs between product goals and technical constraints
- Risk acceptance when audit or security findings are non-trivial

### Agent Authority
- **Project Manager**: Can autonomously invoke any agent based on pipeline state. Does not need human approval for routine orchestration.
- **Tech Lead**: Makes day-to-day technical decisions within the Architect's design and CTO's standards. Escalates when decisions would alter architecture.
- **All other agents**: Operate within their defined scope. Escalate cross-domain issues.

---

## 3. Built-in Behaviors (All Agents)

Every agent, regardless of role, MUST exhibit these behaviors:

- **Critical Thinking**: Never rubber-stamp. Every agent challenges inputs that seem wrong, incomplete, or risky within their domain of expertise.
- **Escalation**: When an agent identifies a problem outside its domain, it escalates to the appropriate agent, Tech Lead, or to the relevant human (Visionary for product, CTO for engineering).
- **Artifact Discipline**: Every agent reads and writes artifacts in the defined standard formats (see Phase 1).
- **Traceability**: Every decision, change, or artifact links back to the specification or requirement that motivated it.
- **Scope Awareness**: Agents do not work outside their defined responsibility. A Developer does not make architectural decisions. A Tester does not write production code. The Tech Lead does not override the Architect's system design.

---

## 4. Pipeline: Idea to Deployment

```
STAGE 1: IDEATION & VALIDATION
  Visionary + Strategic Advisor
       |
       v
STAGE 2: ARCHITECTURE & SPECIFICATION
  Software Architect (+ Visionary for clarification, CTO for tech direction)
       |
       v
STAGE 3: ENGINEERING STANDARDS
  Tech Lead + Code Reviewer (CTO approves)
       |
       v
STAGE 4: DESIGN REVIEW & FEASIBILITY
  All agents review specs within their domain
       |
       v
STAGE 5: PLANNING & TASK BREAKDOWN
  Project Manager
       |
       v
STAGE 6: UX/UI DESIGN (if applicable)
  UX Designer
       |
       v
STAGE 7: IMPLEMENTATION
  Specialist Developers + Tech Lead (guidance) + DevOps (infrastructure)
       |
       v
STAGE 8: TESTING
  Test Engineer (automated) + QA Tester (behavioral)
       |
       v
STAGE 9: REVIEW & SECURITY
  Code Reviewer + Security Analyst + Dependency Manager
       |
       v
STAGE 10: DOCUMENTATION
  Technical Writer
       |
       v
STAGE 11: AUDIT & COMPLIANCE
  Auditor
       |
       v
STAGE 12: RELEASE
  Release Manager + DevOps
       |
       v
DEPLOYED
```

---

## 5. Stage Details

### Stage 1: Ideation & Validation

**Agents**: Visionary (H1), Strategic Advisor (A1)

**Purpose**: Refine the raw idea before any technical work begins. The Strategic Advisor stress-tests the idea from a non-technical perspective.

**Entry Criteria**: Visionary has an idea they want to pursue.

**Activities**:
- Visionary describes the idea, target users, and desired outcomes.
- Strategic Advisor challenges with questions like:
  - "Who specifically is this for?"
  - "What do they use today, and why would they switch?"
  - "What's the smallest version of this that would be useful?"
  - "What assumptions are you making that could be wrong?"
  - "Is this a vitamin or a painkiller?"
  - "What does the competitive landscape look like?"
  - "How will you know if this succeeds?"
- Visionary refines or pivots based on feedback.

**Artifacts Produced**:
- `vision-statement.md` — The refined product vision, target audience, key problems solved, success criteria.

**Exit Gate**: Visionary explicitly approves the vision statement and decides to proceed.

---

### Stage 2: Architecture & Specification

**Agents**: Software Architect (A2, primary), Visionary (H1, for clarification), CTO (H2, for technical direction)

**Purpose**: Transform the vision into concrete, unambiguous technical specifications.

**Entry Criteria**: Approved vision statement exists.

**Activities**:
- Architect reads the vision statement.
- Architect asks the Visionary clarifying questions until ambiguity is eliminated.
- Architect consults CTO on technology choices, infrastructure strategy, and scalability approach.
- Architect produces system design, component specifications, data models, API contracts, and integration points.
- Architect identifies technical risks and trade-offs, presents options to Visionary (product) and CTO (engineering) for decisions.

**Artifacts Produced**:
- `architecture.md` — System design, component diagram, technology choices with rationale.
- `specifications/` — Directory of feature specifications, each with:
  - Functional requirements (what it must do)
  - Non-functional requirements (performance, security, accessibility)
  - Acceptance criteria (how to verify it works)
  - Constraints and assumptions
- `data-model.md` — Entity definitions, relationships, storage strategy.
- `api-contracts.md` — Endpoint definitions, request/response schemas, error codes (if applicable).
- `technical-risks.md` — Known risks, mitigations, open questions.

**Exit Gate**: Visionary approves the specifications (product alignment). CTO approves the architecture (technical soundness). No unresolved ambiguities remain.

---

### Stage 3: Engineering Standards

**Agents**: Tech Lead (A3, primary), Code Reviewer (A12, input), CTO (H2, approval)

**Purpose**: Define the coding standards, conventions, and engineering practices for this specific product. This happens once per project (and is updated as needed).

**Entry Criteria**: Architecture is approved. Technology stack is known.

**Activities**:
- Tech Lead drafts the coding standards document, covering:
  - Language-specific style and conventions
  - Project structure and file organization
  - Naming conventions (files, functions, variables, classes, etc.)
  - Error handling patterns
  - Logging standards
  - Git workflow (branching strategy, commit message format, PR conventions)
  - Code comment expectations
  - Dependency management rules
- Code Reviewer provides input on enforceability — are these standards reviewable? Are they measurable?
- Tech Lead finalizes the document.

**Artifacts Produced**:
- `coding-standards.md` — The authoritative coding standards for this project.
- `git-workflow.md` — Branching strategy, commit conventions, PR process.

**Exit Gate**: CTO approves the coding standards. (This is the "human in the loop" gate for engineering decisions.)

---

### Stage 4: Design Review & Feasibility

**Agents**: ALL agents review specs within their domain

**Purpose**: Every specialist reviews the specs through their lens before implementation begins. This catches problems early when they're cheap to fix.

**Entry Criteria**: Approved specifications and coding standards exist.

**Activities**:
- **Security Analyst** (A14): Reviews for threat surface, authentication/authorization gaps, data exposure risks.
- **Test Engineer** (A11): Reviews for testability — are acceptance criteria measurable? Are there edge cases not covered?
- **DevOps Engineer** (A15): Reviews for deployability — are there infrastructure assumptions? Scaling concerns?
- **UX Designer** (A5): Reviews for user experience gaps — are interaction flows defined? Are error states handled?
- **Auditor** (A16): Reviews for compliance gaps — are there regulatory requirements not addressed?
- **Dependency Manager** (A19): Reviews for third-party needs — what libraries/services are assumed?
- **Tech Lead** (A3): Reviews for implementability — are the standards feasible given the architecture?
- Each agent produces a review document with findings categorized as:
  - **BLOCKER**: Must be resolved before proceeding.
  - **WARNING**: Should be addressed, but not a blocker.
  - **NOTE**: Informational, for awareness.

**Artifacts Produced**:
- `reviews/design-review-{agent}.md` — Each agent's review findings.
- Updated specifications (Architect revises based on feedback).

**Exit Gate**: Zero unresolved BLOCKERs. Visionary acknowledges all WARNINGs (product). CTO acknowledges all WARNINGs (engineering).

---

### Stage 5: Planning & Task Breakdown

**Agents**: Project Manager (A4, primary)

**Purpose**: Break the approved specifications into executable work packages.

**Entry Criteria**: Specifications have passed design review with no unresolved blockers.

**Activities**:
- PM reads all specifications, review feedback, and coding standards.
- PM creates a work breakdown structure: epics, tasks, subtasks.
- PM identifies dependencies between tasks (what must be done before what).
- PM defines the execution order and parallelization opportunities.
- PM assigns tasks to appropriate specialist agents (frontend, backend, database, infrastructure, mobile).
- PM identifies which developers are needed for this specific project (not all specialists are always required).

**Artifacts Produced**:
- `project-plan.md` — Work breakdown, task dependencies, execution order, agent assignments.

**Exit Gate**: Visionary approves the project plan.

---

### Stage 6: UX/UI Design

**Agents**: UX Designer (A5, primary), Visionary (H1, for approval)

**Purpose**: Create interface specifications that developers can implement without guessing.

**Entry Criteria**: Approved specifications and project plan exist. (Skipped for non-UI projects.)

**Activities**:
- UX Designer creates wireframes, component specifications, interaction flows.
- UX Designer defines design tokens (colors, typography, spacing) if applicable.
- UX Designer specifies responsive behavior, accessibility requirements, error states.

**Artifacts Produced**:
- `ux/` — Directory containing:
  - Wireframes or component specs per screen/feature.
  - Interaction flow diagrams.
  - Design system / token definitions.
  - Accessibility requirements.

**Exit Gate**: Visionary approves the UX specifications.

---

### Stage 7: Implementation

**Agents**: Specialist Developers (A6-A10 as needed), Tech Lead (A3, guidance), DevOps Engineer (A15, infrastructure)

**Purpose**: Write the production code.

**Entry Criteria**: Approved project plan, specifications, coding standards, and UX designs (if applicable).

**Activities**:
- DevOps Engineer sets up the repository, CI/CD pipeline, development environment configuration.
- Tech Lead establishes the initial project structure per coding standards.
- PM assigns tasks to specialist developers per the project plan:
  - **Frontend Developer** (A6): UI components, client-side logic, styling, accessibility.
  - **Backend Developer** (A7): APIs, business logic, services, integrations.
  - **Database Developer** (A8): Schema creation, migrations, queries, data access layer.
  - **Infrastructure Developer** (A9): IaC, cloud config, platform tooling.
  - **Mobile Developer** (A10): Mobile app implementation.
- Developers write code that satisfies functional requirements, acceptance criteria, and coding standards.
- Developers create unit tests alongside production code.
- Tech Lead reviews in-progress work for standards adherence and design consistency.
- Developers commit work in logical, reviewable increments per git workflow.

**Artifacts Produced**:
- Source code in the project repository.
- Unit tests.
- DevOps configuration (CI/CD, Dockerfiles, IaC as needed).

**Exit Gate**: All planned tasks are implemented. Code compiles/builds. Unit tests pass. Tech Lead confirms standards adherence.

---

### Stage 8: Testing

**Agents**: Test Engineer (A11, automated testing), QA Tester (A13, behavioral testing)

**Purpose**: Verify the implementation meets specifications and find defects.

**Entry Criteria**: Implementation is complete. Code builds. Unit tests pass.

**Activities**:
- **Test Engineer**:
  - Designs integration and end-to-end test strategy based on specifications.
  - Implements automated test suites.
  - Runs tests and reports results.
  - Files defect reports for failures.
- **QA Tester**:
  - Performs behavioral/exploratory testing of the running product.
  - Tests edge cases, error handling, user workflows.
  - Verifies acceptance criteria from specifications.
  - Files defect reports for unexpected behavior.

**Artifacts Produced**:
- `test-strategy.md` — Test plan, coverage goals, test categories.
- Automated test suites in the repository.
- `defects/` — Defect reports with reproduction steps, expected vs. actual behavior, severity.

**Exit Gate**: All automated tests pass. No unresolved critical/high defects. Acceptance criteria verified.

---

### Stage 9: Review & Security

**Agents**: Code Reviewer (A12), Security Analyst (A14), Dependency Manager (A19)

**Purpose**: Gate check before code is considered release-worthy.

**Entry Criteria**: Testing is complete with acceptable results.

**Activities**:
- **Code Reviewer**:
  - Reviews all code against the `coding-standards.md`.
  - Verifies test coverage is adequate.
  - Checks for anti-patterns, code smells, unnecessary complexity.
  - Produces review findings with required changes.
- **Security Analyst**:
  - Performs security review of the codebase.
  - Checks for OWASP Top 10 vulnerabilities.
  - Reviews authentication, authorization, data handling.
  - Checks for secrets/credentials in code.
  - Produces security findings.
- **Dependency Manager**:
  - Audits all third-party dependencies.
  - Checks for known vulnerabilities (CVEs).
  - Verifies license compatibility.
  - Flags unmaintained or risky dependencies.

**Artifacts Produced**:
- `reviews/code-review.md` — Code review findings.
- `reviews/security-review.md` — Security findings.
- `reviews/dependency-audit.md` — Dependency audit results.

**Exit Gate**: All required code changes made. No unresolved critical security findings. No license violations. CTO reviews and accepts any security risk findings.

---

### Stage 10: Documentation

**Agents**: Technical Writer (A17)

**Purpose**: Produce all necessary documentation for users, developers, and operators.

**Entry Criteria**: Code has passed review. Implementation is stable.

**Activities**:
- Writes/generates API documentation.
- Writes user-facing documentation (guides, tutorials, FAQs).
- Writes developer documentation (setup, contributing, architecture overview).
- Writes operational documentation (deployment, monitoring, troubleshooting).
- Creates/updates CHANGELOG.

**Artifacts Produced**:
- `docs/` — All documentation artifacts appropriate to the project.

**Exit Gate**: Documentation covers all public interfaces and user-facing features. Technical accuracy verified.

---

### Stage 11: Audit & Compliance

**Agents**: Auditor (A16)

**Purpose**: Final compliance and quality gate before release.

**Entry Criteria**: Documentation is complete. All reviews resolved.

**Activities**:
- Verifies code quality metrics meet standards.
- Checks regulatory compliance (GDPR, HIPAA, SOC2 — as applicable).
- Verifies all specifications have been implemented (traceability matrix).
- Confirms all defects have been resolved or explicitly deferred.
- Produces audit report with pass/fail determination.

**Artifacts Produced**:
- `audit-report.md` — Compliance status, quality metrics, traceability matrix, pass/fail.

**Exit Gate**: Audit passes. Or: Visionary + CTO explicitly accept identified risks and proceed.

---

### Stage 12: Release

**Agents**: Release Manager (A18, primary), DevOps Engineer (A15, deployment)

**Purpose**: Package and deploy the product.

**Entry Criteria**: Audit passed (or risks accepted).

**Activities**:
- **Release Manager**:
  - Determines version number per versioning strategy.
  - Finalizes CHANGELOG and release notes.
  - Creates migration guide if applicable.
  - Tags the release in version control.
- **DevOps Engineer**:
  - Executes the deployment pipeline.
  - Verifies deployment success (smoke tests, health checks).
  - Confirms rollback procedures are in place.

**Artifacts Produced**:
- Tagged release in version control.
- Release notes.
- Deployment confirmation.

**Exit Gate**: Product is deployed and verified operational. Visionary is notified.

---

## 6. Escalation Protocol

When an agent encounters a problem:

1. **Within domain**: Agent resolves it and documents the decision.
2. **Implementation question**: Escalate to Tech Lead.
3. **Cross-domain**: Agent escalates to the relevant specialist agent.
4. **Architectural impact**: Any agent escalates to the Architect if a change would affect system design.
5. **Standards question**: Escalate to Tech Lead, then CTO if unresolved.
6. **Scope/priority dispute**: Escalate to Project Manager.
7. **Product direction dispute**: Escalate to Visionary. The Visionary always has final say on product.
8. **Engineering standards dispute**: Escalate to CTO. The CTO always has final say on engineering.
9. **Product vs. Engineering conflict**: Visionary and CTO resolve jointly. If deadlocked, Visionary wins (it's their product).

Escalations are documented in the artifact that triggered them, with a clear record of the question asked, options presented, and decision made.

---

## 7. Feedback Loops

The pipeline is NOT purely linear. These feedback loops exist:

- **Defects found in Testing (Stage 8)** loop back to **Implementation (Stage 7)**.
- **Issues found in Review (Stage 9)** loop back to **Implementation (Stage 7)** or **Architecture (Stage 2)** depending on severity.
- **Audit failures (Stage 11)** loop back to the relevant earlier stage.
- **Post-deployment issues** loop back to **Stage 1** as new work items.

The **Project Manager** tracks all loops and ensures they converge (i.e., issues get resolved, not endlessly bounced).

The **Tech Lead** is involved in all implementation feedback loops, ensuring fixes maintain standards and design consistency.

---

## 8. Artifact Directory Structure

All projects managed by this agent team use a standard directory layout for workflow artifacts:

```
project-root/
  .dev-team/                    # Agent team artifacts (version-controlled, not shipped)
    vision-statement.md
    architecture.md
    data-model.md
    api-contracts.md
    technical-risks.md
    coding-standards.md
    git-workflow.md
    specifications/
      feature-001-*.md
      feature-002-*.md
      ...
    ux/
      wireframes/
      design-system.md
      interaction-flows.md
    project-plan.md
    test-strategy.md
    reviews/
      design-review-{agent}.md
      code-review.md
      security-review.md
      dependency-audit.md
    defects/
      defect-001.md
      ...
    audit-report.md
    release-notes/
      v*.md
  docs/                         # Shipped documentation
    ...
  src/                          # Source code (structure varies by project)
    ...
  tests/                        # Test code
    ...
```

---

## 9. Implementation in Claude Code

Each agent is implemented using **two components**:

### Subagents (`.claude/agents/`)

Each agent has a **subagent definition** (`.claude/agents/<agent-name>.md`) that defines:

- **System prompt**: The agent's expertise, personality, behavioral rules, scope boundaries, and built-in critical thinking. This is the markdown body of the file.
- **Tools**: Which Claude Code tools the agent can use (frontmatter `tools` field):
  - **Developers, Tech Lead, DevOps**: `Read, Write, Edit, Glob, Grep, Bash`
  - **Reviewers, Analysts, Auditor**: `Read, Glob, Grep, Write` (Write limited to review artifacts)
  - **PM**: `Read, Write, Glob, Grep, Bash`
  - **Strategic Advisor**: `Read, Write, Glob, Grep`
- **Model**: Which AI model to use (frontmatter `model` field).
- **Permissions**: Permission mode for the agent (frontmatter `permissionMode` field).
- **Skills**: Any skills to preload into the agent's context (frontmatter `skills` field).

The subagent defines **WHO** the agent is.

### Skills (`.claude/skills/`)

Each agent has one or more **skills** (`.claude/skills/<skill-name>/SKILL.md`) that define:

- **Invocation**: How the agent is triggered — via `/skill-name` slash command or auto-invoked by Claude.
- **Task prompt**: The specific workflow the agent executes when invoked. Uses `context: fork` and `agent: <subagent-name>` to run in the subagent's context.
- **Arguments**: Dynamic input via `$ARGUMENTS` placeholder.
- **Supporting files**: Templates, reference docs, or scripts bundled in the skill directory.

The skill defines **WHAT** task to perform.

### File Structure

```
.claude/
  agents/
    architect.md              # Subagent: WHO the architect is
    strategic-advisor.md
    tech-lead.md
    project-manager.md
    frontend-developer.md
    backend-developer.md
    database-developer.md
    infrastructure-developer.md
    mobile-developer.md
    test-engineer.md
    code-reviewer.md
    qa-tester.md
    security-analyst.md
    devops-engineer.md
    auditor.md
    ux-designer.md
    technical-writer.md
    release-manager.md
    dependency-manager.md
  skills/
    architect/
      SKILL.md                # Skill: WHAT the architect does when invoked
    strategic-advisor/
      SKILL.md
    ... (one skill directory per agent)
```

### Human Roles in Claude Code

- **Visionary**: The primary user of Claude Code. Invokes agents via `/skill-name` or delegates to PM.
- **CTO**: When the same person, they simply respond to CTO-gated questions. When a different person, the system prompts for CTO input at defined gates (Stage 2 architecture approval, Stage 3 coding standards approval, Stage 9 security risk acceptance, Stage 11 audit risk acceptance).

### PM Autonomy

The PM agent has special authority:
- Can invoke any other agent's skill without human approval.
- Can assign tasks to specialist developers.
- Cannot override human decisions (Visionary or CTO gates still require human approval).
- Reports progress to both Visionary and CTO.
- Detects when the pipeline is stuck and escalates.

---

## 10. Resolved Decisions

| Decision | Resolution | Decided By |
|----------|-----------|------------|
| PM autonomy | PM can autonomously invoke agents | Visionary |
| Developer specialization | Five specialists: Frontend, Backend, Database, Infrastructure, Mobile | Visionary |
| Coding standards | Defined per product by Tech Lead + Code Reviewer, approved by CTO | Visionary |
| Artifact location | Inside project repo (`.dev-team/` directory, version-controlled) | Visionary |
| Test role | Single Test Engineer (designs + implements) | Visionary |
| Critical thinking | Baked into every agent AND standalone Strategic Advisor at Visionary level | Visionary |
| Human roles | Visionary (product) + CTO (engineering), may be same person | Visionary |
| Implementation | Subagents (`.claude/agents/`) for identity + Skills (`.claude/skills/`) for workflows | Visionary + CTO |
