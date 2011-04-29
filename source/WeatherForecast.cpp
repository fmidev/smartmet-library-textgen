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
#include "MathTools.h"
#include "SeasonTools.h"
#include "SubMaskExtractor.h"
#include "WeatherForecast.h"

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

 const char* weather_event_string(const weather_event_id& theWeatherEventId)
  {
	const char* retval = "";

	switch(theWeatherEventId)
	  {
	  case PILVISTYY:
		retval = PILVISTYVAA_WORD;
		break;
	  case SELKENEE:
		retval = SELKENEVAA_WORD;
		break;
	  case POUTAANTUU:
		retval = SAA_POUTAANTUU_PHRASE;
		break;
	  case SADE_ALKAA:
		retval = SADE_ALKAA_PHRASE;
		break;
	  default:
		retval = "missing weather event";
		break;
	  }

	return retval;
  }

  const char* precipitation_form_string(const precipitation_form_id& thePrecipitationForm)
  {
	const char* retval = "";

	switch(thePrecipitationForm)
	  {
	  case WATER_FORM:
		retval = "WATER";
		break;
	  case DRIZZLE_FORM:
		retval = "DRIZZLE";
		break;
	  case SLEET_FORM:
		retval = "SLEET";
		break;
	  case SNOW_FORM:
		retval = "SNOW";
		break;
	  case FREEZING_FORM:
		retval = "FREEZING";
		break;
	  case WATER_DRIZZLE_FORM:
		retval = "WATER_DRIZZLE";
		break;
	  case WATER_SLEET_FORM:
		retval = "WATER_SLEET";
		break;
	  case WATER_SNOW_FORM:
		retval = "WATER_SNOW";
		break;
	  case WATER_FREEZING_FORM:
		retval = "WATER_FREEZING";
		break;
	  case WATER_DRIZZLE_SLEET_FORM:
		retval = "WATER_DRIZZLE_SLEET";
		break;
	  case WATER_DRIZZLE_SNOW_FORM:
		retval = "WATER_DRIZZLE_SNOW";
		break;
	  case WATER_DRIZZLE_FREEZING_FORM:
		retval = "WATER_DRIZZLE_FREEZING";
		break;
	  case WATER_SLEET_SNOW_FORM:
		retval = "WATER_SLEET_SNOW";
		break;
	  case WATER_SLEET_FREEZING_FORM:
		retval = "WATER_SLEET_FREEZING";
		break;
	  case WATER_SNOW_FREEZING_FORM:
		retval = "WATER_SNOW_FREEZING";
		break;
	  case DRIZZLE_SLEET_FORM:
		retval = "DRIZZLE_SLEET";
		break;
	  case DRIZZLE_SNOW_FORM:
		retval = "DRIZZLE_SNOW";
		break;
	  case DRIZZLE_FREEZING_FORM:
		retval = "DRIZZLE_FREEZING";
		break;
	  case DRIZZLE_SLEET_SNOW_FORM:
		retval = "DRIZZLE_SLEET_SNOW";
		break;
	  case DRIZZLE_SLEET_FREEZING_FORM:
		retval = "DRIZZLE_SLEET_FREEZING";
		break;
	  case DRIZZLE_SNOW_FREEZING_FORM:
		retval = "DRIZZLE_SNOW_FREEZING";
		break;
	  case SLEET_SNOW_FORM:
		retval = "SLEET_SNOW";
		break;
	  case SLEET_FREEZING_FORM:
		retval = "SLEET_FREEZING";
		break;
	  case SLEET_SNOW_FREEZING_FORM:
		retval = "SLEET_SNOW_FREEZING";
		break;
	  case SNOW_FREEZING_FORM:
		retval = "SNOW_FREEZING";
		break;
	  default:
		retval = "MISSING_VALUE";
		break;
	  }
	return retval;
  }

  const char* precipitation_traverse_string(const precipitation_traverse_id& thePrecipitationTraverseId)
  {
	const char* retval = "";

	switch(thePrecipitationTraverseId)
	  {
	  case FROM_SOUTH_TO_NORTH:
		retval = "etelästä pohjoiseen";
		break;
	  case FROM_NORTH_TO_SOUTH:
		retval = "pohjoiseta etelään";
		break;
	  case FROM_EAST_TO_WEST:
		retval = "idästä länteen";
		break;
	  case FROM_WEST_TO_EAST:
		retval = "lännestä itään";
		break;
	  case FROM_NORTHEAST_TO_SOUTHWEST:
		retval = "koillisesta lounaaseen";
		break;
	  case FROM_SOUTHWEST_TO_NORTHEAST:
		retval = "lounaasta koilliseen";
		break;
	  case FROM_NORTHWEST_TO_SOUTHEAST:
		retval = "luoteesta kaakkoon";
		break;
	  case FROM_SOUTHEAST_TO_NORTHWEST:
		retval = "kaakosta luoteeseen";
		break;
	  case MISSING_TRAVERSE_ID:
		retval = "ei minnekään";
		break;

		 }

	return retval;
  }

  const char* part_of_the_day_string(const part_of_the_day_id& thePartOfTheDayId)
  {
	const char* retval = "";

	switch(thePartOfTheDayId)
	  {
	  case AAMU:
		retval = "aamu";
		break;
	  case AAMUPAIVA:
		retval = "aamupäivä";
		break;
	  case ILTAPAIVA:
		retval = "iltapäivä";
		break;
	  case ILTA:
		retval = "ilta";
		break;
	  case ILTAYO:
		retval = "iltayö";
		break;
	  case KESKIYO:
		retval = "keskiyö";
		break;
	  case AAMUYO:
		retval = "aamuyö";
		break;
	  case PAIVA:
		retval = "päivä";
		break;
	  case YO:
		retval = "yö";
		break;
	  case AAMU_JA_AAMUPAIVA:
		retval = "aamu- ja aamupäivä";
		break;
	  case ILTAPAIVA_JA_ILTA:
		retval = "iltapäivä ja ilta";
		break;
	  case ILTA_JA_ILTAYO:
		retval = "ilta ja iltayö";
		break;
	  case ILTAYO_JA_KESKIYO:
		retval = "iltayö-ja keskiyö";
		break;
	  case KESKIYO_JA_AAMUYO:
		retval = "keskiyö ja aamu";
		break;
	  case AAMUYO_JA_AAMU:
		retval = "aamuyö ja aamu";
		break;
	  default:
		retval = "missing";
		break;
	  }

	return retval;
  }

  const char* story_part_id_string(const story_part_id& theStoryPartId)
  {
	string retval("");
	//	const char* retval = "";

	switch(theStoryPartId)
	  {
	  case PRECIPITATION_STORY_PART:
		retval = "precipitation";
		break;
 	  case CLOUDINESS_STORY_PART:
		retval = "cloudiness";
		break;
 	  case GETTING_CLOUDY_STORY_PART:
		retval = "getting cloudy";
		break;
 	  case CLEARING_UP_STORY_PART:
		retval = "clearing up";
		break;
 	  case PRECIPITATION_TYPE_CHANGE_STORY_PART:
		retval = "precipitation type change";
		break;
	  default:
		retval = "missing";
		break;
	  };

	retval += " story part";
	
	return retval.c_str();
  }

  part_of_the_day_id get_part_of_the_day_id(const NFmiTime& theTimestamp)
  {	
 	if(theTimestamp.GetHour() >= AAMU_START&& theTimestamp.GetHour() <= AAMU_END)
	  return AAMU;
	else if(theTimestamp.GetHour() >= AAMUPAIVA_START && theTimestamp.GetHour() <= AAMUPAIVA_END)
	  return AAMUPAIVA;
	else if(theTimestamp.GetHour() >= ILTA_START && theTimestamp.GetHour() <= ILTA_END)
	  return ILTA;
	else if(theTimestamp.GetHour() >= ILTAPAIVA_START && theTimestamp.GetHour() <= ILTAPAIVA_END)
	  return ILTAPAIVA;
	else if(theTimestamp.GetHour() >= ILTAYO_START)
	  return ILTAYO;
	else if(theTimestamp.GetHour() <= KESKIYO_END)
	  return KESKIYO;
	else if(theTimestamp.GetHour() >= AAMUYO_START && theTimestamp.GetHour() <= AAMUYO_END)
	  return AAMUYO;

	return MISSING_PART_OF_THE_DAY_ID;
 }

  part_of_the_day_id get_part_of_the_day_id(const WeatherPeriod& thePeriod)
  {
	if(thePeriod.localEndTime().DifferenceInHours(thePeriod.localStartTime()) > 10)
	  return MISSING_PART_OF_THE_DAY_ID;

	bool insideSameDay = thePeriod.localStartTime().GetJulianDay() == thePeriod.localEndTime().GetJulianDay();
	bool successiveDays = false;
	if(thePeriod.localStartTime().GetYear() == thePeriod.localEndTime().GetYear() - 1)
	  {
		successiveDays = thePeriod.localStartTime().GetMonth() == 12 && 
		  thePeriod.localStartTime().GetDay() == 31 &&
		  thePeriod.localEndTime().GetMonth() == 1 && 
		  thePeriod.localEndTime().GetDay() == 1;
	  }
	else
	  {
		successiveDays = thePeriod.localStartTime().GetJulianDay() == thePeriod.localEndTime().GetJulianDay() - 1;
	  }
  
	if(thePeriod.localStartTime().GetHour() >= AAMU_START && 
	   thePeriod.localEndTime().GetHour() <= AAMU_END && insideSameDay)
	  return AAMU;
	else if(thePeriod.localStartTime().GetHour() >= AAMUPAIVA_START && 
			thePeriod.localEndTime().GetHour() <= AAMUPAIVA_END && insideSameDay)
	  return AAMUPAIVA;
	else if(thePeriod.localStartTime().GetHour() >= ILTA_START && 
			thePeriod.localEndTime().GetHour() <= ILTA_END && insideSameDay)
	  return ILTA;
	else if(thePeriod.localStartTime().GetHour() >= ILTAPAIVA_START && 
			thePeriod.localEndTime().GetHour() <= ILTAPAIVA_END && insideSameDay)
	  return ILTAPAIVA;
	else if(thePeriod.localStartTime().GetHour() >= ILTAYO_START &&
			thePeriod.localEndTime().GetHour() >= ILTAYO_END && insideSameDay)
	  return ILTAYO;
	else if(thePeriod.localStartTime().GetHour() >= KESKIYO_START && 
			thePeriod.localEndTime().GetHour() <= KESKIYO_END && insideSameDay)
	  return KESKIYO;
	else if(thePeriod.localStartTime().GetHour() >= AAMUYO_START && 
			thePeriod.localEndTime().GetHour() <= AAMUYO_END && insideSameDay)
	  return AAMUYO;
	else if(thePeriod.localStartTime().GetHour() >= AAMU_START && 
			thePeriod.localEndTime().GetHour() <= AAMUPAIVA_END && insideSameDay)
	  return AAMU_JA_AAMUPAIVA;
	else if(thePeriod.localStartTime().GetHour() >= ILTAPAIVA_START && 
			thePeriod.localEndTime().GetHour() <= ILTA_END && insideSameDay)
	  return ILTAPAIVA_JA_ILTA;
	else if((thePeriod.localStartTime().GetHour() >= ILTA_START &&
			 thePeriod.localEndTime().GetHour() <= ILTAYO_END && insideSameDay))
	  return ILTA_JA_ILTAYO;
	else if(thePeriod.localStartTime().GetHour() >= ILTAYO_START && 
			thePeriod.localEndTime().GetHour() <= KESKIYO_END && successiveDays)
	  return ILTAYO_JA_KESKIYO;
	else if(thePeriod.localStartTime().GetHour() >= KESKIYO_START && 
			thePeriod.localEndTime().GetHour() <= AAMUYO_END && insideSameDay)
	  return KESKIYO_JA_AAMUYO;
	else if(thePeriod.localStartTime().GetHour() >= AAMUYO_START && 
			thePeriod.localEndTime().GetHour() <= AAMU_END && insideSameDay)
	  return AAMUYO_JA_AAMU;
	else if(thePeriod.localStartTime().GetHour() >= YO_START && 
			thePeriod.localEndTime().GetHour() <= YO_END && successiveDays)
	  return YO;
	else if(thePeriod.localStartTime().GetHour() >= PAIVA_START - 2 && 
			thePeriod.localEndTime().GetHour() <= PAIVA_END && insideSameDay)
	  return PAIVA;
	
	return MISSING_PART_OF_THE_DAY_ID;
  }

  void get_part_of_the_day(const part_of_the_day_id& thePartOfTheDayId, int& theStartHour, int& theEndHour)
  {
	switch(thePartOfTheDayId)
	  {
	  case AAMU:
		{
		  theStartHour = AAMU_START;
		  theEndHour = AAMU_END;
		}
		break;
	  case AAMUPAIVA:
		{
		  theStartHour = AAMUPAIVA_START;
		  theEndHour = AAMUPAIVA_END;
		}
		break;
	  case ILTAPAIVA:
		{
		  theStartHour = ILTAPAIVA_START;
		  theEndHour = ILTAPAIVA_END;
		}
		break;
	  case ILTA:
		{
		  theStartHour = ILTA_START;
		  theEndHour = ILTA_END;
		}
		break;
	  case ILTAYO:
		{
		  theStartHour = ILTAYO_START;
		  theEndHour = ILTAYO_END;
		}
		break;
	  case KESKIYO:
		{
		  theStartHour = KESKIYO_START;
		  theEndHour = KESKIYO_END;
		}
		break;
	  case AAMUYO:
		{
		  theStartHour = AAMUYO_START;
		  theEndHour = AAMUYO_END;
		}
		break;
	  case PAIVA:
		{
		  theStartHour = PAIVA_START;
		  theEndHour = PAIVA_END;
		}
		break;
	  case YO:
		{
		  theStartHour = YO_START;
		  theEndHour = YO_END;
		}
		break;
	  case AAMU_JA_AAMUPAIVA:
		{
		  theStartHour = AAMU_START;
		  theEndHour = AAMUPAIVA_END;
		}
		break;
	  case ILTAPAIVA_JA_ILTA:
		{
		  theStartHour = ILTAPAIVA_START;
		  theEndHour = ILTA_END;
		}
		break;
	  case ILTA_JA_ILTAYO:
		{
		  theStartHour = ILTA_START;
		  theEndHour = ILTAYO_END;
		}
		break;
	  case ILTAYO_JA_KESKIYO:
		{
		  theStartHour = ILTAYO_START;
		  theEndHour = KESKIYO_END;
		}
		break;
	  case KESKIYO_JA_AAMUYO:
		{
		  theStartHour = KESKIYO_START;
		  theEndHour = AAMUYO_END;
		}
		break;
	  case AAMUYO_JA_AAMU:
		{
		  theStartHour = AAMUYO_START;
		  theEndHour = AAMU_END;
		}
		break;
	  default:
		{
		  theStartHour = -1;
		  theEndHour = -1;
		}
	  }
  }

  WeatherPeriod intersecting_period(const WeatherPeriod& theWeatherPeriod1,
									const WeatherPeriod& theWeatherPeriod2)
  {
	if(theWeatherPeriod1.localEndTime() < theWeatherPeriod2.localStartTime() ||
	   theWeatherPeriod1.localStartTime() > theWeatherPeriod2.localEndTime())
	  return WeatherPeriod(NFmiTime(1970,1,1), NFmiTime(1970,1,1));

	if(is_inside(theWeatherPeriod1.localStartTime(), theWeatherPeriod2) && 
	   is_inside(theWeatherPeriod1.localEndTime(), theWeatherPeriod2))
	  return theWeatherPeriod1;

	if(is_inside(theWeatherPeriod2.localStartTime(), theWeatherPeriod1) && 
	   is_inside(theWeatherPeriod2.localEndTime(), theWeatherPeriod1))
	  return theWeatherPeriod2;

	NFmiTime startTime(theWeatherPeriod1.localStartTime() > theWeatherPeriod2.localStartTime() ?
					   theWeatherPeriod1.localStartTime() : theWeatherPeriod2.localStartTime());
	NFmiTime endTime(theWeatherPeriod1.localEndTime() < theWeatherPeriod2.localEndTime() ?
					   theWeatherPeriod1.localEndTime() : theWeatherPeriod2.localEndTime());

	return WeatherPeriod(startTime, endTime);
	
  }

  bool is_inside(const NFmiTime& theTimeStamp, 
				 const WeatherPeriod& theWeatherPeriod)
  {
	return theTimeStamp >= theWeatherPeriod.localStartTime() && theTimeStamp <= theWeatherPeriod.localEndTime();
  }

  bool is_inside(const NFmiTime& theTimeStamp, 
				 const part_of_the_day_id& thePartOfTheDayId)
  {
	int startHour, endHour;
	int timestampHour(theTimeStamp.GetHour());
	get_part_of_the_day(thePartOfTheDayId, startHour, endHour);

	if(endHour == 0)
	  return timestampHour >= startHour || endHour == timestampHour;
	else if(startHour == 0)
	  return timestampHour <= endHour;
	else
	  {
		if(thePartOfTheDayId == YO || 
		   thePartOfTheDayId == ILTAYO_JA_KESKIYO)
		  {
			return timestampHour  >= startHour || timestampHour <= endHour;
		  }
		else
		  {
			return(timestampHour >= startHour && 
				   timestampHour <= endHour);
		  }
	  }
  }

  bool is_inside(const WeatherPeriod& theWeatherPeriod,
				 const part_of_the_day_id& thePartOfTheDayId)
  {
	int numberOfDays(theWeatherPeriod.localEndTime().GetJulianDay() -
					 theWeatherPeriod.localStartTime().GetJulianDay());
	if(theWeatherPeriod.localEndTime().DifferenceInHours(theWeatherPeriod.localStartTime()) > 10)
	  {
		return false;
	  }
	else if(numberOfDays == 1)
	  {
		if(thePartOfTheDayId != ILTAYO &&
		   thePartOfTheDayId != YO && 
		   thePartOfTheDayId != ILTAYO_JA_KESKIYO &&
		   thePartOfTheDayId != ILTA_JA_ILTAYO)
		  return false;
	  }

	if(thePartOfTheDayId == YO && 
	   is_inside(theWeatherPeriod.localStartTime(), YO) &&
	   is_inside(theWeatherPeriod.localEndTime(), YO))
	  return true;
	else if(thePartOfTheDayId == ILTAYO_JA_KESKIYO && 
			is_inside(theWeatherPeriod.localStartTime(), ILTAYO_JA_KESKIYO) &&
			is_inside(theWeatherPeriod.localEndTime(), ILTAYO_JA_KESKIYO))
	  return true;

	int startHour, endHour;
	get_part_of_the_day(thePartOfTheDayId, startHour, endHour);
	NFmiTime startTimeCompare(theWeatherPeriod.localStartTime());
	NFmiTime endTimeCompare(theWeatherPeriod.localStartTime());

	startTimeCompare.SetHour(startHour);
	startTimeCompare.SetMin(0);
	startTimeCompare.SetSec(0);
	endTimeCompare.SetHour(endHour);
	endTimeCompare.SetMin(0);
	endTimeCompare.SetSec(0);

	if(endHour == 0)
	  {
		endTimeCompare.ChangeByDays(1);
	  }

	return (theWeatherPeriod.localStartTime() >= startTimeCompare &&
			theWeatherPeriod.localStartTime() <= endTimeCompare &&
			theWeatherPeriod.localEndTime() >= startTimeCompare &&
			theWeatherPeriod.localEndTime() <= endTimeCompare);
  }


  Sentence get_narrow_time_phrase(const WeatherPeriod& theWeatherPeriod,
								  vector<std::string>* theStringVector /*= 0*/)
  {
	Sentence sentence;

	switch(get_part_of_the_day_id(theWeatherPeriod))
	  {
	  case AAMU:
		{
		  sentence << AAMULLA_WORD;
		  if(theStringVector)
			{
			  theStringVector->push_back(AAMULLA_WORD);
			}
		}
		break;
	  case AAMUPAIVA:
		{
		  sentence << AAMUPAIVALLA_WORD;
		  if(theStringVector)
			{
			  theStringVector->push_back(AAMUPAIVALLA_WORD);
			}
		}
		break;
	  case ILTA:
		{
		  sentence << ILLALLA_WORD;
		  if(theStringVector)
			{
			  theStringVector->push_back(ILLALLA_WORD);
			}
		}
		break;
	  case ILTAPAIVA:
		{
		  sentence << ILTAPAIVALLA_WORD;
		  if(theStringVector)
			{
			  theStringVector->push_back(ILTAPAIVALLA_WORD);
			}
		}
		break;
	  case ILTAYO:
		{
		  sentence << ILTAYOLLA_WORD;
		  if(theStringVector)
			{
			  theStringVector->push_back(ILTAYOLLA_WORD);
			}
		}
		break;
	  case KESKIYO:
		{
		  sentence << KESKIYOLLA_WORD;
		  if(theStringVector)
			{
			  theStringVector->push_back(KESKIYOLLA_WORD);
			}
		}
		break;
	  case AAMUYO:
		{
		  sentence << AAMUYOLLA_WORD;
		  if(theStringVector)
			{
			  theStringVector->push_back(AAMUYOLLA_WORD);
			}
		}
		break;
	  default:
		break;
	  };

	return sentence;
  }

  Sentence get_large_time_phrase(const WeatherPeriod& theWeatherPeriod,
								 short& theWeekday,
								 vector<std::string>* theStringVector /*= 0*/)
  {
	Sentence sentence;

	theWeekday = theWeatherPeriod.localStartTime().GetWeekday();

	if(is_inside(theWeatherPeriod, AAMU_JA_AAMUPAIVA))
	  {			
		sentence << AAMULLA_WORD << JA_WORD << AAMUPAIVALLA_WORD;
		if(theStringVector)
		  {
			theStringVector->push_back(AAMULLA_WORD);
			theStringVector->push_back(JA_WORD);
			theStringVector->push_back(AAMUPAIVALLA_WORD);
		  }
	  }
	else if(is_inside(theWeatherPeriod, ILTAPAIVA_JA_ILTA))
	  {
		sentence << ILTAPAIVALLA_WORD << JA_WORD << ILLALLA_WORD;
		if(theStringVector)
		  {
			theStringVector->push_back(ILTAPAIVALLA_WORD);
			theStringVector->push_back(JA_WORD);
			theStringVector->push_back(ILLALLA_WORD);
		  }
	  }
	else if(is_inside(theWeatherPeriod, ILTA_JA_ILTAYO))
	  {
		sentence << ILLALLA_WORD << JA_WORD << ILTAYOLLA_WORD;
		if(theStringVector)
		  {
			theStringVector->push_back(ILLALLA_WORD);
			theStringVector->push_back(JA_WORD);
			theStringVector->push_back(ILTAYOLLA_WORD);
		  }
	  }
	else if(is_inside(theWeatherPeriod, ILTAYO_JA_KESKIYO))
	  {
		theWeekday = 0;
		sentence << ILTAYOLLA_WORD << JA_WORD << KESKIYOLLA_WORD;
		if(theStringVector)
		  {
			theStringVector->push_back(ILTAYOLLA_WORD);
			theStringVector->push_back(JA_WORD);
			theStringVector->push_back(KESKIYOLLA_WORD);
		  }
	  }
	else if(is_inside(theWeatherPeriod, KESKIYO_JA_AAMUYO))
	  {
		theWeekday = 0;
		sentence << KESKIYOLLA_WORD << JA_WORD << AAMUYOLLA_WORD;
		if(theStringVector)
		  {
			theStringVector->push_back(KESKIYOLLA_WORD);
			theStringVector->push_back(JA_WORD);
			theStringVector->push_back(AAMUYOLLA_WORD);
		  }
	  }
	else if(is_inside(theWeatherPeriod, AAMUYO_JA_AAMU))
	  {
		sentence << AAMUYOLLA_WORD << JA_WORD << AAMULLA_WORD;
		if(theStringVector)
		  {
			theStringVector->push_back(AAMUYOLLA_WORD);
			theStringVector->push_back(JA_WORD);
			theStringVector->push_back(AAMULLA_WORD);
		  }
	  }
	else if(is_inside(theWeatherPeriod, YO))
	  {
		theWeekday = 0;
		sentence << YOLLA_WORD;
		if(theStringVector)
		  {
			theStringVector->push_back(YOLLA_WORD);
		  }
	  }
	else if(is_inside(theWeatherPeriod, PAIVA))
	  {
		sentence << PAIVALLA_WORD;
		if(theStringVector)
		  {
			theStringVector->push_back(PAIVALLA_WORD);
		  }
	  }
 
	return sentence;
  }


  std::string parse_time_phrase(const short& theWeekday, 
								const bool& theSpecifyDayFlag,
								vector<std::string>& theTimePhraseVector)
  {
	std::ostringstream oss;

	if(theWeekday > 0 && theSpecifyDayFlag)
	  oss << theWeekday << "-";
	for(unsigned int i = 0; i < theTimePhraseVector.size(); i++)
	  {
		oss << theTimePhraseVector[i];
		if(i <  theTimePhraseVector.size() - 1)
		  oss << " ";
	  }
	return oss.str();
  }

  Sentence get_time_phrase_large(const WeatherPeriod& theWeatherPeriod,
								 const bool& theSpecifyDayFlag,
								 const std::string& theVar,
								 bool theAlkaenPhrase /*= false*/,
								 vector<std::string>* theStringVector /*= 0*/)
  {
	Sentence sentence;

	bool specify_part_of_the_day = Settings::optional_bool(theVar + "::specify_part_of_the_day", true);

	if(!specify_part_of_the_day)
	  return sentence;

	vector<std::string> localStringVector;
	short weekday(theWeatherPeriod.localStartTime().GetWeekday());

	if(theWeatherPeriod.localStartTime().GetJulianDay() == 
	   theWeatherPeriod.localEndTime().GetJulianDay() &&
	   get_part_of_the_day_id(theWeatherPeriod.localStartTime()) == 
	   get_part_of_the_day_id(theWeatherPeriod.localEndTime()))
	  {
		get_time_phrase(theWeatherPeriod.localStartTime(), theVar, false, &localStringVector);
		if(localStringVector.size() > 0)
		  sentence << parse_time_phrase(weekday, theSpecifyDayFlag, localStringVector);
	  }
	else
	  {
		get_narrow_time_phrase(theWeatherPeriod, &localStringVector);
		if(localStringVector.size() > 0)
		  {
			sentence << parse_time_phrase(weekday, theSpecifyDayFlag, localStringVector);
		  }
		else
		  {
			if(theWeatherPeriod.localEndTime().DifferenceInHours(theWeatherPeriod.localStartTime()) > 2)
			  {
				// 1 hour tolerance
				NFmiTime startTime(theWeatherPeriod.localStartTime());
				NFmiTime endTime(theWeatherPeriod.localEndTime());
				startTime.ChangeByHours(1);
				endTime.ChangeByHours(-1);
				WeatherPeriod narrowerPeriod(startTime, endTime);
				
				if(endTime.DifferenceInHours(startTime) >= 4)
				  {
					get_narrow_time_phrase(narrowerPeriod, &localStringVector);
					if(localStringVector.size() > 0)
					  {
						sentence << parse_time_phrase(narrowerPeriod.localStartTime().GetWeekday(),
													  theSpecifyDayFlag,
													  localStringVector);
					  }
				  }

				if(sentence.size() == 0)
				  {
					get_large_time_phrase(theWeatherPeriod, weekday, &localStringVector);
					if(localStringVector.size() > 0)
					  {
						sentence << parse_time_phrase(weekday, theSpecifyDayFlag, localStringVector);
					  }

					if(sentence.size() == 0)
					  {
						get_large_time_phrase(narrowerPeriod, weekday, &localStringVector);
						if(localStringVector.size() > 0)
						  {
							sentence << parse_time_phrase(weekday, theSpecifyDayFlag, localStringVector);
						  }
					  }
				  }
			  }
			else
			  {
				get_large_time_phrase(theWeatherPeriod, weekday, &localStringVector);
				if(localStringVector.size() > 0)
				  {
					sentence << parse_time_phrase(weekday, theSpecifyDayFlag, localStringVector);
				  }
			  }

			if(sentence.size() == 0 && theAlkaenPhrase)
			  {
				get_time_phrase(theWeatherPeriod.localStartTime(), theVar, theAlkaenPhrase, &localStringVector);
				if(localStringVector.size() > 0)
				  {
					sentence << parse_time_phrase(weekday, theSpecifyDayFlag, localStringVector);
				  }
			  }
		  }
	  }

	if(theStringVector)
	  {
		for(unsigned int i = 0; i < localStringVector.size(); i++)
		  theStringVector->push_back(localStringVector.at(i));
	  }

	return sentence;
  }


#ifdef TEMPORARILY_BLOCKED
  Sentence get_time_phrase_large(const WeatherPeriod& theWeatherPeriod,
								 const std::string& theVar,
								 bool theAlkaenPhrase /*= false*/,
								 vector<std::string>* theStringVector /*= 0*/)
  {
	Sentence sentence;

	bool specify_part_of_the_day = Settings::optional_bool(theVar + "::specify_part_of_the_day", true);

	if(!specify_part_of_the_day)
	  return sentence;

	if(theWeatherPeriod.localStartTime().GetJulianDay() == 
	   theWeatherPeriod.localEndTime().GetJulianDay() &&
	   get_part_of_the_day_id(theWeatherPeriod.localStartTime()) == 
	   get_part_of_the_day_id(theWeatherPeriod.localEndTime()))
	  {
		sentence << get_time_phrase(theWeatherPeriod.localStartTime(), theVar, false, theStringVector);
	  }
	else
	  {
		sentence << get_narrow_time_phrase(theWeatherPeriod, theStringVector);

		if(sentence.size() == 0)
		  {
			if(theWeatherPeriod.localEndTime().DifferenceInHours(theWeatherPeriod.localStartTime()) > 2)
			  {
				// 1 hour tolerance
				NFmiTime startTime(theWeatherPeriod.localStartTime());
				NFmiTime endTime(theWeatherPeriod.localEndTime());
				startTime.ChangeByHours(1);
				endTime.ChangeByHours(-1);
				WeatherPeriod narrowerPeriod(startTime, endTime);
			
			
				if(endTime.DifferenceInHours(startTime) >= 4)
				  sentence << get_narrow_time_phrase(narrowerPeriod, theStringVector);

				if(sentence.size() == 0)
				  {
					sentence << get_large_time_phrase(theWeatherPeriod, theStringVector);

					if(sentence.size() == 0)
					  sentence << get_large_time_phrase(narrowerPeriod, theStringVector);
				  }
			  }
			else
			  {
				sentence << get_large_time_phrase(theWeatherPeriod, theStringVector);
			  }

			if(sentence.size() == 0 && theAlkaenPhrase)
			  {
				sentence << get_time_phrase(theWeatherPeriod.localStartTime(), theVar, theAlkaenPhrase, theStringVector);
			  }






#ifdef OLD_IMPLEMENTATION

			if(is_inside(theWeatherPeriod, AAMU_JA_AAMUPAIVA))
			  {			
				sentence << AAMULLA_WORD << JA_WORD << AAMUPAIVALLA_WORD;
				if(theStringVector)
				  {
					theStringVector->push_back(AAMULLA_WORD);
					theStringVector->push_back(JA_WORD);
					theStringVector->push_back(AAMUPAIVALLA_WORD);
				  }
			  }
			else if(is_inside(theWeatherPeriod, ILTAPAIVA_JA_ILTA))
			  {
				sentence << ILTAPAIVALLA_WORD << JA_WORD << ILLALLA_WORD;
				if(theStringVector)
				  {
					theStringVector->push_back(ILTAPAIVALLA_WORD);
					theStringVector->push_back(JA_WORD);
					theStringVector->push_back(ILLALLA_WORD);
				  }
			  }
			else if(is_inside(theWeatherPeriod, ILTA_JA_ILTAYO))
			  {
				sentence << ILLALLA_WORD << JA_WORD << ILTAYOLLA_WORD;
				if(theStringVector)
				  {
					theStringVector->push_back(ILLALLA_WORD);
					theStringVector->push_back(JA_WORD);
					theStringVector->push_back(ILTAYOLLA_WORD);
				  }
			  }
			else if(is_inside(theWeatherPeriod, ILTAYO_JA_KESKIYO))
			  {
				sentence << ILTAYOLLA_WORD << JA_WORD << KESKIYOLLA_WORD;
				if(theStringVector)
				  {
					theStringVector->push_back(ILTAYOLLA_WORD);
					theStringVector->push_back(JA_WORD);
					theStringVector->push_back(KESKIYOLLA_WORD);
				  }
			  }
			else if(is_inside(theWeatherPeriod, KESKIYO_JA_AAMUYO))
			  {
				sentence << KESKIYOLLA_WORD << JA_WORD << AAMUYOLLA_WORD;
				if(theStringVector)
				  {
					theStringVector->push_back(KESKIYOLLA_WORD);
					theStringVector->push_back(JA_WORD);
					theStringVector->push_back(AAMUYOLLA_WORD);
				  }
			  }
			else if(is_inside(theWeatherPeriod, AAMUYO_JA_AAMU))
			  {
				sentence << AAMUYOLLA_WORD << JA_WORD << AAMULLA_WORD;
				if(theStringVector)
				  {
					theStringVector->push_back(AAMUYOLLA_WORD);
					theStringVector->push_back(JA_WORD);
					theStringVector->push_back(AAMULLA_WORD);
				  }
			  }
			else if(is_inside(theWeatherPeriod, YO))
			  {
				sentence << YOLLA_WORD;
				if(theStringVector)
				  {
					theStringVector->push_back(YOLLA_WORD);
				  }
			  }
			else if(is_inside(theWeatherPeriod, PAIVA))
			  {
				sentence << PAIVALLA_WORD;
				if(theStringVector)
				  {
					theStringVector->push_back(PAIVALLA_WORD);
				  }
			  }
			else if(theAlkaenPhrase)
			  {
				sentence << get_time_phrase(theWeatherPeriod.localStartTime(), theVar, theAlkaenPhrase, theStringVector);
			  }
#endif
		  }
	  }

	return sentence;
  }
#endif


  Sentence get_time_phrase(const NFmiTime& theTimestamp,
						   const std::string& theVar,
						   bool theAlkaenPhrase /*= false*/, 
						   vector<std::string>* theStringVector /*= 0*/)
  {
	Sentence sentence;

	bool specify_part_of_the_day = Settings::optional_bool(theVar + "::specify_part_of_the_day", true);

	if(!specify_part_of_the_day)
	  return sentence;

	if(is_inside(theTimestamp, AAMU))
	  {
		sentence << (theAlkaenPhrase ? AAMUSTA_ALKAEN_PHRASE : AAMULLA_WORD);
		if(theStringVector)
		  {
			theStringVector->push_back(theAlkaenPhrase ? AAMUSTA_ALKAEN_PHRASE : AAMULLA_WORD);
		  }
	  }
	else if(is_inside(theTimestamp, AAMUPAIVA))
	  {
		sentence << (theAlkaenPhrase ? AAMUPAIVASTA_ALKAEN_PHRASE : AAMUPAIVALLA_WORD);
		if(theStringVector)
		  {
			theStringVector->push_back(theAlkaenPhrase ? AAMUPAIVASTA_ALKAEN_PHRASE : AAMUPAIVALLA_WORD);
		  }
	  }
	else if(is_inside(theTimestamp, ILTA))
	  {
		sentence << (theAlkaenPhrase ? ILLASTA_ALKAEN_PHRASE : ILLALLA_WORD);
		if(theStringVector)
		  {
			theStringVector->push_back(theAlkaenPhrase ? ILLASTA_ALKAEN_PHRASE : ILLALLA_WORD);
		  }
	  }
	else if(is_inside(theTimestamp, ILTAPAIVA))
	  {
		sentence << (theAlkaenPhrase ? ILTAPAIVASTA_ALKAEN_PHRASE : ILTAPAIVALLA_WORD);
		if(theStringVector)
		  {
			theStringVector->push_back(theAlkaenPhrase ? ILTAPAIVASTA_ALKAEN_PHRASE : ILTAPAIVALLA_WORD);
		  }
	  }
	else if(is_inside(theTimestamp, ILTAYO))
	  {
		sentence << (theAlkaenPhrase ? ILTAYOSTA_ALKAEN_PHRASE : ILTAYOLLA_WORD);
		if(theStringVector)
		  {
			theStringVector->push_back(theAlkaenPhrase ? ILTAYOSTA_ALKAEN_PHRASE : ILTAYOLLA_WORD);
		  }
	  }
	else if(is_inside(theTimestamp, KESKIYO))
	  {
		sentence << (theAlkaenPhrase ? KESKIYOSTA_ALKAEN_PHRASE : KESKIYOLLA_WORD);
		if(theStringVector)
		  {
			theStringVector->push_back(theAlkaenPhrase ? KESKIYOSTA_ALKAEN_PHRASE : KESKIYOLLA_WORD);
		  }
	  }
	else if(is_inside(theTimestamp, AAMUYO))
	  {
		sentence << (theAlkaenPhrase ? AAMUYOSTA_ALKAEN_PHRASE : AAMUYOLLA_WORD);
		if(theStringVector)
		  {
			theStringVector->push_back(theAlkaenPhrase ? AAMUYOSTA_ALKAEN_PHRASE : AAMUYOLLA_WORD);
		  }
	  }

	return sentence;
  }

  Sentence get_direction_phrase(const direction_id& theDirectionId, bool theAlkaenPhrase /*= false*/)
  {
	Sentence sentence;

	switch(theDirectionId)
	  {
	  case NORTH:
		sentence << (theAlkaenPhrase ? POHJOISESTA_ALKAEN_PHRASE : POHJOISESTA_WORD);
		break;
	  case SOUTH:
		sentence << (theAlkaenPhrase ? ETELASTA_ALKAEN_PHRASE : ETELASTA_WORD);
		break;
	  case EAST:
		sentence << (theAlkaenPhrase ? IDASTA_ALKAEN_PHRASE : IDASTA_WORD);
		break;
	  case WEST:
		sentence << (theAlkaenPhrase ? LANNESTA_ALKAEN_PHRASE : LANNESTA_WORD);
		break;
	  case NORTHEAST:
		sentence << (theAlkaenPhrase ? KOILLISESTA_ALKAEN_PHRASE : KOILLISESTA_WORD);
		break;
	  case SOUTHEAST:
		sentence << (theAlkaenPhrase ? KAAKOSTA_ALKAEN_PHRASE : KAAKOSTA_WORD);
		break;
	  case SOUTHWEST:
		sentence << (theAlkaenPhrase ? LOUNAASTA_ALKAEN_PHRASE : LOUNAASTA_WORD);
		break;
	  case NORTHWEST:
		sentence << (theAlkaenPhrase ? LUOTEESTA_ALKAEN_PHRASE : LUOTEESTA_WORD);
		break;
	  case NO_DIRECTION :
		break;
	  }
 
	return sentence;
 }

  int get_period_length(const WeatherPeriod& thePeriod)
  {
	return thePeriod.localEndTime().DifferenceInHours(thePeriod.localStartTime());
  }

  // returns the julian day of the first day period
  int get_today_vector(const string& theVariable,
					   const WeatherArea& theArea,
					   const WeatherPeriod& thePeriod,
					   const NFmiTime& theForecastTime,
					   vector<Sentence*>& theTodayVector)
  {
	int retval = -1;

	NightAndDayPeriodGenerator generator(thePeriod, theVariable);
	
	
	for(unsigned int i = 0; i < generator.size(); i++)
	  {
		WeatherPeriod wp(generator.period(i+1));
		if(i == 0 ||
		   generator.period(i).localStartTime().GetJulianDay() !=
		   wp.localStartTime().GetJulianDay())
		  {
			Sentence* sentence = new Sentence();
			*sentence  << PeriodPhraseFactory::create("today",
													  theVariable,
													  theForecastTime,
													  generator.period(i+1),
													  theArea);
			theTodayVector.push_back(sentence);

			if(i == 0)
			  retval = wp.localStartTime().GetJulianDay();
		  }
	  }
	return retval;
  }
  

  Sentence get_today_phrase(const NFmiTime& theEventTimestamp,
							const string& theVariable,
							const WeatherArea& theArea,
							const WeatherPeriod thePeriod,
							const NFmiTime& theForecastTime)
  {
	Sentence sentence;

	part_of_the_day_id partOfTheDayId = get_part_of_the_day_id(theEventTimestamp);

	if(partOfTheDayId == KESKIYO && theEventTimestamp.DifferenceInHours(theForecastTime) <= 24)
	  {
		return sentence;
	  }
	else
	  {
		if(partOfTheDayId == KESKIYO ||
		   partOfTheDayId == AAMUYO)
		  {
			const char* which_day ="";
			if(theEventTimestamp.GetJulianDay() == theForecastTime.GetJulianDay())
			  which_day = "tonight";
			else
			  which_day = "next_night";

			sentence << PeriodPhraseFactory::create(which_day,
													theVariable,
													theForecastTime,
													thePeriod,
													theArea);
		  }
		else
		  {
			const char* which_day ="";
			if(theEventTimestamp.GetJulianDay() == theForecastTime.GetJulianDay())
			  which_day = "today";
			else
			  which_day = "next_day";

			sentence << PeriodPhraseFactory::create(which_day,
													theVariable,
													theForecastTime,
													thePeriod,
													theArea);
		  }
	  }
	
	return sentence;
  }

  void get_precipitation_limit_value(const wf_story_params& theParameters,
									 const unsigned int& thePrecipitationForm,
									 const precipitation_intesity_id& theIntensityId,
									 float& theLowerLimit,
									 float& theUpperLimit)
  {
 	switch(thePrecipitationForm)
	  {
	  case WATER_FREEZING_FORM:
	  case FREEZING_FORM:
	  case WATER_FORM:
	  case WATER_DRIZZLE_FREEZING_FORM:
	  case WATER_DRIZZLE_FORM:
	  case WATER_DRIZZLE_SLEET_FORM:
	  case WATER_SLEET_FREEZING_FORM:
	  case WATER_SLEET_FORM:
	  case WATER_SNOW_FREEZING_FORM:
	  case WATER_SNOW_FORM:
	  case WATER_DRIZZLE_SNOW_FORM:
	  case WATER_SLEET_SNOW_FORM:
		{
		  switch(theIntensityId)
			{
			case DRY_WEATHER:
			  {
				theLowerLimit = 0;
				theUpperLimit = theParameters.theDryWeatherLimitWater;
			  }
			  break;
			case WEAK_PRECIPITATION:
			  {
				theLowerLimit = theParameters.theDryWeatherLimitWater;
				theUpperLimit = theParameters.theWeakPrecipitationLimitWater;
			  }
			  break;
			case MODERATE_PRECIPITATION:
			  {
				theLowerLimit = theParameters.theWeakPrecipitationLimitWater;
				theUpperLimit = theParameters.theHeavyPrecipitationLimitWater;
			  }
			  break;
			case HEAVY_PRECIPITATION:
			  {
				theLowerLimit = theParameters.theHeavyPrecipitationLimitWater;
				theUpperLimit = 1000.0;
			  }
			  break;
			default:
			  break;
			}
		}
		break;
	  case SLEET_FREEZING_FORM:
	  case SLEET_FORM:
	  case SLEET_SNOW_FREEZING_FORM:
	  case SLEET_SNOW_FORM:
		{
		  switch(theIntensityId)
			{
			case DRY_WEATHER:
			  {
				theLowerLimit = 0;
				theUpperLimit = theParameters.theDryWeatherLimitSleet;
			  }
			  break;
			case WEAK_PRECIPITATION:
			  {
				theLowerLimit = theParameters.theDryWeatherLimitSleet;
				theUpperLimit = theParameters.theWeakPrecipitationLimitSleet;
			  }
			  break;
			case MODERATE_PRECIPITATION:
			  {
				theLowerLimit = theParameters.theWeakPrecipitationLimitSleet;
				theUpperLimit = theParameters.theHeavyPrecipitationLimitSleet;
			  }
			  break;
			case HEAVY_PRECIPITATION:
			  {
				theLowerLimit = theParameters.theHeavyPrecipitationLimitSleet;
				theUpperLimit = 1000.0;
			  }
			  break;
			default:
			  break;
			}
		}
		break;
	  case SNOW_FORM:
	  case SNOW_FREEZING_FORM:
		{
		  switch(theIntensityId)
			{
			case DRY_WEATHER:
			  {
				theLowerLimit = 0;
				theUpperLimit = theParameters.theDryWeatherLimitSnow;
			  }
			  break;
			case WEAK_PRECIPITATION:
			  {
				theLowerLimit = theParameters.theDryWeatherLimitSnow;
				theUpperLimit = theParameters.theWeakPrecipitationLimitSnow;
			  }
			  break;
			case MODERATE_PRECIPITATION:
			  {
				theLowerLimit = theParameters.theWeakPrecipitationLimitSnow;
				theUpperLimit = theParameters.theHeavyPrecipitationLimitSnow;
			  }
			  break;
			case HEAVY_PRECIPITATION:
			  {
				theLowerLimit = theParameters.theHeavyPrecipitationLimitSnow;
				theUpperLimit = 1000.0;
			  }
			  break;
			default:
			  break;
			}
		}
		break;
	  case DRIZZLE_FORM:
	  case DRIZZLE_FREEZING_FORM:
	  case DRIZZLE_SLEET_FORM:
	  case DRIZZLE_SLEET_FREEZING_FORM:
	  case DRIZZLE_SNOW_FREEZING_FORM:
	  case DRIZZLE_SNOW_FORM:
	  case DRIZZLE_SLEET_SNOW_FORM:
		{
		  switch(theIntensityId)
			{
			case DRY_WEATHER:
			  {
				theLowerLimit = 0;
				theUpperLimit = theParameters.theDryWeatherLimitDrizzle;
			  }
			  break;
			case WEAK_PRECIPITATION:
			  {
				theLowerLimit = theParameters.theDryWeatherLimitWater;
				theUpperLimit = theParameters.theWeakPrecipitationLimitWater;
			  }
			  break;
			case MODERATE_PRECIPITATION:
			  {
				theLowerLimit = theParameters.theWeakPrecipitationLimitWater;
				theUpperLimit = theParameters.theHeavyPrecipitationLimitWater;
			  }
			  break;
			case HEAVY_PRECIPITATION:
			  {
				theLowerLimit = theParameters.theHeavyPrecipitationLimitWater;
				theUpperLimit = 1000.0;
			  }
			  break;
			default:
			  break;
			}
		}
		break;
	  case MISSING_PRECIPITATION_FORM:
		break;
	  }
  }

   void get_dry_and_weak_precipitation_limit(const wf_story_params& theParameters,
											const unsigned int& thePrecipitationForm,
											float& theDryWeatherLimit, 
											float& theWeakPrecipitationLimit)
  {
	switch(thePrecipitationForm)
	  {
	  case WATER_FREEZING_FORM:
	  case FREEZING_FORM:
	  case WATER_FORM:
		{
		  theDryWeatherLimit = theParameters.theDryWeatherLimitWater;
		  theWeakPrecipitationLimit = theParameters.theWeakPrecipitationLimitWater;
		}
		break;
	  case SLEET_FREEZING_FORM:
	  case SLEET_FORM:
		{
		  theDryWeatherLimit = theParameters.theDryWeatherLimitSleet;
		  theWeakPrecipitationLimit = theParameters.theWeakPrecipitationLimitSleet;
		}
		break;
	  case SNOW_FORM:
	  case SNOW_FREEZING_FORM:
		{
		  theDryWeatherLimit = theParameters.theDryWeatherLimitSnow;
		  theWeakPrecipitationLimit = theParameters.theWeakPrecipitationLimitSnow;
		}
		break;
	  case DRIZZLE_FORM:
	  case DRIZZLE_FREEZING_FORM:
	  case WATER_DRIZZLE_FREEZING_FORM:
	  case WATER_DRIZZLE_FORM:
		{
		  theDryWeatherLimit = theParameters.theDryWeatherLimitDrizzle;
		  theWeakPrecipitationLimit = theParameters.theWeakPrecipitationLimitWater;
		}
		break;
	  case DRIZZLE_SLEET_FORM:
	  case DRIZZLE_SLEET_FREEZING_FORM:
	  case WATER_DRIZZLE_SLEET_FORM:
	  case WATER_SLEET_FREEZING_FORM:
	  case WATER_SLEET_FORM:
		{
		  theDryWeatherLimit = theParameters.theDryWeatherLimitSleet;
		  theWeakPrecipitationLimit = theParameters.theWeakPrecipitationLimitSleet;
		}
		break;
	  case WATER_SNOW_FREEZING_FORM:
	  case WATER_SNOW_FORM:
	  case DRIZZLE_SNOW_FREEZING_FORM:
	  case DRIZZLE_SNOW_FORM:
	  case WATER_DRIZZLE_SNOW_FORM:
	  case WATER_SLEET_SNOW_FORM:
	  case DRIZZLE_SLEET_SNOW_FORM:
	  case SLEET_SNOW_FREEZING_FORM:
	  case SLEET_SNOW_FORM:
		{
		  theDryWeatherLimit = theParameters.theDryWeatherLimitSnow;
		  theWeakPrecipitationLimit = theParameters.theWeakPrecipitationLimitSnow;
		}
		break;
	  case MISSING_PRECIPITATION_FORM:
		break;
	  }
  }

  precipitation_form_id get_complete_precipitation_form(const string& theVariable,
														const float thePrecipitationFormWater,
														const float thePrecipitationFormDrizzle,
														const float thePrecipitationFormSleet,
														const float thePrecipitationFormSnow,
														const float thePrecipitationFormFreezing)
  {
	unsigned int precipitation_form = 0;

	typedef std::pair<float, precipitation_form_id> precipitation_form_type;
	precipitation_form_type water(thePrecipitationFormWater, 
								  (thePrecipitationFormWater != kFloatMissing ? 
								   WATER_FORM : MISSING_PRECIPITATION_FORM));
	precipitation_form_type drizzle(thePrecipitationFormDrizzle, 
									(thePrecipitationFormDrizzle != kFloatMissing ? 
									 DRIZZLE_FORM : MISSING_PRECIPITATION_FORM));
	precipitation_form_type sleet(thePrecipitationFormSleet, 
								  (thePrecipitationFormSleet != kFloatMissing ?
								   SLEET_FORM : MISSING_PRECIPITATION_FORM));
	precipitation_form_type snow(thePrecipitationFormSnow, 
								 (thePrecipitationFormSnow != kFloatMissing ?
								  SNOW_FORM : MISSING_PRECIPITATION_FORM));
	precipitation_form_type freezing(thePrecipitationFormFreezing, 
									 (thePrecipitationFormFreezing != kFloatMissing ?
									  FREEZING_FORM : MISSING_PRECIPITATION_FORM));

	vector<precipitation_form_type> precipitation_forms;
	precipitation_forms.push_back(water);
	precipitation_forms.push_back(drizzle);
	precipitation_forms.push_back(sleet);
	precipitation_forms.push_back(snow);
	precipitation_forms.push_back(freezing);

	sort(precipitation_forms.begin(),precipitation_forms.end());

	precipitation_form_id primaryPrecipitationForm = 
	  (precipitation_forms[4].first != kFloatMissing &&
	   precipitation_forms[4].first > PRECIPITATION_FORM_REPORTING_LIMIT) 
	  ? precipitation_forms[4].second : MISSING_PRECIPITATION_FORM;
	precipitation_form_id secondaryPrecipitationForm = 
	  (precipitation_forms[3].first != kFloatMissing &&
	   precipitation_forms[3].first > PRECIPITATION_FORM_REPORTING_LIMIT)
	  ? precipitation_forms[3].second : MISSING_PRECIPITATION_FORM;
	precipitation_form_id tertiaryPrecipitationForm = 
	  (precipitation_forms[2].first != kFloatMissing &&
	   precipitation_forms[2].first > PRECIPITATION_FORM_REPORTING_LIMIT)
	  ? precipitation_forms[2].second : MISSING_PRECIPITATION_FORM;

	// TODO: merge drizzle and water in some cases, when sleet is involved

	precipitation_form |= primaryPrecipitationForm;
	precipitation_form |= secondaryPrecipitationForm;
	precipitation_form |= tertiaryPrecipitationForm;

	return static_cast<precipitation_form_id>(precipitation_form);
  }

  void get_sub_time_series(const WeatherPeriod& thePeriod,
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

  float get_mean(const weather_result_data_item_vector& theTimeSeries, 
				 const int& theStartIndex /*= 0*/, 
				 const int& theEndIndex /*= 0*/)
  {
	float precipitation_sum = 0.0;
	int counter = 0;
	unsigned int startIndex = theStartIndex > 0 ? theStartIndex : 0;
	unsigned int endIndex = theEndIndex > 0 ? theEndIndex : theTimeSeries.size();
	
	if(startIndex == endIndex || endIndex < startIndex || endIndex > theTimeSeries.size())
	  return kFloatMissing;

	for(unsigned int i = startIndex; i < endIndex; i++)
	  {
		if(theTimeSeries[i]->theResult.value() == kFloatMissing)
		  continue;
		precipitation_sum += theTimeSeries[i]->theResult.value();
		counter++;
	  }

	if((counter == 0 && theTimeSeries.size() > 0) || theTimeSeries.size() == 0)
	  return kFloatMissing;
	else
	  return precipitation_sum / counter;
  }

  
  float get_standard_deviation(const weather_result_data_item_vector& theTimeSeries)
  {
	float deviation_sum_pow2 = 0.0;
	float mean = get_mean(theTimeSeries);
	int counter = 0;
	for(unsigned int i = 0; i < theTimeSeries.size(); i++)
	  {
		if(theTimeSeries[i]->theResult.value() == kFloatMissing)
		  continue;
		deviation_sum_pow2 += std::pow(mean - theTimeSeries[i]->theResult.value(), 2);
		counter++;
	  }

	return std::sqrt(deviation_sum_pow2 / counter);
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

  double get_pearson_coefficient(const weather_result_data_item_vector& theTimeSeries,
								 const unsigned int& theStartIndex,
								 const unsigned int& theEndIndex,
								 const bool& theUseErrorValueFlag /*= false*/)
  {
	vector<double> precipitation;

	for(unsigned int i = theStartIndex; i <= theEndIndex; i++)
	  {
		if(theUseErrorValueFlag)
		  precipitation.push_back(theTimeSeries[i]->theResult.error());
		else
		  precipitation.push_back(theTimeSeries[i]->theResult.value());
	  }

	return MathTools::pearson_coefficient(precipitation);
  }

  Sentence area_specific_sentence(const float& north,
								  const float& south,
								  const float& east,
								  const float& west,
								  const float& northEast,
								  const float& southEast,
								  const float& southWest,
								  const float& northWest,								  
								  const bool& mostlyFlag /*= true*/)
  {
	Sentence sentence;

	area_specific_sentence_id sentenceId =
	  get_area_specific_sentence_id(north,
									south,
									east,
									west,
									northEast,
									southEast,
									southWest,
									northWest,
									mostlyFlag);

	switch(sentenceId)
	  {
	  case ALUEEN_POHJOISOSISSA:
		{
		  sentence << ALUEEN_POHJOISOSISSA_PHRASE;
		}
		break;
	  case ALUEEN_ETELAOSISSA:
		{
		  sentence << ALUEEN_ETELAOSISSA_PHRASE;
		}
		break;
	  case ALUEEN_ITAOSISSA:
		{
		  sentence << ALUEEN_ITAOSISSA_PHRASE;
		}
		break;
	  case ALUEEN_LANSIOSISSA:
		{
		  sentence << ALUEEN_LANSIOSISSA_PHRASE;
		}
		break;
	  case ENIMMAKSEEN_ALUEEN_POHJOISOSISSA:
		{
		  sentence << ENIMMAKSEEN_WORD << ALUEEN_POHJOISOSISSA_PHRASE;
		}
		break;
	  case ENIMMAKSEEN_ALUEEN_ETELAOSISSA:
		{
		  sentence << ENIMMAKSEEN_WORD << ALUEEN_ETELAOSISSA_PHRASE;
		}
		break;
	  case ENIMMAKSEEN_ALUEEN_ITAOSISSA:
		{
		  sentence << ENIMMAKSEEN_WORD << ALUEEN_ITAOSISSA_PHRASE;
		}
		break;
	  case ENIMMAKSEEN_ALUEEN_LANSIOSISSA:
		{
		  sentence << ENIMMAKSEEN_WORD << ALUEEN_LANSIOSISSA_PHRASE;
		}
		break;
	  default:
		break;
	  }

	return sentence;
  }

  area_specific_sentence_id get_area_specific_sentence_id(const float& north,
														  const float& south,
														  const float& east,
														  const float& west,
														  const float& northEast,
														  const float& southEast,
														  const float& southWest,
														  const float& northWest,
														  const bool& mostlyFlag /*= true*/)
  {
	area_specific_sentence_id retval(MISSING_AREA_SPECIFIC_SENTENCE_ID);

	if(north >= 98.0)
	  {
		retval = ALUEEN_POHJOISOSISSA;
	  }
	else if(north >= 95.0 && mostlyFlag)
	  {
		retval = ENIMMAKSEEN_ALUEEN_POHJOISOSISSA;
	  }
	else if(south >= 98.0)
	  {
		retval = ALUEEN_ETELAOSISSA;
	  }
	else if(south >= 95.0 && mostlyFlag)
	  {
		retval = ENIMMAKSEEN_ALUEEN_ETELAOSISSA;
	  }
	else if(east >= 98.0)
	  {
		retval = ALUEEN_ITAOSISSA;
	  }
	else if(east >= 95.0 && mostlyFlag)
	  {
		retval = ENIMMAKSEEN_ALUEEN_ITAOSISSA;
	  }
	else if(west >= 98.0)
	  {
		retval = ALUEEN_LANSIOSISSA;
	  }
	else if(west >= 95.0 && mostlyFlag)
	  {
		retval = ENIMMAKSEEN_ALUEEN_LANSIOSISSA;
	  }
	return retval;
  }

  float get_area_percentage(const WeatherArea& theArea,
							const WeatherAnalysis::WeatherArea::Type& theType,
							const AnalysisSources& theSources,
							const WeatherPeriod& thePeriod)
  {
	WeatherArea maskArea = theArea;
	maskArea.type(theType);

	NFmiIndexMask indexMask;
	NFmiIndexMask fullIndexMask;
	RangeAcceptor precipitationlimits;
	precipitationlimits.lowerLimit(0);
	precipitationlimits.upperLimit(1000);
	ExtractMask(theSources,
				Precipitation,
				maskArea,
				thePeriod,
				precipitationlimits,
				indexMask);
	ExtractMask(theSources,
				Precipitation,
				theArea,
				thePeriod,
				precipitationlimits,
				fullIndexMask);

	if(fullIndexMask.size() == 0)
	  return 0.0;

	return (static_cast<float>(indexMask.size())/static_cast<float>(fullIndexMask.size()))*100.0;
  }


  Sentence time_phrase(const WeatherPeriod& thePeriod, 
					   const std::string& theDayPhasePhrase)
  {
	Sentence sentence;
	/*
	if(thePeriod.localStartTime().GetDay() != (thePeriod.localEndTime().GetDay()))
	  {
		sentence << theDayPhasePhrase;
	  }
	else
	  {
		sentence << thePeriod.localStartTime().GetWeekday() << "-" << theDayPhasePhrase;
	  }
	*/
	sentence << theDayPhasePhrase;

	return sentence;
  }


} // namespace TextGen

