// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::PeriodPhraseFactory
 */
// ======================================================================
/*!
 * \namespace TextGen::PeriodPhraseFactory
 *
 * \brief Tools for generating phrases to describe periods
 *
 * \see page_periodifraasit
 *
 */
// ======================================================================
#include "boost/lexical_cast.hpp"

#include "PeriodPhraseFactory.h"
#include "HourPeriodGenerator.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "TimeTools.h"
#include "WeatherPeriod.h"
#include "WeekdayTools.h"

#include "NFmiStringTools.h"
#include "NFmiTime.h"


using WeatherAnalysis::WeatherPeriod;
using namespace WeatherAnalysis::TimeTools;
using namespace boost;
using namespace std;

namespace
{
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

	  const string var = theVariable + "::until_tonight::phrases";
	  const string preferences = Settings::optional_string(var,"");
	  const string defaults("none,today,atday,weekday");
	  list<string> order = reorder_preferences(preferences,defaults);

	  for(list<string>::const_iterator it=order.begin(); it!=order.end(); ++it)
		{
		  if(*it == "none")
			{
			  if(isSameDay(theForecastTime, thePeriod.localStartTime()))
				return sentence;
			}
		  else if(*it == "atday")
			{
			  if(isSameDay(theForecastTime, thePeriod.localStartTime()))
				return (sentence << "päivällä");
			}
		  else if(*it == "today")
			{
			  if(isSameDay(theForecastTime, thePeriod.localStartTime()))
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

	  const string var = theVariable + "::until_morning::phrases";
	  const string preferences = Settings::optional_string(var,"");
	  const string defaults("none,tonight,atnight,weekday");
	  list<string> order = reorder_preferences(preferences,defaults);

	  for(list<string>::const_iterator it=order.begin(); it!=order.end(); ++it)
		{
		  if(*it == "none")
			{
			  if(isNextDay(theForecastTime, thePeriod.localEndTime()))
				return sentence;
			}
		  else if(*it == "atnight")
			{
			  if(isNextDay(theForecastTime, thePeriod.localEndTime()))
				return (sentence << "yöllä");
			}
		  else if(*it == "tonight")
			{
			  if(isNextDay(theForecastTime, thePeriod.localEndTime()))
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

	  const string var = theVariable + "::today::phrases";
	  const string preferences = Settings::optional_string(var,"");
	  const string defaults("none,today,tomorrow,atday,weekday");
	  list<string> order = reorder_preferences(preferences,defaults);

	  for(list<string>::const_iterator it=order.begin(); it!=order.end(); ++it)
		{
		  if(*it == "weekday")
			return (sentence << on_weekday(thePeriod.localStartTime()));
		  else if(*it == "atday")
			{
			  if(isSameDay(theForecastTime,thePeriod.localStartTime()))
				return (sentence << "päivällä");
			}
		  else if(*it == "none")
			{
			  if(isSameDay(theForecastTime,thePeriod.localStartTime()))
				return sentence;
			}
		  else if(*it == "today")
			{
			  if(isSameDay(theForecastTime,thePeriod.localStartTime()))
				return (sentence << "tänään");
			}
		  else if(*it == "tomorrow")
			{
			  if(isNextDay(theForecastTime,thePeriod.localStartTime()))
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

	  const string var = theVariable + "::tonight::phrases";
	  const string preferences = Settings::optional_string(var,"");
	  const string defaults("none,tonight,atnight,weekday");
	  list<string> order = reorder_preferences(preferences,defaults);

	  for(list<string>::const_iterator it=order.begin(); it!=order.end(); ++it)
		{
		  if(*it == "weekday")
			return (sentence << night_against_weekday(thePeriod.localEndTime()));
		  else if(*it == "atnight")
			{
			  if(isNextDay(theForecastTime,thePeriod.localEndTime()))
				return (sentence << "yöllä");
			}
		  else if(*it == "none")
			{
			  if(isNextDay(theForecastTime,thePeriod.localEndTime()))
				return sentence;
			}
		  else if(*it == "tonight")
			{
			  if(isNextDay(theForecastTime,thePeriod.localEndTime()))
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

	  const string var = theVariable + "::next_night::phrases";
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
			  if(isNextDay(theForecastTime,thePeriod.localEndTime()))
				return (sentence << "yöllä");
			}
		  else if(*it == "tonight")
			{
			  if(isNextDay(theForecastTime,thePeriod.localEndTime()))
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

	  const string var = theVariable + "::next_day::phrases";
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
			  if(isNextDay(theForecastTime,thePeriod.localStartTime()))
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

	  const string var = theVariable + "::next_days::phrases";
	  const string preferences = Settings::optional_string(var,"");
	  const string defaults("tomorrow,weekday");
	  list<string> order = reorder_preferences(preferences,defaults);

	  for(list<string>::const_iterator it=order.begin(); it!=order.end(); ++it)
		{
		  if(*it == "weekday")
			return (sentence << from_weekday(thePeriod.localStartTime()));
		  else if(*it == "tomorrow")
			{
			  // Sonera-sanakirjasta puuttuu "huomisesta alkaen"
			  // if(isNextDay(theForecastTime,thePeriod.localStartTime()))
			  // return (sentence << "huomisesta alkaen");
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
	  if(isSeveralDays(thePeriod.localStartTime(),thePeriod.localEndTime()))
		return next_days(theVariable,theForecastTime,thePeriod);
	  else
		return next_day(theVariable,theForecastTime,thePeriod);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return sentence describing period of type "days"
	 *
	 * \param theVariable The Settings variable containing extra details
	 * \param theForecastTime The forecast time
	 * \param thePeriod The time period itself
	 */
	// ----------------------------------------------------------------------
	
	Sentence days(const string & theVariable,
				  const NFmiTime & theForecastTime,
				  const WeatherPeriod & thePeriod)
	{
	  using WeekdayTools::from_weekday;
	  using WeekdayTools::on_weekday;
	  using WeatherAnalysis::HourPeriodGenerator;

	  Sentence sentence;

	  const string defaults("none,today,tomorrow,followingday,weekday");

	  const string var = theVariable + "::days::phrases";
	  string preferences = Settings::optional_string(var,"");
	  
	  HourPeriodGenerator hours(thePeriod,theVariable+"::day");
	  const int ndays = hours.size();

	  if(ndays==0)
		return sentence;
	  
	  const string nvar = var + "::days" + lexical_cast<string>(ndays);
	  preferences = Settings::optional_string(nvar,preferences);
	  list<string> order = reorder_preferences(preferences,defaults);

	  // the first day may not be the same as thePeriod.localStartTime
	  // due to starthour etc settings

	  const NFmiTime starttime = hours.period(1).localStartTime();

	  if(ndays==1)
		{
		  for(list<string>::const_iterator it=order.begin(); it!=order.end(); ++it)
			{
			  if(*it == "none")
				return sentence;
			  else if(*it == "today")
				{
				  if(isSameDay(theForecastTime, starttime))
					return (sentence << "tänään");
				}
			  else if(*it == "tomorrow")
				{
				  if(isNextDay(theForecastTime, starttime))
					return (sentence << "huomenna");
				}
			  else if(*it == "followingday")
				;
			  else if(*it == "weekday")
				{
				  return (sentence << on_weekday(starttime));
				}
			  else
				throw TextGenError("PeriodPhrasefactory::days does not accept phrase "+*it);
			}
		}
	  else if(ndays==2)
		{
		  NFmiTime nextday(starttime);
		  nextday.ChangeByDays(1);
		  for(list<string>::const_iterator it=order.begin(); it!=order.end(); ++it)
			{
			  if(*it == "none")
				return sentence;
			  else if(*it == "today")
				{
				  if(isSameDay(theForecastTime,starttime))
					{
					  sentence << "tänään" << "ja";
					  for(list<string>::const_iterator jt=order.begin();
						  jt!=order.end();
						  ++jt)
						{
						  if(*jt == "tomorrow")
							return (sentence << "huomenna");
						  if(*jt == "followingday")
							return (sentence << "seuraavana päivänä");
						  if(*jt == "weekday")
							return (sentence << on_weekday(starttime));
						}
					}
				}
			  else if(*it == "tomorrow")
				{
				  if(isNextDay(theForecastTime, starttime))
					{
					  sentence << "huomenna" << "ja";
					  for(list<string>::const_iterator jt=order.begin();
						  jt!=order.end();
						  ++jt)
						{
						  if(*jt == "followingday")
							return (sentence << "seuraavana päivänä");
						  if(*jt == "weekday")
							return (sentence << on_weekday(starttime));
						}
					}
				}
			  else if(*it == "followingday")
				;
			  else if(*it == "weekday")
				{
				  sentence << on_weekday(starttime)
						   << "ja"
						   << on_weekday(nextday);
				  return sentence;
				}
			  else
				throw TextGenError("PeriodPhrasefactory::days does not accept phrase "+*it);
			}
		}
	  else
		{
		  for(list<string>::const_iterator it=order.begin(); it!=order.end(); ++it)
			{
			  if(*it == "none")
				return sentence;
			  else if(*it == "today")
				;
			  else if(*it == "tomorrow")
				{
				  // Sonera-sanakirjasta puuttuu "huomisesta alkaen"
				  // if(isNextDay(theForecastTime, starttime))
				  // return (sentence << "huomisesta alkaen");
				}
			  else if(*it == "followingday")
				;
			  else if(*it == "weekday")
				{
				  return (sentence << from_weekday(starttime));
				}
			  else
				throw TextGenError("PeriodPhrasefactory::days does not accept phrase "+*it);
			}
		}

	  throw TextGenError("PeriodPhrasefactory::days run out of options");

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

	  if(theType == "days")
		return days(theVariable,theForecastTime,thePeriod);

	  throw TextGenError("PeriodPhraseFactory::create does not recognize type "+theType);
	}


  } // namespace PeriodPhraseFactory
} // namespace TextGen
