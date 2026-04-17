# TextGen user guide

For product configurators: pick which stories appear in a forecast text,
set their thresholds, and tune the period / header behaviour.

## Reading order

1. [Getting started](getting-started.md) — minimal working configuration
2. [Configuration layout](configuration.md) — how `libconfig` variables map
   to sections, stories, and `fake::` values
3. [Headers](headers/README.md) — header type catalogue (`until`, `from_until`,
   `several_days`, …)
4. [Periods](periods.md) — `WeatherPeriod` from a forecast time and a period
   variable
5. [Subperiods](subperiods.md) — splitting a period into days
6. [Period phrases](period_phrases.md) — `today`, `tonight`, `next_day`, …
7. [Time-of-day periods](timeperiods.md) — morning / afternoon / late night
   boundaries
8. [Story catalogue](stories/README.md) — the main reference; every generator
   with its status tag

## Story status legend

Every generator in the [story catalogue](stories/README.md) is tagged with
one of the following:

| Tag | Meaning |
| --- | --- |
| **Primary** | Large, modern generator. Usually what you should reach for. |
| **Active** | In active use for a specific product. Not the "modern" path but still current. |
| **Legacy** | Older generator, typically superseded by a Primary one. Fine for simple cases; prefer the Primary if available. |
| **Trivial** | Emits one statistic as a single sentence. Works, but rarely interesting on its own — use only if that is exactly what you want. |
| **Specialized** | Narrow purpose (e.g. `SpecialStory::date`, wave range). |
| **Needs review** | Present in the source but not yet reclassified; treat with care. |

The tags are guidance, not policy — nothing stops you using a "Trivial" story
if that is what the customer actually wants.

## Related reference

* [Quality](quality.md) — what the quality value means and when the library
  splits a period automatically on poor quality
* [Transitioning from localized database translations](transitioning-from-localized-database-translations.md)
  — for deployments whose translators have edited rows directly in the
  `textgen.translation_<lang>` database tables and need to move to the
  new `.po` backend without losing those edits
