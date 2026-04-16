# `PrecipitationForecast`

Narrative helper that identifies precipitation periods, classifies
them, and produces the matching sentence — including the geographic
"paikoin / monin paikoin" qualifier.

**Header:** `textgen/PrecipitationForecast.h`
**Source:** `textgen/PrecipitationForecast.cpp`

## What it does

1. Walks the hourly time series and detects rain / fair periods based
   on per-form intensity thresholds
   (`dry_weather_limit_water`, `dry_weather_limit_drizzle`, …).
2. Merges adjacent rain periods when separated by short fair gaps, per
   the rules in
   [`weather_forecast` → Decision order](../../users/stories/weather/weather_forecast.md#decision-order).
3. For each rain period, produces sentences describing:
   * onset / end
   * form(s) — vesi / tihku / räntä / lumi / jäätävä
   * form changes mid-period
   * intensity (heikko / kohtalainen / runsas)
   * geographic coverage (yleisesti / monin paikoin / paikoin /
     enimmäkseen poutainen)
   * rain-area movement when detectable

## The `InPlacesPhrase` sub-class

```cpp
enum PhraseType
{
    NONEXISTENT_PHRASE,
    IN_SOME_PLACES_PHRASE,
    IN_MANY_PLACES_PHRASE
};

Sentence getInPlacesPhrase(PhraseType type, bool useOllaVerbFlag);
```

Small state machine that avoids tautologies like "paikoin sadetta,
paikoin sadetta" by remembering the most recent in-places phrase and
suppressing repetitions.

## Primary methods

```cpp
Sentence precipitationSentence(
    const WeatherPeriod& period,
    const Sentence& periodPhrase,
    /* ... */) const;

Sentence precipitationChangeSentence(
    const WeatherPeriod& period,
    const Sentence& periodPhrase,
    /* ... */,
    std::vector<std::pair<WeatherPeriod, Sentence>>& additionalSentences) const;

Sentence precipitationPoutaantuuAndCloudiness(
    const Sentence& periodPhrase,
    /* ... */) const;
```

The `additionalSentences` out-parameter collects supplementary
sentences that should be emitted alongside the main precipitation
sentence (e.g. fog or cloudiness sub-clauses that are bound to the
same sub-period).

## Coverage classes

Same thresholds as documented in
[`weather_forecast` → Configuration parameters](../../users/stories/weather/weather_forecast.md#configuration-parameters):

| Threshold | Default (%) | Phrase |
| --- | --- | --- |
| `mostly_dry_weather_limit` | 10 | enimmäkseen poutaa |
| `in_some_places_lower_limit` | 10 | paikoin: lower |
| `in_some_places_upper_limit` | 50 | paikoin: upper |
| `in_many_places_lower_limit` | 50 | monin paikoin: lower |
| `in_many_places_upper_limit` | 90 | monin paikoin: upper |

Above 90 % coverage the modifier is dropped and the phrase reads
"yleisesti" (or simply the rain form without qualifier).

## See also

* [`weather_forecast` precipitation section](../../users/stories/weather/weather_forecast.md#precipitation)
  for the specification this helper implements.
* [`FogForecast`](fog.md) — runs on the inverse ("no-rain") periods.
