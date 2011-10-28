#ifndef TEXTGEN_WIND_FORECAST_H
#define TEXTGEN_WIND_FORECAST_H

#include "WeatherForecast.h"
#include "WindStoryTools.h"

namespace TextGen
{

using namespace Settings;
using namespace WeatherAnalysis;
using namespace AreaTools;
using namespace boost;
using namespace std;
  using namespace WindStoryTools;

#define HEIKKO_LOWER_LIMIT 0.5
#define HEIKKO_UPPER_LIMIT 3.5
#define KOHTALAINEN_LOWER_LIMIT 3.5
#define KOHTALAINEN_UPPER_LIMIT 7.5
#define NAVAKKA_LOWER_LIMIT 7.5
#define NAVAKKA_UPPER_LIMIT 13.5
#define KOVA_LOWER_LIMIT 13.5
#define KOVA_UPPER_LIMIT 20.5
#define MYRSKY_LOWER_LIMIT 20.5
#define MYRSKY_UPPER_LIMIT 32.5
#define HIRMUMYRSKY_LOWER_LIMIT 32.5

  enum wind_speed_id
	{
	  TYYNI,
	  HEIKKO,       // 0.5...3.5
	  KOHTALAINEN,  // 3.5...7.5
	  NAVAKKA,      // 7.5...13.5
	  KOVA,         // 13.5...20.5
	  MYRSKY,       // 20.5...32.5
	  HIRMUMYRSKY   // 32.5...
	};

  enum wind_direction_id
	{
	  POHJOINEN = 0x1,
	  POHJOINEN_KOILLINEN,
	  KOILLINEN,
	  ITA_KOILLINEN,
	  ITA,
	  ITA_KAAKKO,
	  KAAKKO,
	  ETELA_KAAKKO,
	  ETELA,
	  ETELA_LOUNAS,
	  LOUNAS,
	  LANSI_LOUNAS,
	  LANSI,
	  LANSI_LUODE,
	  LUODE,
	  POHJOINEN_LUODE,
	  POHJOISEN_PUOLEINEN,
	  KOILLISEN_PUOLEINEN,
	  IDAN_PUOLEINEN,
	  KAAKON_PUOLEINEN,
	  ETELAN_PUOLEINEN,
	  LOUNAAN_PUOLEINEN,
	  LANNEN_PUOLEINEN,
	  LUOTEEN_PUOLEINEN,
	  VAIHTELEVA,
	  MISSING_WIND_DIRECTION_ID
	};

  enum wind_event_id 
	{
	  TUULI_HEIKKENEE = 0x1,
	  TUULI_VOIMISTUU = 0x2,
	  TUULI_TYYNTYY = 0x4,
	  TUULI_KAANTYY = 0x8,
	  TUULI_MUUTTUU_VAIHTELEVAKSI = 0x10,
	  TUULI_KAANTYY_JA_HEIKKENEE = 0x9,
	  TUULI_KAANTYY_JA_VOIMISTUU = 0xA,
	  TUULI_KAANTYY_JA_TYYNTYY = 0xC,
	  TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE = 0x11,
	  TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU = 0x12,
	  TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY = 0x14,
	  MISSING_WIND_EVENT = 0x0,
	  MISSING_WIND_SPEED_EVENT = -0x1,
	  MISSING_WIND_DIRECTION_EVENT = -0x2
	};
  
  enum change_type
	{
	  INCREASING,
	  DECREASING,
	  AS_BEFORE
	};

  class WindDataItemUnit;
  class WindDataItemContainer;
  class WindSpeedPeriodDataItem;
  class WindDirectionPeriodDataItem;
  class WindEventPeriodDataItem;

  typedef vector<WindDataItemContainer*> wind_data_item_vector;
  typedef vector<WindSpeedPeriodDataItem*> wind_speed_period_data_item_vector;
  typedef vector<WindDirectionPeriodDataItem*> wind_direction_period_data_item_vector;
  typedef vector<WindEventPeriodDataItem*> wind_event_period_data_item_vector;
  typedef std::pair<NFmiTime, wind_event_id> timestamp_wind_event_id_pair;
  typedef vector<timestamp_wind_event_id_pair> wind_event_id_vector;

  struct wo_story_params
  {
	wo_story_params(const string& var,
					const string& areaName,
					const WeatherArea& area,
					const WeatherPeriod& dataPeriod,
					const WeatherPeriod& forecastPeriod,
					const NFmiTime& forecastTime,
					const AnalysisSources& sources,
					MessageLogger& log) :
	  theVar(var),
	  theAreaName(areaName),
	  theArea(area),
	  theDataPeriod(dataPeriod),
	  theForecastPeriod(forecastPeriod),
	  theForecastTime(forecastTime),
	  theSources(sources),
	  theLog(log)
	  
	{}

	const string& theVar;
	const string& theAreaName;
	const WeatherArea& theArea;
	const WeatherPeriod& theDataPeriod;
	const WeatherPeriod& theForecastPeriod;
	const NFmiTime& theForecastTime;
	const AnalysisSources& theSources;
	MessageLogger& theLog;

	double theMaxErrorWindSpeed;
	double theMaxErrorWindDirection;
	double theWindSpeedThreshold;
	double theWindDirectionThreshold;
	string theRangeSeparator;
	int theMinInterval;
	string theMetersPerSecondFormat;

	wind_data_item_vector theRawDataVector;
	wind_data_item_vector theEqualizedDataVector;
	wind_event_id_vector theWindEventVector;
	
	wind_speed_period_data_item_vector theWindSpeedVector;
	wind_direction_period_data_item_vector theWindDirectionVector;
	wind_event_period_data_item_vector theWindEventPeriodVector;
	vector<unsigned int> theOriginalWindSpeedIndexes;
	vector<unsigned int> theEqualizedWindSpeedIndexesForMedianWind;
	vector<unsigned int> theEqualizedWindSpeedIndexesForMaximumWind;
	vector<unsigned int> theOriginalWindDirectionIndexes;
	vector<unsigned int> theEqualizedWindDirectionIndexes;
	vector<pair<string, WeatherArea> > theNamedWeatherAreas;
  };

  struct WindDataItemUnit
  {
	WindDataItemUnit(const WeatherPeriod& period, 
					 const WeatherResult& windSpeedMin, 
					 const WeatherResult& windSpeedMax, 
					 const WeatherResult& windSpeedMean,
					 const WeatherResult& windSpeedMedian,
					 const WeatherResult& windMaximum,
					 const WeatherResult& windDirection,
					 const WeatherResult& gustSpeed)
	  : thePeriod(period),
		theWindSpeedMin(windSpeedMin),
		theWindSpeedMax(windSpeedMax),
		theWindSpeedMean(windSpeedMean),
		theWindSpeedMedian(windSpeedMedian),
		theWindMaximum(windMaximum),
		theWindDirection(windDirection),
		theGustSpeed(gustSpeed),
		theEqualizedMedianWindSpeed(windSpeedMedian.value()),
		theEqualizedMaximumWind(windMaximum.value()),
		theEqualizedWindDirection(theWindDirection.value())
	{}

	const float getWindSpeedShare(const float& theLowerLimit, const float& theUpperLimit) const;
	
	WeatherPeriod thePeriod;
	WeatherResult theWindSpeedMin;
	WeatherResult theWindSpeedMax;
	WeatherResult theWindSpeedMean;
	WeatherResult theWindSpeedMedian;
	WeatherResult theWindMaximum;
	WeatherResult theWindDirection;
	WeatherResult theGustSpeed;
	float theEqualizedMedianWindSpeed;
	float theEqualizedMaximumWind;
	float theEqualizedWindDirection;
	change_type theWindSpeedChangeType;
	change_type theWindDirectionChangeType;
	vector <pair<float, WeatherResult> > theWindSpeedDistribution;
  };

  struct WindDataItemContainer
  {
	WindDataItemContainer() : previousDataItem(0)
	{}
	
	~WindDataItemContainer()
	{
	  map<string, WindDataItemUnit*>::iterator it;
	  for (it=theDataItems.begin() ; it != theDataItems.end(); it++)
		delete it->second;
	}
  
	void addItem(const WeatherPeriod& period, 
				 const WeatherResult& windSpeedMin, 
				 const WeatherResult& windSpeedMax, 
				 const WeatherResult& windSpeedMean,
				 const WeatherResult& windSpeedMedian,
				 const WeatherResult& windMaximum,
				 const WeatherResult& windDirection,
				 const WeatherResult& gustSpeed,
				 const string& name)
	{
	  WindDataItemUnit* dataItem = new WindDataItemUnit(period,
														windSpeedMin,
														windSpeedMax,	
														windSpeedMean,
														windSpeedMedian,
														windMaximum,
														windDirection,
														gustSpeed);
	  theDataItems.insert(make_pair(name, dataItem));

	  if(previousDataItem)
		{
		  int previousWindSpeed = static_cast<int>(round(previousDataItem->theWindSpeedMean.value()));
		  int currentWindSpeed = static_cast<int>(round(dataItem->theWindSpeedMean.value()));
		  int previousDirection = static_cast<int>(round(previousDataItem->theWindDirection.value()));
		  int currentDirection = static_cast<int>(round(dataItem->theWindDirection.value()));
		  if(currentWindSpeed > previousWindSpeed)
			dataItem->theWindSpeedChangeType = INCREASING;
		  else if(currentWindSpeed < previousWindSpeed)
			dataItem->theWindSpeedChangeType = DECREASING;
		  else
			dataItem->theWindSpeedChangeType = AS_BEFORE;

		  if(abs(currentDirection-previousDirection < 180))
			{
			  if(currentDirection > previousDirection)
				dataItem->theWindDirectionChangeType = INCREASING;
			  if(currentDirection < previousDirection)
				dataItem->theWindDirectionChangeType = DECREASING;
			  else
				dataItem->theWindDirectionChangeType = AS_BEFORE;
			}
		  else
			{
			  if(currentDirection > previousDirection)
				dataItem->theWindDirectionChangeType = DECREASING;
			  else
				dataItem->theWindDirectionChangeType = INCREASING;
			}
		}

	  previousDataItem = dataItem;
	}

	const WindDataItemUnit& operator()(const string& name = "full") const
	{
	  return *(theDataItems.find(name)->second);
	}

	WindDataItemUnit& getDataItem(const string& name = "full") const
	{
	  return *(theDataItems.find(name)->second);
	}

	/*
	WindDataItemUnit& operator()(const string& name = "full")
	{
	  return *(theDataItems.find(name)->second);
	}
	*/

  private:
	map<string, WindDataItemUnit*> theDataItems;
	WindDataItemUnit* previousDataItem;
  };

  struct WindSpeedPeriodDataItem
  {
	WindSpeedPeriodDataItem(const WeatherPeriod& period,
							const wind_speed_id& windSpeedId)
	  : thePeriod(period),
		theWindSpeedId(windSpeedId)
	{}
	WeatherPeriod thePeriod;
	wind_speed_id theWindSpeedId;
  };

  struct WindDirectionPeriodDataItem
  {
	WindDirectionPeriodDataItem(const WeatherPeriod& period,
								  const wind_direction_id& windDirection)
	  : thePeriod(period),
		theWindDirection(windDirection)
	{}
	WeatherPeriod thePeriod;
	wind_direction_id theWindDirection;
  };

  struct WindEventPeriodDataItem
  {
	WindEventPeriodDataItem(const WeatherPeriod& period,
							const wind_event_id& windEvent,
							const WindDataItemUnit& periodBeginDataItem,
							const WindDataItemUnit& periodEndDataItem)
	  : thePeriod(period),
		theWindEvent(windEvent),
		thePeriodBeginDataItem(periodBeginDataItem),
		thePeriodEndDataItem(periodEndDataItem),
		theConcurrentEventPeriodItem(0),
		theTransientFlag(false),
		theReportThisEventPeriodFlag(true)
	{}
	WeatherPeriod thePeriod;
	wind_event_id theWindEvent;
	const WindDataItemUnit& thePeriodBeginDataItem;
	const WindDataItemUnit& thePeriodEndDataItem;
	WindEventPeriodDataItem* theConcurrentEventPeriodItem;
	bool theTransientFlag; // direction change can be temporary
	bool theReportThisEventPeriodFlag; // determines weather this event period is reported or not
  };


  class WindForecast
  {
  public:

	WindForecast(wo_story_params& parameters);

	~WindForecast(){}
		
	Sentence windSentence(const WeatherPeriod& thePeriod) const;
	Paragraph windForecastBasedOnEvents(const WeatherPeriod& thePeriod) const;
	Paragraph windForecastBasedOnEventPeriods(const WeatherPeriod& thePeriod) const;
 
	void printOutWindPeriods(std::ostream& theOutput) const;
	void printOutWindData(std::ostream& theOutput) const;

  private:
	

	wo_story_params& theParameters;
	mutable int thePreviousRangeBeg;
	mutable int thePreviousRangeEnd;

	void getRepresentativeInterval(const float& theDistributionSum, 
								   const WeatherPeriod& thePeriod,
								   float& theLowerLimit,
								   float& theUpperLimit);
	wind_direction_id findWindDirectionId(const NFmiTime& timestamp) const;
	const Sentence windDirectionSentence(const wind_direction_id& theDirectionId) const;
	const Sentence windSpeedSentence(const WeatherPeriod& thePeriod, bool theLastSentenceFlag = true) const;
	const Sentence speedRangeSentence(const WeatherPeriod& thePeriod,
									  const WeatherResult & theMaxSpeed,
									  const WeatherResult & theMeanSpeed,
									  const string & theVariable, 
									  bool theLastSentenceFlag) const;
	Sentence getWindSentence(const wind_event_id& speedEventId, 
							 const WeatherPeriod& speedEventPeriod,
							 const wind_event_id& directionEventId, 
							 const WeatherPeriod& directionEventPeriod,
							 const wind_direction_id& directionIdEnd,
							 const bool& firstSentenceInTheStory) const;
	//	Sentence getSpeedInterval(const WeatherPeriod& theWindSpeedFullPeriod) const;
	Sentence getWindSpeedDecreasingIncreasingInterval(const WeatherPeriod& speedEventPeriod,
														   const bool& firstSentenceInTheStory) const;	  
	const Sentence windSpeedDirectionSentence(const WindEventPeriodDataItem* theWindSpeedEventPeriod,
												 const WindEventPeriodDataItem* theWindDirectionEventPeriod) const;
	Sentence getTimePhrase(const WeatherPeriod thePeriod,
						   const bool& alkaenPhrase = true) const;


	wind_direction_id getWindDirectionId(const WeatherPeriod& thePeriod,
										 const CompassType& theComapssType) const;
	wind_direction_id getWindDirectionId(const WeatherPeriod& thePeriod) const;
	  

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
  
  wind_speed_id get_wind_speed_id(const WeatherResult& windSpeed);
  wind_direction_id get_wind_direction_id(const WeatherResult& windDirection, const string& var);
  std::string get_wind_speed_string(const wind_speed_id& theWindSpeedId);
  std::string get_wind_direction_string(const wind_direction_id& theWindDirectionId);
  std::string get_wind_event_string(const wind_event_id& theWindEventId);
  void populate_windspeed_distribution_time_series(const AnalysisSources& theSources,
												   const WeatherArea& theArea,
												   const WeatherPeriod& thePeriod,
												   const string& theVar,
												   vector <pair<float, WeatherResult> >& theWindSpeedDistribution);
} // namespace TextGen

#endif // TEXTGEN_WIND_FORECAST_H
