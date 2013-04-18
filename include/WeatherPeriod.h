// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::WeatherPeriod
 */
// ======================================================================

#ifndef TEXTGEN_WEATHERPERIOD_H
#define TEXTGEN_WEATHERPERIOD_H

#include "TextGenTime.h"

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

	WeatherPeriod(const TextGenTime & theLocalStartTime,
				  const TextGenTime & theLocalEndTime);

	const TextGenTime & localStartTime() const;
	const TextGenTime & localEndTime() const;

	const TextGenTime & utcStartTime() const;
	const TextGenTime & utcEndTime() const;

	bool operator<(const WeatherPeriod & theRhs) const;

  private:

	WeatherPeriod();

	TextGenTime itsLocalStartTime;
	TextGenTime itsLocalEndTime;

	TextGenTime itsUtcStartTime;
	TextGenTime itsUtcEndTime;
  }; // class WeatherPeriod

} // namespace TextGen


// Free functions

bool operator==(const TextGen::WeatherPeriod & theLhs,
				const TextGen::WeatherPeriod & theRhs);

bool operator!=(const TextGen::WeatherPeriod & theLhs,
				const TextGen::WeatherPeriod & theRhs);

#endif // TEXTGEN_WEATHERPERIOD_H

// ======================================================================

