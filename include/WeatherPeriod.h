// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::WeatherPeriod
 */
// ======================================================================

#ifndef TEXTGEN_WEATHERPERIOD_H
#define TEXTGEN_WEATHERPERIOD_H

#include <newbase/NFmiTime.h>

namespace TextGen
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

} // namespace TextGen


// Free functions

bool operator==(const TextGen::WeatherPeriod & theLhs,
				const TextGen::WeatherPeriod & theRhs);

bool operator!=(const TextGen::WeatherPeriod & theLhs,
				const TextGen::WeatherPeriod & theRhs);

#endif // TEXTGEN_WEATHERPERIOD_H

// ======================================================================

