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

// ======================================================================
