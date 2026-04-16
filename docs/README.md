# TextGen documentation index

This index lists the documentation files in this directory.

## Main topics

* [textgen](textgen.md) — documentation overview / main page
* [dictionaries](dictionaries.md) — dictionary objects and the singleton used across the text generator
* [sentences](sentences.md) — building Documents, Paragraphs, Sentences, Phrases, and Numbers
* [timeperiods](timeperiods.md) — naming conventions for times of day
* [periods](periods.md) — creating weather periods with `WeatherPeriodFactory`
* [subperiods](subperiods.md) — splitting a weather period into days
* [titles](titles.md) — header types and the `HeaderFactory` namespace
* [stories](stories.md) — what stories are and the `StoryFactory` namespace
* [period_phrases](period_phrases.md) — phrases describing time periods ("today", "tonight", ...)
* [quality](quality.md) — the quality value and how it is computed
* [functions](functions.md) — analysis functions (`Mean`, `Maximum`, `Trend`, ...)

## Header types

* [header_none](header_none.md)
* [header_until](header_until.md)
* [header_clock_range](header_clock_range.md)
* [header_from_until](header_from_until.md)
* [header_several_days](header_several_days.md)
* [header_morning](header_morning.md)
* [header_forenoon](header_forenoon.md)
* [header_afternoon](header_afternoon.md)
* [header_evening](header_evening.md)
* [header_report_time](header_report_time.md)
* [header_report_area](header_report_area.md)
* [header_report_location](header_report_location.md)

## Stories by weather parameter

### Temperature

* [story_temperature_day](story_temperature_day.md)
* [story_temperature_mean](story_temperature_mean.md)
* [story_temperature_meanmax](story_temperature_meanmax.md)
* [story_temperature_meanmin](story_temperature_meanmin.md)
* [story_temperature_dailymax](story_temperature_dailymax.md)
* [story_temperature_nightlymin](story_temperature_nightlymin.md)
* [story_temperature_minmax](story_temperature_minmax.md)
* [story_temperature_range](story_temperature_range.md)
* [story_temperature_weekly_minmax](story_temperature_weekly_minmax.md)
* [story_temperature_weekly_averages](story_temperature_weekly_averages.md)
* [story_temperature_weekly_averages_trend](story_temperature_weekly_averages_trend.md)

### Dew point and humidity

* [story_dewpoint_range](story_dewpoint_range.md)
* [story_relativehumidity_day](story_relativehumidity_day.md)
* [story_relativehumidity_lowest](story_relativehumidity_lowest.md)
* [story_relativehumidity_range](story_relativehumidity_range.md)

### Precipitation

* [story_precipitation_total](story_precipitation_total.md)
* [story_precipitation_range](story_precipitation_range.md)
* [story_precipitation_classification](story_precipitation_classification.md)
* [story_precipitation_daily_sums](story_precipitation_daily_sums.md)
* [story_precipitation_sums](story_precipitation_sums.md)
* [story_pop_max](story_pop_max.md)
* [story_pop_days](story_pop_days.md)
* [story_pop_twodays](story_pop_twodays.md)
* [rain_oneday](rain_oneday.md) — one-day rain case enumeration (referenced by weather_overview)
* [rain_twoday](rain_twoday.md) — two-day rain case enumeration (referenced by weather_overview)

### Cloudiness & weather

* [story_cloudiness_overview](story_cloudiness_overview.md)
* [story_weather_overview](story_weather_overview.md)
* [story_weather_shortoverview](story_weather_shortoverview.md)
* [story_weather_thunderprobability](story_weather_thunderprobability.md)

### Wind

* [story_wind_range](story_wind_range.md)
* [story_wind_daily_ranges](story_wind_daily_ranges.md)
* [story_wind_summary](story_wind_summary.md)
* [story_wind_simple_overview](story_wind_simple_overview.md)
* [story_wind_overview](story_wind_overview.md)

### Frost

* [story_frost_day](story_frost_day.md)
* [story_frost_mean](story_frost_mean.md)
* [story_frost_maximum](story_frost_maximum.md)
* [story_frost_range](story_frost_range.md)
* [story_frost_twonights](story_frost_twonights.md)

### Pressure

* [story_pressure_mean](story_pressure_mean.md)

### Road weather

* [story_roadtemperature_daynightranges](story_roadtemperature_daynightranges.md)
* [story_roadcondition_overview](story_roadcondition_overview.md)

### Forest

* [story_forestfirewarning_county](story_forestfirewarning_county.md)
* [story_forestfireindex_twodays](story_forestfireindex_twodays.md)
* [story_evaporation_day](story_evaporation_day.md)
