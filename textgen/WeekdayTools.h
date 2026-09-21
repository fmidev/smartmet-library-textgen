// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::WeekdayTools
 */
// ----------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>

#include <calculator/WeatherHistory.h>

class TextGenPosixTime;

namespace TextGen
{
class WeatherPeriod;
}

namespace TextGen
{
namespace WeekdayTools
{
std::string on_weekday(const TextGenPosixTime& theTime);
std::string on_weekday_time(const TextGenPosixTime& theTime);
std::string night_against_weekday(const TextGenPosixTime& theTime);

std::string until_weekday_morning(const TextGenPosixTime& theTime);
std::string until_weekday_forenoon(const TextGenPosixTime& theTime);
std::string until_weekday_noon(const TextGenPosixTime& theTime);
std::string until_weekday_afternoon(const TextGenPosixTime& theTime);
std::string until_weekday_evening(const TextGenPosixTime& theTime);
std::string until_weekday_time(const TextGenPosixTime& theTime);

std::string until_weekday_morning(const TextGenPosixTime& theTime,
                                  const TextGenPosixTime& theForecastTime);
std::string until_weekday_forenoon(const TextGenPosixTime& theTime,
                                   const TextGenPosixTime& theForecastTime);
std::string until_weekday_noon(const TextGenPosixTime& theTime,
                               const TextGenPosixTime& theForecastTime);
std::string until_weekday_afternoon(const TextGenPosixTime& theTime,
                                    const TextGenPosixTime& theForecastTime);
std::string until_weekday_evening(const TextGenPosixTime& theTime,
                                  const TextGenPosixTime& theForecastTime);
std::string until_weekday_before_midnight(const TextGenPosixTime& theTime,
                                          const TextGenPosixTime& theForecastTime);
std::string until_weekday_time(const TextGenPosixTime& theTime,
                               const TextGenPosixTime& theForecastTime);

std::string from_weekday(const TextGenPosixTime& theTime);
std::string from_weekday_midnight(const TextGenPosixTime& theTime);
std::string from_weekday_after_midnight(const TextGenPosixTime& theTime);
std::string from_weekday_morning(const TextGenPosixTime& theTime);
std::string from_weekday_forenoon(const TextGenPosixTime& theTime);
std::string from_weekday_noon(const TextGenPosixTime& theTime);
std::string from_weekday_afternoon(const TextGenPosixTime& theTime);
std::string from_weekday_evening(const TextGenPosixTime& theTime);
std::string from_weekday_before_midnight(const TextGenPosixTime& theTime);
std::string from_weekday_time(const TextGenPosixTime& theTime);
std::string from_weekday_time(const TextGenPosixTime& theTime,
                              const TextGenPosixTime& theForecastTime);

std::string on_weekday_morning(const TextGenPosixTime& theTime);
std::string on_weekday_forenoon(const TextGenPosixTime& theTime);
std::string on_weekday_afternoon(const TextGenPosixTime& theTime);
std::string on_weekday_evening(const TextGenPosixTime& theTime);

std::string on_weekday(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory);
std::string on_weekday_time(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory);
std::string night_against_weekday(const TextGenPosixTime& theTime,
                                  TextGen::WeatherHistory& theHistory);
std::string until_weekday_morning(const TextGenPosixTime& theTime,
                                  TextGen::WeatherHistory& theHistory);
std::string until_weekday_evening(const TextGenPosixTime& theTime,
                                  TextGen::WeatherHistory& theHistory);
std::string until_weekday_time(const TextGenPosixTime& theTime,
                               TextGen::WeatherHistory& theHistory);
std::string from_weekday(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory);
std::string from_weekday_morning(const TextGenPosixTime& theTime,
                                 TextGen::WeatherHistory& theHistory);
std::string from_weekday_evening(const TextGenPosixTime& theTime,
                                 TextGen::WeatherHistory& theHistory);
std::string from_weekday_time(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory);
std::string on_weekday_morning(const TextGenPosixTime& theTime,
                               TextGen::WeatherHistory& theHistory);
std::string on_weekday_forenoon(const TextGenPosixTime& theTime,
                                TextGen::WeatherHistory& theHistory);
std::string on_weekday_afternoon(const TextGenPosixTime& theTime,
                                 TextGen::WeatherHistory& theHistory);
std::string on_weekday_evening(const TextGenPosixTime& theTime,
                               TextGen::WeatherHistory& theHistory);
std::string get_time_phrase(const TextGenPosixTime& theTime,
                            const std::string& theNewWeekdayPhrase,
                            TextGen::WeatherHistory& theHistory);

// Marking a change of day in the time phrases of the wind stories.
//
// The preferences come from <var>::day::phrases as a list of "tomorrow", "weekday" and
// "none", or from the weekdays flag: true gives "weekday", false gives "tomorrow".
std::vector<std::string> day_phrase_preferences(const std::string& theVar, bool theWeekdays);

// Forget a history that is not from the given period, such as one left by another product
void forget_history_outside(TextGen::WeatherHistory& theHistory,
                            const TextGen::WeatherPeriod& thePeriod);

// A time phrase such as "iltapaivalla" or "illasta alkaen" for a moment, with a marker when
// the day differs from the previous time phrase (or from the forecast time for the first one)
// and the reader could not infer it: "huomenna iltapaivalla" or "2-iltapaivalla". The day is
// inferred when the text moves from a later part of the day to an earlier one, and midnight
// needs no marker. The first form uses and updates the history, the second one the given
// previous phrase.
std::string day_phase_phrase(const TextGenPosixTime& theTime,
                             const TextGenPosixTime& theForecastTime,
                             const std::string& thePhrase,
                             const std::vector<std::string>& thePreferences,
                             TextGen::WeatherHistory& theHistory);
std::string day_phase_phrase(const TextGenPosixTime& theTime,
                             const TextGenPosixTime& theForecastTime,
                             const std::string& thePhrase,
                             const std::vector<std::string>& thePreferences,
                             bool theHavePrevious,
                             const TextGenPosixTime& thePreviousTime,
                             const std::string& thePreviousPhrase);

}  // namespace WeekdayTools
}  // namespace TextGen

// ======================================================================
