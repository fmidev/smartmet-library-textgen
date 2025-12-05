// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WaveStory::range
 */
// ======================================================================

#include "Delimiter.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Real.h"
#include "RealRange.h"
#include "Sentence.h"
#include "UnitFactory.h"
#include "WaveStory.h"
#include <calculator/GridForecaster.h>
#include <calculator/MathTools.h>
#include <calculator/Settings.h>
#include <calculator/WeatherResult.h>
#include <calculator/WeatherResultTools.h>
#include <macgyver/Exception.h>

#include <boost/lexical_cast.hpp>

#include <vector>

using namespace TextGen;

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Return wave_ranges story
 *
 * \return Paragraph containing the story
 */
// ----------------------------------------------------------------------

Paragraph WaveStory::range() const
{
  MessageLogger log("WaveStory::range");

  // Establish options

  using namespace Settings;

  // Generate the story

  Paragraph paragraph;

  GridForecaster forecaster;

  // Calculate wave speeds

  const WeatherResult minresult = forecaster.analyze(itsVar + "::fake::height::minimum",
                                                     itsSources,
                                                     WaveHeight,
                                                     Minimum,
                                                     Maximum,
                                                     itsArea,
                                                     itsPeriod);

  const WeatherResult maxresult = forecaster.analyze(itsVar + "::fake::height::maximum",
                                                     itsSources,
                                                     WaveHeight,
                                                     Maximum,
                                                     Maximum,
                                                     itsArea,
                                                     itsPeriod);

  const WeatherResult meanresult = forecaster.analyze(
      itsVar + "::fake::height::mean", itsSources, WaveHeight, Mean, Maximum, itsArea, itsPeriod);

  WeatherResultTools::checkMissingValue(
      "wave_range", WaveHeight, {minresult, maxresult, meanresult});

  log << "WaveHeight Minimum(Mean)  = " << minresult << '\n';
  log << "WaveHeight Maximum(Mean)  = " << maxresult << '\n';
  log << "WaveHeight Mean(Mean)     = " << meanresult << '\n';

  const float mininterval = Settings::optional_double(itsVar + "::mininterval", 0.5);
  const string rangeseparator = Settings::optional_string(itsVar + "::rangeseparator", "-");

  // 0.5 meter accuracy is sufficient according to Niko Tollman
  float n = round(2 * meanresult.value()) / 2;
  float x = round(2 * minresult.value()) / 2;
  float y = round(2 * maxresult.value()) / 2;

  Sentence sentence;

  if (y <= 0.5)
  {
    sentence << "aallonkorkeus on alle [N] [metria]" << Real(0.5) << *UnitFactory::create(Meters);
  }
  else if (y - x < mininterval)
  {
    sentence << "aallonkorkeus on noin [N] [metria]" << Real(n) << *UnitFactory::create(Meters);
  }
  else if (x == 0)
  {
    sentence << "aallonkorkeus on enimmillaan [N] [metria]" << Real(y)
             << *UnitFactory::create(Meters);
  }
  else
  {
    sentence << "aallonkorkeus on [N...M] [metria]" << RealRange(x, y)
             << *UnitFactory::create(Meters);
  }

  paragraph << sentence;

  log << paragraph;
  return paragraph;
}

}  // namespace TextGen

// ======================================================================
