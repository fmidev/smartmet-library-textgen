# Story "temperature_max36hours"

> **Status:** Primary. Flagship 36-hour temperature narrative. Use this for
> almost every temperature product.
>
> **Owner:** `TemperatureStory::max36hours()`.
> **Implementation:** `textgen/temperature_max36hours.cpp` (~4 242 LOC).

## What it produces

A multi-sentence narrative describing the temperature for the next
approximately 36 hours, broken into day and night segments. Depending on
the data and the configuration it may include:

* the day's maximum and night's minimum
* comparatives with the next day / following night
* a coast vs. inland sub-clause when the difference is significant
* a frost sub-clause (via `FrostStory`) when frost probability is high
* climatology-relative phrasing (normal / warmer / colder than usual)

## Configuration root

All settings live under

```
textgen::[section]::story::temperature_max36hours::*
```

The story reads a large set of tuning variables: day/night periods,
comparative thresholds, coast limits, frost integration, climatology
references, and numerous phrase-preference lists. For the exact list see:

* `textgen/temperature_max36hours.cpp` — every `Settings::*` call near
  the top of each helper function
* `test/TemperatureStoryTest.cpp` — working configurations exercised by the
  unit tests (search for `"temperature_max36hours"`)

## Fake values

The generator supports an extensive `::fake::*` subtree for regression
testing. Browse `test/TemperatureStoryTest.cpp` for the key names.

## Replaces

`temperature_max36hours` supersedes the older combinations of
[`temperature_day`](temperature_day.md),
[`temperature_dailymax`](temperature_dailymax.md) +
[`temperature_nightlymin`](temperature_nightlymin.md) for multi-period
products. Reach for it first.
