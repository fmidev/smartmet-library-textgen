// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::WeekdayTools
 */
// ----------------------------------------------------------------------
/*!
 * \namespace TextGen::WeekdayTools
 *
 * \brief Tools for generating weekday phrases
 *
 */
// ----------------------------------------------------------------------

#ifndef TEXTGEN_WEEKDAYTOOLS_H
#define TEXTGEN_WEEKDAYTOOLS_H

#include <string>

class NFmiTime;

namespace TextGen
{
  namespace WeekdayTools
  {

	std::string on_weekday(const NFmiTime & theTime);
	std::string night_against_weekday(const NFmiTime & theTime);
	std::string until_weekday_morning(const NFmiTime & theTime);
	std::string until_weekday_evening(const NFmiTime & theTime);
	std::string from_weekday_morning(const NFmiTime & theTime);
	std::string from_weekday_evening(const NFmiTime & theTime);

  } // namespace WeekdayTools
} // namespace TextGen

#endif // TEXTGEN_WEEKDAYTOOLS_H

// ======================================================================

