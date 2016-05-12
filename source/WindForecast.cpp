// ======================================================================
/*!
 * \file
 * \brief Implementation of WindForecast class
 */
// ======================================================================

#include "WeatherStory.h"
#include "CloudinessStory.h"
#include "CloudinessStoryTools.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "HourPeriodGenerator.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "NightAndDayPeriodGenerator.h"
#include "PeriodPhraseFactory.h"
#include "RangeAcceptor.h"
#include "ValueAcceptor.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "TimeTools.h"
#include "WeatherResult.h"
#include "WeekdayTools.h"
#include "NullPeriodGenerator.h"
#include "WeatherPeriodTools.h"
#include "AreaTools.h"
#include "MathTools.h"
#include "SeasonTools.h"
#include "SubMaskExtractor.h"
#include "IntegerRange.h"
#include "UnitFactory.h"
#include "WindForecast.h"
#include "DebugTextFormatter.h"

#include <newbase/NFmiCombinedParam.h>
#include <newbase/NFmiMercatorArea.h>

#include <boost/lexical_cast.hpp>
#include <vector>
#include <map>
#include <iomanip>
#include <cmath>

using namespace Settings;
using namespace TextGen;
using namespace AreaTools;
using namespace WindStoryTools;
using namespace boost;
using namespace std;

namespace TextGen
{
namespace WindForecastPhrases
{
#define USE_AT_ITS_STRONGEST_PHRASE true
#define DONT_USE_AT_ITS_STRONGEST_PHRASE false
#define USE_ALKAEN_PHRASE true
#define DONT_USE_ALKAEN_PHRASE false

#define TUULI_WORD "tuuli"
#define VAHAN_WORD "vahan"
#define NOPEASTI_WORD "nopeasti"
#define VAHITELLEN_WORD "vahitellen"
#define KOVIMMILLAAN_PHRASE "kovimmillaan"
#define HEIKKENEVAA_WORD "heikkenevaa"
#define VOIMISTUVAA_WORD "voimistuvaa"
#define TUULTA_WORD "tuulta"
#define TUULI_TYYNTYY_PHRASE "tuuli tyyntyy"
#define POHJOISEEN_PHRASE "pohjoiseen"
#define ETELAAN_PHRASE "etelaan"
#define ITAAN_PHRASE "itaan"
#define LANTEEN_PHRASE "lanteen"
#define KOILLISEEN_PHRASE "koilliseen"
#define KAAKKOON_PHRASE "kaakkoon"
#define LOUNAASEEN_PHRASE "lounaaseen"
#define LUOTEESEEN_PHRASE "luoteeseen"
#define POHJOISEN_PUOLELLE_PHRASE "1-puolelle"
#define ETELAN_PUOLELLE_PHRASE "5-puolelle"
#define IDAN_PUOLELLE_PHRASE "3-puolelle"
#define LANNEN_PUOLELLE_PHRASE "7-puolelle"
#define KOILLISEN_PUOLELLE_PHRASE "2-puolelle"
#define KAAKON_PUOLELLE_PHRASE "4-puolelle"
#define LOUNAAN_PUOLELLE_PHRASE "6-puolelle"
#define LUOTEEN_PUOLELLE_PHRASE "8-puolelle"

#define POHJOISEN_JA_KOILLISEN_VALILLE_PHRASE "1- ja 2-valille"
#define IDAN_JA_KOILLISEN_VALILLE_PHRASE "3- ja 2-valille"
#define IDAN_JA_KAAKON_VALILLE_PHRASE "3- ja 4-valille"
#define ETELAN_JA_KAAKON_VALILLE_PHRASE "5- ja 4-valille"
#define ETELAN_JA_LOUNAAN_VALILLE_PHRASE "5- ja 6-valille"
#define LANNEN_JA_LOUNAAN_VALILLE_PHRASE "7- ja 6-valille"
#define LANNEN_JA_LUOTEEN_VALILLE_PHRASE "7- ja 8-valille"
#define POHJOISEN_JA_LUOTEEN_VALILLE_PHRASE "1- ja 8-valille"
#define TUULI_MUUTTUU_VAIHTELEVAKSI_PHRASE "tuuli muuttuu vaihtelevaksi"

#define POHJOINEN_TUULI_P "1-tuulta"
#define POHJOINEN_TUULI "1-tuuli"
#define POHJOISEN_PUOLEINEN_TUULI_P "1-puoleista tuulta"
#define POHJOISEN_PUOLEINEN_TUULI "1-puoleinen tuuli"
#define POHJOINEN_KOILLINEN_TUULI_P "1- ja 2-valista tuulta"
#define POHJOINEN_KOILLINEN_TUULI "1- ja 2-valinen tuuli"
#define KOILLINEN_TUULI_P "2-tuulta"
#define KOILLINEN_TUULI "2-tuuli"
#define KOILLISEN_PUOLEINEN_TUULI_P "2-puoleista tuulta"
#define KOILLISEN_PUOLEINEN_TUULI "2-puoleinen tuuli"
#define ITA_KOILLINEN_TUULI_P "3- ja 2-valista tuulta"
#define ITA_KOILLINEN_TUULI "3- ja 2-valinen tuuli"
#define ITA_TUULI_P "3-tuulta"
#define ITA_TUULI "3-tuuli"
#define IDAN_PUOLEINEN_TUULI_P "3-puoleista tuulta"
#define IDAN_PUOLEINEN_TUULI "3-puoleinen tuuli"
#define ITA_KAAKKO_TUULI_P "3- ja 4-valista tuulta"
#define ITA_KAAKKO_TUULI "3- ja 4-valinen tuuli"
#define KAAKKO_TUULI_P "4-tuulta"
#define KAAKKO_TUULI "4-tuuli"
#define KAAKON_PUOLEINEN_TUULI_P "4-puoleista tuulta"
#define KAAKON_PUOLEINEN_TUULI "4-puoleinen tuuli"
#define ETELA_KAAKKO_TUULI_P "5- ja 4-valista tuulta"
#define ETELA_KAAKKO_TUULI "5- ja 4-valinen tuuli"
#define ETELA_TUULI_P "5-tuulta"
#define ETELA_TUULI "5-tuuli"
#define ETELAN_PUOLEINEN_TUULI_P "5-puoleista tuulta"
#define ETELAN_PUOLEINEN_TUULI "5-puoleinen tuuli"
#define ETELA_LOUNAS_TUULI_P "5- ja 6-valista tuulta"
#define ETELA_LOUNAS_TUULI "5- ja 6-valinen tuuli"
#define LOUNAS_TUULI_P "6-tuulta"
#define LOUNAS_TUULI "6-tuuli"
#define LOUNAAN_PUOLEINEN_TUULI_P "6-puoleista tuulta"
#define LOUNAAN_PUOLEINEN_TUULI "6-puoleinen tuuli"
#define LANSI_LOUNAS_TUULI_P "7- ja 6-valista tuulta"
#define LANSI_LOUNAS_TUULI "7- ja 6-valinen tuuli"
#define LANSI_TUULI_P "7-tuulta"
#define LANSI_TUULI "7-tuuli"
#define LANNEN_PUOLEINEN_TUULI_P "7-puoleista tuulta"
#define LANNEN_PUOLEINEN_TUULI "7-puoleinen tuuli"
#define LANSI_LUODE_TUULI_P "7- ja 8-valista tuulta"
#define LANSI_LUODE_TUULI "7- ja 8-valinen tuuli"
#define LUODE_TUULI_P "8-tuulta"
#define LUODE_TUULI "8-tuuli"
#define LUOTEEN_PUOLEINEN_TUULI_P "8-puoleista tuulta"
#define LUOTEEN_PUOLEINEN_TUULI "8-puoleinen tuuli"
#define POHJOINEN_LUODE_TUULI_P "1- ja 8-valista tuulta"
#define POHJOINEN_LUODE_TUULI "1- ja 8-valinen tuuli"
#define VAIHTELEVA_TUULI_P "suunnaltaan vaihtelevaa tuulta"
#define VAIHTELEVA_TUULI "suunnaltaan vaihteleva tuuli"

#define POHJOISTUULI_ALKAA_VOIMISTUA_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[pohjoistuuli] alkaa voimistua ja muuttuu vaihtelevaksi"
#define POHJOISTUULI_VOIMISTUU_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[pohjoistuuli] voimistuu ja muuttuu vaihtelevaksi"
#define POHJOISTUULI_ALKAA_HEIKETA_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[pohjoistuuli] alkaa heiketa ja muuttuu vaihtelevaksi"
#define POHJOISTUULI_HEIKKENEE_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[pohjoistuuli] heikkenee ja muuttuu vaihtelevaksi"
#define POHJOISTUULI_VOIMISTUU_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[pohjoistuuli] voimistuu ja muuttuu tilapaisesti vaihtelevaksi"
#define POHJOISTUULI_ALKAA_VOIMISTUA_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[pohjoistuuli] alkaa voimistua ja muuttuu tilapaisesti vaihtelevaksi"
#define POHJOISTUULI_HEIKKENEE_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[pohjoistuuli] heikkenee ja muuttuu tilapaisesti vaihtelevaksi"
#define POHJOISTUULI_ALKAA_HEIKETA_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[pohjoistuuli] alkaa heiketa ja muuttuu tilapaisesti vaihtelevaksi"
#define POHJOISTUULI_HEIKKENEE_NOPEASTI_COMPOSITE_PHRASE "[pohjoistuuli] heikkenee [nopeasti]"
#define POHJOISTUULI_VOIMISTUU_NOPEASTI_COMPOSITE_PHRASE "[pohjoistuuli] voimistuu [nopeasti]"

#define PUUSKITTAISTA_ETELATUULTA_COMPOSITE_PHRASE "puuskittaista [etelatuulta]"
#define POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_HEIKETA_COMPOSITE_PHRASE \
  "[pohjoistuulta] [m...n] [metria sekunnissa], joka alkaa heiketa"
#define POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_COMPOSITE_PHRASE \
  "[pohjoistuulta] [m...n] [metria sekunnissa], joka alkaa voimistua"
#define ILTAPAIVALLA_EDELLEEN_HEIKKENEVAA_POHJOISTUULTA \
  "[iltapaivalla] edelleen [heikkenevaa] [pohjoistuulta]"
#define ILTAPAIVALLA_POHJOISTUULI_HEIKKENEE_EDELLEEN \
  "[iltapaivalla] [pohjoistuuli] heikkenee edelleen"
#define ILTAPAIVALLA_POHJOISTUULI_VOIMISTUU_EDELLEEN \
  "[iltapaivalla] [pohjoistuuli] voimistuu edelleen"
#define ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_JA_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli kaantyy [etelaan] ja voimistuu edelleen"
#define ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli kaantyy [etelaan] ja heikkenee edelleen"
#define ILTAPAIVALLA_TUULI_VOIMISTUU_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli voimistuu ja kaantyy [etelaan]"
#define ILTAPAIVALLA_POHJOISTUULI_HEIKKENEE_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE \
  "[iltapaivalla] [pohjoistuuli] heikkenee ja kaantyy [etelaan]"
#define ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli muuttuu vaihtelevaksi ja voimistuu edelleen"
#define ILTAPAIVALLA_TUULI_ALKAA_VOIMISTUA_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli alkaa voimistua ja muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_VOIMISTUU_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli voimistuu ja muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli muuttuu vaihtelevaksi ja heikkenee edelleen"
#define ILTAPAIVALLA_TUULI_HEIKKENEE_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli heikkenee ja muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_ALKAA_HEIKETA_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli alkaa heiketa ja muuttuu vaihtelevaksi"
#define POHJOISTUULTA_INTERVALLI_MS_JOKA_KAANTYY_ILTAPAIVALLA_ETELAAN_COMPOSITE_PHRASE \
  "[pohjoistuulta] [m...n] [metria sekunnissa], joka kaantyy [iltapaivalla] [etelaan]"
#define ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] tuuli kaantyy [etelaan]"
#define ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_ETELATUULI_HEIKKENEE_NOPEASTI_COMPOSITE_PHRASE \
  "[iltapaivalla] [etelatuuli] heikkenee [nopeasti]"
#define ILTAPAIVALLA_ETELATUULI_VOIMISTUU_NOPEASTI_COMPOSITE_PHRASE \
  "[iltapaivalla] [etelatuuli] voimistuu [nopeasti]"
#define ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE \
  "[iltapaivalla] [nopeasti] [heikkenevaa] [etelatuulta]"
#define ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_JOKA_KAANTYY_POHJOISEEN_COMPOSITE_PHRASE \
  "[iltapaivalla] [nopeasti] [heikkenevaa] [etelatuulta], joka kaantyy [pohjoiseen]"
#define ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_JOKA_KAANTYY_ILLALLA_POHJOISEEN_COMPOSITE_PHRASE \
  "[iltapaivalla] [nopeasti] [heikkenevaa] [etelatuulta], joka kaantyy [illalla] [pohjoiseen]"
#define ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_INTERVALLI_MS_JOKA_KAANTYY_ILLALLA_POHJOISEEN_COMPOSITE_PHRASE \
  "[iltap] [nop] [heikken] [tuulta] [m...n] [m sek], joka kaantyy [illalla] [pohj]"
#define ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE "[iltapaivalla] [etelatuulta]"

#define ILTAPAIVALLA_TUULI_VOIMISTUU_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli voimistuu ja muuttuu tilapaisesti vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_ALKAA_VOIMISTUA_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli alkaa voimistua ja muuttuu tilapaisesti vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_JA_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli muuttuu tilapaisesti vaihtelevaksi ja voimistuu edelleen"
#define ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli muuttuu tilapaisesti vaihtelevaksi ja heikkenee edelleen"
#define ILTAPAIVALLA_TUULI_HEIKKENEE_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli heikkenee ja muuttuu tilapaisesti vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_ALKAA_HEIKETA_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli alkaa heiketa ja muuttuu tilapaisesti vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli muuttuu tilapaisesti vaihtelevaksi"

#define ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI_COMPOSITE_PHRASE \
  "[iltapaivalla] [etelatuuli] alkaa heiketa [nopeasti]"
#define ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI_COMPOSITE_PHRASE \
  "[iltapaivalla] [etelatuuli] alkaa voimistua [nopeasti]"
#define ILTAPAIVALLA_TUULI_ALKAA_VOIMISTUA_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli alkaa voimistua ja kaantyy [etelaan]"
#define ILTAPAIVALLA_POHJOISTUULI_ALKAA_HEIKETA_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE \
  "[iltapaivalla] [pohjoistuuli] alkaa heiketa ja kaantyy [etelaan]"
#define ILTAPAIVALLA_POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_HEIKETA_NOPEASTI_JA_KAANTYY_ILLALLA_ETELAAN_COMPOSITE_PHRASE \
  "[ip] [p-tuulta] [m...n] [m/s], joka alkaa heiketa [nopeasti] ja kaantyy [illalla] [etelaan]"
#define ILTAPAIVALLA_POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_NOPEASTI_JA_KAANTYY_ILLALLA_ETELAAN_COMPOSITE_PHRASE \
  "[ip] [p-tuulta] [m...n] [m/s], joka alkaa voimistua [nopeasti] ja kaantyy [illalla] [etelaan]"

}  // namespace WindForecastPhrases

using namespace WindForecastPhrases;

std::string as_string(const GlyphContainer& gc)
{
  DebugTextFormatter dtf;

  return gc.realize(dtf);
}

// return true if equalized top wind is weak duuring whole period
bool is_weak_period(const wo_story_params& theParameters, const WeatherPeriod& thePeriod)
{
  for (auto item : theParameters.theWindDataVector)
  {
    WindDataItemUnit& dataitem = item->getDataItem(theParameters.theArea.type());

    if (is_inside(dataitem.thePeriod.localStartTime(), thePeriod) &&
        dataitem.theEqualizedTopWind.value() > WEAK_WIND_SPEED_UPPER_LIMIT)
      return false;
  }

  return true;
}

unsigned int get_peak_wind(const WeatherPeriod& thePeriod, const wo_story_params& theParameters)
{
  unsigned int upper_index = 0;

  bool upper_index_updated(false);
  for (unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
  {
    const WindDataItemUnit& windDataItem =
        (*theParameters.theWindDataVector[i])(theParameters.theArea.type());

    if (is_inside(windDataItem.thePeriod, thePeriod))
    {
      double totalShareAtTimestep = 0.0;
      // iterate shares at this timestep: shares are cumulated
      for (unsigned int k = 0; k < windDataItem.theWindSpeedDistributionTop.size(); k++)
      {
        WeatherResult shareToAdd(windDataItem.theWindSpeedDistributionTop[k].second);
        totalShareAtTimestep += shareToAdd.value();
        if (totalShareAtTimestep >= theParameters.theWindSpeedTopCoverage)
        {
          if (!upper_index_updated || k > static_cast<unsigned int>(upper_index))
          {
            upper_index = k;
            upper_index_updated = true;
          }
          break;
        }
      }
    }
  }

  return upper_index;
}

float get_wind_direction_share(
    const vector<pair<float, WeatherResult> >& theWindDirectionDistribution,
    WindDirectionId windDirectionId,
    double theWindDirectionMinSpeed)
{
  float retval(0.0);

  for (unsigned int i = 0; i < theWindDirectionDistribution.size(); i++)
  {
    WeatherResult directionVar(theWindDirectionDistribution[i].first + 1.0, 0.0);
    WeatherResult speedVar(WeatherResult(5.0, 0.0));
    WindDirectionId directionId =
        wind_direction_id(directionVar, speedVar, "", theWindDirectionMinSpeed);
    if (directionId == windDirectionId) retval += theWindDirectionDistribution[i].second.value();
  }

  return retval;
}

float get_wind_direction_share(const wo_story_params& theParameters,
                               const WeatherPeriod& period,
                               const WindDirectionId& windDirectionId,
                               WindStoryTools::CompassType compass_type /*= sixteen_directions*/)
{
  float sumShare(0.0);
  unsigned int counter(0);

  for (unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
  {
    WindDataItemUnit& item =
        theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
    if (is_inside(item.thePeriod.localStartTime(), period))
    {
      sumShare += get_wind_direction_share(
          (compass_type == sixteen_directions ? item.theWindDirectionDistribution16
                                              : item.theWindDirectionDistribution8),
          windDirectionId,
          theParameters.theWindDirectionMinSpeed);
      counter++;
    }
  }

  return (counter == 0 ? 0.0 : sumShare / counter);
}

WeatherResult get_wind_direction_result_at(const wo_story_params& theParameters,
                                           const TextGenPosixTime& pointOfTime,
                                           const string& var)
{
  WeatherResult retval(kFloatMissing, 0.0);

  for (unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
  {
    WindDataItemUnit& item =
        theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
    if (item.thePeriod.localStartTime() == pointOfTime)
    {
      retval = item.theEqualizedWindDirection;
      break;
    }
  }

  return retval;
}

bool is_gusty_wind(const wo_story_params& theParameters,
                   const TextGenPosixTime& pointOfTime,
                   const string& var)
{
  for (unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
  {
    WindDataItemUnit& item =
        theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
    if (item.thePeriod.localStartTime() == pointOfTime)
    {
      return (item.theEqualizedTopWind.value() >= 20.0 &&
              item.theGustSpeed.value() - item.theEqualizedTopWind.value() >=
                  theParameters.theGustyWindTopWindDifference);
    }
  }
  return false;
}

bool is_gusty_wind(const wo_story_params& theParameters,
                   const WeatherPeriod& period,
                   const string& var)
{
  float speedSum(0.0);
  float gustSpeedSum(0.0);
  unsigned int n(0);
  for (unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
  {
    WindDataItemUnit& item =
        theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
    if (is_inside(item.thePeriod.localStartTime(), period))
    {
      speedSum += item.theEqualizedTopWind.value();
      gustSpeedSum += item.theGustSpeed.value();
      n++;
    }
  }

  if (n > 0)
  {
    float avgTopWind(speedSum / n);
    float avgGustSpeed(gustSpeedSum / n);
    return (avgTopWind >= 20.0 &&
            avgGustSpeed - avgTopWind >= theParameters.theGustyWindTopWindDifference);
  }

  return false;
}

pair<WeatherResult, WindDirectionId> get_wind_direction_pair_at(
    const wo_story_params& theParameters, const TextGenPosixTime& pointOfTime)
{
  WeatherResult directionValue(kFloatMissing, 0.0);
  WindDirectionId directionId(MISSING_WIND_DIRECTION_ID);

  for (unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
  {
    WindDataItemUnit& item =
        theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());

    if (item.thePeriod.localStartTime() == pointOfTime)
    {
      directionValue = item.theEqualizedWindDirection;
      directionId = wind_direction_id(item.theEqualizedWindDirection,
                                      item.theEqualizedTopWind,
                                      theParameters.theVar,
                                      theParameters.theWindDirectionMinSpeed);
    }
  }

  return make_pair(directionValue, directionId);
}

pair<WeatherResult, WindDirectionId> get_wind_direction_pair_at(
    const wo_story_params& theParameters, const WeatherPeriod& period)
{
  float topSpeedSum(0.0);
  float topSpeedStdDevSum(0.0);
  float medianSpeedSum(0.0);
  float medianSpeedStdDevSum(0.0);
  unsigned int n(0);
  for (unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
  {
    WindDataItemUnit& item =
        theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
    if (is_inside(item.thePeriod.localStartTime(), period))
    {
      topSpeedSum += item.theEqualizedTopWind.value();
      topSpeedStdDevSum += item.theEqualizedTopWind.error();
      medianSpeedSum += item.theEqualizedMedianWind.value();
      medianSpeedStdDevSum += item.theEqualizedMedianWind.error();
      n++;
    }
  }
  float avgTopWind(topSpeedSum / n);
  float avgTopWindStdDev(topSpeedStdDevSum / n);
  float avgMedianWind(medianSpeedSum / n);
  float avgMedianWindStdDev(medianSpeedStdDevSum / n);
  WeatherResult topWindSpeed(avgTopWind, avgTopWindStdDev);
  WeatherResult medianWindSpeed(avgMedianWind, avgMedianWindStdDev);

  WeatherResult resultDirection = mean_wind_direction(theParameters.theSources,
                                                      theParameters.theArea,
                                                      period,
                                                      medianWindSpeed,
                                                      topWindSpeed,
                                                      theParameters.theVar);

  float directionError =
      mean_wind_direction_error(theParameters.theWindDataVector, theParameters.theArea, period);

  if (directionError < resultDirection.error())
    resultDirection = WeatherResult(resultDirection.value(), directionError);

  return make_pair(resultDirection,
                   wind_direction_id(resultDirection,
                                     topWindSpeed,
                                     theParameters.theVar,
                                     theParameters.theWindDirectionMinSpeed));
}

WeatherResult get_wind_direction_at(const wo_story_params& theParameters,
                                    const TextGenPosixTime& pointOfTime)
{
  return (get_wind_direction_pair_at(theParameters, pointOfTime).first);
}

WeatherResult get_wind_direction_at(const wo_story_params& theParameters,
                                    const WeatherPeriod& period)
{
  return (get_wind_direction_pair_at(theParameters, period).first);
}

WindDirectionId get_wind_direction_id_at(const wo_story_params& theParameters,
                                         const TextGenPosixTime& pointOfTime)
{
  return (get_wind_direction_pair_at(theParameters, pointOfTime).second);
}

WindDirectionId get_wind_direction_id_at(const wo_story_params& theParameters,
                                         const WeatherPeriod& period)
{
  return (get_wind_direction_pair_at(theParameters, period).second);
}

std::vector<WeatherPeriod> get_wind_direction_periods(const wo_story_params& theParameters,
                                                      const WeatherPeriod& thePeriod)
{
  std::vector<WeatherPeriod> ret;

  unsigned int firstIndex(UINT_MAX);
  unsigned int lastIndex(UINT_MAX);

  // find start and end index
  bool periodFound = false;
  for (unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
  {
    const WindDataItemUnit& windDataItem =
        theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());

    if (is_inside(windDataItem.thePeriod, thePeriod))
    {
      if (firstIndex == UINT_MAX) firstIndex = i;
      lastIndex = i;
      periodFound = true;
      ;
    }
  }

  if (!periodFound) return ret;

  WindDataItemUnit previousWindDataItem =
      theParameters.theWindDataVector[firstIndex]->getDataItem(theParameters.theArea.type());

  WeatherResult previousDirection =
      get_wind_direction_at(theParameters, previousWindDataItem.thePeriod);

  WindDirectionId idPrevious = wind_direction_id(previousWindDataItem.theEqualizedWindDirection,
                                                 previousWindDataItem.theEqualizedTopWind,
                                                 theParameters.theVar,
                                                 theParameters.theWindDirectionMinSpeed);

  std::vector<TextGenPosixTime> changeTimes;
  changeTimes.push_back(previousWindDataItem.thePeriod.localStartTime());

  for (unsigned int index = firstIndex + 1; index < lastIndex; index++)
  {
    WindDataItemUnit currentWindDataItem =
        theParameters.theWindDataVector[index]->getDataItem(theParameters.theArea.type());

    WindDirectionId idCurrent = wind_direction_id(currentWindDataItem.theEqualizedWindDirection,
                                                  currentWindDataItem.theEqualizedTopWind,
                                                  theParameters.theVar,
                                                  theParameters.theWindDirectionMinSpeed);

    WeatherResult currentDirection =
        get_wind_direction_at(theParameters, currentWindDataItem.thePeriod);

    if (idCurrent == idPrevious) continue;
    if (idCurrent != VAIHTELEVA && idPrevious != VAIHTELEVA &&
        !wind_direction_differ_enough(
            previousDirection, currentDirection, theParameters.theWindDirectionThreshold))
      continue;

    changeTimes.push_back(currentWindDataItem.thePeriod.localStartTime());

    previousWindDataItem = currentWindDataItem;
    previousDirection = currentDirection;
    idPrevious = idCurrent;
  }

  std::vector<WeatherPeriod> directionPeriods;

  for (unsigned int i = 0; i < changeTimes.size(); i++)
  {
    if (i < changeTimes.size() - 1)
    {
      TextGenPosixTime startTime = changeTimes[i];
      TextGenPosixTime endTime = changeTimes[i + 1];
      endTime.ChangeByHours(-1);
      directionPeriods.push_back(WeatherPeriod(startTime, endTime));
    }
    else
    {
      TextGenPosixTime startTime = changeTimes[i];
      TextGenPosixTime endTime = thePeriod.localEndTime();
      directionPeriods.push_back(WeatherPeriod(startTime, endTime));
    }
  }

  // remove short periods (max 2 hours) aroud varying wind, except the last period
  std::set<unsigned int> indexesToRemove;
  for (unsigned int i = 1; i < directionPeriods.size() - 1; i++)
  {
    WeatherPeriod previousPeriod = directionPeriods[i - 1];
    WeatherPeriod currentPeriod = directionPeriods[i];
    WeatherPeriod nextPeriod = directionPeriods[i + 1];
    WindDirectionId currentId =
        get_wind_direction_id_at(theParameters, currentPeriod.localStartTime());
    if (currentId == VAIHTELEVA)
    {
      if (i != 1 && get_period_length(previousPeriod) <= 2)
      {
        indexesToRemove.insert(i - 1);
      }
      if (i + 1 != directionPeriods.size() - 1 && get_period_length(nextPeriod) <= 2)
      {
        indexesToRemove.insert(i + 1);
      }
    }
  }

  std::vector<WeatherPeriod> cleanedPeriods;
  for (unsigned int i = 0; i < directionPeriods.size(); i++)
  {
    // short period in the beginning is ignored
    if (i == 0 && get_period_length(directionPeriods[i]) < 1) continue;

    if (indexesToRemove.find(i) == indexesToRemove.end())
      cleanedPeriods.push_back(directionPeriods[i]);
  }

  indexesToRemove.clear();

  // after previous steps there can be small gaps between periods;
  // merge the successive periods if direction stays the same
  for (unsigned int i = 0; i < cleanedPeriods.size() - 1; i++)
  {
    for (unsigned int k = i + 1; k < cleanedPeriods.size(); k++)
    {
      WindDirectionId previousId = get_wind_direction_id_at(theParameters, cleanedPeriods[i]);
      WindDirectionId currentId = get_wind_direction_id_at(theParameters, cleanedPeriods[k]);
      if (previousId == currentId)
      {
        cleanedPeriods[i] =
            WeatherPeriod(cleanedPeriods[i].localStartTime(), cleanedPeriods[k].localEndTime());
        i++;
        indexesToRemove.insert(k);
      }
      else
        break;
    }
  }

  for (unsigned int i = 0; i < cleanedPeriods.size(); i++)
    if (indexesToRemove.find(i) == indexesToRemove.end()) ret.push_back(cleanedPeriods[i]);

  return ret;
}

TextGenPosixTime get_wind_direction_change_time(const wo_story_params& theParameter,
                                                const WeatherPeriod& thePeriod,
                                                bool fromBeginning = false)
{
  unsigned int startIndex(UINT_MAX);
  unsigned int endIndex(UINT_MAX);

  for (unsigned int i = 0; i < theParameter.theWindDataVector.size(); i++)
  {
    const WindDataItemUnit& windDataItem =
        theParameter.theWindDataVector[i]->getDataItem(theParameter.theArea.type());

    if (is_inside(windDataItem.thePeriod, thePeriod))
    {
      if (startIndex == UINT_MAX) startIndex = i;
      endIndex = i;
    }
  }

  WindDirectionId directionId = MISSING_WIND_DIRECTION_ID;

  // iterate period from end as long as direction stays the same
  unsigned int firstIndex = (fromBeginning ? startIndex : endIndex);
  unsigned int lastIndex = (fromBeginning ? endIndex : startIndex);
  unsigned int index = firstIndex;
  while (index != lastIndex)
  {
    const WindDataItemUnit& windDataItem =
        theParameter.theWindDataVector[index]->getDataItem(theParameter.theArea.type());
    if (index == firstIndex)
    {
      directionId = get_wind_direction_id_at(theParameter, windDataItem.thePeriod.localEndTime());
    }
    else if (directionId !=
             get_wind_direction_id_at(theParameter, windDataItem.thePeriod.localEndTime()))
      return windDataItem.thePeriod.localEndTime();

    if (fromBeginning)
      index += 1;
    else
      index -= 1;
  }

  return thePeriod.localEndTime();
}

std::string get_wind_event_string(WindEventId theWindEventId)
{
  std::string retval;

  switch (theWindEventId)
  {
    case TUULI_HEIKKENEE:
      retval = "tuuli heikkenee";
      break;
    case TUULI_VOIMISTUU:
      retval = "tuuli voimistuu";
      break;
    case TUULI_TYYNTYY:
      retval = "tuuli tyyntyy";
      break;
    case TUULI_KAANTYY:
      retval = "tuuli kaantyy";
      break;
    case TUULI_KAANTYY_JA_HEIKKENEE:
      retval = "tuuli kaantyy ja heikkenee";
      break;
    case TUULI_KAANTYY_JA_VOIMISTUU:
      retval = "tuuli kaantyy ja voimistuu";
      break;
    case TUULI_KAANTYY_JA_TYYNTYY:
      retval = "tuuli kaantyy ja tyyntyy";
      break;
    case TUULI_MUUTTUU_VAIHTELEVAKSI:
      retval = "tuuli muuttuu vaihtelevaksi";
      break;
    case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE:
      retval = "tuuli muuttuu vaihtelevaksi ja heikkenee";
      break;
    case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU:
      retval = "tuuli muuttuu vaihtelevaksi ja voimistuu";
      break;
    case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY:
      retval = "tuuli muuttuu vaihtelevaksi ja tyyntyy";
      break;
    case MISSING_WIND_EVENT:
      retval = "missing wind event";
      break;
    case MISSING_WIND_SPEED_EVENT:
      retval = "missing wind speed event";
      break;
    case MISSING_WIND_DIRECTION_EVENT:
      retval = "missing wind direction event";
      break;
  }

  return retval;
}

bool is_valid_wind_event_id(const int& eventId)
{
  bool retval(false);

  switch (eventId)
  {
    case TUULI_HEIKKENEE:
    case TUULI_VOIMISTUU:
    case TUULI_TYYNTYY:
    case TUULI_KAANTYY:
    case TUULI_KAANTYY_JA_HEIKKENEE:
    case TUULI_KAANTYY_JA_VOIMISTUU:
    case TUULI_KAANTYY_JA_TYYNTYY:
    case TUULI_MUUTTUU_VAIHTELEVAKSI:
    case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE:
    case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU:
    case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY:
    case MISSING_WIND_EVENT:
    case MISSING_WIND_SPEED_EVENT:
    case MISSING_WIND_DIRECTION_EVENT:
      retval = true;
      break;
  }

  return retval;
}

Sentence windDirectionSentence(WindDirectionId theWindDirectionId, bool theBasicForm = false)
{
  Sentence sentence;

  switch (theWindDirectionId)
  {
    case POHJOINEN:
      sentence << (theBasicForm ? POHJOINEN_TUULI : POHJOINEN_TUULI_P);
      break;
    case POHJOISEN_PUOLEINEN:
      sentence << (theBasicForm ? POHJOISEN_PUOLEINEN_TUULI : POHJOISEN_PUOLEINEN_TUULI_P);
      break;
    case POHJOINEN_KOILLINEN:
      sentence << (theBasicForm ? POHJOINEN_KOILLINEN_TUULI : POHJOINEN_KOILLINEN_TUULI_P);
      break;
    case KOILLINEN:
      sentence << (theBasicForm ? KOILLINEN_TUULI : KOILLINEN_TUULI_P);
      break;
    case KOILLISEN_PUOLEINEN:
      sentence << (theBasicForm ? KOILLISEN_PUOLEINEN_TUULI : KOILLISEN_PUOLEINEN_TUULI_P);
      break;
    case ITA_KOILLINEN:
      sentence << (theBasicForm ? ITA_KOILLINEN_TUULI : ITA_KOILLINEN_TUULI_P);
      break;
    case ITA:
      sentence << (theBasicForm ? ITA_TUULI : ITA_TUULI_P);
      break;
    case IDAN_PUOLEINEN:
      sentence << (theBasicForm ? IDAN_PUOLEINEN_TUULI : IDAN_PUOLEINEN_TUULI_P);
      break;
    case ITA_KAAKKO:
      sentence << (theBasicForm ? ITA_KAAKKO_TUULI : ITA_KAAKKO_TUULI_P);
      break;
    case KAAKKO:
      sentence << (theBasicForm ? KAAKKO_TUULI : KAAKKO_TUULI_P);
      break;
    case KAAKON_PUOLEINEN:
      sentence << (theBasicForm ? KAAKON_PUOLEINEN_TUULI : KAAKON_PUOLEINEN_TUULI_P);
      break;
    case ETELA_KAAKKO:
      sentence << (theBasicForm ? ETELA_KAAKKO_TUULI : ETELA_KAAKKO_TUULI_P);
      break;
    case ETELA:
      sentence << (theBasicForm ? ETELA_TUULI : ETELA_TUULI_P);
      break;
    case ETELAN_PUOLEINEN:
      sentence << (theBasicForm ? ETELAN_PUOLEINEN_TUULI : ETELAN_PUOLEINEN_TUULI_P);
      break;
    case ETELA_LOUNAS:
      sentence << (theBasicForm ? ETELA_LOUNAS_TUULI : ETELA_LOUNAS_TUULI_P);
      break;
    case LOUNAS:
      sentence << (theBasicForm ? LOUNAS_TUULI : LOUNAS_TUULI_P);
      break;
    case LOUNAAN_PUOLEINEN:
      sentence << (theBasicForm ? LOUNAAN_PUOLEINEN_TUULI : LOUNAAN_PUOLEINEN_TUULI_P);
      break;
    case LANSI_LOUNAS:
      sentence << (theBasicForm ? LANSI_LOUNAS_TUULI : LANSI_LOUNAS_TUULI_P);
      break;
    case LANSI:
      sentence << (theBasicForm ? LANSI_TUULI : LANSI_TUULI_P);
      break;
    case LANNEN_PUOLEINEN:
      sentence << (theBasicForm ? LANNEN_PUOLEINEN_TUULI : LANNEN_PUOLEINEN_TUULI_P);
      break;
    case LANSI_LUODE:
      sentence << (theBasicForm ? LANSI_LUODE_TUULI : LANSI_LUODE_TUULI_P);
      break;
    case LUODE:
      sentence << (theBasicForm ? LUODE_TUULI : LUODE_TUULI_P);
      break;
    case LUOTEEN_PUOLEINEN:
      sentence << (theBasicForm ? LUOTEEN_PUOLEINEN_TUULI : LUOTEEN_PUOLEINEN_TUULI_P);
      break;
    case POHJOINEN_LUODE:
      sentence << (theBasicForm ? POHJOINEN_LUODE_TUULI : POHJOINEN_LUODE_TUULI_P);
      break;
    case VAIHTELEVA:
    case MISSING_WIND_DIRECTION_ID:
      sentence << (theBasicForm ? VAIHTELEVA_TUULI : VAIHTELEVA_TUULI_P);
      break;
  }

  return sentence;
}

std::string getWindDirectionTurntoString(WindDirectionId theWindDirectionId)
{
  std::string retval;

  switch (theWindDirectionId)
  {
    case POHJOINEN:
      retval = POHJOISEEN_PHRASE;
      break;
    case POHJOISEN_PUOLEINEN:
      retval = POHJOISEN_PUOLELLE_PHRASE;
      break;
    case POHJOINEN_KOILLINEN:
      retval = POHJOISEN_JA_KOILLISEN_VALILLE_PHRASE;
      break;
    case KOILLINEN:
      retval = KOILLISEEN_PHRASE;
      break;
    case KOILLISEN_PUOLEINEN:
      retval = KOILLISEN_PUOLELLE_PHRASE;
      break;
    case ITA_KOILLINEN:
      retval = IDAN_JA_KOILLISEN_VALILLE_PHRASE;
      break;
    case ITA:
      retval = ITAAN_PHRASE;
      break;
    case IDAN_PUOLEINEN:
      retval = IDAN_PUOLELLE_PHRASE;
      break;
    case ITA_KAAKKO:
      retval = IDAN_JA_KAAKON_VALILLE_PHRASE;
      break;
    case KAAKKO:
      retval = KAAKKOON_PHRASE;
      break;
    case KAAKON_PUOLEINEN:
      retval = KAAKON_PUOLELLE_PHRASE;
      break;
    case ETELA_KAAKKO:
      retval = ETELAN_JA_KAAKON_VALILLE_PHRASE;
      break;
    case ETELA:
      retval = ETELAAN_PHRASE;
      break;
    case ETELAN_PUOLEINEN:
      retval = ETELAN_PUOLELLE_PHRASE;
      break;
    case ETELA_LOUNAS:
      retval = ETELAN_JA_LOUNAAN_VALILLE_PHRASE;
      break;
    case LOUNAS:
      retval = LOUNAASEEN_PHRASE;
      break;
    case LOUNAAN_PUOLEINEN:
      retval = LOUNAAN_PUOLELLE_PHRASE;
      break;
    case LANSI_LOUNAS:
      retval = LANNEN_JA_LOUNAAN_VALILLE_PHRASE;
      break;
    case LANSI:
      retval = LANTEEN_PHRASE;
      break;
    case LANNEN_PUOLEINEN:
      retval = LANNEN_PUOLELLE_PHRASE;
      break;
    case LANSI_LUODE:
      retval = LANNEN_JA_LUOTEEN_VALILLE_PHRASE;
      break;
    case LUODE:
      retval = LUOTEESEEN_PHRASE;
      break;
    case LUOTEEN_PUOLEINEN:
      retval = LUOTEEN_PUOLELLE_PHRASE;
      break;
    case POHJOINEN_LUODE:
      retval = POHJOISEN_JA_LUOTEEN_VALILLE_PHRASE;
      break;
    case VAIHTELEVA:
      retval = TUULI_MUUTTUU_VAIHTELEVAKSI_PHRASE;
      break;
    case MISSING_WIND_DIRECTION_ID:
      retval = EMPTY_STRING;
      break;
  }

  return retval;
}

void get_plain_wind_speed_interval(const TextGenPosixTime& pointOfTime,
                                   const wo_story_params& theParameter,
                                   float& lowerLimit,
                                   float& upperLimit)
{
  for (unsigned int i = 0; i < theParameter.theWindDataVector.size(); i++)
  {
    const WindDataItemUnit& windDataItem =
        (*theParameter.theWindDataVector[i])(theParameter.theArea.type());

    if (windDataItem.thePeriod.localStartTime() == pointOfTime)
    {
      lowerLimit = windDataItem.theEqualizedMedianWind.value();
      upperLimit = windDataItem.theEqualizedTopWind.value();
      break;
    }
  }
}

void get_plain_wind_speed_interval(const WeatherPeriod& period,
                                   const wo_story_params& theParameter,
                                   float& lowerLimit,
                                   float& upperLimit)
{
  bool firstMatchProcessed(false);

  float minValue(lowerLimit);
  float maxValue(upperLimit);
  for (unsigned int i = 0; i < theParameter.theWindDataVector.size(); i++)
  {
    const WindDataItemUnit& windDataItem =
        (*theParameter.theWindDataVector[i])(theParameter.theArea.type());

    if (is_inside(windDataItem.thePeriod, period))
    {
      if (!firstMatchProcessed)
      {
        minValue = windDataItem.theEqualizedMedianWind.value();
        maxValue = windDataItem.theEqualizedTopWind.value();
        firstMatchProcessed = true;
      }
      else
      {
        if (windDataItem.theEqualizedMedianWind.value() < minValue)
          minValue = windDataItem.theEqualizedMedianWind.value();
        if (windDataItem.theEqualizedTopWind.value() > maxValue)
          maxValue = windDataItem.theEqualizedTopWind.value();
      }
    }
  }
  lowerLimit = minValue;
  upperLimit = maxValue;
}

float get_median_wind(const WeatherPeriod& period,
                      const WeatherArea& area,
                      const wind_data_item_vector& windDataVector)
{
  float retval(0.0);
  unsigned int counter(0);

  for (unsigned int i = 0; i < windDataVector.size(); i++)
  {
    const WindDataItemUnit& windDataItem = (*windDataVector[i])(area.type());

    if (is_inside(windDataItem.thePeriod, period))
    {
      retval += windDataItem.theEqualizedMedianWind.value();
      counter++;
    }
  }

  return (counter == 0 ? retval : retval / counter);
}

float get_maximum_wind(const WeatherPeriod& period,
                       const WeatherArea& area,
                       const wind_data_item_vector& windDataVector)
{
  float retval(0.0);
  unsigned int counter(0);

  for (unsigned int i = 0; i < windDataVector.size(); i++)
  {
    const WindDataItemUnit& windDataItem = (*windDataVector[i])(area.type());

    if (is_inside(windDataItem.thePeriod, period))
    {
      retval += windDataItem.theEqualizedTopWind.value();
      counter++;
    }
  }

  return (counter == 0 ? retval : retval / counter);
}

unsigned int get_top_wind(const WeatherPeriod& thePeriod, const wo_story_params& theParameters)
{
  unsigned int top_wind(kFloatMissing);

  for (unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
  {
    const WindDataItemUnit& windDataItem =
        (*theParameters.theWindDataVector[i])(theParameters.theArea.type());

    if (is_inside(windDataItem.thePeriod, thePeriod))
    {
      if (top_wind == kFloatMissing || windDataItem.theWindSpeedTop.value() > top_wind)
        top_wind = ceil(windDataItem.theWindSpeedTop.value());
    }
  }

  return top_wind;
}

void windspeed_distribution_interval(const WeatherPeriod& thePeriod,
                                     const wo_story_params& theParameter,
                                     float& lowerLimit,
                                     float& upperLimit)
{
  if (lowerLimit == kFloatMissing || upperLimit == kFloatMissing) return;

  value_distribution_data_vector windSpeedDistributionVector;

  // first calculte the sum of the distribution values for the period
  unsigned int counter(0);
  for (unsigned int i = 0; i < theParameter.theWindDataVector.size(); i++)
  {
    const WindDataItemUnit& windDataItem =
        (*theParameter.theWindDataVector[i])(theParameter.theArea.type());

    if (is_inside(windDataItem.thePeriod, thePeriod))
    {
      if (windSpeedDistributionVector.size() == 0)
      {
        windSpeedDistributionVector = windDataItem.theWindSpeedDistribution;
      }
      else
      {
        for (unsigned int k = 0; k < windDataItem.theWindSpeedDistribution.size(); k++)
        {
          WeatherResult originalShare(windSpeedDistributionVector[k].second);
          WeatherResult shareToAdd(windDataItem.theWindSpeedDistribution[k].second);
          windSpeedDistributionVector[k].second =
              WeatherResult(originalShare.value() + shareToAdd.value(), 0.0);
        }
      }
      counter++;
    }
  }

  // then calculate mean value of the distribution values of the period
  for (unsigned int i = 0; i < windSpeedDistributionVector.size(); i++)
  {
    WeatherResult cumulativeShare(windSpeedDistributionVector[i].second);
    windSpeedDistributionVector[i].second = WeatherResult(cumulativeShare.value() / counter, 0.0);
  }

  float maxvalue = upperLimit;
  float minvalue = lowerLimit;

  // subtract 1 m/s from the lower limit
  lowerLimit = (maxvalue - (minvalue - 1.0) > 5.0 ? maxvalue - 5.0 : minvalue - 1.0);

  // Make sure that 2/3 of values are inside the interval
  int lower_index = static_cast<int>(round(maxvalue));
  if (lower_index >= static_cast<int>(windSpeedDistributionVector.size()))
    lower_index = windSpeedDistributionVector.size() - 1;

  float sumDistribution = windSpeedDistributionVector[maxvalue].second.value();
  while (sumDistribution < 67.0 && lower_index >= 0)
  {
    sumDistribution += windSpeedDistributionVector[lower_index].second.value();
    lower_index--;
  }

  lowerLimit = lower_index;
}

void get_wind_speed_interval(const WeatherPeriod& thePeriod,
                             const wo_story_params& theParameter,
                             float& lowerLimit,
                             float& upperLimit)
{
  get_plain_wind_speed_interval(thePeriod, theParameter, lowerLimit, upperLimit);

  windspeed_distribution_interval(thePeriod, theParameter, lowerLimit, upperLimit);

  // we may cut peak wind based on areal coverage (wind_speed_top_coverage configuration parameter)
  int peakWind = get_peak_wind(thePeriod, theParameter);

  if (peakWind < upperLimit) upperLimit = peakWind;
}

void get_wind_speed_interval(const TextGenPosixTime& pointOfTime,
                             const wo_story_params& theParameter,
                             float& lowerLimit,
                             float& upperLimit)
{
  get_plain_wind_speed_interval(pointOfTime, theParameter, lowerLimit, upperLimit);

  WeatherPeriod period(pointOfTime, pointOfTime);

  windspeed_distribution_interval(period, theParameter, lowerLimit, upperLimit);

  // we may cut peak wind based on areal coverage (wind_speed_top_coverage configuration parameter)
  int peakWind = get_peak_wind(period, theParameter);

  if (peakWind < upperLimit) upperLimit = peakWind;
}

bool wind_speed_differ_enough(const wo_story_params& theParameter,
                              const WeatherPeriod& thePeriod,
                              bool aboveWeakWind /*= false*/)
{
  float begLowerLimit(0), begUpperLimit(0), endLowerLimit(0), endUpperLimit(0);

  get_wind_speed_interval(thePeriod.localStartTime(), theParameter, begLowerLimit, begUpperLimit);
  get_wind_speed_interval(thePeriod.localEndTime(), theParameter, endLowerLimit, endUpperLimit);

  double topWindShare =
      (begUpperLimit < 10.0 && endUpperLimit < 10.0 ? theParameter.theWindCalcTopShareWeak
                                                    : theParameter.theWindCalcTopShare);

  double topWindWeight = (topWindShare / 100.0);
  double topMedianWindWeight = 1.0 - topWindWeight;

  float begSpeed = (begUpperLimit * topWindWeight + begLowerLimit * topMedianWindWeight);
  float endSpeed = (endUpperLimit * topWindWeight + endLowerLimit * topMedianWindWeight);

  if (aboveWeakWind)
  {
    if (begSpeed < WEAK_WIND_SPEED_UPPER_LIMIT) begSpeed = WEAK_WIND_SPEED_UPPER_LIMIT;
    if (endSpeed < WEAK_WIND_SPEED_UPPER_LIMIT) endSpeed = WEAK_WIND_SPEED_UPPER_LIMIT;
  }

  float difference = round(abs(endSpeed - begSpeed));

  return (difference >= theParameter.theWindSpeedThreshold);
}

bool wind_direction_differ_enough(const WeatherResult theWindDirection1,
                                  const WeatherResult theWindDirection2,
                                  float theWindDirectionThreshold)
{
  float dire1 = theWindDirection1.value();
  float dire2 = theWindDirection2.value();

  float difference = abs(dire2 - dire1);
  if (difference > 180.0)
    difference = abs((dire2 > dire1 ? dire2 - (dire1 + 360) : dire1 - (dire2 + 360)));

  return (difference >= theWindDirectionThreshold);
}

bool wind_direction_differ_enough(const wo_story_params& theParameter,
                                  const WindEventPeriodDataItem& windEventPeriodDataItem)
{
  const WeatherResult& windDirection1(
      windEventPeriodDataItem.thePeriodBeginDataItem.theEqualizedWindDirection);
  const WeatherResult& windDirection2(
      windEventPeriodDataItem.thePeriodEndDataItem.theEqualizedWindDirection);
  const WeatherResult& maximumWind1(
      windEventPeriodDataItem.thePeriodBeginDataItem.theEqualizedTopWind);
  const WeatherResult& maximumWind2(
      windEventPeriodDataItem.thePeriodEndDataItem.theEqualizedTopWind);

  WindDirectionId directionIdBeg = wind_direction_id(
      windDirection1, maximumWind1, theParameter.theVar, theParameter.theWindDirectionMinSpeed);
  WindDirectionId directionIdEnd = wind_direction_id(
      windDirection2, maximumWind2, theParameter.theVar, theParameter.theWindDirectionMinSpeed);

  if (directionIdBeg == directionIdEnd) return false;

  return wind_direction_differ_enough(
      windDirection1, windDirection2, theParameter.theWindDirectionThreshold);
}

bool ascending_order(float direction1, float direction2)
{
  bool retval(false);

  if (direction1 >= 180.0)
  {
    if (direction2 > direction1 || direction2 < direction1 - 180.0) retval = true;
  }
  else
  {
    retval = (direction2 > direction1 && direction2 - direction1 < 180);
  }
  return retval;
}

bool wind_turns_to_the_same_direction(float direction1, float direction2, float direction3)
{
  bool ascending12 = ascending_order(direction1, direction2);
  bool ascending23 = ascending_order(direction2, direction3);
  bool ascending13 = ascending_order(direction1, direction3);

  return ((ascending12 && ascending23 && ascending13) ||
          (!ascending12 && !ascending23 && !ascending13));
}

WindEventId mask_wind_event(WindEventId originalId, WindEventId maskToRemove)
{
  unsigned short mask(maskToRemove);
  unsigned short value = originalId;
  value &= (~mask);
  return (static_cast<WindEventId>(value));
}

WeatherPeriod get_wind_turning_period(
    const WeatherPeriod& thePeriod,
    const wind_event_period_data_item_vector& theDirectionEventVector)
{
  for (unsigned int i = 0; i < theDirectionEventVector.size(); i++)
    if (is_inside(theDirectionEventVector[i]->thePeriod, thePeriod))
      return theDirectionEventVector[i]->thePeriod;

  return thePeriod;
}

void get_median_wind_speed_by_area(const wind_data_item_vector& theWindDataVector,
                                   const WeatherArea& theFullArea,
                                   const WeatherPeriod& thePeriod,
                                   float& medianWindFull,
                                   float& medianWindCoastal,
                                   float& medianWindInland)
{
  WeatherArea coastalArea(theFullArea);
  coastalArea.type(WeatherArea::Coast);
  WeatherArea inlandArea(theFullArea);
  inlandArea.type(WeatherArea::Inland);

  medianWindFull = get_median_wind(thePeriod, theFullArea, theWindDataVector);
  medianWindCoastal = get_median_wind(thePeriod, coastalArea, theWindDataVector);
  medianWindInland = get_median_wind(thePeriod, inlandArea, theWindDataVector);
}

void compose_wind_speed_range(const wo_story_params& theParameters,
                              int& intervalLowerLimit,
                              int& intervalUpperLimit,
                              int& peakWind)
{
  int actualIntervalSize(intervalUpperLimit - intervalLowerLimit);

  if (actualIntervalSize < theParameters.theMinIntervalSize)
  {
    // Mikael Frisk 07.03.2013: pelkk‰ 5 m/s on kˆkkˆ => n‰ytet‰‰n minimi-intervalli
    /*
      sentence << intervalLowerLimit
      << *UnitFactory::create(MetersPerSecond);
    */

    intervalUpperLimit = intervalLowerLimit + theParameters.theMinIntervalSize;
    actualIntervalSize = intervalUpperLimit - intervalLowerLimit;
  }

  if (actualIntervalSize > theParameters.theMaxIntervalSize)
  {
    // if size of the interval is one more than allowed, we increase lower limit by 1
    // else we use 'kovimmillaan'-phrase
    if (actualIntervalSize == theParameters.theMaxIntervalSize + 1)
    {
      intervalLowerLimit++;
    }
    else
    {
      intervalUpperLimit = intervalLowerLimit + theParameters.theMaxIntervalSize;
    }
  }

  // if there is only 1 m/s difference between peak wind adn upper limit, make them equal
  if (peakWind - intervalUpperLimit == 1)
  {
    intervalUpperLimit++;
    // and increase lower limit if necessary
    if (intervalUpperLimit - intervalLowerLimit > theParameters.theMaxIntervalSize)
      intervalLowerLimit++;
  }
}

Sentence compose_wind_speed_sentence(const wo_story_params& theParameters,
                                     int intervalLowerLimit,
                                     int intervalUpperLimit,
                                     int peakWind,
                                     bool theUseAtItsStrongestPhrase)
{
  Sentence sentence;

  if (peakWind > intervalUpperLimit)
  {
    if (theUseAtItsStrongestPhrase)
    {
      sentence << IntegerRange(
                      intervalLowerLimit, intervalUpperLimit, theParameters.theRangeSeparator)
               << *UnitFactory::create(MetersPerSecond);
      sentence << Delimiter(COMMA_PUNCTUATION_MARK) << KOVIMMILLAAN_PHRASE << peakWind
               << *UnitFactory::create(MetersPerSecond);
    }
    else
    {
      sentence << IntegerRange(peakWind - theParameters.theMaxIntervalSize,
                               peakWind,
                               theParameters.theRangeSeparator)
               << *UnitFactory::create(MetersPerSecond);
    }
  }
  else
  {
    sentence << IntegerRange(
                    intervalLowerLimit, intervalUpperLimit, theParameters.theRangeSeparator)
             << *UnitFactory::create(MetersPerSecond);
  }

  return sentence;
}

bool sortti(const wind_event_period& item1, const wind_event_period& item2)
{
  // if same starttime direction event is priorized
  if (item1.second.localStartTime() == item2.second.localStartTime())
    return ((item1.first <= TUULI_TYYNTYY || item1.first == MISSING_WIND_SPEED_EVENT) ? true
                                                                                      : false);

  return (item1.second.localStartTime() < item2.second.localStartTime());
}

void handle_period_end(WeatherPeriod& period)
{
  int periodLength = get_period_length(period);
  int endHour = period.localEndTime().GetHour();

  if (periodLength < 2 && (endHour == 18 || endHour == 17 || endHour == 6))
  {
    TextGenPosixTime startTime(period.localEndTime());
    startTime.ChangeByHours(-2);
    period = WeatherPeriod(startTime, period.localEndTime());
  }
}

WindForecast::WindForecast(wo_story_params& parameters)
    : theParameters(parameters),
      thePreviousDayNumber(parameters.theForecastPeriod.localStartTime().GetWeekday()),
      thePreviousPartOfTheDay(MISSING_PART_OF_THE_DAY_ID)
{
}

std::vector<Sentence> WindForecast::reportDirectionChanges(
    const WeatherPeriod& thePeriod,
    std::vector<WeatherPeriod>& theDirectionPeriods,
    WindDirectionInfo& thePreviousWindDirection,
    std::string& thePreviousTimePhrase,
    bool lastPeriod) const
{
  std::vector<Sentence> sentences;

  unsigned int lastReportedIndex = UINT_MAX;
  TextGenPosixTime previousPeriodStartTime;

  theParameters.theLog << "Reporting wind direction changes during wind speed change period "
                       << thePeriod << ". Number of direction changes "
                       << theDirectionPeriods.size() << std::endl;

  for (unsigned int i = 0; i < theDirectionPeriods.size(); i++)
  {
    const WeatherPeriod& period = theDirectionPeriods[i];

    if (is_inside(period.localStartTime(), thePeriod))
    {
      lastReportedIndex = i;

      if (get_period_length(period) < 2)
      {
        theParameters.theLog << "Short direction period " << period << " skipped." << std::endl;
        continue;
      }

      WindDirectionId windDirectionId = get_wind_direction_id_at(theParameters, period);
      // bool temporaryChange = get_period_length(period) <= 5;

      if (windDirectionId == thePreviousWindDirection.id)
      {
        theParameters.theLog << "Same direction as previous "
                             << wind_direction_string(windDirectionId) << " -> skipped."
                             << std::endl;
        continue;
      }

      WeatherResult windDirectionValue = get_wind_direction_at(theParameters, period);

      Sentence timePhrase;

      Sentence sentence;

      if (windDirectionId == VAIHTELEVA)
      {
        timePhrase << getTimePhrase(period.localStartTime(), false);
        sentence << timePhrase << VAIHTELEVA_TUULI_P;
      }
      else
      {
        if (get_period_length(period) >= 6)
          timePhrase << getTimePhrase(period.localStartTime(), get_period_length(period) >= 6);
        else
          timePhrase << getTimePhrase(period, get_period_length(period) >= 6);

        sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE << timePhrase
                 << windDirectionSentence(windDirectionId);
      }

      theParameters.theLog << "Reporting wind direction change on period " << period << std::endl;

      // if wind direction changes rapidly on the period report only the the latest direction
      if (i > 0 && sentences.size() > 0 &&
          period.localStartTime().DifferenceInHours(previousPeriodStartTime) < 8 &&
          as_string(timePhrase) == thePreviousTimePhrase)
        sentences[sentences.size() - 1] = sentence;
      else
        sentences.push_back(sentence);

      thePreviousTimePhrase = as_string(timePhrase);
      thePreviousWindDirection = WindDirectionInfo(windDirectionValue, windDirectionId);
      previousPeriodStartTime = period.localStartTime();
    }
  }

  if (lastReportedIndex != UINT_MAX)
  {
    if (lastReportedIndex == theDirectionPeriods.size() - 1)
      theDirectionPeriods.clear();
    else
      theDirectionPeriods.assign(theDirectionPeriods.begin() + lastReportedIndex + 1,
                                 theDirectionPeriods.end());
  }

  return sentences;
}

std::vector<Sentence> WindForecast::constructWindSentence(
    const WindEventPeriodDataItem* windSpeedItem,
    const WindEventPeriodDataItem* nextWindSpeedItem,
    std::vector<WeatherPeriod> windDirectionReportingPeriods,
    WindDirectionInfo& thePreviousWindDirection,
    bool firstSentence) const
{
  std::vector<Sentence> ret;

  const WeatherPeriod& windSpeedEventPeriod = windSpeedItem->thePeriod;

  // we dont report speed and direction changes on weak period
  if (is_weak_period(theParameters, windSpeedEventPeriod))
  {
    if (!firstSentence)
    {
      theParameters.theLog << "Wind is weak on period " << windSpeedEventPeriod
                           << " -> period is not reported" << std::endl;
    }
    else
    {
      theParameters.theLog << "Wind is weak on period " << windSpeedEventPeriod
                           << " -> reporting the whole period at once!" << std::endl;
      Sentence sentence;
      WeatherResult windDirectionValue = get_wind_direction_at(theParameters, windSpeedEventPeriod);
      WindDirectionId windDirectionId =
          get_wind_direction_id_at(theParameters, windSpeedEventPeriod);
      sentence << windDirectionSentence(windDirectionId)
               << windSpeedIntervalSentence(windSpeedItem->thePeriod, USE_AT_ITS_STRONGEST_PHRASE);

      thePreviousWindDirection = WindDirectionInfo(windDirectionValue, windDirectionId);
      ret.push_back(sentence);
    }
    return ret;
  }

  // get direction changes during whole period
  std::vector<WeatherPeriod> windDirectionChangePeriods;
  for (auto period : windDirectionReportingPeriods)
    if (period.localStartTime() >= windSpeedEventPeriod.localStartTime() &&
        period.localStartTime() <= windSpeedEventPeriod.localEndTime())
      windDirectionChangePeriods.push_back(period);

  WindEventId windSpeedEventId = windSpeedItem->theWindEvent;

  bool lastPeriod = !nextWindSpeedItem;

  switch (windSpeedEventId)
  {
    case MISSING_WIND_SPEED_EVENT:
    {
      theParameters.theLog << "Processing MISSING_WIND_SPEED_EVENT at " << windSpeedEventPeriod
                           << ". " << windDirectionChangePeriods.size()
                           << " direction changes during the period." << std::endl;

      unsigned int numberOfDirectionChanges = 0;
      for (std::vector<WeatherPeriod>::const_iterator it = windDirectionChangePeriods.begin();
           it != windDirectionChangePeriods.end();
           it++)
      {
        Sentence sentence;
        std::vector<WeatherPeriod>::const_iterator iterNext = it;
        iterNext++;
        bool lastDirectionChangeOnCurrentPeriod = (iterNext == windDirectionChangePeriods.end());

        WeatherPeriod directionChangePeriod = *it;

        if (!firstSentence &&
            windSpeedEventPeriod.localStartTime() == directionChangePeriod.localStartTime() &&
            get_period_length(directionChangePeriod) > 0)
        {
          TextGenPosixTime startTime = directionChangePeriod.localStartTime();
          startTime.ChangeByHours(1);
          directionChangePeriod = WeatherPeriod(startTime, directionChangePeriod.localEndTime());
        }

        handle_period_end(directionChangePeriod);

        WeatherResult windDirectionValue =
            get_wind_direction_at(theParameters, directionChangePeriod);
        WindDirectionId windDirectionId =
            get_wind_direction_id_at(theParameters, directionChangePeriod);

        if (windDirectionId == thePreviousWindDirection.id)
        {
          theParameters.theLog << "Direction " << wind_direction_string(windDirectionId)
                               << " is the same as previous period -> skipping" << std::endl;
          continue;
        }

        bool shortPeriod = get_period_length(directionChangePeriod) < 2;

        if (shortPeriod)
        {
          theParameters.theLog << "Short period " << directionChangePeriod << " in the middle ("
                               << wind_direction_string(windDirectionId) << ") -> skipping"
                               << std::endl;
          continue;
        }

        if (firstSentence && ret.empty())
        {
          sentence << windDirectionSentence(windDirectionId)
                   << windSpeedIntervalSentence(windSpeedItem->thePeriod,
                                                USE_AT_ITS_STRONGEST_PHRASE);

          thePreviousWindDirection = WindDirectionInfo(windDirectionValue, windDirectionId);
        }
        else
        {
          bool useAlkaenPhrase = get_period_length(directionChangePeriod) >= 6;

          theParameters.theLog << "Reporting direction change for period " << directionChangePeriod
                               << std::endl;

          if (windDirectionId == VAIHTELEVA)
          {
            bool temporaryChange = get_period_length(directionChangePeriod) <= 5;

            // dont report temporary change if it is not last change
            if (temporaryChange && !lastDirectionChangeOnCurrentPeriod)
            {
              theParameters.theLog << "Temporary VAIHTELEVA period (not last direction change) "
                                   << directionChangePeriod << " -> skipping" << std::endl;
              continue;
            }

            if (temporaryChange && !lastPeriod)
              sentence << ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE;
            else
              sentence << ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE;

            sentence << getTimePhrase(directionChangePeriod.localStartTime(), useAlkaenPhrase);
            theParameters.theLog << "CASE1:  " << as_string(sentence) << std::endl;
          }
          else
          {
            // after varying wind use this form
            if (thePreviousWindDirection.id == VAIHTELEVA)
            {
              sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
                       << getTimePhrase(directionChangePeriod, useAlkaenPhrase)
                       << windDirectionSentence(windDirectionId);
              theParameters.theLog << "CASE2: " << as_string(sentence) << std::endl;
            }
            else
            {
              // to get some variation in phrases
              if (lastDirectionChangeOnCurrentPeriod)
                sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
                         << getTimePhrase(directionChangePeriod, useAlkaenPhrase)
                         << windDirectionSentence(windDirectionId);
              else
                sentence << ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE
                         << getTimePhrase(directionChangePeriod, false)
                         << getWindDirectionTurntoString(windDirectionId);
              theParameters.theLog << "CASE3: " << as_string(sentence) << std::endl;
            }
          }
        }
        numberOfDirectionChanges++;
        ret.push_back(sentence);
        sentence.clear();
        thePreviousWindDirection = WindDirectionInfo(windDirectionValue, windDirectionId);
      }
      theParameters.theLog << "Reporting " << numberOfDirectionChanges
                           << " direction changes on period " << windSpeedEventPeriod << std::endl;
    }
    break;
    case TUULI_VOIMISTUU:
    case TUULI_HEIKKENEE:
    {
      // get point of times/periods to report wind speed diring sthrenghtening/weakening period
      std::vector<WeatherPeriod> windSpeedReportingPeriods =
          getWindSpeedReportingPeriods(*windSpeedItem, firstSentence, windSpeedEventId);

      if (windSpeedReportingPeriods.size() == 0)
      {
        theParameters.theLog
            << "No wind speed reporting periods found -> reporting whole period at once."
            << std::endl;
        windSpeedReportingPeriods.push_back(windSpeedEventPeriod);
      }

      theParameters.theLog << "Processing "
                           << (windSpeedEventId == TUULI_VOIMISTUU ? "TUULI_VOIMISTUU event at "
                                                                   : "TUULI_HEIKKENEE event at ")
                           << windSpeedEventPeriod << " (" << windSpeedReportingPeriods.size()
                           << " reporting times)" << std::endl;

      bool smallChange = false;
      bool gradualChange = false;
      bool fastChange = false;
      std::string changeAttributeStr = EMPTY_STRING;

      if (!getWindSpeedChangeAttribute(
              windSpeedEventPeriod, changeAttributeStr, smallChange, gradualChange, fastChange))
        return ret;

      Sentence sentence;
      std::string previousTimePhrase;

      // iterate reporting points
      for (unsigned int i = 0; i < windSpeedReportingPeriods.size(); i++)
      {
        WeatherPeriod period = windSpeedReportingPeriods[i];

        theParameters.theLog << "Reporting period " << period << std::endl;

        bool nextWindSpeedEventMissing =
            (nextWindSpeedItem && nextWindSpeedItem->theWindEvent == MISSING_WIND_SPEED_EVENT);

        // if no wind speed is happening on next period report wind speed for that period
        // here too
        if (i == windSpeedReportingPeriods.size() - 1 && nextWindSpeedEventMissing)
        {
          period =
              WeatherPeriod(period.localStartTime(), nextWindSpeedItem->thePeriod.localEndTime());
        }

        Sentence speedIntervalSentence(
            windSpeedIntervalSentence(period, USE_AT_ITS_STRONGEST_PHRASE));

        bool startOfTheStory = (firstSentence && i == 0);

        TextGenPosixTime prePeriodStart = windSpeedEventPeriod.localStartTime();
        TextGenPosixTime prePeriodEnd = period.localStartTime();
        // in the beginning there is no previous period
        if (startOfTheStory)
        {
          prePeriodStart.ChangeByHours(-1);
          prePeriodEnd.ChangeByHours(-1);
        }
        else if (prePeriodStart < prePeriodEnd)
        {
          prePeriodStart.ChangeByHours(1);
        }

        WeatherPeriod prePeriod(prePeriodStart, prePeriodEnd);

        Sentence timePhrase;

        WeatherPeriod speedChangePeriod = period;
        bool addInterval = true;

        if (i == 0)
        {
          if (firstSentence)
            timePhrase << EMPTY_STRING;
          else
          {
            speedChangePeriod =
                WeatherPeriod(windSpeedItem->thePeriod.localStartTime(), period.localEndTime());

            timePhrase << getTimePhrase(speedChangePeriod,
                                        get_period_length(speedChangePeriod) >= 6);

            previousTimePhrase = as_string(timePhrase);
          }

          bool tuuliBasicForm = false;
          if (changeAttributeStr == EMPTY_STRING && thePreviousWindDirection.id == VAIHTELEVA)
          {
            if (wind_speed_differ_enough(theParameters, speedChangePeriod, true))
            {
              if (windSpeedEventId == TUULI_HEIKKENEE)
                sentence << ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI_COMPOSITE_PHRASE;
              else
                sentence << ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI_COMPOSITE_PHRASE;
              sentence << timePhrase;
              tuuliBasicForm = true;
            }
            else
            {
              addInterval = false;
            }
          }
          else
          {
            sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE << timePhrase
                     << changeAttributeStr
                     << (windSpeedEventId == TUULI_HEIKKENEE ? HEIKKENEVAA_WORD : VOIMISTUVAA_WORD);
          }

          previousTimePhrase = as_string(timePhrase);

          if (firstSentence)
          {
            // handle just first period
            WeatherPeriod& directionPeriod = windDirectionChangePeriods[0];
            WindDirectionId windDirectionId =
                get_wind_direction_id_at(theParameters, directionPeriod);
            WeatherResult windDirectionValue =
                get_wind_direction_at(theParameters, directionPeriod);

            Sentence firstSentence;
            sentence << windDirectionSentence(windDirectionId);
            thePreviousWindDirection = WindDirectionInfo(windDirectionValue, windDirectionId);
            windDirectionChangePeriods.assign(windDirectionChangePeriods.begin() + 1,
                                              windDirectionChangePeriods.end());
          }
          else
          {
            part_of_the_day_id speedDayPartId =
                get_part_of_the_day_id(speedChangePeriod.localStartTime());

            // handle special cases
            bool windDirectionAdded = false;
            // if sentence starts 'aamuyolla'/'aamuyosta alkaen' (and the actual period
            // starts
            // couple of hours before), then direction change can not happen at 'keskiyo'

            if (windDirectionChangePeriods.size() > 0)
            {
              const WeatherPeriod& directionPeriod = windDirectionChangePeriods[0];
              if (thePreviousPartOfTheDay == AAMUYO &&
                  (speedDayPartId == ILTAYO || speedDayPartId == KESKIYO))
              {
                part_of_the_day_id directionDayPart =
                    get_part_of_the_day_id(directionPeriod.localStartTime());

                if (directionDayPart == KESKIYO)
                {
                  WindDirectionId windDirectionId =
                      get_wind_direction_id_at(theParameters, directionPeriod);
                  WeatherResult windDirectionValue =
                      get_wind_direction_at(theParameters, directionPeriod);
                  sentence << windDirectionSentence(windDirectionId, tuuliBasicForm);

                  thePreviousWindDirection = WindDirectionInfo(windDirectionValue, windDirectionId);
                  windDirectionChangePeriods.assign(windDirectionChangePeriods.begin() + 1,
                                                    windDirectionChangePeriods.end());
                  windDirectionAdded = true;
                }
              }
              if (!windDirectionAdded)
              {
                // report here direction if part of the day is same as for interval
                part_of_the_day_id intervalPeriodId = get_adjusted_part_of_the_day_id(period);
                part_of_the_day_id directionPeriodId =
                    get_adjusted_part_of_the_day_id(directionPeriod);

                if (intervalPeriodId == directionPeriodId)
                {
                  WindDirectionId windDirectionId =
                      get_wind_direction_id_at(theParameters, directionPeriod);

                  if (windDirectionId != thePreviousWindDirection.id)
                  {
                    WeatherResult windDirectionValue =
                        get_wind_direction_at(theParameters, directionPeriod);

                    if (!addInterval)
                    {
                      timePhrase.clear();
                      timePhrase << getTimePhrase(period, get_period_length(period) >= 6);
                      sentence << timePhrase
                               << windDirectionSentence(windDirectionId, tuuliBasicForm);
                      previousTimePhrase = as_string(timePhrase);
                      timePhrase.clear();
                    }
                    else
                    {
                      sentence << windDirectionSentence(windDirectionId, tuuliBasicForm);
                    }

                    thePreviousWindDirection =
                        WindDirectionInfo(windDirectionValue, windDirectionId);
                    windDirectionChangePeriods.assign(windDirectionChangePeriods.begin() + 1,
                                                      windDirectionChangePeriods.end());
                    windDirectionAdded = true;
                  }
                }
              }
            }  // if(windDirectionChangePeriods.size() > 0)
            if (!windDirectionAdded && !sentence.empty())
            {
              sentence << (tuuliBasicForm ? TUULI_WORD : TUULTA_WORD);
            }
          }
        }  // if (i == 0) // first reporting point

        if (startOfTheStory)
        {
          sentence << Delimiter(COMMA_PUNCTUATION_MARK) << ALUKSI_WORD;
          if (windSpeedEventId == TUULI_HEIKKENEE)
          {
            speedIntervalSentence.clear();
            // take lower limit from start and upper limit from whole peroid, since
            // highest speed is not necessarily in the beginning
            speedIntervalSentence << windSpeedIntervalSentence(
                period, windSpeedEventPeriod, DONT_USE_AT_ITS_STRONGEST_PHRASE);
          }
        }
        else
        {
          // report here if wind direction has changed
          std::vector<Sentence> directionChangeSentences =
              reportDirectionChanges(prePeriod,
                                     windDirectionChangePeriods,
                                     thePreviousWindDirection,
                                     previousTimePhrase,
                                     lastPeriod);
          for (auto s : directionChangeSentences)
          {
            if (!sentence.empty()) sentence << Delimiter(COMMA_PUNCTUATION_MARK);
            sentence << s;
          }
        }

        if (i < windSpeedReportingPeriods.size() - 1 && !startOfTheStory)
        {
          WeatherPeriod nextPeriod = windSpeedReportingPeriods[i + 1];

          // dont report several speed interval on same part of the day
          if (nextPeriod.localStartTime().DifferenceInHours(period.localStartTime()) < 6 &&
              get_part_of_the_day_id(nextPeriod.localStartTime() ==
                                     get_part_of_the_day_id(period.localStartTime())))
          {
            continue;
          }
        }

        if (!startOfTheStory)
        {
          timePhrase.clear();
          timePhrase << getTimePhrase(period, get_period_length(period) >= 6);
        }

        if (addInterval)
        {
          if (!startOfTheStory)
          {
            if (get_period_length(period) > 0 && get_period_length(period) < 6)
            {
              // if direction changes same time as interval, add it here
              std::vector<Sentence> directionChangeSentences =
                  reportDirectionChanges(period,
                                         windDirectionChangePeriods,
                                         thePreviousWindDirection,
                                         previousTimePhrase,
                                         lastPeriod);

              for (auto s : directionChangeSentences)
              {
                if (!sentence.empty()) sentence << Delimiter(COMMA_PUNCTUATION_MARK);
                sentence << s;
              }
            }
            else if (as_string(timePhrase) != previousTimePhrase)
            {
              sentence << Delimiter(COMMA_PUNCTUATION_MARK) << timePhrase;
              previousTimePhrase = as_string(timePhrase);
              timePhrase.clear();
            }
          }

          sentence << speedIntervalSentence;
        }
        timePhrase.clear();
      }  // reporting periods

      if (!sentence.empty())
      {
        theParameters.theLog << "Wind speed change sentence: " << std::endl
                             << as_string(sentence) << std::endl;
        ret.push_back(sentence);
      }

      // in the end report rest of wind direction changes during this wind speed change period
      for (auto period : windDirectionChangePeriods)
      {
        if (get_period_length(period) <= 2) continue;

        Sentence timePhrase;
        timePhrase << getTimePhrase(period, get_period_length(period) >= 6);
        previousTimePhrase = as_string(timePhrase);

        WindDirectionId windDirectionId = get_wind_direction_id_at(theParameters, period);

        if (windDirectionId != thePreviousWindDirection.id)
        {
          WeatherResult windDirectionValue = get_wind_direction_at(theParameters, period);

          Sentence sentence;
          sentence << timePhrase << windDirectionSentence(windDirectionId);
          ret.push_back(sentence);

          thePreviousWindDirection = WindDirectionInfo(windDirectionValue, windDirectionId);
        }
      }
    }
    break;
    default:
      break;
  }

  return ret;
}

Paragraph WindForecast::getWindStory(const WeatherPeriod& thePeriod) const
{
  Paragraph paragraph;
  std::vector<Sentence> sentences;

  WindDirectionInfo previouslyReportedWindDirection;

  std::vector<WeatherPeriod> windDirectionPeriods =
      get_wind_direction_periods(theParameters, thePeriod);

  theParameters.theLog << "*** WIND DIRECTION REPORTING PERIODS ***" << std::endl;

  for (auto period : windDirectionPeriods)
    theParameters.theLog << period << " - "
                         << wind_direction_string(get_wind_direction_id_at(theParameters, period))
                         << std::endl;

  theParameters.theLog << "*** WIND SPEED REPORTING PERIODS ***" << std::endl;

  for (auto item : theParameters.theWindSpeedEventPeriodVector)
    theParameters.theLog << item->thePeriod << " - " << get_wind_event_string(item->theWindEvent)
                         << std::endl;

  // iterate wind speed events and report speed and direction changes
  for (unsigned int i = 0; i < theParameters.theWindSpeedEventPeriodVector.size(); i++)
  {
    bool firstPeriod(i == 0);
    const WindEventPeriodDataItem* windSpeedEventPeriodDataItem =
        theParameters.theWindSpeedEventPeriodVector[i];

    const WindEventPeriodDataItem* nextWindSpeedEventPeriodDataItem =
        (i < theParameters.theWindSpeedEventPeriodVector.size() - 1
             ? theParameters.theWindSpeedEventPeriodVector[i + 1]
             : 0);

    std::vector<Sentence> retval = constructWindSentence(windSpeedEventPeriodDataItem,
                                                         nextWindSpeedEventPeriodDataItem,
                                                         windDirectionPeriods,
                                                         previouslyReportedWindDirection,
                                                         firstPeriod);
    sentences.insert(sentences.end(), retval.begin(), retval.end());
  }

  for (auto s : sentences)
    paragraph << s;

  return paragraph;
}

Sentence WindForecast::getTimePhrase(const TextGenPosixTime& theTime, bool useAlkaenPhrase) const
{
  return getTimePhrase(WeatherPeriod(theTime, theTime), useAlkaenPhrase);
}

Sentence WindForecast::getTimePhrase(const WeatherPeriod& thePeriod, bool useAlkaenPhrase) const
{
  Sentence sentence;

  int forecastPeriodLength = get_period_length(theParameters.theForecastPeriod);
  part_of_the_day_id partOfTheDayLarge(get_part_of_the_day_id_large(thePeriod));

  bool specifyDay =
      ((forecastPeriodLength > 24 &&
        abs(theParameters.theForecastTime.DifferenceInHours(thePeriod.localStartTime())) > 6 &&
        theParameters.theForecastTime.GetWeekday() != thePeriod.localStartTime().GetWeekday()) &&
       ((thePreviousDayNumber != thePeriod.localStartTime().GetWeekday() &&
         partOfTheDayLarge != KESKIYO) ||
        partOfTheDayLarge == YO));

  /*
  bool specifyDay =
      ((abs(theParameters.theForecastTime.DifferenceInHours(thePeriod.localStartTime())) > 6 &&
        theParameters.theForecastTime.GetWeekday() != thePeriod.localStartTime().GetWeekday()) &&
       ((thePreviousDayNumber != thePeriod.localStartTime().GetWeekday() &&
         partOfTheDayLarge != KESKIYO) ||
        partOfTheDayLarge == YO));
  */

  std::string dayPhasePhrase;

  theParameters.theAlkaenPhraseUsed = (get_period_length(thePeriod) >= 6 && useAlkaenPhrase &&
                                       !fit_into_narrow_day_part(thePeriod));

  // try to prevent tautology, like "iltap‰iv‰ll‰"... "iltap‰iv‰st‰ alkaen"
  if (thePreviousPartOfTheDay ==
          get_adjusted_part_of_the_day_id(thePeriod, theParameters.theAlkaenPhraseUsed) &&
      thePreviousPartOfTheDay != MISSING_PART_OF_THE_DAY_ID && get_period_length(thePeriod) > 4)
  {
    TextGenPosixTime startTime(thePeriod.localStartTime());
    startTime.ChangeByHours(2);
    WeatherPeriod shortenedPeriod(startTime, thePeriod.localEndTime());
    theParameters.theAlkaenPhraseUsed =
        (get_period_length(shortenedPeriod) >= 6 && useAlkaenPhrase &&
         !fit_into_narrow_day_part(shortenedPeriod));
    sentence << get_time_phrase_large(
        shortenedPeriod, specifyDay, theParameters.theVar, dayPhasePhrase, useAlkaenPhrase);

    if (get_part_of_the_day_id_large(shortenedPeriod) != KESKIYO)
      thePreviousDayNumber = shortenedPeriod.localStartTime().GetWeekday();

    thePreviousPartOfTheDay =
        get_adjusted_part_of_the_day_id(shortenedPeriod, theParameters.theAlkaenPhraseUsed);
  }
  else
  {
    sentence << get_time_phrase_large(
        thePeriod, specifyDay, theParameters.theVar, dayPhasePhrase, useAlkaenPhrase);

    if (get_part_of_the_day_id_large(thePeriod) != KESKIYO)
      thePreviousDayNumber = thePeriod.localStartTime().GetWeekday();

    thePreviousPartOfTheDay =
        get_adjusted_part_of_the_day_id(thePeriod, theParameters.theAlkaenPhraseUsed);
  }

  return sentence;
}

vector<WeatherPeriod> WindForecast::getWindSpeedReportingPeriods(
    const WindEventPeriodDataItem& eventPeriodDataItem,
    bool firstSentenceInTheStory,
    WindEventId eventId) const
{
  vector<WeatherPeriod> retVector;

  WeatherPeriod speedEventPeriod(eventPeriodDataItem.theWindSpeedChangePeriod);

  theParameters.theLog << "Find out wind speed reporting points/period at period "
                       << speedEventPeriod << std::endl;

  // find the point of time when max wind has changed theParameters.theWindSpeedThreshold from
  // the previous take the period from there to the end of the speedEventPeriod
  unsigned int begIndex = 0;
  unsigned int endIndex = 0;

  do
  {
    const WindDataItemUnit& windDataItem =
        (*theParameters.theWindDataVector[begIndex])(theParameters.theArea.type());
    if (is_inside(windDataItem.thePeriod.localStartTime(), speedEventPeriod) ||
        begIndex == theParameters.theWindDataVector.size() - 1)
      break;
    begIndex++;
  } while (true);

  if (begIndex < theParameters.theWindDataVector.size())
  {
    vector<unsigned int> reportingIndexes;
    float previousTopWind(kFloatMissing);
    TextGenPosixTime previousTime;
    for (unsigned int i = begIndex; i < theParameters.theWindDataVector.size(); i++)
    {
      const WindDataItemUnit& windDataItem =
          (*theParameters.theWindDataVector[i])(theParameters.theArea.type());

      // when outside period break the loop
      if (!is_inside(windDataItem.thePeriod.localStartTime(), speedEventPeriod))
      {
        // we must have at least one reporting point
        if ((firstSentenceInTheStory && reportingIndexes.size() == 1) ||
            reportingIndexes.size() == 0)
        {
          const WindDataItemUnit& previousWindDataItem =
              (*theParameters.theWindDataVector[i - 1])(theParameters.theArea.type());
          reportingIndexes.push_back(i - 1);
          previousTopWind = previousWindDataItem.theEqualizedTopWind.value();
          previousTime = previousWindDataItem.thePeriod.localStartTime();
          theParameters.theLog << "Reporting wind speed (case C) at "
                               << previousWindDataItem.thePeriod << std::endl;
        }
        break;
      }

      // windDataItem.theEqualizedTopWind.value() > WEAK_WIND_SPEED_UPPER_LIMIT &&
      // reportingIndexes.size() == 0
      if (i == begIndex)
      {
        // speed at the beginning of the period is reported
        if (firstSentenceInTheStory)
        {
          reportingIndexes.push_back(i);
          previousTopWind = windDataItem.theEqualizedTopWind.value();
          previousTime = windDataItem.thePeriod.localStartTime();
          theParameters.theLog << "Reporting wind speed (case A) at " << windDataItem.thePeriod
                               << std::endl;
        }
      }
      else
      {
        // add reporting point when speed has changed enough
        if (windDataItem.theEqualizedTopWind.value() > WEAK_WIND_SPEED_UPPER_LIMIT)
        {
          if (previousTopWind == kFloatMissing)
          {
            previousTopWind = windDataItem.theEqualizedTopWind.value();
            previousTime = windDataItem.thePeriod.localStartTime();
            continue;
          }
          WeatherPeriod p(previousTime, windDataItem.thePeriod.localStartTime());
          if (previousTopWind > WEAK_WIND_SPEED_UPPER_LIMIT &&
              wind_speed_differ_enough(theParameters, p))
          {
            reportingIndexes.push_back(i);
            previousTopWind = windDataItem.theEqualizedTopWind.value();
            previousTime = windDataItem.thePeriod.localStartTime();
            theParameters.theLog << "Reporting wind speed (case B) at " << windDataItem.thePeriod
                                 << std::endl;
          }
        }
      }
      endIndex = i;
    }  //

    // last
    const WindDataItemUnit& windDataItemLast =
        (*theParameters.theWindDataVector[endIndex])(theParameters.theArea.type());

    for (unsigned int i = 0; i < reportingIndexes.size(); i++)
    {
      unsigned int index = reportingIndexes[i];
      const WindDataItemUnit& windDataItem =
          (*theParameters.theWindDataVector[index])(theParameters.theArea.type());

      if (reportingIndexes.size() == 1)
      {
        // report wind speed at whole remaining increasing/decreasing period,
        // because there is only one reporting point
        WeatherPeriod remainingPeriod(windDataItem.thePeriod.localStartTime(),
                                      windDataItemLast.thePeriod.localEndTime());
        retVector.push_back(remainingPeriod);
        theParameters.theLog << "Only one reporting point, extend the period to the end "
                             << remainingPeriod << std::endl;
      }
      else if (i < reportingIndexes.size() - 1)
      {
        // report wind speed at certain point of time during increasing/decreasing period
        retVector.push_back(windDataItem.thePeriod);
        theParameters.theLog << "Report speed at certain point of time " << windDataItem.thePeriod
                             << std::endl;
      }
      else
      {
        // handle last reporting period when there are many reporting points:
        // last reporting point is at the end of the increasing/decreasing period
        // except when period ends in the evening or in the morning, in that case
        // last reporting period starts three hours before increasing/decreasing period ends
        // so that last time phrase in e.g 'iltaan asti'-forecast is 'iltap‰iv‰ll‰'
        TextGenPosixTime startTime(windDataItemLast.thePeriod.localStartTime());
        TextGenPosixTime endTime(windDataItemLast.thePeriod.localEndTime());
        if (endTime.GetHour() == 18 || endTime.GetHour() == 17 || endTime.GetHour() == 6)
        {
          startTime.ChangeByHours(-3);
          if (startTime < eventPeriodDataItem.thePeriod.localStartTime())
            startTime = eventPeriodDataItem.thePeriod.localStartTime();
        }
        WeatherPeriod lastReportingPeriod(startTime, endTime);

        retVector.push_back(lastReportingPeriod);

        theParameters.theLog << "Last reporting period, special handling " << lastReportingPeriod
                             << std::endl;
      }
    }
  }

  // check that there is no more than three reporting periods:
  // e.g. aluksi 5-7 m/s, iltap‰iv‰ll‰ 9-13 m/s, illasta alkaen 11-15 m/s, ylimmill‰‰n 19 m/s
  if (retVector.size() > 3)
  {
    TextGenPosixTime lastPeriodStartTime = retVector[2].localStartTime();
    TextGenPosixTime lastPeriodEndTime = retVector.back().localEndTime();
    retVector[2] = WeatherPeriod(lastPeriodStartTime, lastPeriodEndTime);
    while (retVector.size() > 3)
      retVector.pop_back();
  }

  return retVector;
}

// tuuli voi voimistua, heiket‰ tai k‰‰nty‰ usealla per‰kk‰isel‰ periodilla (k‰yr‰ polveilee,
// mutta
// ei k‰‰nny)
// t‰m‰ funktio etsii oikean k‰‰nnˆskohdan
WeatherPeriod getActualWindEventPeriod(
    const wind_event_period_data_item_vector& theWindEventPeriodVector, unsigned int theBegIndex)
{
  WindEventPeriodDataItem* eventPeriodItem = theWindEventPeriodVector[theBegIndex];
  TextGenPosixTime periodStartTime(eventPeriodItem->thePeriod.localStartTime());
  TextGenPosixTime periodEndTime(eventPeriodItem->thePeriod.localStartTime());

  bool firstWindSpeedEvent = (eventPeriodItem->theWindEvent == TUULI_HEIKKENEE ||
                              eventPeriodItem->theWindEvent == TUULI_VOIMISTUU ||
                              eventPeriodItem->theWindEvent == TUULI_TYYNTYY);

  for (unsigned int i = theBegIndex + 1; i < theWindEventPeriodVector.size(); i++)
  {
    eventPeriodItem = theWindEventPeriodVector[i];

    bool currentWindSpeedEvent = (eventPeriodItem->theWindEvent == TUULI_HEIKKENEE ||
                                  eventPeriodItem->theWindEvent == TUULI_VOIMISTUU ||
                                  eventPeriodItem->theWindEvent == TUULI_TYYNTYY);

    if (firstWindSpeedEvent == currentWindSpeedEvent)
    {
      periodEndTime = eventPeriodItem->thePeriod.localStartTime();
      break;
    }
    periodEndTime = eventPeriodItem->thePeriod.localEndTime();
  }

  return WeatherPeriod(periodStartTime, periodEndTime);
}

bool WindForecast::getWindSpeedChangeAttribute(const WeatherPeriod& changePeriod,
                                               std::string& phraseStr,
                                               bool& smallChange,
                                               bool& gradualChange,
                                               bool& fastChange) const
{
  float begLowerLimit(kFloatMissing), begUpperLimit(kFloatMissing), endLowerLimit(kFloatMissing),
      endUpperLimit(kFloatMissing);

  smallChange = gradualChange = fastChange = false;

  get_wind_speed_interval(
      changePeriod.localStartTime(), theParameters, begLowerLimit, begUpperLimit);
  get_wind_speed_interval(changePeriod.localEndTime(), theParameters, endLowerLimit, endUpperLimit);

  if (begLowerLimit == kFloatMissing || begUpperLimit == kFloatMissing ||
      endLowerLimit == kFloatMissing || endUpperLimit == kFloatMissing)
  {
    theParameters.theLog << "Error: failed to get wind speed change parameters!" << endl;
    return false;
  }

  begLowerLimit = round(begLowerLimit);
  begUpperLimit = round(begUpperLimit);
  endLowerLimit = round(endLowerLimit);
  endUpperLimit = round(endUpperLimit);

  float changeThreshold(5.0);
  int periodLength(get_period_length(changePeriod));

  phraseStr = EMPTY_STRING;

  if (abs(endUpperLimit - begUpperLimit) <= changeThreshold * 0.5)
  {
    phraseStr = VAHAN_WORD;
    smallChange = true;
  }
  else if (abs(endUpperLimit - begUpperLimit) >= changeThreshold)
  {
    if (periodLength <= 6 && endUpperLimit >= NAVAKKA_UPPER_LIMIT)
    {
      phraseStr = NOPEASTI_WORD;
      fastChange = true;
    }
    else if (periodLength >= 12)
    {
      phraseStr = VAHITELLEN_WORD;
      gradualChange = true;
    }
  }

  return true;
}

Sentence WindForecast::speedRangeSentence(const WeatherPeriod& thePeriod,
                                          const WeatherResult& theTopSpeed,
                                          const WeatherResult& theMedianSpeed,
                                          const string& theVariable,
                                          bool theUseAtItsStrongestPhrase) const
{
  using Settings::optional_int;

  Sentence sentence;

  float intervalLowerLimit(theMedianSpeed.value());
  float intervalUpperLimit(theTopSpeed.value());

  get_wind_speed_interval(thePeriod, theParameters, intervalLowerLimit, intervalUpperLimit);

  int lowerLimit = round(intervalLowerLimit);
  int upperLimit = round(intervalUpperLimit);

  int peakWind(intervalUpperLimit);

  compose_wind_speed_range(theParameters, lowerLimit, upperLimit, peakWind);

  return compose_wind_speed_sentence(
      theParameters, lowerLimit, upperLimit, peakWind, theUseAtItsStrongestPhrase);
}

Sentence WindForecast::windSpeedIntervalSentence(const WeatherPeriod& thePeriodLowerLimit,
                                                 const WeatherPeriod& thePeriodUpperLimit,
                                                 bool theUseAtItsStrongestPhrase /* = true*/) const
{
  float intervalLowerLimit(-kFloatMissing);
  float intervalUpperLimit(kFloatMissing);

  // get upper limit
  get_wind_speed_interval(
      thePeriodUpperLimit, theParameters, intervalLowerLimit, intervalUpperLimit);

  int upperLimit = round(intervalUpperLimit);
  int peakWind = upperLimit;

  // get lower limit
  get_wind_speed_interval(
      thePeriodLowerLimit, theParameters, intervalLowerLimit, intervalUpperLimit);
  int lowerLimit = round(intervalLowerLimit);

  compose_wind_speed_range(theParameters, lowerLimit, upperLimit, peakWind);

  return compose_wind_speed_sentence(
      theParameters, lowerLimit, upperLimit, peakWind, theUseAtItsStrongestPhrase);
}

Sentence WindForecast::windSpeedIntervalSentence(const WeatherPeriod& thePeriod,
                                                 bool theUseAtItsStrongestPhrase /* = true*/) const
{
  Sentence sentence;

  WeatherResult lowerLimit(kFloatMissing, 0.0);
  WeatherResult upperLimit(kFloatMissing, 0.0);

  sentence << speedRangeSentence(
      thePeriod, upperLimit, lowerLimit, theParameters.theVar, theUseAtItsStrongestPhrase);
  return sentence;
}

float WindDataItemUnit::getWindSpeedShare(float theLowerLimit, float theUpperLimit) const
{
  float retval = 0.0;

  for (unsigned int i = 0; i < theWindSpeedDistribution.size(); i++)
  {
    if (theWindSpeedDistribution[i].first >= theLowerLimit &&
        theWindSpeedDistribution[i].first < theUpperLimit)
      retval += theWindSpeedDistribution[i].second.value();
  }

  return retval;
}

float WindDataItemUnit::getTopWindSpeedShare(float theLowerLimit, float theUpperLimit) const
{
  float retval = 0.0;

  for (unsigned int i = 0; i < theWindSpeedDistributionTop.size(); i++)
  {
    if (theWindSpeedDistributionTop[i].first >= theLowerLimit &&
        theWindSpeedDistributionTop[i].first < theUpperLimit)
      retval += theWindSpeedDistributionTop[i].second.value();
  }

  return retval;
}

float WindDataItemUnit::getWindDirectionShare(
    WindDirectionId windDirectionId,
    double theWindDirectionMinSpeed,
    WindStoryTools::CompassType compass_type /* = sixteen_directions*/) const
{
  return get_wind_direction_share(
      (compass_type == sixteen_directions ? theWindDirectionDistribution16
                                          : theWindDirectionDistribution8),
      windDirectionId,
      theWindDirectionMinSpeed);
}

}  // namespace TextGen
