# Story "weather_forecast" (and "weather_forecast_at_sea")

> **Status:** Primary. Composed multi-part weather narrative; `_at_sea` is
> the marine variant.
>
> **Owner:** `WeatherStory::forecast()` / `WeatherStory::forecast_at_sea()`.
> **Implementation:** `textgen/weather_forecast.cpp` (~1 528 LOC).
> **Scaffolding:** `WeatherForecastStory` class.

## What it produces

A full weather paragraph composed from several underlying forecasts:

* cloudiness (`CloudinessForecast`)
* precipitation (`PrecipitationForecast`)
* fog (`FogForecast`)
* thunder (`ThunderForecast`)
* wind context (optional)

The parts are ordered, merged, and stripped of tautology by
`WeatherForecastStory`, which is a composition scaffold rather than a
Story in its own right.

Two entry points exist:

* `weather_forecast` — full continental output
* `weather_forecast_at_sea` — marine variant with different thresholds and
  phrases

## Configuration root

```
textgen::[section]::story::weather_forecast::*
textgen::[section]::story::weather_forecast_at_sea::*
```

Each sub-forecast reads its own subtree. The configuration surface is
large — consult the source and the regression tests under
`test/WeatherStoryTest.cpp` for working examples.

## Related

* [`weather_overview`](weather_overview.md) — alternative composition path.
* [`weather_shortoverview`](weather_shortoverview.md) — condensed variant
  for longer periods.
