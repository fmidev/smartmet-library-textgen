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

#define USE_AT_ITS_STRONGEST_PHRASE true
#define DONT_USE_AT_ITS_STRONGEST_PHRASE false
#define USE_ALKAEN_PHRASE true
#define DONT_USE_ALKAEN_PHRASE false

#define TUULI_WORD "tuuli"
#define VAHAN_WORD "vahan"
#define NOPEASTI_WORD "nopeasti"
#define VAHITELLEN_WORD "vahitellen"
#define KOVIMMILLAAN_PHRASE "kovimmillaan"
#define HEIKKENEVAA_WORD "heikkenevaa"
#define VOIMISTUVAA_WORD "voimistuvaa"
#define TUULTA_WORD "tuulta"
#define TUULI_TYYNTYY_PHRASE "tuuli tyyntyy"
#define VOIMISTUVAA_TUULTA_PHRASE "voimistuvaa tuulta"
#define HEIKKENEVAA_TUULTA_PHRASE "heikkenevaa tuulta"
#define POHJOISEEN_PHRASE "pohjoiseen"
#define ETELAAN_PHRASE "etelaan"
#define ITAAN_PHRASE "itaan"
#define LANTEEN_PHRASE "lanteen"
#define KOILLISEEN_PHRASE "koilliseen"
#define KAAKKOON_PHRASE "kaakkoon"
#define LOUNAASEEN_PHRASE "lounaaseen"
#define LUOTEESEEN_PHRASE "luoteeseen"
#define POHJOISEN_PUOLELLE_PHRASE "pohjoisen puolelle"
#define ETELAN_PUOLELLE_PHRASE "etelan puolelle"
#define IDAN_PUOLELLE_PHRASE "idan puolelle"
#define LANNEN_PUOLELLE_PHRASE "lannen puolelle"
#define KOILLISEN_PUOLELLE_PHRASE "koillisen puolelle"
#define KAAKON_PUOLELLE_PHRASE "kaakon puolelle"
#define LOUNAAN_PUOLELLE_PHRASE "lounaan puolelle"
#define LUOTEEN_PUOLELLE_PHRASE "luoteen puolelle"
#define POHJOISEN_JA_KOILLISEN_VALILLE_PHRASE "pohjoisen ja koillisen valille"
#define IDAN_JA_KOILLISEN_VALILLE_PHRASE "idan ja koillisen valille"
#define IDAN_JA_KAAKON_VALILLE_PHRASE "idan ja kaakon valille"
#define ETELAN_JA_KAAKON_VALILLE_PHRASE "etelan ja kaakon valille"
#define ETELAN_JA_LOUNAAN_VALILLE_PHRASE "etelan ja lounaan valille"
#define LANNEN_JA_LOUNAAN_VALILLE_PHRASE "lannen ja lounaan valille"
#define LANNEN_JA_LUOTEEN_VALILLE_PHRASE "lannen ja luoteen valille"
#define POHJOISEN_JA_LUOTEEN_VALILLE_PHRASE "pohjoisen ja luoteen valille"
#define TUULI_MUUTTUU_VAIHTELEVAKSI_PHRASE "tuuli muuttuu vaihtelevaksi"
#define TUULI_KAANTYY_PHRASE "tuuli kaantyy"

#define POHJOINEN_TUULI_G "1-tuulta"
#define POHJOINEN_TUULI "1-tuuli"
#define POHJOISEN_PUOLEINEN_TUULI_G "1-puoleista tuulta"
#define POHJOISEN_PUOLEINEN_TUULI "1-puoleinen tuuli"
#define POHJOINEN_KOILLINEN_TUULI_G "1- ja 2-valista tuulta"
#define POHJOINEN_KOILLINEN_TUULI "1- ja 2-valinen tuuli"
#define KOILLINEN_TUULI_G "2-tuulta"
#define KOILLINEN_TUULI "2-tuuli"
#define KOILLISEN_PUOLEINEN_TUULI_G "2-puoleista tuulta"
#define KOILLISEN_PUOLEINEN_TUULI "2-puoleinen tuuli"
#define ITA_KOILLINEN_TUULI_G "3- ja 2-valista tuulta"
#define ITA_KOILLINEN_TUULI "3- ja 2-valinen tuuli"
#define ITA_TUULI_G "3-tuulta"
#define ITA_TUULI "3-tuuli"
#define IDAN_PUOLEINEN_TUULI_G "3-puoleista tuulta"
#define IDAN_PUOLEINEN_TUULI "3-puoleinen tuuli"
#define ITA_KAAKKO_TUULI_G "3- ja 4-valista tuulta"
#define ITA_KAAKKO_TUULI "3- ja 4-valinen tuuli"
#define KAAKKO_TUULI_G "4-tuulta"
#define KAAKKO_TUULI "4-tuuli"
#define KAAKON_PUOLEINEN_TUULI_G "4-puoleista tuulta"
#define KAAKON_PUOLEINEN_TUULI "4-puoleinen tuuli"
#define ETELA_KAAKKO_TUULI_G "5- ja 4-valista tuulta"
#define ETELA_KAAKKO_TUULI "5- ja 4-valinen tuuli"
#define ETELA_TUULI_G "5-tuulta"
#define ETELA_TUULI "5-tuuli"
#define ETELAN_PUOLEINEN_TUULI_G "5-puoleista tuulta"
#define ETELAN_PUOLEINEN_TUULI "5-puoleinen tuuli"
#define ETELA_LOUNAS_TUULI_G "5- ja 6-valista tuulta"
#define ETELA_LOUNAS_TUULI "5- ja 6-valinen tuuli"
#define LOUNAS_TUULI_G "6-tuulta"
#define LOUNAS_TUULI "6-tuuli"
#define LOUNAAN_PUOLEINEN_TUULI_G "6-puoleista tuulta"
#define LOUNAAN_PUOLEINEN_TUULI "6-puoleinen tuuli"
#define LANSI_LOUNAS_TUULI_G "7- ja 6-valista tuulta"
#define LANSI_LOUNAS_TUULI "7- ja 6-valinen tuuli"
#define LANSI_TUULI_G "7-tuulta"
#define LANSI_TUULI "7-tuuli"
#define LANNEN_PUOLEINEN_TUULI_G "7-puoleista tuulta"
#define LANNEN_PUOLEINEN_TUULI "7-puoleinen tuuli"
#define LANSI_LUODE_TUULI_G "7- ja 8-valista tuulta"
#define LANSI_LUODE_TUULI "7- ja 8-valinen tuuli"
#define LUODE_TUULI_G "8-tuulta"
#define LUODE_TUULI "8-tuuli"
#define LUOTEEN_PUOLEINEN_TUULI_G "8-puoleista tuulta"
#define LUOTEEN_PUOLEINEN_TUULI "8-puoleinen tuuli"
#define POHJOINEN_LUODE_TUULI_G "1- ja 8-valista tuulta"
#define POHJOINEN_LUODE_TUULI "1- ja 8-valinen tuuli"
#define VAIHTELEVA_TUULI_G "suunnaltaan vaihtelevaa tuulta"
#define VAIHTELEVA_TUULI "suunnaltaan vaihteleva tuuli"



#define ILTAPAIVALLA_EDELLEEN_HEIKKENEVAA_POHJOISTUULTA "[iltapaivalla] edelleen [heikkenevaa] [pohjoistuulta]"
#define ILTAPAIVALLA_POHJOISTUULTA_JOKA_ALKAA_VOIMISTUA "[iltapaivalla] [pohjoistuulta], joka alkaa voimistua"
#define ILTAPAIVALLA_POHJOISTUULTA_JOKA_VOIMISTUU_EDELLEEN "[iltapaivalla] [pohjoistuulta], joka voimistuu edelleen"
#define ILTAPAIVALLA_POHJOISTUULTA_JOKA_ALKAA_HEIKETA "[iltapaivalla] [pohjoistuulta], joka alkaa heiketa"
#define ILTAPAIVALLA_POHJOISTUULTA_JOKA_HEIKKENEE_EDELLEEN "[iltapaivalla] [pohjoistuulta], joka heikkenee edelleen"
#define ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_JA_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE "[iltapaivalla] tuuli kaantyy [etelaan] ja voimistuu edelleen"
#define ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE "[iltapaivalla] tuuli kaantyy [etelaan] ja heikkenee edelleen"
#define ILTAPAIVALLA_TUULI_VOIMISTUU_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] tuuli voimistuu ja kaantyy [etelaan]"
#define ILTAPAIVALLA_POHJOISTUULI_VOIMISTUU_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] [pohjoistuuli] voimistuu ja kaantyy [etelaan]"
#define ILTAPAIVALLA_TUULI_HEIKKENEE_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] tuuli heikkenee ja kaantyy [etelaan]"
#define ILTAPAIVALLA_POHJOISTUULI_HEIKKENEE_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] [pohjoistuuli] heikkenee ja kaantyy [etelaan]"
#define ILTAPAIVALLA_TUULI_TYYNTYY_COMPOSITE_PHRASE "[iltapaivalla] tuuli tyyntyy"
  //#define ILTAPAIVALLA_TUULI_TYYNTYY_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] tuuli tyyntyy ja kaantyy [etelaan]"
#define ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE "[iltapaivalla] tuuli muuttuu vaihtelevaksi ja voimistuu edelleen"
#define ILTAPAIVALLA_TUULI_VOIMISTUU_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli voimistuu ja muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_POHJOISTUULI_VOIMISTUU_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] [pohjoistuuli] voimistuu ja muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE "[iltapaivalla] tuuli muuttuu vaihtelevaksi ja heikkenee edelleen"
#define ILTAPAIVALLA_TUULI_HEIKKENEE_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli heikkenee ja muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_POHJOISTUULI_HEIKKENEE_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] [pohjoistuuli] heikkenee ja muuttuu vaihtelevaksi"
  //#define ILTAPAIVALLA_TUULI_TYYNTYY_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli tyyntyy ja muuttuu vaihtelevaksi"
#define POHJOISTUULTA_INTERVALLI_MS_JOKA_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[pohjoistuulta] [m...n] [metria sekunnissa], joka kaantyy [etelaan]"
#define ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] tuuli kaantyy [etelaan]"
#define ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] tuuli kaantyy [etelaan]"
#define POHJOISTUULTA_INTERVALLI_MS_JOKA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[pohjoistuulta] [m...n] [metria sekunnissa], joka muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_ETELAAN_KAANTYVAA_TUULTA_COMPOSITE_PHRASE "[iltapaivalla] [etelaan] kaantyvaa tuulta"
#define ILTAPAIVALLA_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE "[iltapaivalla] [heikkenevaa] [etelatuulta]"
#define ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE "[iltapaivalla] [nopeasti] [heikkenevaa] [etelatuulta]"
#define ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_JOKA_KAANTYY_POHJOISEEN_COMPOSITE_PHRASE "[iltapaivalla] [nopeasti] [heikkenevaa] [etelatuulta], joka kaantyy [pohjoiseen]"
#define ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE "[iltapaivalla] [etelatuulta]"
#define ILTAPAIVALLA_HEIKKENEVAA_TUULTA_COMPOSITE_PHRASE "[iltapaivalla] [heikkenevaa tuulta]"
#define ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_TUULTA_COMPOSITE_PHRASE "[iltapaivalla] [nopeasti] [heikkenevaa tuulta]"

#define ILTAPAIVALLA_TUULI_VOIMISTUU_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli voimistuu ja muuttuu tilapaisesti vaihtelevaksi"
#define ILTAPAIVALLA_POHJOISTUULI_VOIMISTUU_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] [pohjoistuuli] voimistuu ja muuttuu tilapaisesti vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_JA_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE "[iltapaivalla] tuuli muuttuu tilapaisesti vaihtelevaksi ja voimistuu edelleen"
#define ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE "[iltapaivalla] tuuli muuttuu tilapaisesti vaihtelevaksi ja heikkenee edelleen"
#define ILTAPAIVALLA_POHJOISTUULI_HEIKKENEE_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] [pohjoistuuli] heikkenee ja muuttuu tilapaisesti vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_HEIKKENEE_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli heikkenee ja muuttuu tilapaisesti vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli muuttuu tilapaisesti vaihtelevaksi"

#define ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI "[iltapaivalla] [etelatuuli] alkaa heiketa [nopeasti]"
#define ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI "[iltapaivalla] [etelatuuli] alkaa voimistua [nopeasti]"
#define ILTAPAIVALLA_POHJOISTUULI_ALKAA_VOIMISTUA_JA_KAANTYA_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] [pohjoistuuli] alkaa voimistua ja kaantya [etelaan]"
#define ILTAPAIVALLA_POHJOISTUULI_ALKAA_HEIKETA_JA_KAANTYA_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] [pohjoistuuli] alkaa heiketa ja kaantya [etelaan]"

  enum WindEventId 
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
  class WindDataItemsByArea;
  class WindSpeedPeriodDataItem;
  class WindDirectionPeriodDataItem;
  class WindEventPeriodDataItem;

  typedef vector<WindDataItemsByArea*> wind_data_item_vector;
  typedef vector<WindSpeedPeriodDataItem*> wind_speed_period_data_item_vector;
  typedef vector<WindDirectionPeriodDataItem*> wind_direction_period_data_item_vector;
  typedef vector<WindEventPeriodDataItem*> wind_event_period_data_item_vector;
  typedef std::pair<NFmiTime, WindEventId> timestamp_wind_event_id_pair;
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
					const WeatherArea& area,
					const WeatherPeriod& forecastPeriod,
					const NFmiTime& forecastTime,
					const AnalysisSources& sources,
					MessageLogger& log) :
	  theVar(var),
	  theArea(area),
	  theForecastPeriod(forecastPeriod),
	  theForecastTime(forecastTime),
	  theSources(sources),
	  theDataPeriod(forecastPeriod),
	  theLog(log),
	  theSplitMethod(NO_SPLITTING)
	{}

	const string& theVar;
	const WeatherArea& theArea;
	const WeatherPeriod& theForecastPeriod;
	const NFmiTime& theForecastTime;
	const AnalysisSources& theSources;
	WeatherPeriod theDataPeriod; // currently same as forecast period, but could be longer in both ends
	MessageLogger& theLog;
	split_method theSplitMethod;

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

	map<WeatherArea::Type, index_vectors*> indexes;

	inline vector<unsigned int>& originalWindDataIndexes(WeatherArea::Type type)
	{ 
	  return indexes[type]->theOriginalWindDataIndexes; 
	}
	inline vector<unsigned int>& equalizedWSIndexesMedian(WeatherArea::Type type)
	{ 
	  return indexes[type]->theEqualizedWindSpeedIndexesForMedianWind;
	}
	inline vector<unsigned int>& equalizedWSIndexesMaximum(WeatherArea::Type type)
	{ 
	  return indexes[type]->theEqualizedWindSpeedIndexesForMaximumWind;
	}
	inline vector<unsigned int>& equalizedWDIndexes(WeatherArea::Type type)
	{ 
	  return indexes[type]->theEqualizedWindDirectionIndexes;
	}

	// If the area is split this contains e.g. inland coast, full, eastern, western areas
	vector<WeatherArea> theWeatherAreas;

	std::string areaName() { return (theArea.isNamed() ? theArea.name() : ""); }
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
  struct WindDataItemsByArea
  {
	WindDataItemsByArea()
	{}
	
	~WindDataItemsByArea()
	{
	  map<WeatherArea::Type, WindDataItemUnit*>::iterator it;
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
				 const WeatherArea::Type& type)
	{
	  WindDataItemUnit* dataItem = new WindDataItemUnit(period,
														windSpeedMin,
														windSpeedMax,	
														windSpeedMean,
														windSpeedMedian,
														windMaximum,
														windDirection,
														gustSpeed);
	  theDataItems.insert(make_pair(type, dataItem));
	}

	const WindDataItemUnit& operator()(const WeatherArea::Type& type = WeatherArea::Full) const
	{
	  return *(theDataItems.find(type)->second);
	}

	WindDataItemUnit& getDataItem(const WeatherArea::Type& type = WeatherArea::Full) const
	{
	  return *(theDataItems.find(type)->second);
	}

  private:

	map<WeatherArea::Type, WindDataItemUnit*> theDataItems;
  };

  struct WindSpeedPeriodDataItem
  {
	WindSpeedPeriodDataItem(const WeatherPeriod& period,
							const WindSpeedId& windSpeedId)
	  : thePeriod(period),
		theWindSpeedId(windSpeedId)
	{}
	WeatherPeriod thePeriod;
	WindSpeedId theWindSpeedId;
  };

  struct WindDirectionPeriodDataItem
  {
	WindDirectionPeriodDataItem(const WeatherPeriod& period,
								  const WindDirectionId& windDirection)
	  : thePeriod(period),
		theWindDirection(windDirection)
	{}
	WeatherPeriod thePeriod;
	WindDirectionId theWindDirection;
  };

  struct WindEventPeriodDataItem
  {
	WindEventPeriodDataItem(const WeatherPeriod& period,
							const WindEventId& windEvent,
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
	WindEventId theWindEvent;
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
	Sentence getWindDirectionSentence(const WindDirectionId& theWindDirectionId) const;
	Sentence windDirectionSentence(const WindDirectionId& theDirectionId,
								   const bool& theBasicForm = false) const;
	Sentence windSpeedIntervalSentence(const WeatherPeriod& thePeriod, 
									   bool theUseAtItsStrongestPhrase = true) const;
	Sentence speedRangeSentence(const WeatherPeriod& thePeriod,
								const WeatherResult & theMaxSpeed,
								const WeatherResult & theMeanSpeed,
								const string & theVariable, 
								bool theUseAtItsStrongestPhrase) const;

	Sentence decreasingIncreasingInterval(const WindEventPeriodDataItem& eventPeriodDataItem,
										  const bool& firstSentenceInTheStory,
										  const bool& startsToWeakenStrengthenPhraseUsed,
										  const WindEventId& eventId) const;	  
	Sentence getTimePhrase(const WeatherPeriod thePeriod,
						   const bool& useAlkaenPhrase) const;

	WindDirectionId getWindDirectionId(const WeatherPeriod& thePeriod,
										 const CompassType& theComapssType) const;
	WindDirectionId getWindDirectionId(const WeatherPeriod& thePeriod) const;
	NFmiTime getWindDirectionTurningTime(const WeatherPeriod& period) const;
	NFmiTime getWindDirectionTurningTime(const WeatherPeriod& period,
										 const WindDirectionId& directionId) const;

	vector<WeatherPeriod> getWindSpeedReportingPoints(const WindEventPeriodDataItem& eventPeriodDataItem,
													  const bool& firstSentenceInTheStory,
													  const WindEventId& eventId) const;
	bool getSpeedIntervalLimits(const WeatherPeriod& thePeriod, 
								WeatherResult& lowerLimit,
								WeatherResult& upperLimit) const;
	bool getWindSpeedChangeAttribute(const WeatherPeriod& changePeriod,
									 std::string& phraseStr,
									 bool& smallChange,
									 bool& gradualChange,
									 bool& fastChange) const;
	std::string getWindDirectionTurntoString(const WindDirectionId& theWindDirectionId) const;
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
										  const WindDirectionId& windDirectionIdPrevious) const;

	Sentence windDirectionAndSpeedChangesSentence(const wo_story_params& theParameters,
												  const WeatherPeriod& eventPeriod,
												  const bool& firstSentenceInTheStory,
												  const WindEventId& previousWindEventId,
												  const WindEventId& currentWindEventId,
												  const WindDirectionId& previousWindDirectionId,
												  bool& useAlkaaHeiketaVoimistuaPhrase) const;
	void getWindDirectionBegEnd(const WeatherPeriod& thePeriod,
								WindDirectionId& theWindDirectionIdBeg,
								WindDirectionId& theWindDirectionIdEnd) const;
	bool windDirectionTurns(const WeatherPeriod& thePeriod) const;
	void getPeriodStartAndEndIndex(const WeatherPeriod& period,
								   unsigned int& begIndex,
								   unsigned int& endIndex) const;	  
  };





  std::string get_wind_event_string(const WindEventId& theWindEventId);
  void populate_windspeed_distribution_time_series(const AnalysisSources& theSources,
												   const WeatherArea& theArea,
												   const WeatherPeriod& thePeriod,
												   const string& theVar,
												   vector <pair<float, WeatherResult> >& theWindSpeedDistribution);
  WindDirectionId get_wind_direction_at(const wo_story_params& theParameters,
										  const NFmiTime& pointOfTime,
										  const string& var);
  WindDirectionId get_wind_direction_at(const wo_story_params& theParameters,
										  const WeatherPeriod& period,
										  const string& var);
  bool wind_speed_differ_enough(const AnalysisSources& theSources,
								const WeatherArea& theArea,
								const WeatherPeriod& thePeriod,
								const string& theVar,
								const wind_data_item_vector& windDataVector);
  void get_wind_speed_interval_parameters(const AnalysisSources& theSources,
										  const WeatherArea& theArea,
										  const WeatherPeriod& thePeriod,
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
							   const string& theVar,
							   const wind_data_item_vector& windDataVector,
							   int& lowerLimit,
							   int& upperLimit);

  void get_wind_speed_interval(const AnalysisSources& theSources,
							   const WeatherArea& theArea,
							   const WeatherPeriod& thePeriod,
							   const string& theVar,
							   const wind_data_item_vector& windDataVector,
							   int& lowerLimit,
							   int& upperLimit);
  bool ascending_order(const float& direction1, 
					   const float& direction2);
  bool wind_turns_to_the_same_direction(const float& direction1, 
										const float& direction2,  
										const float& direction3);
} // namespace TextGen

#endif // TEXTGEN_WIND_FORECAST_H
