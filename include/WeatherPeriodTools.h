// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherAnalysis::WeatherPeriodTools
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERPERIODTOOLS_H
#define WEATHERANALYSIS_WEATHERPERIODTOOLS_H

namespace WeatherAnalysis
{
  class WeatherPeriod;

  namespace WeatherPeriodTools
  {

	int hours(const WeatherPeriod & thePeriod);

	int countPeriods(const WeatherPeriod & thePeriod,
					 int theStartHour,
					 int theEndHour);
	
	int countPeriods(const WeatherPeriod & thePeriod,
					 int theStartHour,
					 int theEndHour,
					 int theMaxStartHour,
					 int theMinEndHour);

	WeatherPeriod getPeriod(const WeatherPeriod & thePeriod,
							int theNumber,
							int theStartHour,
							int theEndHour);

	WeatherPeriod getPeriod(const WeatherPeriod & thePeriod,
							int theNumber,
							int theStartHour,
							int theEndHour,
							int theMaxStartHour,
							int theMinEndHour);


  } // namespace WeatherPeriodTools
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERPERIODTOOLS_H

// ======================================================================
