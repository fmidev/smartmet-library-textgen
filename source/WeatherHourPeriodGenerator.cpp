// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherHourPeriodGenerator
 */
// ======================================================================

#include "WeatherHourPeriodGenerator.h"
#include "WeatherPeriodTools.h"

namespace WeatherAnalysis
{
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theMainPeriod The period to iterate
   * \param theStartHour The start hour of each generated period
   * \param theEndHour The end hour of each generated period
   * \param theMaxStartHour The maximum delayed start hour of each period
   * \param theMinEndHour The minimum early end hour of each period
   */
  // ----------------------------------------------------------------------

  WeatherHourPeriodGenerator::WeatherHourPeriodGenerator(const WeatherPeriod & theMainPeriod,
														 int theStartHour,
														 int theEndHour,
														 int theMaxStartHour,
														 int theMinEndHour)
	: itsMainPeriod(theMainPeriod)
	, itsStartHour(theStartHour)
	, itsEndHour(theEndHour)
	, itsMaxStartHour(theMaxStartHour)
	, itsMinEndHour(theMinEndHour)
	, itsSize(WeatherPeriodTools::countPeriods(theMainPeriod,
											   theStartHour,
											   theEndHour,
											   theMaxStartHour,
											   theMinEndHour))
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the number of subperiods
   *
   * \return The number of subperiods
   */
  // ----------------------------------------------------------------------

  WeatherHourPeriodGenerator::size_type
  WeatherHourPeriodGenerator::size() const
  {
	return itsSize;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the desired subperiod
   *
   * Throws if there is no such subperiod
   *
   * \param thePeriod The index of the subperiod
   * \return The subperiod
   */
  // ----------------------------------------------------------------------

  WeatherPeriod WeatherHourPeriodGenerator::period(size_type thePeriod) const
  {
	return WeatherPeriodTools::getPeriod(itsMainPeriod,
										 thePeriod,
										 itsStartHour,
										 itsEndHour,
										 itsMaxStartHour,
										 itsMinEndHour);
  }

} // namespace WeatherAnalysis

// ======================================================================
