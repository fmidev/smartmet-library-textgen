# Story "wind_anomaly"

> **Status:** Primary. Wind narrative coupled with temperature to
> describe windiness and wind chill.
>
> **Owner:** `WindStory::anomaly()`.
> **Implementation:** `textgen/wind_anomaly.cpp` (~2 075 LOC).
>
> Source-mined. No separate spec document.

## What it produces

Up to two sentences:

1. A **windiness sentence** describing how windy the period is —
   "tuulinen sää jatkuu", "tuuli on voimakasta", etc.
2. A **wind-cooling sentence** describing how the wind affects the
   perceived temperature when it cools the air meaningfully —
   "tuuli kylmentää säätä", "tuuli viilentää säätä",
   "tuuli saa sään tuntumaan viileämmältä".

Either sentence can be suppressed via configuration.

## Windiness thresholds

Internal constants drive the "tuulinen / erittäin tuulinen / koleaksi"
classification:

| Constant | Value (m/s) | Meaning |
| --- | --- | --- |
| `WINDY_WEATER_LIMIT` | 7.0 | Lower bound for "tuulinen / windy" |
| `EXTREMELY_WINDY_WEATHER_LIMIT` | 10.0 | Lower bound for "erittäin tuulinen / extremely windy" |
| `WIND_COOLING_THE_WEATHER_LIMIT` | 6.0 | Minimum wind speed for the cooling sentence |

## Wind-chill thresholds

The wind-cooling sentence uses wind chill, not just wind speed:

| Constant | Value | Meaning |
| --- | --- | --- |
| `EXTREME_WINDCHILL_LIMIT` | −35.0 °C | Windchill below which the cooling sentence is always emitted |
| `MILD_WINDCHILL_LIMIT` | −25.0 °C | Milder windchill threshold for a softer phrasing |
| `TEMPERATURE_AND_WINDCHILL_DIFFERENCE_LIMIT` | 7.0 °C | Minimum (temperature − windchill) gap before the cooling sentence triggers |

Further temperature bands for wind-cooling phrasing:

| Constant | Value (°C) | Meaning |
| --- | --- | --- |
| `TUULI_KYLMENTAA_SAATA_LOWER_LIMIT` | 0.0 | Lower edge of the band where "tuuli kylmentää" is the natural phrase |
| `TUULI_KYLMENTAA_SAATA_UPPER_LIMIT` | 10.0 | Upper edge of the same band |
| `TUULI_VIILENTAA_SAATA_LOWER_LIMIT` | 10.0 | Lower edge of the "tuuli viilentää" band |
| `TUULI_VIILENTAA_SAATA_UPPER_LIMIT` | 15.0 | Upper edge |

## Configuration parameters

All settings live under `textgen::[section]::story::wind_anomaly::*`.

| Parameter | Default | Meaning |
| --- | --- | --- |
| `windy_weather_limit` | 7.0 m/s | Override of `WINDY_WEATER_LIMIT` |
| `extremely_windy_weather_limit` | 10.0 m/s | Override of `EXTREMELY_WINDY_WEATHER_LIMIT` |
| `wind_cooling_the_weather_limit` | 6.0 m/s | Override of `WIND_COOLING_THE_WEATHER_LIMIT` |
| `generate_windiness_sentence` | true | If false, skip the windiness sentence |
| `generate_wind_cooling_sentence` | true | If false, skip the wind-cooling sentence |
| `specify_part_of_the_day` | true | Mirror of the `temperature_max36hours` option; allows the sentence to name morning / afternoon when the day is long enough |
| `separate_coastal_area_percentage` | (source) | Minimum share of the area that must be coastal for coast/inland split |

## Area splitting

`wind_anomaly` uses the same `check_area_splitting()` path as
[`temperature_anomaly`](../temperature/temperature_anomaly.md#area-splitting)
and [`temperature_max36hours`](../temperature/temperature_max36hours.md#area-splitting-horizontal--vertical).
The same `textgen::<section>::story::wind_anomaly::areas_to_split`
and `textgen::split_the_area::<area>::method` variables apply.

## Relationship to other wind stories

`wind_anomaly` is typically emitted **together with**
[`wind_overview`](wind_overview.md) (for the main wind narrative) or
[`temperature_anomaly`](../temperature/temperature_anomaly.md) (for the
temperature context). It rarely stands alone.

## See also

* [`wind_overview`](wind_overview.md) — primary wind narrative (no
  climatology required)
* [`temperature_anomaly`](../temperature/temperature_anomaly.md) — the
  temperature-side companion
