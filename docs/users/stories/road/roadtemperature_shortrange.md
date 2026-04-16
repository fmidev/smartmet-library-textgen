# Story "roadtemperature_shortrange"

> **Status:** Active. Compact road-surface-temperature narrative, used
> when the 36-hour day/night decomposition of
> [`roadtemperature_daynightranges`](roadtemperature_daynightranges.md)
> is too long for the product.
>
> **Owner:** `RoadStory::shortrange()`.
> **Implementation:** `textgen/roadtemperature_shortrange.cpp` (~118 LOC).

## What it produces

A single sentence (or short sequence) describing road surface
temperature over one or more day/night periods, without the
elaborate day+night structure of the full `daynightranges` variant.
Example:

* "Tienpintalämpötila on tänään noin +2 astetta."
* "Tienpintalämpötila on iltapäivällä −1…+1 astetta, yöllä −3…−1 astetta."

## Configuration parameters

All settings live under
`textgen::[section]::story::roadtemperature_shortrange::*`.

| Parameter | Default | Meaning |
| --- | --- | --- |
| `maxperiods` | 3 | Maximum number of day/night periods reported |
| `mininterval` | 2 °C | Collapse a range narrower than this to a single "about" value |
| `always_interval_zero` | false | Always render as an interval when 0 falls inside |
| `rangeseparator` | `...` | Separator between min and max |

The day / night split uses the standard
`NightAndDayPeriodGenerator`; configure `day::starthour` /
`day::endhour` / `night::starthour` / `night::endhour` under the same
variable root as usual.

## Fake values

Same key layout as
[`roadtemperature_daynightranges`](roadtemperature_daynightranges.md#the-generated-analysis-functions-can-be-overridden-as-follows)
with the prefix changed to `roadtemperature_shortrange`.

## See also

* [`roadtemperature_daynightranges`](roadtemperature_daynightranges.md)
  — the full-length variant
* [`roadcondition_overview`](roadcondition_overview.md) /
  [`roadwarning_overview`](roadwarning_overview.md) — typical
  companions in a road-weather product
