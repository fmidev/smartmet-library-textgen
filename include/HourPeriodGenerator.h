// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::HourPeriodGenerator
 */
// ======================================================================

#ifndef WEATHERANALYSIS_HOURPERIODGENERATOR_H
#define WEATHERANALYSIS_HOURPERIODGENERATOR_H

#include "WeatherPeriodGenerator.h"
#include "WeatherPeriod.h"

#include <string>

namespace WeatherAnalysis
{
  class WeatherPeriod;

  class HourPeriodGenerator : public WeatherPeriodGenerator
  {
  public:
	typedef WeatherPeriodGenerator::size_type size_type;

	HourPeriodGenerator(const WeatherPeriod & theMainPeriod,
						int theStartHour,
						int theEndHour,
						int theMaxStartHour,
						int theMinEndHour);
	
	HourPeriodGenerator(const WeatherPeriod & theMainPeriod,
						const std::string & theVariable);
	
	virtual ~HourPeriodGenerator() { }
	virtual bool undivided() const;
	virtual size_type size() const;
	virtual const WeatherPeriod period() const;
	virtual const WeatherPeriod period(size_type thePeriod) const;

  private:

	HourPeriodGenerator(void);

	const WeatherPeriod itsMainPeriod;
	const int itsStartHour;
	const int itsEndHour;
	const int itsMaxStartHour;
	const int itsMinEndHour;
	const size_type itsSize;

  }; // class HourPeriodGenerator

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_HOURPERIODGENERATOR_H

// ======================================================================

