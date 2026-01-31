# Dev Team

A multi-agent software development team built on [Claude Code](https://docs.anthropic.com/en/docs/claude-code). Nineteen specialized agents — architect, frontend developer, security analyst, tech lead, and more — collaborate through a structured pipeline to build real software. The agents are the product; working code is the vehicle for learning.

## How It Works

Each agent has a defined role (`.claude/agents/`), an invocable skill (`.claude/skills/`), and access to the full codebase. A project manager agent orchestrates work across a multi-stage pipeline: ideation, architecture, engineering standards, design review, implementation, testing, security review, documentation, audit, and release.

Agents accumulate knowledge as they work. Learnings are captured in `.claude/learnings/` and mature through three levels:

| Location | Maturity | Promotion criteria |
|----------|----------|-------------------|
| `.claude/learnings/` | Observation | New insight, unvalidated |
| `.claude/skills/` | Technique | High confidence, 5+ validations, 2+ projects |
| `CLAUDE.md` | Directive | 10+ validations, 3+ projects, user-approved |

## Projects

- **[Blackjack](blackjack/)** — C++20 game engine with four frontends (CLI, TUI, Web/WASM, Electron), demonstrating shared-library architecture where a passive state machine drives multiple UI targets.

## Repository Structure

```
.claude/
  agents/       19 agent definitions
  skills/       21 callable skills
  learnings/    Accumulated knowledge (by-language, by-domain, by-tool)

.dev-team/
  architecture.md       System architecture
  api-contracts.md      Public API contracts
  coding-standards.md   C++ and web coding standards
  git-workflow.md       Branching and commit conventions
  specifications/       Feature specifications
  issues/               Issue tracker
  ux/                   UX specifications

blackjack/              C++ game engine + frontends
docs/                   Process documentation and templates
```

## Agents

| Agent | Role |
|-------|------|
| Architect | System design, specifications, data models, API contracts |
| Backend Developer | APIs, business logic, services |
| Code Reviewer | Standards compliance, correctness, review reports |
| DevOps Engineer | CI/CD pipelines, build systems, deployment |
| Frontend Developer | UI components, client-side logic, accessibility |
| Infrastructure Developer | IaC, cloud config, containerization |
| Project Manager | Orchestration, planning, pipeline advancement |
| QA Tester | Behavioral and exploratory testing |
| Release Manager | Versioning, changelogs, release notes |
| Security Analyst | Threat modeling, OWASP review, vulnerability analysis |
| Strategic Advisor | Market, user, and business perspective critic |
| Tech Lead | Coding standards, git workflow, technical decisions |
| Technical Writer | API references, user guides, runbooks |
| Test Engineer | Test strategy, automated test suites |
| UX Designer | User flows, screen layouts, interaction specs |
| Auditor | Pre-release compliance and traceability audit |
| Database Developer | Schemas, migrations, data access layers |
| Dependency Manager | Vulnerability and license auditing of third-party packages |
| Mobile Developer | iOS, Android, cross-platform mobile apps |
