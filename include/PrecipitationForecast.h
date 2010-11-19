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


  class PrecipitationForecast
  {
  public:

	PrecipitationForecast(wf_story_params& parameters);
	~PrecipitationForecast();

	Sentence precipitationChangeSentence(const WeatherPeriod& thePeriod) const;
	Sentence precipitationSentence(const WeatherPeriod& thePeriod) const;
	std::string precipitationSentenceString(const WeatherPeriod& thePeriod, 
											const bool& theCheckPrecipitationChange = true) const;
	bool shortTermPrecipitationExists(const WeatherPeriod& thePeriod) const;
	Sentence shortTermPrecipitationSentence(const WeatherPeriod& thePeriod) const;
	bool isDryPeriod(const WeatherPeriod& theWeatherPeriod, 
					 const unsigned short theForecastAreaId) const;
	bool isMostlyDryPeriod(const WeatherPeriod& theWeatherPeriod,
						   const unsigned short theForecastArea) const;
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
	void useOllaVerb(const bool theFlag = true) { theUseOllaVerb = theFlag; }
	void dryPeriodTautologyFlag(const bool theFlag) { theDryPeriodTautologyFlag = theFlag; }
	std::string precipitationSentenceString(const WeatherPeriod& thePeriod);
	
  private:

	Rect getPrecipitationRect(const NFmiTime& theTimestamp, 
							  const float& theLowerLimit,
							  const float& theUpperLimit) const;
	NFmiPoint getPrecipitationRepresentativePoint(const WeatherPeriod& thePeriod) const;
	direction_id getPrecipitationArrivalDirection(const WeatherPeriod& thePeriod) const;
	direction_id getPrecipitationLeavingDirection(const WeatherPeriod& thePeriod) const;
	precipitation_traverse_id getPrecipitationTraverseId(const WeatherPeriod& thePeriod) const;
	const precipitation_data_vector& getPrecipitationDataVector() const;
	precipitation_form_transformation_id getPrecipitationFormTransformationId(const WeatherPeriod& thePeriod,
																			  const precipitation_data_vector& theDataVector) const;
	bool getIntensityFormExtent(const WeatherPeriod& theWeatherPeriod,
								const unsigned short theForecastArea,
								float& theIntensity,
								unsigned int& theForm,
								float& theExtent) const;
	Sentence areaSpecificSentence(const WeatherPeriod& thePeriod) const;
	void joinPrecipitationPeriods(vector<WeatherPeriod>& thePrecipitationPeriodVector);
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

	precipitation_type getPrecipitationType(const precipitation_data_vector& theData,
											const WeatherPeriod& thePeriod) const;
	unsigned int getPrecipitationForm(const precipitation_data_vector& theDataVector,
									  const WeatherPeriod& thePeriod) const;
	unsigned int getPrecipitationCategory(const float& thePrecipitation, 
										  const unsigned int& theType) const;
	bool separateCoastInlandPrecipitation(const WeatherPeriod& theWeatherPeriod) const;
	void gatherPrecipitationData();
	void fillInPrecipitationDataVector(const forecast_area_id& theAreaId);
	void findOutPrecipitationPeriods();
	void findOutPrecipitationPeriods(const forecast_area_id& theAreaId);
	
	void findOutPrecipitationTrends();
	/*
	void findOutPrecipitationTrends(const precipitation_data_vector& theData,
									trend_id_vector& thePrecipitationTrends);
	*/
	void findOutPrecipitationTrends(const vector<WeatherPeriod>& thePrecipitationPeriods,
									trend_id_vector& thePrecipitationTrends);

	void waterAndSnowShowersPhrase(const float& thePrecipitationIntensity,
								   const float& theWaterDrizzleSleetShare,
								   const bool& theCanBeFreezingFlag,
								   vector<std::string>& theStringVector) const;

	void mostlyDryWeatherPhrase(const bool& theIsShowersFlag,
								const WeatherPeriod& thePeriod,
const char* thePhrase,
								vector<std::string>& theStringVector) const;

	int precipitationSentenceStringVectorTransformation(const WeatherPeriod& thePeriod,
														const float& thePrecipitationExtent,
														const precipitation_form_transformation_id& theTransformId,
														vector<std::string>& theStringVector) const;
	int precipitationSentenceStringVector(const WeatherPeriod& thePeriod,
										  const unsigned int& thePrecipitationForm,
										  const float& thePrecipitationIntensity,
										  const float& thePrecipitationExtent,
										  const float& thePrecipitationFormWater,
										  const float& thePrecipitationFormDrizzle,
										  const float& thePrecipitationFormSleet,
										  const float& thePrecipitationFormSnow,
										  const float& thePrecipitationFormFreezing,
										  const precipitation_type& thePrecipitationType,
										  vector<std::string>& theStringVector) const;
	  std::string precipitationSentenceString(const WeatherPeriod& thePeriod,
											  const unsigned int& thePrecipitationForm,
											  const float thePrecipitationIntensity,
											  const float thePrecipitationExtent,
											  const float thePrecipitationFormWater,
											  const float thePrecipitationFormDrizzle,
											  const float thePrecipitationFormSleet,
											  const float thePrecipitationFormSnow,
											  const float thePrecipitationFormFreezing,
											  const precipitation_type& thePrecipitationType) const;
	Sentence selectPrecipitationSentence(const WeatherPeriod& thePeriod,
										 const unsigned int& thePrecipitationForm,
										 const float thePrecipitationIntensity,
										 const float thePrecipitationExtent,
										 const float thePrecipitationFormWater,
										 const float thePrecipitationFormDrizzle,
										 const float thePrecipitationFormSleet,
										 const float thePrecipitationFormSnow,
										 const float thePrecipitationFormFreezing,
										 const precipitation_type& thePrecipitationType,
										 const precipitation_form_transformation_id& theTransformationId) const;

	Sentence constructPrecipitationSentence(const WeatherPeriod& thePeriod,
											const unsigned short& theForecastAreaId) const;

	float getStat(const precipitation_data_vector& theData, 
				  const weather_result_data_id& theDataId,
				  const WeatherPeriod& theWeatherPeriod,
				  const stat_func_id& theStatFunc) const;

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

	mutable bool theUseOllaVerb;
	mutable bool theDryPeriodTautologyFlag;
  };


} // namespace TextGen

#endif // TEXTGEN_PRECIPITATION_FORECAST_H
