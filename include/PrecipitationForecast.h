#ifndef TEXTGEN_PRECIPITATION_FORECAST_H
#define TEXTGEN_PRECIPITATION_FORECAST_H

#include "WeatherForecast.h"

namespace TextGen
{

using namespace Settings;
using namespace WeatherAnalysis;
using namespace AreaTools;
using namespace boost;
using namespace std;


  Sentence precipitation_sentence(wf_story_params& theParameters,
								  const unsigned int& thePrecipitationForm,
								  const float thePrecipitationIntensity,
								  const float thePrecipitationExtent,
								  const float thePrecipitationFormWater,
								  const float thePrecipitationFormDrizzle,
								  const float thePrecipitationFormSleet,
								  const float thePrecipitationFormSnow,
								  const float thePrecipitationFormFreezing,
								  const float thePrecipitationTypeShowers);

  bool is_dry_weather(const wf_story_params& theParameters,
					  const unsigned int& thePrecipitationForm,
					  const float& thePrecipitationIntensity,
					  const float& thePrecipitationExtent);

  const char* precipitation_phrase(const NFmiTime& theTime, const string& theVariable);

  class PrecipitationForecast
  {
  public:

	PrecipitationForecast(wf_story_params& parameters);
	~PrecipitationForecast();

	Sentence precipitationChangeSentence(const WeatherPeriod& thePeriod) const;
	Sentence precipitationSentence(const WeatherPeriod& thePeriod, 
								   const bool& theCheckPrecipitationChange = true) const;
	Sentence shortTermPrecipitationSentence(const WeatherPeriod& thePeriod) const;
	bool isDryPeriod(const WeatherPeriod& theWeatherPeriod, 
					 const unsigned short theForecastPeriodId) const;
	bool getDryPeriods(const WeatherPeriod& theSourcePeriod, 
					   vector<WeatherPeriod>& theDryPeriods) const;
	bool getPrecipitationPeriods(const WeatherPeriod& theSourcePeriod, 
								 vector<WeatherPeriod>& thePrecipitationPeriods) const;

	void getTrendIdVector(trend_id_vector& thePrecipitationTrends) const;

	void printOutPrecipitationVector(std::ostream & theOutput,
									 const precipitation_data_vector& thePrecipitationDataVector) const;
	void printOutPrecipitationData(std::ostream & theOutput) const;
	void printOutPrecipitationPeriods(std::ostream& theOutput) const;
	void printOutPrecipitationTrends(std::ostream& theOutput) const;
	void printOutPrecipitationDistribution(std::ostream& theOutput) const;

  private:

	bool getPrecipitationPeriod(const NFmiTime& theTimestamp, NFmiTime& theStartTime, NFmiTime& theEndTime) const;
	void printOutPrecipitationPeriods(std::ostream& theOutput,
									  const vector<WeatherPeriod>& thePrecipitationPeriods,
									  const precipitation_data_vector& theDataVector) const;
	void printOutPrecipitationDistribution(std::ostream& theOutput, 
										   const precipitation_data_vector& theDataVector) const;
	void printOutPrecipitationDistribution(std::ostream& theOutput,
										   const WeatherPeriod& thePeriod);
	void getPrecipitationDistribution(const WeatherPeriod& thePeriod,
									  float& theNorthPercentage,
									  float& theSouthPercentage,
									  float& theEastPercentage,
									  float& theWestPercentage,
									  float& theNorthEastPercentage,
									  float& theSouthEastPercentage,
									  float& theSouthWestPercentage,
									  float& theNorthWestPercentage) const;
	void cleanUpPrecipitationTrends(trend_id_vector& thePrecipitationTrends);
	void removeRedundantTrends(trend_id_vector& thePrecipitationTrends, const vector<int>& theRemoveIndexes);
	void removeDuplicatePrecipitationTrends(trend_id_vector& thePrecipitationTrends);
	float getMin(const precipitation_data_vector& theData, 
				  const weather_result_data_id& theDataId,
				  const WeatherPeriod& theWeatherPeriod) const;
	float getMax(const precipitation_data_vector& theData, 
				  const weather_result_data_id& theDataId,
				  const WeatherPeriod& theWeatherPeriod) const;
	float getMean(const precipitation_data_vector& theData, 
				  const weather_result_data_id& theDataId,
				  const WeatherPeriod& theWeatherPeriod) const;

	unsigned int getPrecipitationForm(const precipitation_data_vector& theData,
									  const WeatherPeriod& theWeatherPeriod) const;

	unsigned int getPrecipitationGategory(const float& thePrecipitation, 
										  const unsigned int& theType) const;
	bool separateCoastInlandPrecipitation(const WeatherPeriod& theWeatherPeriod) const;
	void gatherPrecipitationData();
	void fillInPrecipitationDataVector(const forecast_area_id& theAreaId);
	void findOutPrecipitationPeriods();
	void findOutPrecipitationPeriods(const forecast_area_id& theAreaId);
	
	void findOutPrecipitationTrends();
	void findOutPrecipitationTrends(const precipitation_data_vector& theData,
									trend_id_vector& thePrecipitationTrends);
	void findOutPrecipitationTrends(const vector<WeatherPeriod>& thePrecipitationPeriods,
									trend_id_vector& thePrecipitationTrends);
	Sentence constructPrecipitationSentence(const WeatherPeriod& thePeriod,
											const bool& theCheckPrecipitationChange, 
											const unsigned short& theForecastAreaId) const;

	float getStat(const precipitation_data_vector& theData, 
				  const weather_result_data_id& theDataId,
				  const WeatherPeriod& theWeatherPeriod,
				  const stat_func_id& theStatFunc) const;

	/*
	const weather_result_data_item_vector* theCoastalData;
	const weather_result_data_item_vector* theInlandData;
	const weather_result_data_item_vector* theFullData;
	*/

	precipitation_data_vector theCoastalData;
    precipitation_data_vector theInlandData;
    precipitation_data_vector theFullData;

	trend_id_vector thePrecipitationTrendsCoastal;
	trend_id_vector thePrecipitationTrendsInland;
	trend_id_vector thePrecipitationTrendsFull;

	vector<WeatherPeriod> thePrecipitationPeriodsCoastal;
	vector<WeatherPeriod> thePrecipitationPeriodsInland;
	vector<WeatherPeriod> thePrecipitationPeriodsFull;

	wf_story_params& theParameters;

  };


} // namespace TextGen

#endif // TEXTGEN_PRECIPITATION_FORECAST_H
