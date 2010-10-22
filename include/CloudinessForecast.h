#ifndef TEXTGEN_CLOUDINESS_FORECAST_H
#define TEXTGEN_CLOUDINESS_FORECAST_H

#include "WeatherForecast.h"

namespace TextGen
{

using namespace Settings;
using namespace WeatherAnalysis;
using namespace AreaTools;
using namespace boost;
using namespace std;

  
  typedef std::pair<WeatherPeriod, cloudiness_id> weather_period_cloudiness_id_pair;
  typedef vector<weather_period_cloudiness_id_pair> cloudiness_period_vector;

  class CloudinessForecast
  {
  public:

	CloudinessForecast(wf_story_params& parameters);

	~CloudinessForecast(){}
	
	void setCoastalData(const weather_result_data_item_vector* coastalData) 
	{ 
	  theCoastalData = coastalData;
	  findOutCloudinessPeriods(theCoastalData, theCloudinessPeriodsCoastal);
	  joinPeriods(theCoastalData, theCloudinessPeriodsCoastal, theCloudinessPeriodsCoastalJoined);
	  findOutCloudinessTrends(theCoastalData, theCloudinessTrendsCoastal);
	}
	void setInlandData(const weather_result_data_item_vector* inlandData) 
	{ 
	  theInlandData = inlandData; 
	  findOutCloudinessPeriods(theInlandData, theCloudinessPeriodsInland);
	  joinPeriods(theInlandData, theCloudinessPeriodsInland, theCloudinessPeriodsInlandJoined);
	  findOutCloudinessTrends(theInlandData, theCloudinessTrendsInland);
	}
	void setFullData(const weather_result_data_item_vector* fullData) 
	{ 
	  theFullData = fullData; 
	  findOutCloudinessPeriods(theFullData, theCloudinessPeriodsFull);
	  joinPeriods(theFullData, theCloudinessPeriodsFull, theCloudinessPeriodsFullJoined);	
	  findOutCloudinessTrends(theFullData, theCloudinessTrendsFull);
	}

	const weather_result_data_item_vector* getCoastalData() const { return theCoastalData; }
	const weather_result_data_item_vector* getInlandData() const { return theInlandData; }
	const weather_result_data_item_vector* getFullData() const { return theFullData; }

	Sentence cloudinessSentence(const WeatherPeriod& thePeriod,	const bool& theShortForm = true) const;
	Sentence cloudinessSentence(const WeatherPeriod& thePeriod, 
								const weather_result_data_item_vector& theCloudinessData) const;
	Sentence cloudinessChangeSentence(const WeatherPeriod& thePeriod) const;

	/*
	Sentence cloudinessSentence(const unsigned int& thePeriodNumber,
								const bool& theCheckCloudinessChange = true);
	*/
	cloudiness_id getCloudinessId(const WeatherPeriod& thePeriod) const;
	cloudiness_id getCloudinessId(const WeatherPeriod& thePeriod,
								  const weather_result_data_item_vector* theCloudinessData) const;

	void getTrendIdVector(trend_id_vector& theCloudinessTrends) const;

	void printOutCloudinessData(std::ostream& theOutput) const;
	void printOutCloudinessTrends(std::ostream& theOutput) const;
	void printOutCloudinessPeriods(std::ostream& theOutput) const;

  private:
	
	float getMeanCloudiness(const WeatherPeriod& theWeatherPeriod,
							const weather_result_data_item_vector& theDataVector) const;
	bool separateCoastInlandCloudiness(const WeatherPeriod& theWeatherPeriod) const;
	// e.g. separate morning afternoon
	bool separateWeatherPeriodCloudiness(const WeatherPeriod& theWeatherPeriod1, 
										 const WeatherPeriod& theWeatherPeriod2,
										 const weather_result_data_item_vector& theCloudinessData) const;

	void printOutCloudinessData(std::ostream& theOutput,
								const weather_result_data_item_vector* theDataVector) const;
	void getWeatherPeriodCloudiness(const WeatherPeriod& thePeriod,
									const cloudiness_period_vector& theSourceCloudinessPeriods,
									cloudiness_period_vector& theWeatherPeriodCloudiness) const;
	  
	void printOutCloudinessPeriods(std::ostream& theOutput, const cloudiness_period_vector& theCloudinessPeriods) const;

	void findOutCloudinessPeriods(const weather_result_data_item_vector* theData, 
								 cloudiness_period_vector& theCloudinessPeriods);
	void findOutCloudinessPeriods();
	void findOutCloudinessTrends(const weather_result_data_item_vector* theData,
								 trend_id_vector& theCloudinessTrends);
	void findOutCloudinessTrends();

	void joinPeriods(const weather_result_data_item_vector* theDataSource,
					 const cloudiness_period_vector& theCloudinessPeriodsSource,
					 cloudiness_period_vector& theCloudinessPeriodsDestination);
	void joinPeriods();
	cloudiness_id getCloudinessPeriodId(const NFmiTime& theObservationTime,
										const cloudiness_period_vector& theCloudinessPeriodVector) const;
	Sentence areaSpecificSentence(const WeatherPeriod& thePeriod) const;

	wf_story_params& theParameters;
	const weather_result_data_item_vector* theCoastalData;
	const weather_result_data_item_vector* theInlandData;
	const weather_result_data_item_vector* theFullData;

	cloudiness_period_vector theCloudinessPeriodsCoastal;
	cloudiness_period_vector theCloudinessPeriodsInland;
	cloudiness_period_vector theCloudinessPeriodsFull;

	cloudiness_period_vector theCloudinessPeriodsCoastalJoined;
	cloudiness_period_vector theCloudinessPeriodsInlandJoined;
	cloudiness_period_vector theCloudinessPeriodsFullJoined;

	trend_id_vector theCloudinessTrendsCoastal;
	trend_id_vector theCloudinessTrendsInland;
	trend_id_vector theCloudinessTrendsFull;

	bool theSeparateCoastInlandMorning;
	bool theSeparateCoastInlandAfternoon;
	bool theSeparateMorningAfternoon; // full area

  };

  const char* cloudiness_string(const cloudiness_id& theCloudinessId);
  bool puolipilvisesta_pilviseen(const cloudiness_id& theCloudinessId1,
								 const cloudiness_id& theCloudinessId2);
  cloudiness_id get_cloudiness_id(const float& theMin, 
								  const float& theMean, 
								  const float& theMax, 
								  const float& theStandardDeviation);
  cloudiness_id get_cloudiness_id(const float& theCloudiness);
  Sentence cloudiness_sentence(const cloudiness_id& theCloudinessId);
  //							   const bool& theShortForm = false);

  std::ostream& operator<<(std::ostream & theOutput,
						   const CloudinessDataItemData& theCloudinessDataItemData);
  std::ostream& operator<<(std::ostream & theOutput,
						   const CloudinessDataItem& theCloudinessDataItem);

} // namespace TextGen

#endif // TEXTGEN_CLOUDINESS_FORECAST_H
