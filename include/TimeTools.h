// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TimeTools
 */
// ======================================================================

#ifndef TEXTGEN_TIMETOOLS_H
#define TEXTGEN_TIMETOOLS_H

class NFmiTime;

namespace TextGen
{
  namespace TimeTools
  {

	bool isSameDay(const NFmiTime & theDate1, const NFmiTime & theDate2);
	bool isNextDay(const NFmiTime & theDate1, const NFmiTime & theDate2);
	bool isSeveralDays(const NFmiTime & theDate1, const NFmiTime & theDate2);

  }
} // namespace TextGen

#endif // TEXTGEN_TIMETOOLS_H

// ======================================================================
