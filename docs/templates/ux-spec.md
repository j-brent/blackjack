<!-- metadata
id: UX-NNN
type: ux-spec
status: draft
author: A5-ux-designer
created: YYYY-MM-DD
updated: YYYY-MM-DD
approved-by: H1-visionary
dependencies: [SPEC-NNN]
-->

# UX Specification: [Feature / Screen Name]

## Overview
<!-- What user task does this screen/flow support? Reference the feature spec. -->

**Related Specification**: [SPEC-NNN](link)

## User Flow

```
[Step 1: Entry point] → [Step 2] → [Step 3] → [Step 4: Completion]
                                  ↘ [Error state] → [Recovery]
```

## Screen/Component Layout

### [Screen Name]

**Purpose**: [What the user accomplishes here]

**Layout Description**:
<!-- Describe the layout structure. Use ASCII wireframe or structured description. -->

```
┌─────────────────────────────────┐
│ Header / Navigation             │
├─────────────────────────────────┤
│                                 │
│  [Main Content Area]            │
│                                 │
│  ┌──────────┐ ┌──────────┐     │
│  │ Element  │ │ Element  │     │
│  └──────────┘ └──────────┘     │
│                                 │
├─────────────────────────────────┤
│ Footer / Actions                │
└─────────────────────────────────┘
```

### Component Inventory

| Component | Type | Behavior | States |
|-----------|------|----------|--------|
| | button / input / list / card / modal / etc. | | default / hover / active / disabled / error / loading |

## Interaction Specifications

| # | User Action | System Response | Transition |
|---|-------------|----------------|-----------|
| INT-1 | | | |

## States & Conditions

| State | Trigger | Display |
|-------|---------|---------|
| Empty / No data | | |
| Loading | | |
| Loaded / Success | | |
| Error | | |
| Partial / Degraded | | |

## Form Specifications (if applicable)

| Field | Type | Required | Validation | Error Message |
|-------|------|----------|-----------|---------------|
| | text / email / password / select / checkbox / etc. | yes / no | | |

**Submission Behavior**:
- On success: [what happens]
- On failure: [what happens]
- While processing: [what the user sees]

## Responsive Behavior

| Breakpoint | Layout Changes |
|-----------|---------------|
| Mobile (< 640px) | |
| Tablet (640-1024px) | |
| Desktop (> 1024px) | |

## Accessibility Requirements

| Requirement | Implementation |
|-------------|---------------|
| Keyboard navigation | |
| Screen reader support | |
| Color contrast | |
| Focus management | |
| ARIA labels | |

## Design Tokens (if applicable)

| Token | Value | Usage |
|-------|-------|-------|
| color-primary | | |
| spacing-base | | |
| font-size-body | | |

## Edge Cases

| Scenario | Expected UX |
|----------|------------|
| Very long text content | |
| Slow network | |
| No JavaScript | |
| First-time user | |
| Return user | |
