// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherHourPeriodGenerator
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherHourPeriodGenerator
 *
 * \brief Generates a sequence of periods
 *
 * This class uses WeatherPeriodTools countPeriods and getPeriod
 * as the basis for generating a sequence of subperiods. Hence
 * the basic idea is to define the acceptable hour interval in
 * the constructor, the accessors will then take care of the rest.
 */
// ----------------------------------------------------------------------

#ifndef WEATHERANALYSIS_WEATHERHOURPERIODGENERATOR_H
#define WEATHERANALYSIS_WEATHERHOURPERIODGENERATOR_H

#include "WeatherPeriodGenerator.h"
#include "WeatherPeriod.h"

namespace WeatherAnalysis
{
  class WeatherPeriod;

  class WeatherHourPeriodGenerator : public WeatherPeriodGenerator
  {
  public:
	typedef WeatherPeriodGenerator::size_type size_type;

	WeatherHourPeriodGenerator(const WeatherPeriod & theMainPeriod,
							   int theStartHour,
							   int theEndHour,
							   int theMaxStartHour,
							   int theMinEndHour);

	virtual ~WeatherHourPeriodGenerator() { }
	virtual size_type size() const;
	virtual WeatherPeriod period(size_type thePeriod) const;

  private:

	WeatherHourPeriodGenerator(void);

	const WeatherPeriod itsMainPeriod;
	const int itsStartHour;
	const int itsEndHour;
	const int itsMaxStartHour;
	const int itsMinEndHour;
	const size_type itsSize;

  }; // class WeatherHourPeriodGenerator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERHOURPERIODGENERATOR_H

// ======================================================================

