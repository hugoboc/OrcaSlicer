# Issue tracker: Local Markdown

Issues and specs live as Markdown files in `.scratch/`.

## Conventions

- One feature per directory: `.scratch/<feature-slug>/`.
- Spec: `.scratch/<feature-slug>/spec.md`.
- Implementation tickets: `.scratch/<feature-slug>/issues/<NN>-<slug>.md`,
  numbered from `01`, with one file per ticket.
- Triage state: a `Status:` line near the top of each issue.
  Use the values in `docs/agents/triage-labels.md`.
- Comments: append under a `## Comments` heading.

## Skill operations

When publishing a spec, create or update the feature's `spec.md`.
When publishing a ticket, create its numbered file under `issues/`.
When fetching a ticket, read its referenced path; resolve bare numbers
within the current feature.

## Wayfinding operations

- Map: `.scratch/<effort>/map.md`, containing Notes, Decisions-so-far,
  and Fog.
- Children: numbered ticket files under `.scratch/<effort>/issues/`,
  with a `Type:` of `research`, `prototype`, `grilling`, or `task`.
- Wayfinding tickets use workflow statuses `open`, `claimed`, and
  `resolved`; ordinary issue triage uses the states described above.
- Dependencies: record `Blocked by: NN, NN` near the top.
  A ticket is unblocked when every listed dependency is resolved.
- Frontier: select the first open, unblocked, unclaimed ticket by number.
- Claim: save `Status: claimed` before starting work.
- Resolve: append an `## Answer`, set `Status: resolved`, and add a
  summary with a relative link to the map's Decisions-so-far.
