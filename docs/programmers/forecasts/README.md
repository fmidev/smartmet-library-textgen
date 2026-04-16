# Narrative-helper classes

These classes are not Stories. They are scaffolding used by the
composed weather narratives
([`weather_overview`](../../users/stories/weather/weather_overview.md),
[`weather_forecast`](../../users/stories/weather/weather_forecast.md))
to classify a forecast period and emit the matching sentence. They
encapsulate the multi-hour / multi-parameter logic that would
otherwise bloat the generator files.

They all share a similar shape:

```cpp
class FooForecast
{
public:
  FooForecast(wf_story_params& parameters);
  Sentence fooSentence(const WeatherPeriod& period /*, ...*/) const;
  // internal: period-finding, classification
};
```

## Pages

* [`CloudinessForecast`](cloudiness.md)
* [`PrecipitationForecast`](precipitation.md)
* [`FogForecast`](fog.md)
* [`ThunderForecast`](thunder.md)
* [`WindForecast`](wind.md)
* [`WeatherForecastStory`](weather_forecast_story.md) —
  the composition scaffold that ties the above together

## Shared utilities

`WeatherForecast.cpp` is a grab-bag of utilities shared by the
narrative helpers:

* `split_the_area()` / `check_area_splitting()` — horizontal or vertical
  area splitting driven by `textgen::split_the_area::<area>::method`.
  Used by `weather_forecast`, `temperature_max36hours`, and
  `temperature_anomaly`.
* `Rect`, `get_area_center()`, `points_in_rectangle()` — geometry
  helpers over a `WeatherArea`.
* `wf_story_params` (in `WeatherForecast.h`) — the bag of references
  passed into every helper's constructor (sources, area, period,
  variable prefix, logger, …).

## See also

* [Stories](../stories.md) — the higher-level concept these helpers
  serve
* [Architecture overview](../architecture.md) — where the helpers fit
  in the request flow
