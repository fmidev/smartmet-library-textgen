// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::TemperatureStory::mean
 */
// ======================================================================

#include "Integer.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "TemperatureStory.h"
#include "UnitFactory.h"
#include <calculator/GridForecaster.h>
#include <calculator/WeatherResult.h>
#include <calculator/WeatherResultTools.h>
#include <macgyver/Exception.h>

using namespace TextGen;
using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Generate story on mean temperature
 *
 * \return The generated paragraph
 *
 * \todo Is throwing the best way to handle missing results?
 *
 * \see page_temperature_mean
 */
// ----------------------------------------------------------------------

Paragraph TemperatureStory::mean() const
{
  MessageLogger log("TemperatureStory::mean");

  Paragraph paragraph;
  Sentence sentence;

  GridForecaster forecaster;

  WeatherResult result = forecaster.analyze(
      itsVar + "::fake::mean", itsSources, Temperature, Mean, Mean, itsArea, itsPeriod);

  WeatherResultTools::checkMissingValue("temperature_mean", Temperature, result);

  log << "Temperature Mean(Mean) = " << result << '\n';

  sentence << "keskilampotila" << Integer(static_cast<int>(round(result.value())))
           << *UnitFactory::create(DegreesCelsius);

  paragraph << sentence;
  log << paragraph;
  return paragraph;
}

}  // namespace TextGen

// ======================================================================
