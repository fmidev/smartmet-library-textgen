// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::WeekdayTools
 */
// ----------------------------------------------------------------------

#ifndef TEXTGEN_WEEKDAYTOOLS_H
#define TEXTGEN_WEEKDAYTOOLS_H

#include <string>

using namespace WeatherAnalysis;
#include "WeatherHistory.h"

class NFmiTime;


namespace TextGen
{
  namespace WeekdayTools
  {

	const std::string on_weekday(const NFmiTime & theTime);
	const std::string on_weekday_time(const NFmiTime & theTime);
	const std::string night_against_weekday(const NFmiTime & theTime);
	const std::string until_weekday_morning(const NFmiTime & theTime);
	const std::string until_weekday_evening(const NFmiTime & theTime);
	const std::string until_weekday_time(const NFmiTime & theTime);
	const std::string from_weekday(const NFmiTime & theTime);
	const std::string from_weekday_morning(const NFmiTime & theTime);
	const std::string from_weekday_evening(const NFmiTime & theTime);
	const std::string from_weekday_time(const NFmiTime & theTime);
	const std::string on_weekday_morning(const NFmiTime & theTime);
	const std::string on_weekday_forenoon(const NFmiTime & theTime);
	const std::string on_weekday_afternoon(const NFmiTime & theTime);
	const std::string on_weekday_evening(const NFmiTime & theTime);

	const std::string on_weekday(const NFmiTime & theTime, WeatherHistory& theHistory);
	const std::string on_weekday_time(const NFmiTime & theTime, WeatherHistory& theHistory);
	const std::string night_against_weekday(const NFmiTime & theTime, WeatherHistory& theHistory);
	const std::string until_weekday_morning(const NFmiTime & theTime, WeatherHistory& theHistory);
	const std::string until_weekday_evening(const NFmiTime & theTime, WeatherHistory& theHistory);
	const std::string until_weekday_time(const NFmiTime & theTime, WeatherHistory& theHistory);
	const std::string from_weekday(const NFmiTime & theTime, WeatherHistory& theHistory);
	const std::string from_weekday_morning(const NFmiTime & theTime, WeatherHistory& theHistory);
	const std::string from_weekday_evening(const NFmiTime & theTime, WeatherHistory& theHistory);
	const std::string from_weekday_time(const NFmiTime & theTime, WeatherHistory& theHistory);
	const std::string on_weekday_morning(const NFmiTime & theTime, WeatherHistory& theHistory);
	const std::string on_weekday_forenoon(const NFmiTime & theTime, WeatherHistory& theHistory);
	const std::string on_weekday_afternoon(const NFmiTime & theTime, WeatherHistory& theHistory);
	const std::string on_weekday_evening(const NFmiTime & theTime, WeatherHistory& theHistory);

  } // namespace WeekdayTools
} // namespace TextGen

#endif // TEXTGEN_WEEKDAYTOOLS_H

// ======================================================================

