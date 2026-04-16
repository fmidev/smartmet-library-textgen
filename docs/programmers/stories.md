# Stories

A **Story** is a self-contained paragraph about one weather parameter.
"Temperature for today", "wind overview for 1–3 days", "forest fire warning
for a county" are all stories. Every story has a name string, and
`StoryFactory` maps the name to the class that produces it.

## Hierarchy

```
Story                       (abstract: hasStory, makeStory)
├── TemperatureStory         → temperature_mean, temperature_max36hours, ...
├── PrecipitationStory       → pop_max, precipitation_classification, ...
├── CloudinessStory          → cloudiness_overview
├── WeatherStory             → weather_overview, weather_forecast, ...
├── WindStory                → wind_overview, wind_anomaly, ...
├── FrostStory               → frost_onenight, frost_twonights, ...
├── RelativeHumidityStory    → relativehumidity_day, ...
├── RoadStory                → roadwarning_overview, roadcondition_overview, ...
├── ForestStory              → evaporation_day, forestfirewarning_county, ...
├── DewPointStory            → dewpoint_range
├── PressureStory            → pressure_mean
├── WaveStory                → wave_range
└── SpecialStory             → none, date, text, table
```

## Dispatch

`StoryFactory::create(name)` walks these classes in order and returns the
`Paragraph` produced by the first one whose `hasStory(name)` returns true:

```cpp
Paragraph StoryFactory::create(
    const TextGenPosixTime& forecastTime,
    const AnalysisSources& sources,
    const WeatherArea& area,
    const WeatherPeriod& period,
    const std::string& name,        // e.g. "wind_overview"
    const std::string& variable);   // e.g. "textgen::day1::story::wind_overview"
```

The `variable` argument is the prefix under which the story reads its own
configuration (`::mininterval`, `::fake::*`, `::today::phrases`, …).

## Per-parameter Story classes

Each concrete class exposes:

```cpp
class XxxStory : public Story
{
 public:
  static bool hasStory(const std::string& name);
  Paragraph makeStory(const std::string& name) const;
  // plus one method per generator, typically defined in its own .cpp:
  //    Paragraph overview() const;
  //    Paragraph anomaly() const;
  //    Paragraph mean() const;
};
```

`makeStory` is a big switch that dispatches on the name to the matching
method. For small stories the method body is inline in the class's `.cpp`;
for large ones the body is extracted into a lowercase file
(`wind_overview.cpp`, `temperature_max36hours.cpp`) — see the
[file naming convention](architecture.md#conventions-used-in-the-code).

## Generator entry points

Naming convention:

* **`WindStory.cpp`** holds the class itself and the trivial generators.
* **`wind_overview.cpp`** holds the body of `WindStory::overview()`.
* **`wind_anomaly.cpp`** holds the body of `WindStory::anomaly()`.

The top of each generator file has a `\brief Implementation of method ...`
comment that identifies the owning class.

## Helper tool namespaces

Several families share helper code:

| Namespace | Used by |
| --- | --- |
| `TemperatureStoryTools` | most `temperature_*` generators |
| `PrecipitationStoryTools` | `precipitation_*`, `pop_*` |
| `WindStoryTools` | `wind_*` |
| `CloudinessStoryTools` | `cloudiness_overview`, `weather_overview` |
| `FrostStoryTools` | `frost_*` |
| `RoadStoryTools` | `roadcondition_*`, `roadwarning_*` |

Larger generators (`weather_overview`, `weather_forecast`, `wind_overview`)
additionally pull in **narrative helper classes** — `WeatherForecast`,
`CloudinessForecast`, `PrecipitationForecast`, `FogForecast`,
`ThunderForecast`, `WindForecast`. These are not Stories themselves; they
encapsulate multi-period classification and sentence composition that would
otherwise bloat the generator file.

## Adding a new story

1. Pick the parameter class it belongs to (`WindStory`, `TemperatureStory`, …).
2. Add the name to that class's `hasStory` and `makeStory`.
3. Add a method on the class.
4. If the body is > ~100 lines, extract it into a lowercase file
   (`my_story.cpp`) and list it in `Makefile`'s sources.
5. Add a user-facing doc under `docs/users/stories/<parameter>/my_story.md`
   with the [standard header](../users/stories/README.md) and a status tag.

## See also

* [architecture.md](architecture.md) — how a story call fits into the
  overall request flow
* [user-facing catalogue](../users/stories/README.md) — all 55 stories
  tagged by status and complexity
