# Story "wind_anomaly"

> **Status:** Primary (anomaly products). Reports wind relative to
> climatology.
>
> **Owner:** `WindStory::anomaly()`.
> **Implementation:** `textgen/wind_anomaly.cpp` (~2 075 LOC).

## What it produces

A narrative describing forecast wind speed and direction relative to the
climatological reference. Includes speed-class comparisons ("stronger than
usual", "about the usual"), direction description, and optional trend
phrases.

## Configuration root

```
textgen::[section]::story::wind_anomaly::*
```

Shares several variables with [`wind_overview`](wind_overview.md)
(direction accuracy thresholds, speed classification) and adds its own
anomaly-threshold subtree.

For the authoritative variable list consult `textgen/wind_anomaly.cpp` and
the `"wind_anomaly"` sections of `test/TemperatureStoryTest.cpp`
(the tests for wind anomaly historically live in that file).

## Climatology dependency

Requires a wind climatology dataset accessible through the configured
climatology source. Without climatology the story degrades to absolute
wind description comparable to [`wind_overview`](wind_overview.md).
