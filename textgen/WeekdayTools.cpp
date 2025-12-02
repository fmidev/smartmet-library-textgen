// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::WeekdayTools
 */
// ======================================================================
/*!
 * \namespace TextGen::WeekdayTools
 *
 * \brief Tools for generating weekday phrases
 *
 */
// ----------------------------------------------------------------------

#include "WeekdayTools.h"
#include <boost/lexical_cast.hpp>  // boost included laitettava ennen newbase:n NFmiGlobals-includea, muuten MSVC:ssa min max maarittelyt jo tehty
#include <calculator/TextGenPosixTime.h>
#include <calculator/TimeTools.h>
#include <calculator/WeatherHistory.h>
#include <macgyver/Exception.h>

using namespace std;

namespace TextGen
{
namespace WeekdayTools
{
bool is_midnight_hour(int hour)
{
  return hour == 0;
}

bool is_after_midnight_hour(int hour)
{
  return hour > 0 && hour < 4;
}

bool start_is_morning_hour(int hour)
{
  return hour >= 4 && hour <= 8;
}

bool end_is_morning_hour(int hour)
{
  return hour >= 6 && hour <= 8;
}

bool start_is_forenoon_hour(int hour)
{
  return hour >= 9 && hour <= 11;
}

bool end_is_forenoon_hour(int hour)
{
  return hour >= 9 && hour <= 11;
}

bool is_noon_hour(int hour)
{
  return hour == 12;
}

bool start_is_afternoon_hour(int hour)
{
  return hour >= 13 && hour <= 16;
}

bool end_is_afternoon_hour(int hour)
{
  return hour >= 14 && hour <= 17;
}

bool start_is_evening_hour(int hour)
{
  return hour >= 17 && hour <= 21;
}

bool end_is_evening_hour(int hour)
{
  return hour >= 17 && hour <= 20;
}

bool is_before_midnight_hour(int hour)
{
  return hour >= 22;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-na" description based on the given time
 *
 * \param theTime The time of the day
 * \return The "N-na" phrase
 */
// ----------------------------------------------------------------------

string on_weekday(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-na");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-na kello" description based on the given time
 *
 * \param theTime The time of the day
 * \return The "N-na kello" phrase
 */
// ----------------------------------------------------------------------

string on_weekday_time(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-na kello");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-vastaisena yona" phrase
 *
 * \param theTime The time
 * \return The "N-vastaisena yona" phrase
 */
// ----------------------------------------------------------------------

string night_against_weekday(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-vastaisena yona");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-aamuun" phrase
 *
 * \param theTime The time
 * \return The "N-aamuun" phrase
 */
// ----------------------------------------------------------------------

string until_weekday_morning(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-aamuun asti");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-aamupaivaan" phrase
 *
 * \param theTime The time
 * \return The "N-aamupaivaan" phrase
 */
// ----------------------------------------------------------------------

string until_weekday_forenoon(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-aamupaivaan asti");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-keskipaivaan" phrase
 *
 * \param theTime The time
 * \return The "N-keskipaivaan" phrase
 */
// ----------------------------------------------------------------------

string until_weekday_noon(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-keskipaivaan asti");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-iltapaivaan" phrase
 *
 * \param theTime The time
 * \return The "N-iltapaivaan" phrase
 */
// ----------------------------------------------------------------------

string until_weekday_afternoon(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-iltapaivaan asti");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-iltaan" phrase
 *
 * \param theTime The time
 * \return The "N-iltaan" phrase
 */
// ----------------------------------------------------------------------

string until_weekday_evening(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-iltaan asti");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-aamuun" or "N-iltaan" phrase
 *
 * \param theTime The time
 * \return The phrase
 */
// ----------------------------------------------------------------------

string until_weekday_time(const TextGenPosixTime& theTime)
{
  const int hour = theTime.GetHour();
  if (end_is_morning_hour(hour))
    return until_weekday_morning(theTime);

  if (end_is_forenoon_hour(hour))
    return until_weekday_forenoon(theTime);

  if (is_noon_hour(hour))
    return until_weekday_noon(theTime);

  if (end_is_afternoon_hour(hour))
    return until_weekday_afternoon(theTime);

  if (end_is_evening_hour(hour))
    return until_weekday_evening(theTime);

  const string msg =
      "WeekdayTools::until_weekday_time: Cannot generate -aamuun, -keskipaivaan, -iltapaivaan or "
      "-iltaan "
      "phrase for hour " +
      std::to_string(hour);
  throw Fmi::Exception(BCP, msg);
}

std::string until_weekday_morning(const TextGenPosixTime& theTime,
                                  const TextGenPosixTime& theForecastTime)
{
  if (TimeTools::isSameDay(theForecastTime, theTime))
    return "aamuun asti";
  if (TimeTools::isNextDay(theForecastTime, theTime))
    return "huomisaamuun asti";
  return until_weekday_morning(theTime);
}

std::string until_weekday_forenoon(const TextGenPosixTime& theTime,
                                   const TextGenPosixTime& theForecastTime)
{
  if (TimeTools::isSameDay(theForecastTime, theTime))
    return "aamupaivaan asti";
  if (TimeTools::isNextDay(theForecastTime, theTime))
    return "huomisaamupaivaan asti";
  return until_weekday_morning(theTime);
}

std::string until_weekday_noon(const TextGenPosixTime& theTime,
                               const TextGenPosixTime& theForecastTime)
{
  if (TimeTools::isSameDay(theForecastTime, theTime))
    return "keskipaivaan asti";
  if (TimeTools::isNextDay(theForecastTime, theTime))
    return "huomisen keskipaivaan asti";
  return until_weekday_noon(theTime);
}

std::string until_weekday_afternoon(const TextGenPosixTime& theTime,
                                    const TextGenPosixTime& theForecastTime)
{
  if (TimeTools::isSameDay(theForecastTime, theTime))
    return "iltapaivaan asti";
  if (TimeTools::isNextDay(theForecastTime, theTime))
    return "huomiseen iltapaivaan asti";
  return until_weekday_afternoon(theTime);
}

std::string until_weekday_evening(const TextGenPosixTime& theTime,
                                  const TextGenPosixTime& theForecastTime)
{
  if (TimeTools::isSameDay(theForecastTime, theTime))
    return "iltaan asti";
  if (TimeTools::isNextDay(theForecastTime, theTime))
    return "huomisiltaan asti";
  return until_weekday_evening(theTime);
}

std::string until_weekday_before_midnight(const TextGenPosixTime& theTime,
                                          const TextGenPosixTime& theForecastTime)
{
  if (TimeTools::isSameDay(theForecastTime, theTime))
    return "iltayohon asti";
  //  if (TimeTools::isNextDay(theForecastTime, theTime))
  // return "huomiseen iltayohon";
  return until_weekday_evening(theTime);
}

std::string until_weekday_time(const TextGenPosixTime& theTime,
                               const TextGenPosixTime& theForecastTime)
{
  const int hour = theTime.GetHour();
  if (end_is_morning_hour(hour))
    return until_weekday_morning(theTime, theForecastTime);

  if (end_is_forenoon_hour(hour))
    return until_weekday_forenoon(theTime, theForecastTime);

  if (is_noon_hour(hour))
    return until_weekday_noon(theTime, theForecastTime);

  if (end_is_afternoon_hour(hour))
    return until_weekday_afternoon(theTime, theForecastTime);

  if (end_is_evening_hour(hour))
    return until_weekday_evening(theTime, theForecastTime);

  if (is_before_midnight_hour(hour))
    return until_weekday_before_midnight(theTime, theForecastTime);

  const string msg =
      "WeekdayTools::until_weekday_time: Cannot generate -aamuun, -keskipaivaan, -iltapaivaan or "
      "-iltaan phrase "
      "for hour " +
      std::to_string(hour);
  throw Fmi::Exception(BCP, msg);
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-alkaen" phrase
 *
 * \param theTime The time
 * \return The "N-alkaen" phrase
 */
// ----------------------------------------------------------------------

string from_weekday(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-alkaen");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-keskiyosta" phrase
 *
 * \param theTime The time
 * \return The "N-keskiyosta" phrase
 */
// ----------------------------------------------------------------------

string from_weekday_midnight(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-keskiyosta");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-aamuyosta" phrase
 *
 * \param theTime The time
 * \return The "N-aamuyosta" phrase
 */
// ----------------------------------------------------------------------

string from_weekday_after_midnight(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-aamuyosta");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-aamusta" phrase
 *
 * \param theTime The time
 * \return The "N-aamusta" phrase
 */
// ----------------------------------------------------------------------

string from_weekday_morning(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-aamusta");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-aamupaivasta" phrase
 *
 * \param theTime The time
 * \return The "N-aamupaivasta" phrase
 */
// ----------------------------------------------------------------------

string from_weekday_forenoon(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-aamupaivasta");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-keskipaivasta" phrase
 *
 * \param theTime The time
 * \return The "N-keskipaivasta" phrase
 */
// ----------------------------------------------------------------------

string from_weekday_noon(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-keskipaivasta");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-iltapaivasta" phrase
 *
 * \param theTime The time
 * \return The "N-iltapaivasta" phrase
 */
// ----------------------------------------------------------------------

string from_weekday_afternoon(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-iltapaivasta");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-illasta" phrase
 *
 * \param theTime The time
 * \return The "N-illasta" phrase
 */
// ----------------------------------------------------------------------

string from_weekday_evening(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-illasta");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-iltayosta" phrase
 *
 * \param theTime The time
 * \return The "N-iltayosta" phrase
 */
// ----------------------------------------------------------------------

string from_weekday_before_midnight(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-iltayosta");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-aamusta" or "N-illasta" phrase
 *
 * \param theTime The time
 * \return The phrase
 */
// ----------------------------------------------------------------------

string from_weekday_time(const TextGenPosixTime& theTime)
{
  const int hour = theTime.GetHour();
  if (is_midnight_hour(hour))
    return from_weekday_midnight(theTime);

  if (is_after_midnight_hour(hour))
    return from_weekday_after_midnight(theTime);

  if (start_is_morning_hour(hour))
    return from_weekday_morning(theTime);

  if (start_is_forenoon_hour(hour))
    return from_weekday_forenoon(theTime);

  if (is_noon_hour(hour))
    return from_weekday_noon(theTime);

  if (start_is_afternoon_hour(hour))
    return from_weekday_afternoon(theTime);

  if (start_is_evening_hour(hour))
    return from_weekday_evening(theTime);

  return from_weekday_before_midnight(theTime);
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "aamusta", "huomisaamusta", "N-aamusta" or "N-illasta" phrase
 *
 * \param theTime The time
 * \param theForecastTime The forecast time
 * \return The phrase
 */
// ----------------------------------------------------------------------

string from_weekday_time(const TextGenPosixTime& theTime, const TextGenPosixTime& theForecastTime)
{
  const int hour = theTime.GetHour();

  bool sameday = TimeTools::isSameDay(theForecastTime, theTime);

  // if (TimeTools::isNextDay(theForecastTime, theTime))
  // return "huomisaamuun asti";

  if (is_midnight_hour(hour))
  {
    if (sameday)
      return "keskiyosta";
    return from_weekday_midnight(theTime);
  }

  if (is_after_midnight_hour(hour))
  {
    if (sameday)
      return "aamuyosta";
    return from_weekday_after_midnight(theTime);
  }

  if (start_is_morning_hour(hour))
  {
    if (sameday)
      return "aamusta";
    return from_weekday_morning(theTime);
  }

  if (start_is_forenoon_hour(hour))
  {
    if (sameday)
      return "aamupaivasta";
    return from_weekday_forenoon(theTime);
  }

  if (is_noon_hour(hour))
  {
    if (sameday)
      return "keskipaivasta";
    return from_weekday_noon(theTime);
  }

  if (start_is_afternoon_hour(hour))
  {
    if (sameday)
      return "iltapaivasta";
    return from_weekday_afternoon(theTime);
  }

  if (start_is_evening_hour(hour))
  {
    if (sameday)
      return "illasta";
    return from_weekday_evening(theTime);
  }

  if (sameday)
    return "iltayosta";
  return from_weekday_before_midnight(theTime);
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-aamulla" phrase
 *
 * \param theTime The time
 * \return The "N-aamulla" phrase
 */
// ----------------------------------------------------------------------

string on_weekday_morning(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-aamulla");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-aamupaivalla" phrase
 *
 * \param theTime The time
 * \return The "N-aamupaivalla" phrase
 */
// ----------------------------------------------------------------------

string on_weekday_forenoon(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-aamupaivalla");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-iltapaivalla" phrase
 *
 * \param theTime The time
 * \return The "N-iltapaivalla" phrase
 */
// ----------------------------------------------------------------------

string on_weekday_afternoon(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-iltapaivalla");
  return out;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the "N-illalla" phrase
 *
 * \param theTime The time
 * \return The "N-illalla" phrase
 */
// ----------------------------------------------------------------------

string on_weekday_evening(const TextGenPosixTime& theTime)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-illalla");
  return out;
}

std::string get_time_phrase(const TextGenPosixTime& theTime,
                            const std::string& theNewWeekdayPhrase,
                            TextGen::WeatherHistory& theHistory)
{
  TextGenPosixTime oldTime = theHistory.latestDate;
  const std::string& oldWeekdayPhrase = theHistory.latestWeekdayPhrase;

  if (theTime.GetJulianDay() == oldTime.GetJulianDay())
  {
    if (oldWeekdayPhrase != theNewWeekdayPhrase)
    {
      theHistory.updateWeekdayPhrase(theNewWeekdayPhrase, theTime);
      return theNewWeekdayPhrase;
    }

    return "";
  }

  theHistory.updateWeekdayPhrase(theNewWeekdayPhrase, theTime);
  return theNewWeekdayPhrase;

  return "";
}

std::string on_weekday(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-na");

  return get_time_phrase(theTime, out, theHistory);
}

string on_weekday_time(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-na kello");
  return get_time_phrase(theTime, out, theHistory);
}

string night_against_weekday(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-vastaisena yona");
  return get_time_phrase(theTime, out, theHistory);
}

string until_weekday_morning(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-aamuun asti");
  return get_time_phrase(theTime, out, theHistory);
}

string until_weekday_evening(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-iltaan asti");
  return get_time_phrase(theTime, out, theHistory);
}

string until_weekday_time(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory)
{
  const int hour = theTime.GetHour();
  if (end_is_morning_hour(hour))
    return until_weekday_morning(theTime, theHistory);
  if (end_is_evening_hour(hour))
    return until_weekday_evening(theTime, theHistory);

  const string msg =
      "WeekdayTools::until_weekday_time: Cannot generate -aamuun or -iltaan phrase for hour " +
      std::to_string(hour);
  throw Fmi::Exception(BCP, msg);
}

string from_weekday(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-alkaen");
  return get_time_phrase(theTime, out, theHistory);
}

string from_weekday_morning(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-aamusta");
  return get_time_phrase(theTime, out, theHistory);
}

string from_weekday_evening(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-illasta");
  return get_time_phrase(theTime, out, theHistory);
}

string from_weekday_time(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory)
{
  const int hour = theTime.GetHour();
  if (start_is_morning_hour(hour))
    return from_weekday_morning(theTime, theHistory);
  if (start_is_evening_hour(hour))
    return from_weekday_evening(theTime, theHistory);

  const string msg =
      "WeekdayTools::from_weekday_time: Cannot generate -aamusta or -illasta phrase for hour " +
      std::to_string(hour);
  throw Fmi::Exception(BCP, msg);
}

string on_weekday_morning(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-aamulla");
  return get_time_phrase(theTime, out, theHistory);
}

string on_weekday_forenoon(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-aamupaivalla");
  return get_time_phrase(theTime, out, theHistory);
}

string on_weekday_afternoon(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-iltapaivalla");
  return get_time_phrase(theTime, out, theHistory);
}

string on_weekday_evening(const TextGenPosixTime& theTime, TextGen::WeatherHistory& theHistory)
{
  string out = (std::to_string(theTime.GetWeekday()) + "-illalla");
  return get_time_phrase(theTime, out, theHistory);
}

/*
const std::string on_weekday_time(const TextGenPosixTime & theTime, const WeatherHistory&
theHistory);
const std::string night_against_weekday(const TextGenPosixTime & theTime, const WeatherHistory&
theHistory);
const std::string until_weekday_morning(const TextGenPosixTime & theTime, const WeatherHistory&
theHistory);
const std::string until_weekday_evening(const TextGenPosixTime & theTime, const WeatherHistory&
theHistory);
const std::string until_weekday_time(const TextGenPosixTime & theTime, const WeatherHistory&
theHistory);
const std::string from_weekday(const TextGenPosixTime & theTime, const WeatherHistory& theHistory);
const std::string from_weekday_morning(const TextGenPosixTime & theTime, const WeatherHistory&
theHistory);
const std::string from_weekday_evening(const TextGenPosixTime & theTime, const WeatherHistory&
theHistory);
const std::string from_weekday_time(const TextGenPosixTime & theTime, const WeatherHistory&
theHistory);
const std::string on_weekday_morning(const TextGenPosixTime & theTime, const WeatherHistory&
theHistory);
const std::string on_weekday_forenoon(const TextGenPosixTime & theTime, const WeatherHistory&
theHistory);
const std::string on_weekday_afternoon(const TextGenPosixTime & theTime, const WeatherHistory&
theHistory);
const std::string on_weekday_evening(const TextGenPosixTime & theTime, const WeatherHistory&
theHistory);


WeekdayTools.h headeriin pitaa sitten lisata tarvittavia funktioita
tyyliin

const std::string on_weekday_time(const TextGenPosixTime & theTime,
                            WeatherHistory & theHistory)
{
// jos paiva on sama kuin historiassa, tutkitaan vain kellon
// aikaa, ja verrataan valittua fraasia viimeeksi kaytettyyn.
// Muutoin pitaa generoida fraasi jossa mainitaan myos
// paiva, ja se olisi joko tanaan, huomenna, ylihuomenna
// tai viikonpaivana.
}
*/

}  // namespace WeekdayTools
}  // namespace TextGen

// ======================================================================
