// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherPeriod
 */
// ======================================================================

#include "WeatherPeriod.h"
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
   * \param theStartTime The start time of the period
   * \param theEndTime The end time of the period
   */
  // ----------------------------------------------------------------------

  WeatherPeriod::WeatherPeriod(const NFmiTime & theStartTime,
							   const NFmiTime & theEndTime)
	: itsStartTime(theStartTime)
	, itsEndTime(theEndTime)
  {
	assert(!theEndTime.IsLessThan(theStartTime));
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Start time accessor
   *
   * \return The start time
   */
  // ----------------------------------------------------------------------

  const NFmiTime & WeatherPeriod::startTime() const
  {
	return itsStartTime;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief End time accessor
   *
   * \return The end time
   */
  // ----------------------------------------------------------------------

  const NFmiTime & WeatherPeriod::endTime() const
  {
	return itsEndTime;
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
  return(theLhs.startTime() == theRhs.startTime() &&
		 theLhs.endTime() == theRhs.endTime());
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
