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

#include "NFmiStringTools.h"

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

  // ----------------------------------------------------------------------
  /*!
   * \brief Reorganizes preferred phrases
   *
   * The input strings contain preferred word orders such as
   * \code
   * today,tomorrow,weekday
   * \endcode
   * This function makes sure the output contains exactly those
   * words given in the second input parameter. However, the
   * first parameter may specify a different preferred ordering
   * for the variables.
   *
   * For example, given two parameters
   * \code
   * foo,atnight,bar,weekday
   * tonight,atnight,followingnight,weekday
   * \endcode
   * the output would be
   * \code
   * atnight,weekday,tonight,followingnight
   * \endcode
   *
   * \param thePreference The preferred order
   * \param theDefault The default order
   * \return List of words
   */
  // ----------------------------------------------------------------------

  list<string> reorder_preferences(const string & thePreference,
								   const string & theDefault)
  {
	const list<string> preferences = NFmiStringTools::SplitWords(thePreference);
	const list<string> defaults = NFmiStringTools::SplitWords(theDefault);

	// fast special case
	if(preferences.empty()) return defaults;

	typedef list<string>::const_iterator const_iterator;

	list<string> output;

	// first the allowed ones from preferences in correct order
	for(const_iterator it=preferences.begin(); it!=preferences.end(); ++it)
	  {
		if(find(defaults.begin(),defaults.end(),*it) != defaults.end())
		  if(find(output.begin(),output.end(),*it) == output.end())
			output.push_back(*it);
	  }
	// then the remaining defaults
	for(const_iterator jt=defaults.begin(); jt!=defaults.end(); ++jt)
	  {
		if(find(output.begin(),output.end(),*jt) == output.end())
			output.push_back(*jt);
	  }
	return output;
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
	  using WeekdayTools::on_weekday;
	  Sentence sentence;

	  const string var = theVariable + "::until_tonight::timephrases";
	  const string preferences = Settings::optional_string(var,"");
	  const string defaults("none,today,atday,weekday");
	  list<string> order = reorder_preferences(preferences,defaults);

	  for(list<string>::const_iterator it=order.begin(); it!=order.end(); ++it)
		{
		  if(*it == "none")
			{
			  if(is_same_day(theForecastTime, thePeriod.localStartTime()))
				return sentence;
			}
		  else if(*it == "atday")
			{
			  if(is_same_day(theForecastTime, thePeriod.localStartTime()))
				return (sentence << "päivällä");
			}
		  else if(*it == "today")
			{
			  if(is_same_day(theForecastTime, thePeriod.localStartTime()))
				return(sentence << "tänään");
			}
		  else if(*it == "weekday")
			return (sentence << on_weekday(thePeriod.localStartTime()));
		  else
			throw TextGenError("PeriodPhraseFactory::until_tonight does not accept phrase "+*it);
		}

	  throw TextGenError("PeriodPhrasefactory::until_tonight run out of options");
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
	  using WeekdayTools::night_against_weekday;
	  Sentence sentence;

	  const string var = theVariable + "::until_morning::timephrases";
	  const string preferences = Settings::optional_string(var,"");
	  const string defaults("none,tonight,atnight,weekday");
	  list<string> order = reorder_preferences(preferences,defaults);

	  for(list<string>::const_iterator it=order.begin(); it!=order.end(); ++it)
		{
		  if(*it == "none")
			{
			  if(is_next_day(theForecastTime, thePeriod.localEndTime()))
				return sentence;
			}
		  else if(*it == "atnight")
			{
			  if(is_next_day(theForecastTime, thePeriod.localEndTime()))
				return (sentence << "yöllä");
			}
		  else if(*it == "tonight")
			{
			  if(is_next_day(theForecastTime, thePeriod.localEndTime()))
				return(sentence << "ensi yönä");
			}
		  else if(*it == "weekday")
			return (sentence << night_against_weekday(thePeriod.localEndTime()));
		  else
			throw TextGenError("PeriodPhrasefactory::until_morning does not accept phrase "+*it);
		}

	  throw TextGenError("PeriodPhrasefactory::until_morning run out of options");
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return sentence describing period of type "today"
	 *
	 * \param theVariable The Settings variable containing extra details
	 * \param theForecastTime The forecast time
	 * \param thePeriod The time period itself
	 */
	// ----------------------------------------------------------------------
	
	Sentence today(const string & theVariable,
				   const NFmiTime & theForecastTime,
				   const WeatherPeriod & thePeriod)
	{
	  using WeekdayTools::on_weekday;
	  Sentence sentence;

	  const string var = theVariable + "::today::timephrases";
	  const string preferences = Settings::optional_string(var,"");
	  const string defaults("none,today,tomorrow,atday,weekday");
	  list<string> order = reorder_preferences(preferences,defaults);

	  for(list<string>::const_iterator it=order.begin(); it!=order.end(); ++it)
		{
		  if(*it == "weekday")
			return (sentence << on_weekday(thePeriod.localStartTime()));
		  else if(*it == "atday")
			{
			  if(is_same_day(theForecastTime,thePeriod.localStartTime()))
				return (sentence << "päivällä");
			}
		  else if(*it == "none")
			{
			  if(is_same_day(theForecastTime,thePeriod.localStartTime()))
				return sentence;
			}
		  else if(*it == "today")
			{
			  if(is_same_day(theForecastTime,thePeriod.localStartTime()))
				return (sentence << "tänään");
			}
		  else if(*it == "tomorrow")
			{
			  if(is_next_day(theForecastTime,thePeriod.localStartTime()))
				return (sentence << "huomenna");
			}
		  else
			throw TextGenError("PeriodPhrasefactory::today does not accept phrase "+*it);
		}

	  throw TextGenError("PeriodPhrasefactory::today run out of options");
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return sentence describing period of type "tonight"
	 *
	 * \param theVariable The Settings variable containing extra details
	 * \param theForecastTime The forecast time
	 * \param thePeriod The time period itself
	 */
	// ----------------------------------------------------------------------
	
	Sentence tonight(const string & theVariable,
					 const NFmiTime & theForecastTime,
					 const WeatherPeriod & thePeriod)
	{
	  using WeekdayTools::night_against_weekday;
	  Sentence sentence;

	  const string var = theVariable + "::tonight::timephrases";
	  const string preferences = Settings::optional_string(var,"");
	  const string defaults("none,tonight,atnight,weekday");
	  list<string> order = reorder_preferences(preferences,defaults);

	  for(list<string>::const_iterator it=order.begin(); it!=order.end(); ++it)
		{
		  if(*it == "weekday")
			return (sentence << night_against_weekday(thePeriod.localEndTime()));
		  else if(*it == "atnight")
			{
			  if(is_next_day(theForecastTime,thePeriod.localEndTime()))
				return (sentence << "yöllä");
			}
		  else if(*it == "none")
			{
			  if(is_next_day(theForecastTime,thePeriod.localEndTime()))
				return sentence;
			}
		  else if(*it == "tonight")
			{
			  if(is_next_day(theForecastTime,thePeriod.localEndTime()))
				return (sentence << "ensi yönä");
			}
		  else
			throw TextGenError("PeriodPhrasefactory::tonight does not accept phrase "+*it);
		}

	  throw TextGenError("PeriodPhrasefactory::tonight run out of options");
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
	  using WeekdayTools::night_against_weekday;
	  Sentence sentence;

	  const string var = theVariable + "::next_night::timephrases";
	  const string preferences = Settings::optional_string(var,"");
	  const string defaults("tonight,atnight,followingnight,weekday");
	  list<string> order = reorder_preferences(preferences,defaults);

	  for(list<string>::const_iterator it=order.begin(); it!=order.end(); ++it)
		{
		  if(*it == "weekday")
			return (sentence << night_against_weekday(thePeriod.localEndTime()));
		  else if(*it == "followingnight")
			return (sentence << "seuraavana yönä");
		  else if(*it == "atnight")
			{
			  if(is_next_day(theForecastTime,thePeriod.localEndTime()))
				return (sentence << "yöllä");
			}
		  else if(*it == "tonight")
			{
			  if(is_next_day(theForecastTime,thePeriod.localEndTime()))
				return (sentence << "ensi yönä");
			}
		  else
			throw TextGenError("PeriodPhrasefactory::next_night does not accept phrase "+*it);
		}

	  throw TextGenError("PeriodPhrasefactory::next_night run out of options");
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
	  using WeekdayTools::on_weekday;
	  Sentence sentence;

	  const string var = theVariable + "::next_day::timephrases";
	  const string preferences = Settings::optional_string(var,"");
	  const string defaults("tomorrow,followingday,weekday");
	  list<string> order = reorder_preferences(preferences,defaults);

	  for(list<string>::const_iterator it=order.begin(); it!=order.end(); ++it)
		{
		  if(*it == "weekday")
			return (sentence << on_weekday(thePeriod.localStartTime()));
		  else if(*it == "followingday")
			return (sentence << "seuraavana päivänä");
		  else if(*it == "tomorrow")
			{
			  if(is_next_day(theForecastTime,thePeriod.localStartTime()))
				return (sentence << "huomenna");
			}
		  else
			throw TextGenError("PeriodPhrasefactory::next_day does not accept phrase "+*it);
		}

	  throw TextGenError("PeriodPhrasefactory::next_day run out of options");
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
	  using WeekdayTools::from_weekday;
	  Sentence sentence;

	  const string var = theVariable + "::next_days::timephrases";
	  const string preferences = Settings::optional_string(var,"");
	  const string defaults("tomorrow,weekday");
	  list<string> order = reorder_preferences(preferences,defaults);

	  for(list<string>::const_iterator it=order.begin(); it!=order.end(); ++it)
		{
		  if(*it == "weekday")
			return (sentence << from_weekday(thePeriod.localStartTime()));
		  else if(*it == "tomorrow")
			{
			  if(is_next_day(theForecastTime,thePeriod.localStartTime()))
				return (sentence << "huomisesta alkaen");
			}
		  else
			throw TextGenError("PeriodPhrasefactory::next_days does not accept phrase "+*it);
		}

	  throw TextGenError("PeriodPhrasefactory::next_days run out of options");
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

	  if(theType == "today")
		return today(theVariable,theForecastTime,thePeriod);

	  if(theType == "tonight")
		return tonight(theVariable,theForecastTime,thePeriod);

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
