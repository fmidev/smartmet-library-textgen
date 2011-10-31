// ======================================================================
/*!
 * \file
 * \brief Implementation of WindForecast class
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
#include "MathTools.h"
#include "SeasonTools.h"
#include "SubMaskExtractor.h"
#include "IntegerRange.h"
#include "UnitFactory.h"
#include "WindForecast.h"

#include <newbase/NFmiCombinedParam.h>
#include <newbase/NFmiMercatorArea.h>

#include <boost/lexical_cast.hpp>
#include <vector>
#include <map>

namespace TextGen
{

  using namespace Settings;
  using namespace WeatherAnalysis;
  using namespace AreaTools;
  using namespace WindStoryTools;
  using namespace boost;
  using namespace std;

#define KOVIMMILLAAN_PHRASE "kovimmillaan"
#define HEIKKENEVAA_PHRASE "heikkenevaa"
#define VOIMISTUVAA_PHRASE "voimistuvaa"
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


#define ILTAPAIVALLA_TUULI_VOIMISTUU_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] tuuli voimistuu ja kaantyy [etelaan]"
#define ILTAPAIVALLA_TUULI_HEIKKENEE_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] tuuli heikkenee ja kaantyy [etelaan]"
#define ILTAPAIVALLA_TUULI_TYYNTYY_COMPOSITE_PHRASE "[iltapaivalla] tuuli tyyntyy"
  //#define ILTAPAIVALLA_TUULI_TYYNTYY_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] tuuli tyyntyy ja kaantyy [etelaan]"
#define ILTAPAIVALLA_TUULI_VOIMISTUU_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli voimistuu ja muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_HEIKKENEE_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli heikkenee ja muuttuu vaihtelevaksi"
  //#define ILTAPAIVALLA_TUULI_TYYNTYY_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli tyyntyy ja muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] tuuli kaantyy [etelaan]"
#define ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli muuttuu tilapaisesti vaihtelevaksi"
#define ILTAPAIVALLA_ETELAAN_KAANTYVAA_TUULTA_COMPOSITE_PHRASE "[iltapaivalla] [etelaan] kaantyvaa tuulta"
#define ILTAPAIVALLA_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE "[iltapaivalla] [heikkenevaa] [etelatuulta]"
#define ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE "[iltapaivalla] [etelatuulta]"
#define ILTAPAIVALLA_HEIKKENEVAA_TUULTA_COMPOSITE_PHRASE "[iltapaivalla] [heikkenevaa tuulta]"

  wind_speed_id get_wind_speed_id(const WeatherResult& windSpeed)
  {
	if(windSpeed.value() < 0.5)
	  return TYYNI;
	else if(windSpeed.value() >= 0.5 && windSpeed.value() < 3.5)
	  return HEIKKO;
	else if(windSpeed.value() >= 3.5 && windSpeed.value() < 7.5)
	  return KOHTALAINEN;
	else if(windSpeed.value() >= 7.5 && windSpeed.value() < 13.5)
	  return NAVAKKA;
	else if(windSpeed.value() >= 13.5 && windSpeed.value() < 20.5)
	  return KOVA;
	else if(windSpeed.value() >= 20.5 && windSpeed.value() < 32.5)
	  return MYRSKY;
	else
	  return HIRMUMYRSKY;
  }

  wind_direction_id get_wind_direction_id16(const WeatherResult& windDirection, const string& var)
  {
	WindDirectionAccuracy accuracy(direction_accuracy(windDirection.error(), var));

	if(accuracy == bad_accuracy)
	  return VAIHTELEVA;
  	
	int windDirectionId = direction16th(windDirection.value());

	if(accuracy == moderate_accuracy)
	  {
		switch(windDirectionId)
		  {
		  case 1:
			{
			  windDirectionId = POHJOISEN_PUOLEINEN;
			}
			break;
		  case 2:
			{
			  if(windDirection.value() > 337.5 || windDirection.value() < 22.5)
				windDirectionId = POHJOISEN_PUOLEINEN;
			  else
				windDirectionId = KOILLISEN_PUOLEINEN;
			}
			break;
		  case 3:
			{
			  windDirectionId = KOILLISEN_PUOLEINEN;
			}
			break;
		  case 4:
			{
			  if(windDirection.value() < 67.5)
				windDirectionId = KOILLISEN_PUOLEINEN;
			  else
				windDirectionId = IDAN_PUOLEINEN;
			}
			break;
		  case 5:
			{
			  windDirectionId = IDAN_PUOLEINEN;
			}
			break;
		  case 6:
			{
			  if(windDirection.value() < 112.5)
				windDirectionId = IDAN_PUOLEINEN;
			  else
				windDirectionId = KAAKON_PUOLEINEN;
			}
			break;
		  case 7:
			{
			  windDirectionId = KAAKON_PUOLEINEN;
			}
			break;
		  case 8:
			{
			  if(windDirection.value() < 157.5)
				windDirectionId = KAAKON_PUOLEINEN;
			  else
				windDirectionId = ETELAN_PUOLEINEN;
			}
			break;
		  case 9:
			{
			  windDirectionId = ETELAN_PUOLEINEN;
			}
			break;
		  case 10:
			{
			  if(windDirection.value() < 202.5)
				windDirectionId = ETELAN_PUOLEINEN;
			  else
				windDirectionId = LOUNAAN_PUOLEINEN;
			}
			break;
		  case 11:
			{
			  windDirectionId = LOUNAAN_PUOLEINEN;
			}
			break;
		  case 12:
			{
			  if(windDirection.value() < 247.5)
				windDirectionId = LOUNAAN_PUOLEINEN;
			  else
				windDirectionId = LANNEN_PUOLEINEN;
			}
			break;
		  case 13:
			{
			  windDirectionId = LANNEN_PUOLEINEN;
			}
			break;
		  case 14:
			{
			  if(windDirection.value() < 292.5)
				windDirectionId = LANNEN_PUOLEINEN;
			  else
				windDirectionId = LUOTEEN_PUOLEINEN;
			}
			break;
		  case 15:
			{
			  windDirectionId = LUOTEEN_PUOLEINEN;
			}
			break;
		  case 16:
			{
			  if(windDirection.value() < 337.5)
				windDirectionId = LUOTEEN_PUOLEINEN;
			  else
				windDirectionId = POHJOISEN_PUOLEINEN;
			}
			break;
		  default:
			break;
		  }
	  };
	
	return static_cast<wind_direction_id>(windDirectionId);
  }

  wind_direction_id get_wind_direction_id8(const WeatherResult& windDirection, const string& var)
  {
	WindDirectionAccuracy accuracy(direction_accuracy(windDirection.error(), var));

	if(accuracy == bad_accuracy)
	  return VAIHTELEVA;
	
	int windDirectionId = direction8th(windDirection.value());
	
	return static_cast<wind_direction_id>(windDirectionId);
  }

  wind_direction_id get_wind_direction_id(const WeatherResult& windDirection, const string& var)
  {
	WindDirectionAccuracy accuracy(direction_accuracy(windDirection.error(), var));

	if(accuracy == bad_accuracy)
	  return VAIHTELEVA;
	
	int windDirectionId = direction16th(windDirection.value());
	
	return static_cast<wind_direction_id>(windDirectionId);
  }

  wind_direction_id map_direction8_to_direction16(const int& direction8,
												  const WindDirectionAccuracy& accuracy)
  {
	wind_direction_id direction16(VAIHTELEVA);

	if(accuracy == bad_accuracy)
	  return VAIHTELEVA;

	switch(direction8  )
	  {
	  case 1:
		direction16 = (accuracy == good_accuracy ? POHJOINEN : POHJOISEN_PUOLEINEN);
		break;
	  case 2:
		direction16 = (accuracy == good_accuracy ? KOILLINEN : KOILLISEN_PUOLEINEN);		
		break;
	  case 3:
		direction16 = (accuracy == good_accuracy ? ITA : IDAN_PUOLEINEN);
		break;
	  case 4:
		direction16 = (accuracy == good_accuracy ? KAAKKO : KAAKON_PUOLEINEN);
		break;
	  case 5:
		direction16 = (accuracy == good_accuracy ? ETELA : ETELAN_PUOLEINEN);
		break;
	  case 6:
		direction16 = (accuracy == good_accuracy ? LOUNAS : LOUNAAN_PUOLEINEN);
		break;
	  case 7:
		direction16 = (accuracy == good_accuracy ? LANSI : LANNEN_PUOLEINEN);
		break;
	  case 8:
		direction16 = (accuracy == good_accuracy ? LUODE : LUOTEEN_PUOLEINEN);
		break;
	  default:
		direction16 = VAIHTELEVA;
		break;
	  };

	return direction16;

  }

  std::string get_wind_speed_string(const wind_speed_id& theWindSpeedId)
  {
	std::string retval;

	switch(theWindSpeedId)
	  {
	  case TYYNI:
		retval = "tyyni";
		break;
	  case HEIKKO:
		retval = "heikko";
		break;
	  case KOHTALAINEN:
		retval = "kohtalainen";
		break;
	  case NAVAKKA:
		retval = "navakka";
		break;
	  case KOVA:
		retval = "kova";
		break;
	  case MYRSKY:
		retval = "myrsky";
		break;
	  case HIRMUMYRSKY:
		retval = "hirmumyrsky";
		break;
	  }

	return retval;
  }

  std::string get_wind_direction_string(const wind_direction_id& theWindDirectionId)
  {
	std::string retval;

	switch(theWindDirectionId)
	  {
	  case POHJOINEN:
		retval = "pohjoinen";
		break;
	  case POHJOISEN_PUOLEINEN:
		retval = "pohjoisen puoleinen";
		break;
	  case POHJOINEN_KOILLINEN:
		retval = "pohjoisen ja koillisen valinen";
		break;
	  case KOILLINEN:
		retval = "koillinen";
		break;
	  case KOILLISEN_PUOLEINEN:
		retval = "koillisen puoleinen";
		break;
	  case ITA_KOILLINEN:
		retval = "idan ja koillisen valinen";
		break;
	  case ITA:
		retval = "ita";
		break;
	  case IDAN_PUOLEINEN:
		retval = "idan puoleinen";
		break;
	  case ITA_KAAKKO:
		retval = "idan ja kaakon valinen";
		break;
	  case KAAKKO:
		retval = "kaakko";
		break;
	  case KAAKON_PUOLEINEN:
		retval = "kaakon puoleinen";
		break;
	  case ETELA_KAAKKO:
		retval = "etelan ja kaakon valinen";
		break;
	  case ETELA:
		retval = "etela";
		break;
	  case ETELAN_PUOLEINEN:
		retval = "etelan puoleinen";
		break;
	  case ETELA_LOUNAS:
		retval = "etelan ja lounaan valinen";
		break;
	  case LOUNAS:
		retval = "lounas";
		break;
	  case LOUNAAN_PUOLEINEN:
		retval = "lounaan puoleinen";
		break;
	  case LANSI_LOUNAS:
		retval = "lannen ja lounaan valinen";
		break;
	  case LANSI:
		retval = "lansi";
		break;
	  case LANNEN_PUOLEINEN:
		retval = "lannen puoleinen";
		break;
	  case LANSI_LUODE:
		retval = "lannen ja luoteen valinen";
		break;
	  case LUODE:
		retval = "luode";
		break;
	  case LUOTEEN_PUOLEINEN:
		retval = "luoteen puoleinen";
		break;
	  case POHJOINEN_LUODE:
		retval = "pohjoisen ja luoteen valinen";
		break;
	  case VAIHTELEVA:
	  case MISSING_WIND_DIRECTION_ID:
		retval = "vaihteleva";
		break;
	  }

	return retval;
  }


  std::string get_wind_event_string(const wind_event_id& theWindEventId)
  {
	std::string retval;

	switch(theWindEventId)
	  {
	  case TUULI_HEIKKENEE:
		retval = "tuuli heikkenee";
		break;
	  case TUULI_VOIMISTUU:
		retval = "tuuli voimistuu";
		break;
	  case TUULI_TYYNTYY:
		retval = "tuuli tyyntyy";
		break;
	  case TUULI_KAANTYY:
		retval = "tuuli kaantyy";
		break;
	  case TUULI_KAANTYY_JA_HEIKKENEE:
		retval = "tuuli kaantyy ja heikkenee";
		break;
	  case TUULI_KAANTYY_JA_VOIMISTUU:
		retval = "tuuli kaantyy ja voimistuu";
		break;

	  case TUULI_KAANTYY_JA_TYYNTYY:
		retval = "tuuli kaantyy ja tyyntyy";
		break;
	  case TUULI_MUUTTUU_VAIHTELEVAKSI:
		retval = "tuuli muuttuu vaihtelevaksi";
		break;
	  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE:
		retval = "tuuli muuttuu vaihtelevaksi ja heikkenee";
		break;
	  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU:
		retval = "tuuli muuttuu vaihtelevaksi ja voimistuu";
		break;
	  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY:
		retval = "tuuli muuttuu vaihtelevaksi ja tyyntyy";
		break;
	  case MISSING_WIND_EVENT:
		retval = "missing wind event";
		break;
	  case MISSING_WIND_SPEED_EVENT:
		retval = "missing wind speed event";
		break;
	  case MISSING_WIND_DIRECTION_EVENT:
		retval = "missing wind direction event";
		break;
	  }

	return retval;
  }


  std::string get_wind_direction_turnto_string(const wind_direction_id& theWindDirectionId)
  {
	std::string retval;

	switch(theWindDirectionId)
	  {
	  case POHJOINEN:
		retval = POHJOISEEN_PHRASE;
		break;
	  case POHJOISEN_PUOLEINEN:
		retval = POHJOISEN_PUOLELLE_PHRASE;
		break;
	  case POHJOINEN_KOILLINEN:
		retval = POHJOISEN_JA_KOILLISEN_VALILLE_PHRASE;
		break;
	  case KOILLINEN:
		retval = KOILLISEEN_PHRASE;
		break;
	  case KOILLISEN_PUOLEINEN:
		retval = KOILLISEN_PUOLELLE_PHRASE;
		break;
	  case ITA_KOILLINEN:
		retval = IDAN_JA_KOILLISEN_VALILLE_PHRASE;
		break;
	  case ITA:
		retval = ITAAN_PHRASE;
		break;
	  case IDAN_PUOLEINEN:
		retval = IDAN_PUOLELLE_PHRASE;
		break;
	  case ITA_KAAKKO:
		retval = IDAN_JA_KAAKON_VALILLE_PHRASE;
		break;
	  case KAAKKO:
		retval = KAAKKOON_PHRASE;
		break;
	  case KAAKON_PUOLEINEN:
		retval = KAAKON_PUOLELLE_PHRASE;
		break;
	  case ETELA_KAAKKO:
		retval = ETELAN_JA_KAAKON_VALILLE_PHRASE;
		break;
	  case ETELA:
		retval = ETELAAN_PHRASE;
		break;
	  case ETELAN_PUOLEINEN:
		retval = ETELAN_PUOLELLE_PHRASE;
		break;
	  case ETELA_LOUNAS:
		retval = ETELAN_JA_LOUNAAN_VALILLE_PHRASE;
		break;
	  case LOUNAS:
		retval = LOUNAASEEN_PHRASE;
		break;
	  case LOUNAAN_PUOLEINEN:
		retval = LOUNAAN_PUOLELLE_PHRASE;
		break;
	  case LANSI_LOUNAS:
		retval = LANNEN_JA_LOUNAAN_VALILLE_PHRASE;
		break;
	  case LANSI:
		retval = LANTEEN_PHRASE;
		break;
	  case LANNEN_PUOLEINEN:
		retval = LANNEN_PUOLELLE_PHRASE;
		break;
	  case LANSI_LUODE:
		retval = LANNEN_JA_LUOTEEN_VALILLE_PHRASE;
		break;
	  case LUODE:
		retval = LUOTEESEEN_PHRASE;
		break;
	  case LUOTEEN_PUOLEINEN:
		retval = LUOTEEN_PUOLELLE_PHRASE;
		break;
	  case POHJOINEN_LUODE:
		retval = POHJOISEN_JA_LUOTEEN_VALILLE_PHRASE;
		break;
	  case VAIHTELEVA:
		retval = TUULI_MUUTTUU_VAIHTELEVAKSI_PHRASE;
		break;
	  case MISSING_WIND_DIRECTION_ID:
		retval = EMPTY_STRING;
		break;
	  }

	return retval;
  }
  
  wind_direction_id get_moderate_accuracy_direction(const wind_direction_id& directionId)
  {
	switch(directionId)
	  {
	  case POHJOINEN:
		return POHJOISEN_PUOLEINEN;
		break;
	  case ETELA:
		return ETELAN_PUOLEINEN;
		break;
	  case ITA:
		return IDAN_PUOLEINEN;
		break;
	  case LANSI:
		return LANNEN_PUOLEINEN;
		break;
	  case KOILLINEN:
		return KOILLISEN_PUOLEINEN;
		break;
	  case KAAKKO:
		return KAAKON_PUOLEINEN;
		break;
	  case LOUNAS:
		return LOUNAAN_PUOLEINEN;
		break;
	  case LUODE:
		return LUOTEEN_PUOLEINEN;
		break;
	  default:
		return VAIHTELEVA;
		break;
	  }
  }

  bool is_principal_compass_point(const wind_direction_id& directionId)
  {
	return (directionId == POHJOINEN ||
			directionId == ETELA ||
			directionId == ITA ||
			directionId == LANSI ||
			directionId == KOILLINEN ||
			directionId == KAAKKO ||
			directionId == LOUNAS ||
			directionId == LUODE);
  }

  void populate_windspeed_distribution_time_series(const AnalysisSources& theSources,
												   const WeatherArea& theArea,
												   const WeatherPeriod& thePeriod,
												   const string& theVar,
												   vector <pair<float, WeatherResult> >& theWindSpeedDistribution)
  {
	GridForecaster forecaster;

	float ws_lower_limit = 0.0;
	float ws_upper_limit = 1.0;

	while (ws_lower_limit < HIRMUMYRSKY_LOWER_LIMIT)
	  {
		RangeAcceptor acceptor;
		acceptor.lowerLimit(ws_lower_limit);
		if(ws_lower_limit < HIRMUMYRSKY_LOWER_LIMIT + 1)
		  acceptor.upperLimit(ws_upper_limit-0.0001);

				
		WeatherResult share =
		  forecaster.analyze(theVar+"::fake::tyyni::share",
							 theSources,
							 WindSpeed,
							 Mean,
							 Percentage,
							 theArea,
							 thePeriod,
							 DefaultAcceptor(),
							 DefaultAcceptor(),
							 acceptor);

		pair<float, WeatherResult> shareItem(ws_lower_limit, share);
		theWindSpeedDistribution.push_back(shareItem);

		ws_lower_limit += 1.0;
		ws_upper_limit += 1.0;

	  }
  }

  // returns wind direction id in the given time
  wind_direction_id WindForecast::findWindDirectionId(const NFmiTime& timestamp) const
  {
	for(unsigned int i = 0; i < theParameters.theWindDirectionVector.size(); i++)
	  {
		// find the correcponding period and find out the exact wind direction for the period
		if(is_inside(timestamp, theParameters.theWindDirectionVector[i]->thePeriod))
		  {
			if(is_principal_compass_point(theParameters.theWindDirectionVector[i]->theWindDirection))
			  {
				int totalCount = 0;
				int moderateAccuracyCount = 0;
				for(unsigned int k = 0; k < theParameters.theRawDataVector.size(); k++)
				  {
					const WindDataItemUnit& windDataItem = (*theParameters.theRawDataVector[k])();

					if(is_inside(windDataItem.thePeriod,
								 theParameters.theWindDirectionVector[i]->thePeriod))
					  {
						totalCount++;
						if(direction_accuracy(windDataItem.theWindDirection.error(),
											  theParameters.theVar) == moderate_accuracy)
						  moderateAccuracyCount++;
					  }
				  }
				if(totalCount > 0 && moderateAccuracyCount > totalCount / 2)
				  return get_moderate_accuracy_direction(theParameters.theWindDirectionVector[i]->theWindDirection);
				else
				  return theParameters.theWindDirectionVector[i]->theWindDirection;
			  }
			else
			  {
				return theParameters.theWindDirectionVector[i]->theWindDirection;
			  }
		  }
	  }
	  
	return VAIHTELEVA;
  }

  // returns the period during which the wind turns
  WeatherPeriod get_wind_turning_period(const NFmiTime& timestamp)
  {
	WeatherPeriod period(timestamp, timestamp);

	return period;
  }

  std::string dayPhasePhrase("");

  Sentence WindForecast::getTimePhrase(const WeatherPeriod thePeriod,
									   const bool& alkaenPhrase /*= true */) const
  {
	Sentence sentence;

	int forecastPeriodLength = get_period_length(theParameters.theForecastPeriod);
			
	bool specifyDay = (forecastPeriodLength > 24 &&
					   abs(theParameters.theForecastTime.DifferenceInHours(thePeriod.localStartTime())) > 21);
	// day phase specifier

	if(thePeriod.localEndTime() == theParameters.theForecastPeriod.localEndTime()
	   && get_period_length(thePeriod) > 5)
	  {
		WeatherPeriod begPeriod(thePeriod.localStartTime(), thePeriod.localStartTime());

		// make sure that "alkaen"-phrase is used when wanted
		sentence << get_time_phrase_large(begPeriod,
										  specifyDay,
										  theParameters.theVar, 
										  dayPhasePhrase,
										  alkaenPhrase);
	  }
	else
	  {
		sentence << get_time_phrase_large(thePeriod,
										  specifyDay,
										  theParameters.theVar, 
										  dayPhasePhrase,
										  get_period_length(thePeriod) > 4 && alkaenPhrase);
	  }

	//	cout << dayPhasePhrase << endl;

	return sentence;
  }

  WindForecast::WindForecast(wo_story_params& parameters):
	theParameters(parameters)
  {
  }

  void WindForecast::printOutWindData(std::ostream& theOutput) const
  {
  }
  
  void WindForecast::printOutWindPeriods(std::ostream& theOutput) const
  {
	
  }

  wind_direction_id WindForecast::getWindDirectionId(const WeatherPeriod& thePeriod,
													 const CompassType& theComapssType) const
  {
	for(unsigned int i = 0; i < theParameters.theRawDataVector.size() ; i++)
	  {
		WindDataItemUnit& item = theParameters.theRawDataVector[i]->getDataItem();
		if(!is_inside(item.thePeriod.localStartTime(), thePeriod))
		  continue;
		
		WindDirectionAccuracy accuracy(direction_accuracy(item.theWindDirection.error(), theParameters.theVar));

		int direction(eight_directions ? 
					  direction8th(item.theEqualizedWindDirection) :
					  direction16th(item.theEqualizedWindDirection));

		
		if(accuracy == bad_accuracy)
		  return VAIHTELEVA;
		else
		  return static_cast<wind_direction_id>(direction);
		
	  }

	return VAIHTELEVA;
  }

  wind_direction_id WindForecast::getWindDirectionId(const WeatherPeriod& thePeriod) const
  {
	wind_direction_id retval(VAIHTELEVA);

	// for short-term forecasts (8 hours or less) use 16-direction compass, for long-term forecasts 8-direction compass
	if(thePeriod.localStartTime().DifferenceInHours(theParameters.theForecastTime) < 8)
	  retval = getWindDirectionId(thePeriod,
								  sixteen_directions);
	else
	  retval = getWindDirectionId(thePeriod,
								  eight_directions);
	
	return retval;
  }


  Sentence WindForecast::getWindSpeedDecreasingIncreasingInterval(const WeatherPeriod& speedEventPeriod,
																  const bool& firstSentenceInTheStory) const
  {
	Sentence sentence;

	// find the time when max wind has strenghtened > theParameters.theWindSpeedThreshold and
	// take the period from there to the end of the speedEventPeriod
	int counter = 0;
	float maxWindBeg = 0.0;
	unsigned int index1 = UINT_MAX;
	for(unsigned int i = 0; i < theParameters.theRawDataVector.size(); i++)
	  {
		const WindDataItemUnit& windDataItem = (*theParameters.theRawDataVector[i])();
			
		if(is_inside(windDataItem.thePeriod.localStartTime(), speedEventPeriod))
		  {
			// TODO: should we check here equalized maximum wind
			if(counter == 0)
			  {
				maxWindBeg = windDataItem.theWindMaximum.value();
				counter++;
			  }
			else if(abs(windDataItem.theWindMaximum.value() - maxWindBeg) > theParameters.theWindSpeedThreshold)
			  {
				index1 = i;
			  }
		  }
	  }

	if(index1 != UINT_MAX)
	  {
		const WindDataItemUnit& windDataItem = (*theParameters.theRawDataVector[index1])();
		WeatherPeriod periodEnd(windDataItem.thePeriod.localEndTime(), speedEventPeriod.localEndTime());
		
		wind_direction_id windDirectionId1(MISSING_WIND_DIRECTION_ID);
		if(firstSentenceInTheStory)
		  {
			WeatherPeriod begPeriod(speedEventPeriod.localStartTime(), speedEventPeriod.localStartTime());
			sentence  << Delimiter(COMMA_PUNCTUATION_MARK)
					  << ALUKSI_WORD
					  << windSpeedSentence(begPeriod, false);
			windDirectionId1 = getWindDirectionId(begPeriod);
		  }

		wind_direction_id windDirectionId2 = getWindDirectionId(periodEnd);
		
		sentence  << Delimiter(COMMA_PUNCTUATION_MARK)
				  << getTimePhrase(periodEnd, false);
		if(windDirectionId2 != windDirectionId1 &&
		   windDirectionId1 !=MISSING_WIND_DIRECTION_ID)
		  sentence << windDirectionSentence(windDirectionId2);

		sentence << windSpeedSentence(periodEnd, true);
	  }
	else
	  {
		sentence  << Delimiter(COMMA_PUNCTUATION_MARK)
				  << windSpeedSentence(speedEventPeriod, true);
	  }

	return sentence;
  }

  Sentence WindForecast::getWindSentence(const wind_event_id& speedEventId, 
										 const WeatherPeriod& speedEventPeriod,
										 const wind_event_id& directionEventId, 
										 const WeatherPeriod& directionEventPeriod,
										 const wind_direction_id& directionIdEnd,
										 const bool& firstSentenceInTheStory) const
  {
	Sentence sentence;

	Sentence mainSentence;
	Sentence speedIntervalSentence;
	Sentence timePhraseOfSpeedEvent;
	if(!firstSentenceInTheStory)
	  {
		timePhraseOfSpeedEvent << getTimePhrase(speedEventPeriod, true);
	  }
	else
	  {
		timePhraseOfSpeedEvent << EMPTY_STRING;
	  }
	Sentence timePhraseOfDirectionEvent;

	wind_direction_id windDirectionIdDuringPeriod = getWindDirectionId(directionEventPeriod);
																	   

	if(!firstSentenceInTheStory)
	  {
		timePhraseOfDirectionEvent << getTimePhrase(directionEventPeriod, false);
	  }
	else
	  timePhraseOfDirectionEvent << EMPTY_STRING;

	if(speedEventId == TUULI_HEIKKENEE)
	  {
		if(directionEventId == TUULI_KAANTYY)
		  {
			mainSentence << ILTAPAIVALLA_TUULI_HEIKKENEE_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE
						 << timePhraseOfSpeedEvent
						 << get_wind_direction_turnto_string(directionIdEnd);
		  }
		else if(directionEventId == TUULI_MUUTTUU_VAIHTELEVAKSI)
		  {
			mainSentence << ILTAPAIVALLA_TUULI_HEIKKENEE_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
						 << timePhraseOfSpeedEvent;
		  }
		else
		  {
			if(firstSentenceInTheStory)
			  {
				mainSentence << ILTAPAIVALLA_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
							 << timePhraseOfSpeedEvent
							 << HEIKKENEVAA_PHRASE
							 << windDirectionSentence(windDirectionIdDuringPeriod);
			  }
			else
			  {
				mainSentence << ILTAPAIVALLA_HEIKKENEVAA_TUULTA_COMPOSITE_PHRASE
							 << timePhraseOfSpeedEvent
							 << HEIKKENEVAA_TUULTA_PHRASE;
			  }
		  }
	  }
	else if(speedEventId == TUULI_VOIMISTUU)
	  {
		if(directionEventId == TUULI_KAANTYY)
		  {
			mainSentence << ILTAPAIVALLA_TUULI_VOIMISTUU_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE
						 << timePhraseOfSpeedEvent
						 << get_wind_direction_turnto_string(directionIdEnd);
		  }
		else if(directionEventId == TUULI_MUUTTUU_VAIHTELEVAKSI)
		  {
			mainSentence << ILTAPAIVALLA_TUULI_VOIMISTUU_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
						 << timePhraseOfSpeedEvent;
		  }
		else
		  {
			if(firstSentenceInTheStory)
			  {
				mainSentence << ILTAPAIVALLA_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
							 << timePhraseOfSpeedEvent
							 << VOIMISTUVAA_PHRASE
							 << windDirectionSentence(windDirectionIdDuringPeriod);
			  }
			else
			  {
				mainSentence << ILTAPAIVALLA_HEIKKENEVAA_TUULTA_COMPOSITE_PHRASE
							 << timePhraseOfSpeedEvent
							 << VOIMISTUVAA_TUULTA_PHRASE;
			  }
		  }
	  }
	else if(speedEventId == MISSING_WIND_SPEED_EVENT)
	  {
		if(directionEventId == TUULI_KAANTYY)
		  {
			sentence << ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE
					 << timePhraseOfDirectionEvent
					 << get_wind_direction_turnto_string(directionIdEnd);

			/*
			mainSentence << ILTAPAIVALLA_ETELAAN_KAANTYVAA_TUULTA_COMPOSITE_PHRASE
						 << timePhraseOfDirectionEvent
						 << get_wind_direction_turnto_string(directionIdEnd);
			*/
		  }
		else if(directionEventId == TUULI_MUUTTUU_VAIHTELEVAKSI)
		  {
			mainSentence << ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
						 << timePhraseOfSpeedEvent;
		  }
		else
		  {
			if(firstSentenceInTheStory)
			  {
				mainSentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
							 << timePhraseOfSpeedEvent
							 << windDirectionSentence(windDirectionIdDuringPeriod);
			  }
		  }

	  }

	sentence << mainSentence;

	if(speedEventId == TUULI_VOIMISTUU ||
	   speedEventId == TUULI_HEIKKENEE)
	  {
		sentence << getWindSpeedDecreasingIncreasingInterval(speedEventPeriod,
															 firstSentenceInTheStory);
	  }
	else
	  {
		speedIntervalSentence << windSpeedSentence(speedEventPeriod, true);
		if(!speedIntervalSentence.empty() && !sentence.empty())
		  {
			sentence << Delimiter(COMMA_PUNCTUATION_MARK)
					 << speedIntervalSentence;
		  }
	  }
	return sentence;
  }

  // tuuli voi voimistua, heiketä tai kääntyä useaslla peräkkäiselä periodilla (käyrä polveilee, mutta ei käänny)
  // tämä funktio etsii oikean käännöskohdan
  WeatherPeriod getActualWindEventPeriod(const wind_event_period_data_item_vector& theWindEventPeriodVector,
										 unsigned int theBegIndex)
  {
	WindEventPeriodDataItem* eventPeriodItem = theWindEventPeriodVector[theBegIndex];
	NFmiTime periodStartTime(eventPeriodItem->thePeriod.localStartTime());
	NFmiTime periodEndTime(eventPeriodItem->thePeriod.localStartTime());
	
	bool firstWindSpeedEvent = (eventPeriodItem->theWindEvent == TUULI_HEIKKENEE ||
								eventPeriodItem->theWindEvent == TUULI_VOIMISTUU ||
								eventPeriodItem->theWindEvent == TUULI_TYYNTYY);


	for(unsigned int i = theBegIndex + 1; i < theWindEventPeriodVector.size(); i++)
	  {
		eventPeriodItem = theWindEventPeriodVector[i];

		bool currentWindSpeedEvent = (eventPeriodItem->theWindEvent == TUULI_HEIKKENEE ||
									  eventPeriodItem->theWindEvent == TUULI_VOIMISTUU ||
									  eventPeriodItem->theWindEvent == TUULI_TYYNTYY);
		
		if(firstWindSpeedEvent == currentWindSpeedEvent)
		  {
			periodEndTime = eventPeriodItem->thePeriod.localStartTime();
			//periodEndTime.ChangeByHours(-1);
			break;
		  }
		periodEndTime = eventPeriodItem->thePeriod.localEndTime();
	  }

	return WeatherPeriod(periodStartTime, periodEndTime);
	  /*
	  TUULI_HEIKKENEE = 0x1,
	  TUULI_VOIMISTUU = 0x2,
	  TUULI_TYYNTYY = 0x4,
	  TUULI_KAANTYY = 0x8,
	  TUULI_MUUTTUU_VAIHTELEVAKSI = 0x10,
	  */
	
  }

  Paragraph WindForecast::windForecastBasedOnEventPeriods(const WeatherPeriod& thePeriod) const
  {
	Paragraph paragraph;

	for(unsigned int i = 0; i < theParameters.theWindEventPeriodVector.size(); i++)
	  {
		if(theParameters.theWindEventPeriodVector[i]->theReportThisEventPeriodFlag == false)
		  continue;

		WindEventPeriodDataItem* eventPeriodItem = theParameters.theWindEventPeriodVector[i];
		WindEventPeriodDataItem* concurrentItem = theParameters.theWindEventPeriodVector[i]->theConcurrentEventPeriodItem;

		const wind_event_id& eventId = eventPeriodItem->theWindEvent;
		//		const WeatherPeriod& eventPeriod = eventPeriodItem->thePeriod;
		bool firstSentenceInTheStory = paragraph.empty();

		WeatherPeriod actualWindEventPeriod = getActualWindEventPeriod(theParameters.theWindEventPeriodVector, i);

		Sentence sentence;
		switch(eventId)
		  {
		  case TUULI_HEIKKENEE:
		  case TUULI_VOIMISTUU:
			{
			  if(concurrentItem)
				{
				  wind_direction_id windDirectionEnd = get_wind_direction_id(concurrentItem->thePeriodEndDataItem.theWindDirection, theParameters.theVar);

				  sentence << getWindSentence(eventId,
											  actualWindEventPeriod,//eventPeriodItem->thePeriod,
											  concurrentItem->theWindEvent,
											  concurrentItem->thePeriod,
											  windDirectionEnd,
											  firstSentenceInTheStory);
				}
			  else
				{
				  sentence << ILTAPAIVALLA_HEIKKENEVAA_TUULTA_COMPOSITE_PHRASE
						   << getTimePhrase(eventPeriodItem->thePeriod, true)
						   << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_TUULTA_PHRASE : VOIMISTUVAA_TUULTA_PHRASE);


				  sentence << getWindSpeedDecreasingIncreasingInterval(eventPeriodItem->thePeriod,
																	   firstSentenceInTheStory);
				}
			}
			break;
		  case TUULI_TYYNTYY:
			sentence << TUULI_TYYNTYY_PHRASE;
			break;
		  case TUULI_KAANTYY:
		  case TUULI_MUUTTUU_VAIHTELEVAKSI:
			{
			  wind_direction_id windDirectionIdEnd = get_wind_direction_id(eventPeriodItem->thePeriodEndDataItem.theWindDirection, theParameters.theVar);

			  if(concurrentItem)
				{
				  sentence << getWindSentence(concurrentItem->theWindEvent,
											  actualWindEventPeriod,//concurrentItem->thePeriod,
											  eventId,
											  eventPeriodItem->thePeriod,
											  windDirectionIdEnd,
											  firstSentenceInTheStory);
				}
			  else
				{
				  WeatherPeriod eventPeriod(eventPeriodItem->thePeriod.localStartTime(), 
											eventPeriodItem->thePeriod.localStartTime());

				  if(eventId == TUULI_MUUTTUU_VAIHTELEVAKSI)
					{
					  if(eventPeriodItem->theTransientFlag)
						sentence << ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE
								 << getTimePhrase(eventPeriod, false);
					  else
						sentence << ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
								 << getTimePhrase(eventPeriod, true);
					}
				  else
					{
					  sentence << ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE
							   << getTimePhrase(eventPeriod, true)
							   << get_wind_direction_turnto_string(windDirectionIdEnd);
					  
					  /*
						sentence << ILTAPAIVALLA_ETELAAN_KAANTYVAA_TUULTA_COMPOSITE_PHRASE
						<< getTimePhrase(eventPeriod, true)
						<< get_wind_direction_turnto_string(windDirectionIdEnd);
					  */
					}
				}
			}
			break;
		  case TUULI_KAANTYY_JA_HEIKKENEE:
			break;
		  case TUULI_KAANTYY_JA_VOIMISTUU:
 			break;
		  case TUULI_KAANTYY_JA_TYYNTYY:
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE:
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU:
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY:
			break;
		  case MISSING_WIND_SPEED_EVENT:
			{
			  if(concurrentItem)
				{
				  wind_direction_id windDirectionEnd = get_wind_direction_id(concurrentItem->thePeriodEndDataItem.theWindDirection, theParameters.theVar);

				  sentence << getWindSentence(eventId,
											  eventPeriodItem->thePeriod,
											  concurrentItem->theWindEvent,
											  concurrentItem->thePeriod,
											  windDirectionEnd,
											  firstSentenceInTheStory);
				}
			}
			break;
		  case MISSING_WIND_DIRECTION_EVENT:
			{
			  wind_direction_id windDirectionIdEnd = get_wind_direction_id(eventPeriodItem->thePeriodEndDataItem.theWindDirection, theParameters.theVar);

			  if(concurrentItem)
				{
				  sentence << getWindSentence(concurrentItem->theWindEvent,
											  concurrentItem->thePeriod,
											  eventId,
											  eventPeriodItem->thePeriod,
											  windDirectionIdEnd,
											  firstSentenceInTheStory);
				}
			}
			break;
		  default:
			break;
		  };
		
		paragraph << sentence;
		
	  } // for


	return paragraph;
  }

  Paragraph WindForecast::windForecastBasedOnEvents(const WeatherPeriod& thePeriod) const
  {
	Paragraph paragraph;

	// first tell about the beginning of the forecast period
	Sentence theOpeningSentence;
	theOpeningSentence << windDirectionSentence(findWindDirectionId(theParameters.theWindDirectionVector[0]->thePeriod.localStartTime()));
	theOpeningSentence << windSpeedSentence(theParameters.theWindDirectionVector[0]->thePeriod);
	paragraph << theOpeningSentence;

	// then iterate through the events and raport about them
	//    unsigned int forecastPeriodLength = get_period_length(theParameters.theForecastPeriod);
	thePreviousRangeBeg = INT_MAX;
	thePreviousRangeEnd = INT_MAX;

	unsigned int eventCount = theParameters.theWindEventVector.size();
	for(unsigned int i = 0; i < eventCount; i++)
	  {
		Sentence eventSentence;
		Sentence timePhrase;
		Sentence windDirectionTurningToPhrase;

		wind_event_id windEventId = theParameters.theWindEventVector[i].second;
		timePhrase << get_time_phrase(theParameters.theWindEventVector[i].first, theParameters.theVar);
		wind_direction_id directionId = findWindDirectionId(theParameters.theWindEventVector[i].first);
		windDirectionTurningToPhrase << get_wind_direction_turnto_string(directionId);

		NFmiTime reportPeriodStartTime(theParameters.theWindEventVector[i].first);
		NFmiTime reportPeriodEndTime(i < eventCount - 1 ? theParameters.theWindEventVector[i+1].first :
									 theParameters.theForecastPeriod.localEndTime());
		if(i < eventCount - 1)
		  reportPeriodEndTime.ChangeByHours(-1);

		/*
		  NFmiTime reportPeriodStartTime(i == 0 ? theParameters.theForecastPeriod.localStartTime() :
		  theParameters.theWindEventVector[i-1].first);
		  NFmiTime reportPeriodEndTime(theParameters.theWindEventVector[i].first);
		*/
		//		reportPeriodEndTime.ChangeByHours(-1);

		WeatherPeriod periodToReport(reportPeriodStartTime, reportPeriodEndTime);
		// the period during which the wind direction turns to another
		// TODO: not this way: merge the periods before coming here
		//		WeatherPeriod directionTurningPeriod(get_wind_turning_period(theParameters.theWindEventVector[i].first));

		/*

		int forecastPeriodLength = get_period_length(theParameters.theForecastPeriod);

		bool specifyDay = (forecastPeriodLength > 24 &&
		abs(theParameters.theForecastTime.DifferenceInHours(periodToReport.localStartTime())) > 21);
		// day phase specifier
		std::string dayPhasePhrase;
		timePhrase << get_time_phrase_large(periodToReport,
		specifyDay,
		theParameters.theVar, 
		dayPhasePhrase,
		true);
		*/
		switch (windEventId)
		  {
		  case TUULI_HEIKKENEE:
			{
			  eventSentence << timePhrase << HEIKKENEVAA_TUULTA_PHRASE;			
			  eventSentence << Delimiter(COMMA_PUNCTUATION_MARK);
			  eventSentence << windSpeedSentence(periodToReport);
			}
			break;
		  case TUULI_VOIMISTUU:
			{
			  eventSentence << timePhrase << VOIMISTUVAA_TUULTA_PHRASE;
			  eventSentence << Delimiter(COMMA_PUNCTUATION_MARK);
			  eventSentence << windSpeedSentence(periodToReport);
			}
			break;
		  case TUULI_TYYNTYY:
		  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY:
		  case TUULI_KAANTYY_JA_TYYNTYY:
			eventSentence << ILTAPAIVALLA_TUULI_TYYNTYY_COMPOSITE_PHRASE
						  << timePhrase;
			break;
		  case TUULI_KAANTYY:
			eventSentence << ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE
						  << timePhrase 
						  << windDirectionTurningToPhrase;
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI:
			eventSentence << ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
						  << timePhrase; 
			break;
		  case TUULI_KAANTYY_JA_HEIKKENEE:
			eventSentence << ILTAPAIVALLA_TUULI_HEIKKENEE_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE
						  << timePhrase 
						  << windDirectionTurningToPhrase
						  << windSpeedSentence(periodToReport);
			break;
		  case TUULI_KAANTYY_JA_VOIMISTUU:
			eventSentence << ILTAPAIVALLA_TUULI_VOIMISTUU_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE
						  << timePhrase 
						  << windDirectionTurningToPhrase
						  << windSpeedSentence(periodToReport);
			break;
			/*
		  case TUULI_KAANTYY_JA_TYYNTYY:
			eventSentence << ILTAPAIVALLA_TUULI_TYYNTYY_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE
						  << timePhrase 
						  << windDirectionTurningToPhrase;
			break;
			*/
		  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE:
			eventSentence << ILTAPAIVALLA_TUULI_HEIKKENEE_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
						  << timePhrase 
						  << windSpeedSentence(periodToReport);
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU:
			eventSentence << ILTAPAIVALLA_TUULI_VOIMISTUU_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
						  << timePhrase 
						  << windSpeedSentence(periodToReport);
			break;
			/*
		  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY:
			eventSentence << ILTAPAIVALLA_TUULI_TYYNTYY_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
						  << timePhrase 
						  << windDirectionTurningToPhrase;
			*/
			break;
		  case MISSING_WIND_EVENT:
		  case MISSING_WIND_SPEED_EVENT:
		  case MISSING_WIND_DIRECTION_EVENT:
			break;
		  };

		paragraph << eventSentence;
	  }

	return paragraph;

#ifdef LATER

	/*
	  void log_wind_events(wo_story_params& storyParams)
	  {
	  storyParams.theLog << "*********** WIND EVENTS ***********" << endl;

	  unsigned int windDirectionIndex = 0;
	  unsigned int windSpeedIndex = 0;

	  for(unsigned int i = 0; i < storyParams.theWindEventVector.size(); i++)
	  {
	  wind_event_id windEventId = storyParams.theWindEventVector[i].second;

	  storyParams.theLog << storyParams.theWindEventVector[i].first 
	  << " "
	  <<  get_wind_event_string(storyParams.theWindEventVector[i].second)
	  << " ";

	  if(windEventId > 0x0 && windEventId <= TUULI_TYYNTYY ||
	  windEventId >= TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE && windEventId <= TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY)
	  {
	  storyParams.theLog << "(";
	  storyParams.theLog << get_wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex++]->theWindSpeedId);
	  storyParams.theLog <<  " -> ";
	  storyParams.theLog << get_wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex]->theWindSpeedId);
	  storyParams.theLog << ")" << endl;
	  if(windEventId >= TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE)
	  windDirectionIndex++;
	  }
	  else if(windEventId == TUULI_KAANTYY)
	  {
	  storyParams.theLog << "(";
	  storyParams.theLog << get_wind_direction_string(storyParams.theWindDirectionVector[windDirectionIndex++]->theWindDirection);
	  storyParams.theLog <<  " -> ";
	  storyParams.theLog <<  get_wind_direction_string(storyParams.theWindDirectionVector[windDirectionIndex]->theWindDirection);			
	  storyParams.theLog << ")" << endl;
	  }
	  else if(windEventId >= TUULI_KAANTYY_JA_HEIKKENEE && windEventId <= TUULI_KAANTYY_JA_TYYNTYY)
	  {
	  storyParams.theLog << "(";
	  storyParams.theLog << get_wind_direction_string(storyParams.theWindDirectionVector[windDirectionIndex++]->theWindDirection);
	  storyParams.theLog <<  " -> ";
	  storyParams.theLog << get_wind_direction_string(storyParams.theWindDirectionVector[windDirectionIndex]->theWindDirection);
	  storyParams.theLog << "; ";
	  storyParams.theLog << get_wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex++]->theWindSpeedId);
	  storyParams.theLog <<  " -> ";
	  storyParams.theLog <<  get_wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex]->theWindSpeedId);
	  storyParams.theLog << ")" << endl;
	  }
	  else
	  {
	  storyParams.theLog << "" << endl;

	  if(windEventId == TUULI_MUUTTUU_VAIHTELEVAKSI)
	  windDirectionIndex++;
	  }

	*/
#endif

  }




  Sentence WindForecast::windSentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

    unsigned int forecastPeriodLength = get_period_length(theParameters.theForecastPeriod);
	thePreviousRangeBeg = INT_MAX;
	thePreviousRangeEnd = INT_MAX;

	for(unsigned int i = 0; i < theParameters.theWindDirectionVector.size(); i++)
	  {
		if(sentence.size() > 0)
		  sentence << Delimiter(COMMA_PUNCTUATION_MARK);

		if(i > 0)
		  {
			bool spefifyDay = (forecastPeriodLength > 24 &&
							   abs(theParameters.theForecastTime.DifferenceInHours(theParameters.theWindDirectionVector[i]->thePeriod.localStartTime())) > 21);
			// day phase specifier
			std::string dayPhasePhrase;
			sentence << get_time_phrase_large(theParameters.theWindDirectionVector[i]->thePeriod,
											  spefifyDay,
											  theParameters.theVar, 
											  dayPhasePhrase,
											  true);
		  }
		sentence << windDirectionSentence(theParameters.theWindDirectionVector[i]->theWindDirection);
		sentence << windSpeedSentence(theParameters.theWindDirectionVector[i]->thePeriod);
	  }

	return sentence;
  }

  const Sentence WindForecast::speedRangeSentence(const WeatherPeriod& thePeriod,
												  const WeatherResult & theMaxSpeed,
												  const WeatherResult & theMedianSpeed,
												  const string & theVariable, 
												  bool theLastSentenceFlag) const
  {
	using Settings::optional_int;

	Sentence sentence;

	const int maxvalue = static_cast<int>(round(theMaxSpeed.value()));
	const int medianvalue = static_cast<int>(round(theMedianSpeed.value()));
	int peakWind = -1;

	if(theParameters.theMetersPerSecondFormat == TEXTPHRASE_WORD)
	  {
		sentence << *UnitFactory::create(MetersPerSecond, maxvalue) << TUULTA_WORD;
	  }
	else
	  {
		if(abs(maxvalue - medianvalue) <= theParameters.theMinInterval)
		  {
			if(medianvalue != thePreviousRangeBeg)
			  {
				sentence << medianvalue;
				thePreviousRangeBeg = medianvalue;
			  }
		  }
		else
		  {
			// subtract 1 m/s from the lowest limit
			int intervalMin  = (maxvalue - (medianvalue - 1) > 5 ? maxvalue - 5 : medianvalue - 1);
			int intervalMax = maxvalue;
			if(intervalMin != thePreviousRangeBeg || maxvalue != thePreviousRangeEnd)
			  {
				// TODO: make sure that 2/3 of values are inside the interval
				vector <pair<float, WeatherResult> > windSpeedDistribution;
				
				populate_windspeed_distribution_time_series(theParameters.theSources,
															theParameters.theArea,
															thePeriod,
															theParameters.theVar,
															windSpeedDistribution);
				// 		pair<float, WeatherResult> shareItem(ws_lower_limit, share);

				std::ostringstream oss;

				int lower_index = maxvalue - 1;
				float sumDistribution = windSpeedDistribution[maxvalue].second.value();
				oss << lower_index+1 << ":"  << sumDistribution;
				while(sumDistribution < 67.0 && lower_index > 0)
				  {
					lower_index--;

					oss << "," << lower_index+1 << ":"  << windSpeedDistribution[lower_index].second.value();
				  
					sumDistribution += windSpeedDistribution[lower_index].second.value();
				  }
				if(lower_index+1 < intervalMin)
				  {
					/*
					cout << "interval extended on period "
						 << thePeriod.localStartTime() << "..." << thePeriod.localEndTime()
						 << "; data: "
						 << oss.str()
						 << "; original: " 
						 << intervalMin
						 << "..." 
						 << maxvalue 
						 <<  "; new: "
						 << lower_index+1
						 << "..." 
						 << maxvalue 
						 << endl;
					*/
					
					intervalMin = lower_index+1;

					if(intervalMax - intervalMin > 5)
					  {
						peakWind = maxvalue;
						intervalMax = intervalMin + 5;
					  }
					  /*
					  cout << "interval bigger than 5! " 
						   << thePeriod.localStartTime() << "..." << thePeriod.localEndTime()
						   << "; data: "
						   << oss.str()
						   << "; original: " 
						   << intervalMin
						   << "..." 
						   << maxvalue 
						   <<  "; new: "
						   << lower_index+1
						   << "..." 
						   << maxvalue 
						   << endl;
					  */

				  }

				sentence << IntegerRange(intervalMin , intervalMax, theParameters.theRangeSeparator);
				thePreviousRangeBeg = intervalMin;
				thePreviousRangeEnd = intervalMax;
			  }
		  }
		if(!sentence.empty())
		  {
			sentence << *UnitFactory::create(MetersPerSecond);
			if(peakWind != -1 && theLastSentenceFlag)
			  {
				sentence << Delimiter(",") 
						 << KOVIMMILLAAN_PHRASE
						 << peakWind
						 << *UnitFactory::create(MetersPerSecond);
			  }
		  }
	  }
	  
	return sentence;
	  
  }

  void WindForecast::getRepresentativeInterval(const float& theDistributionSum, 
											   const WeatherPeriod& thePeriod,
											   float& theLowerLimit,
											   float& theUpperLimit)
  {
	vector <pair<float, WeatherResult> > windSpeedDistribution;
	
	populate_windspeed_distribution_time_series(theParameters.theSources,
												theParameters.theArea,
												thePeriod,
												theParameters.theVar,
												windSpeedDistribution);

	
  }

  // theLastSentenceFlag tells wheather this is the last wind speed sentence on this event period
  // so that we can use "at its strongest"-expression if needed
  const Sentence WindForecast::windSpeedSentence(const WeatherPeriod& thePeriod, bool theLastSentenceFlag /* = true*/) const
  {
	Sentence sentence;

	double medianValueSum = 0.0;
	double medianErrorSum = 0.0;
	unsigned int counter = 0;

	WindDataItemUnit dataItem = (*theParameters.theRawDataVector[0])();
	WindDataItemUnit dataItemMinMax = (*theParameters.theRawDataVector[0])();

	//	cout << "period: " << thePeriod.localStartTime() << "..." << thePeriod.localEndTime() << endl;
	for(unsigned int i = 0; i < theParameters.theRawDataVector.size(); i++)
	  {
		dataItem = (*theParameters.theRawDataVector[i])();
		if(is_inside(dataItem.thePeriod.localStartTime(), thePeriod))
		  {
			if (counter == 0 ||
				dataItem.theWindMaximum.value() > 
				dataItemMinMax.theWindMaximum.value())
			  {
				dataItemMinMax.theWindMaximum = dataItem.theWindMaximum;
			  }
			if (counter == 0 ||
				dataItem.theWindSpeedMin.value() < 
				dataItemMinMax.theWindSpeedMin.value())
			  {
				dataItemMinMax.theWindSpeedMin = dataItem.theWindSpeedMin;
			  }
			
			medianValueSum += dataItem.theWindSpeedMedian.value();
			medianErrorSum += dataItem.theWindSpeedMedian.error();
			counter++;
		  }
	  }

	if(counter == 0)
	  return sentence;

	// claculate average value of median time series
	WeatherResult medianResult(medianValueSum / counter, medianErrorSum / counter);

	sentence <<  speedRangeSentence(thePeriod,
									dataItemMinMax.theWindMaximum,
									medianResult,
									theParameters.theVar,
									theLastSentenceFlag);

	return sentence;
  }

  const Sentence WindForecast::windDirectionSentence(const wind_direction_id& theWindDirectionId) const
  {
	Sentence sentence;

	switch(theWindDirectionId)
	  {
	  case POHJOINEN:
		sentence << "1-tuulta";
		break;
	  case POHJOISEN_PUOLEINEN:
		sentence << "1-puoleista tuulta";
		break;
	  case POHJOINEN_KOILLINEN:
		sentence << "pohjoisen ja koillisen valista tuulta";
		break;
	  case KOILLINEN:
		sentence << "2-tuulta";
		break;
	  case KOILLISEN_PUOLEINEN:
		sentence << "2-puoleista tuulta";
		break;
	  case ITA_KOILLINEN:
		sentence << "idan ja koillisen valista tuulta";
		break;
	  case ITA:
		sentence << "3-tuulta";
		break;
	  case IDAN_PUOLEINEN:
		sentence << "3-puoleista tuulta";
		break;
	  case ITA_KAAKKO:
		sentence << "idan ja kaakon valista tuulta";
		break;
	  case KAAKKO:
		sentence << "4-tuulta";
		break;
	  case KAAKON_PUOLEINEN:
		sentence << "4-puoleista tuulta";
		break;
	  case ETELA_KAAKKO:
		sentence << "etelan ja kaakon valista tuulta";
		break;
	  case ETELA:
		sentence << "5-tuulta";
		break;
	  case ETELAN_PUOLEINEN:
		sentence << "5-puoleista tuulta";
		break;
	  case ETELA_LOUNAS:
		sentence << "etelan ja lounaan valista tuulta";
		break;
	  case LOUNAS:
		sentence << "6-tuulta";
		break;
	  case LOUNAAN_PUOLEINEN:
		sentence << "6-puoleista tuulta";
		break;
	  case LANSI_LOUNAS:
		sentence << "lannen ja lounaan valista tuulta";
		break;
	  case LANSI:
		sentence << "7-tuulta";
		break;
	  case LANNEN_PUOLEINEN:
		sentence << "7-puoleista tuulta";
		break;
	  case LANSI_LUODE:
		sentence << "lannen ja luoteen valista tuulta";
		break;
	  case LUODE:
		sentence << "8-tuulta";
		break;
	  case LUOTEEN_PUOLEINEN:
		sentence << "8-puoleista tuulta";
		break;
	  case POHJOINEN_LUODE:
		sentence << "pohjoisen ja luoteen valista tuulta";
		break;
	  case VAIHTELEVA:
	  case MISSING_WIND_DIRECTION_ID:
		sentence << "suunnaltaan vaihtelevaa tuulta";
		break;
	  }

	return sentence;
  }

  const Sentence WindForecast::windSpeedDirectionSentence(const WindEventPeriodDataItem* theWindSpeedItem,
														  const WindEventPeriodDataItem* theWindDirectionItem) const
  {
	Sentence sentence;

	if(theWindDirectionItem)
	  {
		sentence << windDirectionSentence(get_wind_direction_id(theWindDirectionItem->thePeriodBeginDataItem.theWindDirection,
															theParameters.theVar));
	  }

	if(theWindDirectionItem && theWindSpeedItem)
	  sentence << Delimiter(COMMA_PUNCTUATION_MARK);
		
	if(theWindSpeedItem)
	  sentence << windSpeedSentence(theWindSpeedItem->thePeriod);

	return sentence;
  }

  const float WindDataItemUnit::getWindSpeedShare(const float& theLowerLimit, const float& theUpperLimit) const
  {
	float retval = 0.0;
	
	for(unsigned int i = 0; i < theWindSpeedDistribution.size(); i++)
	  {
		if(theWindSpeedDistribution[i].first >= theLowerLimit &&
		   theWindSpeedDistribution[i].first < theUpperLimit)
		  retval += theWindSpeedDistribution[i].second.value();
	  }
	
	return retval;
  }

}
