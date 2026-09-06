# Story "wind_sea_overview"

> **Status:** Primary for open-sea (marine warning area) products. New in
> 26.9.
>
> A wind narrative built on the **area mean wind**. It replaces the
> "top wind" statistic of [`wind_overview`](wind_overview.md) with the
> spatial mean for change detection, spatial percentiles for the reported
> range, and the vector mean for direction. Gusts never enter the speed
> statistics; an optional separate sentence can mention strong gusts.
>
> **Owner:** `WindStory::sea_overview()`.
> **Implementation:** `textgen/wind_sea_overview.cpp`.
> **Tests:** `test/WindSeaOverviewTest.cpp` (synthetic hourly series),
> `smartmet-textgenapps` `test/cnf/marine_sea_12_12.cnf` (real data).

## Why a separate story

`wind_overview` anchors both its change detection and its range upper
bound on a 95th spatial percentile of a "maximum wind" field. On a fine
grid that statistic follows the windiest few percent of the area and
produces speed changes the mean wind never made. Professional marine
forecasts describe the mean wind over the open sea; coastal effects are
already excluded by the area polygons. This story therefore uses only
robust statistics of the mean wind and keeps gusts strictly separate.

## What it produces

One paragraph of one to four sentences for a period of up to about 36
hours, for example:

```
Luoteistuulta 6-8 m/s.
Iltapäivästä alkaen vähitellen heikkenevää tuulta, aamulla 1-3 m/s.
Aamusta alkaen voimistuvaa tuulta, aamupäivällä etelätuulta 5-7 m/s.
```

Sentence forms:

| Situation | Finnish | English |
| --- | --- | --- |
| Steady wind (first sentence) | "Luoteistuulta 6-8 m/s." | "North-westerly wind 6-8 m/s." |
| Change from the start | "Vähitellen voimistuvaa etelätuulta, aluksi 4-6 m/s, illalla 11-13 m/s." | "Gradually strengthening southerly wind, at first 4-6 m/s, in the evening 11-13 m/s." |
| Later change | "Illasta alkaen vähitellen heikkenevää tuulta, aamulla 2-4 m/s." | "Gradually weakening wind from the evening, in the morning 2-4 m/s." |
| Change with a new direction | "…heikkenevää tuulta, aamulla pohjoistuulta 2-4 m/s." | "…weakening wind from the evening, northerly wind in the morning 2-4 m/s." |
| Turn without a speed change | "Keskiyöllä tuuli kääntyy länteen." | "The wind turns to the west at midnight." |
| Veering / backing (optional) | "Keskiyöllä tuuli kääntyy myötäpäivään länteen." | "The wind veers to the west at midnight." |
| Weak variable wind | "Suunnaltaan vaihtelevaa tuulta 1-3 m/s." | "Variable wind 1-3 m/s." |
| Strong gusts (optional) | "Iltapäivällä paikoin voimakkaita puuskia, kovimmillaan 18 m/s." | "In the afternoon, in some places strong gusts, up to 18 m/s." |

All phrases exist in every po dictionary shipped with the library
(`sonera` excepted).

## The algorithm

### 1. Hourly area statistics

For every hour of the period the story computes over the area:

* the **mean** wind speed (`WindSpeedMS`),
* the spatial **lower and upper percentiles** of wind speed
  (`range_lower_percentile`, `range_upper_percentile`, default 25 and 75)
  from 1 m/s bins,
* the **vector mean direction** and its spread (the `error` of the
  direction analysis),
* optionally the area maximum **gust** (`HourlyMaximumGust`) when
  `gust_reporting` is on.

The hourly means are smoothed in time with a centred running mean of
`smoothing_hours` (default 3, always odd).

### 2. Phases

The smoothed mean is cut at every change between rising, flat and
falling. Then:

1. Points whose two legs have the same tendency are merged.
2. Small wiggles inside a trend are removed: an interior leg with
   amplitude below `speed_change_threshold` (default 3 m/s) whose
   neighbours share the same tendency is dropped together with its
   turning points. Small legs between opposite trends are kept and become
   steady phases.
3. Significant legs are sharpened: hours at either end where the series
   only creeps (step below half the mean slope, within 15 % of the total
   change) are split off as steady.
4. Each leg becomes a phase: **strengthening** or **weakening** when its
   amplitude reaches the threshold and either end is above
   `weak_wind_limit` (default 4 m/s), otherwise **steady**. Consecutive
   steady phases merge, and a steady lead-in or tail shorter than two
   hours is folded into the neighbouring change.
5. At most `max_changes` (default 3) changes are kept; the smallest are
   turned into steady phases.
6. A steady phase of at least `direction_split_min_hours` (default 6) is
   split where the direction turns by `direction_change_threshold`
   (default 45°), so a turn without a speed change gets its own sentence.

### 3. Ranges

A range is computed for a run of hours from the mean of the hourly lower
and upper percentiles, rounded to integers and made to contain the
rounded mean. Its width is forced between `range_min_width` (2) and
`range_max_width` (5) m/s by widening around the mean or shrinking from
the side further away from it. No percentile above the upper one is ever
used for the range, so a local speed maximum cannot widen it.

The first sentence of a change reports the range of the first
`range_hours` (3) hours ("aluksi"). The end state of a change is the
range of the first `range_hours` hours *after* the change when a steady
phase follows, otherwise of its last hours.

### 4. Direction

The direction of a run of hours is the speed-weighted circular mean of
the hourly vector means. Its spread is the larger of the mean hourly
spread and the angular scatter of the hourly means. The spread is
classified with `wind_direction::accurate_limit` (22.5°) and
`wind_direction::variable_limit` (45°) exactly as in `wind_overview`:

| Spread | Mean speed | Phrase |
| --- | --- | --- |
| ≤ accurate | any | "luoteistuulta" |
| ≤ variable | any | "luoteen puoleista tuulta" |
| > variable | < `direction_variable_max_speed` (6 m/s) | "suunnaltaan vaihtelevaa tuulta" |
| > variable | ≥ `direction_variable_max_speed` | "luoteen puoleista tuulta" |

A direction is mentioned in the first sentence and whenever it changes by
at least `direction_change_threshold` on the 8-way compass. In a change
sentence a new direction is attached to the end of the change
("…heikkenevää tuulta, aamulla pohjoistuulta 2-4 m/s"); a variable
direction is never attached to a change. A steady phase after a reported
phase only produces a sentence when the direction changed.

### 5. Rate phrases

| Condition | Phrase |
| --- | --- |
| Change lasts at least `gradual_hours` (8) | "vähitellen" |
| Change of at least `fast_change` (5 m/s) within `fast_hours` (3) | "nopeasti" |

### 6. Veering and backing

With `turn_phrases = veering_backing` a turn is described as clockwise
(veering) or counterclockwise (backing) relative to the previously
reported direction. Finnish uses "kääntyy myötäpäivään / vastapäivään",
Swedish "vrider medurs / moturs", English "veers / backs", `en-marine`
"veering / backing". The default `plain` keeps the neutral "kääntyy"
phrases. The translations of the veering and backing phrases in
languages other than Finnish, Swedish and English were written without a
native speaker and should be reviewed.

### 7. Gusts

Gusts are reported only when `gust_reporting` is true and the area
maximum gust of some hour reaches `gust_limit` (15 m/s):
"[aika] paikoin voimakkaita puuskia, kovimmillaan N m/s". The default is
off because official gust warnings are written by meteorologists and the
generator cannot see them.

## Configuration parameters

All variables live under `textgen::[section]::story::wind_sea_overview::*`.

| Parameter | Default | Meaning |
| --- | --- | --- |
| `speed_change_threshold` | 3.0 m/s | Minimum change of the smoothed area mean that is reported |
| `weak_wind_limit` | 4.0 m/s | Changes with both ends below this are not reported |
| `smoothing_hours` | 3 | Running mean window for the hourly area mean (odd) |
| `max_changes` | 3 | Maximum number of reported speed changes |
| `gradual_hours` | 8 | Duration from which a change is "vähitellen" |
| `fast_hours` | 3 | Duration within which a change of `fast_change` is "nopeasti" |
| `fast_change` | 5.0 m/s | See above |
| `range_lower_percentile` | 25 % | Spatial percentile for the lower bound of the range |
| `range_upper_percentile` | 75 % | Spatial percentile for the upper bound of the range |
| `range_min_width` | 2 m/s | Minimum range width |
| `range_max_width` | 5 m/s | Maximum range width |
| `range_hours` | 3 | Hours used for the start and end ranges of a change |
| `range_report_min_difference` | 2 m/s | A steady phase with a new direction repeats the range only if a bound moved at least this much |
| `direction_change_threshold` | 45° | Minimum turn that is reported |
| `direction_variable_max_speed` | 6.0 m/s | "Suunnaltaan vaihtelevaa" only below this mean speed |
| `direction_split_min_hours` | 6 | Minimum steady phase length that may be split by a turn |
| `wind_direction::accurate_limit` | 22.5° | Spread up to which the direction is exact |
| `wind_direction::variable_limit` | 45° | Spread up to which the direction is "puoleinen" |
| `turn_phrases` | `plain` | `veering_backing` to distinguish clockwise and counterclockwise turns |
| `gust_reporting` | `false` | Emit the gust sentence |
| `gust_limit` | 15 m/s | Area maximum gust that triggers the gust sentence |
| `rangeseparator` | `-` | Separator in "6-8 m/s" |
| `specify_part_of_the_day` | `true` | Set to false to drop all time phrases |

## Data requirements

`WindSpeedMS` and `WindDirection` are required. `HourlyMaximumGust` is
used only for the optional gust sentence. Nothing else is read, so the
story works identically on data with and without editor-specific
parameters such as `HourlyMaximumWindSpeed`.

## Testing without gridded data

Every hourly statistic can be injected through the `::fake::` settings
used by the library tests:

```
<var>::fake::YYYYMMDDHHMM::speed::mean      = "7.0,0"
<var>::fake::YYYYMMDDHHMM::speed::lower     = "6.0,0"
<var>::fake::YYYYMMDDHHMM::speed::upper     = "8.0,0"
<var>::fake::YYYYMMDDHHMM::direction::mean  = "315,5"    (degrees, spread)
<var>::fake::YYYYMMDDHHMM::gust::maximum    = "18,0"
```

The time stamp is the local hour of the forecast period. See
`test/WindSeaOverviewTest.cpp` for complete scenarios.

## Log output

The decision log lists the hourly statistics table, the detected phases
with their type and amplitude, and for each sentence the direction and
range chosen. Look for `Entering WindStory::sea_overview`.

## See also

* [`wind_overview`](wind_overview.md) — the land-oriented story with top
  wind, gusty wind and convective cell handling
* [`wind_range`](wind_range.md) — single-sentence range
