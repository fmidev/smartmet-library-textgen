// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherAnalysis::TimeTools
 */
// ======================================================================

#ifndef WEATHERANALYSIS_TIMETOOLS_H
#define WEATHERANALYSIS_TIMETOOLS_H

class NFmiTime;

namespace WeatherAnalysis
{
  namespace TimeTools
  {
	
	NFmiTime toLocalTime(const NFmiTime & theUtcTime);
	NFmiTime toUtcTime(const NFmiTime & theLocalTime);

	bool isSameDay(const NFmiTime & theDate1, const NFmiTime & theDate2);
	bool isNextDay(const NFmiTime & theDate1, const NFmiTime & theDate2);
	bool isSeveralDays(const NFmiTime & theDate1, const NFmiTime & theDate2);

	NFmiTime dayStart(const NFmiTime & theDate);
	NFmiTime dayEnd(const NFmiTime & theDate);
	NFmiTime nextDay(const NFmiTime & theDate);
	NFmiTime addHours(const NFmiTime & theDate, int theHours);

  }
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_TIMETOOLS_H

// ======================================================================
