# Story "wave_range"

> **Status:** Specialized. The only story owned by `WaveStory`.
>
> **Owner:** `WaveStory::range()`.
> **Implementation:** `textgen/wave_range.cpp` (~120 LOC).

## What it produces

A single sentence describing significant wave height over a period. The
values are rounded to 0.5 metres:

1. "Aallonkorkeus on alle 0.5 metriä." if the maximum is at most 0.5 m
1. "Aallonkorkeus on noin N metriä." if the range is narrower than `mininterval`
1. "Aallonkorkeus on enimmillään N metriä." if the minimum is zero
1. "Aallonkorkeus on N...M metriä." otherwise

## Configuration root

```
textgen::[section]::story::wave_range::*
```

Key variables:

* `mininterval` — collapse the range to a single "about" value if the
  min/max span is smaller than this (default 0.5).
* `rangeseparator` — separator between the limits of a range (default `-`).

## Fake values

```
textgen::[section]::story::wave_range::fake::height::minimum = [result]
textgen::[section]::story::wave_range::fake::height::maximum = [result]
textgen::[section]::story::wave_range::fake::height::mean    = [result]
```
