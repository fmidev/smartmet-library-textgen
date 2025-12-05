// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::TemperatureStory::range
 */
// ======================================================================

#include "Integer.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "TemperatureStory.h"
#include "TemperatureStoryTools.h"
#include <calculator/GridForecaster.h>
#include <calculator/Settings.h>
#include <calculator/WeatherResult.h>
#include <calculator/WeatherResultTools.h>
#include <macgyver/Exception.h>

using namespace TextGen;
using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Generate story on temperature min/max
 *
 * \return The story
 *
 * \see page_temperature_range
 */
// ----------------------------------------------------------------------

Paragraph TemperatureStory::range() const
{
  MessageLogger log("TemperatureStory::range");

  using namespace Settings;

  const int mininterval = optional_int(itsVar + "::mininterval", 2);
  const bool interval_zero = optional_bool(itsVar + "::always_interval_zero", false);
  const string rangeseparator = optional_string(itsVar + "::rangeseparator", "...");

  Paragraph paragraph;
  Sentence sentence;

  GridForecaster forecaster;

  WeatherResult minresult = forecaster.analyze(
      itsVar + "::fake::minimum", itsSources, Temperature, Mean, Minimum, itsArea, itsPeriod);

  WeatherResult maxresult = forecaster.analyze(
      itsVar + "::fake::maximum", itsSources, Temperature, Mean, Maximum, itsArea, itsPeriod);

  WeatherResult meanresult = forecaster.analyze(
      itsVar + "::fake::mean", itsSources, Temperature, Mean, Mean, itsArea, itsPeriod);

  WeatherResultTools::checkMissingValue(
      "temperature_range", Temperature, {minresult, maxresult, meanresult});

  log << "Temperature Mean(Min(Maximum())) = " << minresult << '\n'
      << "Temperature Mean(Mean(Maximum())) = " << meanresult << '\n'
      << "Temperature Mean(Max(Maximum())) = " << maxresult << '\n';

  const int tmin = static_cast<int>(round(minresult.value()));
  const int tmax = static_cast<int>(round(maxresult.value()));
  const int tmean = static_cast<int>(round(meanresult.value()));

  sentence << "lampotila"
           << "on"
           << TemperatureStoryTools::temperature_sentence(
                  tmin, tmean, tmax, mininterval, interval_zero, rangeseparator);

  paragraph << sentence;
  log << paragraph;
  return paragraph;
}

}  // namespace TextGen

// ======================================================================
