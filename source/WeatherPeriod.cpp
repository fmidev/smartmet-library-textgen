// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::WeatherPeriod
 */
// ======================================================================

#include "WeatherPeriod.h"
#include "WeatherAnalysisError.h"
#include <cassert>
#include <ctime>

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Convert local time to UTC time using current TZ
   *
   * \param theLocalTime The local time
   * \return The UTC time
   */
  // ----------------------------------------------------------------------

  NFmiTime toutc(const NFmiTime & theLocalTime)
  {
	::tm tlocal;
	tlocal.tm_sec   = theLocalTime.GetSec();
	tlocal.tm_min   = theLocalTime.GetMin();
	tlocal.tm_hour  = theLocalTime.GetHour();
	tlocal.tm_mday  = theLocalTime.GetDay();
	tlocal.tm_mon   = theLocalTime.GetMonth()-1;
	tlocal.tm_year  = theLocalTime.GetYear()-1900;
	tlocal.tm_wday  = -1;
	tlocal.tm_yday  = -1;
	tlocal.tm_isdst = -1;

	::time_t tsec = mktime(&tlocal);

	::tm * tutc = ::gmtime(&tsec);

	NFmiTime out(tutc->tm_year + 1900,
				 tutc->tm_mon + 1,
				 tutc->tm_mday,
				 tutc->tm_hour,
				 tutc->tm_min,
				 tutc->tm_sec);
	
	return out;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * Construction is possible only by explicitly stating the
   * start and end times, or by copy constructing. The void
   * constructor is intentionally disabled.
   *
   * It is assumed that the proper timezone has been set
   * prior to constructing the object. This can be done
   * for example by using
   * \code
   * putenv("TZ=Europe/Helsinki");
   * tzset();
   * \endcode
   * See the man-pages for \c tzset for more information.
   * In Linux the available timezones are listed in
   * directory \c /usr/share/zoneinfo
   *
   * \param theLocalStartTime The start time of the period
   * \param theLocalEndTime The end time of the period
   */
  // ----------------------------------------------------------------------

  WeatherPeriod::WeatherPeriod(const NFmiTime & theLocalStartTime,
							   const NFmiTime & theLocalEndTime)
	: itsLocalStartTime(theLocalStartTime)
	, itsLocalEndTime(theLocalEndTime)
	, itsUtcStartTime(toutc(theLocalStartTime))
	, itsUtcEndTime(toutc(theLocalEndTime))
  {
	if(!theLocalEndTime.IsLessThan(theLocalStartTime))
	  throw WeatherAnalysisError("WeatherPeriod: end time must be after start time");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Start time accessor
   *
   * \return The start time
   */
  // ----------------------------------------------------------------------

  const NFmiTime & WeatherPeriod::localStartTime() const
  {
	return itsLocalStartTime;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief End time accessor
   *
   * \return The end time
   */
  // ----------------------------------------------------------------------

  const NFmiTime & WeatherPeriod::localEndTime() const
  {
	return itsLocalEndTime;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Start time accessor
   *
   * \return The start time
   */
  // ----------------------------------------------------------------------

  const NFmiTime & WeatherPeriod::utcStartTime() const
  {
	return itsUtcStartTime;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief End time accessor
   *
   * \return The end time
   */
  // ----------------------------------------------------------------------

  const NFmiTime & WeatherPeriod::utcEndTime() const
  {
	return itsUtcEndTime;
  }

} // namespace WeatherAnalysis

// ----------------------------------------------------------------------
/*!
 * \brief Equality comparison for WeatherAnalysis::WeatherPeriod
 *
 * \param theLhs The left hand side
 * \param theRhs The right hand side
 * \return True if the periods are equal
 */
// ----------------------------------------------------------------------

bool operator==(const WeatherAnalysis::WeatherPeriod & theLhs,
				const WeatherAnalysis::WeatherPeriod & theRhs)
{
  return(theLhs.utcStartTime() == theRhs.utcStartTime() &&
		 theLhs.utcEndTime() == theRhs.utcEndTime());
}

// ----------------------------------------------------------------------
/*!
 * \brief Inequality comparison for WeatherAnalysis::WeatherPeriod
 *
 * \param theLhs The left hand side
 * \param theRhs The right hand side
 * \return True if the periods are not equal
 */
// ----------------------------------------------------------------------

bool operator!=(const WeatherAnalysis::WeatherPeriod & theLhs,
				const WeatherAnalysis::WeatherPeriod & theRhs)
{
  return !(theLhs == theRhs);
}

// ======================================================================
