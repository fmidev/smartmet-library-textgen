// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::PressureStory::range
 */
// ======================================================================

#include "Integer.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PressureStory.h"
#include "Sentence.h"
#include "UnitFactory.h"
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
 * \brief Generate story on pressure mean
 *
 * \return The story
 *
 * \see page_pressure_range
 */
// ----------------------------------------------------------------------

Paragraph PressureStory::mean() const
{
  MessageLogger log("PressureStory::range");

  using namespace Settings;

  Paragraph paragraph;
  Sentence sentence;

  GridForecaster forecaster;

  WeatherResult meanresult = forecaster.analyze(
      itsVar + "::fake::mean", itsSources, Pressure, Mean, Mean, itsArea, itsPeriod);

  WeatherResultTools::checkMissingValue("pressure_mean", Pressure, meanresult);

  log << "Pressure Mean(Mean(Maximum())) = " << meanresult << endl;

  const int pmean = static_cast<int>(round(meanresult.value()));

  sentence << "paine"
           << "on" << Integer(pmean) << *UnitFactory::create(HectoPascal);

  paragraph << sentence;
  log << paragraph;
  return paragraph;
}

}  // namespace TextGen

// ======================================================================
