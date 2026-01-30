<!-- metadata
id: API-001
type: api-contract
status: draft
author: A2-architect
created: YYYY-MM-DD
updated: YYYY-MM-DD
approved-by: H2-cto
dependencies: [ARCH-001, DATA-001]
-->

# API Contract: [Product Name / Service Name]

## Overview
<!-- What does this API do? Who are its consumers? -->

## Base Configuration

| Property | Value |
|----------|-------|
| Base URL | |
| API Version | |
| Protocol | REST / GraphQL / gRPC / WebSocket |
| Authentication | Bearer token / API key / OAuth2 / None |
| Content Type | application/json |
| Rate Limiting | |

## Authentication & Authorization
<!-- How do consumers authenticate? What authorization model is used? -->

## Common Headers

| Header | Required | Description |
|--------|----------|-------------|
| Authorization | yes / no | |
| Content-Type | yes | |
| X-Request-ID | no | Trace ID for request correlation |

## Common Error Format

```json
{
  "error": {
    "code": "ERROR_CODE",
    "message": "Human-readable description",
    "details": {}
  }
}
```

## Common Error Codes

| HTTP Status | Error Code | Meaning |
|------------|-----------|---------|
| 400 | BAD_REQUEST | |
| 401 | UNAUTHORIZED | |
| 403 | FORBIDDEN | |
| 404 | NOT_FOUND | |
| 409 | CONFLICT | |
| 422 | VALIDATION_ERROR | |
| 429 | RATE_LIMITED | |
| 500 | INTERNAL_ERROR | |

---

## Endpoints

### [Group Name]

#### `METHOD /path`

**Description**: [What does this endpoint do?]

**Authorization**: [Required role/permission]

**Request**:

| Parameter | Location | Type | Required | Description |
|-----------|----------|------|----------|-------------|
| | path / query / header / body | | yes / no | |

**Request Body** (if applicable):
```json
{
}
```

**Response** `200 OK`:
```json
{
}
```

**Error Responses**:

| Status | Code | Condition |
|--------|------|-----------|
| | | |

---

<!-- Repeat for each endpoint -->

## Pagination Convention
<!-- How are list endpoints paginated? -->

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| page | int | 1 | |
| per_page | int | 20 | |

**Pagination Response Envelope**:
```json
{
  "data": [],
  "pagination": {
    "page": 1,
    "per_page": 20,
    "total": 100,
    "total_pages": 5
  }
}
```

## Versioning Strategy
<!-- How is the API versioned? URL path, header, query param? -->

## Webhook Contracts (if applicable)

### [Event Name]

**Trigger**: [What causes this webhook?]

**Payload**:
```json
{
  "event": "event.name",
  "timestamp": "ISO-8601",
  "data": {}
}
```
