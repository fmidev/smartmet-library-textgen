// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::WeatherPeriod
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherPeriod
 *
 * \brief Representation of an analysis time period
 *
 * A WeatherPeriod stores the start and end times of analysis.
 * The start and end times may be equal, but is rarely used.
 *
 * The class is intended for storage only, the provided accessors
 * are to be used externally in actual analysis code.
 *
 * Any additional functions, if any are needed, are to be placed
 * into a new WeatherPeriodTools namespace. It is possible that
 * a function for testing inclusion will be added to such a namespace
 * later on. The intention is to avoid cluttering this small class
 * with a lot of methods, since it is forseeable that all kinds
 * of interval splitting algorithms will also be needed.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERPERIOD_H
#define WEATHERANALYSIS_WEATHERPERIOD_H

#include "NFmiTime.h"

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

	WeatherPeriod(const NFmiTime & theStartTime, const NFmiTime & theEndTime);

	const NFmiTime & startTime() const;
	const NFmiTime & endTime() const;

  private:

	WeatherPeriod();

	NFmiTime itsStartTime;
	NFmiTime itsEndTime;

  }; // class WeatherPeriod

} // namespace WeatherAnalysis


// Free functions

bool operator==(const WeatherAnalysis::WeatherPeriod & theLhs,
				const WeatherAnalysis::WeatherPeriod & theRhs);

bool operator!=(const WeatherAnalysis::WeatherPeriod & theLhs,
				const WeatherAnalysis::WeatherPeriod & theRhs);

#endif // WEATHERANALYSIS_WEATHERPERIOD_H

// ======================================================================

