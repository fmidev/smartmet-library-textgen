# Story "frost_onenight"

> **Status:** Primary (modern frost product).
>
> **Owner:** `FrostStory::onenight()`.
> **Implementation:** `textgen/frost_onenight.cpp` (~834 LOC).

## What it produces

A detailed single-night frost narrative combining:

* probability of frost / severe frost
* coast vs. inland decomposition
* trend relative to the previous night
* "already obvious" suppression (if severe frost probability is very high
  the story can stay silent — it is self-evident)

## Configuration root

```
textgen::[section]::story::frost_onenight::*
```

Relevant thresholds include `severe_frost_limit`, `frost_limit`,
`obvious_frost`, `precision`, `coast_limit`, day/night definitions, and
phrase preference lists.

See `textgen/frost_onenight.cpp` for the authoritative list and
`test/FrostStoryTest.cpp` for working configurations.

## Seasonal silence

Like the other frost stories, `frost_onenight` returns an empty paragraph
between mid-October and the end of March — frost in that window is not
news.

## Related

* [`frost_day`](frost_day.md) — simpler single-night form.
* [`frost_twonights`](frost_twonights.md) — two-night comparison.
