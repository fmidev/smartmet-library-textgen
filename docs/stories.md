# Stories

1. [What is a story](#story)
1. [StoryFactory namespace](#storyfactory)
1. [Special stories](#specialstory)
1. [TemperatureStory](#temperaturestory)
   1. [story_temperature_day](story_temperature_day.md)
   1. [story_temperature_mean](story_temperature_mean.md)
   1. [story_temperature_meanmax](story_temperature_meanmax.md)
   1. [story_temperature_meanmin](story_temperature_meanmin.md)
   1. [story_temperature_dailymax](story_temperature_dailymax.md)
   1. [story_temperature_nightlymin](story_temperature_nightlymin.md)
   1. [story_temperature_weekly_minmax](story_temperature_weekly_minmax.md)
   1. [story_temperature_weekly_averages](story_temperature_weekly_averages.md)
   1. [story_temperature_weekly_averages_trend](story_temperature_weekly_averages_trend.md)
1. [DewPointStory](#dewpointstory)
   1. [story_dewpoint_range](story_dewpoint_range.md)
1. [PrecipitationStory](#precipitationstory)
   1. [story_precipitation_total](story_precipitation_total.md)
   1. [story_precipitation_range](story_precipitation_range.md)
   1. [story_precipitation_classification](story_precipitation_classification.md)
   1. [story_precipitation_daily_sums](story_precipitation_daily_sums.md)
   1. [story_pop_max](story_pop_max.md)
   1. [story_pop_days](story_pop_days.md)
   1. [story_pop_twodays](story_pop_twodays.md)
1. [CloudinessStory](#cloudinesstory)
   1. [story_cloudiness_overview](story_cloudiness_overview.md)
1. [WeatherStory](#weatherstory)
   1. [story_weather_overview](story_weather_overview.md)
   1. [story_weather_shortoverview](story_weather_shortoverview.md) (precipitation form not implemented)
   1. [story_weather_thunderprobability](story_weather_thunderprobability.md)
1. [WindStory](#windstory)
   1. [story_wind_range](story_wind_range.md)
   1. [story_wind_daily_ranges](story_wind_daily_ranges.md)
   1. [story_wind_summary](story_wind_summary.md)
   1. [story_wind_simple_overview](story_wind_simple_overview.md) (missing)
   1. [story_wind_overview](story_wind_overview.md) (missing)
1. [FrostStory](#froststory)
   1. [story_frost_day](story_frost_day.md)
   1. [story_frost_mean](story_frost_mean.md)
   1. [story_frost_maximum](story_frost_maximum.md)
   1. [story_frost_range](story_frost_range.md)
   1. [story_frost_twonights](story_frost_twonights.md)
1. [RelativeHumidityStory](#relativehumidity_story)
   1. [story_relativehumidity_day](story_relativehumidity_day.md)
   1. [story_relativehumidity_lowest](story_relativehumidity_lowest.md)
   1. [story_relativehumidity_range](story_relativehumidity_range.md)
1. [RoadStory](#roadstory)
   1. [story_roadtemperature_daynightranges](story_roadtemperature_daynightranges.md)
   1. [story_roadcondition_overview](story_roadcondition_overview.md)
1. [ForestStory](#foreststory)
   1. [story_forestfirewarning_county](story_forestfirewarning_county.md)
   1. [story_forestfireindex_twodays](story_forestfireindex_twodays.md)
   1. [story_evaporation_day](story_evaporation_day.md)
1. [PressureStory](#pressurestory)
   1. [story_pressure_mean](story_pressure_mean.md)

<a id="story"></a>
## What is a story

A story is a single paragraph that contains a description related to a single weather parameter. The paragraph may be empty if there is nothing to report about the parameter, or it may consist of one or more sentences.

<a id="storyfactory"></a>
## StoryFactory namespace

The `TextGen::StoryFactory` namespace contains a `create` function that can be used to create named stories.

<a id="specialstory"></a>
## Special stories

A story deviating from the norm is "none", which returns an empty `Paragraph` object.

<a id="temperaturestory"></a>
## TemperatureStory class

The `TextGen::TemperatureStory` class manages stories related to temperature.

<a id="precipitationstory"></a>
## PrecipitationStory class

The `TextGen::PrecipitationStory` class manages stories related to precipitation.

One central problem is how total precipitation is defined. For example, when talking about the average total precipitation for an area, should only the points that actually receive some precipitation be included, or all points?

<a id="cloudinesstory"></a>
## CloudinessStory class

The `TextGen::CloudinessStory` class manages stories related purely to cloudiness.

<a id="weatherstory"></a>
## WeatherStory class

The `TextGen::WeatherStory` class manages stories related to general weather.

<a id="windstory"></a>
## WindStory class

The `TextGen::WindStory` class manages stories related to wind.

<a id="froststory"></a>
## FrostStory class

The `TextGen::FrostStory` class manages stories related to frost.

<a id="relativehumidity_story"></a>
## RelativeHumidityStory class

The `TextGen::RelativeHumidityStory` class manages stories related to relative humidity.

<a id="dewpointstory"></a>
## DewPointStory class

The `TextGen::DewPointStory` class manages stories related to dew point.

<a id="roadstory"></a>
## RoadStory class

The `TextGen::RoadStory` class manages stories related to road weather.

<a id="foreststory"></a>
## ForestStory class

The `TextGen::ForestStory` class manages stories related to the humidity model and forest fire warnings.

<a id="pressurestory"></a>
## PressureStory class

The `TextGen::PressureStory` class manages stories related to atmospheric pressure.
