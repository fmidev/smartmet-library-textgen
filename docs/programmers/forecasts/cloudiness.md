# `CloudinessForecast`

Narrative helper that classifies a forecast period's cloudiness into
discrete bands and produces the matching sentence.

**Header:** `textgen/CloudinessForecast.h`
**Source:** `textgen/CloudinessForecast.cpp`

## What it does

Given a `wf_story_params` bag and optional coastal / inland data
arrays, the class:

1. Splits the forecast period into **cloudiness periods** — adjacent
   hours that share the same cloudiness band.
2. Joins adjacent bands when the transition is small enough that
   reporting them separately would add noise.
3. Emits a `Sentence` describing the band (or the change in band) for
   a requested period.

## Primary methods

```cpp
Sentence cloudinessSentence(const WeatherPeriod& period,
                            bool shortForm) const;

Sentence cloudinessSentence(const WeatherPeriod& period,
                            /* ... */,
                            const Sentence& periodPhrase,
                            /* ... */) const;

Sentence cloudinessChangeSentence(const WeatherPeriod& period) const;

static Sentence areaSpecificSentence(const WeatherPeriod& period);
```

The `shortForm` flag selects between a full sentence and a fragment
that can be embedded in a larger sentence.

## Cloudiness bands

The bands correspond to Lasse Winberg's classification:

| Total cloudiness (%) | Phrase |
| --- | --- |
| 0 – 9.9 | selkeää |
| 9.9 – 35 | melkein selkeää |
| 35 – 65 | puolipilvistä |
| 65 – 85 | verrattain pilvistä |
| 85 – 110 | pilvistä |

The "sää vaihtelee puolipilvisestä pilviseen" phrase is emitted when
the min falls in 35…65 and the max in 85…110.

## Change phrases

* `sää selkenee` — cloudiness ≥ 65 % at the start, ≤ 20 % at the end
* `sää pilvistyy` — cloudiness ≤ 20 % at the start, ≥ 65 % at the end

Both mirror the specification of
[`weather_forecast`](../../users/stories/weather/weather_forecast.md#change-detection-rules).

## Coast/inland distinction

Coast and inland are reported separately only when one is fully
cloudless (< 5 %) and the other very cloudy (> 70 %). For other cases
the shared sentence is used.

## See also

* [`weather_forecast`](../../users/stories/weather/weather_forecast.md#cloudiness)
  for the spec the helper implements.
* [`cloudiness_overview`](../../users/stories/cloudiness/cloudiness_overview.md)
  for the standalone cloudiness story.
