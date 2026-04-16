# Story "temperature_anomaly"

> **Status:** Primary (anomaly products). Reports temperature relative to
> climatology.
>
> **Owner:** `TemperatureStory::anomaly()`.
> **Implementation:** `textgen/temperature_anomaly.cpp` (~1 563 LOC).

## What it produces

A narrative that describes forecast temperature as normal, warmer, or
colder than the climatological reference for the same calendar time and
area. The story combines absolute day/night ranges with the relative
assessment.

## Configuration root

All settings live under

```
textgen::[section]::story::temperature_anomaly::*
```

Key subtrees include the thresholds that define "somewhat warmer" / "warmer"
/ "significantly warmer", the climatology source (`GridClimatology`), the
day/night periods, and the phrase-preference lists for each sub-clause.

See `textgen/temperature_anomaly.cpp` for the full list of variables (look
for `Settings::` calls) and `test/TemperatureStoryTest.cpp` for working
example configurations.

## Climatology dependency

This story requires a grid-climatology dataset to be available through the
configured climatology source. If climatology is missing the story falls
back to an absolute-only sentence comparable to
[`temperature_max36hours`](temperature_max36hours.md).
