// ======================================================================
/*!
 * \file
 * \brief Implementation of class HourPeriodGenerator
 */
// ======================================================================

#include "HourPeriodGenerator.h"
#include "WeatherPeriodTools.h"
#include "Settings.h"

using namespace std;

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

  HourPeriodGenerator::HourPeriodGenerator(const WeatherPeriod & theMainPeriod,
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
   * \brief Constructor
   *
   * The variable is expected to contain definitions for
   * \code
   * [variable]::starthour = [0-23]
   * [variable]::endhour = [0-23]
   * \endcode
   * The variables
   * \code
   * [variable]::starthour = [0-23]
   * [variable]::endhour = [0-23]
   * \endcode
   * are optional.
   *
   * \param theMainPeriod The period to iterate
   * \param theVariable The variable containing the period definitions
   */
  // ----------------------------------------------------------------------

  HourPeriodGenerator::HourPeriodGenerator(const WeatherPeriod & theMainPeriod,
														 const string & theVariable)
	: itsMainPeriod(theMainPeriod)
	, itsStartHour(Settings::require_hour(theVariable+"::starthour"))
	, itsEndHour(Settings::require_hour(theVariable+"::endhour"))
	, itsMaxStartHour(Settings::optional_hour(theVariable+"::maxstarthour",itsStartHour))
	, itsMinEndHour(Settings::optional_hour(theVariable+"::minendhour",itsEndHour))
	, itsSize(WeatherPeriodTools::countPeriods(itsMainPeriod,
											   itsStartHour,
											   itsEndHour,
											   itsMaxStartHour,
											   itsMinEndHour))
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test if the period is undivided
   *
   * \return Always false, huour period is never in principle the original
   */
  // ----------------------------------------------------------------------

  bool HourPeriodGenerator::undivided() const
  {
	return false;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the number of subperiods
   *
   * \return The number of subperiods
   */
  // ----------------------------------------------------------------------

  HourPeriodGenerator::size_type
  HourPeriodGenerator::size() const
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

  WeatherPeriod HourPeriodGenerator::period(size_type thePeriod) const
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
