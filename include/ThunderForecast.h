#ifndef TEXTGEN_THUNDER_FORECAST_H
#define TEXTGEN_THUNDER_FORECAST_H

#include "WeatherForecast.h"
#include "PrecipitationForecast.h"

namespace TextGen
{

using namespace Settings;
using namespace WeatherAnalysis;
using namespace AreaTools;
using namespace boost;
using namespace std;


  class ThunderForecast
  {
  public:

	ThunderForecast(wf_story_params& parameters);
		
	Sentence thunderSentence(const WeatherPeriod& thePeriod) const;

  private:
	
	Sentence areaSpecificSentence(const WeatherPeriod& thePeriod) const;
	float getMaxThunderProbability(const WeatherPeriod& theWeatherPeriod,
								   const weather_result_data_item_vector& theDataVector) const;


	wf_story_params& theParameters;

	const weather_result_data_item_vector* theCoastalData;
	const weather_result_data_item_vector* theInlandData;
	const weather_result_data_item_vector* theFullAreaData;
  };
} // namespace TextGen

#endif // TEXTGEN_THUNDER_FORECAST_H
