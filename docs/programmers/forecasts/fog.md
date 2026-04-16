# `FogForecast`

Narrative helper that classifies fog periods and emits matching
sentences.

**Header:** `textgen/FogForecast.h`
**Source:** `textgen/FogForecast.cpp`

## What it does

For the requested period the helper:

1. Computes moderate-fog and dense-fog area coverage (percentages) per
   hour.
2. Identifies fog periods (adjacent hours with coverage above the
   minimum).
3. Classifies each period by coverage band.
4. Emits a `Sentence` with the appropriate phrase and time qualifier.

Dense fog is compiled behind the `ENABLE_DENSE_FOG` feature flag:

```cpp
#define ENABLE_DENSE_FOG 0
```

When 1, the helper additionally emits the "joka voi olla sakeaa"
qualifier if dense-fog coverage exceeds a configured fraction of the
moderate-fog extent.

## Fog-type identifiers

```cpp
enum fog_type_id
{
    FOG,
    FOG_POSSIBLY_DENSE,
    FOG_IN_SOME_PLACES,
    /* ... */
};
```

## Coverage bands

| Area coverage (%) | Phrase |
| --- | --- |
| < 20 | — (no sentence) |
| 20 – 50 | paikoin sumua |
| 50 – 90 | monin paikoin sumua |
| ≥ 90 | sumua |

When `ENABLE_DENSE_FOG` is on and dense-fog coverage ≥ 20 % of the
area, ", joka voi olla sakeaa" is appended.

## Primary methods

```cpp
Sentence fogSentence(const WeatherPeriod& period) const;

Sentence fogSentence(const WeatherPeriod& period,
                     /* ... */) const;

Sentence areaSpecificSentence(const WeatherPeriod& period) const;

static Sentence getFogPhrase(fog_type_id id);

static Sentence constructFogSentence(const std::string& dayPhasePhrase,
                                     /* ... */);
```

The `areaSpecificSentence` variant splits coast and inland into
separate fragments when their fog coverage differs by at least 50 pp,
as described in the
[`weather_forecast` fog section](../../users/stories/weather/weather_forecast.md#fog).

## See also

* [`weather_forecast` fog section](../../users/stories/weather/weather_forecast.md#fog)
  for the spec.
* [`PrecipitationForecast`](precipitation.md) — fog is reported only
  inside "no-rain" periods, so the two helpers are usually driven
  together.
