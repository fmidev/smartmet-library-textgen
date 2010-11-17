
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

 const char* trend_string(const trend_id& theTrendId)
  {
	const char* retval = "";

	switch(theTrendId)
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
		retval = "no trend";
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

  part_of_the_day_id get_part_of_the_day_id(const NFmiTime& theTimestamp)
  {
	
 	if(theTimestamp.GetHour() >= 6 && theTimestamp.GetHour() <= 9)
	  return AAMU;
	else if(theTimestamp.GetHour() >= 9 && theTimestamp.GetHour() <= 11)
	  return AAMUPAIVA;
	else if(theTimestamp.GetHour() >= 11 && theTimestamp.GetHour() <= 13)
	  return KESKIPAIVA;
	else if(theTimestamp.GetHour() >= 13 && theTimestamp.GetHour() <= 18)
	  return ILTAPAIVA;
	else if(theTimestamp.GetHour() >= 18 && theTimestamp.GetHour() <= 22)
	  return ILTA;
	else if(theTimestamp.GetHour() >= 22)
	  return ILTAYO;
	else if(theTimestamp.GetHour() <= 3)
	  return KESKIYO;
	else if(theTimestamp.GetHour() >= 3 && theTimestamp.GetHour() <= 6)
	  return AAMUYO;

	return MISSING_PART_OF_THE_DAY_ID;
 }

  part_of_the_day_id get_part_of_the_day_id(const WeatherPeriod& thePeriod)
  {
	if(thePeriod.localEndTime().DifferenceInHours(thePeriod.localStartTime()) > 9)
	  return MISSING_PART_OF_THE_DAY_ID;

	if(thePeriod.localStartTime().GetHour() >= 6 && thePeriod.localEndTime().GetHour() <= 9)
	  return AAMU;
	else if(thePeriod.localStartTime().GetHour() >= 9 && thePeriod.localEndTime().GetHour() <= 11)
	  return AAMUPAIVA;
	else if(thePeriod.localStartTime().GetHour() >= 11 && thePeriod.localEndTime().GetHour() <= 13)
	  return KESKIPAIVA;
	else if(thePeriod.localStartTime().GetHour() >= 13 && thePeriod.localEndTime().GetHour() <= 18)
	  return ILTAPAIVA;
	else if(thePeriod.localStartTime().GetHour() >= 18 && thePeriod.localEndTime().GetHour() <= 22)
	  return ILTA;
	else if(thePeriod.localStartTime().GetHour() >= 22)
	  return ILTAYO;
	else if(thePeriod.localStartTime().GetHour() <= 3 && thePeriod.localEndTime().GetHour() <= 3)
	  return KESKIYO;
	else if(thePeriod.localStartTime().GetHour() >= 3 && thePeriod.localEndTime().GetHour() <= 6)
	  return AAMUYO;
	else if(thePeriod.localStartTime().GetHour() >= 6 && thePeriod.localEndTime().GetHour() <= 12)
	  return AAMU_JA_AAMUPAIVA;
	else if(thePeriod.localStartTime().GetHour() >= 9 && thePeriod.localEndTime().GetHour() <= 13)
	  return AAMUPAIVA_JA_KESKIPAIVA;
	else if(thePeriod.localStartTime().GetHour() >= 11 && thePeriod.localEndTime().GetHour() <= 18)
	  return KESKIPAIVA_JA_ILTAPAIVA;
	else if(thePeriod.localStartTime().GetHour() >= 13 && thePeriod.localEndTime().GetHour() <= 21)
	  return ILTAPAIVA_JA_ILTA;
	else if(thePeriod.localStartTime().GetHour() >= 18)
	  return ILTA_JA_ILTAYO;
	else if(thePeriod.localStartTime().GetHour() >= 22 && thePeriod.localEndTime().GetHour() <= 3)
	  return ILTAYO_JA_KESKIYO;
	else if(thePeriod.localStartTime().GetHour() <= 6 && thePeriod.localEndTime().GetHour() <= 6)
	  return KESKIYO_JA_AAMUYO;
	else if(thePeriod.localStartTime().GetHour() >= 3 && thePeriod.localEndTime().GetHour() <= 9)
	  return AAMUYO_JA_AAMU;
	
	return MISSING_PART_OF_THE_DAY_ID;
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

  void get_part_of_the_day(const part_of_the_day_id& thePartOfTheDayId, int& theStartHour, int& theEndHour)
  {
	switch(thePartOfTheDayId)
	  {
	  case AAMU:
		{
		  theStartHour = 6;
		  theEndHour = 9;
		}
		break;
	  case AAMUPAIVA:
		{
		  theStartHour = 9;
		  theEndHour = 11;
		}
		break;
	  case KESKIPAIVA:
		{
		  theStartHour = 11;
		  theEndHour = 13;
		}
		break;
	  case ILTAPAIVA:
		{
		  theStartHour = 13;
		  theEndHour = 18;
		}
		break;
	  case ILTA:
		{
		  theStartHour = 18;
		  theEndHour = 22;
		}
		break;
	  case ILTAYO:
		{
		  theStartHour = 22;
		  theEndHour = 0;
		}
		break;
	  case KESKIYO:
		{
		  theStartHour = 0;
		  theEndHour = 3;
		}
		break;
	  case AAMUYO:
		{
		  theStartHour = 3;
		  theEndHour = 6;
		}
		break;
	  case PAIVA:
		{
		  theStartHour = 9;
		  theEndHour = 18;
		}
		break;
	  case YO:
		{
		  theStartHour = 0;
		  theEndHour = 6;
		}
		break;
	  case YOPUOLI:
		{
		  theStartHour = 18;
		  theEndHour = 6;
		}
		break;
	  case PAIVAPUOLI:
		{
		  theStartHour = 6;
		  theEndHour = 18;
		}
		break;
	  case AAMU_JA_AAMUPAIVA:
		{
		  theStartHour = 6;
		  theEndHour = 12;
		}
		break;
	  case AAMUPAIVA_JA_KESKIPAIVA:
		{
		  theStartHour = 9;
		  theEndHour = 13;
		}
		break;
	  case KESKIPAIVA_JA_ILTAPAIVA:
		{
		  theStartHour = 11;
		  theEndHour = 18;
		}
		break;
	  case ILTAPAIVA_JA_ILTA:
		{
		  theStartHour = 13;
		  theEndHour = 21;
		}
		break;
	  case ILTA_JA_ILTAYO:
		{
		  theStartHour = 18;
		  theEndHour = 0;
		}
		break;
	  case ILTAYO_JA_KESKIYO:
		{
		  theStartHour = 22;
		  theEndHour = 3;
		}
		break;
	  case KESKIYO_JA_AAMUYO:
		{
		  theStartHour = 0;
		  theEndHour = 6;
		}
		break;
	  case AAMUYO_JA_AAMU:
		{
		  theStartHour = 3;
		  theEndHour = 9;
		}
		break;
	  default:
		{
		  theStartHour = -1;
		  theEndHour = -1;
		}
	  }
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
	get_part_of_the_day(thePartOfTheDayId, startHour, endHour);
	NFmiTime startTimeCompare(theTimeStamp);
	NFmiTime endTimeCompare(theTimeStamp);
	startTimeCompare.SetHour(startHour);
	endTimeCompare.SetHour(endHour);

	if(endHour == 0)
	  return theTimeStamp >= startTimeCompare;
	else if(startHour == 0)
	  return theTimeStamp <= endTimeCompare;
	else
	  return(theTimeStamp >= startTimeCompare && 
			 theTimeStamp <= endTimeCompare);
  }

  /*
  WeatherPeriod get_part_of_the_day_period(const NFmiTime& theReferencTime,
										   const part_of_the_day_id& thePartOfTheDayId)
  {
	int startHour, endHour;
	get_part_of_the_day(thePartOfTheDayId, startHour, endHour);
	
	NFmiTime startTime(theReferencTime);
	NFmiTime endTime(theReferencTime);

	startTime.SetHour(startHour);
	endTime.SetHour(endHour);

	if(endHour == 0)
	  endTime.ChangeByDays(1);
  }
*/
  bool is_inside(const WeatherPeriod& theWeatherPeriod,
				 const part_of_the_day_id& thePartOfTheDayId)
  {
	int startHour, endHour;
	get_part_of_the_day(thePartOfTheDayId, startHour, endHour);
	NFmiTime startTimeCompare(theWeatherPeriod.localStartTime());
	NFmiTime endTimeCompare(theWeatherPeriod.localStartTime());
	startTimeCompare.SetHour(startHour);
	endTimeCompare.SetHour(endHour);
	startTimeCompare.SetMin(0);
	endTimeCompare.SetMin(0);
	startTimeCompare.SetSec(0);
	endTimeCompare.SetSec(0);
	if(endHour == 0)
	  endTimeCompare.ChangeByDays(1);

	return (theWeatherPeriod.localStartTime() >= startTimeCompare &&
			theWeatherPeriod.localStartTime() <= endTimeCompare &&
			theWeatherPeriod.localEndTime() >= startTimeCompare &&
			theWeatherPeriod.localEndTime() <= endTimeCompare);

	/*
	if(endHour == 0)
	  return (theWeatherPeriod.localStartTime() >= startTimeCompare && 
			  theWeatherPeriod.localEndTime() >= startTimeCompare);
	else if(startHour == 0)
	  return (theWeatherPeriod.localStartTime() <= endTimeCompare && 
			  theWeatherPeriod.localEndTime() <= endTimeCompare);
	else
	  return(theWeatherPeriod.localStartTime() >= startTimeCompare && 
			 theWeatherPeriod.localEndTime() <= endTimeCompare);
	*/
  }

  Sentence get_time_phrase_large(const WeatherPeriod& theWeatherPeriod)
  {
	Sentence sentence;

	if(get_part_of_the_day_id(theWeatherPeriod.localStartTime()) == 
	   get_part_of_the_day_id(theWeatherPeriod.localEndTime()))
	  {
		sentence << get_time_phrase(theWeatherPeriod.localStartTime(), false);
	  }
	else
	  {
		// aamulla ja aamupäivällä
		if(is_inside(theWeatherPeriod, AAMU_JA_AAMUPAIVA))
		  sentence << AAMULLA_WORD << JA_WORD << AAMUPAIVALLA_WORD;
		else if(is_inside(theWeatherPeriod, AAMUPAIVA_JA_KESKIPAIVA))
		  sentence << AAMUPAIVALLA_WORD << JA_WORD << KESKIPAIVALLA_WORD;
		else if(is_inside(theWeatherPeriod, KESKIPAIVA_JA_ILTAPAIVA))
		  sentence << KESKIPAIVALLA_WORD << JA_WORD << ILTAPAIVALLA_WORD;
		else if(is_inside(theWeatherPeriod, ILTAPAIVA_JA_ILTA))
		  sentence << ILTAPAIVALLA_WORD << JA_WORD << ILLALLA_WORD;
		else if(is_inside(theWeatherPeriod, ILTA_JA_ILTAYO))
		  sentence << ILLALLA_WORD << JA_WORD << ILTAYOSTA_WORD;
		else if(is_inside(theWeatherPeriod, ILTAYO_JA_KESKIYO))
		  sentence << ILTAYOSTA_WORD << JA_WORD << KESKIYOLLA_WORD;
		else if(is_inside(theWeatherPeriod, KESKIYO_JA_AAMUYO))
		  sentence << KESKIYOLLA_WORD << JA_WORD << AAMUYOLLA_WORD;
		else if(is_inside(theWeatherPeriod, AAMUYO_JA_AAMU))
		  sentence << AAMUYOLLA_WORD << JA_WORD << AAMULLA_WORD;
	  }

	return sentence;
  }

  Sentence get_time_phrase(const NFmiTime& theTimestamp, bool theAlkaenPhrase /*= false*/)
  {
	Sentence sentence;

	if(is_inside(theTimestamp, AAMU))
		sentence << (theAlkaenPhrase ? AAMUSTA_ALKAEN_PHRASE : AAMULLA_WORD);
	else if(is_inside(theTimestamp, AAMUPAIVA))
	  sentence << (theAlkaenPhrase ? AAMUPAIVASTA_ALKAEN_PHRASE : AAMUPAIVALLA_WORD);
	else if(is_inside(theTimestamp, KESKIPAIVA))
	  sentence << (theAlkaenPhrase ? KESKIPAIVASTA_ALKAEN_PHRASE : KESKIPAIVALLA_WORD);
	else if(is_inside(theTimestamp, ILTAPAIVA))
	  sentence << (theAlkaenPhrase ? ILTAPAIVASTA_ALKAEN_PHRASE : ILTAPAIVALLA_WORD);
	else if(is_inside(theTimestamp, ILTA))
	  sentence << (theAlkaenPhrase ? ILLASTA_ALKAEN_PHRASE : ILLALLA_WORD);
	else if(is_inside(theTimestamp, ILTAYO))
	  sentence << (theAlkaenPhrase ? ILTAYOSTA_ALKAEN_PHRASE : ILTAYOSTA_WORD);
	else if(is_inside(theTimestamp, KESKIYO))
	  sentence << (theAlkaenPhrase ? KESKIYOSTA_ALKAEN_PHRASE : KESKIYOLLA_WORD);
	else if(is_inside(theTimestamp, AAMUYO))
		sentence << (theAlkaenPhrase ? AAMUYOSTA_ALKAEN_PHRASE : AAMUYOLLA_WORD);

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

  unsigned int get_complete_precipitation_form(const string& theVariable,
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

	return precipitation_form;
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

}
