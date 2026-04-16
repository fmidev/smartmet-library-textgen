# Story "roadcondition_shortview"

> **Status:** Active. Shorter variant of
> [`roadcondition_overview`](roadcondition_overview.md).
>
> **Owner:** `RoadStory::condition_shortview()`.
> **Implementation:** `textgen/roadcondition_shortview.cpp` (~647 LOC).

## What it produces

Compact road-surface-condition summary (icy / partly icy / frost / slush /
snow / wet / moist / dry) for a short forecast window. Same condition
vocabulary as [`roadcondition_overview`](roadcondition_overview.md) but
fewer time phrases.

## Configuration root

```
textgen::[section]::story::roadcondition_shortview::*
```

Shares the `generally_limit`, `manyplaces_limit`, and `someplaces_limit`
variables documented on the
[`roadcondition_overview`](roadcondition_overview.md) page.
