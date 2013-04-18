// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::TimeTools
 */
// ======================================================================

#ifndef TEXTGEN_TIMETOOLS_H
#define TEXTGEN_TIMETOOLS_H

#include <ctime>

class TextGenTime;

namespace TextGen
{
  namespace TimeTools
  {
	
	TextGenTime toLocalTime(const TextGenTime & theUtcTime);
	TextGenTime toUtcTime(const TextGenTime & theLocalTime);

	bool isSameDay(const TextGenTime & theDate1, const TextGenTime & theDate2);
	bool isNextDay(const TextGenTime & theDate1, const TextGenTime & theDate2);
	bool isSeveralDays(const TextGenTime & theDate1, const TextGenTime & theDate2);

	TextGenTime dayStart(const TextGenTime & theDate);
	TextGenTime dayEnd(const TextGenTime & theDate);
	TextGenTime nextDay(const TextGenTime & theDate);
	TextGenTime addHours(const TextGenTime & theDate, int theHours);

  }
} // namespace TextGen

#endif // TEXTGEN_TIMETOOLS_H

// ======================================================================
