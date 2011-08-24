#ifndef TEXTGEN_WIND_FORECAST_H
#define TEXTGEN_WIND_FORECAST_H

#include "WeatherForecast.h"

namespace TextGen
{

using namespace Settings;
using namespace WeatherAnalysis;
using namespace AreaTools;
using namespace boost;
using namespace std;


  class WindForecast
  {
  public:

	WindForecast(wo_story_params& parameters);

	~WindForecast(){}
		
	Sentence windSentence(const WeatherPeriod& thePeriod) const;
	Sentence windSentenceWithEvents(const WeatherPeriod& thePeriod) const;
 
	void printOutWindPeriods(std::ostream& theOutput) const;
	void printOutWindData(std::ostream& theOutput) const;

  private:
	

	wo_story_params& theParameters;
	mutable int thePreviousRangeBeg;
	mutable int thePreviousRangeEnd;

	const Sentence direction_sentence(const wind_direction_id& theDirectionId) const;
	const Sentence direction16_sentence(const wind_direction16_id& theDirectionId) const;
	const Sentence wind_speed_sentence(const WeatherPeriod& thePeriod) const;
	const Sentence speed_range_sentence_(const WeatherResult & theMinSpeed,
										const WeatherResult & theMaxSpeed,
										const WeatherResult & theMeanSpeed,
										const string & theVariable) const;

	/*
	const weather_result_data_item_vector* theCoastalModerateFogData;
	const weather_result_data_item_vector* theInlandModerateFogData;
	const weather_result_data_item_vector* theFullAreaModerateFogData;
	const weather_result_data_item_vector* theCoastalDenseFogData;
	const weather_result_data_item_vector* theInlandDenseFogData;
	const weather_result_data_item_vector* theFullAreaDenseFogData;

	fog_period_vector theCoastalFog;
	fog_period_vector theInlandFog;
	fog_period_vector theFullAreaFog;

	fog_type_period_vector theCoastalFogType;
	fog_type_period_vector theInlandFogType;
	fog_type_period_vector theFullAreaFogType;

	static std::string theDayPhasePhraseOld;
	*/
  };
} // namespace TextGen

#endif // TEXTGEN_WIND_FORECAST_H
