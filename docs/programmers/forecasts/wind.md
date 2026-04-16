# `WindForecast`

Narrative helper that composes wind-description sentences for the
`weather_forecast` family. **Not** the same thing as
[`wind_overview`](../../users/stories/wind/wind_overview.md) — that is a
standalone story with its own `WindStory::overview()` entry point.

**Header:** `textgen/WindForecast.h` +
`textgen/WindForecastStructs.h`
**Source:** `textgen/WindForecast.cpp`

## What it does

`WindForecast` is the composition engine the standalone `wind_overview`
story actually calls into after the
[time-series smoothing and change-point detection](../../users/stories/wind/wind_overview.md#story-construction)
phases have produced the event periods. Its job is to turn the list of
change-point events into the final sequence of Finnish sentences.

## Key types

```cpp
struct WindDirectionPeriodInfo
{
    WeatherPeriod period;
    WindDirectionInfo info;
};

enum SentenceParameterType
{
    VOID_TYPE,
    /* ... */
};

struct sentence_parameter
{
    SentenceParameterType type;
    Sentence sentence;
};

struct interval_sentence_info
{
    Sentence sentence;
    /* ... */
};

// Sentence is constructed by populating `sentenceParameterTypes`
// with the desired parameter ordering and filling each slot.
```

`sentence_parameter` and `interval_sentence_info` together let the
helper build a sentence slot-by-slot (direction, speed, rate phrase,
time qualifier, range, …) and then reorder or drop slots per the
"tarkentavat säännöt" in the
[`wind_overview` specification](../../users/stories/wind/wind_overview.md#5-story-formation).

## Primary responsibilities

* Populate period-level information structs from the change-point
  output.
* Construct direction + speed sentences, honouring
  * "vähän" / "nopeasti" / "vähitellen" rate qualifiers,
  * "kovimmillaan X m/s" when the range's upper bound is capped,
  * "puuskittaista" when gusty-wind thresholds are met,
  * the 8-way / 16-way compass choice based on direction deviation.
* Emit a combined-event phrase when direction and speed change
  together ("tuuli voimistuu ja kääntyy etelään").

## See also

* [`wind_overview`](../../users/stories/wind/wind_overview.md) — the
  owning story and its specification.
* [`WindForecastStructs`](https://github.com/fmidev/smartmet-library-textgen/blob/master/textgen/WindForecastStructs.h)
  — auxiliary structs, kept in a separate header to reduce compile
  coupling.
