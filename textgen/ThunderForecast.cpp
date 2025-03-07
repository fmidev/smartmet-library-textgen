// ======================================================================
/*!
 * \file
 * \brief Implementation of ThunderForecast class
 */
// ======================================================================

#include "ThunderForecast.h"

#include "AreaTools.h"
#include "CloudinessStory.h"
#include "CloudinessStoryTools.h"
#include "Delimiter.h"
#include "MessageLogger.h"
#include "NightAndDayPeriodGenerator.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "PrecipitationPeriodTools.h"
#include "PrecipitationStoryTools.h"
#include "SeasonTools.h"
#include "Sentence.h"
#include "SubMaskExtractor.h"
#include "ValueAcceptor.h"
#include "WeatherStory.h"
#include "WeekdayTools.h"
#include <calculator/GridForecaster.h>
#include <calculator/HourPeriodGenerator.h>
#include <calculator/MathTools.h>
#include <calculator/NullPeriodGenerator.h>
#include <calculator/RangeAcceptor.h>
#include <calculator/Settings.h>
#include <macgyver/Exception.h>
#include <calculator/TimeTools.h>
#include <calculator/WeatherPeriodTools.h>
#include <calculator/WeatherResult.h>

#include <newbase/NFmiCombinedParam.h>

#include <boost/lexical_cast.hpp>
#include <map>
#include <vector>

namespace TextGen
{
using namespace Settings;
using namespace TextGen;
using namespace AreaTools;

using namespace std;

ThunderForecast::ThunderForecast(wf_story_params& parameters) : theParameters(parameters) {}
float ThunderForecast::getMaxValue(const WeatherPeriod& theWeatherPeriod,
                                   const weather_result_data_item_vector& theDataVector) const
{
  float maxValue(0.0);
  for (const auto& i : theDataVector)
  {
    if (i->thePeriod.localStartTime() >= theWeatherPeriod.localStartTime() &&
        i->thePeriod.localStartTime() <= theWeatherPeriod.localEndTime() &&
        i->thePeriod.localEndTime() >= theWeatherPeriod.localStartTime() &&
        i->thePeriod.localEndTime() <= theWeatherPeriod.localEndTime())
    {
      if (i->theResult.value() > maxValue)
        maxValue = i->theResult.value();
    }
  }
  return maxValue;
}

Sentence ThunderForecast::thunderSentence(const WeatherPeriod& thePeriod,
                                          const forecast_area_id& theForecastAreaId,
                                          const string& /*theVariable*/) const
{
  Sentence sentence;

  const weather_result_data_item_vector* thunderProbabilityData = nullptr;
  const weather_result_data_item_vector* thunderExtentData = nullptr;

  std::string areaString;

  if (theForecastAreaId == FULL_AREA)
  {
    thunderProbabilityData =
        ((*theParameters.theCompleteData[FULL_AREA])[THUNDER_PROBABILITY_DATA].get());
    thunderExtentData = ((*theParameters.theCompleteData[FULL_AREA])[THUNDER_EXTENT_DATA].get());
    areaString = "full area";
  }
  if (theForecastAreaId == COASTAL_AREA)
  {
    thunderProbabilityData =
        ((*theParameters.theCompleteData[COASTAL_AREA])[THUNDER_PROBABILITY_DATA].get());
    thunderExtentData = ((*theParameters.theCompleteData[COASTAL_AREA])[THUNDER_EXTENT_DATA].get());
    areaString = "coast";
  }
  if (theForecastAreaId == INLAND_AREA)
  {
    thunderProbabilityData =
        ((*theParameters.theCompleteData[INLAND_AREA])[THUNDER_PROBABILITY_DATA].get());
    thunderExtentData = ((*theParameters.theCompleteData[INLAND_AREA])[THUNDER_EXTENT_DATA].get());
    areaString = "inland";
  }

  if (thunderProbabilityData && thunderExtentData)
  {
    float maxThunderProbability(0.0);
    float maxThunderExtent(0.0);

    // 5% in summer,10% other seasons
    //   float thunderExtentLowerLimit = theParameters.theThuderNormalExtentMin;

    maxThunderProbability = getMaxValue(thePeriod, *thunderProbabilityData);
    maxThunderExtent = getMaxValue(thePeriod, *thunderExtentData);

    if (maxThunderProbability > 0.0 || maxThunderExtent > 0.0)
    {
      theParameters.theLog << "Area, period: " << areaString << ", " << thePeriod.localStartTime()
                           << "..." << thePeriod.localEndTime() << endl;
      theParameters.theLog << "Thunder probability (max): " << maxThunderProbability << endl;
      theParameters.theLog << "Thunder extent (max): " << maxThunderExtent << endl;
    }

    if (maxThunderExtent >= theParameters.theThuderNormalExtentMin &&
        maxThunderExtent < theParameters.theThuderNormalExtentMax)
    {
      if (maxThunderProbability >= theParameters.theThunderSmallProbabilityMin &&
          maxThunderProbability < theParameters.theThunderSmallProbabilityMax)
      {
        sentence << Delimiter(",");
        sentence << PAIKOIN_VOI_MYOS_UKKOSTAA_PHRASE;
      }
      else if (maxThunderProbability >= theParameters.theThunderNormalProbabilityMin &&
               maxThunderProbability < theParameters.theThunderNormalProbabilityMax)
      {
        sentence << Delimiter(",");
        sentence << PAIKOIN_MYOS_UKKOSTAA_PHRASE;
      }
      else if (maxThunderProbability >= theParameters.theThunderNormalProbabilityMax)
      {
        sentence << JA_WORD;
        sentence << TODENNAKOISESTI_MYOS_UKKOSTAA_PHRASE;
      }
    }
    else if (maxThunderExtent >= theParameters.theThuderNormalExtentMax)
    {
      if (maxThunderProbability >= theParameters.theThunderSmallProbabilityMin &&
          maxThunderProbability < theParameters.theThunderSmallProbabilityMax)
      {
        sentence << Delimiter(",");
        sentence << MAHDOLLISESTI_MYOS_UKKOSTAA_PHRASE;
      }
      else if (maxThunderProbability >= theParameters.theThunderNormalProbabilityMin &&
               maxThunderProbability < theParameters.theThunderNormalProbabilityMax)
      {
        sentence << Delimiter(",");
        sentence << MYOS_UKKOSTA_ESIINTYY_PHRASE;
      }
      else if (maxThunderProbability >= theParameters.theThunderNormalProbabilityMax)
      {
        sentence << JA_WORD;
        sentence << TODENNAKOISESTI_MYOS_UKKOSTAA_PHRASE;
      }
    }
  }

  return sentence;
}

Sentence ThunderForecast::areaSpecificSentence(const WeatherPeriod& thePeriod) const
{
  Sentence sentence;

  WeatherResult northEastShare(kFloatMissing, 0);
  WeatherResult southEastShare(kFloatMissing, 0);
  WeatherResult southWestShare(kFloatMissing, 0);
  WeatherResult northWestShare(kFloatMissing, 0);

  RangeAcceptor thunderlimits;
  thunderlimits.lowerLimit(SMALL_PROBABILITY_FOR_THUNDER_LOWER_LIMIT);
  AreaTools::getArealDistribution(theParameters.theSources,
                                  Thunder,
                                  theParameters.theArea,
                                  thePeriod,
                                  thunderlimits,
                                  northEastShare,
                                  southEastShare,
                                  southWestShare,
                                  northWestShare);

  float north = northEastShare.value() + northWestShare.value();
  float south = southEastShare.value() + southWestShare.value();
  float east = northEastShare.value() + southEastShare.value();
  float west = northWestShare.value() + southWestShare.value();

  sentence << area_specific_sentence(north,
                                     south,
                                     east,
                                     west,
                                     northEastShare.value(),
                                     southEastShare.value(),
                                     southWestShare.value(),
                                     northWestShare.value(),
                                     false);

  return sentence;
}
}  // namespace TextGen
