#pragma once

#include "PrecipitationForecast.h"
#include "WeatherForecast.h"

namespace TextGen
{
class ThunderForecast
{
 public:
  ThunderForecast(wf_story_params& parameters);

  Sentence thunderSentence(const WeatherPeriod& thePeriod,
                           const AreaTools::forecast_area_id& theForecastAreaId,
                           const std::string& theVariable) const;

 private:
  Sentence areaSpecificSentence(const WeatherPeriod& thePeriod) const;
  float getMaxValue(const WeatherPeriod& theWeatherPeriod,
                    const weather_result_data_item_vector& theDataVector) const;

  wf_story_params& theParameters;
};
}  // namespace TextGen
