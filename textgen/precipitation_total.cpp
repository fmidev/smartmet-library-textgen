// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::PrecipitationStory::total
 */
// ======================================================================

#include "Integer.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PrecipitationStory.h"
#include "Sentence.h"
#include "UnitFactory.h"
#include <calculator/GridForecaster.h>
#include <calculator/RangeAcceptor.h>
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
 * \brief Generate story on mean total precipitation
 *
 * Sample story: "Sadesumma 10 millimetria."
 *
 * \return The story
 */
// ----------------------------------------------------------------------

Paragraph PrecipitationStory::total() const
{
  MessageLogger log("PrecipitationStory::total");

  Paragraph paragraph;
  Sentence sentence;

  GridForecaster forecaster;

  RangeAcceptor rainlimits;
  rainlimits.lowerLimit(Settings::optional_double(itsVar + "::minrain", 0));

  WeatherResult result = forecaster.analyze(itsVar + "::fake::mean",
                                            itsSources,
                                            Precipitation,
                                            Mean,
                                            Sum,
                                            itsArea,
                                            itsPeriod,
                                            DefaultAcceptor(),
                                            rainlimits);

  WeatherResultTools::checkMissingValue("precipitation_totsl", Precipitation, result);

  log << "Precipitation Mean(Sum) " << result << '\n';

  sentence << "sadesumma" << Integer(static_cast<int>(round(result.value())))
           << *UnitFactory::create(Millimeters);
  paragraph << sentence;
  log << paragraph;
  return paragraph;
}

}  // namespace TextGen

// ======================================================================
