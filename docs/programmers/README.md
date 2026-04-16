# TextGen developer guide

For developers who want to read or extend the library itself.

## Reading order

1. [Architecture overview](architecture.md) — how the patterns fit together
   and how a call flows from `TextGenerator::generate()` to final text
2. [Class index](classes.md) — one-line purpose for every class,
   grouped by role (Glyph, Dictionary, TextFormatter, Story, …)
3. [Glyphs](glyphs.md) — `Document` / `Paragraph` / `Sentence` / `Phrase`
   Composite hierarchy
4. [Dictionaries](dictionaries.md) — `Dictionary` interface and backends
5. [Text formatters](text-formatters.md) — Visitor hierarchy (plain / HTML /
   CSS / speech / debug)
6. [Stories](stories.md) — `Story` base class, `StoryFactory`,
   and how per-parameter stories dispatch to generators
7. [Narrative helpers](forecasts/README.md) — `CloudinessForecast`,
   `PrecipitationForecast`, `FogForecast`, `ThunderForecast`,
   `WindForecast`, `WeatherForecastStory`
8. [Mask sources](masks.md) — geographic filtering (coast / inland /
   halves)
9. [Acceptors](acceptors.md) — value filtering for analyser reductions
10. [Analysis functions](functions.md) — `Mean`, `Maximum`, `Trend`, …
11. [Testing](testing.md) — how to write and run a generator unit test

## Conventions used in the code

* **Capital-letter file** = a class (`Document.cpp`, `WeatherFact.cpp`,
  `HeaderFactory.cpp`). Class-file layout follows the SmartMet convention:
  `Foo.h` / `Foo.cpp` hold `class TextGen::Foo`.
* **Lowercase file** = a generator entry point — one per named story
  (`wind_overview.cpp`, `temperature_max36hours.cpp`). These implement a
  single method of the owning Story class (usually
  `WindStory::overview()`, `TemperatureStory::max36hours()`, …) and are
  split out purely because they are too large to share a translation unit.
* Everything lives in `namespace TextGen`.
* Internal headers are `#include "X.h"`. SmartMet library headers use angle
  brackets: `#include <calculator/Settings.h>`.

## Dependencies

* `calculator` — forecast analysis primitives (`AnalysisSources`,
  `Settings`, `WeatherPeriod`, `HourPeriodGenerator`, …)
* `macgyver` — utilities (`Exception`, caching, charset, datetime)
* `newbase` — QueryData / NFmi* types
* Boost (locale, thread), fmt, libpqxx, mysqlpp, ICU, GDAL

## See also

* [User guide](../users/README.md) — configuration layout, per-story
  reference, and status tags
