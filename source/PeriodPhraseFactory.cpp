// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::PeriodPhraseFactory
 */
// ======================================================================

#include "PeriodPhraseFactory.h"
#include "NFmiTime.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "WeatherPeriod.h"
#include "WeekdayTools.h"

using WeatherAnalysis::WeatherPeriod;
using namespace std;

namespace
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Test if the dates are the same
   *
   * \param theDate1 The first date
   * \param theDate2 The second date
   * \return True if the dates are the same
   */
  // ----------------------------------------------------------------------

  bool is_same_day(const NFmiTime & theDate1,
				   const NFmiTime & theDate2)
  {
	return(theDate1.GetDay() == theDate2.GetDay() &&
		   theDate1.GetMonth() == theDate2.GetMonth() &&
		   theDate1.GetYear() == theDate2.GetYear());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test if the second day is the day after the first
   *
   * \param theDate1 The first date
   * \param theDate2 The second date
   * \return True if date1+1==date2
   */
  // ----------------------------------------------------------------------

  bool is_next_day(const NFmiTime & theDate1,
				   const NFmiTime & theDate2)
  {
	NFmiTime date1(theDate1);
	date1.ChangeByDays(1);

	return(date1.GetDay() == theDate2.GetDay() &&
		   date1.GetMonth() == theDate2.GetMonth() &&
		   date1.GetYear() == theDate2.GetYear());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test if the period covers several days
   *
   * Note that a period that ends at 00:00 the next day is not considered
   * to span several days, the end hour must be atleast 01:00 if the period
   * ends the day after the start date.
   *
   * \param theDate1 The first date
   * \param theDate2 The second date
   * \return True if the period covers several days
   */
  // ----------------------------------------------------------------------

  bool is_several_days(const NFmiTime & theDate1,
					   const NFmiTime & theDate2)
  {
	if(is_same_day(theDate1,theDate2))
	  return false;
	if(is_next_day(theDate1,theDate2) && theDate2.GetHour()==0)
	  return false;
	return true;
  }
  
}

namespace TextGen
{
  
  namespace
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return sentence describing period of type "until_tonight"
	 *
	 * \param theVariable The Settings variable containing extra details
	 * \param theForecastTime The forecast time
	 * \param thePeriod The time period itself
	 */
	// ----------------------------------------------------------------------
	
	Sentence until_tonight(const string & theVariable,
						   const NFmiTime & theForecastTime,
						   const WeatherPeriod & thePeriod)
	{
	  using Settings::optional_bool;
	  using WeekdayTools::on_weekday;
	  Sentence sentence;

	  if(!is_same_day(theForecastTime, thePeriod.localStartTime()))
		sentence << on_weekday(thePeriod.localStartTime());
	  else if(optional_bool(theVariable+"::prefer_phrase_weekday",false))
		sentence << on_weekday(thePeriod.localStartTime());
	  else if(optional_bool(theVariable+"::prefer_phrase_today",false))
		sentence << "tänään";
	  else if(optional_bool(theVariable+"::prefer_phrase_atday",false))
		sentence << "päivällä";
	  else
		;

	  return sentence;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return sentence describing period of type "until_morning"
	 *
	 * \param theVariable The Settings variable containing extra details
	 * \param theForecastTime The forecast time
	 * \param thePeriod The time period itself
	 */
	// ----------------------------------------------------------------------
	
	Sentence until_morning(const string & theVariable,
						   const NFmiTime & theForecastTime,
						   const WeatherPeriod & thePeriod)
	{
	  using Settings::optional_bool;
	  using WeekdayTools::night_against_weekday;
	  Sentence sentence;
	  
	  if(!is_next_day(theForecastTime, thePeriod.localEndTime()))
		sentence << night_against_weekday(thePeriod.localEndTime());
	  else if(optional_bool(theVariable+"::prefer_phrase_weekday",false))
		sentence << night_against_weekday(thePeriod.localEndTime());
	  else if(optional_bool(theVariable+"::prefer_phrase_tonight",false))
		sentence << "ensi yönä";
	  else if(optional_bool(theVariable+"::prefer_phrase_atnight",false))
		sentence << "yöllä";
	  else
		;
	  
	  return sentence;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return sentence describing period of type "next_night"
	 *
	 * \param theVariable The Settings variable containing extra details
	 * \param theForecastTime The forecast time
	 * \param thePeriod The time period itself
	 */
	// ----------------------------------------------------------------------
	
	Sentence next_night(const string & theVariable,
						const NFmiTime & theForecastTime,
						const WeatherPeriod & thePeriod)
	{
	  using Settings::optional_bool;
	  using WeekdayTools::night_against_weekday;
	  Sentence sentence;

	  if(optional_bool(theVariable+"::prefer_phrase_weekday",false))
		sentence << night_against_weekday(thePeriod.localEndTime());
	  else if(optional_bool(theVariable+"::prefer_phrase_followingnight",false))
		sentence << "seuraavana yönä";
	  else if(optional_bool(theVariable+"::prefer_phrase_tonight",false))
		sentence << "ensi yönä";
	  else if(optional_bool(theVariable+"::prefer_phrase_atnight",false))
		sentence << "yöllä";
	  else
		sentence << "yöllä";

	  return sentence;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return sentence describing period of type "next_day"
	 *
	 * \param theVariable The Settings variable containing extra details
	 * \param theForecastTime The forecast time
	 * \param thePeriod The time period itself
	 */
	// ----------------------------------------------------------------------
	
	Sentence next_day(const string & theVariable,
					  const NFmiTime & theForecastTime,
					  const WeatherPeriod & thePeriod)
	{
	  using Settings::optional_bool;
	  using WeekdayTools::on_weekday;
	  Sentence sentence;

	  if(!is_next_day(theForecastTime, thePeriod.localStartTime()))
		sentence << on_weekday(thePeriod.localStartTime());
	  else if(optional_bool(theVariable+"::prefer_phrase_weekday",false))
		sentence << on_weekday(thePeriod.localStartTime());
	  else if(optional_bool(theVariable+"::prefer_phrase_followingday",false))
		sentence << "seuraavana päivänä";
	  else if(optional_bool(theVariable+"::prefer_phrase_tomorrow",false))
		sentence << "huomenna";
	  else
		sentence << "huomenna";
	  
	  return sentence;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return sentence describing period of type "next_days"
	 *
	 * \param theVariable The Settings variable containing extra details
	 * \param theForecastTime The forecast time
	 * \param thePeriod The time period itself
	 */
	// ----------------------------------------------------------------------
	
	Sentence next_days(const string & theVariable,
					   const NFmiTime & theForecastTime,
					   const WeatherPeriod & thePeriod)
	{
	  using Settings::optional_bool;
	  using WeekdayTools::from_weekday;
	  Sentence sentence;

	  if(!is_next_day(theForecastTime, thePeriod.localStartTime()))
		sentence << from_weekday(thePeriod.localStartTime());
	  else if(optional_bool(theVariable+"::prefer_phrase_weekday",false))
		sentence << from_weekday(thePeriod.localStartTime());
	  else if(optional_bool(theVariable+"::prefer_phrase_tomorrow",false))
		sentence << "huomisesta alkaen";
	  else
		sentence << "huomisesta alkaen";

	  return sentence;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return sentence describing period of type "remaining_days"
	 *
	 * \param theVariable The Settings variable containing extra details
	 * \param theForecastTime The forecast time
	 * \param thePeriod The time period itself
	 */
	// ----------------------------------------------------------------------
	
	Sentence remaining_days(const string & theVariable,
							const NFmiTime & theForecastTime,
							const WeatherPeriod & thePeriod)
	{
	  if(is_several_days(thePeriod.localStartTime(),thePeriod.localEndTime()))
		return next_days(theVariable,theForecastTime,thePeriod);
	  else
		return next_day(theVariable,theForecastTime,thePeriod);
	}

  } // namespace anonymous
  

  namespace PeriodPhraseFactory
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return sentence describing the period
	 *
	 * \param theType The name of the period type
	 * \param theVariable The Settings variable containing extra details
	 * \param theForecastTime The forecast time
	 * \param thePeriod The time period itself
	 */
	// ----------------------------------------------------------------------

	Sentence create(const string & theType,
					const string & theVariable,
					const NFmiTime & theForecastTime,
					const WeatherPeriod & thePeriod)
	{
	  if(theType == "until_tonight")
		return until_tonight(theVariable,theForecastTime,thePeriod);

	  if(theType == "until_morning")
		return until_morning(theVariable,theForecastTime,thePeriod);

	  if(theType == "next_night")
		return next_night(theVariable,theForecastTime,thePeriod);

	  if(theType == "next_day")
		return next_day(theVariable,theForecastTime,thePeriod);

	  if(theType == "next_days")
		return next_days(theVariable,theForecastTime,thePeriod);

	  if(theType == "remaining_days")
		return remaining_days(theVariable,theForecastTime,thePeriod);

	  throw TextGenError("PeriodPhraseFactory::create does not recognize type "+theType);
	}


  } // namespace PeriodPhraseFactory
} // namespace TextGen
