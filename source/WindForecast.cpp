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

bool same_content(const GlyphContainer& gc1, const GlyphContainer& gc2)
{
  return (as_string(gc1) == as_string(gc2));
}

void print_period_and_text(const string& label, const WeatherPeriod& period, const string& text)
{
  cout << label << period.localStartTime() << "..." << period.localEndTime() << " " << text << endl;
}

int wind_forecast_period_length(const WeatherPeriod& thePeriod)
{
  return get_period_length(thePeriod) + 1;
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

bool wind_speed_differ_enough(const wo_story_params& theParameter, const WeatherPeriod& thePeriod)
{
  float begLowerLimit(0), begUpperLimit(0), endLowerLimit(0), endUpperLimit(0);

  get_wind_speed_interval(thePeriod.localStartTime(), theParameter, begLowerLimit, begUpperLimit);
  get_wind_speed_interval(thePeriod.localEndTime(), theParameter, endLowerLimit, endUpperLimit);

  double topWindWeight = (theParameter.theWindCalcTopShare / 100.0);
  double topMedianWindWeight = 1.0 - topWindWeight;

  float begSpeed = (begUpperLimit * topWindWeight + begLowerLimit * topMedianWindWeight);
  float endSpeed = (endUpperLimit * topWindWeight + endLowerLimit * topMedianWindWeight);

  float difference = endSpeed - begSpeed;

  return (abs(difference) >= theParameter.theWindSpeedThreshold);
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

WindForecast::WindForecast(wo_story_params& parameters)
    : theParameters(parameters),
      thePreviousDayNumber(parameters.theForecastPeriod.localStartTime().GetWeekday()),
      thePreviousPartOfTheDay(MISSING_PART_OF_THE_DAY_ID)
{
}

int WindForecast::windSpeedDifference(const WeatherPeriod& thePeriod1,
                                      const WeatherPeriod& thePeriod2) const
{
  float lowerLim1(0), upperLim1(0), lowerLim2(0), upperLim2(0);

  get_wind_speed_interval(thePeriod1, theParameters, lowerLim1, upperLim1);

  get_wind_speed_interval(thePeriod2, theParameters, lowerLim2, upperLim2);

  return ((upperLim2 - upperLim1) + (lowerLim2 - lowerLim1));
}

Paragraph WindForecast::getWindStory(const WeatherPeriod& thePeriod) const
{
  Paragraph paragraph;

  WindEventId eventIdPreviousReported(MISSING_WIND_EVENT);
  WeatherPeriod weatherPeriodPreviousReported(
      WeatherPeriod(TextGenPosixTime(1971, 1, 1), TextGenPosixTime(1971, 1, 1)));
  WeatherPeriod windSpeedIntervalPeriodPreviousReported(
      WeatherPeriod(TextGenPosixTime(1971, 1, 1), TextGenPosixTime(1971, 1, 1)));

  WindEventPeriodDataItem* eventPeriodItemPrevious(0);
  WindEventPeriodDataItem* eventPeriodItemNext(0);
  WindDirectionId previousWindDirectionId(MISSING_WIND_DIRECTION_ID);
  unsigned int nPeriods = theParameters.theMergedWindEventPeriodVector.size();

  for (unsigned int i = 0; i < nPeriods; i++)
  {
    std::vector<Sentence> sentences;
    Sentence sentence;
    WindEventPeriodDataItem* eventPeriodItem = theParameters.theMergedWindEventPeriodVector[i];

    WindEventId eventId = eventPeriodItem->theWindEvent;
    bool lastPeriod(i == theParameters.theMergedWindEventPeriodVector.size() - 1);
    bool firstSentenceInTheStory = paragraph.empty();
    bool firstPeriod(i == 0);
    bool eventPeriodNextIsMissingEvent(false);
    eventPeriodItemNext = 0;

    // if the first or last period is 1 hour long or wind weakens/strenthens in 1 hour, ignore it
    if ((firstPeriod || lastPeriod || (eventId == TUULI_HEIKKENEE || eventId == TUULI_VOIMISTUU)) &&
        get_period_length(eventPeriodItem->thePeriod) == 0)
      continue;

    if (!lastPeriod)
    {
      eventPeriodItemNext = theParameters.theMergedWindEventPeriodVector[i + 1];
      eventPeriodNextIsMissingEvent = (eventPeriodItemNext->theWindEvent == MISSING_WIND_EVENT);
    }

    theParameters.theAlkaenPhraseUsed = false;

    const WeatherPeriod& eventPeriod = eventPeriodItem->thePeriod;

    WeatherPeriod eventStartPeriod(eventPeriod.localStartTime(), eventPeriod.localStartTime());
    WeatherPeriod eventEndPeriod(eventPeriod.localEndTime(), eventPeriod.localEndTime());

    WindDirectionId windDirectionIdBeg(MISSING_WIND_DIRECTION_ID);
    WindDirectionId windDirectionIdEnd(MISSING_WIND_DIRECTION_ID);

    getWindDirectionBegEnd(eventPeriod, windDirectionIdBeg, windDirectionIdEnd);

    WindDirectionId windDirectionIdAvg(get_wind_direction_id_at(theParameters, eventPeriod));

    if (get_period_length(eventPeriod) == 0) windDirectionIdAvg = windDirectionIdBeg;

    // when varying wind strenghtens, we ignore it
    if (eventId & TUULI_VOIMISTUU && windDirectionIdAvg == VAIHTELEVA)
      eventId = mask_wind_event(eventId, TUULI_VOIMISTUU);

    bool same_part_of_the_day(get_part_of_the_day_id(eventPeriod.localStartTime()) ==
                                  get_part_of_the_day_id(eventPeriod.localEndTime()) &&
                              eventPeriod.localStartTime().GetDay() ==
                                  eventPeriod.localEndTime().GetDay());

    bool windSpeedDifferEnough(false);
    bool windSpeedDifferEnoughOnNextPeriod(false);
    bool windSpeedDifferEnoughOnCombinedPeriodPeriod(false);

    if ((theParameters.theMergedWindEventPeriodVector.size() > 1 &&
         eventId == MISSING_WIND_EVENT) ||
        firstSentenceInTheStory)
    {
      if (eventPeriodItemPrevious &&
          windSpeedIntervalPeriodPreviousReported.localStartTime().GetYear() != 1971)
      {
        WeatherPeriod windSpeedPeriod(eventPeriod);

        if (eventId & TUULI_HEIKKENEE || eventId & TUULI_VOIMISTUU)
          windSpeedPeriod = WeatherPeriod(eventPeriod.localEndTime(), eventPeriod.localEndTime());

        int windSpeedDiff =
            windSpeedDifference(windSpeedIntervalPeriodPreviousReported, windSpeedPeriod);

        windSpeedDifferEnough = abs(windSpeedDiff) >= theParameters.theWindSpeedThreshold;

        if (lastPeriod && eventPeriodItemPrevious->theWindEvent == MISSING_WIND_EVENT &&
            (eventId & TUULI_HEIKKENEE || eventId & TUULI_VOIMISTUU) && !windSpeedDifferEnough)
        {
          continue;
        }
      }
      else
      {
        windSpeedDifferEnough = wind_speed_differ_enough(theParameters, eventPeriod);
      }

      windSpeedDifferEnoughOnNextPeriod =
          (eventPeriodItemNext &&
           wind_speed_differ_enough(theParameters, eventPeriodItemNext->thePeriod));

      windSpeedDifferEnoughOnCombinedPeriodPeriod =
          (eventPeriodItemNext &&
           wind_speed_differ_enough(theParameters,
                                    WeatherPeriod(eventPeriod.localStartTime(),
                                                  eventPeriodItemNext->thePeriod.localEndTime())));
    }
    // default condition for wind speed reporting
    bool reportWindSpeed(firstPeriod || windSpeedDifferEnough);

    if ((eventId & TUULI_HEIKKENEE || eventId & TUULI_VOIMISTUU) &&
        ((eventPeriodItem->theWindSpeedChangeEnds && !windSpeedDifferEnough) ||
         (eventPeriodItemNext && eventPeriodItem->theWindSpeedChangeStarts &&
          eventPeriodItemNext->theWindSpeedChangeEnds &&
          !windSpeedDifferEnoughOnCombinedPeriodPeriod)))
    {
      if (eventId & TUULI_HEIKKENEE)
      {
        if ((eventPeriodItemNext && (eventPeriodItemNext->theWindEvent & TUULI_HEIKKENEE)))
        {
          eventId = mask_wind_event(eventId, TUULI_HEIKKENEE);
          reportWindSpeed = false;
        }
      }
      else if (eventId & TUULI_VOIMISTUU)
      {
        if ((eventPeriodItemNext && (eventPeriodItemNext->theWindEvent & TUULI_VOIMISTUU)))
        {
          eventId = mask_wind_event(eventId, TUULI_VOIMISTUU);
          reportWindSpeed = false;
        }
      }
    }

    // when wind is weak dont report changes in speed and direction, just
    // tell speed and direction for the whole weak period if it is different from the perevious
    if (eventPeriodItem->theWeakWindPeriodFlag)
    {
      if (eventIdPreviousReported & TUULI_MUUTTUU_VAIHTELEVAKSI) continue;
      // if we have short weak period in the middle, ignore it
      float period_len(get_period_length(eventPeriod));
      float forecast_period_len(get_period_length(theParameters.theForecastPeriod));

      if (period_len <= 2 && period_len / forecast_period_len < 0.2 && i > 0 && !firstPeriod &&
          !lastPeriod)
        continue;

      // if previous period event was MISSING_WIND_EVENT and it was not weak (has been reported) ->
      // dont report this period
      if (eventId == MISSING_WIND_EVENT && eventPeriodItemPrevious &&
          eventIdPreviousReported == MISSING_WIND_EVENT &&
          !eventPeriodItemPrevious->theWeakWindPeriodFlag)
        continue;

      // if short weak period is the last period and on previous period wind weakened and
      // part of the day on last period and in the end of previous period are the same, dont report
      // the last period
      // expect when wind was varying in the previous period
      WeatherPeriod previousEndPeriod(weatherPeriodPreviousReported.localEndTime(),
                                      weatherPeriodPreviousReported.localEndTime());

      if (lastPeriod && period_len <= 2 && eventIdPreviousReported & TUULI_HEIKKENEE &&
          get_part_of_the_day_id_narrow(previousEndPeriod) ==
              get_part_of_the_day_id_narrow(eventPeriod) &&
          !(eventIdPreviousReported & TUULI_MUUTTUU_VAIHTELEVAKSI))
        continue;

      eventId = MISSING_WIND_EVENT;
    }
    else if ((eventIdPreviousReported & TUULI_MUUTTUU_VAIHTELEVAKSI) &&
             eventId == MISSING_WIND_EVENT && windDirectionIdAvg == VAIHTELEVA)
    {
      // if previous event was 'tuuli muuttuu vaihtelevaksi',
      // dont report 'tuuli on vaihtelevaa' on current period
      continue;
    }

    // if wind vas vaihteleva on previous period, dont report 'tuuli muuttuu vaihtelevaksi'
    // on this period
    if ((eventId & TUULI_MUUTTUU_VAIHTELEVAKSI) && eventPeriodItemPrevious &&
        get_wind_direction_id_at(theParameters, eventPeriodItemPrevious->thePeriod) == VAIHTELEVA)
      continue;

    // if wind direction changes in two successive periods,
    // it can not be the same in the end of these periods
    if (eventId & TUULI_KAANTYY)
    {
      if (windDirectionIdBeg == windDirectionIdEnd && get_period_length(eventPeriod) > 0)
      {
        eventId = mask_wind_event(eventId, TUULI_KAANTYY);
      }
      else if (eventPeriodItemPrevious && (eventIdPreviousReported & TUULI_KAANTYY))
      {
        if (get_wind_direction_id_at(theParameters, eventPeriodItem->thePeriod.localEndTime()) ==
            get_wind_direction_id_at(theParameters, weatherPeriodPreviousReported.localEndTime()))
          eventId = mask_wind_event(eventId, TUULI_KAANTYY);
      }
    }

    if ((eventId & TUULI_HEIKKENEE) &&
        (eventPeriodItem->theWindSpeedChangeStarts && eventPeriodItem->theWindSpeedChangeEnds))
    {
      if (get_maximum_wind(eventStartPeriod,
                           theParameters.theArea,
                           theParameters.theWindDataVector) < WEAK_WIND_SPEED_UPPER_LIMIT + 1.0)
        eventId = mask_wind_event(eventId, TUULI_HEIKKENEE);
    }

    bool windStrengthStartsToChange(eventPeriodItem->theWindSpeedChangeStarts &&
                                    !eventPeriodItem->theWindSpeedChangeEnds);

    switch (eventId)
    {
      case TUULI_HEIKKENEE:
      case TUULI_VOIMISTUU:
      {
        bool smallChange(false);
        bool gradualChange(false);
        bool fastChange(false);
        std::string changeAttributeStr(EMPTY_STRING);

        if (!getWindSpeedChangeAttribute(
                eventPeriod, changeAttributeStr, smallChange, gradualChange, fastChange))
        {
          return paragraph;
        }

        Sentence directionSentenceP;
        Sentence directionSentence;

        if ((i > 0 && same_direction(previousWindDirectionId, windDirectionIdAvg, true)) ||
            (eventIdPreviousReported == TUULI_MUUTTUU_VAIHTELEVAKSI &&
             windDirectionIdAvg == VAIHTELEVA))
        {
          directionSentenceP << TUULTA_WORD;
          directionSentence << TUULI_WORD;
        }
        else
        {
          directionSentenceP << windDirectionSentence(windDirectionIdAvg);
          directionSentence << windDirectionSentence(windDirectionIdAvg, true);
        }
        bool useAlkaaHeiketaVoimistuaPhrase(false);
        bool appendDecreasingIncreasingInterval(true);

        reportWindSpeed = (eventPeriodItem->theWindSpeedChangeEnds ||
                           (!eventPeriodItem->theWindSpeedChangeEnds && windSpeedDifferEnough &&
                            windSpeedDifferEnoughOnNextPeriod));

        if (firstSentenceInTheStory)
        {
          if (windStrengthStartsToChange)
          {
            if (eventId == TUULI_HEIKKENEE)
              sentence << POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_HEIKETA_COMPOSITE_PHRASE;
            else
              sentence << POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_COMPOSITE_PHRASE;

            sentence << directionSentenceP
                     << windSpeedIntervalSentence(eventStartPeriod,
                                                  DONT_USE_AT_ITS_STRONGEST_PHRASE);
            windSpeedIntervalPeriodPreviousReported = eventStartPeriod;
            appendDecreasingIncreasingInterval = false;
          }
          else
          {
            sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
                     << EMPTY_STRING << changeAttributeStr
                     << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_WORD : VOIMISTUVAA_WORD)
                     << directionSentenceP;
            /*
if (windSpeedDifferEnough)
sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
         << EMPTY_STRING << changeAttributeStr
         << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_WORD : VOIMISTUVAA_WORD)
         << directionSentenceP;
else
sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE << EMPTY_STRING
         << directionSentenceP;
            */

            windSpeedIntervalPeriodPreviousReported = eventPeriod;
            appendDecreasingIncreasingInterval = true;
          }
        }
        else
        {
          useAlkaaHeiketaVoimistuaPhrase =
              (!smallChange && wind_forecast_period_length(eventPeriod) > 6 &&
               !same_part_of_the_day);
          bool useAlkaenPhrase(get_period_length(eventPeriod) >= 6);
          if (useAlkaaHeiketaVoimistuaPhrase)
          {
            if ((eventPeriodItemPrevious &&
                 eventPeriodItemPrevious->theWindEvent != MISSING_WIND_EVENT) &&
                ((eventIdPreviousReported & TUULI_VOIMISTUU && eventId == TUULI_VOIMISTUU) ||
                 (eventIdPreviousReported & TUULI_HEIKKENEE && eventId == TUULI_HEIKKENEE)))
            {
              sentence << ILTAPAIVALLA_EDELLEEN_HEIKKENEVAA_POHJOISTUULTA
                       << getTimePhrase(eventPeriod, USE_ALKAEN_PHRASE)
                       << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_WORD : VOIMISTUVAA_WORD)
                       << directionSentenceP;
            }
            else
            {
              if (windStrengthStartsToChange)
              {
                sentence
                    << (eventId == TUULI_HEIKKENEE
                            ? ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI_COMPOSITE_PHRASE
                            : ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI_COMPOSITE_PHRASE)
                    << getTimePhrase(eventPeriod, useAlkaenPhrase) << directionSentence
                    << changeAttributeStr;

                appendDecreasingIncreasingInterval = false;
              }
              else
              {
                sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
                         << getTimePhrase(eventPeriod, useAlkaenPhrase) << changeAttributeStr
                         << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_WORD : VOIMISTUVAA_WORD)
                         << directionSentenceP;
              }
            }
          }
          else
          {
            if ((eventPeriodItemPrevious &&
                 eventPeriodItemPrevious->theWindEvent != MISSING_WIND_EVENT) &&
                ((eventIdPreviousReported & TUULI_VOIMISTUU && eventId == TUULI_VOIMISTUU) ||
                 (eventIdPreviousReported & TUULI_HEIKKENEE && eventId == TUULI_HEIKKENEE)))
            {
              sentence << ILTAPAIVALLA_EDELLEEN_HEIKKENEVAA_POHJOISTUULTA
                       << getTimePhrase(eventPeriod, useAlkaenPhrase)
                       << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_WORD : VOIMISTUVAA_WORD)
                       << directionSentenceP;
            }
            else
            {
              if (windStrengthStartsToChange)
              {
                sentence
                    << (eventId == TUULI_HEIKKENEE
                            ? ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI_COMPOSITE_PHRASE
                            : ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI_COMPOSITE_PHRASE)
                    << getTimePhrase(eventPeriod, useAlkaenPhrase) << directionSentence
                    << EMPTY_STRING;

                appendDecreasingIncreasingInterval = false;
              }
              else
              {
                sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
                         << getTimePhrase(eventPeriod, useAlkaenPhrase) << changeAttributeStr
                         << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_WORD : VOIMISTUVAA_WORD)
                         << directionSentenceP;
              }
            }
          }
        }

        if (appendDecreasingIncreasingInterval && reportWindSpeed)
        {
          sentence << decreasingIncreasingInterval(
              *eventPeriodItem,
              (eventPeriodNextIsMissingEvent ? eventPeriodItemNext : NULL),
              firstSentenceInTheStory,
              eventId);

          windSpeedIntervalPeriodPreviousReported =
              WeatherPeriod(eventPeriod.localEndTime(), eventPeriod.localEndTime());
        }

        previousWindDirectionId = windDirectionIdAvg;
      }
      break;
      case TUULI_TYYNTYY:
      {
        sentence << TUULI_TYYNTYY_PHRASE;
      }
      break;
      case TUULI_KAANTYY:
      {
        if (firstSentenceInTheStory)
        {
          int periodLengthAfterDirectionChange(get_period_length(eventPeriod) + 1);
          if (eventPeriodItemNext && eventPeriodItemNext->theWindEvent == MISSING_WIND_EVENT)
            periodLengthAfterDirectionChange +=
                get_period_length(eventPeriodItemNext->thePeriod) + 1;
          bool useAlkaenPhrase(periodLengthAfterDirectionChange >= 5);

          if (windDirectionIdBeg == VAIHTELEVA || windDirectionIdBeg == MISSING_WIND_DIRECTION_ID)
          {
            sentence << windDirectionSentence(windDirectionIdAvg);
            sentence << windSpeedIntervalSentence(eventPeriod, USE_AT_ITS_STRONGEST_PHRASE);
            previousWindDirectionId = windDirectionIdAvg;
          }
          else
          {
            sentence
                << POHJOISTUULTA_INTERVALLI_MS_JOKA_KAANTYY_ILTAPAIVALLA_ETELAAN_COMPOSITE_PHRASE
                << windDirectionSentence(windDirectionIdBeg)
                << windSpeedIntervalSentence(eventPeriod, DONT_USE_AT_ITS_STRONGEST_PHRASE)
                << getTimePhrase(eventPeriod, useAlkaenPhrase)
                << getWindDirectionTurntoString(windDirectionIdEnd);
            previousWindDirectionId = windDirectionIdEnd;
          }
          windSpeedIntervalPeriodPreviousReported = eventPeriod;
        }
        else
        {
          sentence << windDirectionChangeSentence(theParameters,
                                                  eventPeriod,
                                                  eventIdPreviousReported,
                                                  previousWindDirectionId,
                                                  eventPeriodItemNext);

          if (lastPeriod && eventIdPreviousReported == TUULI_MUUTTUU_VAIHTELEVAKSI)
            sentence << windSpeedIntervalSentence(eventPeriod, USE_AT_ITS_STRONGEST_PHRASE);
        }
      }
      break;
      case TUULI_MUUTTUU_VAIHTELEVAKSI:
      {
        WeatherPeriod actualEventPeriod(eventPeriod);
        // report together
        if (eventPeriodItemNext && eventPeriodNextIsMissingEvent &&
            get_wind_direction_id_at(theParameters, eventPeriodItemNext->thePeriod) == VAIHTELEVA)
          actualEventPeriod = WeatherPeriod(eventPeriod.localStartTime(),
                                            eventPeriodItemNext->thePeriod.localEndTime());

        if (firstSentenceInTheStory)
        {
          sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE << EMPTY_STRING
                   << windDirectionSentence(VAIHTELEVA)
                   << windSpeedIntervalSentence(actualEventPeriod, USE_AT_ITS_STRONGEST_PHRASE);
        }
        else
        {
          if (eventPeriodItem->theTransientDirectionChangeFlag)
          {
            sentence << ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE
                     << getTimePhrase(eventStartPeriod, DONT_USE_ALKAEN_PHRASE);
          }
          else
          {
            if (get_period_length(actualEventPeriod) >= 6)
            {
              sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
                       << getTimePhrase(eventStartPeriod, USE_ALKAEN_PHRASE)
                       << windDirectionSentence(VAIHTELEVA);
            }
            else
            {
              sentence << ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
                       << getTimePhrase(eventStartPeriod, DONT_USE_ALKAEN_PHRASE);
            }
          }
        }
        previousWindDirectionId = VAIHTELEVA;
      }
      break;
      case TUULI_KAANTYY_JA_VOIMISTUU:
      case TUULI_KAANTYY_JA_HEIKKENEE:
      {
        bool appendDecreasingIncreasingInterval(true);

        sentences = windDirectionAndSpeedChangesSentence(theParameters,
                                                         eventPeriod,
                                                         firstSentenceInTheStory,
                                                         eventIdPreviousReported,
                                                         eventId,
                                                         previousWindDirectionId,
                                                         eventPeriodItem->theWindSpeedChangeStarts,
                                                         eventPeriodItem->theWindSpeedChangeEnds,
                                                         windSpeedIntervalPeriodPreviousReported,
                                                         appendDecreasingIncreasingInterval);

        if (!sentences.empty() && appendDecreasingIncreasingInterval)
        {
          reportWindSpeed = (eventPeriodItem->theWindSpeedChangeEnds ||
                             (!eventPeriodItem->theWindSpeedChangeEnds && windSpeedDifferEnough &&
                              windSpeedDifferEnoughOnNextPeriod));
          if (reportWindSpeed)
          {
            // append to the end of last sentence
            sentences.back() << decreasingIncreasingInterval(
                *eventPeriodItem,
                (eventPeriodNextIsMissingEvent ? eventPeriodItemNext : NULL),
                firstSentenceInTheStory,
                eventId);

            windSpeedIntervalPeriodPreviousReported =
                WeatherPeriod(eventPeriod.localEndTime(), eventPeriod.localEndTime());
          }
        }
      }
      break;
      case TUULI_KAANTYY_JA_TYYNTYY:
      {
        // cout << "TUULI_KAANTYY_JA_TYYNTYY NOT ALLOWED!!" << endl;
      }
      break;
      case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE:
      {
        reportWindSpeed = ((eventPeriodItem->theWindSpeedChangeEnds && windSpeedDifferEnough) ||
                           (!eventPeriodItem->theWindSpeedChangeEnds && windSpeedDifferEnough &&
                            windSpeedDifferEnoughOnNextPeriod));

        if (eventIdPreviousReported & TUULI_HEIKKENEE)
        {
          if (eventPeriodItem->theTransientDirectionChangeFlag)
          {
            sentence
                << ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE
                << getTimePhrase(eventStartPeriod, DONT_USE_ALKAEN_PHRASE);
          }
          else
          {
            sentence
                << ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE
                << getTimePhrase(eventStartPeriod, DONT_USE_ALKAEN_PHRASE);
          }
        }
        else
        {
          if (firstSentenceInTheStory)
          {
            if (eventPeriodItem->theTransientDirectionChangeFlag)
            {
              if (windStrengthStartsToChange)
                sentence
                    << POHJOISTUULI_ALKAA_HEIKETA_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE
                    << windDirectionSentence(windDirectionIdBeg, true);
              else
                sentence
                    << POHJOISTUULI_HEIKKENEE_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE
                    << windDirectionSentence(windDirectionIdBeg, true);
            }
            else
            {
              if (windStrengthStartsToChange)
                sentence << POHJOISTUULI_ALKAA_HEIKETA_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
                         << windDirectionSentence(windDirectionIdBeg, true);
              else
                sentence << POHJOISTUULI_HEIKKENEE_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
                         << windDirectionSentence(windDirectionIdBeg, true);
            }
          }
          else
          {
            if (eventPeriodItem->theTransientDirectionChangeFlag)
            {
              if (windStrengthStartsToChange)
                sentence
                    << ILTAPAIVALLA_TUULI_ALKAA_HEIKETA_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE;
              else
                sentence
                    << ILTAPAIVALLA_TUULI_HEIKKENEE_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE;
            }
            else
            {
              if (windStrengthStartsToChange)
                sentence
                    << ILTAPAIVALLA_TUULI_ALKAA_HEIKETA_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE;
              else
                sentence << ILTAPAIVALLA_TUULI_HEIKKENEE_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE;
            }
            sentence << getTimePhrase(eventStartPeriod, !firstSentenceInTheStory);
          }
        }

        if (reportWindSpeed)
        {
          sentence << decreasingIncreasingInterval(
              *eventPeriodItem,
              (eventPeriodNextIsMissingEvent ? eventPeriodItemNext : NULL),
              firstSentenceInTheStory,
              eventId);

          WeatherPeriod windSpeedPeriod(
              eventPeriod.localEndTime(),
              (eventPeriodNextIsMissingEvent ? eventPeriodItemNext->thePeriod.localEndTime()
                                             : eventPeriod.localEndTime()));

          windSpeedIntervalPeriodPreviousReported = windSpeedPeriod;
        }
        previousWindDirectionId = VAIHTELEVA;
      }
      break;
      case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU:
      {
        reportWindSpeed = (eventPeriodItem->theWindSpeedChangeEnds ||
                           (!eventPeriodItem->theWindSpeedChangeEnds && windSpeedDifferEnough &&
                            windSpeedDifferEnoughOnNextPeriod));
        if (eventIdPreviousReported == TUULI_VOIMISTUU)
        {
          if (eventPeriodItem->theTransientDirectionChangeFlag)
          {
            sentence
                << ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_JA_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE
                << getTimePhrase(eventStartPeriod, !firstSentenceInTheStory);
          }
          else
          {
            sentence
                << ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE
                << getTimePhrase(eventStartPeriod, !firstSentenceInTheStory);
          }
        }
        else
        {
          if (firstSentenceInTheStory)
          {
            if (eventPeriodItem->theTransientDirectionChangeFlag)
            {
              if (windStrengthStartsToChange)
                sentence
                    << POHJOISTUULI_ALKAA_VOIMISTUA_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE
                    << windDirectionSentence(windDirectionIdBeg, true);
              else
                sentence
                    << POHJOISTUULI_VOIMISTUU_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE
                    << windDirectionSentence(windDirectionIdBeg, true);
            }
            else
            {
              if (windStrengthStartsToChange)
                sentence << POHJOISTUULI_ALKAA_VOIMISTUA_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
                         << windDirectionSentence(windDirectionIdBeg, true);
              else
                sentence << POHJOISTUULI_VOIMISTUU_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
                         << windDirectionSentence(windDirectionIdBeg, true);
            }
          }
          else
          {
            if (eventPeriodItem->theTransientDirectionChangeFlag)
            {
              if (windStrengthStartsToChange)
                sentence
                    << ILTAPAIVALLA_TUULI_ALKAA_VOIMISTUA_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE;
              else
                sentence
                    << ILTAPAIVALLA_TUULI_VOIMISTUU_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE;
            }
            else
            {
              if (windStrengthStartsToChange)
                sentence
                    << ILTAPAIVALLA_TUULI_ALKAA_VOIMISTUA_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE;
              else
                sentence << ILTAPAIVALLA_TUULI_VOIMISTUU_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE;
            }
            sentence << getTimePhrase(eventStartPeriod, !firstSentenceInTheStory);
          }
        }

        if (reportWindSpeed)
        {
          sentence << decreasingIncreasingInterval(
              *eventPeriodItem,
              (eventPeriodNextIsMissingEvent ? eventPeriodItemNext : NULL),
              firstSentenceInTheStory,
              eventId);
          windSpeedIntervalPeriodPreviousReported =
              WeatherPeriod(eventPeriod.localEndTime(), eventPeriod.localEndTime());
        }
        previousWindDirectionId = VAIHTELEVA;
      }
      break;
      case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY:
      {
        // cout << "TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY NOT ALLOWED!!" << endl;
      }
      break;
      case MISSING_WIND_EVENT:
      {
        if (firstSentenceInTheStory)
        {
          if (is_gusty_wind(theParameters, eventPeriod, theParameters.theVar))
            sentence << PUUSKITTAISTA_ETELATUULTA_COMPOSITE_PHRASE;

          sentence << windDirectionSentence(windDirectionIdAvg);
          sentence << windSpeedIntervalSentence(eventPeriod, USE_AT_ITS_STRONGEST_PHRASE);
          windSpeedIntervalPeriodPreviousReported = eventPeriod;
        }
        else if (eventPeriodItem->theWeakWindPeriodFlag ||
                 eventIdPreviousReported & TUULI_MUUTTUU_VAIHTELEVAKSI)
        {
          bool useAlkaenPhrase(get_period_length(eventPeriod) >= 6);

          if (previousWindDirectionId != windDirectionIdAvg || windSpeedDifferEnough)
          {
            sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
                     << getTimePhrase(eventPeriod, useAlkaenPhrase)
                     << windDirectionSentence(windDirectionIdAvg);

            if (windSpeedDifferEnough)  // || windDirectionIdAvg & VAIHTELEVA)
            {
              sentence << windSpeedIntervalSentence(eventPeriod, USE_AT_ITS_STRONGEST_PHRASE);
              windSpeedIntervalPeriodPreviousReported = eventPeriod;
            }
          }
        }
        else if ((eventIdPreviousReported & TUULI_MUUTTUU_VAIHTELEVAKSI) &&
                 i < theParameters.theMergedWindEventPeriodVector.size() - 1 &&
                 get_period_length(eventPeriod) > 2)
        {
          sentence << windDirectionChangeSentence(theParameters,
                                                  eventPeriod,
                                                  eventIdPreviousReported,
                                                  previousWindDirectionId,
                                                  eventPeriodItemNext);
        }

        if (!sentence.empty()) previousWindDirectionId = windDirectionIdAvg;
      }
      break;
      default:
        break;
    };

    if (sentences.empty()) sentences.push_back(sentence);

    if (!sentences.front().empty())
    {
      eventIdPreviousReported = eventId;
      weatherPeriodPreviousReported = eventPeriod;
      eventPeriodItemPrevious = eventPeriodItem;
    }

    for (auto s : sentences)
      paragraph << s;
  }  // for

  return paragraph;
}

Sentence WindForecast::windDirectionChangeSentence(
    const wo_story_params& theParameters,
    const WeatherPeriod& eventPeriod,
    WindEventId eventIdPreviousReported,
    WindDirectionId& previousWindDirectionId,
    const WindEventPeriodDataItem* eventPeriodItemNext) const
{
  Sentence sentence;

  int periodLengthAfterDirectionChange(get_period_length(eventPeriod) + 1);
  if (eventPeriodItemNext && eventPeriodItemNext->theWindEvent == MISSING_WIND_EVENT)
    periodLengthAfterDirectionChange += get_period_length(eventPeriodItemNext->thePeriod) + 1;
  bool useAlkaenPhrase(periodLengthAfterDirectionChange >= 5);
  WindDirectionId windDirectionIdBeg(MISSING_WIND_DIRECTION_ID);
  WindDirectionId windDirectionIdEnd(MISSING_WIND_DIRECTION_ID);

  getWindDirectionBegEnd(eventPeriod, windDirectionIdBeg, windDirectionIdEnd);

  if (previousWindDirectionId == VAIHTELEVA ||
      eventIdPreviousReported & TUULI_MUUTTUU_VAIHTELEVAKSI)
  {
    WindDirectionId windDirectionIdAvg(get_wind_direction_id_at(theParameters, eventPeriod));

    if (useAlkaenPhrase)
      sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
               << getTimePhrase(
                      WeatherPeriod(eventPeriod.localStartTime(), eventPeriod.localStartTime()),
                      USE_ALKAEN_PHRASE)
               << windDirectionSentence(windDirectionIdAvg);
    else
      sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
               << getTimePhrase(eventPeriod, DONT_USE_ALKAEN_PHRASE)
               << windDirectionSentence(windDirectionIdAvg);
    previousWindDirectionId = windDirectionIdAvg;
  }
  else if (windDirectionIdEnd != previousWindDirectionId)
  {
    // etsit‰‰n se ajankohta, jolloin tuuli on k‰‰ntynyt lopulliseen suuntaansa,
    // ja raportoidaan, ett‰ senn suuntaista tuulta
    TextGenPosixTime windDirectionChangeTime =
        get_wind_direction_change_time(theParameters, eventPeriod);

    TextGenPosixTime periodEndTime = eventPeriod.localEndTime();
    if (eventPeriodItemNext && eventPeriodItemNext->theWindEvent == MISSING_WIND_EVENT)
      periodEndTime = eventPeriodItemNext->thePeriod.localEndTime();

    useAlkaenPhrase = periodEndTime.DifferenceInHours(windDirectionChangeTime) >= 5;

    sentence << ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE
             << getTimePhrase(WeatherPeriod(windDirectionChangeTime, windDirectionChangeTime),
                              useAlkaenPhrase)
             << getWindDirectionTurntoString(windDirectionIdEnd);

    previousWindDirectionId = windDirectionIdEnd;
  }

  return sentence;
}

std::vector<Sentence> WindForecast::windDirectionAndSpeedChangesSentence(
    const wo_story_params& theParameters,
    const WeatherPeriod& eventPeriod,
    bool firstSentenceInTheStory,
    WindEventId previousWindEventId,
    WindEventId currentWindEventId,
    WindDirectionId& previousWindDirectionId,
    bool theWindSpeedChangeStarts,
    bool theWindSpeedChangeEnds,
    WeatherPeriod& windSpeedIntervalPeriodPreviousReported,
    bool& appendDecreasingIncreasingInterval) const
{
  std::vector<Sentence> sentences;
  Sentence sentence;

  WindDirectionId windDirectionIdBeg(MISSING_WIND_DIRECTION_ID);
  WindDirectionId windDirectionIdEnd(MISSING_WIND_DIRECTION_ID);
  getWindDirectionBegEnd(eventPeriod, windDirectionIdBeg, windDirectionIdEnd);
  WindDirectionId windDirectionIdAvg(get_wind_direction_id_at(theParameters, eventPeriod));

  bool smallChange(false);
  bool gradualChange(false);
  bool fastChange(false);
  std::string changeAttributeStr(EMPTY_STRING);
  bool windSpeedChangeAttribute(getWindSpeedChangeAttribute(
      eventPeriod, changeAttributeStr, smallChange, gradualChange, fastChange));

  if (!windSpeedChangeAttribute)
  {
    sentences.push_back(sentence);
    appendDecreasingIncreasingInterval = false;
    return sentences;
  }

  bool useAlkaenPhrase(get_period_length(eventPeriod) >= 5);
  bool strengtheningWind = (currentWindEventId & TUULI_VOIMISTUU);

  WeatherPeriod eventStartPeriod(eventPeriod.localStartTime(), eventPeriod.localStartTime());

  if (previousWindEventId & TUULI_VOIMISTUU ||
      previousWindEventId & TUULI_HEIKKENEE)  // indicates that this is not first period
  {
    if (previousWindDirectionId == VAIHTELEVA)
    {
      if (theWindSpeedChangeStarts && !theWindSpeedChangeEnds)
      {
        sentence << (currentWindEventId == TUULI_HEIKKENEE
                         ? ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI_COMPOSITE_PHRASE
                         : ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI_COMPOSITE_PHRASE)
                 << getTimePhrase(eventPeriod, useAlkaenPhrase)
                 << windDirectionSentence(windDirectionIdAvg, true)
                 << (smallChange ? EMPTY_STRING : changeAttributeStr);

        appendDecreasingIncreasingInterval = false;
        previousWindDirectionId = windDirectionIdAvg;
      }
      else
      {
        if ((previousWindEventId & TUULI_HEIKKENEE && currentWindEventId & TUULI_HEIKKENEE) ||
            (previousWindEventId & TUULI_VOIMISTUU && currentWindEventId & TUULI_VOIMISTUU))
        {
          sentence << ILTAPAIVALLA_EDELLEEN_HEIKKENEVAA_POHJOISTUULTA
                   << getTimePhrase(eventPeriod, useAlkaenPhrase)
                   << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
                   << windDirectionSentence(windDirectionIdAvg);

          previousWindDirectionId = windDirectionIdAvg;
        }
        else
        {
          if (strengtheningWind)
            sentence << ILTAPAIVALLA_ETELATUULI_VOIMISTUU_NOPEASTI_COMPOSITE_PHRASE;
          else
            sentence << ILTAPAIVALLA_ETELATUULI_HEIKKENEE_NOPEASTI_COMPOSITE_PHRASE;
          sentence << getTimePhrase(eventPeriod, useAlkaenPhrase)
                   << windDirectionSentence(windDirectionIdEnd, true) << changeAttributeStr;

          previousWindDirectionId = windDirectionIdEnd;
        }
      }
    }
    else
    {
      if (previousWindDirectionId == windDirectionIdEnd)
      {
        if (previousWindEventId & TUULI_VOIMISTUU)
        {
          if (currentWindEventId & TUULI_VOIMISTUU)
          {
            sentence << ILTAPAIVALLA_POHJOISTUULI_VOIMISTUU_EDELLEEN
                     << getTimePhrase(eventPeriod, useAlkaenPhrase) << TUULI_WORD;
          }
          else
          {
            if (theWindSpeedChangeStarts && !theWindSpeedChangeEnds)
            {
              sentence << ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI_COMPOSITE_PHRASE
                       << getTimePhrase(eventPeriod, useAlkaenPhrase) << TUULI_WORD
                       << (smallChange ? EMPTY_STRING : changeAttributeStr);
              appendDecreasingIncreasingInterval = false;
            }
            else
            {
              sentence << ILTAPAIVALLA_ETELATUULI_HEIKKENEE_NOPEASTI_COMPOSITE_PHRASE
                       << getTimePhrase(eventPeriod, useAlkaenPhrase) << TUULI_WORD
                       << changeAttributeStr;
            }
          }
        }
        else
        {
          if (currentWindEventId & TUULI_HEIKKENEE)
          {
            sentence << ILTAPAIVALLA_POHJOISTUULI_HEIKKENEE_EDELLEEN
                     << getTimePhrase(eventPeriod, useAlkaenPhrase) << TUULI_WORD;
          }
          else
          {
            if (theWindSpeedChangeStarts && !theWindSpeedChangeEnds)
            {
              sentence << ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI_COMPOSITE_PHRASE
                       << getTimePhrase(eventPeriod, useAlkaenPhrase) << TUULI_WORD
                       << (smallChange ? EMPTY_STRING : changeAttributeStr);
              appendDecreasingIncreasingInterval = false;
            }
            else
            {
              sentence << ILTAPAIVALLA_ETELATUULI_VOIMISTUU_NOPEASTI_COMPOSITE_PHRASE
                       << getTimePhrase(eventPeriod, useAlkaenPhrase) << TUULI_WORD
                       << changeAttributeStr;
            }
          }
        }
      }
      else
      {
        if (previousWindEventId & TUULI_VOIMISTUU)
        {
          if (currentWindEventId & TUULI_VOIMISTUU)
          {
            sentence << ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_JA_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE
                     << getTimePhrase(eventPeriod, useAlkaenPhrase)
                     << getWindDirectionTurntoString(windDirectionIdEnd);
          }
          else
          {
            Sentence timePhraseAtWindDirectionChange;
            /*
TextGenPosixTime directionChangeTime =
    get_wind_direction_change_time(theParameters, eventPeriod, true);
            */

            if (theWindSpeedChangeStarts && !theWindSpeedChangeEnds)
            {
              sentence
                  << ILTAPAIVALLA_POHJOISTUULI_ALKAA_HEIKETA_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE
                  << getTimePhrase(eventPeriod, useAlkaenPhrase)
                  << windDirectionSentence(windDirectionIdBeg, true)
                  << getWindDirectionTurntoString(windDirectionIdEnd);
              appendDecreasingIncreasingInterval = false;
            }
            else
            {
              bool speedDifferEnough = wind_speed_differ_enough(theParameters, eventPeriod);
              if (!speedDifferEnough)
                sentence << ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE
                         << getTimePhrase(eventPeriod, useAlkaenPhrase)
                         << getWindDirectionTurntoString(windDirectionIdEnd);
              else
                sentence << ILTAPAIVALLA_POHJOISTUULI_HEIKKENEE_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE
                         << getTimePhrase(eventPeriod, useAlkaenPhrase)
                         << windDirectionSentence(windDirectionIdBeg, true)
                         << getWindDirectionTurntoString(windDirectionIdEnd);

              appendDecreasingIncreasingInterval = speedDifferEnough;
            }
          }
        }
        else
        {
          if (currentWindEventId & TUULI_HEIKKENEE)
          {
            sentence << ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE
                     << getTimePhrase(eventPeriod, useAlkaenPhrase)
                     << getWindDirectionTurntoString(windDirectionIdEnd);
          }
          else
          {
            if (theWindSpeedChangeStarts && !theWindSpeedChangeEnds)
            {
              sentence << ILTAPAIVALLA_TUULI_ALKAA_VOIMISTUA_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE
                       << getTimePhrase(eventPeriod, useAlkaenPhrase)
                       << getWindDirectionTurntoString(windDirectionIdEnd);
              appendDecreasingIncreasingInterval = false;
            }
            else
            {
              sentence << ILTAPAIVALLA_TUULI_VOIMISTUU_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE
                       << getTimePhrase(eventPeriod, useAlkaenPhrase)
                       << getWindDirectionTurntoString(windDirectionIdEnd);
            }
          }
        }
      }
      previousWindDirectionId = windDirectionIdEnd;
    }
  }
  else
  {
    Sentence timePhrase;
    if (firstSentenceInTheStory)
      timePhrase << EMPTY_STRING;
    else
      timePhrase << getTimePhrase(eventPeriod, useAlkaenPhrase);

    TextGenPosixTime begTimePlus1(eventPeriod.localStartTime());
    if (get_period_length(eventPeriod) > 0) begTimePlus1.ChangeByHours(1);
    WindDirectionId windDirectionIdBegPlus1(get_wind_direction_id_at(theParameters, begTimePlus1));
    WindDirectionId windDirectionIdAvgPlus1(get_wind_direction_id_at(
        theParameters, WeatherPeriod(begTimePlus1, eventPeriod.localEndTime())));
    if (firstSentenceInTheStory)
    {
      if (windDirectionIdBeg == VAIHTELEVA || windDirectionIdBeg == MISSING_WIND_DIRECTION_ID)
      {
        bool directionTurns =
            windDirectionTurns(WeatherPeriod(begTimePlus1, eventPeriod.localEndTime()));
        if (windDirectionIdBegPlus1 == windDirectionIdEnd || !directionTurns)
        {
          if (theWindSpeedChangeStarts && !theWindSpeedChangeEnds)
          {
            if (strengtheningWind)
              sentence << ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI_COMPOSITE_PHRASE;
            else
            {
              sentence << ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI_COMPOSITE_PHRASE
                       << timePhrase << windDirectionSentence(windDirectionIdAvgPlus1)
                       << (smallChange ? EMPTY_STRING : changeAttributeStr);
            }
          }
          else
          {
            if (strengtheningWind)
            {
              Sentence sentence1;
              sentence1 << POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_COMPOSITE_PHRASE
                        << windDirectionSentence(windDirectionIdBegPlus1)
                        << windSpeedIntervalSentence(WeatherPeriod(eventPeriod.localStartTime(),
                                                                   eventPeriod.localStartTime()),
                                                     DONT_USE_AT_ITS_STRONGEST_PHRASE);

              TextGenPosixTime directionChangeTime =
                  get_wind_direction_change_time(theParameters, eventPeriod, true);

              WindDirectionId windDirectionIdAfterChange(
                  get_wind_direction_id_at(theParameters, directionChangeTime));

              useAlkaenPhrase =
                  eventPeriod.localEndTime().DifferenceInHours(directionChangeTime) >= 5;

              Sentence sentence2;
              sentence2 << ILTAPAIVALLA_EDELLEEN_HEIKKENEVAA_POHJOISTUULTA
                        << getTimePhrase(WeatherPeriod(directionChangeTime, directionChangeTime),
                                         useAlkaenPhrase)
                        << VOIMISTUVAA_WORD << windDirectionSentence(windDirectionIdAfterChange)
                        << windSpeedIntervalSentence(
                               WeatherPeriod(directionChangeTime, directionChangeTime),
                               DONT_USE_AT_ITS_STRONGEST_PHRASE)
                        << Delimiter(COMMA_PUNCTUATION_MARK)
                        << getTimePhrase(WeatherPeriod(eventPeriod.localEndTime(),
                                                       eventPeriod.localEndTime()),
                                         DONT_USE_ALKAEN_PHRASE)
                        << windSpeedIntervalSentence(WeatherPeriod(eventPeriod.localEndTime(),
                                                                   eventPeriod.localEndTime()),
                                                     USE_AT_ITS_STRONGEST_PHRASE);

              // TODO: ReportingPoints

              appendDecreasingIncreasingInterval = false;

              sentences.push_back(sentence1);
              sentences.push_back(sentence2);

              /*
                                sentence <<
                 ILTAPAIVALLA_ETELATUULI_VOIMISTUU_NOPEASTI_COMPOSITE_PHRASE << timePhrase
                                << windDirectionSentence(windDirectionIdAvgPlus1, true)
                                << changeAttributeStr;
               */
            }
            else
            {
              sentence << ILTAPAIVALLA_ETELATUULI_HEIKKENEE_NOPEASTI_COMPOSITE_PHRASE << timePhrase
                       << windDirectionSentence(windDirectionIdAvgPlus1, true)
                       << changeAttributeStr;
            }
          }
          previousWindDirectionId = windDirectionIdAvgPlus1;
        }
        else
        {
          if (theWindSpeedChangeStarts && !theWindSpeedChangeEnds)
          {
            if (strengtheningWind)
              sentence
                  << ILTAPAIVALLA_POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_NOPEASTI_JA_KAANTYY_ILLALLA_ETELAAN_COMPOSITE_PHRASE;
            else
              sentence
                  << ILTAPAIVALLA_POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_HEIKETA_NOPEASTI_JA_KAANTYY_ILLALLA_ETELAAN_COMPOSITE_PHRASE;

            sentence << timePhrase << windDirectionSentence(windDirectionIdBegPlus1)
                     << windSpeedIntervalSentence(WeatherPeriod(eventPeriod.localStartTime(),
                                                                eventPeriod.localStartTime()),
                                                  DONT_USE_AT_ITS_STRONGEST_PHRASE)
                     << (smallChange ? EMPTY_STRING : changeAttributeStr) << EMPTY_STRING
                     << getWindDirectionTurntoString(windDirectionIdEnd);
            windSpeedIntervalPeriodPreviousReported =
                WeatherPeriod(eventPeriod.localStartTime(), eventPeriod.localStartTime());
            appendDecreasingIncreasingInterval = false;
          }
          else
          {
            sentence
                << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_JOKA_KAANTYY_POHJOISEEN_COMPOSITE_PHRASE
                << timePhrase << changeAttributeStr
                << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
                << windDirectionSentence(windDirectionIdBegPlus1)
                << getWindDirectionTurntoString(windDirectionIdEnd);
          }
          previousWindDirectionId = windDirectionIdEnd;
        }
      }
      else
      {
        bool directionTurns = windDirectionTurns(eventPeriod);
        if (directionTurns)
        {
          Sentence timePhraseAtWindDirectionChange;
          TextGenPosixTime directionChangeTime =
              get_wind_direction_change_time(theParameters, eventPeriod, true);

          if (get_part_of_the_day_id(directionChangeTime) ==
              get_part_of_the_day_id(eventPeriod.localStartTime()))
          {
            timePhraseAtWindDirectionChange << EMPTY_STRING;
          }
          else
          {
            timePhraseAtWindDirectionChange
                << getTimePhrase(WeatherPeriod(directionChangeTime, directionChangeTime), false);
          }

          if (theWindSpeedChangeStarts && !theWindSpeedChangeEnds)
          {
            if (strengtheningWind)
              sentence
                  << ILTAPAIVALLA_POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_NOPEASTI_JA_KAANTYY_ILLALLA_ETELAAN_COMPOSITE_PHRASE;
            else
              sentence
                  << ILTAPAIVALLA_POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_HEIKETA_NOPEASTI_JA_KAANTYY_ILLALLA_ETELAAN_COMPOSITE_PHRASE;

            sentence << timePhrase << windDirectionSentence(windDirectionIdBeg)
                     << windSpeedIntervalSentence(WeatherPeriod(eventPeriod.localStartTime(),
                                                                eventPeriod.localStartTime()),
                                                  DONT_USE_AT_ITS_STRONGEST_PHRASE)
                     << (smallChange ? EMPTY_STRING : changeAttributeStr)
                     << timePhraseAtWindDirectionChange
                     << getWindDirectionTurntoString(windDirectionIdEnd);
            windSpeedIntervalPeriodPreviousReported =
                WeatherPeriod(eventPeriod.localStartTime(), eventPeriod.localStartTime());
            appendDecreasingIncreasingInterval = false;
          }
          else
          {
            sentence
                << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_JOKA_KAANTYY_ILLALLA_POHJOISEEN_COMPOSITE_PHRASE
                << timePhrase << changeAttributeStr
                << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
                << windDirectionSentence(windDirectionIdBeg) << timePhraseAtWindDirectionChange
                << getWindDirectionTurntoString(windDirectionIdEnd);
          }
          previousWindDirectionId = windDirectionIdEnd;
        }
        else
        {
          if (theWindSpeedChangeStarts && !theWindSpeedChangeEnds)
          {
            if (strengtheningWind)
              sentence << POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_COMPOSITE_PHRASE;
            else
              sentence << POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_HEIKETA_COMPOSITE_PHRASE;

            sentence << windDirectionSentence(windDirectionIdAvg)
                     << windSpeedIntervalSentence(eventStartPeriod,
                                                  DONT_USE_AT_ITS_STRONGEST_PHRASE);
            windSpeedIntervalPeriodPreviousReported = eventStartPeriod;
            appendDecreasingIncreasingInterval = false;
          }
          else
          {
            sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE << timePhrase
                     << changeAttributeStr
                     << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
                     << windDirectionSentence(windDirectionIdAvg);
          }
          previousWindDirectionId = windDirectionIdAvg;
        }
      }
    }
    else  // not first sentence
    {
      if (previousWindEventId & TUULI_MUUTTUU_VAIHTELEVAKSI ||
          previousWindDirectionId == VAIHTELEVA)
      {
        // direction doesn't turn if turn is puolienen -> suuntainen
        bool directionTurns =
            windDirectionTurns(WeatherPeriod(begTimePlus1, eventPeriod.localEndTime()));
        if (windDirectionIdBegPlus1 == windDirectionIdEnd || !directionTurns)
        {
          if (theWindSpeedChangeStarts && !theWindSpeedChangeEnds)
          {
            if (strengtheningWind)
              sentence << ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI_COMPOSITE_PHRASE
                       << timePhrase << windDirectionSentence(windDirectionIdAvgPlus1)
                       << (smallChange ? EMPTY_STRING : changeAttributeStr);
            else
            {
              sentence << ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI_COMPOSITE_PHRASE
                       << timePhrase << windDirectionSentence(windDirectionIdAvgPlus1)
                       << (smallChange ? EMPTY_STRING : changeAttributeStr);
            }
          }
          else
          {
            sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE << timePhrase
                     << changeAttributeStr
                     << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
                     << windDirectionSentence(windDirectionIdAvgPlus1);
          }
          previousWindDirectionId = windDirectionIdAvgPlus1;
        }
        else
        {
          Sentence timePhraseAtWindDirectionChange;
          TextGenPosixTime directionChangeTime =
              get_wind_direction_change_time(theParameters, eventPeriod, true);

          if (get_part_of_the_day_id(directionChangeTime) ==
              get_part_of_the_day_id(eventPeriod.localStartTime()))
          {
            timePhraseAtWindDirectionChange << EMPTY_STRING;
          }
          else
          {
            timePhraseAtWindDirectionChange
                << getTimePhrase(WeatherPeriod(directionChangeTime, directionChangeTime), false);
          }

          if (theWindSpeedChangeStarts && !theWindSpeedChangeEnds)
          {
            if (strengtheningWind)
              sentence
                  << ILTAPAIVALLA_POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_NOPEASTI_JA_KAANTYY_ILLALLA_ETELAAN_COMPOSITE_PHRASE;
            else
              sentence
                  << ILTAPAIVALLA_POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_HEIKETA_NOPEASTI_JA_KAANTYY_ILLALLA_ETELAAN_COMPOSITE_PHRASE;

            sentence << timePhrase << windDirectionSentence(windDirectionIdBegPlus1)
                     << windSpeedIntervalSentence(WeatherPeriod(eventPeriod.localStartTime(),
                                                                eventPeriod.localStartTime()),
                                                  DONT_USE_AT_ITS_STRONGEST_PHRASE)
                     << (smallChange ? EMPTY_STRING : changeAttributeStr)
                     << timePhraseAtWindDirectionChange
                     << getWindDirectionTurntoString(windDirectionIdEnd);
            windSpeedIntervalPeriodPreviousReported =
                WeatherPeriod(eventPeriod.localStartTime(), eventPeriod.localStartTime());
            appendDecreasingIncreasingInterval = false;
          }
          else
          {
            sentence
                << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_JOKA_KAANTYY_ILLALLA_POHJOISEEN_COMPOSITE_PHRASE
                << timePhrase << changeAttributeStr
                << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
                << windDirectionSentence(windDirectionIdBegPlus1) << timePhraseAtWindDirectionChange
                << getWindDirectionTurntoString(windDirectionIdEnd);
          }
          previousWindDirectionId = windDirectionIdEnd;
        }
      }
      else
      {
        bool directionTurns = windDirectionTurns(eventPeriod);
        if (directionTurns)
        {
          Sentence timePhraseAtWindDirectionChange;
          TextGenPosixTime directionChangeTime =
              get_wind_direction_change_time(theParameters, eventPeriod, true);

          if (get_part_of_the_day_id(directionChangeTime) ==
              get_part_of_the_day_id(eventPeriod.localStartTime()))
          {
            timePhraseAtWindDirectionChange << EMPTY_STRING;
          }
          else
          {
            timePhraseAtWindDirectionChange
                << getTimePhrase(WeatherPeriod(directionChangeTime, directionChangeTime), false);
          }

          sentence
              << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_JOKA_KAANTYY_ILLALLA_POHJOISEEN_COMPOSITE_PHRASE
              << timePhrase << changeAttributeStr
              << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
              << windDirectionSentence(windDirectionIdBeg);
          if (get_part_of_the_day_id(directionChangeTime) ==
              get_part_of_the_day_id(eventPeriod.localStartTime()))
            sentence << EMPTY_STRING << getWindDirectionTurntoString(windDirectionIdEnd);
          else
            sentence << timePhraseAtWindDirectionChange
                     << getWindDirectionTurntoString(windDirectionIdEnd);
          previousWindDirectionId = windDirectionIdEnd;
        }
        else
        {
          sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE << timePhrase
                   << changeAttributeStr
                   << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
                   << windDirectionSentence(windDirectionIdAvg);
          previousWindDirectionId = windDirectionIdAvg;
        }
      }
    }
  }

  if (sentences.empty()) sentences.push_back(sentence);

  return sentences;
}

void WindForecast::logWindSpeedPeriod(const WeatherPeriod& period,
                                      const Sentence& intervalSentence) const
{
  theParameters.theLog << "windspeed reporting period: ";
  theParameters.theLog << period;
  theParameters.theLog << ": ";
  theParameters.theLog << intervalSentence;
}

// nextItemWithMissingEvent is set only when there is no change on next period (MISSING_WIND_EVENT)
Sentence WindForecast::decreasingIncreasingInterval(
    const WindEventPeriodDataItem& eventPeriodDataItem,
    WindEventPeriodDataItem* nextItemWithMissingEvent,
    bool firstSentenceInTheStory,
    WindEventId eventId) const
{
  Sentence sentence;

  WeatherPeriod speedEventPeriod(eventPeriodDataItem.theWindSpeedChangePeriod);
  WeatherPeriod periodBeg(speedEventPeriod.localStartTime(), speedEventPeriod.localStartTime());
  WeatherPeriod periodEnd(speedEventPeriod.localEndTime(), speedEventPeriod.localEndTime());

  if (speedEventPeriod.localEndTime().GetHour() == 18 ||
      speedEventPeriod.localEndTime().GetHour() == 17 ||
      speedEventPeriod.localEndTime().GetHour() == 6)
  {
    TextGenPosixTime endPeriodStartTime(speedEventPeriod.localEndTime());
    endPeriodStartTime.ChangeByHours(-3);
    periodEnd = WeatherPeriod(endPeriodStartTime, speedEventPeriod.localEndTime());
  }

  TextGenPosixTime phrasePeriodEndTimestamp(speedEventPeriod.localEndTime());

  bool useTimePhrase = true;

  bool windIsDecreasing = (eventId & TUULI_HEIKKENEE);

#ifdef TODO
  // lieneekˆ tarpeen
  if (!wind_speed_differ_enough(theParameters, speedEventPeriod))
  {
    if (useTimePhrase && !firstSentenceInTheStory)
    {
      if (thePreviousPartOfTheDay ==
          get_adjusted_part_of_the_day_id(speedEventPeriod, theParameters.theAlkaenPhraseUsed))
      {
        sentence << windSpeedIntervalSentence(periodEnd,
                                              USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
      }
      else
      {
        sentence << Delimiter(COMMA_PUNCTUATION_MARK)
                 << getTimePhrase(periodEnd, eventPeriodDataItem.theLongTermSpeedChangeFlag)
                 << windSpeedIntervalSentence(periodEnd,
                                              USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
      }
      logWindSpeedPeriod(
          periodEnd,
          windSpeedIntervalSentence(periodEnd, USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing));
    }
    else
    {
      if (firstSentenceInTheStory)
      {
        sentence << windSpeedIntervalSentence(speedEventPeriod,
                                              USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
        logWindSpeedPeriod(speedEventPeriod,
                           windSpeedIntervalSentence(
                               speedEventPeriod, USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing));
      }
      else
      {
        sentence << windSpeedIntervalSentence(periodEnd,
                                              USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);

        logWindSpeedPeriod(
            periodEnd,
            windSpeedIntervalSentence(periodEnd, USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing));
      }
    }
    return sentence;
  }
#endif

  if (get_period_length(speedEventPeriod) <= 2)
  {
    WeatherPeriod intervalPeriod = WeatherPeriod(
        speedEventPeriod.localEndTime(),
        (nextItemWithMissingEvent != NULL ? nextItemWithMissingEvent->thePeriod.localEndTime()
                                          : speedEventPeriod.localEndTime()));

    sentence << windSpeedIntervalSentence(intervalPeriod,
                                          USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
    return sentence;
  }

  vector<WeatherPeriod> speedChangePeriods =
      getWindSpeedReportingPoints(eventPeriodDataItem, firstSentenceInTheStory, eventId);

  if (speedChangePeriods.size() <= 1)
  {
    WeatherPeriod intervalPeriod = WeatherPeriod(
        speedEventPeriod.localEndTime(),
        (nextItemWithMissingEvent != NULL ? nextItemWithMissingEvent->thePeriod.localEndTime()
                                          : speedEventPeriod.localEndTime()));

    if (firstSentenceInTheStory)
    {
      sentence << Delimiter(COMMA_PUNCTUATION_MARK) << ALUKSI_WORD
               << windSpeedIntervalSentence(periodBeg, DONT_USE_AT_ITS_STRONGEST_PHRASE);

      sentence << Delimiter(COMMA_PUNCTUATION_MARK)
               << getTimePhrase(periodEnd, eventPeriodDataItem.theLongTermSpeedChangeFlag)
               << windSpeedIntervalSentence(intervalPeriod,
                                            USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);

      logWindSpeedPeriod(periodBeg,
                         windSpeedIntervalSentence(periodBeg, DONT_USE_AT_ITS_STRONGEST_PHRASE));
      logWindSpeedPeriod(periodBeg,
                         windSpeedIntervalSentence(
                             intervalPeriod, USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing));
    }
    else
    {
      std::string dayPhasePhraseWholePeriod;
      std::string dayPhasePhrasePeriodEnd;
      get_time_phrase_large(
          speedEventPeriod, false, theParameters.theVar, dayPhasePhraseWholePeriod, false);

      get_time_phrase_large(periodEnd, false, theParameters.theVar, dayPhasePhrasePeriodEnd, false);

      if (dayPhasePhraseWholePeriod == dayPhasePhrasePeriodEnd &&
          get_adjusted_part_of_the_day_id(speedEventPeriod) ==
              get_part_of_the_day_id_narrow(periodEnd))
      {
        sentence << windSpeedIntervalSentence(intervalPeriod,
                                              USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);

        logWindSpeedPeriod(periodEnd, sentence);
      }
      else
      {
        sentence << Delimiter(COMMA_PUNCTUATION_MARK)
                 << getTimePhrase(periodEnd, eventPeriodDataItem.theLongTermSpeedChangeFlag)
                 << windSpeedIntervalSentence(intervalPeriod,
                                              USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);

        logWindSpeedPeriod(periodEnd,
                           windSpeedIntervalSentence(
                               intervalPeriod, USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing));
      }
    }
  }
  else
  {
    if (firstSentenceInTheStory)
    {
      // first sentence in the story -> report wind speed in the beginning of whole period
      sentence << Delimiter(COMMA_PUNCTUATION_MARK) << ALUKSI_WORD
               << windSpeedIntervalSentence(periodBeg,
                                            USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);

      useTimePhrase = true;
      logWindSpeedPeriod(
          periodBeg,
          windSpeedIntervalSentence(periodBeg, USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing));
    }

    if (!useTimePhrase) useTimePhrase = speedChangePeriods.size() > 2;

    for (unsigned int i = 0; i < speedChangePeriods.size(); i++)
    {
      Sentence intervalSentence;
      bool lastPeriod(i == speedChangePeriods.size() - 1);

      if (!lastPeriod)
      {
        // here we check that we have different wind speed intervals in successive periods
        // so that we dont end up for example to phrase
        // aamup‰iv‰ll‰ 13-18 m/s, iltap‰iv‰ll‰ 13-18 m/s, ylimmill‰‰n 23 m/s.
        WeatherPeriod currentPeriodEnd(speedChangePeriods[i].localEndTime(),
                                       speedChangePeriods[i].localEndTime());
        WeatherPeriod nextPeriodEnd(speedChangePeriods[i + 1].localEndTime(),
                                    speedChangePeriods[i + 1].localEndTime());
        part_of_the_day_id currentDayPart(get_part_of_the_day_id_narrow(currentPeriodEnd));
        part_of_the_day_id nextDayPart(get_part_of_the_day_id_narrow(nextPeriodEnd));

        if (nextPeriodEnd.localEndTime().GetHour() == 18 ||
            nextPeriodEnd.localEndTime().GetHour() == 17 ||
            nextPeriodEnd.localEndTime().GetHour() == 6)
        {
          TextGenPosixTime endPeriodStartTime(speedEventPeriod.localEndTime());
          endPeriodStartTime.ChangeByHours(-3);
          nextPeriodEnd = WeatherPeriod(endPeriodStartTime, endPeriodStartTime);
        }

        // wind speed is mentioned only once per day part, for example we don't say
        // iltap‰iv‰ll‰ 10...13 m/s, iltap‰iv‰ll‰ 15...18
        if (currentDayPart == nextDayPart) continue;

        float lowerLimit1(kFloatMissing), lowerLimit2(kFloatMissing), upperLimit1(kFloatMissing),
            upperLimit2(kFloatMissing);

        get_wind_speed_interval(currentPeriodEnd, theParameters, lowerLimit1, upperLimit1);

        get_wind_speed_interval(nextPeriodEnd, theParameters, lowerLimit2, upperLimit2);

        lowerLimit1 = round(lowerLimit1);
        upperLimit1 = round(upperLimit1);
        lowerLimit2 = round(lowerLimit2);
        upperLimit2 = round(upperLimit2);

        if (abs(lowerLimit2 - lowerLimit1) <= 1.0) continue;
      }

      WeatherPeriod intervalPeriod(speedChangePeriods[i].localStartTime(),
                                   lastPeriod && nextItemWithMissingEvent != NULL
                                       ? nextItemWithMissingEvent->thePeriod.localEndTime()
                                       : speedChangePeriods[i].localEndTime());

      intervalSentence << windSpeedIntervalSentence(
          intervalPeriod, lastPeriod && USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);

      if (!intervalSentence.empty())
      {
        if (useTimePhrase)
        {
          if (!lastPeriod)
          {
            TextGenPosixTime currentTimestamp(speedChangePeriods[i].localStartTime());
            TextGenPosixTime nextTimestamp(speedChangePeriods[i + 1].localStartTime());
            if (get_part_of_the_day_id(currentTimestamp) == get_part_of_the_day_id(nextTimestamp))
              continue;
          }

          sentence << Delimiter(COMMA_PUNCTUATION_MARK);

          TextGenPosixTime phrasePeriodEndTimestamp(intervalPeriod.localStartTime());

          WeatherPeriod phrasePeriod(phrasePeriodEndTimestamp, phrasePeriodEndTimestamp);

          if (lastPeriod)
          {
            Sentence timePhrase;
            if (get_adjusted_part_of_the_day_id(phrasePeriod) != MISSING_PART_OF_THE_DAY_ID)
              timePhrase << getTimePhrase(phrasePeriod,
                                          eventPeriodDataItem.theLongTermSpeedChangeFlag);
            if (timePhrase.empty()) timePhrase << getTimePhrase(phrasePeriod, USE_ALKAEN_PHRASE);
            sentence << timePhrase << intervalSentence;
          }
          else
          {
            sentence << getTimePhrase(phrasePeriod, DONT_USE_ALKAEN_PHRASE) << intervalSentence;
          }
        }
        else
        {
          sentence << intervalSentence;
        }
      }
    }  // for(unsigned int i = 1; i < speedChangePeriods.size(); i++)
  }

  return sentence;
}

bool WindForecast::samePartOfTheDay(const TextGenPosixTime& time1,
                                    const TextGenPosixTime& time2) const
{
  TextGenPosixTime iterTime(time1);

  while (iterTime < time2 && get_part_of_the_day_id(iterTime) == get_part_of_the_day_id(time2))
  {
    iterTime.ChangeByHours(1);
  }

  return (iterTime == time2);
}

void WindForecast::getPeriodStartAndEndIndex(const WeatherPeriod& period,
                                             unsigned int& begIndex,
                                             unsigned int& endIndex) const
{
  for (unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
  {
    const WindDataItemUnit& windDataItem =
        (*theParameters.theWindDataVector[i])(theParameters.theArea.type());

    if (period.localStartTime() == windDataItem.thePeriod.localStartTime())
    {
      begIndex = i;
      endIndex = i;
    }
    else if (period.localEndTime() == windDataItem.thePeriod.localEndTime())
    {
      endIndex = i;
      break;
    }
  }
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

  std::string dayPhasePhrase;

  theParameters.theAlkaenPhraseUsed = (wind_forecast_period_length(thePeriod) >= 6 &&
                                       useAlkaenPhrase && !fit_into_narrow_day_part(thePeriod));

  // try to prevent tautology, like "iltap‰iv‰ll‰"... "iltap‰iv‰st‰ alkaen"
  if (thePreviousPartOfTheDay ==
          get_adjusted_part_of_the_day_id(thePeriod, theParameters.theAlkaenPhraseUsed) &&
      thePreviousPartOfTheDay != MISSING_PART_OF_THE_DAY_ID && get_period_length(thePeriod) > 4)
  {
    TextGenPosixTime startTime(thePeriod.localStartTime());
    startTime.ChangeByHours(2);
    WeatherPeriod shortenedPeriod(startTime, thePeriod.localEndTime());
    theParameters.theAlkaenPhraseUsed =
        (wind_forecast_period_length(shortenedPeriod) >= 6 && useAlkaenPhrase &&
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

vector<WeatherPeriod> WindForecast::getWindSpeedReportingPoints(
    const WindEventPeriodDataItem& eventPeriodDataItem,
    bool firstSentenceInTheStory,
    WindEventId eventId) const
{
  vector<WeatherPeriod> retVector;

  WeatherPeriod speedEventPeriod(eventPeriodDataItem.theWindSpeedChangePeriod);
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
    float previousThresholdTopWind(kFloatMissing);
    for (unsigned int i = begIndex; i < theParameters.theWindDataVector.size(); i++)
    {
      const WindDataItemUnit& windDataItem =
          (*theParameters.theWindDataVector[i])(theParameters.theArea.type());
      if (!is_inside(windDataItem.thePeriod.localStartTime(), speedEventPeriod)) break;

      if (i == begIndex)
      {
        // speed at the beginning of the period is reported
        // reportingIndexes.push_back(i);
        previousThresholdTopWind = windDataItem.theEqualizedTopWind.value();
      }
      else if (abs(windDataItem.theEqualizedTopWind.value() - previousThresholdTopWind) > 4.0 &&
               ((round(previousThresholdTopWind) > WEAK_WIND_SPEED_UPPER_LIMIT &&
                 (eventId & TUULI_HEIKKENEE)) ||
                (round(windDataItem.theEqualizedTopWind.value()) > WEAK_WIND_SPEED_UPPER_LIMIT &&
                 (eventId & TUULI_VOIMISTUU))))
      {
        // speed is reported when it has changed 4.0 from the previous reporting point
        reportingIndexes.push_back(i);
        previousThresholdTopWind = windDataItem.theEqualizedTopWind.value();
      }
      endIndex = i;
    }

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
        retVector.push_back(WeatherPeriod(windDataItem.thePeriod.localStartTime(),
                                          windDataItemLast.thePeriod.localEndTime()));
      }
      else if (i < reportingIndexes.size() - 1)
      {
        // report wind speed at certain point of time during increasing/decreasing period
        retVector.push_back(windDataItem.thePeriod);
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
          startTime.ChangeByHours(-3);
        WeatherPeriod lastReportingPeriod(startTime, endTime);
        retVector.push_back(lastReportingPeriod);
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
  int periodLength(wind_forecast_period_length(changePeriod));

  phraseStr = EMPTY_STRING;

  if (abs(endUpperLimit - begUpperLimit) <= changeThreshold * 0.5)
  {
    phraseStr = VAHAN_WORD;
    smallChange = true;
  }
  else if (abs(endUpperLimit - begUpperLimit) >= changeThreshold)
  {
    if (periodLength <= 6)
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

  intervalLowerLimit = round(intervalLowerLimit);
  intervalUpperLimit = round(intervalUpperLimit);

  int actualIntervalSize(intervalUpperLimit - intervalLowerLimit);
  int peakWind(intervalUpperLimit);

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

  // if interval upper limit is 2, we set lower limit to zero
  if (intervalUpperLimit == 2) intervalLowerLimit = 0;

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

// return upper and lower limit for the interval
bool WindForecast::getSpeedIntervalLimits(const WeatherPeriod& thePeriod,
                                          WeatherResult& lowerLimit,
                                          WeatherResult& upperLimit) const
{
  WindDataItemUnit dataItem((*theParameters.theWindDataVector[0])(theParameters.theArea.type()));
  WindDataItemUnit dataItemMinTop(
      (*theParameters.theWindDataVector[0])(theParameters.theArea.type()));

  bool firstRound(true);

  for (unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
  {
    dataItem = (*theParameters.theWindDataVector[i])(theParameters.theArea.type());

    if (is_inside(dataItem.thePeriod.localStartTime(), thePeriod))
    {
      if (firstRound)
      {
        dataItemMinTop.theEqualizedTopWind = dataItem.theEqualizedTopWind;
        dataItemMinTop.theEqualizedMedianWind = dataItem.theEqualizedMedianWind;
        firstRound = false;
        continue;
      }

      if (dataItem.theEqualizedTopWind.value() > dataItemMinTop.theEqualizedTopWind.value())
      {
        dataItemMinTop.theEqualizedTopWind = dataItem.theEqualizedTopWind;
      }
      if (dataItem.theEqualizedMedianWind.value() < dataItemMinTop.theEqualizedMedianWind.value())
      {
        dataItemMinTop.theEqualizedMedianWind = dataItem.theEqualizedMedianWind;
      }
    }
  }

  // no data found for thePeriod
  if (firstRound) return false;

  lowerLimit = dataItemMinTop.theEqualizedMedianWind;
  upperLimit = dataItemMinTop.theEqualizedTopWind;

  return true;
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

Sentence WindForecast::windDirectionSentence(WindDirectionId theWindDirectionId,
                                             bool theBasicForm /*=  false */) const
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

void WindForecast::getWindDirectionBegEnd(const WeatherPeriod& thePeriod,
                                          WindDirectionId& theWindDirectionIdBeg,
                                          WindDirectionId& theWindDirectionIdEnd) const
{
  theWindDirectionIdBeg = get_wind_direction_id_at(theParameters, thePeriod.localStartTime());
  theWindDirectionIdEnd = get_wind_direction_id_at(theParameters, thePeriod.localEndTime());
}

std::string WindForecast::getWindDirectionTurntoString(WindDirectionId theWindDirectionId) const
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

bool WindForecast::windDirectionTurns(const WeatherPeriod& thePeriod) const
{
  WeatherResult windDirectionBeg(kFloatMissing, kFloatMissing);
  WeatherResult windDirectionEnd(kFloatMissing, kFloatMissing);
  WeatherResult windTopSpeedBeg(kFloatMissing, kFloatMissing);
  WeatherResult windTopSpeedEnd(kFloatMissing, kFloatMissing);

  for (unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
  {
    WindDataItemUnit& item =
        theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
    if (item.thePeriod.localStartTime() == thePeriod.localStartTime())
    {
      windDirectionBeg = item.theEqualizedWindDirection;
      windTopSpeedBeg = item.theEqualizedTopWind;
    }
    else if (item.thePeriod.localStartTime() == thePeriod.localEndTime())
    {
      windDirectionEnd = item.theEqualizedWindDirection;
      windTopSpeedEnd = item.theEqualizedTopWind;
      break;
    }
  }

  return wind_direction_turns(windDirectionBeg,
                              windDirectionEnd,
                              windTopSpeedBeg,
                              windTopSpeedEnd,
                              theParameters.theVar,
                              theParameters.theWindDirectionMinSpeed);
}

void WindForecast::findOutActualWindSpeedChangePeriod(
    WindEventPeriodDataItem* currentEventPeriodItem,
    WindEventPeriodDataItem* nextEventPeriodItem) const
{
  bool windStrengthStartsToChange(currentEventPeriodItem->theWindSpeedChangeStarts &&
                                  !currentEventPeriodItem->theWindSpeedChangeEnds);

  if (windStrengthStartsToChange && nextEventPeriodItem)
  {
    if (!wind_speed_differ_enough(theParameters, nextEventPeriodItem->thePeriod))
    {
      currentEventPeriodItem->theWindSpeedChangePeriod =
          WeatherPeriod(currentEventPeriodItem->thePeriod.localStartTime(),
                        nextEventPeriodItem->thePeriod.localEndTime());
      currentEventPeriodItem->theWindSpeedChangeEnds = true;
      nextEventPeriodItem->theWindSpeedChangeEnds = false;

      WindEventId nextEventId(nextEventPeriodItem->theWindEvent);
      nextEventPeriodItem->theWindEvent = mask_wind_event(
          nextEventId, (nextEventId & TUULI_HEIKKENEE ? TUULI_HEIKKENEE : TUULI_VOIMISTUU));
    }
  }
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
