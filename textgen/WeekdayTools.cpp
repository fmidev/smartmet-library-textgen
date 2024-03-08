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
#include <calculator/TextGenError.h>
#include <calculator/TextGenPosixTime.h>
#include <calculator/TimeTools.h>
#include <calculator/WeatherHistory.h>

using namespace std;

namespace TextGen
{
namespace WeekdayTools
{
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
  if (hour == 6)
    return until_weekday_morning(theTime);

  if (hour == 12)
    return until_weekday_noon(theTime);

  if (hour >= 13 && hour <= 16)
    return until_weekday_afternoon(theTime);

  if (hour >= 18 && hour <= 21)
    return until_weekday_evening(theTime);

  const string msg =
      "WeekdayTools::until_weekday_time: Cannot generate -aamuun, -keskipaivaan, -iltapaivaan or "
      "-iltaan "
      "phrase for hour " +
      std::to_string(hour);
  throw TextGenError(msg);
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

std::string until_weekday_time(const TextGenPosixTime& theTime,
                               const TextGenPosixTime& theForecastTime)
{
  const int hour = theTime.GetHour();
  if (hour == 6)
    return until_weekday_morning(theTime, theForecastTime);

  if (hour == 12)
    return until_weekday_noon(theTime, theForecastTime);

  if (hour >= 13 && hour <= 16)
    return until_weekday_afternoon(theTime, theForecastTime);

  if (hour >= 18 && hour <= 21)
    return until_weekday_evening(theTime, theForecastTime);

  const string msg =
      "WeekdayTools::until_weekday_time: Cannot generate -aamuun, -keskipaivaan, -iltapaivaan or "
      "-iltaan phrase "
      "for hour " +
      std::to_string(hour);
  throw TextGenError(msg);
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
 * \brief Return the "N-aamusta" or "N-illasta" phrase
 *
 * \param theTime The time
 * \return The phrase
 */
// ----------------------------------------------------------------------

string from_weekday_time(const TextGenPosixTime& theTime)
{
  const int hour = theTime.GetHour();
  if (hour == 6)
    return from_weekday_morning(theTime);
  if (hour == 12)
    return from_weekday_noon(theTime);
  if (hour == 18)
    return from_weekday_evening(theTime);

  const string msg =
      "WeekdayTools::from_weekday_time: Cannot generate -aamusta, -keskipaivasta or -illasta "
      "phrase for hour " +
      std::to_string(hour);
  throw TextGenError(msg);
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
  if (hour == 6)
    return until_weekday_morning(theTime, theHistory);
  if (hour == 18)
    return until_weekday_evening(theTime, theHistory);

  const string msg =
      "WeekdayTools::until_weekday_time: Cannot generate -aamuun or -iltaan phrase for hour " +
      std::to_string(hour);
  throw TextGenError(msg);
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
  if (hour == 6)
    return from_weekday_morning(theTime, theHistory);
  if (hour == 18)
    return from_weekday_evening(theTime, theHistory);

  const string msg =
      "WeekdayTools::from_weekday_time: Cannot generate -aamusta or -illasta phrase for hour " +
      std::to_string(hour);
  throw TextGenError(msg);
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
