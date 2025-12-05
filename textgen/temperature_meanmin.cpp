// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::TemperatureStory::meanmin
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
 * \brief Generate story on mean minimum temperature
 *
 * Throws if the weather period does not evenly divide into 24 hour
 * segments.
 *
 * \return The story
 *
 * \see page_temperature_meanmin
 */
// ----------------------------------------------------------------------

Paragraph TemperatureStory::meanmin() const
{
  MessageLogger log("TemperatureStory::meanmin");

  Paragraph paragraph;
  Sentence sentence;

  GridForecaster forecaster;

  HourPeriodGenerator periods(itsPeriod, 18, 06, 18, 06);

  WeatherResult result = forecaster.analyze(
      itsVar + "::fake::mean", itsSources, Temperature, Mean, Mean, Minimum, itsArea, periods);

  WeatherResultTools::checkMissingValue("temperature_meanmin", Temperature, result);

  log << "Temperature Mean(Mean(Minimum())) = " << result << endl;

  sentence << "keskimaarainen alin lampotila" << Integer(static_cast<int>(round(result.value())))
           << *UnitFactory::create(DegreesCelsius);
  paragraph << sentence;
  log << paragraph;
  return paragraph;
}

}  // namespace TextGen

// ======================================================================
