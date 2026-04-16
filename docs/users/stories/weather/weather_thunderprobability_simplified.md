# Story "weather_thunderprobability_simplified"

> **Status:** Active. Simplified variant of
> [`weather_thunderprobability`](weather_thunderprobability.md).
>
> **Owner:** `WeatherStory::thunderprobability_simplified()`.
> **Implementation:** `textgen/weather_thunderprobability_simplified.cpp`
> (~88 LOC).

## What it produces

Same one-line thunder-probability sentence as
[`weather_thunderprobability`](weather_thunderprobability.md), but with a
simplified rounding and threshold scheme intended for customer-facing
summary products.

## Configuration root

```
textgen::[section]::story::weather_thunderprobability_simplified::*
```

Reads `precision` and `limit` (the probability threshold below which the
sentence is suppressed). See the source for any simplified-specific
knobs.
