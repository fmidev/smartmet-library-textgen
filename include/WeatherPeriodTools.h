// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherPeriodTools
 */
// ======================================================================
/*!
 * \namespace WeatherAnalysis::WeatherPeriodTools
 *
 * \brief Various tools related to WeatherPeriod objects
 *
 * The namespace provides tools for extracting subperiods
 * from weather periods, counting the number of specified
 * subperiods (for example nights) and so on.
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERPERIODTOOLS_H
#define WEATHERANALYSIS_WEATHERPERIODTOOLS_H

namespace WeatherAnalysis
{
  class WeatherPeriod;

  namespace WeatherPeriodTools
  {

	int countPeriods(const WeatherPeriod & thePeriod,
					 int theStartHour,
					 int theEndHour);
	
	int countPeriods(const WeatherPeriod & thePeriod,
					 int theStartHour,
					 int theEndHour,
					 int theMaxStartHour,
					 int theMinEndHour);

	WeatherPeriod getPeriod(const WeatherPeriod & thePeriod,
							int thePeriod,
							int theStartHour,
							int theEndHour);

	WeatherPeriod getPeriod(const WeatherPeriod & thePeriod,
							int thePeriod,
							int theStartHour,
							int theEndHour,
							int theMaxStartHour,
							int theMinEndHour);


  } // namespace WeatherPeriodTools
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERPERIODTOOLS_H

// ======================================================================
