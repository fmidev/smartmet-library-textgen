// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::WeatherPeriodFactory
 */
// ======================================================================

#include "WeatherPeriodFactory.h"
#include "WeatherPeriod.h"
#include "TextGenError.h"

#include "NFmiTime.h"

using namespace WeatherAnalysis;

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
   * \brief Create period from now until tomorrow (or next) evening
   *
   * \param theTime The local reference time
   * \return The period
   */
  // ----------------------------------------------------------------------

  WeatherPeriod farmer_until_tomorrow_evening(const NFmiTime & theTime)
  {
	// Start time is first full hour after the reference time
	NFmiTime start(round_up(theTime));
	// End time is evening at +1 or +2 days
	NFmiTime end(start);
	if(start.GetHour() < 12)
	  end.ChangeByDays(1);
	else
	  end.ChangeByDays(2);
	end.SetHour(18);

	WeatherPeriod period(start,end);
	return period;

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Create period from tomorrow (or next) evening to +4 days
   *
   * \param theTime The local reference time
   * \return The period
   */
  // ----------------------------------------------------------------------

  WeatherPeriod farmer_from_tomorrow_evening(const NFmiTime & theTime)
  {
	// Start time is end time of farmer_until_tomorrow_evening

	WeatherPeriod previous_period(farmer_until_tomorrow_evening(theTime));
	NFmiTime start(previous_period.localEndTime());

	// End time is morning at +4 days

	NFmiTime end(start);
	end.ChangeByDays(4);
	end.SetHour(6);
	
	WeatherPeriod period(start,end);
	return period;
	
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Create period from morning until morning at days +5
   *
   * \param theTime The local reference time
   * \return The period
   */
  // ----------------------------------------------------------------------

  WeatherPeriod farmer_five_days(const NFmiTime & theTime)
  {
	// Start time is next 06-hour

	NFmiTime start(round_up(theTime));
	if(start.GetHour() > 6)
	  start.ChangeByDays(1);
	start.SetHour(6);

	// End time
	NFmiTime end(start);
	end.ChangeByDays(5);

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
	 * Throws if the type is unknown.
	 *
	 * \see page_aikavalit
	 *
	 * \param theTime The reference time
	 * \param theName The name of the weather type
	 * \return The desired weather period
	 */
	// ----------------------------------------------------------------------

	WeatherPeriod create(const NFmiTime & theTime,
						 const std::string & theName)
	{

	  if(theName == "farmer_until_tomorrow_evening")
		return farmer_until_tomorrow_evening(theTime);
	  if(theName == "farmer_from_tomorrow_evening")
		return farmer_from_tomorrow_evening(theTime);
	  if(theName == "farmer_five_days")
		return farmer_five_days(theTime);
	  throw TextGenError("WeatherPeriodFactory does not recognize period name "+theName);

	}

  }
} // namespace TextGen
