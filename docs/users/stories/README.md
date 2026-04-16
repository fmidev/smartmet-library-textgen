# Story catalogue

Every named story recognised by `StoryFactory`, grouped by weather parameter.

See the [status legend](../README.md#story-status-legend) for what **Primary**,
**Active**, **Legacy**, **Trivial**, **Specialized**, and **Needs review** mean.

LOC counts are approximate (source file line counts as of the last
documentation pass).

## Temperature

| Story | Status | LOC | Notes |
| --- | --- | --- | --- |
| [`temperature_max36hours`](temperature/temperature_max36hours.md) | **Primary** | 4 242 | Flagship 36-hour temperature narrative. Day/night, coast/inland, trend, frost integration. Use this for almost every temperature product. |
| [`temperature_anomaly`](temperature/temperature_anomaly.md) | **Primary** | 1 563 | Temperature compared to climatology. Active for anomaly products. |
| [`temperature_day`](temperature/temperature_day.md) | Active | 534 | Day+night structure with optional coast sub-clause. |
| [`temperature_dailymax`](temperature/temperature_dailymax.md) | Active | 167 | Day maximum sequence with between-day comparatives. |
| [`temperature_nightlymin`](temperature/temperature_nightlymin.md) | Active | 173 | Mirror of `temperature_dailymax` for nights. |
| [`temperature_weekly_minmax`](temperature/temperature_weekly_minmax.md) | Active | 161 | "Päivien ylin X, öiden alin Y" for ≥2 days. |
| [`temperature_weekly_averages`](temperature/temperature_weekly_averages.md) | Active | 151 | Mean-over-days variant. |
| [`temperature_weekly_averages_trend`](temperature/temperature_weekly_averages_trend.md) | Needs review | — | Original doc exists but no matching `*.cpp` in the current tree — probably folded into another story. |
| [`temperature_range`](temperature/temperature_range.md) | Trivial | 89 | Single "Lämpötila on X…Y astetta" sentence. |
| [`temperature_mean`](temperature/temperature_mean.md) | Trivial | 69 | One mean temperature in one sentence. |
| [`temperature_meanmax`](temperature/temperature_meanmax.md) | Trivial | 69 | One mean of daily maxima. |
| [`temperature_meanmin`](temperature/temperature_meanmin.md) | Trivial | 72 | One mean of daily minima. |
| [`temperature_minmax`](temperature/temperature_minmax.md) | Needs review | — | Placeholder stub only; see if any config still references the name. |

## Wind

| Story | Status | LOC | Notes |
| --- | --- | --- | --- |
| [`wind_overview`](wind/wind_overview.md) | **Primary** | 3 959 | Modern wind narrative: speed class, direction class, trend, day-/night-aware phrasing. The one to use. |
| [`wind_anomaly`](wind/wind_anomaly.md) | **Primary** | 2 075 | Wind compared to climatology. |
| [`wind_daily_ranges`](wind/wind_daily_ranges.md) | Legacy | 567 | Per-day range+direction. Works, but `wind_overview` supersedes it for most products. |
| [`wind_simple_overview`](wind/wind_simple_overview.md) | Legacy | 297 | The original doc said "not well specified — do not use yet". Superseded by `wind_overview`. |
| [`wind_range`](wind/wind_range.md) | Trivial | 108 | Single sentence "N-tuulta X…Y m/s". |
| [`wind_summary`](wind/wind_summary.md) | Needs review | — | Documented in the original material but the matching `.cpp` does not appear to exist today. Possibly folded into `wind_overview`. |

## Precipitation

| Story | Status | LOC | Notes |
| --- | --- | --- | --- |
| [`precipitation_classification`](precipitation/precipitation_classification.md) | **Primary** | 313 | Precipitation class with "paikoin / monin paikoin" modifiers. Preferred when reporting a single period. |
| [`precipitation_daily_sums`](precipitation/precipitation_daily_sums.md) | Active | 187 | Sequence of day totals. |
| [`precipitation_sums`](precipitation/precipitation_sums.md) | Active | 150 | First-12h / next-12h split totals. |
| [`pop_days`](precipitation/pop_days.md) | Active | 232 | Probability-of-precipitation sequence for 1–2 days with between-day comparatives. |
| [`pop_twodays`](precipitation/pop_twodays.md) | Active | 165 | Same idea restricted to exactly two days. |
| [`pop_max`](precipitation/pop_max.md) | Trivial | 89 | Single peak probability sentence. |
| [`precipitation_range`](precipitation/precipitation_range.md) | Trivial | 109 | Min/max total with optional "yli N mm" cap. |
| [`precipitation_total`](precipitation/precipitation_total.md) | Trivial | 78 | Single total sentence. |
| [`precipitation_total_day`](precipitation/precipitation_total_day.md) | Trivial | 86 | Same, restricted to a single day. No dedicated doc yet — see source. |
| [`rain_oneday`](precipitation/rain_oneday.md) | reference | — | Enumeration of one-day rain cases. Used by `weather_overview`. |
| [`rain_twoday`](precipitation/rain_twoday.md) | reference | — | Enumeration of two-day rain cases. Used by `weather_overview`. |

## Cloudiness

| Story | Status | LOC | Notes |
| --- | --- | --- | --- |
| [`cloudiness_overview`](cloudiness/cloudiness_overview.md) | Active | 189 | Merges consecutive days with the same cloudiness description; includes "pilvistyvää / selkenevää" trend phrases. |

## Weather (combined precipitation + cloudiness + thunder)

| Story | Status | LOC | Notes |
| --- | --- | --- | --- |
| [`weather_overview`](weather/weather_overview.md) | **Primary** | 1 927 | Flagship combined weather narrative. Builds on the `rain_oneday` / `rain_twoday` case tables. |
| [`weather_forecast`](weather/weather_forecast.md) | **Primary** | 1 528 | Structured weather forecast composition. Also dispatches `weather_forecast_at_sea`. |
| [`weather_shortoverview`](weather/weather_shortoverview.md) | Active | 270 | Condensed variant for >3-day periods. The original doc says the precipitation-form part is unimplemented. |
| [`weather_thunderprobability`](weather/weather_thunderprobability.md) | Trivial | 81 | Single thunder-probability sentence. |
| [`weather_thunderprobability_simplified`](weather/weather_thunderprobability_simplified.md) | Needs review | 88 | Simplified variant of the above; no legacy doc. |

## Frost

| Story | Status | LOC | Notes |
| --- | --- | --- | --- |
| [`frost_onenight`](frost/frost_onenight.md) | **Primary** | 834 | Single-night narrative with coast handling. Used in modern frost products. |
| [`frost_twonights`](frost/frost_twonights.md) | Active | 257 | Two-night comparison. |
| [`frost_day`](frost/frost_day.md) | Active | 235 | Single-night compact form. |
| [`frost_range`](frost/frost_range.md) | Trivial | 138 | Min/max probability over an interval. |
| [`frost_maximum`](frost/frost_maximum.md) | Trivial | 120 | Peak probability only. |
| [`frost_mean`](frost/frost_mean.md) | Trivial | 115 | Mean probability only. |

## Relative humidity

| Story | Status | LOC | Notes |
| --- | --- | --- | --- |
| [`relativehumidity_day`](humidity/relativehumidity_day.md) | Active | 172 | Lowest daily humidity with coast handling — aimed at farming customers. |
| [`relativehumidity_lowest`](humidity/relativehumidity_lowest.md) | Active | 148 | One-/two-day lowest humidity with comparatives. |
| [`relativehumidity_range`](humidity/relativehumidity_range.md) | Trivial | 90 | Min/max humidity sentence. |

## Dew point

| Story | Status | LOC | Notes |
| --- | --- | --- | --- |
| [`dewpoint_range`](dewpoint/dewpoint_range.md) | Trivial | 88 | Min/max dew-point sentence. |

## Pressure

| Story | Status | LOC | Notes |
| --- | --- | --- | --- |
| [`pressure_mean`](pressure/pressure_mean.md) | Trivial | 72 | Single mean-pressure sentence. |

## Road weather

| Story | Status | LOC | Notes |
| --- | --- | --- | --- |
| [`roadwarning_overview`](road/roadwarning_overview.md) | **Primary** | 1 353 | Road warning narrative (slipperiness, visibility, wind) — used by road-weather products. |
| [`roadcondition_overview`](road/roadcondition_overview.md) | **Primary** | 1 029 | Merges adjacent periods with similar road conditions; produces "aamusta alkaen …" phrases. |
| [`roadwarning_shortview`](road/roadwarning_shortview.md) | Active | 945 | Shorter variant of `roadwarning_overview`. |
| [`roadcondition_shortview`](road/roadcondition_shortview.md) | Active | 647 | Shorter variant of `roadcondition_overview`. |
| [`roadtemperature_daynightranges`](road/roadtemperature_daynightranges.md) | Active | 137 | Road surface temperature by day/night. |
| [`roadtemperature_shortrange`](road/roadtemperature_shortrange.md) | Active | 118 | Shorter variant. |

## Forest

| Story | Status | LOC | Notes |
| --- | --- | --- | --- |
| [`forestfireindex_twodays`](forest/forestfireindex_twodays.md) | Active | 146 | Two-day forest fire index numeric sequence. |
| [`forestfirewarning_county`](forest/forestfirewarning_county.md) | Active | 88 | Reads an external warning file and emits "in effect / not in effect". |
| [`evaporation_day`](forest/evaporation_day.md) | Active | 104 | Verbal evaporation class plus millimetres. |

## Wave

| Story | Status | LOC | Notes |
| --- | --- | --- | --- |
| [`wave_range`](wave/wave_range.md) | Specialized | 120 | Significant wave height range. Only story in `WaveStory`. No dedicated doc yet — see source. |

## Special (non-weather)

Dispatched by `SpecialStory` rather than a parameter class.

| Story | Status | LOC | Notes |
| --- | --- | --- | --- |
| [`none`](special/none.md) | Specialized | 43 | Emits an empty paragraph. Useful for placeholder slots. |
| [`date`](special/date.md) | Specialized | 72 | Emits a date phrase. |
| [`text`](special/text.md) | Specialized | 161 | Emits literal text taken from a config variable. |
| [`table`](special/table.md) | Specialized | — | Table rendering helper. Dispatched from `SpecialStory::hasStory` — no dedicated doc yet. |

## Generators that are *not* currently named stories

These exist as `.cpp` files but are not matched by any `hasStory()`; they
are helpers or residue from older refactors. Check before using.

* (none found in the current tree)
