// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::WeatherPeriod
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherPeriod
 *
 * \brief Representation of an analysis time period
 *
 * A WeatherPeriod stores the start and end times of analysis.
 * The start and end times may be equal, but is rarely used.
 *
 * The class is intended for storage only, the provided accessors
 * are to be used externally in actual analysis code.
 *
 * Any additional functions, if any are needed, are to be placed
 * into a new WeatherPeriodTools namespace. It is possible that
 * a function for testing inclusion will be added to such a namespace
 * later on. The intention is to avoid cluttering this small class
 * with a lot of methods, since it is forseeable that all kinds
 * of interval splitting algorithms will also be needed.
 *
 */
// ======================================================================

#include "WeatherPeriod.h"
#include "TimeTools.h"
#include "WeatherAnalysisError.h"
#include <cassert>

namespace WeatherAnalysis
{

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
	, itsUtcStartTime(TimeTools::toUtcTime(theLocalStartTime))
	, itsUtcEndTime(TimeTools::toUtcTime(theLocalEndTime))
  {
	if(theLocalEndTime.IsLessThan(theLocalStartTime))
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

  // ----------------------------------------------------------------------
  /*!
   * \brief Less-than comparison for WeatherAnalysis::WeatherPeriod
   *
   * We define < to mean the lexicographic ordering based on the
   * start time and then the end time.
   *
   * \param theRhs The right hand side
   * \return True if the theLhs < theRhs
   */
  // ----------------------------------------------------------------------
  
  bool WeatherPeriod::operator<(const WeatherAnalysis::WeatherPeriod & theRhs) const
  {
	if(utcStartTime() != theRhs.utcStartTime())
	  return (utcStartTime() < theRhs.utcStartTime());
	return (utcEndTime() < theRhs.utcEndTime());
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
