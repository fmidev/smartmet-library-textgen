// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::PrecipitationStory::total_day
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
#include <calculator/TextGenError.h>
#include <calculator/WeatherResult.h>
#include <calculator/WeatherResultTools.h>

using namespace TextGen;
using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Generate story on mean total precipitation
 *
 * Sample story: "Sadesumma 10 millimetria."
 * Sample story: "Sadesumma alle 1 millimetri."
 *
 * \return The story
 */
// ----------------------------------------------------------------------

Paragraph PrecipitationStory::total_day() const
{
  MessageLogger log("PrecipitationStory::total_day");

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

  WeatherResultTools::checkMissingValue("precipitation_total_day", Precipitation, result);

  log << "Precipitation Mean(Sum) " << result << endl;

  int rain = static_cast<int>(round(result.value()));

  if (rain == 0 && result.value() > 0)
    sentence << "sadesumma"
             << "alle"
             << "yksi millimetri";
  else
    sentence << "sadesumma" << Integer(static_cast<int>(round(result.value())))
             << *UnitFactory::create(Millimeters);
  paragraph << sentence;
  log << paragraph;
  return paragraph;
}

}  // namespace TextGen

// ======================================================================
