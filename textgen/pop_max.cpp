// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::PrecipitationStory::pop_max
 */
// ======================================================================

#include "Delimiter.h"
#include "Integer.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PrecipitationStory.h"
#include "Sentence.h"
#include "UnitFactory.h"
#include <calculator/DefaultAcceptor.h>
#include <calculator/GridForecaster.h>
#include <calculator/MathTools.h>
#include <calculator/Settings.h>
#include <macgyver/Exception.h>
#include <calculator/WeatherResult.h>
#include <calculator/WeatherResultTools.h>

#include <boost/lexical_cast.hpp>

using namespace TextGen;

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Generate story on maximum probability of precipitation
 *
 * \return The generated paragraph
 */
// ----------------------------------------------------------------------

Paragraph PrecipitationStory::pop_max() const
{
  MessageLogger log("PrecipitationStory::pop_max");

  using MathTools::to_precision;

  Paragraph paragraph;

  const int minimum = Settings::optional_percentage(itsVar + "::minimum", 10);
  const int maximum = Settings::optional_percentage(itsVar + "::maximum", 100);
  const int precision = Settings::optional_percentage(itsVar + "::precision", 10);

  GridForecaster forecaster;

  WeatherResult maxresult = forecaster.analyze(itsVar + "::fake::max",
                                               itsSources,
                                               PrecipitationProbability,
                                               Mean,
                                               Maximum,
                                               itsArea,
                                               itsPeriod);

  WeatherResultTools::checkMissingValue("pop_max", PrecipitationProbability, maxresult);

  log << "PoP Mean(Max) " << maxresult << endl;

  const int pop = to_precision(maxresult.value(), precision);

  Sentence sentence;

  if (pop >= minimum && pop <= maximum)
  {
    sentence << "sateen todennakoisyys"
             << "on" << Integer(pop) << *UnitFactory::create(Percent);
  }

  paragraph << sentence;
  log << paragraph;
  return paragraph;
}

}  // namespace TextGen

// ======================================================================
