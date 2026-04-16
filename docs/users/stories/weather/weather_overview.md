# Story "weather_overview"

> **Status:** Primary. Flagship combined weather narrative
> (cloudiness + precipitation, with thunder and fog sub-stories).
>
> **Owner:** `WeatherStory::overview()`.
> **Implementation:** `textgen/weather_overview.cpp` (~1 927 LOC).

## What it produces

A multi-sentence weather paragraph covering a 1–N day period. The story
combines cloudiness description with precipitation timing, pulling on the
`rain_oneday` and `rain_twoday` case tables.

## One-day precipitation

[rain_oneday](../precipitation/rain_oneday.md) enumerates all possible
cases where a single spell of rain both starts and ends on the same day,
and the forecasts they map to.

## Two-day precipitation

[rain_twoday](../precipitation/rain_twoday.md) enumerates the cases where
a single spell of rain starts on one day and ends on the following day.

## Configuration root

```
textgen::[section]::story::weather_overview::*
```

The configuration surface is large — consult `textgen/weather_overview.cpp`
(the top of each helper function lists the `Settings::*` keys it reads)
and `test/WeatherStoryTest.cpp` for working examples.
