// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::WeatherPeriod
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERPERIOD_H
#define WEATHERANALYSIS_WEATHERPERIOD_H

#include <newbase/NFmiTime.h>

namespace WeatherAnalysis
{
  class WeatherPeriod
  {
  public:

#ifdef NO_COMPILER_GENERATED
	~WeatherPeriod();
	WeatherPeriod(const WeatherPeriod & thePeriod);
	WeatherPeriod & operator=(const WeatherPeriod & thePeriod);
#endif

	WeatherPeriod(const NFmiTime & theLocalStartTime,
				  const NFmiTime & theLocalEndTime);

	const NFmiTime & localStartTime() const;
	const NFmiTime & localEndTime() const;

	const NFmiTime & utcStartTime() const;
	const NFmiTime & utcEndTime() const;

	bool operator<(const WeatherPeriod & theRhs) const;

  private:

	WeatherPeriod();

	NFmiTime itsLocalStartTime;
	NFmiTime itsLocalEndTime;

	NFmiTime itsUtcStartTime;
	NFmiTime itsUtcEndTime;

  }; // class WeatherPeriod

} // namespace WeatherAnalysis


// Free functions

bool operator==(const WeatherAnalysis::WeatherPeriod & theLhs,
				const WeatherAnalysis::WeatherPeriod & theRhs);

bool operator!=(const WeatherAnalysis::WeatherPeriod & theLhs,
				const WeatherAnalysis::WeatherPeriod & theRhs);

#endif // WEATHERANALYSIS_WEATHERPERIOD_H

// ======================================================================

