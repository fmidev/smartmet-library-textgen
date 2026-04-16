# `ThunderForecast`

Narrative helper that picks the appropriate thunder phrase from the
thunder probability and thunder-area extent over a forecast period.

**Header:** `textgen/ThunderForecast.h`
**Source:** `textgen/ThunderForecast.cpp`

## What it does

Thunder is always reported as a supplementary clause to a precipitation
sentence — it is never an independent narrative in
`weather_forecast`. The helper takes the maximum thunder probability
and the maximum thunder-area extent over a period, and dispatches them
through a 3×3 table to select one of five phrases.

## Primary methods

```cpp
Sentence thunderSentence(
    const WeatherPeriod& period,
    AreaTools::forecast_area_id forecastArea,
    const std::string& variable) const;

Sentence areaSpecificSentence(const WeatherPeriod& period) const;
```

## Phrase-selection table

From the
[`weather_forecast` thunder section](../../users/stories/weather/weather_forecast.md#thunder):

| Extent × Probability | 20 ≤ tn ≤ 30 | 30 ≤ tn ≤ 60 | 60 ≤ tn ≤ 100 |
| --- | --- | --- | --- |
| summer: 5 ≤ laajuus < 30 | paikoin voi myös ukkostaa | paikoin myös ukkostaa | todennäköisesti myös ukkostaa |
| winter: 10 ≤ laajuus < 30 | paikoin voi myös ukkostaa | paikoin myös ukkostaa | todennäköisesti myös ukkostaa |
| laajuus ≥ 30 | mahdollisesti myös ukkostaa | myös ukkostaa esiintyy | todennäköisesti myös ukkostaa |

## Internal helpers

The helper has a private `getMaxValue()` static that walks a
`weather_result_data_item_vector` for a given period and returns the
maximum — used both for probability and extent.

## See also

* [`weather_forecast` thunder section](../../users/stories/weather/weather_forecast.md#thunder)
  for the spec.
* [`weather_thunderprobability`](../../users/stories/weather/weather_thunderprobability.md)
  — the standalone thunder story (without precipitation coupling).
