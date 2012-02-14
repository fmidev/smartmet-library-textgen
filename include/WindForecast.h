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

#define USE_AT_ITS_STRONGEST_PHRASE true
#define DONT_USE_AT_ITS_STRONGEST_PHRASE false
#define USE_ALKAEN_PHRASE true
#define DONT_USE_ALKAEN_PHRASE false


#define FULL_AREA_NAME "full"
#define NORTHERN_AREA_NAME "north"
#define SOUTHERN_AREA_NAME "south"
#define EASTERN_AREA_NAME "east"
#define WESTERN_AREA_NAME "west"

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

  enum wind_event_type
	{
	  WIND_DIRECTION_EVENT,
	  WIND_SPEED_EVENT,
	  MISSING_EVENT_TYPE
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

  struct index_vectors
  {
	// contains all indexes to 
	vector<unsigned int> theOriginalWindDataIndexes;
	vector<unsigned int> theEqualizedWindSpeedIndexesForMedianWind;
	vector<unsigned int> theEqualizedWindSpeedIndexesForMaximumWind;
	vector<unsigned int> theEqualizedWindDirectionIndexes;
  };

  struct wo_story_params
  {
	wo_story_params(const string& var,
					const string& areaName,
					const WeatherArea& area,
					const WeatherPeriod& forecastPeriod,
					const NFmiTime& forecastTime,
					const AnalysisSources& sources,
					MessageLogger& log) :
	  theVar(var),
	  theAreaName(areaName),
	  theArea(area),
	  theForecastPeriod(forecastPeriod),
	  theForecastTime(forecastTime),
	  theSources(sources),
	  theDataPeriod(forecastPeriod),
	  theLog(log),
	  theSplitMethod(NO_SPLITTING),
	  theAreaPart(FULL_AREA_NAME)	  	  
	{}

	const string& theVar;
	const string& theAreaName;
	const WeatherArea& theArea;
	const WeatherPeriod& theForecastPeriod;
	const NFmiTime& theForecastTime;
	const AnalysisSources& theSources;
	WeatherPeriod theDataPeriod; // currently same as forecast period, but could be longer in both ends
	MessageLogger& theLog;
	split_method theSplitMethod;
	string theAreaPart;

	double theMaxErrorWindSpeed;
	double theMaxErrorWindDirection;
	double theWindSpeedThreshold;
	double windSpeedReportingThreshold;
	double theWindDirectionThreshold;
	string theRangeSeparator;
	int theMinIntervalSize;
	int theMaxIntervalSize;
	string theMetersPerSecondFormat;
	bool theAlkaenPhraseUsed;

	// contains raw data
	wind_data_item_vector theWindDataVector; 
	//	wind_data_item_vector theEqualizedDataVector;
	// contains wind events (direction changes, wind speed increases/decreases)
	//	wind_event_id_vector theWindEventVector; 
	
	wind_speed_period_data_item_vector theWindSpeedVector;
	wind_direction_period_data_item_vector theWindDirectionVector;
	wind_event_period_data_item_vector theWindEventPeriodVector;
	wind_event_period_data_item_vector theWindSpeedEventPeriodVector;
	wind_event_period_data_item_vector theWindDirectionEventPeriodVector;
	wind_event_period_data_item_vector theMergedWindEventPeriodVector;

	map<string, index_vectors*> indexes;
	inline vector<unsigned int>& originalWindDataIndexes(const string& areaPart)
	{ 
	  return indexes[areaPart]->theOriginalWindDataIndexes; 
	}
	inline vector<unsigned int>& equalizedWSIndexesMedian(const string& areaPart)
	{ 
	  return indexes[areaPart]->theEqualizedWindSpeedIndexesForMedianWind;
	}
	inline vector<unsigned int>& equalizedWSIndexesMaximum(const string& areaPart)
	{ 
	  return indexes[areaPart]->theEqualizedWindSpeedIndexesForMaximumWind;
	}
	inline vector<unsigned int>& equalizedWDIndexes(const string& areaPart)
	{ 
	  return indexes[areaPart]->theEqualizedWindDirectionIndexes;
	}

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
		theEqualizedWindDirection(theWindDirection)
	{}

	const float getWindSpeedShare(const float& theLowerLimit, const float& theUpperLimit) const;
    bool operator==(const WindDataItemUnit& dataItemUnit) const
	{
	  return thePeriod == dataItemUnit.thePeriod;
	}
	
	
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
	WeatherResult theEqualizedWindDirection;
	vector <pair<float, WeatherResult> > theWindSpeedDistribution;
  };

  // contains WindDataItemUnit structs for different areas (coastal, inland, full area)
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

	  previousDataItem = dataItem;
	}

	const WindDataItemUnit& operator()(const string& name = FULL_AREA_NAME) const
	{
	  return *(theDataItems.find(name)->second);
	}

	WindDataItemUnit& getDataItem(const string& name = FULL_AREA_NAME) const
	{
	  return *(theDataItems.find(name)->second);
	}


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
		theTransientFlag(false),
		theReportThisEventPeriodFlag(true)
	{}

	WeatherPeriod thePeriod;
	wind_event_id theWindEvent;
	const WindDataItemUnit& thePeriodBeginDataItem;
	const WindDataItemUnit& thePeriodEndDataItem;
	bool theTransientFlag; // direction change can be temporary
	bool theReportThisEventPeriodFlag; // determines weather this event period is reported or not

	
	wind_event_type getWindEventType()
	{
	  if(theWindEvent == MISSING_WIND_EVENT)
		return MISSING_EVENT_TYPE;
	  else
		return ((theWindEvent == TUULI_KAANTYY || 
				 theWindEvent == MISSING_WIND_DIRECTION_EVENT ||
				 theWindEvent == TUULI_MUUTTUU_VAIHTELEVAKSI) ? WIND_DIRECTION_EVENT : WIND_SPEED_EVENT);
	}
  };


  class WindForecast
  {
  public:

	WindForecast(wo_story_params& parameters);

	~WindForecast(){}
		
	Paragraph getWindStory(const WeatherPeriod& thePeriod) const;
 
  private:
	

	wo_story_params& theParameters;
	mutable int thePreviousRangeBeg;
	mutable int thePreviousRangeEnd;
	mutable short thePreviousDayNumber;

	//	Sentence windSentence(const WeatherPeriod& thePeriod) const;
	Sentence getWindDirectionSentence(const wind_direction_id& theWindDirectionId) const;
	Sentence windDirectionSentence(const wind_direction_id& theDirectionId,
								   const bool& theBasicForm = false) const;
	Sentence windSpeedIntervalSentence(const WeatherPeriod& thePeriod, 
									   bool theUseAtItsStrongestPhrase = true) const;
	Sentence speedRangeSentence(const WeatherPeriod& thePeriod,
								const WeatherResult & theMaxSpeed,
								const WeatherResult & theMeanSpeed,
								const string & theVariable, 
								bool theUseAtItsStrongestPhrase) const;

	//	Sentence getSpeedInterval(const WeatherPeriod& theWindSpeedFullPeriod) const;
	Sentence decreasingIncreasingInterval(const WindEventPeriodDataItem& eventPeriodDataItem,
										  const bool& firstSentenceInTheStory,
										  const bool& startsToWeakenStrengthenPhraseUsed,
										  const wind_event_id& eventId) const;	  
	Sentence getTimePhrase(const WeatherPeriod thePeriod,
						   const bool& useAlkaenPhrase) const;

	wind_direction_id getWindDirectionId(const WeatherPeriod& thePeriod,
										 const CompassType& theComapssType) const;
	wind_direction_id getWindDirectionId(const WeatherPeriod& thePeriod) const;
	NFmiTime getWindDirectionTurningTime(const WeatherPeriod& period) const;
	NFmiTime getWindDirectionTurningTime(const WeatherPeriod& period,
										 const wind_direction_id& directionId) const;

	vector<WeatherPeriod> getWindSpeedReportingPoints(const WindEventPeriodDataItem& eventPeriodDataItem,
													  const bool& firstSentenceInTheStory,
													  const wind_event_id& eventId) const;
	bool getSpeedIntervalLimits(const WeatherPeriod& thePeriod, 
								WeatherResult& lowerLimit,
								WeatherResult& upperLimit) const;
	bool getWindSpeedChangeAttribute(const WeatherPeriod& changePeriod,
									 std::string& phraseStr,
									 bool& smallChange,
									 bool& gradualChange,
									 bool& fastChange) const;
	std::string getWindDirectionTurntoString(const wind_direction_id& theWindDirectionId) const;
	int getLastSentenceIndex() const;
	Sentence reportIntermediateSpeed(const WeatherPeriod& speedEventPeriod) const;
	/*
	Sentence windSpeedIntervalSentence2(const WeatherPeriod& speedEventPeriod,
										bool theUseAtItsStrongestPhrase,
										bool theFirstSentenceInTheStory) const;
	*/
	bool samePartOfTheDay(const NFmiTime& time1, const NFmiTime& time2) const;
	Sentence directedWindSentenceAfterVaryingWind(const wo_story_params& theParameters,
												  const WeatherPeriod& eventPeriod,
												  const bool& firstSentenceInTheStory) const;
	Sentence windDirectionChangesSentence(const wo_story_params& theParameters,
										  const WeatherPeriod& eventPeriod,
										  const bool& firstSentenceInTheStory,
										  const wind_direction_id& windDirectionIdPrevious) const;

	Sentence windDirectionAndSpeedChangesSentence(const wo_story_params& theParameters,
												  const WeatherPeriod& eventPeriod,
												  const bool& firstSentenceInTheStory,
												  const wind_event_id& previousWindEventId,
												  const wind_event_id& currentWindEventId,
												  const wind_direction_id& previousWindDirectionId,
												  bool& useAlkaaHeiketaVoimistuaPhrase) const;
	void getWindDirectionBegEnd(const WeatherPeriod& thePeriod,
								wind_direction_id& theWindDirectionIdBeg,
								wind_direction_id& theWindDirectionIdEnd) const;
	bool windDirectionTurns(const WeatherPeriod& thePeriod) const;
	void getPeriodStartAndEndIndex(const WeatherPeriod& period,
								   unsigned int& begIndex,
								   unsigned int& endIndex) const;
	  
  };

  
  wind_speed_id get_wind_speed_id(const WeatherResult& windSpeed);
  wind_speed_id get_wind_speed_id(const float& windSpeed);
  wind_direction_id get_wind_direction_id(const WeatherResult& windDirection, const string& var);
  std::string get_wind_speed_string(const wind_speed_id& theWindSpeedId);
  std::string get_wind_direction_string(const wind_direction_id& theWindDirectionId);
  std::string get_wind_event_string(const wind_event_id& theWindEventId);
  void populate_windspeed_distribution_time_series(const AnalysisSources& theSources,
												   const WeatherArea& theArea,
												   const WeatherPeriod& thePeriod,
												   const string& theVar,
												   vector <pair<float, WeatherResult> >& theWindSpeedDistribution);
  /*
  void get_wind_speed_interval_parameters(const WeatherPeriod& period,
										  const wind_data_item_vector& windDataVector,
										  const string& areaPart,
										  float& begLowerLimit,
										  float& begUpperLimit,
										  float& endLowerLimit,
										  float& endUpperLimit,
										  float& changeRatePerHour);
  bool wind_speed_differ_enough(const WeatherPeriod& weatherPeriod, 
								const wind_data_item_vector& windDataVector,
								const string& areaPart);
  */
  wind_direction_id get_wind_direction_at(const wo_story_params& theParameters,
										  const NFmiTime& pointOfTime,
										  const string& var);
  wind_direction_id get_wind_direction_at(const wo_story_params& theParameters,
										  const WeatherPeriod& period,
										  const string& var);
  bool wind_speed_differ_enough(const AnalysisSources& theSources,
								const WeatherArea& theArea,
								const WeatherPeriod& thePeriod,
								const string& theAreaPart,
								const string& theVar,
								const wind_data_item_vector& windDataVector);
  void get_wind_speed_interval_parameters(const AnalysisSources& theSources,
										  const WeatherArea& theArea,
										  const WeatherPeriod& thePeriod,
										  const string& theAreaPart,
										  const string& theVar,
										  const wind_data_item_vector& windDataVector,
										  int& begLowerLimit,
										  int& begUpperLimit,
										  int& endLowerLimit,
										  int& endUpperLimit,
										  float& changeRatePerHour);
  void get_wind_speed_interval(const AnalysisSources& theSources,
							   const WeatherArea& theArea,
							   const NFmiTime& pointOfTime,
							   const string& theAreaPart,
							   const string& theVar,
							   const wind_data_item_vector& windDataVector,
							   int& lowerLimit,
							   int& upperLimit);

  void get_wind_speed_interval(const AnalysisSources& theSources,
							   const WeatherArea& theArea,
							   const WeatherPeriod& thePeriod,
							   const string& theAreaPart,
							   const string& theVar,
							   const wind_data_item_vector& windDataVector,
							   int& lowerLimit,
							   int& upperLimit);
  float direction_difference(const float& direction1, 
							 const float& direction2);
  bool ascending_order(const float& direction1, 
					   const float& direction2);
  bool wind_turns_to_the_same_direction(const float& direction1, 
										const float& direction2,  
										const float& direction3);
} // namespace TextGen

#endif // TEXTGEN_WIND_FORECAST_H
