// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WindStory::daily_ranges
 */
// ======================================================================

#include "Delimiter.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "WindStory.h"
#include "WindStoryTools.h"
#include <calculator/GridForecaster.h>
#include <calculator/Settings.h>
#include <macgyver/Exception.h>
#include <calculator/WeatherResult.h>
#include <calculator/WeatherResultTools.h>

#include <boost/lexical_cast.hpp>

#include <vector>

using namespace TextGen;

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Return wind_ranges story
 *
 * \return Paragraph containing the story
 */
// ----------------------------------------------------------------------

Paragraph WindStory::range() const
{
  MessageLogger log("WindStory::range");

  // Establish options

  using namespace Settings;

  // Generate the story

  Paragraph paragraph;

  GridForecaster forecaster;

  // Calculate wind speeds

  const WeatherResult minresult = forecaster.analyze(
      itsVar + "::fake::speed::minimum", itsSources, WindSpeed, Mean, Minimum, itsArea, itsPeriod);

  const WeatherResult maxresult = forecaster.analyze(
      itsVar + "::fake::speed::maximum", itsSources, WindSpeed, Mean, Maximum, itsArea, itsPeriod);

  const WeatherResult meanresult = forecaster.analyze(
      itsVar + "::fake::speed::mean", itsSources, WindSpeed, Mean, Mean, itsArea, itsPeriod);

  const WeatherResult dirresult = forecaster.analyze(itsVar + "::fake::direction::mean",
                                                     itsSources,
                                                     WindDirection,
                                                     Mean,
                                                     Mean,
                                                     itsArea,
                                                     itsPeriod);

  log << "WindSpeed Minimum(Mean)  = " << minresult << endl;
  log << "WindSpeed Maximum(Mean)  = " << maxresult << endl;
  log << "WindSpeed Mean(Mean)     = " << meanresult << endl;

  WeatherResultTools::checkMissingValue("wind_simple_overview", WindSpeed, maxresult);
  WeatherResultTools::checkMissingValue("wind_simple_overview", WindSpeed, minresult);
  WeatherResultTools::checkMissingValue("wind_simple_overview", WindSpeed, meanresult);
  WeatherResultTools::checkMissingValue("wind_simple_overview", WindDirection, dirresult);

  Sentence sentence;
  sentence << WindStoryTools::directed_speed_sentence(
      minresult, maxresult, meanresult, dirresult, itsVar);
  paragraph << sentence;

  log << paragraph;
  return paragraph;
}

}  // namespace TextGen

// ======================================================================
