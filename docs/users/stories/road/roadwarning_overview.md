# Story "roadwarning_overview"

> **Status:** Primary (road weather products).
>
> **Owner:** `RoadStory::warning_overview()`.
> **Implementation:** `textgen/roadwarning_overview.cpp` (~1 353 LOC).

## What it produces

A road-warning narrative combining slipperiness, poor visibility, and
strong wind over a forecast period. The story merges adjacent periods with
similar warning states and produces "from morning onwards", "in the
afternoon", … time phrases analogous to
[`roadcondition_overview`](roadcondition_overview.md).

## Configuration root

```
textgen::[section]::story::roadwarning_overview::*
```

Reads threshold variables for each warning class, day/night definitions,
`maxhours` (default 30), and phrase-preference lists. The full list is in
`textgen/roadwarning_overview.cpp` and the regression tests in
`test/RoadStoryTest.cpp`.

## Related

* [`roadcondition_overview`](roadcondition_overview.md) — road surface
  condition; usually used together with `roadwarning_overview`.
* [`roadwarning_shortview`](roadwarning_shortview.md) — shorter variant.
