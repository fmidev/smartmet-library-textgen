// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::WeekdayTools
 */
// ----------------------------------------------------------------------

#ifndef TEXTGEN_WEEKDAYTOOLS_H
#define TEXTGEN_WEEKDAYTOOLS_H

#include <string>

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

	const std::string on_weekday(const NFmiTime & theTime, WeatherAnalysis::WeatherHistory& theHistory);
	const std::string on_weekday_time(const NFmiTime & theTime, WeatherAnalysis::WeatherHistory& theHistory);
	const std::string night_against_weekday(const NFmiTime & theTime, WeatherAnalysis::WeatherHistory& theHistory);
	const std::string until_weekday_morning(const NFmiTime & theTime, WeatherAnalysis::WeatherHistory& theHistory);
	const std::string until_weekday_evening(const NFmiTime & theTime, WeatherAnalysis::WeatherHistory& theHistory);
	const std::string until_weekday_time(const NFmiTime & theTime, WeatherAnalysis::WeatherHistory& theHistory);
	const std::string from_weekday(const NFmiTime & theTime, WeatherAnalysis::WeatherHistory& theHistory);
	const std::string from_weekday_morning(const NFmiTime & theTime, WeatherAnalysis::WeatherHistory& theHistory);
	const std::string from_weekday_evening(const NFmiTime & theTime, WeatherAnalysis::WeatherHistory& theHistory);
	const std::string from_weekday_time(const NFmiTime & theTime, WeatherAnalysis::WeatherHistory& theHistory);
	const std::string on_weekday_morning(const NFmiTime & theTime, WeatherAnalysis::WeatherHistory& theHistory);
	const std::string on_weekday_forenoon(const NFmiTime & theTime, WeatherAnalysis::WeatherHistory& theHistory);
	const std::string on_weekday_afternoon(const NFmiTime & theTime, WeatherAnalysis::WeatherHistory& theHistory);
	const std::string on_weekday_evening(const NFmiTime & theTime, WeatherAnalysis::WeatherHistory& theHistory);
	const std::string get_time_phrase(const NFmiTime & theTime, 
									  const std::string theNewPhrase, 
									  WeatherAnalysis::WeatherHistory& theHistory);

  } // namespace WeekdayTools
} // namespace TextGen

#endif // TEXTGEN_WEEKDAYTOOLS_H

// ======================================================================

