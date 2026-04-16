// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WindStory::daily_ranges
 */
// ======================================================================

#include "Delimiter.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "Sentence.h"
#include "WeekdayTools.h"
#include "WindStory.h"
#include "WindStoryTools.h"
#include <calculator/GridForecaster.h>
#include <calculator/HourPeriodGenerator.h>
#include <calculator/Settings.h>
#include <calculator/WeatherResult.h>
#include <calculator/WeatherResultTools.h>
#include <macgyver/Exception.h>

#include <boost/lexical_cast.hpp>

#include <vector>

using namespace TextGen::WindStoryTools;
using namespace TextGen;
using namespace std;

namespace
{

using namespace TextGen::WindStoryTools;
using namespace TextGen;
using WeatherResultTools::isSimilarRange;
using WeatherResultTools::max;
using WeatherResultTools::mean;
using WeatherResultTools::min;

struct DailyWindData
{
  vector<WeatherPeriod> periods;
  vector<WeatherResult> meanspeeds;
  vector<WeatherResult> maxspeeds;
  vector<WeatherResult> minspeeds;
  vector<WeatherResult> directions;
  vector<WindDirectionAccuracy> accuracies;
};

Sentence daily_ranges_1day(const DailyWindData& d,
                           const string& var,
                           const TextGenPosixTime& forecastTime)
{
  Sentence sentence;
  sentence << PeriodPhraseFactory::create("today", var, forecastTime, d.periods[0])
           << directed_speed_sentence(
                  d.minspeeds[0], d.maxspeeds[0], d.meanspeeds[0], d.directions[0], var);
  return sentence;
}

Sentence daily_ranges_2days_good_accuracy(const DailyWindData& d,
                                          const string& var,
                                          const TextGenPosixTime& forecastTime,
                                          const WeatherResult& direction12,
                                          bool similar_speeds,
                                          const string& opt)
{
  Sentence sentence;
  if (similar_speeds)
  {
    WeatherPeriod days12(d.periods[0].localStartTime(), d.periods[1].localEndTime());
    sentence << PeriodPhraseFactory::create("days", var, forecastTime, days12)
             << directed_speed_sentence(min(d.minspeeds[0], d.minspeeds[1]),
                                        max(d.maxspeeds[0], d.maxspeeds[1]),
                                        mean(d.meanspeeds[0], d.meanspeeds[1]),
                                        direction12,
                                        var);
    return sentence;
  }

  if (opt == "textphrase")
  {
    const std::string speed_str(speed_string(d.meanspeeds[0]));
    Sentence todaySentence;
    if (!speed_str.empty())
      todaySentence << PeriodPhraseFactory::create("today", var, forecastTime, d.periods[0])
                    << speed_str;
    Sentence nextdaySpeedSentence;
    nextdaySpeedSentence << directed_speed_sentence(
        d.minspeeds[1], d.maxspeeds[1], d.meanspeeds[1], direction12, var);
    Sentence nextdaySentence;
    if (!nextdaySpeedSentence.empty())
      nextdaySentence << PeriodPhraseFactory::create("next_day", var, forecastTime, d.periods[1])
                      << nextdaySpeedSentence;
    if (!todaySentence.empty() && !nextdaySentence.empty())
      sentence << todaySentence << Delimiter(",") << nextdaySentence;
    else if (!nextdaySentence.empty())
      sentence << nextdaySentence;
  }
  else
  {
    sentence << PeriodPhraseFactory::create("today", var, forecastTime, d.periods[0])
             << directed_speed_sentence(
                    d.minspeeds[0], d.maxspeeds[0], d.meanspeeds[0], direction12, var)
             << Delimiter(",")
             << PeriodPhraseFactory::create("next_day", var, forecastTime, d.periods[1])
             << speed_range_sentence(d.minspeeds[1], d.maxspeeds[1], d.meanspeeds[1], var);
  }
  return sentence;
}

Sentence daily_ranges_2days_bad_accuracy(const DailyWindData& d,
                                         const string& var,
                                         const TextGenPosixTime& forecastTime,
                                         bool similar_speeds)
{
  Sentence sentence;
  Sentence todaySpeedSentence;
  todaySpeedSentence << directed_speed_sentence(
      d.minspeeds[0], d.maxspeeds[0], d.meanspeeds[0], d.directions[0], var);
  if (!todaySpeedSentence.empty())
    sentence << PeriodPhraseFactory::create("today", var, forecastTime, d.periods[0])
             << todaySpeedSentence << Delimiter(",")
             << PeriodPhraseFactory::create("next_day", var, forecastTime, d.periods[1]);
  else
    sentence << PeriodPhraseFactory::create("next_day", var, forecastTime, d.periods[1]);

  if (similar_speeds)
    sentence << direction_sentence(d.directions[1], var);
  else
    sentence << directed_speed_sentence(
        d.minspeeds[1], d.maxspeeds[1], d.meanspeeds[1], d.directions[1], var);
  return sentence;
}

Sentence daily_ranges_2days(const DailyWindData& d,
                            const string& var,
                            const TextGenPosixTime& forecastTime,
                            GridForecaster& forecaster,
                            const AnalysisSources& sources,
                            const WeatherArea& area,
                            const HourPeriodGenerator& periodgenerator)
{
  const WeatherResult direction12 = forecaster.analyze(var + "::fake::days1-2::direction::mean",
                                                       sources,
                                                       WindDirection,
                                                       Mean,
                                                       Mean,
                                                       Mean,
                                                       area,
                                                       periodgenerator);
  WeatherResultTools::checkMissingValue("wind_daily_ranges", WindDirection, direction12);
  const WindDirectionAccuracy accuracy12 = direction_accuracy(direction12.error(), var);

  bool similar_speeds =
      isSimilarRange(d.minspeeds[0], d.maxspeeds[0], d.minspeeds[1], d.maxspeeds[1], var);
  const string opt = Settings::optional_string("textgen::units::meterspersecond::format", "SI");
  if (opt == "textphrase")
    similar_speeds = (speed_string(d.meanspeeds[0]) == speed_string(d.meanspeeds[1]));

  bool goodAccuracy = (accuracy12 != bad_accuracy ||
                       (d.accuracies[0] == bad_accuracy && d.accuracies[1] == bad_accuracy));
  if (goodAccuracy)
    return daily_ranges_2days_good_accuracy(d, var, forecastTime, direction12, similar_speeds, opt);
  else
    return daily_ranges_2days_bad_accuracy(d, var, forecastTime, similar_speeds);
}

void append_3days_day1(Sentence& sentence,
                       const DailyWindData& d,
                       const string& var,
                       const TextGenPosixTime& forecastTime,
                       const WeatherPeriod& days123,
                       const WeatherResult& dirCommon,
                       bool similar12,
                       bool similar123)
{
  if (!similar12)
    sentence << PeriodPhraseFactory::create("today", var, forecastTime, d.periods[0])
             << directed_speed_sentence(
                    d.minspeeds[0], d.maxspeeds[0], d.meanspeeds[0], dirCommon, var);
  else if (!similar123)
    sentence << directed_speed_sentence(min(d.minspeeds[0], d.minspeeds[1]),
                                        max(d.maxspeeds[0], d.maxspeeds[1]),
                                        mean(d.meanspeeds[0], d.meanspeeds[1]),
                                        dirCommon,
                                        var);
  else
    sentence << PeriodPhraseFactory::create("days", var, forecastTime, days123)
             << directed_speed_sentence(min(d.minspeeds[0], d.minspeeds[1], d.minspeeds[2]),
                                        max(d.maxspeeds[0], d.maxspeeds[1], d.maxspeeds[2]),
                                        mean(d.meanspeeds[0], d.meanspeeds[1], d.maxspeeds[2]),
                                        dirCommon,
                                        var);
}

Sentence daily_ranges_3days_acc123(const DailyWindData& d,
                                   const string& var,
                                   const TextGenPosixTime& forecastTime,
                                   const WeatherPeriod& days123,
                                   const WeatherResult& direction123,
                                   bool similar12,
                                   bool similar23,
                                   bool similar123flag)
{
  Sentence sentence;
  append_3days_day1(
      sentence, d, var, forecastTime, days123, direction123, similar12, similar123flag);

  if (!similar123flag)
  {
    if (!similar12)
    {
      sentence << Delimiter(",");
      if (similar23)
        sentence << PeriodPhraseFactory::create("next_days", var, forecastTime, d.periods[1])
                 << speed_range_sentence(min(d.minspeeds[1], d.minspeeds[2]),
                                         max(d.maxspeeds[1], d.maxspeeds[2]),
                                         mean(d.meanspeeds[1], d.meanspeeds[2]),
                                         var);
      else
        sentence << PeriodPhraseFactory::create("next_day", var, forecastTime, d.periods[1])
                 << speed_range_sentence(d.minspeeds[1], d.maxspeeds[1], d.meanspeeds[1], var);
    }
    if (!similar23)
      sentence << Delimiter(",")
               << PeriodPhraseFactory::create("next_day", var, forecastTime, d.periods[2])
               << speed_range_sentence(d.minspeeds[2], d.maxspeeds[2], d.meanspeeds[2], var);
  }
  return sentence;
}

Sentence daily_ranges_3days_acc12(const DailyWindData& d,
                                  const string& var,
                                  const TextGenPosixTime& forecastTime,
                                  const WeatherPeriod& days123,
                                  const WeatherResult& direction12,
                                  bool similar12,
                                  bool similar23,
                                  bool similar123flag)
{
  using WeekdayTools::on_weekday;
  Sentence sentence;
  append_3days_day1(
      sentence, d, var, forecastTime, days123, direction12, similar12, similar123flag);

  if (!similar12)
  {
    sentence << Delimiter(",")
             << PeriodPhraseFactory::create("next_day", var, forecastTime, d.periods[1]);
    if (!similar23)
      sentence << speed_range_sentence(d.minspeeds[1], d.maxspeeds[1], d.meanspeeds[1], var);
    else
      sentence << speed_range_sentence(min(d.minspeeds[1], d.minspeeds[2]),
                                       max(d.maxspeeds[1], d.maxspeeds[2]),
                                       mean(d.meanspeeds[1], d.meanspeeds[2]),
                                       var);
  }

  sentence << Delimiter(",");
  if (!similar12)
    sentence << on_weekday(d.periods[2].localStartTime());
  else
    sentence << PeriodPhraseFactory::create("next_day", var, forecastTime, d.periods[2]);
  if (similar23)
    sentence << direction_sentence(d.directions[2], var);
  else
    sentence << directed_speed_sentence(
        d.minspeeds[2], d.maxspeeds[2], d.meanspeeds[2], d.directions[2], var);
  return sentence;
}

Sentence daily_ranges_3days_acc23(const DailyWindData& d,
                                  const string& var,
                                  const TextGenPosixTime& forecastTime,
                                  const WeatherResult& direction23,
                                  bool similar12,
                                  bool similar23,
                                  bool similar123flag)
{
  Sentence sentence;
  sentence << PeriodPhraseFactory::create("today", var, forecastTime, d.periods[0]);
  if (!similar12)
    sentence << directed_speed_sentence(
        d.minspeeds[0], d.maxspeeds[0], d.meanspeeds[0], d.directions[0], var);
  else if (!similar123flag)
    sentence << directed_speed_sentence(min(d.minspeeds[0], d.minspeeds[1]),
                                        max(d.maxspeeds[0], d.maxspeeds[1]),
                                        mean(d.meanspeeds[0], d.meanspeeds[1]),
                                        d.directions[0],
                                        var);
  else
    sentence << directed_speed_sentence(min(d.minspeeds[0], d.minspeeds[1], d.minspeeds[2]),
                                        max(d.maxspeeds[0], d.maxspeeds[1], d.maxspeeds[2]),
                                        mean(d.meanspeeds[0], d.meanspeeds[1], d.maxspeeds[2]),
                                        d.directions[0],
                                        var);

  sentence << Delimiter(",");
  if (similar23)
    sentence << PeriodPhraseFactory::create("next_days", var, forecastTime, d.periods[1]);
  else
    sentence << PeriodPhraseFactory::create("next_day", var, forecastTime, d.periods[1]);
  sentence << direction_sentence(direction23, var);

  if (!similar12)
  {
    if (similar23)
      sentence << speed_range_sentence(min(d.minspeeds[1], d.minspeeds[2]),
                                       max(d.maxspeeds[1], d.maxspeeds[2]),
                                       mean(d.meanspeeds[1], d.meanspeeds[2]),
                                       var);
    else
      sentence << speed_range_sentence(d.minspeeds[1], d.maxspeeds[1], d.meanspeeds[1], var);
  }

  if (!similar23)
    sentence << Delimiter(",")
             << PeriodPhraseFactory::create("next_day", var, forecastTime, d.periods[2])
             << speed_range_sentence(d.minspeeds[2], d.maxspeeds[2], d.meanspeeds[2], var);
  return sentence;
}

Sentence daily_ranges_3days_no_acc(const DailyWindData& d,
                                   const string& var,
                                   const TextGenPosixTime& forecastTime,
                                   bool similar12,
                                   bool similar23,
                                   bool similar123flag)
{
  Sentence sentence;
  sentence << PeriodPhraseFactory::create("today", var, forecastTime, d.periods[0])
           << direction_sentence(d.directions[0], var);

  if (similar123flag)
    sentence << speed_range_sentence(min(d.minspeeds[0], d.minspeeds[1], d.minspeeds[2]),
                                     max(d.maxspeeds[0], d.maxspeeds[1], d.maxspeeds[2]),
                                     mean(d.meanspeeds[0], d.meanspeeds[1], d.meanspeeds[2]),
                                     var);
  else if (similar12)
    sentence << speed_range_sentence(min(d.minspeeds[0], d.minspeeds[1]),
                                     max(d.maxspeeds[0], d.maxspeeds[1]),
                                     mean(d.meanspeeds[0], d.meanspeeds[1]),
                                     var);
  else
    sentence << speed_range_sentence(d.minspeeds[0], d.maxspeeds[0], d.meanspeeds[0], var);

  sentence << Delimiter(",")
           << PeriodPhraseFactory::create("next_day", var, forecastTime, d.periods[1])
           << direction_sentence(d.directions[1], var);

  if (!similar12)
  {
    if (similar23)
      sentence << speed_range_sentence(min(d.minspeeds[1], d.minspeeds[2]),
                                       max(d.maxspeeds[1], d.maxspeeds[2]),
                                       mean(d.meanspeeds[1], d.meanspeeds[2]),
                                       var);
    else
      sentence << speed_range_sentence(d.minspeeds[1], d.maxspeeds[1], d.meanspeeds[1], var);
  }

  sentence << Delimiter(",")
           << PeriodPhraseFactory::create("next_day", var, forecastTime, d.periods[2])
           << direction_sentence(d.directions[2], var);
  if (!similar23)
    sentence << speed_range_sentence(d.minspeeds[2], d.maxspeeds[2], d.meanspeeds[2], var);
  return sentence;
}

Sentence daily_ranges_3days(const DailyWindData& d,
                            const string& var,
                            const TextGenPosixTime& forecastTime,
                            GridForecaster& forecaster,
                            const AnalysisSources& sources,
                            const WeatherArea& area)
{
  const WeatherPeriod days123(d.periods[0].localStartTime(), d.periods[2].localEndTime());
  const WeatherPeriod days12(d.periods[0].localStartTime(), d.periods[1].localEndTime());
  const WeatherPeriod days23(d.periods[1].localStartTime(), d.periods[2].localEndTime());

  const WeatherResult direction123 =
      forecaster.analyze(var + "::fake::days1-3::direction::mean",
                         sources,
                         WindDirection,
                         Mean,
                         Mean,
                         Mean,
                         area,
                         HourPeriodGenerator(days123, var + "::day"));
  const WeatherResult direction12 = forecaster.analyze(var + "::fake::days1-2::direction::mean",
                                                       sources,
                                                       WindDirection,
                                                       Mean,
                                                       Mean,
                                                       Mean,
                                                       area,
                                                       HourPeriodGenerator(days12, var + "::day"));
  const WeatherResult direction23 = forecaster.analyze(var + "::fake::days2-3::direction::mean",
                                                       sources,
                                                       WindDirection,
                                                       Mean,
                                                       Mean,
                                                       Mean,
                                                       area,
                                                       HourPeriodGenerator(days23, var + "::day"));

  WeatherResultTools::checkMissingValue("wind_daily_ranges", WindDirection, direction123);
  WeatherResultTools::checkMissingValue("wind_daily_ranges", WindDirection, direction12);
  WeatherResultTools::checkMissingValue("wind_daily_ranges", WindDirection, direction23);

  const WindDirectionAccuracy accuracy123 = direction_accuracy(direction123.error(), var);
  const WindDirectionAccuracy accuracy12 = direction_accuracy(direction12.error(), var);
  const WindDirectionAccuracy accuracy23 = direction_accuracy(direction23.error(), var);

  const bool similar12 =
      isSimilarRange(d.minspeeds[0], d.maxspeeds[0], d.minspeeds[1], d.maxspeeds[1], var);
  const bool similar23 =
      isSimilarRange(d.minspeeds[1], d.maxspeeds[1], d.minspeeds[2], d.maxspeeds[2], var);
  const bool similar13 =
      isSimilarRange(d.minspeeds[0], d.maxspeeds[0], d.minspeeds[2], d.maxspeeds[2], var);
  const bool similar123 = (similar12 && similar23 && similar13);

  bool acc123 = accuracy123 != bad_accuracy ||
                (d.accuracies[0] == bad_accuracy && d.accuracies[1] == bad_accuracy &&
                 d.accuracies[2] == bad_accuracy);
  bool acc12 = accuracy12 != bad_accuracy ||
               (d.accuracies[0] == bad_accuracy && d.accuracies[1] == bad_accuracy);
  bool acc23 = accuracy23 != bad_accuracy ||
               (d.accuracies[1] == bad_accuracy && d.accuracies[2] == bad_accuracy);

  if (acc123)
    return daily_ranges_3days_acc123(
        d, var, forecastTime, days123, direction123, similar12, similar23, similar123);
  if (acc12)
    return daily_ranges_3days_acc12(
        d, var, forecastTime, days123, direction12, similar12, similar23, similar123);
  if (acc23)
    return daily_ranges_3days_acc23(
        d, var, forecastTime, direction23, similar12, similar23, similar123);
  return daily_ranges_3days_no_acc(d, var, forecastTime, similar12, similar23, similar123);
}

}  // namespace

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Return wind_daily_ranges story
 *
 * \return Paragraph containing the story
 */
// ----------------------------------------------------------------------

Paragraph WindStory::daily_ranges() const
{
  try
  {
    MessageLogger log("WindStory::daily_ranges");

    // Generate the story

    Paragraph paragraph;

    GridForecaster forecaster;

    // All day periods

    const HourPeriodGenerator periodgenerator(itsPeriod, itsVar + "::day");
    const int ndays = periodgenerator.size();

    log << "Period covers " << ndays << " days\n";

    if (ndays <= 0)
    {
      log << paragraph;
      return paragraph;
    }

    // Calculate wind speeds for max 3 days

    DailyWindData d;

    for (int day = 1; day <= std::min(ndays, 3); day++)
    {
      const WeatherPeriod period(periodgenerator.period(day));
      const string daystr = "day" + std::to_string(day);

      const WeatherResult minspeed =
          forecaster.analyze(itsVar + "::fake::" + daystr + "::speed::minimum",
                             itsSources,
                             WindSpeed,
                             Minimum,
                             Mean,
                             itsArea,
                             period);
      const WeatherResult maxspeed =
          forecaster.analyze(itsVar + "::fake::" + daystr + "::speed::maximum",
                             itsSources,
                             WindSpeed,
                             Maximum,
                             Mean,
                             itsArea,
                             period);
      const WeatherResult meanspeed =
          forecaster.analyze(itsVar + "::fake::" + daystr + "::speed::mean",
                             itsSources,
                             WindSpeed,
                             Mean,
                             Mean,
                             itsArea,
                             period);
      const WeatherResult direction =
          forecaster.analyze(itsVar + "::fake::" + daystr + "::direction::mean",
                             itsSources,
                             WindDirection,
                             Mean,
                             Mean,
                             itsArea,
                             period);

      log << "WindSpeed Minimum(Mean) " << daystr << " = " << minspeed << '\n';
      log << "WindSpeed Maximum(Mean) " << daystr << " = " << maxspeed << '\n';
      log << "WindSpeed Mean(Mean) " << daystr << " = " << meanspeed << '\n';

      WeatherResultTools::checkMissingValue("wind_daily_ranges", WindSpeed, maxspeed);
      WeatherResultTools::checkMissingValue("wind_daily_ranges", WindSpeed, minspeed);
      WeatherResultTools::checkMissingValue("wind_daily_ranges", WindSpeed, meanspeed);
      WeatherResultTools::checkMissingValue("wind_daily_ranges", WindDirection, direction);

      d.periods.push_back(period);
      d.minspeeds.push_back(minspeed);
      d.maxspeeds.push_back(maxspeed);
      d.meanspeeds.push_back(meanspeed);
      d.directions.push_back(direction);
      d.accuracies.push_back(direction_accuracy(direction.error(), itsVar));
    }

    switch (ndays)
    {
      case 1:
        paragraph << daily_ranges_1day(d, itsVar, itsForecastTime);
        break;
      case 2:
        paragraph << daily_ranges_2days(
            d, itsVar, itsForecastTime, forecaster, itsSources, itsArea, periodgenerator);
        break;
      default:
        paragraph << daily_ranges_3days(
            d, itsVar, itsForecastTime, forecaster, itsSources, itsArea);
        break;
    }

    log << paragraph;
    return paragraph;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

}  // namespace TextGen

// ======================================================================
