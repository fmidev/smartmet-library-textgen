// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::WeatherPeriodFactory
 */
// ======================================================================

#include "WeatherPeriodFactory.h"
#include "WeatherPeriod.h"
#include "TextGenError.h"

#include "NFmiSettings.h"
#include "NFmiTime.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace std;
using namespace boost;

// ======================================================================
//				IMPLEMENTATION HIDING FUNCTIONS
// ======================================================================

namespace
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Round up the given time to full hours
   *
   * \param theTime The time to round up
   * \return The time rounded up
   */
  // ----------------------------------------------------------------------

  NFmiTime round_up(const NFmiTime & theTime)
  {
	NFmiTime ret(theTime);
	if(ret.GetMin()>0 || ret.GetSec()>0)
	  ret.ChangeByHours(1);
	ret.SetMin(0);
	ret.SetSec(0);
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Get the given setting as an integer
   *
   * Throws if the variable is not set or there is a parsing error.
   *
   * \param theVariable The variable name
   * \return The integer
   */
  // ----------------------------------------------------------------------

  int require_int(const string & theVariable)
  {
	const string str = NFmiSettings::instance().require(theVariable);
	const int value = lexical_cast<int>(str);
	return value;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Get the given hour setting
   *
   * Throws if the value is not in range 0-23
   *
   * \param theVariable The variable name
   * \return The integer
   */
  // ----------------------------------------------------------------------

  int require_hour(const string & theVariable)
  {
	using namespace TextGen;

	const int value = require_int(theVariable);
	if(value>=0 && value<24)
	  return value;

	throw TextGenError("The value of variable "+theVariable+" is not in range 0-23");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Get the given days setting
   *
   * Throws if the value is not >= 0
   *
   * \param theVariable The variable name
   * \return The integer
   */
  // ----------------------------------------------------------------------

  int require_days(const string & theVariable)
  {
	using namespace TextGen;

	const int value = require_int(theVariable);
	if(value>=0)
	  return value;

	throw TextGenError("The value of variable "+theVariable+" is not >= 0");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Create period from now until some given time
   *
   * Required variables are
   *
   *  - \c variable::days
   *  - \c variable::endhour
   *  - \c variable::switchhour
   *
   * The start hour is formed by rounding up the given time to the
   * next full hour. The end hour is formed by adding \c days to
   * the start hour. If the hour is after \c switchhour, an extra
   * day is added. The end hour is then set from \c endhour.
   *
   * For example, if days=1, switchhour=12, endhour=18 we get
   *
   * - On Monday 11:00 the period Monday 11:00 - Tuesday 18:00
   * - On Monday 13:00 the period Monday 13:00 - Wednesday 18:00
   *
   * \param theTime The local reference time
   * \return The period
   */
  // ----------------------------------------------------------------------

  WeatherPeriod period_until(const NFmiTime & theTime,
							 const string & theVariable)
  {
	const int days       = require_days(theVariable+"::days");
	const int endhour    = require_hour(theVariable+"::endhour");
	const int switchhour = require_hour(theVariable+"::switchhour");

	NFmiTime start(round_up(theTime));

	NFmiTime end(start);
	end.ChangeByDays(days);
	end.SetHour(endhour);
	if(start.GetHour() >= switchhour)
	  end.ChangeByDays(1);

	WeatherPeriod period(start,end);
	return period;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Create period from given start time to given end time
   *
   * Required variables are
   *
   *  - \c variable::startday
   *  - \c variable::starthour
   *  - \c variable::switchhour
   *  - \c variable::days
   *  - \c variable::endhour
   *
   * The start time is formed by rounding up the given time to the
   * next full hour. Then \c startday is added to the days. If the
   * hour is atleast \c switchhour, another day is added. Finally
   * the hour is set to \c starthour.
   *
   * The end time is formed by adding \c days to the start time,
   * and then setting the hour to \c endhour.
   *
   * For example, if startday=1, starthour=18, switchhour=12,
   *                 days=2, endhour=6 we get
   *
   * - On Monday 11:00 the period Tueday 18:00 - Thursday 06:00
   * - On Monday 13:00 the period Wednesday 18:00 - Friday 06:00
   *
   * \param theTime The local reference time
   * \return The period
   */
  // ----------------------------------------------------------------------

  WeatherPeriod period_from_until(const NFmiTime & theTime,
								  const string & theVariable)
  {
	const int startday   = require_days(theVariable+"::startday");
	const int starthour  = require_hour(theVariable+"::starthour");
	const int switchhour = require_hour(theVariable+"::switchhour");
	const int days       = require_days(theVariable+"::days");
	const int endhour    = require_hour(theVariable+"::endhour");

	NFmiTime start(round_up(theTime));
	start.ChangeByDays(startday);
	if(start.GetHour() >= switchhour)
	  start.ChangeByDays(1);
	start.SetHour(starthour);

	NFmiTime end(start);
	end.ChangeByDays(days);
	end.SetHour(endhour);

	WeatherPeriod period(start,end);
	return period;
  }

} // namespace anonymous

namespace TextGen
{
  namespace WeatherPeriodFactory
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Create a weather period of desired type
	 *
	 * The period information is parsed from global settings
	 * in the given variable. For example, given variable name
	 * \c textgen::period the factory expects to find
	 * variable \c textgen::period::type and all other subvariables
	 * associated with the period type.
	 *
	 * Throws if the type is unknown.
	 *
	 * \see page_aikavalit
	 *
	 * \param theTime The reference time
	 * \param theVariable The name of the variable describing the period
	 * \return The desired weather period
	 */
	// ----------------------------------------------------------------------

	WeatherPeriod create(const NFmiTime & theTime,
						 const std::string & theVariable)
	{
	  const string var = theVariable + "::type";
	  const string type = NFmiSettings::instance().require(var);
	  if(type == "until")
		return period_until(theTime,theVariable);
	  if(type == "from_until")
		return period_from_until(theTime,theVariable);

	  throw TextGenError("WeatherPeriodFactory does not recognize period name "+type);

	}

  }
} // namespace TextGen
