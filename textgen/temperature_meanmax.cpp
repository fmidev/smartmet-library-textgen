// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::TemperatureStory::meanmax
 */
// ======================================================================

#include "Integer.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "TemperatureStory.h"
#include "UnitFactory.h"
#include <calculator/GridForecaster.h>
#include <calculator/HourPeriodGenerator.h>
#include <calculator/WeatherResult.h>
#include <calculator/WeatherResultTools.h>
#include <macgyver/Exception.h>

using namespace TextGen;
using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Generate story on mean maximum temperature
 *
 * \return The story
 *
 * \see page_temperature_meanmax
 */
// ----------------------------------------------------------------------

Paragraph TemperatureStory::meanmax() const
{
  MessageLogger log("TemperatureStory::meanmax");

  Paragraph paragraph;
  Sentence sentence;

  GridForecaster forecaster;

  HourPeriodGenerator periods(itsPeriod, 06, 18, 06, 18);

  WeatherResult result = forecaster.analyze(
      itsVar + "::fake::mean", itsSources, Temperature, Mean, Mean, Maximum, itsArea, periods);

  WeatherResultTools::checkMissingValue("temperature_meanmax", Temperature, result);

  log << "Temperature Mean(Mean(Maximum())) = " << result << endl;

  sentence << "keskimaarainen ylin lampotila" << Integer(static_cast<int>(round(result.value())))
           << *UnitFactory::create(DegreesCelsius);
  paragraph << sentence;
  log << paragraph;
  return paragraph;
}

}  // namespace TextGen

// ======================================================================
