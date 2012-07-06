// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::TimeTools
 */
// ======================================================================

#ifndef TEXTGEN_TIMETOOLS_H
#define TEXTGEN_TIMETOOLS_H

#include <ctime>

class NFmiTime;

namespace TextGen
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
} // namespace TextGen

#endif // TEXTGEN_TIMETOOLS_H

// ======================================================================
