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
	  POHJOINEN = 1,
	  POHJOINEN_KOILLINEN,
	  KOILLINEN,
	  KOILLINEN_ITA,
	  ITA,
	  ITA_KAAKKO,
	  KAAKKO,
	  KAAKKO_ETELA,
	  ETELA,
	  ETELA_LOUNAS,
	  LOUNAS,
	  LOUNAS_LANSI,
	  LANSI,
	  LANSI_LUODE,
	  LUODE,
	  LUODE_POHJOINEN,
	  POHJOINEN_PUOLEINEN,
	  KOILLINEN_PUOLEINEN,
	  ITA_PUOLEINEN,
	  KAAKKO_PUOLEINEN,
	  ETELA_PUOLEINEN,
	  LOUNAS_PUOLEINEN,
	  LANSI_PUOLEINEN,
	  LUODE_PUOLEINEN,
	  VAIHTELEVA
	};

  enum wind_event_id 
	{
	  TUULI_HEIKKENEE = 0x1,
	  TUULI_VOIMISTUU = 0x2,
	  TUULI_TYYNTYY = 0x4,
	  TUULI_KAANTYY = 0x8,
	  TUULI_MUUTTUU_VAIHTELEVAKSI = 0x10,
	  TUULI_KAANTYY_JA_HEIKEKNEE = 0x9,
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
	double theWindDirectionTreshold;

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
					 const WeatherResult& windMaximumMedian,
					 const WeatherResult& windDirection,
					 const WeatherResult& gustSpeed)
	  : thePeriod(period),
		theWindSpeedMin(windSpeedMin),
		theWindSpeedMax(windSpeedMax),
		theWindSpeedMean(windSpeedMean),
		theWindSpeedMedian(windSpeedMedian),
		theWindMaximum(windMaximum),
		theWindMaximumMedian(windMaximumMedian),
		theWindDirection(windDirection),
		theGustSpeed(gustSpeed),
		theEqualizedMedianWindSpeed(windSpeedMedian.value()),
		theEqualizedWindDirection(theWindDirection.value())
	{}
	
	WeatherPeriod thePeriod;
	WeatherResult theWindSpeedMin;
	WeatherResult theWindSpeedMax;
	WeatherResult theWindSpeedMean;
	WeatherResult theWindSpeedMedian;
	WeatherResult theWindMaximum;
	WeatherResult theWindMaximumMedian;
	WeatherResult theWindDirection;
	WeatherResult theGustSpeed;
	float theEqualizedMedianWindSpeed;
	float theEqualizedMaximumWind;
	float theEqualizedWindDirection;
	change_type theWindSpeedChangeType;
	change_type theWindDirectionChangeType;
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
				 const WeatherResult& windMaximumMedian,
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
														windMaximumMedian,
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
		thePeriodEndDataItem(periodEndDataItem)
	{}
	WeatherPeriod thePeriod;
	wind_event_id theWindEvent;
	const WindDataItemUnit& thePeriodBeginDataItem;
	const WindDataItemUnit& thePeriodEndDataItem;
  };


  class WindForecast
  {
  public:

	WindForecast(wo_story_params& parameters);

	~WindForecast(){}
		
	Sentence windSentence(const WeatherPeriod& thePeriod) const;
	Paragraph windForecastBasedOnEvents(const WeatherPeriod& thePeriod) const;
 
	void printOutWindPeriods(std::ostream& theOutput) const;
	void printOutWindData(std::ostream& theOutput) const;

  private:
	

	wo_story_params& theParameters;
	mutable int thePreviousRangeBeg;
	mutable int thePreviousRangeEnd;

	wind_direction_id get_wind_direction_id(const NFmiTime& timestamp) const;
	const Sentence direction_sentence(const wind_direction_id& theDirectionId) const;
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
