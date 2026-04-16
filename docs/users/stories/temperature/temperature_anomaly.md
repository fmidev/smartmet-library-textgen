# Story "temperature_anomaly"

> **Status:** Primary. Temperature narrative that compares the forecast
> to climatology and reports the anomaly and short-run trend.
>
> **Owner:** `TemperatureStory::anomaly()`.
> **Implementation:** `textgen/temperature_anomaly.cpp` (~1 563 LOC).
>
> Source-mined. No separate spec document.

## What it produces

A paragraph combining:

1. An **anomaly sentence** that says how unusual the forecast is
   relative to a historical fractile (e.g. "Sää on
   poikkeuksellisen lämmintä", "Sää on harvinaisen lämmintä",
   "Sää on hyvin kylmää").
2. A **short-run trend sentence** that describes the day-to-day
   change (e.g. "Sää lämpenee huomattavasti", "Pakkanen heikkenee",
   "Helteinen sää jatkuu").

The two may be combined into a single sentence — see
`handle_anomaly_and_shortrun_trend_sentences()` in the source.

## Climatology dependency

The story reads a `tmax` climatology file:

```
textgen::tmax_climatology = /path/to/tmax/climatology
```

If the file is absent, the anomaly sentence cannot be emitted. Only the
short-run trend remains.

The climatology yields, per day in the forecast, the 12% and 37%
cold / warm fractile reference temperatures (`f12`, `f37`, and
symmetric upper fractiles). The current forecast value is placed into
one of the fractile bands, which selects the phrase.

## Anomaly phrases

Enumerated in the source as `anomaly_phrase_id`. The key phrases:

| Phrase id | English gloss |
| --- | --- |
| `SAA_ON_POIKKEUKSELLISEN_KOLEAA` | Exceptionally chilly |
| `SAA_ON_POIKKEUKSELLISEN_KYLMAA` | Exceptionally cold |
| `SAA_ON_KOLEAA` | Chilly |
| `SAA_ON_HYVIN_KYLMAA` | Very cold |
| `SAA_ON_HARVINAISEN_LAMMINTA` | Unusually warm |
| `SAA_ON_HYVIN_LEUTOA` | Very mild |
| `SAA_ON_POIKKEUKSLLISEN_LAMMINTA` | Exceptionally warm |
| `SAA_ON_POIKKEUKSLLISEN_LEUTOA` | Exceptionally mild |

Choice depends on which fractile band the forecast falls into and on
whether we are in a warm or cold season ("kylmä / kolea" for colder
seasons, "leuto / lämmin" for warmer seasons).

## Short-run trend phrases

Enumerated as `shortrun_trend_id`. Representative entries:

| Phrase id | English gloss |
| --- | --- |
| `SAA_ON_EDELLEEN_LAUHAA` | The weather remains mild |
| `SAA_LAUHTUU` | The weather becomes milder |
| `KIREA_PAKKANEN_HEIKKENEE` | The severe frost is weakening |
| `KIREA_PAKKANEN_HELLITTAA` | The severe frost is letting go |
| `PAKKANEN_HEIKKENEE` / `PAKKANEN_HELLITTAA` | Frost weakening / letting go |
| `KIREA_PAKKANEN_JATKUU` | The severe frost continues |
| `PAKKANEN_KIRISTYY` | The frost intensifies |
| `HELTEINEN_SAA_JATKUU` | The hot weather continues |
| `KOLEA_SAA_JATKUU` / `VIILEA_SAA_JATKUU` | Chilly / cool weather continues |
| `SAA_MUUTTUU_HELTEISEKSI` / `SAA_ON_HELTEISTA` | Becoming hot / it is hot |
| `SAA_LAMPENEE_HUOMATTAVASTI` / `SAA_LAMPENEE` / `SAA_LAMPENEE_VAHAN` | Warming significantly / warming / warming a little |
| `SAA_VIILENEE_HUOMATTAVASTI` / `SAA_VIILENEE` / `SAA_VIILENEE_VAHAN` | Cooling significantly / cooling / cooling a little |

The trend direction uses four anchor points: `dayBefore`, `day1`,
`day2`, `dayAfter`. The source distinguishes summer, winter, and
transition seasons.

## Change-size thresholds

Internal constants (source-level, not user-configurable) drive the
"vähän / huomattavasti" distinction:

| Constant | Value (°C) |
| --- | --- |
| `SMALL_CHANGE_LOWER_LIMIT` | 2.0 |
| `SMALL_CHANGE_UPPER_LIMIT` | 3.0 |
| `MODERATE_CHANGE_LOWER_LIMIT` | 3.0 |
| `MODERATE_CHANGE_UPPER_LIMIT` | 5.0 |
| `SIGNIFIGANT_CHANGE_LOWER_LIMIT` | 5.0 |
| `NOTABLE_TEMPERATURE_CHANGE_LIMIT` | 2.5 |

Other internal temperature thresholds:

| Constant | Value (°C) | Meaning |
| --- | --- | --- |
| `MILD_TEMPERATURE_LOWER_LIMIT` | −3.0 | Lower edge of the "mild" band |
| `MILD_TEMPERATURE_UPPER_LIMIT` | +5.0 | Upper edge of the "mild" band |
| `HOT_WEATHER_LIMIT` | +25.0 | "Helle" threshold (configurable, see below) |
| `GETTING_COOLER_NOTIFICATION_LIMIT` | +20.0 | Above this, "cooling" phrases are emitted |
| `VERY_COLD_TEMPERATURE_UPPER_LIMIT` | −10.0 | Very-cold band upper edge |

## Configuration parameters

All settings live under `textgen::[section]::story::temperature_anomaly::*`.

| Parameter | Default | Meaning |
| --- | --- | --- |
| `hot_weather_limit` | 25.0 °C | Temperature at which summer "helle" phrases kick in |

Global:

| Parameter | Meaning |
| --- | --- |
| `textgen::tmax_climatology` | Path to the TMAX climatology file used for the fractile bands |

## Area splitting

`temperature_anomaly` honours the same `areas_to_split` mechanism as
[`temperature_max36hours`](temperature_max36hours.md#area-splitting-horizontal--vertical).

## See also

* [`temperature_max36hours`](temperature_max36hours.md) — the absolute
  version of the same basic structure; use that when climatology is
  unavailable.
* [`wind_anomaly`](../wind/wind_anomaly.md) — wind version, companion
  story.
