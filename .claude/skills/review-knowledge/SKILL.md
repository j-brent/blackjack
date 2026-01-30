---
name: review-knowledge
description: Comprehensive review and maintenance of the learning system. Use periodically or when prompted to review accumulated knowledge.
---

# Knowledge Review

Perform a comprehensive review of the learning system.

## Steps

1. **Scan learnings/**
   - List promotion candidates (high confidence, 5+ validations, 2+ projects)
   - List prune candidates (low confidence, 0 validations, 30+ days old)
   - List disputed entries needing resolution

2. **Scan skills/**
   - List CLAUDE.md promotion candidates (10+ validations, 3+ projects)
   - Identify stale or contradicted techniques

3. **Check file sizes**
   - Flag files exceeding 50 entries for curation

4. **Generate report**

   === Knowledge Review [YYYY-MM-DD] ===
   
   📈 Promotion Candidates (learnings → skills):
   - [<id>]: "<title>" → skills/<file>.md (N validations, M projects)
   
   🎯 CLAUDE.md Candidates (require approval):
   - "<skill title>" (N validations, M projects)
   
   🗑️ Prune Candidates:
   - [<id>]: "<title>" — <reason>
   
   ⚠️ Disputed (needs resolution):
   - [<id>]: "<title>" (N validations, M contradictions)
   
   📚 Curation Needed:
   - <file>: N entries

5. **Prompt for actions**
   - "Execute promotions to skills/? (y/n)"
   - "Archive prune candidates? (y/n)"
   - "Run curation on flagged files? (y/n)"
   - For CLAUDE.md candidates: show evidence, request explicit approval

6. **Log all actions to `learnings/_maintenance.log`**
   [YYYY-MM-DD HH:MM] ACTION: <promoted|pruned|curated|merged> <target> <details>
   