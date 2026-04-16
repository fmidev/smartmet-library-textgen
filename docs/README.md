# TextGen documentation

`smartmet-library-textgen` turns gridded weather data (QueryData, GRIB, NetCDF)
into natural-language forecast text. The library is used by
[smartmet-textgenapps](https://github.com/fmidev/smartmet-textgenapps) for
automated forecast production at FMI.

The documentation is split into two tracks:

## For product configurators

You want to configure a running `textgen` pipeline — pick which weather stories
appear in the output, tune their thresholds, set up period definitions and
headers for a new area or customer. Start here:

* **[users/](users/README.md)** — user guide
  * [Getting started](users/getting-started.md) — smallest working example
  * [Configuration layout](users/configuration.md) — `libconfig` variable naming,
    sections, `fake::` overrides
  * [Story catalogue](users/stories/README.md) — every generator with its
    complexity and current status (primary / active / legacy / trivial)
  * [Headers](users/headers/README.md) — header types (`until`, `several_days`, …)
  * [Periods](users/periods.md), [subperiods](users/subperiods.md)
  * [Period phrases](users/period_phrases.md) — `today`, `tonight`, `next_day`, …
  * [Time-of-day periods](users/timeperiods.md) — morning / afternoon / late
    night boundaries
  * [Quality](users/quality.md) — what the quality value means and when the
    library splits a period automatically

## For library developers

You want to understand or extend the C++ library — add a new story, write a
new text formatter, plug in a different dictionary backend. Start here:

* **[programmers/](programmers/README.md)** — architecture guide
  * [Architecture overview](programmers/architecture.md) — design patterns
    (Composite, Visitor, Factory) and how a call flows from
    `TextGenerator` to final text
  * [Class index](programmers/classes.md) — one-line purpose for every class,
    grouped by role
  * [Glyphs](programmers/glyphs.md) — `Document` / `Paragraph` / `Sentence` /
    `Phrase` / … Composite hierarchy
  * [Dictionaries](programmers/dictionaries.md) — `Dictionary` interface and
    backends
  * [Text formatters](programmers/text-formatters.md) — Visitor hierarchy
    (plain / HTML / CSS / speech / debug)
  * [Stories](programmers/stories.md) — `Story` base class, `StoryFactory`,
    and how per-parameter stories dispatch to generators
  * [Analysis functions](programmers/functions.md) — `Mean`, `Maximum`,
    `Trend`, … and what each one's quality means

## Project layout

```
textgen/                  C++ sources
  <Something>.cpp/.h      class files (capital letter) — the infrastructure
  <something_name>.cpp    generator entry points (lowercase) — each
                          implements one named story (e.g. wind_overview)
test/                     Boost.Test unit tests
sql/                      MariaDB/PostgreSQL schema for the phrase dictionary
docs/                     this directory
```

## History

The original documentation was written in Finnish with Doxygen markup in
2003–2005 and drifted out of date relative to the implementation. The current
pages are a full rewrite in English Markdown; where the rewrite adds a status
tag ("legacy", "trivial", "needs review") it reflects the state of the code
at the time of writing rather than an official deprecation.
