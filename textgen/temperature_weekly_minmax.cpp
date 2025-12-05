// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::TemperatureStory::weekly_minmax
 */
// ======================================================================

#include "Delimiter.h"
#include "Integer.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "TemperatureStory.h"
#include "TemperatureStoryTools.h"
#include "UnitFactory.h"
#include <calculator/GridForecaster.h>
#include <calculator/HourPeriodGenerator.h>
#include <calculator/Settings.h>
#include <calculator/WeatherResult.h>
#include <calculator/WeatherResultTools.h>
#include <macgyver/Exception.h>

using namespace TextGen::TemperatureStoryTools;
using namespace TextGen;
using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Generate story on several day minmax temperatures
 *
 * \return The story
 *
 * \see page_temperature_weekly_minmax
 *
 */
// ----------------------------------------------------------------------

Paragraph TemperatureStory::weekly_minmax() const
{
  MessageLogger log("TemperatureStory::weekly_minmax");

  using namespace Settings;

  Paragraph paragraph;

  const int daymininterval = optional_int(itsVar + "::day::mininterval", 2);
  const int nightmininterval = optional_int(itsVar + "::night::mininterval", 2);
  const bool day_interval_zero = optional_bool(itsVar + "::day::always_interval_zero", false);
  const bool night_interval_zero = optional_bool(itsVar + "::night::always_interval_zero", false);
  const bool emphasize_night_minimum = optional_bool(itsVar + "::emphasize_night_minimum", false);
  const string rangeseparator = optional_string(itsVar + "::rangeseparator", "...");

  const HourPeriodGenerator days(itsPeriod, itsVar + "::day");
  const HourPeriodGenerator nights(itsPeriod, itsVar + "::night");

  GridForecaster forecaster;

  const WeatherResult dayminresult = forecaster.analyze(itsVar + "::fake::day::minimum",
                                                        itsSources,
                                                        Temperature,
                                                        Minimum,
                                                        Mean,
                                                        Maximum,
                                                        itsArea,
                                                        days);

  const WeatherResult daymaxresult = forecaster.analyze(itsVar + "::fake::day::maximum",
                                                        itsSources,
                                                        Temperature,
                                                        Maximum,
                                                        Mean,
                                                        Maximum,
                                                        itsArea,
                                                        days);

  const WeatherResult daymeanresult = forecaster.analyze(
      itsVar + "::fake::day::mean", itsSources, Temperature, Mean, Mean, Maximum, itsArea, days);

  const WeatherResult nightminresult = forecaster.analyze(itsVar + "::fake::night::minimum",
                                                          itsSources,
                                                          Temperature,
                                                          Minimum,
                                                          Mean,
                                                          Minimum,
                                                          itsArea,
                                                          nights);

  const WeatherResult nightmaxresult = forecaster.analyze(itsVar + "::fake::night::maximum",
                                                          itsSources,
                                                          Temperature,
                                                          Maximum,
                                                          Mean,
                                                          Minimum,
                                                          itsArea,
                                                          nights);

  const WeatherResult nightmeanresult = forecaster.analyze(itsVar + "::fake::night::mean",
                                                           itsSources,
                                                           Temperature,
                                                           Mean,
                                                           Mean,
                                                           Minimum,
                                                           itsArea,
                                                           nights);
  WeatherResultTools::checkMissingValue(
      "temperature_weekly_minmax",
      Temperature,
      {dayminresult, daymaxresult, daymeanresult, nightminresult, nightmaxresult, nightmeanresult});

  log << "Temperature Minimum(Mean(Maximum)) = " << dayminresult << '\n';
  log << "Temperature Mean(Mean(Maximum)) = " << daymeanresult << '\n';
  log << "Temperature Maximum(Mean(Maximum)) = " << daymaxresult << '\n';

  log << "Temperature Minimum(Mean(Minimum)) = " << nightminresult << '\n';
  log << "Temperature Mean(Mean(Minimum)) = " << nightmeanresult << '\n';
  log << "Temperature Maximum(Mean(Minimum)) = " << nightmaxresult << '\n';

  const int daymin = static_cast<int>(round(dayminresult.value()));
  const int daymax = static_cast<int>(round(daymaxresult.value()));
  const int daymean = static_cast<int>(round(daymeanresult.value()));
  const int nightmin = static_cast<int>(round(nightminresult.value()));
  const int nightmax = static_cast<int>(round(nightmaxresult.value()));
  const int nightmean = static_cast<int>(round(nightmeanresult.value()));

  Sentence sentence;
  sentence << "paivien ylin lampotila"
           << "on"
           << temperature_sentence(
                  daymin, daymean, daymax, daymininterval, day_interval_zero, rangeseparator)
           << Delimiter(",") << "oiden alin lampotila";
  if (emphasize_night_minimum)
  {
    sentence << "noin" << Integer(nightmin) << *UnitFactory::create(DegreesCelsius);
  }
  else
  {
    sentence << temperature_sentence(
        nightmin, nightmean, nightmax, nightmininterval, night_interval_zero, rangeseparator);
  }

  paragraph << sentence;
  log << paragraph;
  return paragraph;
}

}  // namespace TextGen

// ======================================================================
