// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherAnalysis::TimeTools
 */
// ======================================================================

#ifndef WEATHERANALYSIS_TIMETOOLS_H
#define WEATHERANALYSIS_TIMETOOLS_H

#include <ctime>

class NFmiTime;

namespace WeatherAnalysis
{
  namespace TimeTools
  {
	
	const NFmiTime toLocalTime(const NFmiTime & theUtcTime);
	const NFmiTime toUtcTime(const NFmiTime & theLocalTime);

	bool isSameDay(const NFmiTime & theDate1, const NFmiTime & theDate2);
	bool isNextDay(const NFmiTime & theDate1, const NFmiTime & theDate2);
	bool isSeveralDays(const NFmiTime & theDate1, const NFmiTime & theDate2);

	const NFmiTime dayStart(const NFmiTime & theDate);
	const NFmiTime dayEnd(const NFmiTime & theDate);
	const NFmiTime nextDay(const NFmiTime & theDate);
	const NFmiTime addHours(const NFmiTime & theDate, int theHours);

  }
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_TIMETOOLS_H

// ======================================================================
