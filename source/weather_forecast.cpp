// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WeatherStory::overview
 */
// ======================================================================

#include "WeatherStory.h"
#include "CloudinessStory.h"
#include "CloudinessStoryTools.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "HourPeriodGenerator.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "NightAndDayPeriodGenerator.h"
#include "PeriodPhraseFactory.h"
#include "PrecipitationPeriodTools.h"
#include "PrecipitationStoryTools.h"
#include "RangeAcceptor.h"
#include "ValueAcceptor.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "TimeTools.h"
#include "WeatherResult.h"
#include "WeekdayTools.h"
#include "NullPeriodGenerator.h"
#include "WeatherPeriodTools.h"
#include "AreaTools.h"
#include "SeasonTools.h"

#include <boost/lexical_cast.hpp>
#include <vector>
#include <map>

namespace TextGen
{

using namespace Settings;
using namespace WeatherAnalysis;
using namespace AreaTools;
using namespace boost;
using namespace std;


#define AAMUPAIVALLA_WORD "aamup‰iv‰ll‰"
#define ILTAPAIVALLA_WORD "iltap‰iv‰ll‰"
#define SAA_WORD "s‰‰"
#define ON_WORD "on"
#define HEIKKOA_WORD "heikkoa"
#define KOHTALAISTA_WORD "kohtalaista"
#define KOVAA_WORD "kovaa"
#define SADETTA_WORD "sadetta"
#define POUTAA_WORD "poutaa"
#define RUNSASTA_SADETTA_PHRASE "runsasta sadetta"
#define SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE "s‰‰ vaihtelee puolipilvisest‰ pilviseen"
#define VERRATTAIN_SELKEAA_PHRASE "verrattain selke‰‰"
#define VERRATTAIN_PILVISTA_PHRASE "verrattain pilvist‰"
#define SELKEAA_WORD "selke‰‰"
#define PILVISTA_WORD "pilvist‰"
#define SADETTA_WORD "sadetta"
#define MELKO_SELKEAA_PHRASE "melko selke‰‰"
#define PUOLIPILVISTA_WORD "puolipilvist‰"
#define SAA_ON_POUTAINEN_PHRASE "poutaa"
#define HEIKKOA_SADETTA_PHRASE "heikkoa sadetta"
#define KOHTALAISTA_SADETTA_PHRASE "kohtalaista sadetta"
#define SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE "s‰‰ on enimm‰kseen poutaista"
#define PAIKOIN_WORD "paikoin"
#define MONIN_PAIKOIN_WORD "monin paikoin"
#define PILVISTYVAA_WORD "pilvistyv‰‰"
#define SELKENEVAA_WORD "selkenev‰‰"
#define SAA_POUTAANTUU_PHRASE "s‰‰ poutaantuu"
#define MAHDOLLISESTI_UKKOSTA_PHRASE "mahdollisesti ukkosta"
#define PIENI_UKKOSEN_TODENNAKOISYYS_PHRASE "pieni ukkosen todenn‰kˆisyys"
#define AJOITTAIN_UKKOSTA_PHRASE "ajoittain ukkosta"

#define HEIKKOJA_SADEKUUROJA_PHRASE "heikkoja sadekuuroja"
#define VOIMAKKAITA_SADEKUUROJA_PHRASE "voimakkaita sadekuuroja"
#define SADEKUUROJA_WORD "sadekuuroja"
#define LUMIKUUROJA_WORD "lumikuuroja"
#define SADEKUUROJA_WORD "sadekuuroja"
#define RANTAKUUROJA_WORD "r‰nt‰kuuroja"
#define RANTASADETTA_WORD "r‰nt‰sadetta"
#define VESISADETTA_WORD "vesisadetta"
#define LUMISADETTA_WORD "lumisadetta"
#define TIHKUSADETTA_WORD "tihkusadetta"

#define HEIKKOJA_LUMIKUUROJA_PHRASE "heikkoja lumikuuroja"
#define SAKEITA_LUMIKUUROJA_PHRASE "sakeita lumikuuroja"
#define HEIKKOA_LUMISADETTA_PHRASE "heikkoa lumisadetta"
#define SAKEAA_LUMISADETTA_PHRASE "sakeaa lumisadetta"
#define VESI_TAVUVIIVA_WORD "vesi-"
#define RANTA_TAVUVIIVA_WORD "r‰nt‰-"
#define LUMI_TAVUVIIVA_WORD "lumi-"
#define TAI_WORD "tai"
#define JOKA_VOI_OLLA_JAATAVAA_PHRASE "joka voi olla j‰‰t‰v‰‰"
#define JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE "jotka voivat olla j‰‰t‰vi‰"
#define YKSITTAISET_SADEKUUROT_MAHDOLLISIA "yksitt‰iset sadekuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_RANTAKUUROT_MAHDOLLISIA "yksitt‰iset r‰nt‰kuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_LUMIKUUROT_MAHDOLLISIA "yksitt‰iset lumikuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_VESI_RANTA_KUUROT_MAHDOLLISIA "yksitt‰iset vesi- tai r‰nt‰kuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_RANTA_VESI_KUUROT_MAHDOLLISIA "yksitt‰iset r‰nt‰- tai vesikuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_LUMI_RANTA_KUUROT_MAHDOLLISIA "yksitt‰iset lumi- tai r‰nt‰kuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_VESI_LUMI_KUUROT_MAHDOLLISIA "yksitt‰iset vesi- tai lumikuurot ovat kuitenkin mahdollisia"

#define VESI_RANTA_KUUROJA "vesi- tai r‰nt‰kuuroja"
#define RANTA_VESI_KUUROJA "r‰nt‰- tai vesikuuroja"

#define SELKEAA_LIMIT 9.9
#define MELKEIN_SELKEAA_LIMIT 35.0
#define PUOLIPILVISTA_LIMIT 65.0
#define VERRATTAIN_PILVISTA_LIMIT 85.0


  enum weather_result_data_id
	{
	  TEMPERATURE_DATA = 0x1,
	  CLOUDINESS_DATA,
	  PRECIPITATION_DATA,
	  PRECIPITATION_EXTENT_DATA,
	  PRECIPITATION_TYPE_DATA,
	  PRECIPITATION_FORM_WATER_DATA,
	  PRECIPITATION_FORM_DRIZZLE_DATA,
	  PRECIPITATION_FORM_SLEET_DATA,
	  PRECIPITATION_FORM_SNOW_DATA,
	  PRECIPITATION_FORM_FREEZING_DATA,
	  THUNDERPROBABILITY_DATA,
	  UNDEFINED_DATA_ID
	};

  enum rain_form_id
	{
	  WATER_FORM = 0x1,
	  DRIZZLE_FORM = 0x2,
	  SLEET_FORM = 0x4,
	  SNOW_FORM = 0x8,
	  FREEZING_FORM = 0x10,
	  WATER_DRIZZLE_FORM = 0x3,
	  WATER_SLEET_FORM = 0x5,
	  WATER_SNOW_FORM = 0x9,
      WATER_FREEZING_FORM = 0x11,
	  WATER_DRIZZLE_SLEET_FORM = 0x7,
	  WATER_DRIZZLE_SNOW_FORM = 0xB,
	  WATER_DRIZZLE_FREEZING_FORM = 0x13,
	  WATER_SLEET_SNOW_FORM = 0xD,
	  WATER_SLEET_FREEZING_FORM = 0x15,
	  WATER_SNOW_FREEZING_FORM = 0x19,
	  DRIZZLE_SLEET_FORM = 0x6,
	  DRIZZLE_SNOW_FORM = 0xA,
	  DRIZZLE_FREEZING_FORM = 0x12,
	  DRIZZLE_SLEET_SNOW_FORM = 0xE,
	  DRIZZLE_SLEET_FREEZING_FORM = 0x16,
	  DRIZZLE_SNOW_FREEZING_FORM = 0x1A,
	  SLEET_SNOW_FORM = 0xC,
	  SLEET_FREEZING_FORM = 0x14,
	  SLEET_SNOW_FREEZING_FORM = 0x1C,
	  SNOW_FREEZING_FORM = 0x18,
	  MISSING_RAIN_FORM = 0x0
	};

  enum rain_intesity_id
	{
	  DRY_WEATHER,
	  WEAK_RAIN,
	  MODERATE_RAIN,
	  HEAVY_RAIN,
	  MOSTLY_DRY_WEATHER,
	  UNDEFINED_INTENSITY_ID
	};

  enum cloudiness_id
	{
	  SELKEAA,
	  MELKO_SELKEAA,
	  PUOLIPILVISTA,
	  VERRATTAIN_PILVISTA,
	  PUOLIPILVISTA_JA_PILVISTA,
	  PILVISTA,
	  MISSING_CLOUDINESS_ID
	};

  enum part_of_the_day_id
	{
	  AAMU, // 06-09
	  AAMUPAIVA, // 09-11
	  PITKA_AAMUPAIVA, // 09-12
	  KESKIPAIVA, // 11-13
	  ILTAPAIVA, // 13-18
	  PITKA_ILTAPAIVA, // 12-18
	  ILTA, // 18-21
	  ILTAYO, // 22-00
	  KESKIYO, // 00-03
	  AAMUYO, // 03-06
	  PAIVA, // 09-18
	  PITKA_PAIVA, // 06-18
	  YO, // 00-06
	  PITKA_YO, // 22-06
	  YOPUOLI, // 18-06
	  PAIVAPUOLI // 06-18
	};

  enum trend_id
	{
	  PILVISTYY,
	  SELKENEE,
	  POUTAANTUU,
	  MISSING_TREND
	};

	struct WeatherResultDataItem
	{
	  WeatherResultDataItem(const WeatherPeriod& period, 
							const WeatherResult& result, 
							const part_of_the_day_id& partOfTheDay) :
		thePeriod(period),
		theResult(result),
		thePartOfTheDay(partOfTheDay)
	  {}

	  WeatherPeriod thePeriod;
	  WeatherResult theResult;
	  part_of_the_day_id thePartOfTheDay;
	};

  struct PrecipitationDataItem
  {
	PrecipitationDataItem(const unsigned int& rainForm,
						  const float& rainIntensity,
						  const float& rainExtent,
						  const float& rainFormWater,
						  const float& rainFormDrizzle,
						  const float& rainFormSleet,
						  const float& rainFormSnow,
						  const float& rainFormFreezing,
						  const float& rainTypeShowers,
						  const trend_id& trendId) :
	  theRainForm(rainForm),
	  theRainIntensity(rainIntensity),
	  theRainExtent(rainExtent),
	  theRainFormWater(rainFormWater),
	  theRainFormDrizzle(rainFormDrizzle),
	  theRainFormSleet(rainFormSleet),
	  theRainFormSnow(rainFormSnow),
	  theRainFormFreezing(rainFormFreezing),
	  theRainTypeShowers(rainTypeShowers),
	  theTrendId(trendId)
	{}
	
	unsigned int theRainForm;
	float theRainIntensity;
	float theRainExtent;
	float theRainFormWater;
	float theRainFormDrizzle;
	float theRainFormSleet;
	float theRainFormSnow;
	float theRainFormFreezing;
	float theRainTypeShowers;
	trend_id theTrendId;
	
	bool operator==(const PrecipitationDataItem& theItem)
	{
	  return(theRainForm == theItem.theRainForm &&
			 theRainIntensity == theItem.theRainIntensity &&
			 theRainExtent == theItem.theRainExtent &&
			 theRainFormWater == theItem.theRainFormWater &&
			 theRainFormDrizzle == theItem.theRainFormDrizzle &&
			 theRainFormSleet == theItem.theRainFormSleet &&
			 theRainFormSnow == theItem.theRainFormSnow &&
			 theRainFormFreezing == theItem.theRainFormFreezing &&
			 theRainTypeShowers == theItem.theRainTypeShowers &&
			 theTrendId == theItem.theTrendId);
	}
  };

  struct CloudinessDataItem
  {
	CloudinessDataItem(const cloudiness_id& id,
					   const float& min,
					   const float& mean,
					   const float& max,
					   const float& standardDeviation,
					   const trend_id& trendId) :
	  theId(id),
	  theMin(min),
	  theMean(mean),
	  theMax(max),
	  theStandardDeviation(standardDeviation),
	  theTrendId(trendId)
	{
	}

	cloudiness_id theId;
	float theMin;
	float theMean;
	float theMax;
	float theStandardDeviation;
	trend_id theTrendId;
  };

  struct ThunderDataItem
  {
	ThunderDataItem(const float& minProbability,
					const float& meanProbability,
					const float& maxProbability,
					const float& standardDeviationProbability):
	  theMinProbability(minProbability),
	  theMeanProbability(meanProbability),
	  theMaxProbability(maxProbability),
	  theStandardDeviationProbability(standardDeviationProbability)
	{}

	float theMinProbability;
	float theMeanProbability;
	float theMaxProbability;
	float theStandardDeviationProbability;
  };

  typedef vector<WeatherResultDataItem*> weather_result_data_item_vector;
  typedef map<int, weather_result_data_item_vector*> weather_forecast_result_container;
  typedef map<int, weather_forecast_result_container*> weather_forecast_data_container;
  typedef map<int, CloudinessDataItem*> cloudiness_data_item_container;
  typedef vector<cloudiness_data_item_container*> cloudiness_data_container;
  typedef map<int, PrecipitationDataItem*> precipitation_data_item_container;
  typedef vector<precipitation_data_item_container*> precipitation_data_container;
  typedef map<int, ThunderDataItem*> thunder_data_item_container;
  typedef vector<thunder_data_item_container*> thunder_data_container;

	struct wf_story_params
	{								 
	  wf_story_params(const string& variable,
					  const WeatherArea& weatherArea,
					  const WeatherPeriod weatherPeriod,
					  const AnalysisSources& analysisSources,
					  MessageLogger& log) :
		theVariable(variable),
		theArea(weatherArea),
		thePeriod(weatherPeriod),
		theSources(analysisSources),
		theLog(log),
		theHourPeriodCount(0),
		theOriginalPeriodCount(0),
		theForecastArea(NO_AREA),
		theReportCloudiness(false)
	  {
	  }

	  const string& theVariable;
	  const WeatherArea& theArea;
	  const WeatherPeriod thePeriod;
	  const AnalysisSources& theSources;
	  MessageLogger& theLog;
	  unsigned int theHourPeriodCount;
	  unsigned int theOriginalPeriodCount;
	  unsigned short theForecastArea;
	  bool theReportCloudiness;
	  weather_forecast_data_container theCompleteData;
	  cloudiness_data_container theCloudinessData;
	  precipitation_data_container thePrecipitationData;
	  thunder_data_container theThunderData;
	};

  part_of_the_day_id get_part_of_the_day_id(const WeatherPeriod& thePeriod)
  {
	if(thePeriod.localStartTime().GetHour() > 0 && thePeriod.localStartTime().GetHour() < 6)
	  return AAMUYO;
	else if(thePeriod.localStartTime().GetHour() >= 6 && thePeriod.localStartTime().GetHour() <= 12)
	  return AAMUPAIVA;
	else if(thePeriod.localStartTime().GetHour() > 12 && thePeriod.localStartTime().GetHour() <= 18)
	  return ILTAPAIVA;
	else //if(thePeriod.localStartTime().GetHour() > 18)
	  return ILTA;
	

	/*
  enum part_of_the_day_id
	{
	  AAMU, // 06-09
	  AAMUPAIVA, // 09-11
	  PITKA_AAMUPAIVA, // 09-12
	  KESKIPAIVA, // 11-13
	  ILTAPAIVA, // 13-18
	  PITKA_ILTAPAIVA, // 12-18
	  ILTA, // 18-21
	  ILTAYO, // 22-00
	  KESKIYO, // 00-03
	  AAMUYO, // 03-06
	  PAIVA, // 09-18
	  PITKA_PAIVA, // 06-18
	  YO, // 00-06
	  PITKA_YO, // 22-06
	  YOPUOLI, // 18-06
	  PAIVAPUOLI // 06-18
	};

	 */
  }

  bool get_part_of_the_day(const WeatherPeriod& theSourcePeriod, 
						   const part_of_the_day_id& thePartOfTheDayId, 
						   WeatherPeriod& theDestinationPeriod)
  {
	int starthour = -1;
	int endhour = -1;

	switch(thePartOfTheDayId)
	  {
	  case AAMUPAIVA:
		{
		  starthour = 7;
		  endhour = 12;
		break;
		}
	  case ILTAPAIVA:
		  starthour = 13;
		  endhour = 18;
		break;
	  case YO:
		  starthour = 19;
		  endhour = 6;
		break;
	  default:
		break;
	  }

	int new_start_year =  theSourcePeriod.localStartTime().GetYear();
	int new_start_month = theSourcePeriod.localStartTime().GetMonth();
	int new_start_day = theSourcePeriod.localStartTime().GetDay();
	int new_start_hour = -1;
	int new_end_year = theSourcePeriod.localEndTime().GetYear();
	int new_end_month = theSourcePeriod.localEndTime().GetMonth();
	int new_end_day = theSourcePeriod.localEndTime().GetDay();
	int new_end_hour = -1;
	int old_start_hour = theSourcePeriod.localStartTime().GetHour();
	int old_end_hour = theSourcePeriod.localEndTime().GetHour();

	if(old_start_hour <= starthour)
	  new_start_hour = starthour;
	else if(old_start_hour <= endhour)
	  new_start_hour = old_start_hour;

	if(old_end_hour >= endhour)
	  new_end_hour = endhour;
	else if(old_end_hour >= starthour)
	  new_end_hour = old_end_hour;
	  
	if(new_start_hour == -1 || new_end_hour == -1)
	  {
		return false;
	  }
	else
	  {
		NFmiTime start(new_start_year, new_start_month, new_start_day, new_start_hour);
		NFmiTime end(new_end_year, new_end_month, new_end_day, new_end_hour);
		WeatherPeriod wp(start, end);
		theDestinationPeriod = wp;
		return true;
	  }
  }



  void get_sub_time_series(wf_story_params& theParameters,
						   const WeatherPeriod& thePeriod,
						   const weather_result_data_item_vector& theSourceVector,						   
						   weather_result_data_item_vector& theDestinationVector)
  {
	for(unsigned int i = 0; i < theSourceVector.size(); i++)
	  {
		WeatherResultDataItem* item = theSourceVector[i];
		if(item->thePeriod.localStartTime() >= thePeriod.localStartTime() &&
		   item->thePeriod.localEndTime() <= thePeriod.localEndTime())
		  theDestinationVector.push_back(item);		
	  }
  }

  void get_sub_time_series(const part_of_the_day_id& thePartOfTheDay,
						   const weather_result_data_item_vector& theSourceVector,
						   weather_result_data_item_vector& theDestinationVector)
  {
	for(unsigned int i = 0; i < theSourceVector.size(); i++)
	  {
		WeatherResultDataItem* item = theSourceVector[i];
		if(item->thePartOfTheDay == thePartOfTheDay)
		  theDestinationVector.push_back(item);		
	  }
  }

  void get_part_of_the_day_time_series(wf_story_params& theParameters,
									   const WeatherPeriod& theWeatherPeriod,
									   const part_of_the_day_id& thePartOfTheDayId, 
									   const weather_result_data_id& theDataId,
									   weather_result_data_item_vector& theResultVector)
  {
#ifdef LATER
	weather_result_data_item_vector* theCompleteDataVector = theParameters.theHourlyDataContainer[theDataId];

	for(unsigned int i = 0; i < theCompleteDataVector->size(); i++)
	  {
		//		WeatherResult& weatherResult = (*theCompleteDataVector)[i]->theResult;
		WeatherPeriod& weatherPeriod = (*theCompleteDataVector)[i]->thePeriod;
		if(weatherPeriod.localStartTime().EpochTime() >= theWeatherPeriod.localStartTime().EpochTime() &&
		   weatherPeriod.localEndTime().EpochTime() <= theWeatherPeriod.localEndTime().EpochTime() &&
		   weatherPeriod.localStartTime().GetJulianDay() == theWeatherPeriod.localStartTime().GetJulianDay() &&
		   weatherPeriod.localEndTime().GetJulianDay() == theWeatherPeriod.localEndTime().GetJulianDay())
		  {
			int starthour = -1;
			int endhour = -1;
			switch(thePartOfTheDayId)
			  {
			  case AAMU:
				{
				  starthour = 6;
				  endhour = 9;
				}
				break;
			  case AAMUPAIVA:
				{
				  starthour = 9;
				  endhour = 11;
				}
				break;
			  case PITKA_AAMUPAIVA:
				{
				  starthour = 9;
				  endhour = 12;
				}
				break;
			  case KESKIPAIVA:
				{
				  starthour = 11;
				  endhour = 13;
				}
				break;
			  case ILTAPAIVA:
				{
				  starthour = 13;
				  endhour = 18;
				}
				break;
			  case PITKA_ILTAPAIVA:
				{
				  starthour = 12;
				  endhour = 18;
				}
				break;
			  case ILTA:
				{
				  starthour = 18;
				  endhour = 21;
				}
				break;
			  case ILTAYO:
				{
				  starthour = 22;
				  endhour = 0;
				}
				break;
			  case KESKIYO:
				{
				  starthour = 0;
				  endhour = 3;
				}
				break;
			  case AAMUYO:
				{
				  starthour = 3;
				  endhour = 6;
				}
				break;
			  case PAIVA:
				{
				  starthour = 9;
				  endhour = 18;
				}
				break;
				break;
			  case PITKA_PAIVA:
				{
				  starthour = 6;
				  endhour = 18;
				}
				break;
			  case YO:
				{
				  starthour = 0;
				  endhour = 6;
				}
				break;
			  case PITKA_YO:
				{
				  starthour = 22;
				  endhour = 6;
				}
				break;
			  case YOPUOLI:
				{
				  starthour = 18;
				  endhour = 6;
				}
				break;
			  case PAIVAPUOLI:
				{
				  starthour = 6;
				  endhour = 18;
				}
				break;
			  }
			if(starthour != -1 && endhour != -1)
			  {
				if(weatherPeriod.localStartTime().GetHour() >= starthour &&
				   weatherPeriod.localEndTime().GetHour() <= endhour)
				  theResultVector.push_back((*theCompleteDataVector)[i]);
			  }
		  }
	  }
#endif
  }

	const void log_start_time_and_end_time(MessageLogger& theLog, 
										   const std::string& theLogMessage, 
										   const WeatherPeriod& thePeriod)
	{
	  theLog << NFmiStringTools::Convert(theLogMessage)
			 << thePeriod.localStartTime()
			 << " ... "
			 << thePeriod.localEndTime()
			 << endl;
	}

  const void log_weather_result_data_item(MessageLogger& theLog, 
										  const WeatherResultDataItem& theWeatherResultDataItem)
  {
	theLog << theWeatherResultDataItem.thePeriod.localStartTime()
		   << " ... "
		   << theWeatherResultDataItem.thePeriod.localEndTime()
		   << ": "
		   << theWeatherResultDataItem.theResult
		   << endl;
  }

  const void log_weather_result_time_series(MessageLogger& theLog, 
											const std::string& theLogMessage, 
											const weather_result_data_item_vector& theTimeSeries)
  {
	theLog << NFmiStringTools::Convert(theLogMessage) << endl;

	for(unsigned int i = 0; i < theTimeSeries.size(); i++)
	  {
		//		const WeatherResultDataItem& theWeatherResultDataItem = *theTimeSeries[i]; 
		log_weather_result_data_item(theLog, *theTimeSeries[i]);
	  } 
  }

  void log_weather_result_data(MessageLogger& theLog, 
							   weather_forecast_result_container& theDataContainer)
  {
	std::string theLogMessage;
	weather_result_data_item_vector* timeSeries = 0;
	for(unsigned int i = TEMPERATURE_DATA; i < UNDEFINED_DATA_ID; i++)
	  {
		timeSeries = theDataContainer[i];
		if(i == TEMPERATURE_DATA)
		  theLogMessage = "*** temperature ****";
		else if(i == PRECIPITATION_DATA)
		  theLogMessage = "*** precipitation ****";
		else if(i == PRECIPITATION_EXTENT_DATA)
		  theLogMessage = "*** precipitation extent ****";
		else if(i == PRECIPITATION_TYPE_DATA)
		  theLogMessage = "*** precipitation type ****";
		else if(i == CLOUDINESS_DATA)
		  theLogMessage = "*** cloudiness ****";
		else if(i == PRECIPITATION_FORM_WATER_DATA)
		  theLogMessage = "*** precipitation form water ****";
		else if(i == PRECIPITATION_FORM_DRIZZLE_DATA)
		  theLogMessage = "*** precipitation form drizzle ****";
		else if(i == PRECIPITATION_FORM_SLEET_DATA)
		  theLogMessage = "*** precipitation form sleet ****";
		else if(i == PRECIPITATION_FORM_SNOW_DATA)
		  theLogMessage = "*** precipitation form snow ****";
		else if(i == PRECIPITATION_FORM_FREEZING_DATA)
		  theLogMessage = "*** precipitation form freezing ****";
		else if(i == THUNDERPROBABILITY_DATA)
		  theLogMessage = "*** thunder probability ****";
		else
		  continue;

		log_weather_result_time_series(theLog, theLogMessage, *timeSeries);
	  }
  }

  const void log_weather_result_data(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		theParameters.theLog << "**  INLAND AREA DATA   **" << endl;
		log_weather_result_data(theParameters.theLog, *theParameters.theCompleteData[INLAND_AREA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		theParameters.theLog << "**  COASTAL AREA DATA   **" << endl;
		log_weather_result_data(theParameters.theLog, *theParameters.theCompleteData[COASTAL_AREA]);
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		theParameters.theLog << "**  FULL AREA DATA   **" << endl;
		log_weather_result_data(theParameters.theLog, *theParameters.theCompleteData[FULL_AREA]);
	  }
  }

  const void log_subperiods(wf_story_params& theParameters)
  {
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	for(unsigned int i = 1; i <= generator.size(); i++ )
	  {
		weather_result_data_item_vector resultVector;
		if(generator.isday(i))
		  {
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											AAMU, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "AAMUPERIODI",
										   resultVector);
			resultVector.clear();

			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											AAMUPAIVA, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "AAMUPƒIVƒPERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											ILTAPAIVA, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "ILTAPƒIVƒPERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											ILTA, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "ILTAPERIODI",
										   resultVector);
			resultVector.clear();

		  }
		else
		  {
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											ILTA, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "ILTAPERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											ILTAYO, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "ILTAY÷PERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											YO, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "Y÷PERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											AAMUYO, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "AAMUY÷PERIODI",
										   resultVector);
			resultVector.clear();
		  }
	  }
  }

   void populate_temperature_time_series(const string& theVariable, 
										const AnalysisSources& theSources,
										const WeatherArea& theArea,										
										weather_result_data_item_vector& theResultData)
  {
	GridForecaster theForecaster;
	for(unsigned int i = 0; i < theResultData.size(); i++)
	  {
		theResultData[i]->theResult = theForecaster.analyze(theVariable,
															theSources,
															Temperature,
															Mean,
															Maximum,
															theArea,
															theResultData[i]->thePeriod);
	  }
  }

  void populate_temperature_time_series(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		WeatherArea inlandArea = theParameters.theArea;
		inlandArea.type(WeatherArea::Inland);
		populate_temperature_time_series(theParameters.theVariable,
										 theParameters.theSources,
										 inlandArea,
										 *(*theParameters.theCompleteData[INLAND_AREA])[TEMPERATURE_DATA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_temperature_time_series(theParameters.theVariable,
										 theParameters.theSources,
										 coastalArea,
										 *(*theParameters.theCompleteData[COASTAL_AREA])[TEMPERATURE_DATA]);
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_temperature_time_series(theParameters.theVariable,
										 theParameters.theSources,
										 theParameters.theArea,
										 *(*theParameters.theCompleteData[FULL_AREA])[TEMPERATURE_DATA]);
	  }
  }

  void populate_precipitation_time_series(const string& theVariable, 
										  const AnalysisSources& theSources,
										  const WeatherArea& theArea,	
										  weather_forecast_result_container& theHourlyDataContainer)
  {
	GridForecaster theForecaster;

	weather_result_data_item_vector* precipitationHourly = 
	  theHourlyDataContainer[PRECIPITATION_DATA];
	weather_result_data_item_vector* precipitationExtentHourly = 
	  theHourlyDataContainer[PRECIPITATION_EXTENT_DATA];
	weather_result_data_item_vector* precipitationFormWaterHourly = 
	  theHourlyDataContainer[PRECIPITATION_FORM_WATER_DATA];
	weather_result_data_item_vector* precipitationFormDrizzleHourly = 
	  theHourlyDataContainer[PRECIPITATION_FORM_DRIZZLE_DATA];
	weather_result_data_item_vector* precipitationFormSleetHourly = 
	  theHourlyDataContainer[PRECIPITATION_FORM_SLEET_DATA];
	weather_result_data_item_vector* precipitationFormSnowHourly = 
	  theHourlyDataContainer[PRECIPITATION_FORM_SNOW_DATA];
	weather_result_data_item_vector* precipitationFormFreezingHourly = 
	  theHourlyDataContainer[PRECIPITATION_FORM_FREEZING_DATA];
	weather_result_data_item_vector* precipitationTypeHourly = 
	  theHourlyDataContainer[PRECIPITATION_TYPE_DATA];

	// dry weather: 0.3 mm/day == 0.0125/hour
	float dry_weather_limit = Settings::optional_double(theVariable+"::dry_weather_limit", 0.0125);
	RangeAcceptor rainlimits;
	rainlimits.lowerLimit(dry_weather_limit);
	RangeAcceptor waterfilter;
	waterfilter.lowerLimit(0);	// 0 = drizzle
	waterfilter.upperLimit(1);	// 1 = water
	//RangeAcceptor percentagelimits;
	//percentagelimits.lowerLimit(maxrainlimit);
	ValueAcceptor drizzlefilter;
	drizzlefilter.value(0);	// 0 = drizzle
	ValueAcceptor sleetfilter;
	sleetfilter.value(2);	// 2 = sleet
	ValueAcceptor snowfilter;
	snowfilter.value(3);	// 3 = snow
	ValueAcceptor freezingfilter;
	freezingfilter.value(4);	// 4 = freezing
	ValueAcceptor showerfilter;
	showerfilter.value(2);	// 1=large scale, 2=showers

	for(unsigned int i = 0; i < precipitationHourly->size(); i++)
	  {
        (*precipitationHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								Precipitation,
								Mean,
								Sum,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								rainlimits);

        (*precipitationExtentHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								Precipitation,
								Percentage,
								Sum,
								theArea,
								(*precipitationExtentHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								rainlimits);

		(*precipitationFormWaterHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								waterfilter);

		(*precipitationFormDrizzleHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								drizzlefilter);

		(*precipitationFormSleetHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								sleetfilter);

		(*precipitationFormSnowHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								snowfilter);

		(*precipitationFormFreezingHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								freezingfilter);

		(*precipitationTypeHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationType,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								showerfilter);
	  }
  }

  void populate_precipitation_time_series(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		WeatherArea inlandArea = theParameters.theArea;
		inlandArea.type(WeatherArea::Inland);
		populate_precipitation_time_series(theParameters.theVariable,
										   theParameters.theSources,
										   inlandArea,
										   *theParameters.theCompleteData[INLAND_AREA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_precipitation_time_series(theParameters.theVariable,
										   theParameters.theSources,
										   coastalArea,
										   *theParameters.theCompleteData[COASTAL_AREA]);
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_precipitation_time_series(theParameters.theVariable,
										   theParameters.theSources,
										   theParameters.theArea,
										   *theParameters.theCompleteData[FULL_AREA]);
	  }
  }

  void populate_thunderprobability_time_series(const string& theVariable, 
											   const AnalysisSources& theSources,
											   const WeatherArea& theArea,										
											   weather_forecast_result_container& theHourlyDataContainer)
  {

	weather_result_data_item_vector& thunderProbabilityHourly = 
	  *(theHourlyDataContainer[THUNDERPROBABILITY_DATA]);
	  
 	GridForecaster theForecaster;

	for(unsigned int i = 0; i < thunderProbabilityHourly.size(); i++)
	  {
		thunderProbabilityHourly[i]->theResult = theForecaster.analyze(theVariable,
																	   theSources,
																	   Thunder,
																	   Maximum,
																	   Maximum,
																	   theArea,
																	   thunderProbabilityHourly[i]->thePeriod);
	  }
  }

 void populate_thunderprobability_time_series(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		WeatherArea inlandArea = theParameters.theArea;
		inlandArea.type(WeatherArea::Inland);
		populate_thunderprobability_time_series(theParameters.theVariable,
												theParameters.theSources,
												inlandArea,
												*theParameters.theCompleteData[INLAND_AREA]);
	  }

	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_thunderprobability_time_series(theParameters.theVariable,
												theParameters.theSources,
												coastalArea,
												*theParameters.theCompleteData[COASTAL_AREA]);
	  }

	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_thunderprobability_time_series(theParameters.theVariable,
												theParameters.theSources,
												theParameters.theArea,
												*theParameters.theCompleteData[FULL_AREA]);
	  }
  }


  void populate_cloudiness_time_series(const string& theVariable, 
										const AnalysisSources& theSources,
										const WeatherArea& theArea,										
										weather_result_data_item_vector& theResultData)
  {
 	GridForecaster theForecaster;
	for(unsigned int i = 0; i < theResultData.size(); i++)
	  {
		theResultData[i]->theResult = theForecaster.analyze(theVariable,
															theSources,
															Cloudiness,
															Maximum,
															Mean,
															theArea,
															theResultData[i]->thePeriod);
	  }
 }

  void populate_cloudiness_time_series(wf_story_params& theParameters)
  {
	// theParameters.theVariable+"::fake::"+hourstr+"::cloudiness",
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		WeatherArea inlandArea = theParameters.theArea;
		inlandArea.type(WeatherArea::Inland);
		populate_cloudiness_time_series(theParameters.theVariable,
										theParameters.theSources,
										inlandArea,
										*(*theParameters.theCompleteData[INLAND_AREA])[CLOUDINESS_DATA]);
	  }

	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_cloudiness_time_series(theParameters.theVariable,
										theParameters.theSources,
										coastalArea,
										*(*theParameters.theCompleteData[COASTAL_AREA])[CLOUDINESS_DATA]);
	  }

	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_cloudiness_time_series(theParameters.theVariable,
										theParameters.theSources,
										theParameters.theArea,
										*(*theParameters.theCompleteData[FULL_AREA])[CLOUDINESS_DATA]);
	  }
  }

  float get_standard_deviation(const float& theAverage, const weather_result_data_item_vector& theTimeSeries)
  {
	float deviation_sum_pow2 = 0.0;
	int counter = 0;
	for(unsigned int i = 0; i < theTimeSeries.size(); i++)
	  {
		if(theTimeSeries[i]->theResult.value() == kFloatMissing)
		  continue;
		deviation_sum_pow2 += std::pow(theAverage - theTimeSeries[i]->theResult.value(), 2);
		counter++;
	  }

	return std::sqrt(deviation_sum_pow2 / counter);
  }

  float get_mean(const weather_result_data_item_vector& theTimeSeries)
  {
	float rain_sum = 0.0;
	int counter = 0;
	for(unsigned int i = 0; i < theTimeSeries.size(); i++)
	  {
		if(theTimeSeries[i]->theResult.value() == kFloatMissing)
		  continue;
		rain_sum += theTimeSeries[i]->theResult.value();
		counter++;
	  }

	if((counter == 0 && theTimeSeries.size() > 0) || theTimeSeries.size() == 0)
	  return kFloatMissing;
	else
	  return rain_sum / counter;
  }

  void get_min_max(const weather_result_data_item_vector& theTimeSeries, float& theMin, float& theMax)
  {
	theMin = theMax = kFloatMissing;

	for(unsigned int i = 0; i < theTimeSeries.size(); i++)
	  {
		if(theTimeSeries[i]->theResult.value() == kFloatMissing)
		  continue;
		if(i == 0)
		  {
			theMin = theTimeSeries[i]->theResult.value();
			theMax = theTimeSeries[i]->theResult.value();
		  }
		else
		  {
			if(theMin > theTimeSeries[i]->theResult.value())
			  theMin = theTimeSeries[i]->theResult.value();
			else
			if(theMax < theTimeSeries[i]->theResult.value())
			  theMax = theTimeSeries[i]->theResult.value();
		  }
	  }
  }

  cloudiness_id get_cloudiness_id(const float& theCloudiness) 
  {
	cloudiness_id id(MISSING_CLOUDINESS_ID);

	if(theCloudiness < 0)
	  return id;

	if(theCloudiness <= SELKEAA_LIMIT)
	  {
		id = SELKEAA;
	  }
	else if(theCloudiness  <= MELKEIN_SELKEAA_LIMIT)
	  {
		id = MELKO_SELKEAA;
	  }
	else if(theCloudiness <= PUOLIPILVISTA_LIMIT)
	  {
		id = PUOLIPILVISTA;
	  }
	else if(theCloudiness <= VERRATTAIN_PILVISTA_LIMIT)
	  {
		id = VERRATTAIN_PILVISTA;
	  }
	else
	  {
		id = PILVISTA;
	  }

	return id;
  }

  cloudiness_id get_cloudiness_id(const float& theMin, 
								  const float& theMean, 
								  const float& theMax, 
								  const float& theStandardDeviation)
  {
	cloudiness_id id(MISSING_CLOUDINESS_ID);

	if(theMean == -1)
	  return id;

	//#define SELKEAA_LIMIT 9.9
	//#define MELKEIN_SELKEAA_LIMIT 35.0
	//#define PUOLIPILVISTA_LIMIT 65.0
	//#define VERRATTAIN_PILVISTA_LIMIT 85.0

	if(theMin + theStandardDeviation >= MELKEIN_SELKEAA_LIMIT && 
	   theMin - theStandardDeviation <= PUOLIPILVISTA_LIMIT && 
	   theMax + theStandardDeviation >= VERRATTAIN_PILVISTA_LIMIT)
	  {
		id = PUOLIPILVISTA_JA_PILVISTA;
	  }
	else if(theMean <= SELKEAA_LIMIT)
	  {
		if(theMean + theStandardDeviation <= SELKEAA_LIMIT)
		  id = SELKEAA;
		else
		  id = MELKO_SELKEAA;
	  }
	else if(theMean  <= MELKEIN_SELKEAA_LIMIT)
	  {
		if(theMean + theStandardDeviation <= MELKEIN_SELKEAA_LIMIT)
		  id = MELKO_SELKEAA;
		else
		  id = PUOLIPILVISTA;
	  }
	else if(theMean <= PUOLIPILVISTA_LIMIT)
	  {
		if(theMean + theStandardDeviation <= PUOLIPILVISTA_LIMIT)
		  id = PUOLIPILVISTA;
		else
		  id = VERRATTAIN_PILVISTA;
	  }
	else if(theMean + theStandardDeviation <= VERRATTAIN_PILVISTA_LIMIT)
	  {
		  id = VERRATTAIN_PILVISTA;
	  }
	else
	  {
		id = PILVISTA;
	  }

	return id;
  }

  CloudinessDataItem* get_cloudiness_data_item(wf_story_params& theParameters, const WeatherPeriod& thePeriod)
  {
	CloudinessDataItem* item = 0;

	cloudiness_id theCloudinessId(MISSING_CLOUDINESS_ID);

	float min = -1.0;
	float mean = -1.0;
	float max = -1.0;   
	float standard_deviation = -1.0;
	weather_result_data_item_vector theInterestingData;

	trend_id theTrendId(MISSING_TREND);

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[FULL_AREA])[CLOUDINESS_DATA],
							theInterestingData);
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[INLAND_AREA])[CLOUDINESS_DATA],
							theInterestingData);
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[COASTAL_AREA])[CLOUDINESS_DATA],
							theInterestingData);
	  }

	if(theInterestingData.size() > 0)
	  {
		mean = get_mean(theInterestingData);
		get_min_max(theInterestingData, min, max);
		standard_deviation = get_standard_deviation(mean, theInterestingData);	
		theCloudinessId = get_cloudiness_id(min, mean, max, standard_deviation);

		if(theInterestingData.size() > 2)
		  {
			cloudiness_id cloudinessAtStart = 
			  get_cloudiness_id(theInterestingData[0]->theResult.value());
			cloudiness_id cloudinessAtEnd = 
			  get_cloudiness_id(theInterestingData[theInterestingData.size()-1]->theResult.value());

			bool dataOk = (cloudinessAtStart != kFloatMissing && cloudinessAtEnd != kFloatMissing);
			if(dataOk && cloudinessAtStart >= VERRATTAIN_PILVISTA && cloudinessAtEnd == SELKEAA)
			  theTrendId = SELKENEE;
			else if(dataOk && cloudinessAtStart <= MELKO_SELKEAA && cloudinessAtEnd >= VERRATTAIN_PILVISTA)
			  theTrendId = PILVISTYY;
		  }
	  }

	item = new CloudinessDataItem(theCloudinessId, min, mean, max, standard_deviation, theTrendId);

	return item;
  }

  Sentence cloudiness_sentence(const cloudiness_id& theCloudinessId)
  {
	Sentence sentence;

	std::string cloudiness_phrase;
	switch(theCloudinessId)
	  {
	  case PUOLIPILVISTA_JA_PILVISTA:
		cloudiness_phrase = SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
		break;
	  case SELKEAA:
		cloudiness_phrase = SELKEAA_WORD;
		break;
	  case MELKO_SELKEAA:
		cloudiness_phrase = MELKO_SELKEAA_PHRASE;
		break;
	  case PUOLIPILVISTA:
		cloudiness_phrase = PUOLIPILVISTA_WORD;
		break;
	  case VERRATTAIN_PILVISTA:
		cloudiness_phrase = VERRATTAIN_PILVISTA_PHRASE;
		break;
	  case PILVISTA:
		cloudiness_phrase = PILVISTA_WORD;
		break;
	  default:
		break;
	  }

	if(cloudiness_phrase.length() > 0)
	  {
		if(theCloudinessId == PUOLIPILVISTA_JA_PILVISTA)
		  sentence << cloudiness_phrase;
		else
		  sentence << SAA_WORD << ON_WORD << cloudiness_phrase;
	  }
	
	return sentence;
  }

  Sentence cloudiness_sentence(wf_story_params& theParameters, const WeatherPeriod& thePeriod)
  {
	Sentence sentence;

	float min = -1.0;
	float mean = -1.0;
	float max = -1.0;   
	float standard_deviation = -1.0;

	/*
	float min_morning = 0.0;
	float mean_morning = 0.0;
	float max_morning = 0.0;
	float min_afternoon = 0.0;
	float mean_afternoon = 0.0;
	float max_afternoon = 0.0;
   
	float standard_deviation_morning = 0.0;
	float standard_deviation_afternoon = 0.0;
	float standard_deviation_morning = 0.0;
	*/	/*
	const weather_result_data_item_vector* theCloudinessInland = 0;
	const weather_result_data_item_vector* theCloudinessCoastal = 0;
	const weather_result_data_item_vector* theCloudinessFull = 0;
	*/
	weather_result_data_item_vector theInterestingData;

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[FULL_AREA])[CLOUDINESS_DATA],
							theInterestingData);
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[INLAND_AREA])[CLOUDINESS_DATA],
							theInterestingData);
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[COASTAL_AREA])[CLOUDINESS_DATA],
							theInterestingData);
	  }

	if(theInterestingData.size() > 0)
	  {
		get_min_max(theInterestingData, min, max);
		mean = get_mean(theInterestingData);
		standard_deviation = get_standard_deviation(mean, theInterestingData);

		cloudiness_id id = get_cloudiness_id(min, mean, max, standard_deviation);

		sentence << cloudiness_sentence(id);
	  }

	return sentence;
  }

  Sentence cloudiness_change_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	Sentence cloudinessSentence;

	cloudiness_data_item_container* theResultData = theParameters.theCloudinessData[thePeriodNumber - 1];

	if(theResultData)
	  {
		trend_id theTrendMorning(MISSING_TREND);
		trend_id theTrendAfternoon(MISSING_TREND);
		trend_id theTrendNight(MISSING_TREND);

		if(theResultData->find(AAMUPAIVA) != theResultData->end())
		  theTrendMorning = ((*theResultData)[AAMUPAIVA])->theTrendId;
		if(theResultData->find(ILTAPAIVA) != theResultData->end())
		  theTrendAfternoon = ((*theResultData)[ILTAPAIVA])->theTrendId;
		if(theResultData->find(YO) != theResultData->end())
		  theTrendNight = ((*theResultData)[YO])->theTrendId;

		if(theTrendNight == MISSING_TREND && theTrendMorning == PILVISTYY && theTrendAfternoon == MISSING_TREND)
		  sentence << AAMUPAIVALLA_WORD << PILVISTYVAA_WORD;
		else if(theTrendNight == MISSING_TREND && theTrendMorning == MISSING_TREND && theTrendAfternoon == PILVISTYY)
		  sentence << ILTAPAIVALLA_WORD << PILVISTYVAA_WORD;
		if(theTrendNight == MISSING_TREND && theTrendMorning == SELKENEE && theTrendAfternoon == MISSING_TREND)
		  sentence << AAMUPAIVALLA_WORD << SELKENEVAA_WORD;
		else if(theTrendNight == MISSING_TREND && theTrendMorning == MISSING_TREND && theTrendAfternoon == SELKENEE)
		  sentence << ILTAPAIVALLA_WORD << SELKENEVAA_WORD;
	  }

	return sentence;
  }

  Sentence cloudiness_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	if(generator.size() < thePeriodNumber)
	  return sentence;

	Sentence cloudinessSentence;

	cloudiness_data_item_container* theResultData = theParameters.theCloudinessData[thePeriodNumber - 1];

	if(theResultData)
	  {
		cloudiness_id theCloudinessIdMorning(MISSING_CLOUDINESS_ID);
		cloudiness_id theCloudinessIdAfternoon(MISSING_CLOUDINESS_ID);
		cloudiness_id theCloudinessIdNight(MISSING_CLOUDINESS_ID);

		if(theResultData->find(YO) != theResultData->end())
		  theCloudinessIdNight = ((*theResultData)[YO])->theId;
		if(theResultData->find(AAMUPAIVA) != theResultData->end())
		  theCloudinessIdMorning = ((*theResultData)[AAMUPAIVA])->theId;
		if(theResultData->find(ILTAPAIVA) != theResultData->end())
		  theCloudinessIdAfternoon = ((*theResultData)[ILTAPAIVA])->theId;
		
		if(theCloudinessIdNight != MISSING_CLOUDINESS_ID)
		  {
			sentence << cloudiness_sentence(theCloudinessIdNight);
		  }
		else
		  {
			if(theCloudinessIdMorning != MISSING_CLOUDINESS_ID &&
			   theCloudinessIdAfternoon != MISSING_CLOUDINESS_ID)
			  {
				if(theCloudinessIdMorning == PILVISTA && theCloudinessIdAfternoon == PUOLIPILVISTA ||
				   theCloudinessIdMorning == PUOLIPILVISTA && theCloudinessIdAfternoon == PILVISTA )
				  {
					sentence << SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
				  }
				else
				  {
					if(abs(theCloudinessIdMorning - theCloudinessIdAfternoon) < 3)
					  {
						cloudiness_id cid = theCloudinessIdMorning > theCloudinessIdAfternoon ? 
						  theCloudinessIdMorning : theCloudinessIdAfternoon;
						sentence << cloudiness_sentence(cid);
					  }
					else
					  {
						sentence << AAMUPAIVALLA_WORD << cloudiness_sentence(theCloudinessIdMorning);
						sentence << Delimiter(",");
						sentence << ILTAPAIVALLA_WORD << cloudiness_sentence(theCloudinessIdAfternoon);
					  }
				  }
			  }
			else if(theCloudinessIdMorning != MISSING_CLOUDINESS_ID)
			  {
				sentence << cloudiness_sentence(theCloudinessIdMorning);
			  }
			else if(theCloudinessIdAfternoon != MISSING_CLOUDINESS_ID)
			  {
				sentence << cloudiness_sentence(theCloudinessIdAfternoon);
			  }
		  }
	  }

	return sentence;
  }

  void analyze_cloudiness_data(wf_story_params& theParameters)
  {
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	for(unsigned int i = 1; i <= generator.size(); i++)
	  {
		cloudiness_data_item_container* resultContainer = new cloudiness_data_item_container();

		WeatherPeriod theWeatherPeriod =  generator.period(i);
		WeatherPeriod theSubPeriod(theWeatherPeriod);

		if(generator.isday(i))
		  {
			if(get_part_of_the_day(theWeatherPeriod, AAMUPAIVA, theSubPeriod))
			  {
				CloudinessDataItem* dataItem = get_cloudiness_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(AAMUPAIVA, dataItem));
			  }
			if(get_part_of_the_day(theWeatherPeriod, ILTAPAIVA, theSubPeriod))
			  {
				CloudinessDataItem* dataItem = get_cloudiness_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(ILTAPAIVA, dataItem));
			  }
		  }
		else
		  {
			if(get_part_of_the_day(theWeatherPeriod, YO, theSubPeriod))
			  {
				CloudinessDataItem* dataItem = get_cloudiness_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(YO, dataItem));
			  }
		  }
		theParameters.theCloudinessData.push_back(resultContainer);
	  }
  }

  unsigned int get_complete_rain_form(const string& theVariable,
									  const float theRainFormWater,
									  const float theRainFormDrizzle,
									  const float theRainFormSleet,
									  const float theRainFormSnow,
									  const float theRainFormFreezing)
  {
	unsigned int rain_form = 0;

	typedef std::pair<float, rain_form_id> rain_form_type;
	rain_form_type water(theRainFormWater, WATER_FORM);
	rain_form_type drizzle(theRainFormDrizzle, DRIZZLE_FORM);
	rain_form_type sleet(theRainFormSleet, SLEET_FORM);
	rain_form_type snow(theRainFormSnow, SNOW_FORM);
	rain_form_type freezing(theRainFormFreezing, FREEZING_FORM);

	vector<rain_form_type> rain_forms;
	rain_forms.push_back(water);
	rain_forms.push_back(drizzle);
	rain_forms.push_back(sleet);
	rain_forms.push_back(snow);
	rain_forms.push_back(freezing);

	sort(rain_forms.begin(),rain_forms.end());

	rain_form_id primaryRainForm = rain_forms[4].first > 0.0 ? rain_forms[4].second : MISSING_RAIN_FORM;
	rain_form_id secondaryRainForm = rain_forms[3].first > 0.0 ? rain_forms[3].second : MISSING_RAIN_FORM;
	rain_form_id tertiaryRainForm = rain_forms[2].first > 0.0 ? rain_forms[2].second : MISSING_RAIN_FORM;

	rain_form |= primaryRainForm;
	rain_form |= secondaryRainForm;
	rain_form |= tertiaryRainForm;

	return rain_form;
  }

  void extract_the_interesting_precipitation_data(wf_story_params& theParameters,
												  const WeatherPeriod& thePeriod,
												  const forecast_area_id& theAreaId,
												  weather_result_data_item_vector& thePrecipitation,
												  weather_result_data_item_vector& thePrecipitationExtent,
												  weather_result_data_item_vector& thePrecipitationFromWater,
												  weather_result_data_item_vector& thePrecipitationFromDrizzle,
												  weather_result_data_item_vector& thePrecipitationFromSleet,
												  weather_result_data_item_vector& thePrecipitationFromSnow,
												  weather_result_data_item_vector& thePrecipitationFromFreezing,
												  weather_result_data_item_vector& thePrecipitationType)
  {
		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_DATA],
							thePrecipitation);

		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_EXTENT_DATA],
							thePrecipitationExtent);

		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_WATER_DATA],
							thePrecipitationFromWater);

		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_DRIZZLE_DATA],
							thePrecipitationFromDrizzle);

		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_SLEET_DATA],
							thePrecipitationFromSleet);

		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_SNOW_DATA],
							thePrecipitationFromSnow);

		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_FREEZING_DATA],
							thePrecipitationFromFreezing);

		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_TYPE_DATA],
							thePrecipitationType);
  }

trend_id get_precipitation_trend(const weather_result_data_item_vector& thePrecipitationData)
{
  trend_id precipitation_trend(MISSING_TREND);
  
  if(thePrecipitationData.size() >= 2)
	{
	  float atStart = thePrecipitationData[0]->theResult.value();
	  float atEnd = thePrecipitationData[thePrecipitationData.size()-1]->theResult.value();
		
		if(atStart >= 0.4 && atEnd <= 0.04)
		  precipitation_trend = POUTAANTUU;
	}
  
  return precipitation_trend;
}

PrecipitationDataItem* get_precipitation_data_item(wf_story_params& theParameters, 
													 const WeatherPeriod& thePeriod)
  {
	PrecipitationDataItem* item = 0;

	/*	const double freezing_rain_limit = optional_double(theParameters.theVariable + "::freezing_rain_limit", 10.0);
	const int shower_limit = optional_percentage(theParameters.theVariable + "::shower_limit",80);

	const double weak_rain_limit = 0.4;
	const double hard_rain_limit = 2.0;
	const double weak_snow_limit = 0.4;
	const double hard_snow_limit = 1.5;
	const double ignore_rain_limit = 0.04;
	const double ignore_snow_limit = 0.02;
	const double ignore_sleet_limit = 0.02;
	*/
	weather_result_data_item_vector thePrecipitationInland;
	weather_result_data_item_vector thePrecipitationExtentInland;
	weather_result_data_item_vector thePrecipitationFromWaterInland;
	weather_result_data_item_vector thePrecipitationFromDrizzleInland;
	weather_result_data_item_vector thePrecipitationFromSleetInland;
	weather_result_data_item_vector thePrecipitationFromSnowInland;
	weather_result_data_item_vector thePrecipitationFromFreezingInland;
	weather_result_data_item_vector thePrecipitationTypeInland;
	weather_result_data_item_vector thePrecipitationCoastal;
	weather_result_data_item_vector thePrecipitationExtentCoastal;
	weather_result_data_item_vector thePrecipitationFromWaterCoastal;
	weather_result_data_item_vector thePrecipitationFromDrizzleCoastal;
	weather_result_data_item_vector thePrecipitationFromSleetCoastal;
	weather_result_data_item_vector thePrecipitationFromSnowCoastal;
	weather_result_data_item_vector thePrecipitationFromFreezingCoastal;
	weather_result_data_item_vector thePrecipitationTypeCoastal;
	weather_result_data_item_vector thePrecipitationFull;
	weather_result_data_item_vector thePrecipitationExtentFull;
	weather_result_data_item_vector thePrecipitationFromWaterFull;
	weather_result_data_item_vector thePrecipitationFromDrizzleFull;
	weather_result_data_item_vector thePrecipitationFromSleetFull;
	weather_result_data_item_vector thePrecipitationFromSnowFull;
	weather_result_data_item_vector thePrecipitationFromFreezingFull;
	weather_result_data_item_vector thePrecipitationTypeFull;


	const double ignore_drizzle_limit = 0.02;


	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		extract_the_interesting_precipitation_data(theParameters,
												   thePeriod,
												   INLAND_AREA,
												   thePrecipitationInland,
												   thePrecipitationExtentInland,
												   thePrecipitationFromWaterInland,
												   thePrecipitationFromDrizzleInland,
												   thePrecipitationFromSleetInland,
												   thePrecipitationFromSnowInland,
												   thePrecipitationFromFreezingInland,
												   thePrecipitationTypeInland);

	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		extract_the_interesting_precipitation_data(theParameters,
												   thePeriod,
												   COASTAL_AREA,
												   thePrecipitationCoastal,
												   thePrecipitationExtentCoastal,
												   thePrecipitationFromWaterCoastal,
												   thePrecipitationFromDrizzleCoastal,
												   thePrecipitationFromSleetCoastal,
												   thePrecipitationFromSnowCoastal,
												   thePrecipitationFromFreezingCoastal,
												   thePrecipitationTypeCoastal);

	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		extract_the_interesting_precipitation_data(theParameters,
												   thePeriod,
												   FULL_AREA,
												   thePrecipitationFull,
												   thePrecipitationExtentFull,
												   thePrecipitationFromWaterFull,
												   thePrecipitationFromDrizzleFull,
												   thePrecipitationFromSleetFull,
												   thePrecipitationFromSnowFull,
												   thePrecipitationFromFreezingFull,
												   thePrecipitationTypeFull);
	  }

	float rain_intesity = 0.0;
	float rain_extent = 0.0;
	float rain_form_water = 0.0;
	float rain_form_drizzle = 0.0;
	float rain_form_sleet = 0.0;
	float rain_form_snow = 0.0;
	float rain_form_freezing = 0.0;
	float rain_type_showers = 0.0;
	trend_id rain_trend(MISSING_TREND);


	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		rain_intesity = get_mean(thePrecipitationFull);
		rain_extent = get_mean(thePrecipitationExtentFull);
		rain_trend = get_precipitation_trend(thePrecipitationFull);
		if(rain_extent > 0.0 && rain_intesity > ignore_drizzle_limit)
		  {
			rain_form_water = get_mean(thePrecipitationFromWaterFull);
			rain_form_drizzle = get_mean(thePrecipitationFromDrizzleFull);
			rain_form_sleet = get_mean(thePrecipitationFromSleetFull);
			rain_form_snow = get_mean(thePrecipitationFromSnowFull);
			rain_type_showers = get_mean(thePrecipitationTypeFull);
			float min_freezing = 0.0;
			float max_freezing = 0.0;
			get_min_max(thePrecipitationFromFreezingFull, min_freezing, max_freezing);
			rain_form_freezing = max_freezing;
 		  }
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		rain_intesity = get_mean(thePrecipitationInland);
		rain_extent = get_mean(thePrecipitationExtentInland);
		rain_trend = get_precipitation_trend(thePrecipitationInland);
		if(rain_extent > 0.0 && rain_intesity > ignore_drizzle_limit)
		  {
			rain_form_water = get_mean(thePrecipitationFromWaterInland);
			rain_form_drizzle = get_mean(thePrecipitationFromDrizzleInland);
			rain_form_sleet = get_mean(thePrecipitationFromSleetInland);
			rain_form_snow = get_mean(thePrecipitationFromSnowInland);
			rain_type_showers = get_mean(thePrecipitationTypeInland);
			float min_freezing = 0.0;
			float max_freezing = 0.0;
			get_min_max(thePrecipitationFromFreezingInland, min_freezing, max_freezing);
			rain_form_freezing = max_freezing;
		  }
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		rain_intesity = get_mean(thePrecipitationCoastal);
		rain_extent = get_mean(thePrecipitationExtentCoastal);
		rain_trend = get_precipitation_trend(thePrecipitationCoastal);
		if(rain_extent > 0.0 && rain_intesity > ignore_drizzle_limit)
		  {
			rain_form_water = get_mean(thePrecipitationFromWaterCoastal);
			rain_form_drizzle = get_mean(thePrecipitationFromDrizzleCoastal);
			rain_form_sleet = get_mean(thePrecipitationFromSleetCoastal);
			rain_form_snow = get_mean(thePrecipitationFromSnowCoastal);
			rain_type_showers = get_mean(thePrecipitationTypeCoastal);
			float min_freezing = 0.0;
			float max_freezing = 0.0;
			get_min_max(thePrecipitationFromFreezingCoastal, min_freezing, max_freezing);
			rain_form_freezing = max_freezing;
		  }
	  }

	const unsigned int rain_form = get_complete_rain_form(theParameters.theVariable,
														  rain_form_water,
														  rain_form_drizzle,
														  rain_form_sleet,
														  rain_form_snow,
														  rain_form_freezing);

	item = new PrecipitationDataItem(rain_form, 
									 rain_intesity, 
									 rain_extent,
									 rain_form_water, 
									 rain_form_drizzle, 
									 rain_form_sleet,
									 rain_form_snow, 
									 rain_form_freezing,
									 rain_type_showers,
									 rain_trend);

	return item;
  }


  void analyze_precipitation_data(wf_story_params& theParameters)
  {
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	for(unsigned int i = 1; i <= generator.size(); i++)
	  {
		precipitation_data_item_container* resultContainer = new precipitation_data_item_container();

		WeatherPeriod theWeatherPeriod =  generator.period(i);
		WeatherPeriod theSubPeriod(theWeatherPeriod);

		if(generator.isday(i))
		  {
			if(get_part_of_the_day(theWeatherPeriod, AAMUPAIVA, theSubPeriod))
			  {
				PrecipitationDataItem* item = get_precipitation_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(AAMUPAIVA, item));
			  }
			if(get_part_of_the_day(theWeatherPeriod, ILTAPAIVA, theSubPeriod))
			  {
				PrecipitationDataItem* item = get_precipitation_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(ILTAPAIVA, item));
			  }
		  }
		else
		  {
			if(get_part_of_the_day(theWeatherPeriod, YO, theSubPeriod))
			  {
				PrecipitationDataItem* item = get_precipitation_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(YO, item));
			  }
		  }
		theParameters.thePrecipitationData.push_back(resultContainer);
	  }
  }

  Sentence precipitation_sentence(wf_story_params& theParameters,
								  const unsigned int& theRainForm,
								  const float theRainIntensity,
								  const float theRainExtent,
								  const float theRainFormWater,
								  const float theRainFormDrizzle,
								  const float theRainFormSleet,
								  const float theRainFormSnow,
								  const float theRainFormFreezing,
								  const float theRainTypeShowers)
  {
	Sentence sentence;

	//	const double weak_rain_lower_limit = optional_double(theParameters.theVariable + "::weak_rain_limit",0.04);
	//	const double moderate_rain_lower_limit = optional_double(theParameters.theVariable + "::moderate_rain_limit",0.24);
	//	const double hard_rain_lower_limit = optional_double(theParameters.theVariable + "::hard_rain_limit",2.0);
	const double freezing_rain_limit = optional_double(theParameters.theVariable + "::freezing_rain_limit", 10.0);

	const int shower_limit = optional_percentage(theParameters.theVariable + "::shower_limit",80);
	//	const double extensive_rain_limit = optional_double(theParameters.theVariable + "::extensive_rain_limit", 90.0);
	//	const double in_many_places_limit = optional_double(theParameters.theVariable + "::in_many_places_limit", 50.0);
	//const double in_some_places_limit = optional_double(theParameters.theVariable + "::in_some_places_limit", 10.0);

	const double weak_rain_limit = 0.4;
	const double hard_rain_limit = 2.0;
	//	const double weak_sleet_limit = 0.4;
	//const double hard_sleet_limit = 1.7;
	const double weak_snow_limit = 0.4;
	const double hard_snow_limit = 1.5;
	//const double drizzle_limit = 0.02;
	const double ignore_rain_limit = 0.04;
	const double ignore_snow_limit = 0.02;
	const double ignore_sleet_limit = 0.02;
	const double ignore_drizzle_limit = 0.02;

	bool report_cloudiness = true;

	if(theRainExtent == 0.0 || theRainIntensity < ignore_rain_limit || theRainForm == MISSING_RAIN_FORM)
	  {
		sentence << SAA_ON_POUTAINEN_PHRASE;
	  }
	else
	  {
		//const bool isSummer = SeasonTools::isSummer(theParameters.thePeriod.localStartTime(), theParameters.theVariable);

		const bool has_showers = (theRainTypeShowers != kFloatMissing && theRainTypeShowers >= shower_limit);
		const bool mostly_dry_weather = theRainExtent <= 10.0; // enimm‰kseen poutaa
		const bool in_some_places = theRainExtent <= 50.0 && theRainExtent > 10.0;
		const bool in_many_places = theRainExtent > 50.0 && theRainExtent < 90.0;
		  		
		bool can_be_freezing =  theRainFormFreezing > freezing_rain_limit;

		if(has_showers)
		  ; // report the cloudiness


		switch(theRainForm)
		  {
		  case WATER_FREEZING_FORM:
		  case FREEZING_FORM:
		  case WATER_FORM:
			{
			  if(theRainIntensity >= ignore_rain_limit)
				{
				  theParameters.theLog << "Rain type is WATER" << endl;
 
				  if(mostly_dry_weather)
					{
					  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
					  if(has_showers)
						sentence << YKSITTAISET_SADEKUUROT_MAHDOLLISIA;
					}
				  else
					{
					  sentence << (in_some_places ? PAIKOIN_WORD : (in_many_places ? MONIN_PAIKOIN_WORD : ""));

					  if(has_showers)
						{
						  if(theRainIntensity < weak_rain_limit)
							sentence << HEIKKOJA_SADEKUUROJA_PHRASE;
						  else if(theRainIntensity >= hard_rain_limit)
							sentence << VOIMAKKAITA_SADEKUUROJA_PHRASE;
						  else
							sentence << SADEKUUROJA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE;
						}
					  else
						{
						  // TODO: check wheather it is summer
						  if(theRainIntensity < weak_rain_limit)
							sentence << HEIKKOA_SADETTA_PHRASE;
						  else if(theRainIntensity >= hard_rain_limit)
							sentence << RUNSASTA_SADETTA_PHRASE;
						  else
							sentence << SADETTA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;

						  report_cloudiness = false;
						}
					}
				}
			  else
				{
				  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
				}
			  break;
			}

		  case SLEET_FREEZING_FORM:
		  case SLEET_FORM:
			{
			  if(theRainIntensity >= ignore_sleet_limit)
				{
				  theParameters.theLog << "Rain type is SLEET" << endl;

				  if(mostly_dry_weather)
					{
					  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
					  if(has_showers)
						sentence << YKSITTAISET_RANTAKUUROT_MAHDOLLISIA;
					}
				  else
					{
					  sentence << (in_some_places ? PAIKOIN_WORD : (in_many_places ? MONIN_PAIKOIN_WORD : ""));

					  if(has_showers)
						{						
						  sentence << RANTAKUUROJA_WORD;
						}
					  else
						{
						  sentence << RANTASADETTA_WORD;
						  report_cloudiness = false;
						}
					}
				}
			  else
				{
				  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
				}
			  break;
			}
		  case SNOW_FORM:
		  case SNOW_FREEZING_FORM:
			{
			  if(theRainIntensity >= ignore_snow_limit)
				{
				  theParameters.theLog << "Rain type is SNOW" << endl;

				  if(mostly_dry_weather)
					{
					  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
					  if(has_showers)
						sentence << YKSITTAISET_LUMIKUUROT_MAHDOLLISIA;
					}
				  else
					{
					  sentence << (in_some_places ? PAIKOIN_WORD : (in_many_places ? MONIN_PAIKOIN_WORD : ""));

					  if(has_showers)
						{
						  if(theRainIntensity < weak_snow_limit)
							sentence << HEIKKOJA_LUMIKUUROJA_PHRASE;
						  else if(theRainIntensity >= hard_snow_limit)
							sentence << SAKEITA_LUMIKUUROJA_PHRASE;
						  else
							sentence << LUMIKUUROJA_WORD;
						}
					  else
						{
						  if(theRainIntensity < weak_snow_limit)
							sentence << HEIKKOA_LUMISADETTA_PHRASE;
						  else if(theRainIntensity >= hard_snow_limit)
							sentence << SAKEAA_LUMISADETTA_PHRASE;
						  else
							sentence << LUMISADETTA_WORD;

						  report_cloudiness = false;
						}
					}
				}
			  else
				{
				  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
				}
			  break;
			}

		  case DRIZZLE_FORM:
		  case DRIZZLE_FREEZING_FORM:

			{
			  if(theRainIntensity >= ignore_drizzle_limit)
				{
				  theParameters.theLog << "Rain type is DRIZZLE" << endl;

				  if(mostly_dry_weather)
					{
					  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
					  if(has_showers)
						sentence << YKSITTAISET_SADEKUUROT_MAHDOLLISIA;
					}
				  else
					{
					  sentence << (in_some_places ? PAIKOIN_WORD : (in_many_places ? MONIN_PAIKOIN_WORD : ""));

					  if(has_showers)
						{
						  sentence << SADEKUUROJA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE;
						}
					  else
						{
						  sentence << TIHKUSADETTA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;

						  report_cloudiness = false;
						}
					}
				}
			  else
				{
				  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
				}
			  break;
			}

		  case WATER_DRIZZLE_FREEZING_FORM:
		  case WATER_DRIZZLE_FORM:
			{
			  if(theRainIntensity >= ignore_drizzle_limit)
				{
				  theParameters.theLog << "Rain type is WATER_DRIZZLE" << endl;

				  if(mostly_dry_weather)
					{
					  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
					  if(has_showers)
						{
						  sentence << YKSITTAISET_SADEKUUROT_MAHDOLLISIA;
						}
					}
				  else
					{
					  sentence << (in_some_places ? PAIKOIN_WORD : (in_many_places ? MONIN_PAIKOIN_WORD : ""));

					  if(has_showers)
						{
						  sentence << SADEKUUROJA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE;
						}
					  else
						{
						  if(theRainFormWater >= theRainFormDrizzle)
							sentence << VESISADETTA_WORD;
						  else
							sentence << TIHKUSADETTA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;

						  report_cloudiness = false;
						}
					}
				}
			  else
				{
				  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
				}
			  break;
			}

		  case DRIZZLE_SLEET_FORM:
		  case DRIZZLE_SLEET_FREEZING_FORM:
		  case WATER_DRIZZLE_SLEET_FORM:
		  case WATER_SLEET_FREEZING_FORM:
		  case WATER_SLEET_FORM:
			{
			  if(theRainIntensity >= ignore_sleet_limit)
				{
				  theParameters.theLog << "Rain type is WATER_SLEET" << endl;

				  if(mostly_dry_weather)
					{
					  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
					  if(has_showers)
						{
						  if(theRainFormWater >= theRainFormSleet)
							sentence << YKSITTAISET_VESI_RANTA_KUUROT_MAHDOLLISIA;
						  else
							sentence << YKSITTAISET_RANTA_VESI_KUUROT_MAHDOLLISIA;
						}
					}
				  else
					{
					  sentence << (in_some_places ? PAIKOIN_WORD : (in_many_places ? MONIN_PAIKOIN_WORD : ""));

					  if(has_showers)
						{
						  if(theRainFormWater >= theRainFormSleet)
							sentence << VESI_RANTA_KUUROJA;
						  else
							sentence << RANTA_VESI_KUUROJA;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE;
						}
					  else
						{
						  if(theRainFormWater >= theRainFormSleet)
							sentence << VESI_TAVUVIIVA_WORD << TAI_WORD << RANTASADETTA_WORD;
						  else
							sentence << RANTA_TAVUVIIVA_WORD << TAI_WORD << VESISADETTA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;

						  report_cloudiness = false;
						}
					}
				}
			  else
				{
				  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
				}
			  break;
			}

		  case WATER_SNOW_FORM:
		  case DRIZZLE_SNOW_FORM:
		  case WATER_DRIZZLE_SNOW_FORM:
		  case WATER_SLEET_SNOW_FORM:
		  case DRIZZLE_SNOW_FREEZING_FORM:
		  case WATER_SNOW_FREEZING_FORM:
			{
			  if(theRainIntensity >= ignore_drizzle_limit)
				{
				  if(theRainForm == (WATER_FORM & SNOW_FORM & SLEET_FORM))
					theParameters.theLog << "Rain type is WATER_SNOW_SLEET" << endl;
				  else
					theParameters.theLog << "Rain type is WATER_SNOW" << endl;

				  if(mostly_dry_weather)
					{
					  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;

					  if(has_showers)
						sentence << YKSITTAISET_VESI_LUMI_KUUROT_MAHDOLLISIA;
					}
				  else
					{
					  sentence << (in_some_places ? PAIKOIN_WORD : (in_many_places ? MONIN_PAIKOIN_WORD : ""));

					  if(has_showers)
						{
						  if(theRainIntensity < weak_snow_limit)
							sentence << VESI_TAVUVIIVA_WORD << TAI_WORD << LUMIKUUROJA_WORD;
						  else if(theRainIntensity >= hard_snow_limit)
							sentence << VOIMAKKAITA_SADEKUUROJA_PHRASE << TAI_WORD << SAKEITA_LUMIKUUROJA_PHRASE;
						  else
							sentence << VESI_TAVUVIIVA_WORD << TAI_WORD << LUMIKUUROJA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE;
						}
					  else
						{
						  if(theRainIntensity < weak_snow_limit)
							sentence << HEIKKOA_WORD;
						  else if(theRainIntensity >= hard_snow_limit)
							sentence << KOVAA_WORD;
						  if(theRainFormWater >= theRainFormSnow)
							sentence << VESI_TAVUVIIVA_WORD << TAI_WORD << LUMISADETTA_WORD;
						  else
							sentence << LUMI_TAVUVIIVA_WORD << TAI_WORD << VESISADETTA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;

						  report_cloudiness = false;
						}
					}
				}
			  else
				{
				  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
				}
			  break;
			}

		  case DRIZZLE_SLEET_SNOW_FORM:
		  case SLEET_SNOW_FREEZING_FORM:
		  case SLEET_SNOW_FORM:
			{
			  if(theRainIntensity >= ignore_sleet_limit)
				{
				  theParameters.theLog << "Rain type is SNOW_SLEET" << endl;

				  if(mostly_dry_weather)
					{
					  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
					  if(has_showers)
						sentence << YKSITTAISET_LUMI_RANTA_KUUROT_MAHDOLLISIA;
					}
				  else
					{
					  sentence << (in_some_places ? PAIKOIN_WORD : (in_many_places ? MONIN_PAIKOIN_WORD : ""));

					  if(has_showers)
						{
						  if(theRainIntensity < weak_snow_limit)
							sentence << RANTA_TAVUVIIVA_WORD << TAI_WORD << LUMIKUUROJA_WORD;
						  else if(theRainIntensity >= hard_snow_limit)
							sentence << SAKEITA_LUMIKUUROJA_PHRASE << TAI_WORD << RANTASADETTA_WORD;
						  else
							sentence << RANTA_TAVUVIIVA_WORD << TAI_WORD << LUMIKUUROJA_WORD;
						}
					  else
						{
						  if(theRainIntensity < weak_snow_limit)
							sentence << HEIKKOA_LUMISADETTA_PHRASE << TAI_WORD << RANTASADETTA_WORD;
						  else if(theRainIntensity >= hard_snow_limit)
							sentence << SAKEAA_LUMISADETTA_PHRASE << TAI_WORD << RANTASADETTA_WORD;
						  else
							{
							  if(theRainFormSnow >= theRainFormSleet)
								sentence << LUMI_TAVUVIIVA_WORD << TAI_WORD << RANTASADETTA_WORD;
							  else
								sentence << RANTA_TAVUVIIVA_WORD << TAI_WORD << VESISADETTA_WORD;
							}

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;

						  report_cloudiness = false;
						}
					}
				}
			  else
				{
				  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
				}
			  break;
			}
		  }
	  }

	theParameters.theReportCloudiness = report_cloudiness;

	return sentence;
  }


  Sentence precipitation_change_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	if(generator.size() < thePeriodNumber)
	  return sentence;

	trend_id theTrendMorning(MISSING_TREND);
	trend_id theTrendAfternoon(MISSING_TREND);
	trend_id theTrendNight(MISSING_TREND);

	precipitation_data_item_container* theResultData = theParameters.thePrecipitationData[thePeriodNumber - 1];

	if(theResultData->find(AAMUPAIVA) != theResultData->end())
	  theTrendMorning = ((*theResultData)[AAMUPAIVA])->theTrendId;
	if(theResultData->find(ILTAPAIVA) != theResultData->end())
	  theTrendAfternoon = ((*theResultData)[ILTAPAIVA])->theTrendId;
	if(theResultData->find(YO) != theResultData->end())
	  theTrendNight = ((*theResultData)[YO])->theTrendId;

	if(theTrendNight == MISSING_TREND && theTrendMorning == POUTAANTUU && theTrendAfternoon == MISSING_TREND)
	  sentence << AAMUPAIVALLA_WORD << SAA_POUTAANTUU_PHRASE;
	else if(theTrendNight == MISSING_TREND && theTrendMorning == MISSING_TREND && theTrendAfternoon == POUTAANTUU)
	  sentence << ILTAPAIVALLA_WORD << SAA_POUTAANTUU_PHRASE;

	return sentence;
  }

  Sentence handle_morning_afternoon_sentence(Sentence& theMorningSentence, Sentence& theAfternoonSentence)
  {
	Sentence sentence;

	// TODO: is this enought or should we examine the content?
	if(theMorningSentence.size() == theAfternoonSentence.size())
	  {
		sentence << theAfternoonSentence;
	  }
	else
	  {
		if(!theMorningSentence.empty())
		  sentence << AAMUPAIVALLA_WORD << theMorningSentence;

		if(!theMorningSentence.empty() && !theAfternoonSentence.empty())
		  sentence << Delimiter(",");

		if(!theAfternoonSentence.empty())
		  if(!theMorningSentence.empty())
			sentence << ILTAPAIVALLA_WORD << theAfternoonSentence;
		else
			sentence << theAfternoonSentence;
	  }

	return sentence;
  }

  Sentence precipitation_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	if(generator.size() < thePeriodNumber)
	  return sentence;

	PrecipitationDataItem* morningDataItem = 0;
	PrecipitationDataItem* afternoonDataItem = 0;
	PrecipitationDataItem* nightDataItem = 0;

	precipitation_data_item_container* theResultData = theParameters.thePrecipitationData[thePeriodNumber - 1];

	if(theResultData->find(AAMUPAIVA) != theResultData->end())
		morningDataItem = (*theResultData)[AAMUPAIVA];
	if(theResultData->find(ILTAPAIVA) != theResultData->end())
		afternoonDataItem = (*theResultData)[ILTAPAIVA];
	if(theResultData->find(YO) != theResultData->end())
		nightDataItem = (*theResultData)[YO];


	Sentence morningSentence;
	Sentence afternoonSentence;

	/*
	if(morningDataItem && afternoonDataItem && (*morningDataItem == *afternoonDataItem))
	  {
		sentence << precipitation_sentence(theParameters,
										   afternoonDataItem->theRainForm,
										   afternoonDataItem->theRainIntensity,
										   afternoonDataItem->theRainExtent,
										   afternoonDataItem->theRainFormWater,
										   afternoonDataItem->theRainFormDrizzle,
										   afternoonDataItem->theRainFormSleet,
										   afternoonDataItem->theRainFormSnow,
										   afternoonDataItem->theRainFormFreezing,
										   afternoonDataItem->theRainTypeShowers);
		return sentence;
	  }
	*/

	if(morningDataItem)
	  morningSentence << precipitation_sentence(theParameters,
												morningDataItem->theRainForm,
												morningDataItem->theRainIntensity,
												morningDataItem->theRainExtent,
												morningDataItem->theRainFormWater,
												morningDataItem->theRainFormDrizzle,
												morningDataItem->theRainFormSleet,
												morningDataItem->theRainFormSnow,
												morningDataItem->theRainFormFreezing,
												morningDataItem->theRainTypeShowers);

	if(afternoonDataItem)
	  afternoonSentence << precipitation_sentence(theParameters,
												  afternoonDataItem->theRainForm,
												  afternoonDataItem->theRainIntensity,
												  afternoonDataItem->theRainExtent,
												  afternoonDataItem->theRainFormWater,
												  afternoonDataItem->theRainFormDrizzle,
												  afternoonDataItem->theRainFormSleet,
												  afternoonDataItem->theRainFormSnow,
												  afternoonDataItem->theRainFormFreezing,
												  afternoonDataItem->theRainTypeShowers);

	sentence << handle_morning_afternoon_sentence(morningSentence, afternoonSentence);

	if(nightDataItem)
	  sentence << precipitation_sentence(theParameters,
										 nightDataItem->theRainForm,
										 nightDataItem->theRainIntensity,
										 nightDataItem->theRainExtent,
										 nightDataItem->theRainFormWater,
										 nightDataItem->theRainFormDrizzle,
										 nightDataItem->theRainFormSleet,
										 nightDataItem->theRainFormSnow,
										 nightDataItem->theRainFormFreezing,
										 nightDataItem->theRainTypeShowers);

	return sentence;
  }

  ThunderDataItem* get_thunder_data_item(wf_story_params& theParameters, const WeatherPeriod& thePeriod)
  {
	ThunderDataItem* item = 0;

	weather_result_data_item_vector theThunderProbabilityData;

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[FULL_AREA])[THUNDERPROBABILITY_DATA],
							theThunderProbabilityData);
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[INLAND_AREA])[THUNDERPROBABILITY_DATA],
							theThunderProbabilityData);
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[COASTAL_AREA])[THUNDERPROBABILITY_DATA],
							theThunderProbabilityData);
	  }

	float min_probability = kFloatMissing;
	float mean_probability = kFloatMissing;
	float max_probability = kFloatMissing;
	float standard_deviation_probability = kFloatMissing;

	mean_probability = get_mean(theThunderProbabilityData);
	get_min_max(theThunderProbabilityData, min_probability, max_probability);
	standard_deviation_probability = get_standard_deviation(mean_probability, theThunderProbabilityData);
	

	item = new ThunderDataItem(min_probability,
							   mean_probability,
							   max_probability,
							   standard_deviation_probability);

	return item;
  }

  void analyze_thunder_data(wf_story_params& theParameters)
  {
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	for(unsigned int i = 1; i <= generator.size(); i++)
	  {
		thunder_data_item_container* resultContainer = new thunder_data_item_container();

		WeatherPeriod theWeatherPeriod =  generator.period(i);
		WeatherPeriod theSubPeriod(theWeatherPeriod);

		if(generator.isday(i))
		  {
			if(get_part_of_the_day(theWeatherPeriod, AAMUPAIVA, theSubPeriod))
			  {
				ThunderDataItem* item = get_thunder_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(AAMUPAIVA, item));
			  }
			if(get_part_of_the_day(theWeatherPeriod, ILTAPAIVA, theSubPeriod))
			  {
				ThunderDataItem* item = get_thunder_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(ILTAPAIVA, item));
			  }
		  }
		else
		  {
			if(get_part_of_the_day(theWeatherPeriod, YO, theSubPeriod))
			  {
				ThunderDataItem* item = get_thunder_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(YO, item));
			  }
		  }
		theParameters.theThunderData.push_back(resultContainer);
	  }
  }

  Sentence thunder_sentence(wf_story_params& theParameters,
							const float& theMinProbability,
							const float& theMeanProbability,
							const float& theMaxProbability,
							const float& theStandardDeviationProbability)
  {
	Sentence sentence;

	if(theMaxProbability == 0)
	  return sentence;

	if(theMaxProbability <= 15.0)
	  {
		sentence << PIENI_UKKOSEN_TODENNAKOISYYS_PHRASE;
	  }
	else if(theMaxProbability <= 35.0)
	  {
		sentence << MAHDOLLISESTI_UKKOSTA_PHRASE;
	  }
	else
	  {
		sentence << AJOITTAIN_UKKOSTA_PHRASE;
	  }

	return sentence;
  }

  Sentence thunder_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	ThunderDataItem* morningDataItem = 0;
	ThunderDataItem* afternoonDataItem = 0;
	ThunderDataItem* nightDataItem = 0;

	thunder_data_item_container* theResultData = theParameters.theThunderData[thePeriodNumber - 1];

	if(theResultData->find(AAMUPAIVA) != theResultData->end())
		morningDataItem = (*theResultData)[AAMUPAIVA];
	if(theResultData->find(ILTAPAIVA) != theResultData->end())
		afternoonDataItem = (*theResultData)[ILTAPAIVA];
	if(theResultData->find(YO) != theResultData->end())
		nightDataItem = (*theResultData)[YO];

	Sentence morningSentence;
	Sentence afternoonSentence;
	Sentence nightSentence;

	if(morningDataItem)
	  morningSentence << thunder_sentence(theParameters,
										  morningDataItem->theMinProbability,
										  morningDataItem->theMeanProbability,
										  morningDataItem->theMaxProbability,
										  morningDataItem->theStandardDeviationProbability);
	
	if(afternoonDataItem)
	  afternoonSentence << thunder_sentence(theParameters,
											afternoonDataItem->theMinProbability,
											afternoonDataItem->theMeanProbability,
											afternoonDataItem->theMaxProbability,
											afternoonDataItem->theStandardDeviationProbability);

	if(nightDataItem)
	  nightSentence << thunder_sentence(theParameters,
										nightDataItem->theMinProbability,
										nightDataItem->theMeanProbability,
										nightDataItem->theMaxProbability,
										nightDataItem->theStandardDeviationProbability);

	sentence << handle_morning_afternoon_sentence(morningSentence, afternoonSentence);

	sentence << nightSentence;

	return sentence;
  }

  /*

  Sentence cloudiness_sentence(wf_story_params& theParameters, const WeatherPeriod& thePeriod)
  {
	Sentence sentence;

	float min = -1.0;
	float mean = -1.0;
	float max = -1.0;   
	float standard_deviation = -1.0;

	weather_result_data_item_vector theInterestingData;

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[FULL_AREA])[CLOUDINESS_DATA],
							theInterestingData);
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[INLAND_AREA])[CLOUDINESS_DATA],
							theInterestingData);
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		get_sub_time_series(theParameters,
							thePeriod,
							*(*theParameters.theCompleteData[COASTAL_AREA])[CLOUDINESS_DATA],
							theInterestingData);
	  }

	if(theInterestingData.size() > 0)
	  {
		get_min_max(theInterestingData, min, max);
		mean = get_mean(theInterestingData);
		standard_deviation = get_standard_deviation(mean, theInterestingData);

		cloudiness_id id = get_cloudiness_id(min, mean, max, standard_deviation);

		sentence << cloudiness_sentence(id);
	  }

	return sentence;
  }


  */


  void allocate_data_structures(wf_story_params& theParameters, const forecast_area_id& theAreaId)
  {
	weather_result_data_item_vector* hourlyTemperature = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitation = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationExtent = new weather_result_data_item_vector();

	weather_result_data_item_vector* hourlyPrecipitationType = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormWater = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormDrizzle = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormSleet = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormSnow = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormFreezing = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyCloudiness = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyThunderProbability = new weather_result_data_item_vector();

	// first split the whole period to one-hour subperiods
	NFmiTime periodStartTime = theParameters.thePeriod.localStartTime();

	theParameters.theHourPeriodCount = 0;
	while(periodStartTime.IsLessThan(theParameters.thePeriod.localEndTime()))
	  {
		NFmiTime periodEndTime = periodStartTime;
		periodEndTime.ChangeByHours(1);
		WeatherPeriod theWeatherPeriod(periodEndTime, periodEndTime);
		//		WeatherPeriod theWeatherPeriod(periodStartTime, periodEndTime);
		WeatherResult theWeatherResult(kFloatMissing, 0);

		part_of_the_day_id  partOfTheDayId = get_part_of_the_day_id(theWeatherPeriod);

		hourlyTemperature->push_back(new WeatherResultDataItem(theWeatherPeriod, 
															   theWeatherResult, 
															   partOfTheDayId));
		hourlyPrecipitation->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																 theWeatherResult, 
																 partOfTheDayId));
		hourlyPrecipitationExtent->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																	   theWeatherResult, 
																	   partOfTheDayId));
		hourlyPrecipitationType->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																	 theWeatherResult, 
																	 partOfTheDayId));
		hourlyPrecipitationFormWater->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																		  theWeatherResult, 
																		  partOfTheDayId));
		hourlyPrecipitationFormDrizzle->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			theWeatherResult, 
																			partOfTheDayId));
		hourlyPrecipitationFormSleet->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																		  theWeatherResult, 
																		  partOfTheDayId));
		hourlyPrecipitationFormSnow->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																		 theWeatherResult, 
																		 partOfTheDayId));
		hourlyPrecipitationFormFreezing->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			 theWeatherResult, 
																			 partOfTheDayId));
		hourlyCloudiness->push_back(new WeatherResultDataItem(theWeatherPeriod, 
															  theWeatherResult, 
															  partOfTheDayId));
		hourlyThunderProbability->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																	  theWeatherResult, 
																	  partOfTheDayId));

		periodStartTime.ChangeByHours(1);
		theParameters.theHourPeriodCount++;
	  }

	weather_forecast_result_container* resultContainer = new weather_forecast_result_container();

	resultContainer->insert(make_pair(TEMPERATURE_DATA, hourlyTemperature));
	resultContainer->insert(make_pair(PRECIPITATION_DATA, hourlyPrecipitation));
	resultContainer->insert(make_pair(PRECIPITATION_EXTENT_DATA, hourlyPrecipitationExtent));
	resultContainer->insert(make_pair(PRECIPITATION_TYPE_DATA, hourlyPrecipitationType));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_WATER_DATA, hourlyPrecipitationFormWater));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_DRIZZLE_DATA, hourlyPrecipitationFormDrizzle));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_SLEET_DATA, hourlyPrecipitationFormSleet));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_SNOW_DATA, hourlyPrecipitationFormSnow));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_FREEZING_DATA, hourlyPrecipitationFormFreezing));
	resultContainer->insert(make_pair(CLOUDINESS_DATA, hourlyCloudiness));
	resultContainer->insert(make_pair(THUNDERPROBABILITY_DATA, hourlyThunderProbability));
	
	theParameters.theCompleteData.insert(make_pair(theAreaId, resultContainer));
  }

  void create_data_structures(wf_story_params& theParameters)
  {
	// first check which areas exists and create the data structures accordinly
	WeatherArea fullArea = theParameters.theArea;
	WeatherArea inlandArea = theParameters.theArea;
	inlandArea.type(WeatherArea::Inland);
	WeatherArea coastalArea = theParameters.theArea;
	coastalArea.type(WeatherArea::Coast);
	bool inlandExists = false;
	bool coastExists = false;
 	GridForecaster theForecaster;
	WeatherResult result = theForecaster.analyze(theParameters.theVariable,
												 theParameters.theSources,
												 Temperature,
												 Mean,
												 Maximum,
												 inlandArea,
												 theParameters.thePeriod);
	if(result.value() != kFloatMissing)
	  {
		inlandExists = true;
	  }
	
	result = theForecaster.analyze(theParameters.theVariable,
								   theParameters.theSources,
								   Temperature,
								   Mean,
								   Maximum,
								   coastalArea,
								   theParameters.thePeriod);
	if(result.value() != kFloatMissing)
	  {
		coastExists = true;
	  }

	if(inlandExists && coastExists)
	  {
		// create data structures for inland, coast and full areas
		allocate_data_structures(theParameters, INLAND_AREA);
		allocate_data_structures(theParameters, COASTAL_AREA);
		allocate_data_structures(theParameters, FULL_AREA);
		theParameters.theForecastArea |= INLAND_AREA;
		theParameters.theForecastArea |= COASTAL_AREA;
		theParameters.theForecastArea |= FULL_AREA;

		theParameters.theLog << "inland area included" << endl;
		theParameters.theLog << "coastal area included" << endl;
		theParameters.theLog << "full area included" << endl;

	  }
	else if(inlandExists)
	  {
		// create data structures for inland only
		allocate_data_structures(theParameters, INLAND_AREA);
		theParameters.theForecastArea |= INLAND_AREA;
		theParameters.theLog << "inland area included" << endl;
	  }
	else if(coastExists)
	  {
		// create data structures for coast only
		allocate_data_structures(theParameters, COASTAL_AREA);
		theParameters.theForecastArea |= COASTAL_AREA;
		theParameters.theLog << "coastal area included" << endl;
	  }
	else
	  {
		// error not inland, nor coastal area included
	  }
  }

  void deallocate_data_structure(unsigned int& thePeriodCount, weather_forecast_result_container& theResultContainer)
  {
	for(unsigned int i = 0; i < thePeriodCount; i++)
	  {
		delete (*theResultContainer[TEMPERATURE_DATA])[i];
		delete (*theResultContainer[CLOUDINESS_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_EXTENT_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_TYPE_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_WATER_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_DRIZZLE_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_SLEET_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_SNOW_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_FREEZING_DATA])[i];
		delete (*theResultContainer[THUNDERPROBABILITY_DATA])[i];
	  }

	theResultContainer[TEMPERATURE_DATA]->clear();
	theResultContainer[PRECIPITATION_DATA]->clear();
	theResultContainer[PRECIPITATION_EXTENT_DATA]->clear();
	theResultContainer[PRECIPITATION_TYPE_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_WATER_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_DRIZZLE_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_SLEET_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_SNOW_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_FREEZING_DATA]->clear();
	theResultContainer[THUNDERPROBABILITY_DATA]->clear();

	delete theResultContainer[TEMPERATURE_DATA];
	delete theResultContainer[CLOUDINESS_DATA];
	delete theResultContainer[PRECIPITATION_DATA];
	delete theResultContainer[PRECIPITATION_EXTENT_DATA];
	delete theResultContainer[PRECIPITATION_TYPE_DATA];
	delete theResultContainer[PRECIPITATION_FORM_WATER_DATA];
	delete theResultContainer[PRECIPITATION_FORM_DRIZZLE_DATA];
	delete theResultContainer[PRECIPITATION_FORM_SLEET_DATA];
	delete theResultContainer[PRECIPITATION_FORM_SNOW_DATA];
	delete theResultContainer[PRECIPITATION_FORM_FREEZING_DATA];

	theResultContainer.clear();
  }

  void delete_data_structures(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		deallocate_data_structure(theParameters.theHourPeriodCount, *theParameters.theCompleteData[INLAND_AREA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		deallocate_data_structure(theParameters.theHourPeriodCount, *theParameters.theCompleteData[COASTAL_AREA]);
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		deallocate_data_structure(theParameters.theHourPeriodCount, *theParameters.theCompleteData[FULL_AREA]);
	  }

	for(unsigned int i = 0; i < theParameters.theCloudinessData.size(); i++)
	  {
		cloudiness_data_item_container* cloudinessDataItemContainer = theParameters.theCloudinessData[i];
		for(unsigned int k = 0; k < cloudinessDataItemContainer->size(); k++)
		  delete (*cloudinessDataItemContainer)[k];
		cloudinessDataItemContainer->clear();
	  }
	theParameters.theCloudinessData.clear();
	for(unsigned int i = 0; i < theParameters.thePrecipitationData.size(); i++)
	  {
		precipitation_data_item_container* precipitationDataItemContainer = theParameters.thePrecipitationData[i];
		for(unsigned int k = 0; k < precipitationDataItemContainer->size(); k++)
		  delete (*precipitationDataItemContainer)[k];
		precipitationDataItemContainer->clear();
	  }
	theParameters.thePrecipitationData.clear();

	for(unsigned int i = 0; i < theParameters.theThunderData.size(); i++)
	  {
		thunder_data_item_container* thunderDataItemContainer = theParameters.theThunderData[i];
		for(unsigned int k = 0; k < thunderDataItemContainer->size(); k++)
		  delete (*thunderDataItemContainer)[k];
		thunderDataItemContainer->clear();
	  }
	theParameters.theThunderData.clear();
  }


// ----------------------------------------------------------------------
  /*!populate_temperature_time_series
   * \brief Generate overview on weather
   *
   * \return The story
   *
   * \see page_weather_forecast
   *
   * \todo Much missing
   */
  // ----------------------------------------------------------------------

  const Paragraph WeatherStory::forecast() const
  {
	MessageLogger log("WeatherStory::forecast");

	using namespace PrecipitationPeriodTools;

	Paragraph paragraph;

	// Period generator
	NightAndDayPeriodGenerator generator00(itsPeriod, itsVar);

	if(generator00.size() == 0)
	  {
		log << "No weather periods available!" << endl;
		log << paragraph;
		return paragraph;
	  }

	NFmiTime periodStartTime(itsPeriod.localStartTime());
	NFmiTime periodEndTime(itsPeriod.localEndTime());

	const int day_starthour = optional_hour(itsVar+"::day::starthour",6);

	if(periodStartTime.GetHour() > day_starthour)
	  {
		periodStartTime.ChangeByHours(-1*(periodStartTime.GetHour() - day_starthour));
		if(generator00.isday(1))
		  periodStartTime.ChangeByHours(-24);
	  }
	else if(periodStartTime.GetHour() < day_starthour)
	  {
		periodStartTime.ChangeByHours(day_starthour - periodStartTime.GetHour());
		periodStartTime.ChangeByHours(-24);
	  }

	WeatherPeriod theExtendedPeriod(periodStartTime, periodEndTime);

	wf_story_params theParameters(itsVar,
								  itsArea,
								  theExtendedPeriod,
								  itsSources,
								  log);


	if(itsArea.isNamed())
	  {
		std::string name(itsArea.name());
		log << "** " << name  << " **" << endl;
	  }

	create_data_structures(theParameters);

	if(theParameters.theForecastArea == NO_AREA)
	  return paragraph;

	populate_temperature_time_series(theParameters);
	populate_cloudiness_time_series(theParameters);
	populate_thunderprobability_time_series(theParameters);
	populate_precipitation_time_series(theParameters);

	analyze_cloudiness_data(theParameters);
	analyze_precipitation_data(theParameters);
	analyze_thunder_data(theParameters);

	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	int thePeriodNumber = 3;

	Sentence cloudinessSentence;
	Sentence cloudinessChangeSentence;
	Sentence precipitationSentence;
	Sentence precipitationChangeSentence;

	cloudinessSentence << cloudiness_sentence(theParameters, thePeriodNumber);
	cloudinessChangeSentence << cloudiness_change_sentence(theParameters, thePeriodNumber);
	
	precipitationSentence << precipitation_sentence(theParameters, thePeriodNumber);
	precipitationChangeSentence << precipitation_change_sentence(theParameters, thePeriodNumber);

	if(!cloudinessChangeSentence.empty())
	  paragraph << cloudinessChangeSentence;
	else
	  paragraph << cloudinessSentence;

	if(!precipitationChangeSentence.empty())
	  paragraph << precipitationChangeSentence;
	else
	  paragraph << precipitationSentence;

	paragraph << thunder_sentence(theParameters, thePeriodNumber);

#ifdef LATER
	for(unsigned int i = 1; i <= generator.size(); i++)
	  {
		if(!(i > 1 && generator.isday(i)))
		  continue;

		Sentence sentence;

		std::string theType;
		if(i == 1)
		  {
			if(generator.isday(i))
			  theType = "";//"t‰n‰‰n";
			else
			  theType = "yˆll‰";
		  }
		else if(i == 2)
		  {
			if(generator.isday(i))
			  theType = "huomenna";
			else
			  theType = "yˆll‰";
		  }
		else if(i == 3)
		  {
			if(generator.isday(i))
			  theType = "huomenna";
			else
			  theType = "seuraavana yˆn‰";
		  }
		else
		  {
			theType = "remaining_days";
		  }
		/*
		sentence << theType;
		sentence << cloudiness_sentence(theParameters, theCloudinessData, i);
		sentence << Delimiter(";");
		sentence << precipitation_sentence(theParameters, thePrecipitationData, i);
		paragraph << sentence;
		*/

		paragraph << cloudiness_sentence(theParameters, i);
		paragraph << cloudiness_change_sentence(theParameters, i);

		paragraph << precipitation_sentence(theParameters, i);
		paragraph << precipitation_change_sentence(theParameters, i);

		paragraph << thunder_sentence(theParameters, i);

	  }
#endif

	/*
	analyzed_cloudiness_data_container theCloudinessData;
	analyze_cloudiness_data(theParameters, theCloudinessData);
	
	paragraph << cloudiness_sentence(theParameters, theCloudinessData, 1);
	paragraph << cloudiness_sentence(theParameters, theCloudinessData, 2);
	paragraph << cloudiness_sentence(theParameters, theCloudinessData, 3);

	analyzed_precipitation_data_container thePrecipitationData;
	analyze_precipitation_data(theParameters, thePrecipitationData);

	paragraph << precipitation_sentence(theParameters, thePrecipitationData,1);
	paragraph << precipitation_sentence(theParameters, thePrecipitationData,2);
	paragraph << precipitation_sentence(theParameters, thePrecipitationData,3);
	*/

	/*
	Sentence precipitationSentence;

	cloudinessSentence << cloudiness_sentence(theParameters);
	precipitationSentence << precipitation_sentence(theParameters);

	if(theParameters.theReportCloudiness)
	  paragraph << cloudinessSentence;
	paragraph << precipitationSentence;

	*/

		/*
	NightAndDayPeriodGenerator generator(itsPeriod, itsVar);

	for(unsigned int i = 1; i <= generator.size(); i++)
	  {
		Sentence cloudinessSentence;
		WeatherPeriod wp =  generator.period(i);
		  WeatherPeriod wp(itsPeriod);
		  if(generator.isday(i))
		  {
		  log_start_time_and_end_time(log, "ORIGINAL DAY: ", generator.period(i));
		  if(get_part_of_the_day(generator.period(i), AAMUPAIVA, wp))
		  log_start_time_and_end_time(log, "AAMUPƒIVƒ: ", wp);
		  if(get_part_of_the_day(generator.period(i), ILTAPAIVA, wp))
		  log_start_time_and_end_time(log, "ILTAPƒIVƒ: ", wp);
		  }
		  else
		  {
		  log_start_time_and_end_time(log, "ORIGINAL NIGHT ", generator.period(i));
		  if(get_part_of_the_day(generator.period(i), YO, wp))
		  log_start_time_and_end_time(log, "Y÷: ", wp);
		  }
	  }
		*/





	/*

	paragraph << thunder_probability_sentence(theParameters);

	log_subperiods(theParameters);
	*/

	log_weather_result_data(theParameters);

	delete_data_structures(theParameters);

	log << paragraph;
	return paragraph;
  }


} // namespace TextGen

// ======================================================================
