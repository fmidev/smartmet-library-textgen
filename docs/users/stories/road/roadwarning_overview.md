# Story "roadwarning_overview"

> **Status:** Primary (road-weather products).
>
> **Owner:** `RoadStory::warning_overview()`.
> **Implementation:** `textgen/roadwarning_overview.cpp` (~1 353 LOC).
>
> Source-mined. No separate spec document.

## What it produces

A road-warning narrative for up to 30 hours (the road-weather model is
short-range). The story enumerates the warning conditions present in
each sub-period and merges adjacent sub-periods with similar warning
states into phrases like "aamusta alkaen liukasta pakkaslumesta …".

## Warning classes

From `RoadWarningType` in the source:

| Enum | Name | Describes |
| --- | --- | --- |
| `NORMAL` | normal | No warning; baseline |
| `FROSTY` | frosty | Frost on the road |
| `ICING` | icing | Road icing up |
| `SNOWY` | snowy | Snow-covered |
| `FROSTSLIPPERY` | frostslippery | Slippery due to frost |
| `WINDWARNING` | windwarning | High wind affecting driving |
| `WHIRLING` | whirling | Drifting / whirling snow |
| `HEAVY_SNOWFALL` | heavy_snowfall | Intense snowfall |
| `FASTWORSENING` | fastworsening | Conditions worsening quickly |
| `SLEET_TO_ICY` | sleet_to_icy | Sleet turning to ice |
| `RAIN_TO_ICY` | rain_to_icy | Rain turning to ice (freezing rain) |
| `ICYRAIN` | icyrain | Icy rain |

They are ordered by importance in `warning_importance()` — more severe
warnings shadow less severe ones when both are simultaneously present.

## Time-of-day structure

The story inherits the four sub-period structure from `RoadStory`:

* **morning**  — `morning::starthour` to `day::starthour`
* **day**      — `day::starthour` to `evening::starthour`
* **evening**  — `evening::starthour` to `night::starthour`
* **night**    — `night::starthour` to next `morning::starthour`

All four hours must be set. The story refuses to run otherwise.

## Coverage classification

Same three-tier classification as
[`roadcondition_overview`](roadcondition_overview.md):

| Setting | Default | Meaning |
| --- | --- | --- |
| `generally_limit` | 90 % | A warning is "general" (phrase dropped / "yleisesti") when coverage exceeds this |
| `manyplaces_limit` | 50 % | Warning is "monin paikoin" above this and below `generally_limit` |
| `someplaces_limit` | 10 % | Warning is "paikoin" above this and below `manyplaces_limit` |

Below `someplaces_limit` the warning is not emitted.

## Configuration parameters

All settings live under `textgen::[section]::story::roadwarning_overview::*`.

| Parameter | Default | Meaning |
| --- | --- | --- |
| `maxhours` | 30 | Maximum hours reported (bounded by the road model's range) |
| `generally_limit` | 90 % | "Yleisesti" threshold |
| `manyplaces_limit` | 50 % | "Monin paikoin" threshold |
| `someplaces_limit` | 10 % | "Paikoin" threshold |
| `morning::starthour` | *required* | Start of the morning sub-period |
| `day::starthour` | *required* | Start of the day sub-period |
| `evening::starthour` | *required* | Start of the evening sub-period |
| `night::starthour` | *required* | Start of the night sub-period |

The default phrase order for each sub-period is documented in
[`period_phrases.md`](../../period_phrases.md) (`today` / `next_day` /
`next_days` / `days`).

## Merging adjacent sub-periods

`roadwarning_overview` merges two adjacent sub-periods with the same
dominant warning state into a shared "alkaen" sentence, analogous to
the merging rules in
[`roadcondition_overview`](roadcondition_overview.md#road-condition-for-multiple-periods).
This is why the story can produce compact phrasings like
"iltapäivästä alkaen monin paikoin jäistä" instead of separate
afternoon / evening / night sentences.

## See also

* [`roadcondition_overview`](roadcondition_overview.md) — surface
  condition (icy / snowy / wet / …) rather than warning state. Usually
  used together with `roadwarning_overview`.
* [`roadwarning_shortview`](roadwarning_shortview.md) — shorter
  variant.
