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

class TextGenTime;

namespace TextGen
{
  namespace WeekdayTools
  {

	std::string on_weekday(const TextGenTime & theTime);
	std::string on_weekday_time(const TextGenTime & theTime);
	std::string night_against_weekday(const TextGenTime & theTime);
	std::string until_weekday_morning(const TextGenTime & theTime);
	std::string until_weekday_evening(const TextGenTime & theTime);
	std::string until_weekday_time(const TextGenTime & theTime);
	std::string from_weekday(const TextGenTime & theTime);
	std::string from_weekday_morning(const TextGenTime & theTime);
	std::string from_weekday_evening(const TextGenTime & theTime);
	std::string from_weekday_time(const TextGenTime & theTime);
	std::string on_weekday_morning(const TextGenTime & theTime);
	std::string on_weekday_forenoon(const TextGenTime & theTime);
	std::string on_weekday_afternoon(const TextGenTime & theTime);
	std::string on_weekday_evening(const TextGenTime & theTime);

	std::string on_weekday(const TextGenTime & theTime, TextGen::WeatherHistory& theHistory);
	std::string on_weekday_time(const TextGenTime & theTime, TextGen::WeatherHistory& theHistory);
	std::string night_against_weekday(const TextGenTime & theTime, TextGen::WeatherHistory& theHistory);
	std::string until_weekday_morning(const TextGenTime & theTime, TextGen::WeatherHistory& theHistory);
	std::string until_weekday_evening(const TextGenTime & theTime, TextGen::WeatherHistory& theHistory);
	std::string until_weekday_time(const TextGenTime & theTime, TextGen::WeatherHistory& theHistory);
	std::string from_weekday(const TextGenTime & theTime, TextGen::WeatherHistory& theHistory);
	std::string from_weekday_morning(const TextGenTime & theTime, TextGen::WeatherHistory& theHistory);
	std::string from_weekday_evening(const TextGenTime & theTime, TextGen::WeatherHistory& theHistory);
	std::string from_weekday_time(const TextGenTime & theTime, TextGen::WeatherHistory& theHistory);
	std::string on_weekday_morning(const TextGenTime & theTime, TextGen::WeatherHistory& theHistory);
	std::string on_weekday_forenoon(const TextGenTime & theTime, TextGen::WeatherHistory& theHistory);
	std::string on_weekday_afternoon(const TextGenTime & theTime, TextGen::WeatherHistory& theHistory);
	std::string on_weekday_evening(const TextGenTime & theTime, TextGen::WeatherHistory& theHistory);
	std::string get_time_phrase(const TextGenTime & theTime, 
								const std::string theNewPhrase, 
								TextGen::WeatherHistory& theHistory);

  } // namespace WeekdayTools
} // namespace TextGen

#endif // TEXTGEN_WEEKDAYTOOLS_H

// ======================================================================

