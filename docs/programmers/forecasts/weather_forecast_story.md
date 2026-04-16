# `WeatherForecastStory`

Composition scaffold used by
[`weather_forecast`](../../users/stories/weather/weather_forecast.md) to
glue together the four phenomenon helpers
([`CloudinessForecast`](cloudiness.md),
[`PrecipitationForecast`](precipitation.md),
[`FogForecast`](fog.md),
[`ThunderForecast`](thunder.md))
into a single coherent paragraph.

**Header:** `textgen/WeatherForecastStory.h`
**Source:** `textgen/WeatherForecastStory.cpp`

## What it does

Where a story like `temperature_max36hours` builds up a `Paragraph`
by directly concatenating `Sentence` objects, the weather forecast has
to compose several independent phenomena (cloudiness, precipitation,
thunder, fog) per sub-period while avoiding:

* tautologies (two adjacent sentences that both say "paikoin",
  two adjacent "huomenna" time qualifiers, …)
* tautology-violating period phrases (e.g. "huomenna" followed by
  "iltapäivällä" without re-asserting the day)
* empty sentences

`WeatherForecastStory` drives this as a small state machine that walks
the sub-periods in order and asks each phenomenon helper for its
contribution, then merges them.

## Helper classes used

### `PeriodPhraseGenerator`

Small sub-helper that remembers which period phrases have already been
emitted and returns a suitable replacement ("huomenna" → "päivällä")
when a date qualifier would otherwise repeat.

```cpp
class PeriodPhraseGenerator
{
public:
    PeriodPhraseGenerator(const std::string& var);
    bool dayExists(int n) const;
    Sentence getPeriodPhrase(const WeatherPeriod& period);

private:
    std::string itsVar;
    std::map<WeatherPeriod, Sentence> periodPhrases;
};
```

### `WeatherForecastStoryItem`

One entry in the composition's output queue — a period + phenomenon
fragment produced by one of the helpers. The scaffold collects items
for the whole forecast before rendering them, so that merging and
tautology-avoidance can look across item boundaries.

## Responsibilities

1. Build the list of reporting sub-periods (typically day / night
   sub-periods derived from a `NightAndDayPeriodGenerator`).
2. For each sub-period, query each phenomenon helper for a fragment.
3. Merge adjacent fragments that share the same phenomenon state.
4. Select period phrases via `PeriodPhraseGenerator`.
5. Emit the final `Paragraph` in chronological order.

## See also

* [`weather_forecast`](../../users/stories/weather/weather_forecast.md)
  — the public-facing specification of what this scaffold produces.
* The four phenomenon helpers:
  [cloudiness](cloudiness.md),
  [precipitation](precipitation.md),
  [fog](fog.md),
  [thunder](thunder.md).
