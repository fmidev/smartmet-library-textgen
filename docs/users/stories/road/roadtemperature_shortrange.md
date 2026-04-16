# Story "roadtemperature_shortrange"

> **Status:** Active. Compact variant of
> [`roadtemperature_daynightranges`](roadtemperature_daynightranges.md).
>
> **Owner:** `RoadStory::temperature_shortrange()`.
> **Implementation:** `textgen/roadtemperature_shortrange.cpp` (~118 LOC).

## What it produces

A single-period road-surface-temperature sentence ("Tienpintalämpötila on
X…Y astetta") without the day/night decomposition that
`roadtemperature_daynightranges` applies.

## Configuration root

```
textgen::[section]::story::roadtemperature_shortrange::*
```

Reads `mininterval`, `always_interval_zero`, day/night definitions, and the
`today` / `tonight` phrase-preference lists.
