# Story "wave_range"

> **Status:** Specialized. The only story owned by `WaveStory`.
>
> **Owner:** `WaveStory::range()`.
> **Implementation:** `textgen/wave_range.cpp` (~120 LOC).

## What it produces

A single sentence describing significant wave height over a period, as a
range ("aallonkorkeus on X…Y metriä") or as a single "about X" value when
the interval is narrow.

## Configuration root

```
textgen::[section]::story::wave_range::*
```

Key variables:

* `mininterval` — collapse the range to a single "about" value if the
  min/max span is smaller than this.
* `always_interval_zero` — if true, always report as an interval when 0
  falls inside.

## Fake values

```
textgen::[section]::story::wave_range::fake::minimum = [result]
textgen::[section]::story::wave_range::fake::maximum = [result]
textgen::[section]::story::wave_range::fake::mean    = [result]
```
