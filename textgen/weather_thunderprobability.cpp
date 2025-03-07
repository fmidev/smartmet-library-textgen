// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WeatherStory::thunderprobability
 */
// ======================================================================

#include "Integer.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "UnitFactory.h"
#include "WeatherStory.h"
#include <calculator/GridForecaster.h>
#include <calculator/MathTools.h>
#include <calculator/Settings.h>
#include <macgyver/Exception.h>
#include <calculator/WeatherPeriodTools.h>
#include <calculator/WeatherResult.h>
#include <calculator/WeatherResultTools.h>

using namespace TextGen;
using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Generate story on thunder probability
 *
 * \return The story
 *
 * \see page_weather_thunderprobability
 */
// ----------------------------------------------------------------------

Paragraph WeatherStory::thunderprobability() const
{
  MessageLogger log("WeatherStory::thunderprobability");

  using namespace Settings;
  using namespace WeatherPeriodTools;

  Paragraph paragraph;

  const int precision = optional_percentage(itsVar + "::precision", 10);
  const int limit = optional_percentage(itsVar + "::limit", 10);

  GridForecaster forecaster;

  WeatherResult result = forecaster.analyze(
      itsVar + "::fake::probability", itsSources, Thunder, Maximum, Maximum, itsArea, itsPeriod);

  WeatherResultTools::checkMissingValue("weather_thunderprobability", Thunder, result);

  log << "Thunder Maximum(Maximum) = " << result << endl;

  const int probability = MathTools::to_precision(result.value(), precision);

  if (probability >= limit)
  {
    Sentence sentence;
    sentence << "ukkosen todennakoisyys"
             << "on" << Integer(probability) << *UnitFactory::create(Percent);
    paragraph << sentence;
  }

  log << paragraph;
  return paragraph;
}

}  // namespace TextGen

// ======================================================================
