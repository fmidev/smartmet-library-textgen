# Story "weather_overview"

> **Status:** Active. Combined weather narrative built on the
> `rain_oneday` / `rain_twoday` case tables.
>
> For new products prefer [`weather_forecast`](weather_forecast.md) —
> that is the modern composed weather narrative with explicit
> `CloudinessForecast` / `PrecipitationForecast` / `FogForecast` /
> `ThunderForecast` sub-stories, and the only one with a current
> authoritative specification.
>
> **Owner:** `WeatherStory::overview()`.
> **Implementation:** `textgen/weather_overview.cpp` (~1 927 LOC).

## What it produces

A multi-sentence weather paragraph covering a 1–N day period. The story
combines cloudiness description with precipitation timing, driven by
lookup tables that enumerate every possible rain-start / rain-end
combination for a single day or a two-day window.

## One-day precipitation

[rain_oneday](../precipitation/rain_oneday.md) enumerates all 300 cases
where a single spell of rain both starts and ends on the same day, and
the forecasts they map to.

## Two-day precipitation

[rain_twoday](../precipitation/rain_twoday.md) enumerates all 576 cases
where a single spell of rain starts on one day and ends on the next.

## Configuration root

```
textgen::[section]::story::weather_overview::*
```

The configuration surface is large — consult `textgen/weather_overview.cpp`
(the top of each helper function lists the `Settings::*` keys it reads)
and `test/WeatherStoryTest.cpp` for working examples.

## See also

* [`weather_forecast`](weather_forecast.md) — modern replacement with a
  current spec
* [`weather_shortoverview`](weather_shortoverview.md) — condensed
  multi-day variant
