// ======================================================================
/*!
 * \file
 * \brief Implementation of WindForecast class
 */
// ======================================================================

#include "WindForecast.h"

#include "AreaTools.h"
#include "CloudinessStory.h"
#include "CloudinessStoryTools.h"
#include "Delimiter.h"
#include "IntegerRange.h"
#include "MessageLogger.h"
#include "NightAndDayPeriodGenerator.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "SeasonTools.h"
#include "Sentence.h"
#include "SubMaskExtractor.h"
#include "UnitFactory.h"
#include "ValueAcceptor.h"
#include "WeatherStory.h"
#include "WeekdayTools.h"
#include "WindStoryTools.h"
#include <calculator/GridForecaster.h>
#include <calculator/HourPeriodGenerator.h>
#include <calculator/MathTools.h>
#include <calculator/NullPeriodGenerator.h>
#include <calculator/RangeAcceptor.h>
#include <calculator/Settings.h>
#include <calculator/TextGenError.h>
#include <calculator/TimeTools.h>
#include <calculator/WeatherPeriodTools.h>
#include <calculator/WeatherResult.h>

#include <newbase/NFmiCombinedParam.h>
#include <newbase/NFmiMercatorArea.h>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <macgyver/StringConversion.h>

#include <cmath>
#include <iomanip>
#include <map>
#include <vector>

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
#define ILTAPAIVALLA_EDELLEEN_HEIKKENEVAA_POHJOISTUULTA_COMPOSITE_PHRASE \
  "[iltapaivalla] edelleen [heikkenevaa] [pohjoistuulta]"
#define ILTAPAIVALLA_POHJOISTUULI_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE \
  "[iltapaivalla] [pohjoistuuli] heikkenee edelleen"
#define ILTAPAIVALLA_POHJOISTUULI_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE \
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

part_of_the_day_id get_most_relevant_part_of_the_day_id_narrow(const WeatherPeriod& thePeriod);

part_of_the_day_id get_part_of_the_day_id_wind(const TextGenPosixTime& theTime)
{
  int hour = theTime.GetHour();

  //  if (hour == 23 || hour <= 2) return KESKIYO;

  if (hour >= 0 && hour <= 6)
    return AAMUYO;
  else if (hour > 6 && hour <= 9)
    return AAMU;
  else if (hour > 9 && hour < 12)
    return AAMUPAIVA;
  else if (hour >= 12 && hour <= 18)
    return ILTAPAIVA;
  else if (hour > 18 && hour <= 21)
    return ILTA;
  else  // if (hour > 21)
    return ILTAYO;
}

part_of_the_day_id get_part_of_the_day_id_wind(const WeatherPeriod& thePeriod)
{
  if (get_period_length(thePeriod) == 0 && thePeriod.localStartTime().GetHour() == 12)
    return KESKIPAIVA;
  /*
  if (get_period_length(thePeriod) <= 3 && get_period_length(thePeriod) > 0 &&
      (thePeriod.localStartTime().GetHour() == 23 || thePeriod.localStartTime().GetHour() <= 2) &&
      thePeriod.localEndTime().GetHour() <= 2)
    return KESKIYO;
  */

  part_of_the_day_id ret;
  if (get_period_length(thePeriod) == 0)
    ret = get_part_of_the_day_id_wind(thePeriod.localStartTime());
  else
    ret = get_most_relevant_part_of_the_day_id_narrow(thePeriod);

  return ret;
}

bool operator==(const TimePhraseInfo& p1, const TimePhraseInfo& p2)
{
  // if (abs(p1.starttime.DifferenceInHours(p2.starttime)) <= 2) return true;

  return p1.day_number == p2.day_number && p1.part_of_the_day == p2.part_of_the_day;
}

bool operator!=(const TimePhraseInfo& p1, const TimePhraseInfo& p2)
{
  return ((p1.day_number != p2.day_number || p1.part_of_the_day != p2.part_of_the_day));
}

std::ostream& operator<<(std::ostream& os, const interval_info& info)
{
  os << info.lowerLimit << "..." << info.upperLimit << " (" << info.startTime << "..."
     << info.endTime << ")";
  if (info.peakWind != info.upperLimit)
    os << ", kovimmillaan " << info.peakWind << " (" << info.peakWindTime << ")";

  return os;
}

std::ostream& operator<<(std::ostream& os, const TimePhraseInfo& tpi)
{
  os << tpi.starttime << "..." << tpi.endtime << " -> ";
  switch (tpi.day_number)
  {
    case 1:
      os << "maanantai";
      break;
    case 2:
      os << "tiistai";
      break;
    case 3:
      os << "keskiviikko";
      break;
    case 4:
      os << "torstai";
      break;
    case 5:
      os << "perjantai";
      break;
    case 6:
      os << "lauantai";
      break;
    case 7:
      os << "sunnuntai";
      break;
  }
  os << part_of_the_day_string(tpi.part_of_the_day) << std::endl;

  return os;
}

Paragraph operator<<(Paragraph& p, const ParagraphInfoVector& piv)
{
  for (const auto& pi : piv)
  {
    Sentence sentence;
    sentence << pi.sentence;
    for (const auto& sp : pi.sentenceParameters)
    {
      sentence << sp.sentence;
    }
    p << sentence;
  }

  return p;
}

float wind_direction_error(const wind_data_item_vector& theWindDataVector,
                           const WeatherArea& theArea,
                           const WeatherPeriod& thePeriod)
{
  unsigned int counter(0);
  float cumulativeWindDirectionError(0.0);

  for (unsigned int i = 0; i < theWindDataVector.size(); i++)
  {
    WindDataItemUnit& item = theWindDataVector[i]->getDataItem(theArea.type());
    if (is_inside(item.thePeriod.localStartTime(), thePeriod))
    {
      cumulativeWindDirectionError += item.theEqualizedWindDirection.error();
      counter++;
    }
  }

  return (counter > 0 ? (cumulativeWindDirectionError / counter) : 0.0);
}

// return true if equalized top wind is weak during whole period
bool is_weak_period(const wo_story_params& theParameters, const WeatherPeriod& thePeriod)
{
  BOOST_FOREACH (const WindDataItemsByArea* item, theParameters.theWindDataVector)
  {
    WindDataItemUnit& dataitem = item->getDataItem(theParameters.theArea.type());

    if (is_inside(dataitem.thePeriod.localStartTime(), thePeriod) &&
        dataitem.theEqualizedTopWind.value() > WEAK_WIND_SPEED_UPPER_LIMIT)
      return false;
  }

  return true;
}

// find top wind on period
unsigned int get_peak_wind(const WeatherPeriod& thePeriod,
                           const wo_story_params& theParameters,
                           TextGenPosixTime& thePeakWindTime)
{
  unsigned int upper_index = 0;

  bool upper_index_updated(false);
  // iterate timesteps
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
            thePeakWindTime = windDataItem.thePeriod.localStartTime();
          }
          break;
        }
      }
    }
  }

  return upper_index;
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

// If proposed direction is 'PUOLEINEN' and the previous was in the same direction, change the
// latter to prevent tautology
// for example (LOUNAISTUULI, LOUNAAN PUOLEINEN) -> (LOUNAISTUULI, LÄNNEN JA LOUNAAN VÄLINEN)
WindDirectionInfo negotiateWindDirection(WindDirectionInfo& theProposedWindDirection,
                                         const WindDirectionInfo& thePreviousWindDirection)
{
  if ((theProposedWindDirection.id == POHJOISEN_PUOLEINEN &&
       thePreviousWindDirection.id == POHJOINEN) ||
      (theProposedWindDirection.id == KOILLISEN_PUOLEINEN &&
       thePreviousWindDirection.id == KOILLINEN) ||
      (theProposedWindDirection.id == IDAN_PUOLEINEN && thePreviousWindDirection.id == ITA) ||
      (theProposedWindDirection.id == KAAKON_PUOLEINEN && thePreviousWindDirection.id == KAAKKO) ||
      (theProposedWindDirection.id == ETELAN_PUOLEINEN && thePreviousWindDirection.id == ETELA) ||
      (theProposedWindDirection.id == LOUNAAN_PUOLEINEN && thePreviousWindDirection.id == LOUNAS) ||
      (theProposedWindDirection.id == LANNEN_PUOLEINEN && thePreviousWindDirection.id == LANSI) ||
      (theProposedWindDirection.id == LUOTEEN_PUOLEINEN && thePreviousWindDirection.id == LUODE))
    theProposedWindDirection.id = direction_between_id(theProposedWindDirection.direction.value());

  return theProposedWindDirection;
}

WindDirectionInfo get_wind_direction(const wo_story_params& theParameters,
                                     const TextGenPosixTime& pointOfTime,
                                     const WindDirectionInfo* thePreviousWindDirection /*= 0*/)
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

  WindDirectionInfo windDirectionInfo(
      WeatherPeriod(pointOfTime, pointOfTime), directionValue, directionId);

  return windDirectionInfo;
}

WindDirectionInfo get_wind_direction(const wo_story_params& theParameters,
                                     const WeatherPeriod& period,
                                     const WindDirectionInfo* thePreviousWindDirection /*= 0*/)
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

  WeatherResult resultDirection = WindStoryTools::mode_wind_direction(theParameters.theSources,
                                                                      theParameters.theArea,
                                                                      period,
                                                                      medianWindSpeed,
                                                                      topWindSpeed,
                                                                      theParameters.theVar);

  float directionError =
      wind_direction_error(theParameters.theWindDataVector, theParameters.theArea, period);

  if (directionError < resultDirection.error())
    resultDirection = WeatherResult(resultDirection.value(), directionError);

  WindDirectionInfo windDirectionInfo =
      WindDirectionInfo(period,
                        resultDirection,
                        wind_direction_id(resultDirection,
                                          topWindSpeed,
                                          theParameters.theVar,
                                          theParameters.theWindDirectionMinSpeed));
  return windDirectionInfo;
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

Sentence windDirectionSentence(const WindDirectionInfo& theWindDirectionInfo,
                               bool theBasicForm = false)
{
  Sentence sentence;
  if (theWindDirectionInfo.id == VAIHTELEVA && get_period_length(theWindDirectionInfo.period) < 4)
    sentence << TILAPAISESTI_WORD;

  sentence << windDirectionSentence(theWindDirectionInfo.id, theBasicForm);

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

void get_plain_wind_speed_interval(const WeatherPeriod& period,
                                   wo_story_params& theParameter,
                                   float& lowerLimit,
                                   float& upperLimit)
{
  bool firstMatchProcessed(false);

  theParameter.theContextualMaxIntervalSize = theParameter.theMaxIntervalSize;

  float minValue(lowerLimit);
  float topValue(upperLimit);
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
        topValue = windDataItem.theEqualizedTopWind.value();
        maxValue = windDataItem.theEqualizedMaxWind.value();

        firstMatchProcessed = true;
      }
      else
      {
        if (windDataItem.theEqualizedMedianWind.value() < minValue)
          minValue = windDataItem.theEqualizedMedianWind.value();
        if (windDataItem.theEqualizedTopWind.value() > topValue)
          topValue = windDataItem.theEqualizedTopWind.value();
        if (windDataItem.theEqualizedMaxWind.value() > maxValue)
          maxValue = windDataItem.theEqualizedMaxWind.value();
      }
    }
  }
  lowerLimit = minValue;
  // if top wind is smaller than configuration value, use maximum wind as upper limit
  if (topValue < theParameter.theWindSpeedWarningThreshold)
  {
    // Annakaisa / Punkka 06.06.2016: pienennetään maksimikoko neljään
    theParameter.theLog << "Top wind speed (" << topValue
                        << ") is smaller than theWindSpeedWarningThreshold ("
                        << theParameter.theWindSpeedWarningThreshold
                        << ") setting interval maximum size to 4." << std::endl;
    theParameter.theContextualMaxIntervalSize = 4;
    upperLimit = maxValue;
  }
  else
  {
    upperLimit = topValue;
  }
}

float get_median_wind(const WeatherPeriod& thePeriod, const wo_story_params& theParameters)
{
  float retval(0.0);
  unsigned int counter(0);

  for (unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
  {
    const WindDataItemUnit& windDataItem =
        (*theParameters.theWindDataVector[i])(theParameters.theArea.type());

    if (is_inside(windDataItem.thePeriod, thePeriod))
    {
      retval += windDataItem.theEqualizedMedianWind.value();
      counter++;
    }
  }

  return (counter == 0 ? retval : retval / counter);
}

float get_top_wind(const WeatherPeriod& thePeriod, const wo_story_params& theParameters)
{
  float top_wind(kFloatMissing);

  for (unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
  {
    const WindDataItemUnit& windDataItem =
        (*theParameters.theWindDataVector[i])(theParameters.theArea.type());

    if (is_inside(windDataItem.thePeriod, thePeriod))
    {
      if (top_wind == kFloatMissing || windDataItem.theWindSpeedTop.value() > top_wind)
        top_wind = windDataItem.theWindSpeedTop.value();
    }
  }

  return top_wind;
}

void windspeed_distribution_interval(const WeatherPeriod& thePeriod,
                                     const wo_story_params& theParameter,
                                     float coverage,
                                     float& lowerLimit,
                                     float& upperLimit,
                                     unsigned int& mostTypicalWindSpeed)
{
  if (lowerLimit == kFloatMissing || upperLimit == kFloatMissing) return;

  value_distribution_data_vector windSpeedDistributionVector;

  // first calculte the sum of the distribution values for the period for each wind speed
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

  mostTypicalWindSpeed = 0;
  // then calculate mean value of the distribution values of the period
  for (unsigned int i = 0; i < windSpeedDistributionVector.size(); i++)
  {
    WeatherResult cumulativeShare(windSpeedDistributionVector[i].second);
    windSpeedDistributionVector[i].second = WeatherResult(cumulativeShare.value() / counter, 0.0);
    // store most typical wind speed category
    if (i > 0 &&
        windSpeedDistributionVector[mostTypicalWindSpeed].second.value() <
            windSpeedDistributionVector[i].second.value())
      mostTypicalWindSpeed = i;
  }

  // Make sure that there is enough values (coverage) inside the interval
  int lower_index = static_cast<int>(round(upperLimit));
  if (lower_index >= static_cast<int>(windSpeedDistributionVector.size()))
    lower_index = windSpeedDistributionVector.size() - 1;

  float sumDistribution = 0.0;
  while (sumDistribution < coverage && lower_index >= 0)
  {
    sumDistribution += windSpeedDistributionVector[lower_index].second.value();
    lower_index--;
  }

  lowerLimit = lower_index;
}

void get_wind_speed_interval(const WeatherPeriod& thePeriod,
                             wo_story_params& theParameter,
                             float& lowerLimit,
                             float& upperLimit,
                             TextGenPosixTime& peakWindTime)
{
  //  std::stringstream ss_log;

  //  ss_log << " wind_speed_interval:\n period: " << thePeriod << std::endl;

  // first get plain interval: smallest median wind on period, highest top wind or maximum wind on
  // period
  float plainLowerLimit = lowerLimit;
  float plainUpperLimit = upperLimit;
  get_plain_wind_speed_interval(thePeriod, theParameter, plainLowerLimit, plainUpperLimit);

  //  ss_log << " plain interval: " << plainLowerLimit << "..." << plainUpperLimit << std::endl;

  float distributionLowerLimit67 = plainLowerLimit;
  float distributionUpperLimit67 = plainUpperLimit;
  unsigned int mostTypicalWindSpeed = 0;
  // interval must contain 67% of winds
  float intervalCoverage = 67.0;
  windspeed_distribution_interval(thePeriod,
                                  theParameter,
                                  intervalCoverage,
                                  distributionLowerLimit67,
                                  distributionUpperLimit67,
                                  mostTypicalWindSpeed);

  /*
  ss_log << " distribution interval 67: " << distributionLowerLimit67 << "..."
         << distributionUpperLimit67 << " most typical: " << mostTypicalWindSpeed << std::endl;
  */

  if (abs(distributionUpperLimit67 - distributionLowerLimit67) >
      theParameter.theContextualMaxIntervalSize)
  {
    float distributionLowerLimit50 = plainLowerLimit;
    float distributionUpperLimit50 = plainUpperLimit;
    // try with 50% of winds
    intervalCoverage = 50.0;
    windspeed_distribution_interval(thePeriod,
                                    theParameter,
                                    intervalCoverage,
                                    distributionLowerLimit50,
                                    distributionUpperLimit50,
                                    mostTypicalWindSpeed);

    /*
ss_log << " distribution interval 50: " << distributionLowerLimit50 << "..."
       << distributionUpperLimit50 << " most typical: " << mostTypicalWindSpeed << std::endl;
    */

    if (distributionLowerLimit50 >= mostTypicalWindSpeed)
      lowerLimit = distributionLowerLimit50;
    else
      lowerLimit = distributionLowerLimit67;
  }
  else
  {
    lowerLimit = distributionLowerLimit67;
  }
  upperLimit = plainUpperLimit;

  // we may cut peak wind based on areal coverage (wind_speed_top_coverage configuration parameter)
  int peakWind = get_peak_wind(thePeriod, theParameter, peakWindTime);

  /*
  ss_log << " interval before peak check: " << lowerLimit << "..." << upperLimit
         << " peak: " << peakWind << std::endl;
  */

  if (peakWind < upperLimit) upperLimit = peakWind;

  /*
   ss_log << " interval after peak check: " << lowerLimit << "..." << upperLimit << std::endl;

  theParameter.theLog << ss_log.str();
  */
}

void get_wind_speed_interval(const TextGenPosixTime& pointOfTime,
                             wo_story_params& theParameters,
                             float& lowerLimit,
                             float& upperLimit)
{
  WeatherPeriod period(pointOfTime, pointOfTime);

  TextGenPosixTime peakWindTime;
  get_wind_speed_interval(period, theParameters, lowerLimit, upperLimit, peakWindTime);
}

float get_wind_speed_at(const wo_story_params& theParameters, const TextGenPosixTime& theTime)
{
  WeatherPeriod wp(theTime, theTime);

  float lowerLimit = get_median_wind(wp, theParameters);
  float upperLimit = get_top_wind(wp, theParameters);

  double topWindShare = (theParameters.theWeakTopWind ? theParameters.theWindCalcTopShareWeak
                                                      : theParameters.theWindCalcTopShare);

  double topWindWeight = (topWindShare / 100.0);
  double topMedianWindWeight = 1.0 - topWindWeight;

  return (upperLimit * topWindWeight + lowerLimit * topMedianWindWeight);
}

// return time when wind has changes n% of configured threshold
TextGenPosixTime get_wind_change_point(const wo_story_params& theParameters,
                                       const WeatherPeriod& thePeriod,
                                       float thePerCent)
{
  TextGenPosixTime timeIter(thePeriod.localStartTime());
  timeIter.ChangeByHours(1);

  while (timeIter < thePeriod.localEndTime())
  {
    WeatherPeriod end(timeIter, timeIter);

    float begSpeed = get_wind_speed_at(theParameters, thePeriod.localStartTime());
    float endSpeed = get_wind_speed_at(theParameters, timeIter);

    float difference = abs(endSpeed - begSpeed);

    if (difference >= theParameters.theWindSpeedThreshold * (thePerCent / 100.0)) break;
    timeIter.ChangeByHours(1);
  }

  return timeIter;
}

bool wind_speed_differ_enough(const wo_story_params& theParameters, const WeatherPeriod& thePeriod)
{
  float begSpeed = get_wind_speed_at(theParameters, thePeriod.localStartTime());
  float endSpeed = get_wind_speed_at(theParameters, thePeriod.localEndTime());

  // dont report change when wind is weak
  if (begSpeed < WEAK_WIND_SPEED_UPPER_LIMIT && endSpeed < WEAK_WIND_SPEED_UPPER_LIMIT)
    return false;

  float difference = abs(endSpeed - begSpeed);

  return (difference >= theParameters.theWindSpeedThreshold);
}

bool wind_speed_differ_enough(const interval_info& firstInterval,
                              const interval_info& secondInterval)
{
  unsigned int lowerLimitDifference = abs(firstInterval.lowerLimit - secondInterval.lowerLimit);
  unsigned int upperLimitDifference = abs(firstInterval.upperLimit - secondInterval.upperLimit);
  //  unsigned int peakSpeedDifference = abs(firstInterval.peakWind - secondInterval.peakWind);
  if (lowerLimitDifference + upperLimitDifference <= 3) return false;

  return true;
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

bool almost_same_direction(WindDirectionId id1, WindDirectionId id2)
{
  if (id1 == id2) return true;
  if ((id1 == POHJOINEN && id2 == POHJOISEN_PUOLEINEN) ||
      (id2 == POHJOINEN && id1 == POHJOISEN_PUOLEINEN))
    return true;
  if ((id1 == KOILLINEN && id2 == KOILLISEN_PUOLEINEN) ||
      (id2 == KOILLINEN && id1 == KOILLISEN_PUOLEINEN))
    return true;
  if ((id1 == ITA && id2 == IDAN_PUOLEINEN) || (id2 == ITA && id1 == IDAN_PUOLEINEN)) return true;
  if ((id1 == KAAKKO && id2 == KAAKON_PUOLEINEN) || (id2 == KAAKKO && id1 == KAAKON_PUOLEINEN))
    return true;
  if ((id1 == ETELA && id2 == ETELAN_PUOLEINEN) || (id2 == ETELA && id1 == ETELAN_PUOLEINEN))
    return true;
  if ((id1 == LOUNAS && id2 == LOUNAAN_PUOLEINEN) || (id2 == LOUNAS && id1 == LOUNAAN_PUOLEINEN))
    return true;
  if ((id1 == LANSI && id2 == LANNEN_PUOLEINEN) || (id2 == LANSI && id1 == LANNEN_PUOLEINEN))
    return true;
  if ((id1 == LUODE && id2 == LUOTEEN_PUOLEINEN) || (id2 == LUODE && id1 == LUOTEEN_PUOLEINEN))
    return true;

  return false;
}

void compose_wind_speed_range(const wo_story_params& theParameters,
                              int& intervalLowerLimit,
                              int& intervalUpperLimit,
                              int& peakWind)
{
  int actualIntervalSize(intervalUpperLimit - intervalLowerLimit);
  std::stringstream ss_log;
  ss_log << std::endl;

  if (actualIntervalSize < theParameters.theMinIntervalSize)
  {
    // Mikael Frisk 07.03.2013: pelkkä 5 m/s on kökkö => näytetään minimi-intervalli
    /*
      sentence << intervalLowerLimit
      << *UnitFactory::create(MetersPerSecond);
    */
    ss_log << " interval size < theMinIntervalSize: " << actualIntervalSize << " <  "
           << theParameters.theMinIntervalSize << " -> use minimum interval size: ";

    intervalUpperLimit = intervalLowerLimit + theParameters.theMinIntervalSize;
    actualIntervalSize = intervalUpperLimit - intervalLowerLimit;

    ss_log << intervalLowerLimit << "..." << intervalUpperLimit << std::endl;
  }

  if (actualIntervalSize > theParameters.theContextualMaxIntervalSize)
  {
    ss_log << " interval size > theContextualMaxIntervalSize: " << actualIntervalSize << " <  "
           << theParameters.theContextualMaxIntervalSize << " -> adapt interval: ";
    // if size of the interval is one more than allowed, we increase lower limit by 1
    // else we use 'kovimmillaan'-phrase
    if (actualIntervalSize == theParameters.theContextualMaxIntervalSize + 1)
    {
      intervalLowerLimit++;
    }
    else
    {
      intervalUpperLimit = intervalLowerLimit + theParameters.theContextualMaxIntervalSize;
    }
    ss_log << intervalLowerLimit << "..." << intervalUpperLimit << std::endl;
  }

  // if there is only 1 m/s difference between peak wind and upper limit, make them equal
  if (peakWind - intervalUpperLimit == 1)
  {
    intervalUpperLimit++;
    // and increase lower limit if necessary
    if (intervalUpperLimit - intervalLowerLimit > theParameters.theContextualMaxIntervalSize)
      intervalLowerLimit++;

    ss_log << " difference between interval upper limit and peak wind is only 1 m/s -> adapt "
              "interval: "
           << intervalLowerLimit << "..." << intervalUpperLimit << std::endl;
  }

  if (ss_log.str().size() > 1) theParameters.theLog << ss_log.str();
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
      sentence << IntegerRange(peakWind - theParameters.theContextualMaxIntervalSize,
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

Sentence compose_wind_speed_sentence(const wo_story_params& theParameters,
                                     const interval_info& intervalInfo,
                                     const Sentence& timePhrase,
                                     bool theUseAtItsStrongestPhrase)
{
  Sentence sentence;

  if (intervalInfo.peakWind > intervalInfo.upperLimit)
  {
    if (theUseAtItsStrongestPhrase)
    {
      sentence << IntegerRange(intervalInfo.lowerLimit,
                               intervalInfo.upperLimit,
                               theParameters.theRangeSeparator)
               << *UnitFactory::create(MetersPerSecond);
      sentence << Delimiter(COMMA_PUNCTUATION_MARK) << timePhrase << KOVIMMILLAAN_PHRASE
               << intervalInfo.peakWind << *UnitFactory::create(MetersPerSecond);
    }
    else
    {
      sentence << IntegerRange(intervalInfo.peakWind - theParameters.theContextualMaxIntervalSize,
                               intervalInfo.peakWind,
                               theParameters.theRangeSeparator)
               << *UnitFactory::create(MetersPerSecond);
    }
  }
  else
  {
    sentence << IntegerRange(intervalInfo.lowerLimit,
                             intervalInfo.upperLimit,
                             theParameters.theRangeSeparator)
             << *UnitFactory::create(MetersPerSecond);
  }

  return sentence;
}

void handle_period_end(WeatherPeriod& period)
{
  int periodLength = get_period_length(period);
  int endHour = period.localEndTime().GetHour();

  // short period in the end: forecast can not end to 'illalla'/'aamulla', but
  // 'iltapäivällä'/'aamuyöllä'
  if (periodLength < 2 && (endHour == 18 || endHour == 17 || endHour == 6))
  {
    TextGenPosixTime startTime(period.localEndTime());
    startTime.ChangeByHours(-2);
    period = WeatherPeriod(startTime, period.localEndTime());
  }
}

WindForecast::WindForecast(wo_story_params& parameters) : theParameters(parameters) {}
// report direction if it is same as thePreviousTimePhraseInfo
std::vector<Sentence> WindForecast::reportDirectionChanges(
    std::vector<WeatherPeriod>& theDirectionPeriods,
    WindDirectionInfo& thePreviousWindDirection,
    const TimePhraseInfo& thePreviousTimePhraseInfo,
    bool theTuuliBasicForm) const
{
  std::vector<Sentence> sentences;
  unsigned int lastReportedIndex = UINT_MAX;

  theParameters.theLog << "Find out direction changes that happen nearby "
                       << thePreviousTimePhraseInfo << std::endl;

  for (unsigned int i = 0; i < theDirectionPeriods.size(); i++)
  {
    const WeatherPeriod& period = theDirectionPeriods[i];

    if (get_period_length(period) < 2)
    {
      theParameters.theLog << "Short direction period " << as_string(period) << " skipped."
                           << std::endl;
      continue;
    }

    Sentence timePhrase;
    TimePhraseInfo timePhraseInfo;
    timePhraseInfo.day_number = thePreviousTimePhraseInfo.day_number;
    timePhraseInfo.part_of_the_day = thePreviousTimePhraseInfo.part_of_the_day;

    timePhrase << getTimePhrase(
        period.localStartTime(), timePhraseInfo, get_period_length(period) > 6);

    if (timePhraseInfo == thePreviousTimePhraseInfo)
    {
      lastReportedIndex = i;
      WindDirectionInfo windDirection = get_wind_direction(theParameters, period);

      if (windDirection.id == thePreviousWindDirection.id)
      {
        theParameters.theLog << "Same direction as previous "
                             << wind_direction_string(windDirection.id) << " -> skipped."
                             << std::endl;
        continue;
      }

      theParameters.theLog << "Reporting direction change at " << as_string(period) << " -> "
                           << wind_direction_string(windDirection.id) << std::endl;

      negotiateWindDirection(windDirection, thePreviousWindDirection);
      sentences.push_back(windDirectionSentence(windDirection.id, theTuuliBasicForm));

      thePreviousWindDirection = windDirection;
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

  // report only last change
  if (sentences.size() > 0) sentences.erase(sentences.begin(), sentences.end() - 1);

  theParameters.theLog << "Reporting wind direction changes #" << sentences.size() << std::endl;

  return sentences;
}

std::vector<Sentence> WindForecast::reportDirectionChanges(
    const WeatherPeriod& thePeriod,
    std::vector<WeatherPeriod>& theDirectionPeriods,
    WindDirectionInfo& thePreviousWindDirection,
    TimePhraseInfo& thePreviousTimePhraseInfo,
    bool lastPeriod,
    bool voimistuvaa) const
{
  std::vector<Sentence> sentences;

  unsigned int lastReportedIndex = UINT_MAX;
  TextGenPosixTime previousPeriodStartTime;
  TimePhraseInfo timePhraseInfo;
  timePhraseInfo.day_number = thePreviousTimePhraseInfo.day_number;
  timePhraseInfo.part_of_the_day = thePreviousTimePhraseInfo.part_of_the_day;

  theParameters.theLog << "Reporting wind direction changes during period " << as_string(thePeriod)
                       << ", " << theDirectionPeriods.size() << std::endl;

  for (unsigned int i = 0; i < theDirectionPeriods.size(); i++)
  {
    const WeatherPeriod& period = theDirectionPeriods[i];

    if (get_period_length(period) < 2)
    {
      theParameters.theLog << "Short direction period " << as_string(period) << " skipped."
                           << std::endl;
      continue;
    }

    if (is_inside(period.localStartTime(), thePeriod))
    {
      lastReportedIndex = i;

      WindDirectionInfo windDirection = get_wind_direction(theParameters, period);

      if (windDirection.id == thePreviousWindDirection.id)
      {
        theParameters.theLog << "Same direction as previous "
                             << wind_direction_string(windDirection.id) << " -> skipped."
                             << std::endl;
        continue;
      }

      Sentence timePhrase;

      Sentence sentence;

      timePhraseInfo.day_number = thePreviousTimePhraseInfo.day_number;
      timePhraseInfo.part_of_the_day = thePreviousTimePhraseInfo.part_of_the_day;

      if (windDirection.id == VAIHTELEVA)
      {
        timePhrase << getTimePhrase(
            period.localStartTime(), timePhraseInfo, get_period_length(period) >= 6);

        sentence << timePhrase << VAIHTELEVA_TUULI_P;
      }
      else
      {
        if (get_period_length(period) >= 6)
          timePhrase << getTimePhrase(
              period.localStartTime(), timePhraseInfo, get_period_length(period) >= 6);
        else
          timePhrase << getTimePhrase(period, timePhraseInfo, get_period_length(period) >= 6);

        negotiateWindDirection(windDirection, thePreviousWindDirection);
        /*
if (thePreviousWindDirection.id == VAIHTELEVA && voimistuvaa)
  sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE << timePhrase
           << EMPTY_STRING << VOIMISTUVAA_WORD << windDirectionSentence(windDirection.id);
else
        */
        sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE << timePhrase
                 << windDirectionSentence(windDirection.id);
      }

      theParameters.theLog << "Reporting wind direction change on period " << as_string(period)
                           << std::endl;

      // if wind direction changes rapidly on the period report only the the latest direction
      if (i > 0 && sentences.size() > 0 &&
          abs(period.localStartTime().DifferenceInHours(previousPeriodStartTime)) < 8 &&
          timePhraseInfo == thePreviousTimePhraseInfo)
        sentences[sentences.size() - 1] = sentence;
      else
        sentences.push_back(sentence);

      thePreviousTimePhraseInfo = timePhraseInfo;
      //      WeatherResult windDirectionValue = get_wind_direction_at(theParameters, period);
      thePreviousWindDirection = get_wind_direction(theParameters, period);
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

  // report only last change
  //  if (sentences.size() > 0) sentences.erase(sentences.begin(), sentences.end() - 1);

  theParameters.theLog << " Number of direction changes " << sentences.size() << std::endl;

  return sentences;
}

void WindForecast::constructWindSentence(const WindEventPeriodDataItem* windSpeedItem,
                                         const WindEventPeriodDataItem* nextWindSpeedItem,
                                         const WindDirectionPeriodInfo& firstDirectionPeriodInfo,
                                         WindDirectionInfo& thePreviousWindDirection,
                                         WindSpeedSentenceInfo& sentenceInfoVector) const
{
  const WeatherPeriod& windSpeedEventPeriod = windSpeedItem->thePeriod;

  bool firstSentence = (sentenceInfoVector.empty());

  theParameters.theLog << "This is #" << sentenceInfoVector.size() << " sentence." << std::endl;

  // we dont report speed and direction changes on weak period
  if (is_weak_period(theParameters, windSpeedEventPeriod))
  {
    // weak period in the middle is not reported
    if (!firstSentence &&
        get_period_length(windSpeedEventPeriod) !=
            get_period_length(theParameters.theForecastPeriod))
    {
      theParameters.theLog << "Wind is weak on period " << as_string(windSpeedEventPeriod)
                           << " -> period is not reported" << std::endl;
    }
    else
    {
      theParameters.theLog << "Wind is weak on period " << as_string(windSpeedEventPeriod)
                           << " -> reporting the whole period at once!" << std::endl;

      sentence_info sentenceInfo;
      sentenceInfo.period = windSpeedEventPeriod;
      WindDirectionInfo windDirection = get_wind_direction(theParameters, windSpeedEventPeriod);
      //      negotiateWindDirection(windDirection, thePreviousWindDirection);
      sentenceInfo.sentence << windDirectionSentence(windDirection.id)
                            << windSpeedIntervalSentence(windSpeedItem->thePeriod,
                                                         USE_AT_ITS_STRONGEST_PHRASE);

      //      thePreviousWindDirection = windDirection;
      sentenceInfoVector.push_back(sentenceInfo);
    }
    return;
  }

  WindEventId windSpeedEventId = windSpeedItem->theWindEvent;

  bool windStrenghtens = (windSpeedEventId == TUULI_VOIMISTUU);
  bool windWeakens = (windSpeedEventId == TUULI_HEIKKENEE);
  if (windSpeedEventId == MISSING_WIND_SPEED_EVENT)
  {
    theParameters.theLog << "Processing MISSING_WIND_SPEED_EVENT at "
                         << as_string(windSpeedEventPeriod) << firstSentence << "" << std::endl;

    // if MISSING_WIND_SPEED_EVENT is in the beginning, report direction and speed at start
    if (firstSentence && get_period_length(windSpeedEventPeriod) > 3)
    {
      sentence_info sentenceInfo;
      sentenceInfo.sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE << EMPTY_STRING
                            << windDirectionSentence(firstDirectionPeriodInfo.info.id)
                            << windSpeedIntervalSentence(windSpeedEventPeriod,
                                                         USE_AT_ITS_STRONGEST_PHRASE);

      sentenceInfo.period = windSpeedEventPeriod;
      sentenceInfoVector.push_back(sentenceInfo);
    }
  }
  else if (windStrenghtens || windWeakens)
  {
    theParameters.theLog << (windStrenghtens ? "Processing TUULI_VOIMISTUU event at "
                                             : "Processing TUULI_HEIKKENEE event at ")
                         << as_string(windSpeedEventPeriod) << std::endl;

    // get point of times/periods to report wind speed diring strenghtening/weakening period
    std::vector<WeatherPeriod> windSpeedReportingPeriods;

    bool nextPeriodIsMissing =
        (nextWindSpeedItem && nextWindSpeedItem->theWindEvent == MISSING_WIND_SPEED_EVENT);

    // if no wind speed change is happening on next period
    // report wind speed for that period here too
    if (nextPeriodIsMissing)
    {
      WeatherPeriod combinedPeriod(windSpeedItem->thePeriod.localStartTime(),
                                   nextWindSpeedItem->thePeriod.localEndTime());

      theParameters.theLog << "Reporting next period here also, since no wind speed changes there, "
                              "combined period is: "
                           << as_string(combinedPeriod) << std::endl;
      WindEventPeriodDataItem combinedItem(combinedPeriod,
                                           windSpeedItem->theWindEvent,
                                           windSpeedItem->thePeriodBeginDataItem,
                                           nextWindSpeedItem->thePeriodEndDataItem);
      windSpeedReportingPeriods = getWindSpeedReportingPeriods(combinedItem, firstSentence);
    }
    else
    {
      windSpeedReportingPeriods = getWindSpeedReportingPeriods(*windSpeedItem, firstSentence);
    }

    if (windSpeedReportingPeriods.size() == 0)
    {
      // we shouldnt never be here, but just in case
      TextGenPosixTime sTime = windSpeedEventPeriod.localStartTime();
      TextGenPosixTime eTime = windSpeedEventPeriod.localEndTime();
      if (get_period_length(windSpeedEventPeriod) > 3)
      {
        sTime = eTime;
        sTime.ChangeByHours(-3);
      }
      WeatherPeriod p(sTime, eTime);
      windSpeedReportingPeriods.push_back(p);
      theParameters.theLog << "No wind speed reporting periods found -> reporting wind speed at "
                              "the end of the period: "
                           << as_string(p) << std::endl;
    }
    else if (!windStrenghtens && windSpeedReportingPeriods.size() > 1)
    {
#ifdef LATER
      // if TUULI_HEIKKENEE on last period and it is longer than 3h,
      // report speed only on only three last hours
      WeatherPeriod& lp = windSpeedReportingPeriods[windSpeedReportingPeriods.size() - 1];
      if (get_period_length(lp) > 3)
      {
        // three last hours
        TextGenPosixTime st = lp.localEndTime();
        st.ChangeByHours(-3);
        windSpeedReportingPeriods[windSpeedReportingPeriods.size() - 1] =
            WeatherPeriod(st, lp.localEndTime());
        theParameters.theLog << "Wind weakens on last period and it is longer than 3h, report "
                                "speed only on last 3 hours"
                             << std::endl;
      }
#endif
    }

    bool smallChange = false;
    bool gradualChange = false;
    bool fastChange = false;
    sentence_info sentenceInfo;
    sentenceInfo.changeSpeed = EMPTY_STRING;
    getWindSpeedChangeAttribute(
        windSpeedEventPeriod, sentenceInfo.changeSpeed, smallChange, gradualChange, fastChange);
    // Annakaisa 9.6.2017: if weakening wind, dont use word 'vähän'
    if (windWeakens && sentenceInfo.changeSpeed == VAHAN_WORD)
      sentenceInfo.changeSpeed = EMPTY_STRING;
    sentenceInfo.period = windSpeedEventPeriod;

    // iterate reporting periods
    for (unsigned int i = 0; i < windSpeedReportingPeriods.size(); i++)
    {
      WeatherPeriod period = windSpeedReportingPeriods[i];
      interval_sentence_info isi;
      isi.period = period;
      isi.intervalInfo = windSpeedIntervalInfo(period);
      if (i == 0)
        sentenceInfo.intervalSentences.push_back(isi);
      else
        sentenceInfoVector.back().intervalSentences.push_back(isi);

      theParameters.theLog << "Reporting "
                           << (windStrenghtens ? "strenghtening wind at " : "weakening wind at ")
                           << as_string(period) << std::endl;

      // first wind speed reporting point
      if (i == 0)
      {
        if (firstSentence)
        {
          sentenceInfo.directionChange = firstDirectionPeriodInfo.info;
          // get_wind_direction(theParameters, windSpeedEventPeriod.localStartTime());
          theParameters.theLog << "First sentence -> period is " << as_string(windSpeedEventPeriod)
                               << std::endl;

          WindDirectionInfo windDirection = firstDirectionPeriodInfo.info;

          negotiateWindDirection(windDirection, thePreviousWindDirection);

          if (windDirection.id == VAIHTELEVA)
          {
            // varying wind can not weaken
            if (windStrenghtens)
            {
              sentenceInfo.sentence
                  << POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_COMPOSITE_PHRASE;
              sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::WIND_DIRECTION);
            }
            else
            {
              sentenceInfo.sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE;
              sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::TIME_PERIOD);
              sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::WIND_DIRECTION);
            }
          }
          else
          {
            // sentenceInfo.sentence << windDirectionSentence(windDirection.id);
            sentenceInfo.sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE;
            sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::TIME_PERIOD);
            sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::CHANGE_SPEED);
            sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::CHANGE_TYPE);
            sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::WIND_DIRECTION);
          }
        }
        else if (sentenceInfo.changeSpeed == EMPTY_STRING)
        {
          sentenceInfo.sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE;
          sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::TIME_PERIOD);
          sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::CHANGE_SPEED);
          sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::CHANGE_TYPE);
          sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::WIND_DIRECTION);
        }
        else
        {
          if (windSpeedItem->theSuccessiveEventFlag)
          {
            sentenceInfo.sentence
                << ILTAPAIVALLA_EDELLEEN_HEIKKENEVAA_POHJOISTUULTA_COMPOSITE_PHRASE;
            sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::TIME_PERIOD);
            sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::CHANGE_TYPE);
            sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::WIND_DIRECTION);
          }
          else
          {
            sentenceInfo.sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE;
            sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::TIME_PERIOD);
            sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::CHANGE_SPEED);
            sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::CHANGE_TYPE);
            sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::WIND_DIRECTION);
          }
          sentenceInfo.changeType = (windStrenghtens ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD);
        }
        sentenceInfo.changeType = (windStrenghtens ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD);

        sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::WIND_SPEED_INTERVAL);
        sentenceInfoVector.push_back(sentenceInfo);
      }  // if (i == 0) // first reporting point
    }
  }
}

std::ostream& operator<<(std::ostream& theOutput, const sentence_info& sentenceInfo)
{
  theOutput << "   " << as_string(sentenceInfo.sentence) << std::endl;

  if (sentenceInfo.directionChange)
    theOutput << "  direction changes same time as wind speed starts to change: "
              << sentenceInfo.directionChange->startTime() << " ~ "
              << sentenceInfo.period.localStartTime() << std::endl;

  for (unsigned int i = 0; i < sentenceInfo.sentenceParameterTypes.size(); i++)
  {
    SentenceParameterType parameterType = sentenceInfo.sentenceParameterTypes[i];

    switch (parameterType)
    {
      case TIME_PERIOD:
        theOutput << "  TIME_PERIOD: " << sentenceInfo.period.localStartTime() << "..."
                  << sentenceInfo.period.localEndTime() << std::endl;
        break;
      case WIND_DIRECTION:
        if (sentenceInfo.directionChange)
          theOutput << "  WIND_DIRECTION: " << sentenceInfo.directionChange->period.localStartTime()
                    << " (" << wind_direction_string(sentenceInfo.directionChange->id) << ")"
                    << std::endl;
        else
          theOutput << "  WIND_DIRECTION: " << (sentenceInfo.useWindBasicForm
                                                    ? "use basic form: tuuli"
                                                    : "use partitive form: tuulta")
                    << std::endl;

        break;
      case CHANGE_TYPE:
        theOutput << "  CHANGE_TYPE: " << sentenceInfo.changeType << std::endl;
        break;
      case CHANGE_SPEED:
        theOutput << "  CHANGE_SPEED: " << sentenceInfo.changeSpeed << std::endl;
        break;
      case WIND_SPEED_INTERVAL:
      {
        theOutput << "  WIND_SPEED_INTERVAL: " << std::endl;
        unsigned int index = 0;
        for (auto isi : sentenceInfo.intervalSentences)
        {
          theOutput << "  interval #" << index++ << std::endl
                    << "   sentence: " << as_string(isi.sentence) << std::endl
                    << "   period: " << as_string(isi.period) << std::endl
                    << "   interval_info: " << isi.intervalInfo << std::endl
                    << "   alkaen: " << isi.useAlkaenPhrase << std::endl
                    << "   skip: " << isi.skip << std::endl;
          if (isi.directionChangesBefore.size() > 0)
          {
            theOutput << "   direction changes changes before interval at: " << std::endl;
            for (auto direction : isi.directionChangesBefore)
              theOutput << "   " << direction.startTime() << " ("
                        << wind_direction_string(direction.id) << ")" << std::endl;
          }
          if (isi.directionChange)
            theOutput << "   direction changes same time as interval at: "
                      << isi.directionChange->startTime() << " ("
                      << wind_direction_string(isi.directionChange->id) << ")" << std::endl;
          if (isi.directionChangesAfter.size() > 0)
          {
            theOutput << "   direction changes changes after interval at: " << std::endl;
            for (auto direction : isi.directionChangesAfter)
              theOutput << "   " << direction.startTime() << " ("
                        << wind_direction_string(direction.id) << ")" << std::endl;
          }
        }
      }
      break;
    }
  }

  return theOutput;
}

void log_sentence_info(MessageLogger& theLog,
                       const std::string& theHeader,
                       const WindSpeedSentenceInfo& sentenceInfoVector)
{
  theLog << theHeader << std::endl;
  for (unsigned int i = 0; i < sentenceInfoVector.size(); i++)
    theLog << " sentence #" << i << ": " << std::endl << sentenceInfoVector[i] << std::endl;
}

// originally story contains just wind speed changes
// this function injects wind direction changes into story
void WindForecast::injectWindDirections(WindSpeedSentenceInfo& sentenceInfoVector) const
{
  WindSpeedSentenceInfo resultVector;
  WindDirectionInfo previousWindDirectionInfo;
  unsigned int directionChanges = theParameters.theWindDirectionPeriods.size();
  // direction in the beginning already reported, so start from index 1
  unsigned int directionChangeStartIndex = 1;

  theParameters.theLog << "Injecting wind directions into story" << std::endl;

  WindSpeedSentenceInfo separateDirectionSentences;
  for (unsigned int i = 0; i < sentenceInfoVector.size(); i++)
  {
    WindSpeedSentenceInfo sentenceInfoBefore;
    WindSpeedSentenceInfo sentenceInfoAfter;

    sentence_info& sentenceInfo = sentenceInfoVector[i];
    WeatherPeriod windSpeedChangePeriod = sentenceInfo.period;
    bool firstSentence = (i == 0);
    bool lastSentence = (i == sentenceInfoVector.size() - 1);

    if (sentenceInfo.intervalSentences.size() > 0)
      if (sentenceInfo.intervalSentences.back().period.localEndTime() >
          windSpeedChangePeriod.localEndTime())
      {
        windSpeedChangePeriod =
            WeatherPeriod(windSpeedChangePeriod.localStartTime(),
                          sentenceInfo.intervalSentences.back().period.localEndTime());
      }

    theParameters.theLog << "Sentence #" << i << " -> " << as_string(sentenceInfo.period)
                         << " extended to " << as_string(windSpeedChangePeriod) << std::endl;

    // Iterate direction changes during wind speed change period
    for (unsigned int j = directionChangeStartIndex; j < directionChanges; j++)
    {
      WeatherPeriod windDirectionPeriod = theParameters.theWindDirectionPeriods[j];
      if (get_period_length(windDirectionPeriod) < 3)
      {
        theParameters.theLog << "Direction change #" << j
                             << " period is short: " << get_period_length(windDirectionPeriod)
                             << ", skipping!" << std::endl;
        continue;
      }

      WindDirectionInfo windDirectionInfo = get_wind_direction(theParameters, windDirectionPeriod);

      theParameters.theLog << "Direction change #" << j << " -> "
                           << as_string(windDirectionInfo.period) << " ("
                           << wind_direction_string(windDirectionInfo.id) << ")" << std::endl;

      if ((windDirectionInfo.id == VAIHTELEVA && get_period_length(windDirectionPeriod) < 2) ||
          (windDirectionInfo.id != VAIHTELEVA && get_period_length(windDirectionPeriod) < 3))
      {
        theParameters.theLog << "Period " << as_string(windDirectionPeriod) << "("
                             << wind_direction_string(windDirectionInfo.id) << ") shorter than "
                             << get_period_length(windDirectionPeriod) << " hours -> skip it."
                             << std::endl;
        continue;
      }

      if (windDirectionPeriod.localStartTime() > windSpeedChangePeriod.localEndTime())
      {
        theParameters.theLog << "Direction changes after wind speed change ends "
                             << as_string(windDirectionPeriod) << " < "
                             << as_string(windSpeedChangePeriod) << std::endl;
        break;
      }

      if (windDirectionPeriod.localStartTime() < windSpeedChangePeriod.localStartTime() &&
          (abs(windSpeedChangePeriod.localStartTime().DifferenceInHours(
               windDirectionPeriod.localStartTime())) > 2 ||
           (windDirectionInfo.id == VAIHTELEVA && get_period_length(windDirectionPeriod) > 2)))
      {
        // direction changes before wind speed change period
        sentence_info si;
        TimePhraseInfo timePhraseInfo;

        negotiateWindDirection(windDirectionInfo, previousWindDirectionInfo);
        if (windDirectionInfo.id == VAIHTELEVA)
        {
          if (get_period_length(windDirectionPeriod) <= 5)
          {
            si.sentence << ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE;
            si.sentenceParameterTypes.push_back(SentenceParameterType::TIME_PERIOD);
          }
          else
          {
            si.sentence << ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE;
            si.sentenceParameterTypes.push_back(SentenceParameterType::TIME_PERIOD);
          }
        }
        else
        {
          si.sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE;
          si.sentenceParameterTypes.push_back(SentenceParameterType::TIME_PERIOD);
          si.sentenceParameterTypes.push_back(SentenceParameterType::WIND_DIRECTION);
          si.directionChange = windDirectionInfo;
        }
        si.period = windDirectionPeriod;
        sentenceInfoBefore.push_back(si);
        directionChangeStartIndex = j + 1;
        theParameters.theLog << "Direction changes at (before wind speed change): "
                             << windDirectionPeriod.localStartTime() << " < "
                             << windSpeedChangePeriod.localStartTime() << std::endl;

        continue;
      }
      else if (abs(windSpeedChangePeriod.localStartTime().DifferenceInHours(
                   windDirectionPeriod.localStartTime())) < 4)
      {
        // direction changes about same time as wind speed starts to increase / decrease
        if (windDirectionInfo.id != VAIHTELEVA)
          theParameters.theLog
              << "Direction changes about same time as wind speed starts to change "
                 "(report direction along with increasing/decreasing phrase): "
              << windDirectionPeriod.localStartTime() << " ~ "
              << windSpeedChangePeriod.localStartTime() << ", "
              << wind_direction_string(windDirectionInfo.id) << std::endl;

        if (sentenceInfo.intervalSentences.size() == 0) continue;
        if (windDirectionInfo.id != VAIHTELEVA)
        {
          sentenceInfo.directionChange = windDirectionInfo;
        }
        directionChangeStartIndex = j + 1;
      }
      else if (is_inside(windDirectionPeriod.localStartTime(), windSpeedChangePeriod))
      {
        // if no interval sentences,
        if (sentenceInfo.intervalSentences.size() == 0)
        {
          if (lastSentence)
          {
            sentenceInfo.directionChange = windDirectionInfo;
            theParameters.theLog << "No interval sentences for direction change #" << j
                                 << " -> set direction change time to "
                                 << sentenceInfo.directionChange->startTime() << std::endl;

            sentence_info separateSentenceInfo;
            separateSentenceInfo.period = windDirectionPeriod;
            separateSentenceInfo.directionChange = windDirectionInfo;
            separateSentenceInfo.sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE;
            separateSentenceInfo.sentenceParameterTypes.push_back(
                SentenceParameterType::TIME_PERIOD);
            separateSentenceInfo.sentenceParameterTypes.push_back(
                SentenceParameterType::WIND_DIRECTION);
            separateDirectionSentences.push_back(separateSentenceInfo);
            directionChangeStartIndex = j + 1;
          }
        }
        else  // interval sentences exists
          theParameters.theLog << "Wind direction changes during strenghtening/weakening period:"
                               << std::endl;

        // wind direction changes during wind strenghtening/weakening period
        // browse the intervals and inject wind direction change sentences into right places
        for (unsigned int k = 0; k < sentenceInfo.intervalSentences.size(); k++)
        {
          interval_sentence_info& intervalSentenceInfo = sentenceInfo.intervalSentences[k];
          bool firstInterval = (k == 0);
          bool lastInterval = (k == sentenceInfo.intervalSentences.size() - 1);

          // interval does not differ enough, dont report interval, but report direction change
          if (intervalSentenceInfo.skip)
          {
            theParameters.theLog << "Interval sentence #" << k
                                 << " does not differ enough from previous interval -> dont report "
                                    "interval, but report direction change "
                                 << intervalSentenceInfo.intervalInfo << std::endl;
          }

          theParameters.theLog << "Interval sentence #" << k << " -> "
                               << as_string(intervalSentenceInfo.period) << ", "
                               << directionChangeStartIndex << std::endl;

          for (unsigned int h = directionChangeStartIndex; h < directionChanges; h++)
          {
            windDirectionPeriod = theParameters.theWindDirectionPeriods[h];

            if (get_period_length(windDirectionPeriod) < 3)
            {
              theParameters.theLog << "Direction change #" << h
                                   << " period is short: " << get_period_length(windDirectionPeriod)
                                   << ", skipping!" << std::endl;
              continue;
            }

            windDirectionInfo = get_wind_direction(theParameters, windDirectionPeriod);
            theParameters.theLog << "Processing direction #" << h << "("
                                 << as_string(windDirectionPeriod) << ", "
                                 << wind_direction_string(windDirectionInfo.id)
                                 << ") during interval #" << k << " ("
                                 << as_string(intervalSentenceInfo.period) << ", "
                                 << intervalSentenceInfo.intervalInfo << ")" << std::endl;
            bool same_part_of_day =
                (get_part_of_the_day_id_wind(windDirectionPeriod.localStartTime()) ==
                 get_part_of_the_day_id_wind(intervalSentenceInfo.period.localStartTime()));
            unsigned int differenceInHours =
                abs(intervalSentenceInfo.period.localStartTime().DifferenceInHours(
                    windDirectionPeriod.localStartTime()));

            if (windDirectionPeriod.localStartTime() <
                    intervalSentenceInfo.period.localStartTime() &&
                ((differenceInHours > 3 && !same_part_of_day) ||
                 (differenceInHours > 3 &&
                  windDirectionPeriod.localStartTime().GetDay() !=
                      intervalSentenceInfo.period.localStartTime().GetDay())))
            {
              theParameters.theLog << "Direction changes more than 3 hours before speed reporting "
                                      "point -> report it in separate sentence: "
                                   << windDirectionPeriod.localStartTime() << " < "
                                   << intervalSentenceInfo.period.localStartTime() << "("
                                   << intervalSentenceInfo.intervalInfo << ") "
                                   << wind_direction_string(windDirectionInfo.id) << ", "
                                   << as_string(windDirectionInfo.period) << std::endl;

              intervalSentenceInfo.directionChangesBefore.push_back(windDirectionInfo);

              directionChangeStartIndex = h + 1;
            }
            else if ((differenceInHours <= 3 || (differenceInHours < 8 && same_part_of_day) ||
                      (differenceInHours < 6 && lastInterval && lastSentence)) &&
                     !(firstInterval && firstSentence))
            {
              /*
  if (intervalSentenceInfo.directionChange)
  {
    theParameters.theLog << "Only one direction change allowed for interval, skipping: "
                         << wind_direction_string(windDirectionInfo.id) << ", "
                         << as_string(windDirectionInfo.period) << std::endl;
    continue;
  }
              */
              theParameters.theLog << "Direction changes about same time as speed reporting point "
                                      " -> report direction along with interval: "
                                   << windDirectionPeriod.localStartTime() << " ~ "
                                   << intervalSentenceInfo.period.localStartTime() << "("
                                   << intervalSentenceInfo.intervalInfo << ") "
                                   << wind_direction_string(windDirectionInfo.id) << ", "
                                   << as_string(windDirectionInfo.period) << std::endl;

              intervalSentenceInfo.directionChange = windDirectionInfo;
              directionChangeStartIndex = h + 1;
            }
            else if (lastInterval && lastSentence)
            {
              theParameters.theLog << "Direction changes after last interval start time and this "
                                      "is the last sentence (report direction change in separate "
                                      "sentence): "
                                   << windDirectionPeriod.localStartTime() << " > "
                                   << intervalSentenceInfo.period.localStartTime() << "("
                                   << intervalSentenceInfo.intervalInfo << ") "
                                   << wind_direction_string(windDirectionInfo.id) << ", "
                                   << as_string(windDirectionInfo.period) << std::endl;

              intervalSentenceInfo.directionChangesAfter.push_back(windDirectionInfo);

              directionChangeStartIndex = h + 1;
            }
            else
            {
              theParameters.theLog << "Direction period " << as_string(windDirectionPeriod)
                                   << " could not be attached to this interval period "
                                   << as_string(intervalSentenceInfo.period) << std::endl;
            }
          }
        }
      }
    }

    if (i == sentenceInfoVector.size() - 1 && directionChanges - directionChangeStartIndex > 1 &&
        directionChangeStartIndex > 0)
    {
      WeatherPeriod windDirectionPeriodPreviousReported =
          theParameters.theWindDirectionPeriods[directionChangeStartIndex - 1];
      WeatherPeriod windDirectionPeriodLast =
          theParameters.theWindDirectionPeriods[directionChanges - 1];

      // report last even if it is short
      if (get_period_length(windDirectionPeriodLast) > 1 &&
          get_part_of_the_day_id_wind(windDirectionPeriodPreviousReported) !=
              get_part_of_the_day_id_wind(windDirectionPeriodLast))
      {
        WindDirectionInfo windDirectionInfoLast =
            get_wind_direction(theParameters, windDirectionPeriodLast);

        sentence_info si;
        si.sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE;
        si.sentenceParameterTypes.push_back(SentenceParameterType::TIME_PERIOD);
        si.sentenceParameterTypes.push_back(SentenceParameterType::WIND_DIRECTION);
        si.directionChange = windDirectionInfoLast;
        si.period = windDirectionPeriodLast;
        sentenceInfoAfter.push_back(si);
        theParameters.theLog << "Reporting last short direction period: "
                             << as_string(windDirectionPeriodLast) << std::endl;
      }
    }

    if (sentenceInfo.directionChange)
      theParameters.theLog << "No interval sentences for direction change2 "
                           << as_string(sentenceInfo.directionChange->period) << std::endl;
    else
      theParameters.theLog << "No sentenceInfo.directionChange #" << std::endl;

    resultVector.insert(resultVector.end(), sentenceInfoBefore.begin(), sentenceInfoBefore.end());
    resultVector.insert(resultVector.end(), sentenceInfo);
    resultVector.insert(resultVector.end(), sentenceInfoAfter.begin(), sentenceInfoAfter.end());
  }

  sentenceInfoVector = resultVector;
  if (separateDirectionSentences.size() > 0)
  {
    sentenceInfoVector.insert(sentenceInfoVector.end(),
                              separateDirectionSentences.begin(),
                              separateDirectionSentences.end());
  }
}

// check the consecutive intervals and check that same or almost same interval is not reported twice
void WindForecast::checkWindSpeedIntervals(WindSpeedSentenceInfo& sentenceInfoVector) const
{
  if (sentenceInfoVector.size() > 1)
  {
    sentence_info& sentenceInfoFirst = sentenceInfoVector[0];
    sentence_info& sentenceInfoSecond = sentenceInfoVector[1];

    // if wind doesn't change during first period, but changes on the second period, check the
    // difference (testcase: Ahvenanmeri B4W 20170601135000)
    if (sentenceInfoFirst.intervalSentences.size() == 0 &&
        sentenceInfoSecond.intervalSentences.size() == 1)
    {
      interval_info intervalInfoFirst = windSpeedIntervalInfo(sentenceInfoFirst.period);
      const interval_sentence_info& isi = sentenceInfoSecond.intervalSentences[0];
      interval_info intervalInfoSecond = windSpeedIntervalInfo(isi.period);
      if (!wind_speed_differ_enough(intervalInfoFirst, intervalInfoSecond))
      {
        sentenceInfoFirst.period =
            WeatherPeriod(sentenceInfoFirst.period.localStartTime(), isi.period.localEndTime());

        sentenceInfoVector.erase(sentenceInfoVector.begin() + 1);
      }
    }
  }

  // interval_sentence_info* previousIntervalSentenceInfo = 0;
  interval_info intervalInfo;
  interval_info intervalInfoPrevious;
  for (unsigned int i = 0; i < sentenceInfoVector.size(); i++)
  {
    sentence_info& sentenceInfo = sentenceInfoVector[i];

    if (i == 0 && sentenceInfo.intervalSentences.size() == 0)
    {
      intervalInfo = windSpeedIntervalInfo(sentenceInfo.period);
      intervalInfoPrevious = intervalInfo;
    }

    std::set<unsigned int> toRemove;
    for (unsigned int k = 0; k < sentenceInfo.intervalSentences.size(); k++)
    {
      interval_sentence_info& isi = sentenceInfo.intervalSentences[k];

      if (i == 0 || (i > 0 && get_period_length(sentenceInfo.period) <= 6))
        isi.useAlkaenPhrase = (get_period_length(isi.period) > 6);

      interval_info intervalInfoIsi = windSpeedIntervalInfo(isi.period);

      // check if first reporting point differs enough from previous's sentences last interval
      if (k == 0 && i > 0)
      {
        intervalInfoPrevious.peakWind = intervalInfo.upperLimit;

        interval_info intervalInfoIsi2 = intervalInfoIsi;
        intervalInfoIsi2.peakWind = intervalInfoIsi.upperLimit;
        // report direction even if interval is not reported
        if (!wind_speed_differ_enough(intervalInfoIsi2, intervalInfoPrevious))
        {
          if (k == sentenceInfo.intervalSentences.size() - 1)
          {
            sentenceInfo.sentence.clear();
            sentenceInfo.sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE;
            sentenceInfo.changeType = "";
          }
          else
          {
            theParameters.theLog << "Interval " << intervalInfoIsi
                                 << " does not differ enough from " << intervalInfoPrevious
                                 << " -> not reported" << std::endl;
          }

          isi.skip = true;
          continue;
        }
      }

      if (k < sentenceInfo.intervalSentences.size() - 1)
      {
        interval_sentence_info& isiNext = sentenceInfo.intervalSentences[k + 1];
        interval_info intervalInfoIsiNext = windSpeedIntervalInfo(isiNext.period);
        if (intervalInfoIsi.upperLimit != intervalInfoIsi.peakWind) continue;

        intervalInfoIsi.peakWind = intervalInfoIsi.upperLimit;
        intervalInfoIsiNext.peakWind = intervalInfoIsiNext.upperLimit;
        if (!wind_speed_differ_enough(intervalInfoIsi, intervalInfoIsiNext))
        {
          isiNext.skip = true;
        }
      }

      intervalInfoPrevious = intervalInfoIsi;
    }
  }
}

// move or shrink the period till part_of_the_day is different from original
WeatherPeriod WindForecast::getNewPeriod(part_of_the_day_id& partOfTheDay,
                                         const WeatherPeriod& period,
                                         const WeatherPeriod& guardPeriod,
                                         unsigned int& hours,
                                         bool backwards) const
{
  WeatherPeriod movedPeriod = period;
  WeatherPeriod shrunkPeriod = period;
  unsigned int period_len = get_period_length(period);
  hours = 0;
  int hours_moved = 0;
  int hours_shrunk = 0;

  TextGenPosixTime startTime = period.localStartTime();
  TextGenPosixTime endTime = period.localEndTime();
  while (1)
  {
    startTime.ChangeByHours(backwards ? -1 : 1);
    endTime.ChangeByHours(backwards ? -1 : 1);
    if (backwards && startTime <= guardPeriod.localEndTime())
      break;
    else if (!backwards && endTime >= guardPeriod.localStartTime())
      break;

    WeatherPeriod phrasePeriod(startTime, endTime);
    TimePhraseInfo tpi;

    getTimePhrase(phrasePeriod, tpi, get_period_length(phrasePeriod) > 6);
    if (tpi.part_of_the_day != partOfTheDay)
    {
      movedPeriod = WeatherPeriod(startTime, endTime);
      hours_moved = abs(startTime.DifferenceInHours(period.localStartTime()));
      break;
    }
  }

  if (period_len >= 3)
  {
    startTime = period.localStartTime();
    endTime = period.localEndTime();
    // shrink maximum 90 %
    unsigned int max_shrink = max(period_len * 0.9, 1.0);

    for (unsigned int i = 0; i < max_shrink; i++)
    {
      if (backwards)
      {
        endTime.ChangeByHours(-1);
        if (endTime <= guardPeriod.localEndTime()) break;
      }
      else
      {
        startTime.ChangeByHours(1);
        if (startTime >= guardPeriod.localStartTime()) break;
      }

      WeatherPeriod phrasePeriod(startTime, endTime);
      TimePhraseInfo tpi;
      getTimePhrase(phrasePeriod, tpi, get_period_length(phrasePeriod) > 6);

      if (tpi.part_of_the_day != partOfTheDay)
      {
        shrunkPeriod = WeatherPeriod(startTime, endTime);
        if (backwards)
          hours_shrunk = abs(endTime.DifferenceInHours(period.localEndTime()));
        else
          hours_shrunk = abs(startTime.DifferenceInHours(period.localStartTime()));

        break;
      }
    }

    if (hours_moved == 0 && hours_shrunk == 0)
    {
      return period;
    }
    else if (hours_moved == hours_shrunk || hours_moved == 0)
    {
      hours = hours_shrunk;
      return shrunkPeriod;
    }
    else if (hours_shrunk == 0)
    {
      hours = hours_moved;
      return movedPeriod;
    }
    else
    {
      hours = (hours_moved < hours_shrunk ? hours_moved : hours_shrunk);
      return (hours_moved < hours_shrunk ? movedPeriod : shrunkPeriod);
    }
  }

  hours = hours_moved;

  return movedPeriod;
}

// update either period1 (change it to earlier) or period2 (change it to later)
//
// TODO: when moving periods check borders
bool WindForecast::negotiateNewPeriod2(part_of_the_day_id& partOfTheDay,
                                       WeatherPeriod& period1,
                                       WeatherPeriod& period2,
                                       const WeatherPeriod& notEarlierThanPeriod,
                                       const WeatherPeriod& notLaterThanPeriod) const
{
  // get candidates for new period
  unsigned int hoursChanged1 = 0;
  unsigned int hoursChanged2 = 0;
  WeatherPeriod period1Candidate =
      getNewPeriod(partOfTheDay, period1, notEarlierThanPeriod, hoursChanged1, true);
  WeatherPeriod period2Candidate =
      getNewPeriod(partOfTheDay, period2, notLaterThanPeriod, hoursChanged2, false);

  if (hoursChanged1 == 0 && hoursChanged2 == 0) return false;

  if (hoursChanged1 > 0 && hoursChanged2 > 0)
  {
    if (hoursChanged1 <= hoursChanged2)
      period1 = period1Candidate;
    else
      period2 = period2Candidate;
  }
  else if (hoursChanged1 > 0)
    period1 = period1Candidate;
  else if (hoursChanged2 > 0)
    period2 = period2Candidate;

  return true;
}

WeatherPeriod WindForecast::negotiateNewPeriod(part_of_the_day_id& partOfTheDay,
                                               const WeatherPeriod& period) const
{
  WeatherPeriod ret = period;

  TextGenPosixTime newStartTime = period.localStartTime();
  TextGenPosixTime newEndTime = period.localEndTime();
  TimePhraseInfo tpi;
  bool periodUpdated = false;

  for (unsigned int i = 0; i < 6; i++)  // max 6 hours
  {
    newStartTime.ChangeByHours(1);  // roll time forwards 1 hour
    newEndTime.ChangeByHours(1);
    if (newEndTime > theParameters.theForecastPeriod.localEndTime())
    {
      theParameters.theLog << "Can not negotiate new period: forecast period end reached: "
                           << theParameters.theForecastPeriod.localEndTime() << std::endl;
      break;
    }
    WeatherPeriod newPeriod(newStartTime, newEndTime);
    Sentence timePhrase;
    tpi.part_of_the_day = partOfTheDay;
    timePhrase << getTimePhrase(newPeriod, tpi, get_period_length(newPeriod) > 6);
    if (tpi.part_of_the_day != partOfTheDay)
    {
      partOfTheDay = tpi.part_of_the_day;
      ret = newPeriod;
      periodUpdated = true;
      theParameters.theLog << "New period negotiated: " << as_string(ret) << " -> "
                           << part_of_the_day_string(tpi.part_of_the_day) << " -> "
                           << as_string(timePhrase) << std::endl;
      break;
    }
  }

  if (!periodUpdated)
    theParameters.theLog << "Could not negotiate new period: " << as_string(period) << std::endl;

  return ret;
}

// check time phrases, so that same phrase is not used in successive sentences
// if same phrase is in successive sentences -> change either of the phrase to the nearest fitting
// phrase
void WindForecast::checkTimePhrases2(WindSpeedSentenceInfo& sentenceInfoVector) const
{
  std::vector<WeatherPeriod*> timePhrasePeriods;

  theParameters.theLog << "Checking for time phrase tautology" << std::endl;

  // 1) Add all time phrase periods into vector
  TimePhraseInfo previousTPI;
  for (unsigned int i = 0; i < sentenceInfoVector.size(); i++)
  {
    sentence_info& si = sentenceInfoVector[i];
    if (i > 0)
    {
      timePhrasePeriods.push_back(&(si.period));
    }
    for (unsigned int j = 0; j < si.intervalSentences.size(); j++)
    {
      interval_sentence_info& isi = si.intervalSentences[j];
      // direction changes before
      for (auto& isi_dcb : isi.directionChangesBefore)
      {
        timePhrasePeriods.push_back(&(isi_dcb.period));
      }
      if (i == 0 && j == 0) continue;

      // interval period
      timePhrasePeriods.push_back(&(isi.period));
    }
  }

  // 2) Iterate the vector and check for tautology
  int n = timePhrasePeriods.size();
  for (int i = 1; i < n; i++)
  {
    WeatherPeriod* currentPeriod = timePhrasePeriods[i];
    WeatherPeriod* previousPeriod = timePhrasePeriods[i - 1];
    TimePhraseInfo previousTPI;
    TimePhraseInfo currentTPI;
    getTimePhrase(*previousPeriod, previousTPI, get_period_length(*previousPeriod) > 6);
    getTimePhrase(*currentPeriod, currentTPI, get_period_length(*currentPeriod) > 6);

    if (previousTPI.part_of_the_day != MISSING_PART_OF_THE_DAY_ID && previousTPI == currentTPI)
    {
      theParameters.theLog << "Tautology found between ";

      WeatherPeriod period1 = *previousPeriod;
      WeatherPeriod period2 = *currentPeriod;
      theParameters.theLog << as_string(*previousPeriod);
      theParameters.theLog << " and " << as_string(*currentPeriod) << " ("
                           << part_of_the_day_string(currentTPI.part_of_the_day) << ")"
                           << std::endl;

      TextGenPosixTime beforeForecastStartTime = theParameters.theForecastPeriod.localStartTime();
      beforeForecastStartTime.ChangeByHours(-1);
      TextGenPosixTime afterForecastEndTime = theParameters.theForecastPeriod.localEndTime();
      afterForecastEndTime.ChangeByHours(1);
      WeatherPeriod beforePreviousPeriod =
          WeatherPeriod(beforeForecastStartTime, beforeForecastStartTime);
      WeatherPeriod afterCurrentPeriod = WeatherPeriod(afterForecastEndTime, afterForecastEndTime);

      if (i > 1) beforePreviousPeriod = *(timePhrasePeriods[i - 2]);
      if (i < n - 1) afterCurrentPeriod = *(timePhrasePeriods[i + 1]);

      if (negotiateNewPeriod2(currentTPI.part_of_the_day,
                              *previousPeriod,
                              *currentPeriod,
                              beforePreviousPeriod,
                              afterCurrentPeriod))
      {
        if (period1.localStartTime() == previousPeriod->localStartTime() &&
            period1.localEndTime() == previousPeriod->localEndTime())
        {
          theParameters.theLog << "Tautology resolved, period " << as_string(period2) << " ("
                               << part_of_the_day_string(currentTPI.part_of_the_day) << ")";
          TimePhraseInfo tpi;
          getTimePhrase(*currentPeriod, tpi, get_period_length(*currentPeriod) > 6);
          theParameters.theLog << " changed to " << as_string(*currentPeriod) << " ("
                               << part_of_the_day_string(tpi.part_of_the_day) << ")" << std::endl;
        }
        else
        {
          theParameters.theLog << "Tautology resolved, period " << as_string(period1) << " ("
                               << part_of_the_day_string(previousTPI.part_of_the_day) << ")";
          TimePhraseInfo tpi;
          getTimePhrase(*previousPeriod, tpi, get_period_length(*previousPeriod) > 6);
          theParameters.theLog << " changed to " << as_string(*previousPeriod) << " ("
                               << part_of_the_day_string(tpi.part_of_the_day) << ")" << std::endl;
        }
      }
      else
      {
        theParameters.theLog << "Tautology COULD NOT BE resolved between periods "
                             << as_string(*previousPeriod) << " and " << as_string(*currentPeriod)
                             << std::endl;
      }
    }
  }
}

// check time phrases, so that same phrase is not used in successive sentences
// if same phrase is in successive sentences -> change either of the phrase to the nearest fitting
// phrase
void WindForecast::checkTimePhrases(WindSpeedSentenceInfo& sentenceInfoVector) const
{
  TimePhraseInfo previousTPI;
  //  part_of_the_day_id previousPartOfTheDay = MISSING_PART_OF_THE_DAY_ID;
  //  TextGenPosixTime* previousTime = nullptr;
  boost::optional<WeatherPeriod> previousPeriod = boost::none;

  theParameters.theLog << "Checking for time phrase tautology" << std::endl;

  for (auto& si : sentenceInfoVector)
  {
    TimePhraseInfo currentTPI = previousTPI;
    getTimePhrase(si.period, currentTPI, get_period_length(si.period) > 6);

    bool periodUpdated = false;
    if (previousTPI.part_of_the_day != MISSING_PART_OF_THE_DAY_ID && previousTPI == currentTPI)
    {
      theParameters.theLog << "Tautology found: ";

      theParameters.theLog << as_string(*(previousPeriod));
      theParameters.theLog << " and " << as_string(si.period) << " ("
                           << part_of_the_day_string(previousTPI.part_of_the_day) << ") -> ";

      si.period = negotiateNewPeriod(previousTPI.part_of_the_day, si.period);
      periodUpdated = true;
      previousPeriod = si.period;

      previousTPI = currentTPI;
      theParameters.theLog << "New period: " << as_string(si.period) << " ("
                           << part_of_the_day_string(previousTPI.part_of_the_day) << ")"
                           << std::endl;
    }

    for (auto& isi : si.intervalSentences)
    {
      for (auto& isi_dcb : isi.directionChangesBefore)
      {
        getTimePhrase(isi_dcb.period.localStartTime(), currentTPI, false);
        if (previousTPI.part_of_the_day != MISSING_PART_OF_THE_DAY_ID && previousTPI == currentTPI)
        {
          theParameters.theLog << "Tautology found: ";

          theParameters.theLog << as_string(*(previousPeriod));
          theParameters.theLog << " and " << isi_dcb.startTime() << " ("
                               << part_of_the_day_string(previousTPI.part_of_the_day) << ") -> ";

          isi_dcb.period = negotiateNewPeriod(previousTPI.part_of_the_day, isi_dcb.period);

          previousPeriod = isi_dcb.period;  // nullptr;
          previousTPI = currentTPI;
          periodUpdated = true;

          theParameters.theLog << "New time: " << as_string(isi_dcb.period) << " ("
                               << part_of_the_day_string(previousTPI.part_of_the_day) << ")"
                               << std::endl;
        }
      }

      getTimePhrase(isi.period, currentTPI, get_period_length(isi.period) > 6);
      if (previousTPI.part_of_the_day != MISSING_PART_OF_THE_DAY_ID && previousTPI == currentTPI)
      {
        theParameters.theLog << "Tautology found: ";
        theParameters.theLog << as_string(*(previousPeriod));
        theParameters.theLog << " and " << as_string(isi.period) << " ("
                             << part_of_the_day_string(previousTPI.part_of_the_day) << ") -> ";

        isi.period = negotiateNewPeriod(previousTPI.part_of_the_day, isi.period);

        theParameters.theLog << "New period: " << as_string(isi.period) << " ("
                             << part_of_the_day_string(previousTPI.part_of_the_day) << ")"
                             << std::endl;
      }
      previousPeriod = isi.period;
      periodUpdated = true;
      previousTPI = currentTPI;
    }

    if (!periodUpdated && !si.firstSentence)
    {
      previousPeriod = si.period;
      periodUpdated = true;
      previousTPI = currentTPI;
    }
  }
}

// check that wind directionchanges  are not reported too often if direction changes
// a lot at once
void WindForecast::checkWindDirections(WindSpeedSentenceInfo& sentenceInfoVector) const
{
  for (auto& si : sentenceInfoVector)
  {
    std::vector<WindDirectionInfo*> directionChanges;
    if (si.directionChange) directionChanges.push_back(&(*(si.directionChange)));

    for (auto& isi : si.intervalSentences)
    {
      for (auto& dc : isi.directionChangesBefore)
        directionChanges.push_back(&dc);
      if (isi.directionChange) directionChanges.push_back(&(*(isi.directionChange)));
      for (auto& dc : isi.directionChangesAfter)
        directionChanges.push_back(&dc);
    }

    unsigned int step = 3;
    if (directionChanges.size() >= 3)
      for (unsigned int i = 0; i < directionChanges.size(); i += step)
      {
        if (directionChanges.size() - i <= 3) break;
        // ignore varying wind
        if (directionChanges[i]->id == VAIHTELEVA || directionChanges[i + 1]->id == VAIHTELEVA ||
            directionChanges[i + 2]->id == VAIHTELEVA)
        {
          step = 1;
          continue;
        }
        else
        {
          step = 3;
        }

        WeatherResult direction1 = directionChanges[i]->direction;
        WeatherResult direction2 = directionChanges[i + 1]->direction;
        WeatherResult direction3 = directionChanges[i + 2]->direction;

        // if direction changes continuously more than 90 degrees but less than 180 degrees report
        // only first and last direction
        if (((direction1.value() > direction2.value() && direction2.value() > direction3.value()) ||
             (direction1.value() < direction2.value() &&
              direction2.value() < direction3.value())) &&
            fabs(direction3.value() - direction1.value()) >= 90.0 &&
            fabs(direction3.value() - direction1.value()) < 180)
        {
          *(directionChanges[i + 1]) = WindDirectionInfo();  // reset
        }
      }
  }
}
// get paragraph info
ParagraphInfoVector WindForecast::getParagraphInfo(
    const WeatherPeriod& thePeriod, const WindSpeedSentenceInfo& sentenceInfoVector) const
{
  ParagraphInfoVector ret;
  TimePhraseInfo timePhraseInfo;

  for (unsigned int i = 0; i < sentenceInfoVector.size(); i++)
  {
    paragraph_info pi;
    paragraph_info piAfterLastInterval;
    const sentence_info& sentenceInfo = sentenceInfoVector[i];
    pi.sentence << sentenceInfo.sentence;
    for (unsigned int j = 0; j < sentenceInfo.sentenceParameterTypes.size(); j++)
    {
      SentenceParameterType parameterType = sentenceInfo.sentenceParameterTypes[j];

      switch (parameterType)
      {
        case TIME_PERIOD:
        {
          if (i > 0)
          {
            sentence_parameter sp;
            WeatherPeriod period = sentenceInfo.period;
            sp.sentence << getTimePhrase(period, timePhraseInfo, get_period_length(period) > 6);
            sp.tpi = timePhraseInfo;
            pi.sentenceParameters.push_back(sp);
          }
        }
        break;
        case WIND_DIRECTION:
        {
          if (sentenceInfo.directionChange)
          {
            sentence_parameter sp;
            sp.sentence << windDirectionSentence(sentenceInfo.directionChange->id,
                                                 sentenceInfo.useWindBasicForm);
            pi.sentenceParameters.push_back(sp);
          }
          else
          {
            sentence_parameter sp;
            sp.sentence << (sentenceInfo.useWindBasicForm ? TUULI_WORD : TUULTA_WORD);
            pi.sentenceParameters.push_back(sp);
          }
        }
        break;
        case CHANGE_TYPE:
        {
          sentence_parameter sp;
          sp.sentence << sentenceInfo.changeType;
          pi.sentenceParameters.push_back(sp);
        }
        break;
        case CHANGE_SPEED:
        {
          sentence_parameter sp;
          sp.sentence << sentenceInfo.changeSpeed;
          pi.sentenceParameters.push_back(sp);
        }
        break;
        case WIND_SPEED_INTERVAL:
        {
          Sentence intervalSentence;
          for (unsigned int k = 0; k < sentenceInfo.intervalSentences.size(); k++)
          {
            interval_sentence_info isi = sentenceInfo.intervalSentences[k];

            // first report direction changes that happen before interval
            std::vector<sentence_parameter> sps =
                reportWindDirectionChanges2(isi.directionChangesBefore, timePhraseInfo);

            pi.sentenceParameters.insert(pi.sentenceParameters.end(), sps.begin(), sps.end());

            if (isi.skip)
            {
              interval_info intervalInfoIsi = windSpeedIntervalInfo(isi.period);

              // ylimmillään must be reported
              if (intervalInfoIsi.upperLimit < intervalInfoIsi.peakWind)
              {
                sentence_parameter sp1;
                sp1.sentence << Delimiter(COMMA_PUNCTUATION_MARK);
                sentence_parameter sp2;
                sp2.sentence << getTimePhrase(isi.period, timePhraseInfo, isi.useAlkaenPhrase);
                sp2.tpi = timePhraseInfo;
                sentence_parameter sp3;
                sp3.sentence << KOVIMMILLAAN_PHRASE << intervalInfoIsi.peakWind << "m/s";
                pi.sentenceParameters.push_back(sp1);
                pi.sentenceParameters.push_back(sp2);
                pi.sentenceParameters.push_back(sp3);
              }
            }

            if (!isi.skip)
            {
              // then report the interval itself
              WeatherPeriod period = isi.period;
              if (ret.empty() && k == 0)
              {
                if (as_string(sentenceInfo.sentence)
                        .substr(
                            0,
                            strlen(
                                POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_COMPOSITE_PHRASE)) !=
                    POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_COMPOSITE_PHRASE)
                {
                  sentence_parameter sp;
                  sp.sentence << Delimiter(COMMA_PUNCTUATION_MARK) << ALUKSI_WORD;
                  pi.sentenceParameters.push_back(sp);
                }
              }
              else
              {
                Sentence timePhrase;
                TimePhraseInfo tpi = timePhraseInfo;
                timePhrase << getTimePhrase(period, tpi, isi.useAlkaenPhrase);
                if (tpi != timePhraseInfo)
                {
                  sentence_parameter sp;
                  sp.sentence << Delimiter(COMMA_PUNCTUATION_MARK) << timePhrase;
                  timePhraseInfo = tpi;
                  sp.tpi = tpi;
                  pi.sentenceParameters.push_back(sp);
                }
              }

              // direction change may happen same time as interval
              if (isi.directionChange)
              {
                sentence_parameter sp;
                sp.sentence << windDirectionSentence(*(isi.directionChange));
                pi.sentenceParameters.push_back(sp);
              }

              sentence_parameter sp1;
              sp1.sentence << windSpeedIntervalSentence2(
                  period, timePhraseInfo, sentenceInfo.changeType.compare(VOIMISTUVAA_WORD) == 0);
              if (!sp1.sentence.empty())
              {
                sp1.tpi = timePhraseInfo;
                pi.sentenceParameters.push_back(sp1);
              }
            }
            else
            {
              // direction change may happen same time as interval
              if (isi.directionChange)
              {
                sentence_parameter sp;
                Sentence timePhrase;
                TimePhraseInfo tpi = timePhraseInfo;
                timePhrase << getTimePhrase(isi.period, tpi, isi.useAlkaenPhrase);
                if (tpi != timePhraseInfo)
                {
                  sentence_parameter sp;
                  sp.sentence << Delimiter(COMMA_PUNCTUATION_MARK) << timePhrase;
                  timePhraseInfo = tpi;
                  sp.tpi = tpi;
                  pi.sentenceParameters.push_back(sp);
                }
                sp.sentence << windDirectionSentence(*(isi.directionChange));
                pi.sentenceParameters.push_back(sp);
              }
            }

            // first report direction changes that happen after interval
            std::vector<sentence_parameter> sps2 =
                reportWindDirectionChanges2(isi.directionChangesAfter, timePhraseInfo, false);

            piAfterLastInterval.sentenceParameters.insert(
                piAfterLastInterval.sentenceParameters.end(), sps2.begin(), sps2.end());
          }
        }
        break;
      }
    }
    ret.push_back(pi);
    if (!piAfterLastInterval.sentenceParameters.empty()) ret.push_back(piAfterLastInterval);
  }

  return ret;
}

Paragraph WindForecast::constructParagraph(const WeatherPeriod& thePeriod,
                                           const WindSpeedSentenceInfo& sentenceInfoVector) const
{
  Paragraph paragraph;

  TimePhraseInfo timePhraseInfo;

  theParameters.theLog << "CONSTRUCTING SENTENCES" << std::endl;

  for (unsigned int i = 0; i < sentenceInfoVector.size(); i++)
  {
    const sentence_info& sentenceInfo = sentenceInfoVector[i];
    Sentence sentence;
    sentence << sentenceInfo.sentence;

    theParameters.theLog << " Sentence #" << i << ": " << as_string(sentence) << std::endl;

    for (unsigned int j = 0; j < sentenceInfo.sentenceParameterTypes.size(); j++)
    {
      theParameters.theLog << "  sentence parameter #" << j << std::endl;

      SentenceParameterType parameterType = sentenceInfo.sentenceParameterTypes[j];

      switch (parameterType)
      {
        case TIME_PERIOD:
        {
          theParameters.theLog << "   TIME_PERIOD parameter " << as_string(sentenceInfo.period)
                               << std::endl;
          if (i > 0)
          {
            WeatherPeriod period = sentenceInfo.period;
            Sentence timePhrase;
            timePhrase << getTimePhrase(period, timePhraseInfo, get_period_length(period) > 6);
            theParameters.theLog << "   TIME_PERIOD parameter value: " << as_string(timePhrase)
                                 << std::endl;
            sentence << timePhrase;
          }
        }
        break;
        case WIND_DIRECTION:
        {
          theParameters.theLog << "   WIND_DIRECTION parameter " << as_string(sentenceInfo.period)
                               << std::endl;

          if (!sentenceInfo.directionChange && sentenceInfo.intervalSentences.size() > 0)
          {
            theParameters.theLog << "   Handle WIND_DIRECTION parameter along with first interval"
                                 << std::endl;
            continue;
          }

          Sentence directionSentence;
          if (sentenceInfo.directionChange)
          {
            theParameters.theLog << "    direction changes to "
                                 << wind_direction_string(sentenceInfo.directionChange->id)
                                 << " at " << sentenceInfo.directionChange->period.localStartTime()
                                 << std::endl;
            directionSentence << windDirectionSentence(sentenceInfo.directionChange->id,
                                                       sentenceInfo.useWindBasicForm);
          }
          else
          {
            directionSentence << (sentenceInfo.useWindBasicForm ? TUULI_WORD : TUULTA_WORD);
            theParameters.theLog << "    direction sentence: " << as_string(directionSentence)
                                 << std::endl;
          }
          sentence << directionSentence;
        }
        break;
        case CHANGE_TYPE:
        {
          theParameters.theLog << "   CHANGE_TYPE parameter " << as_string(sentenceInfo.period)
                               << ": " << sentenceInfo.changeType << std::endl;
          sentence << sentenceInfo.changeType;
        }
        break;
        case CHANGE_SPEED:
        {
          theParameters.theLog << "   CHANGE_SPEED parameter " << as_string(sentenceInfo.period)
                               << ": " << sentenceInfo.changeSpeed << std::endl;
          sentence << sentenceInfo.changeSpeed;
        }
        break;
        case WIND_SPEED_INTERVAL:
        {
          theParameters.theLog << "   WIND_SPEED_INTERVAL parameter "
                               << as_string(sentenceInfo.period) << std::endl;
          Sentence intervalSentence;

          if (!sentenceInfo.directionChange && sentenceInfo.intervalSentences.size() > 0)
          {
            sentence << (sentenceInfo.useWindBasicForm ? TUULI_WORD : TUULTA_WORD);
            theParameters.theLog << "    adding '"
                                 << (sentenceInfo.useWindBasicForm ? TUULI_WORD : TUULTA_WORD)
                                 << "'-word to the sentence" << std::endl;
          }

          for (unsigned int k = 0; k < sentenceInfo.intervalSentences.size(); k++)
          {
            interval_sentence_info isi = sentenceInfo.intervalSentences[k];

            // first report direction changes that happen before interval
            intervalSentence << reportWindDirectionChanges(isi.directionChangesBefore,
                                                           timePhraseInfo);

            if (!isi.skip)
            {
              // then report the interval itself
              WeatherPeriod period = isi.period;
              if (paragraph.empty() && k == 0)
              {
                if (as_string(sentenceInfo.sentence)
                        .substr(
                            0,
                            strlen(
                                POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_COMPOSITE_PHRASE)) !=
                    POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_COMPOSITE_PHRASE)
                {
                  intervalSentence << Delimiter(COMMA_PUNCTUATION_MARK);
                  intervalSentence << ALUKSI_WORD;
                }
              }
              else
              {
                Sentence timePhraseSentence;
                TimePhraseInfo tpi = timePhraseInfo;
                //
                int endHour = period.localEndTime().GetHour();
                if (get_period_length(period) == 0 &&
                    period.localEndTime() == theParameters.theForecastPeriod.localEndTime() &&
                    (endHour == 6 || endHour == 18))
                {
                  TextGenPosixTime startTime = period.localStartTime();
                  TextGenPosixTime endTime = period.localEndTime();
                  endTime.ChangeByHours(-1);
                  if (endTime < startTime) startTime.ChangeByHours(-1);
                  period = WeatherPeriod(startTime, endTime);
                }
                timePhraseSentence << getTimePhrase(period, tpi, isi.useAlkaenPhrase);
                if (tpi != timePhraseInfo)
                {
                  intervalSentence << Delimiter(COMMA_PUNCTUATION_MARK);
                  intervalSentence << timePhraseSentence;
                  timePhraseInfo = tpi;
                }
                else
                {
                  if (k > 0 && k != sentenceInfo.intervalSentences.size() - 1)
                  {
                    theParameters.theLog
                        << "    Time phrase tautology prevented, dont report interval at "
                        << as_string(period) << std::endl;
                    continue;
                  }
                }
              }

              // direction change may happen same time as interval is reported
              if (isi.directionChange && isi.directionChange->id != MISSING_WIND_DIRECTION_ID)
              {
                intervalSentence << windDirectionSentence(*(isi.directionChange));
                theParameters.theLog << "    direction changes at the same time as interval: "
                                     << as_string(intervalSentence) << std::endl;
              }

              intervalSentence << windSpeedIntervalSentence2(
                  period, timePhraseInfo, sentenceInfo.changeType.compare(VOIMISTUVAA_WORD) == 0);
            }

            // in the end report direction changes that happen after interval
            intervalSentence << reportWindDirectionChanges(isi.directionChangesAfter,
                                                           timePhraseInfo);
            theParameters.theLog << "    interval sentence: " << as_string(intervalSentence)
                                 << std::endl;
          }
          sentence << intervalSentence;
        }
        break;
      }
    }
    theParameters.theLog << "   sentence text #" << i << ": " << as_string(sentence) << std::endl;

    paragraph << sentence;
  }

  return paragraph;
}

Paragraph WindForecast::getWindStory(const WeatherPeriod& thePeriod) const
{
  Paragraph paragraph;
  std::vector<Sentence> sentences;

  WindDirectionInfo previouslyReportedWindDirection;

  theParameters.theLog << "*** WIND DIRECTION REPORTING PERIODS ***" << std::endl;

  BOOST_FOREACH (const WeatherPeriod& period, theParameters.theWindDirectionPeriods)
    theParameters.theLog << as_string(period) << " - "
                         << wind_direction_string(get_wind_direction(theParameters, period).id)
                         << std::endl;

  theParameters.theLog << "*** WIND SPEED REPORTING PERIODS ***" << std::endl;

  BOOST_FOREACH (const WindEventPeriodDataItem* item, theParameters.theWindSpeedEventPeriodVector)
    theParameters.theLog << as_string(item->thePeriod) << " - "
                         << get_wind_event_string(item->theWindEvent) << std::endl;

  std::vector<WindDirectionPeriodInfo> windDirectionPeriodInfo;

  for (auto p : theParameters.theWindDirectionPeriods)
    windDirectionPeriodInfo.push_back(
        WindDirectionPeriodInfo(p, get_wind_direction(theParameters, p)));

  WindSpeedSentenceInfo sentenceInfoVector;
  WindDirectionInfo previousWindDirection;

  for (unsigned int i = 0; i < theParameters.theWindSpeedEventPeriodVector.size(); i++)
  {
    const WindEventPeriodDataItem* windSpeedEventPeriodDataItem =
        theParameters.theWindSpeedEventPeriodVector[i];

    const WindEventPeriodDataItem* nextWindSpeedEventPeriodDataItem =
        (i < theParameters.theWindSpeedEventPeriodVector.size() - 1
             ? theParameters.theWindSpeedEventPeriodVector[i + 1]
             : 0);

    constructWindSentence(windSpeedEventPeriodDataItem,
                          nextWindSpeedEventPeriodDataItem,
                          windDirectionPeriodInfo.front(),
                          previousWindDirection,
                          sentenceInfoVector);

    WeatherPeriod previousDirectionPeriod(windSpeedEventPeriodDataItem->thePeriod.localEndTime(),
                                          windSpeedEventPeriodDataItem->thePeriod.localEndTime());
    previousWindDirection = get_wind_direction(theParameters, previousDirectionPeriod);

    // missing period reported already
    if (nextWindSpeedEventPeriodDataItem &&
        nextWindSpeedEventPeriodDataItem->theWindEvent == MISSING_WIND_SPEED_EVENT)
      i++;
  }

  if (sentenceInfoVector.size() > 0) sentenceInfoVector[0].firstSentence = true;

  checkWindSpeedIntervals(sentenceInfoVector);

  log_sentence_info(
      theParameters.theLog, "SENTENCE INFO BEFORE INJECTING DIRECTIONS", sentenceInfoVector);

  injectWindDirections(sentenceInfoVector);

  log_sentence_info(
      theParameters.theLog, "SENTENCE INFO AFTER INJECTING DIRECTIONS", sentenceInfoVector);

  checkTimePhrases2(sentenceInfoVector);
  //  checkTimePhrases(sentenceInfoVector);

  checkWindDirections(sentenceInfoVector);
  /*
  Paragraph newParagraph =
      constructParagraph(thePeriod, sentenceInfoVector);
  */

  Paragraph newParagraph;
  newParagraph << getParagraphInfo(thePeriod, sentenceInfoVector);

  /*
  BOOST_FOREACH(const Sentence & s, sentences)
  paragraph << s;
  */
  paragraph << newParagraph;

  return paragraph;
}

Sentence WindForecast::getTimePhrase(const TextGenPosixTime& theTime,
                                     TimePhraseInfo& timePhraseInfo,
                                     bool useAlkaenPhrase) const
{
  return getTimePhrase(WeatherPeriod(theTime, theTime), timePhraseInfo, useAlkaenPhrase);
}

Sentence WindForecast::getTimePhrase(const WeatherPeriod& thePeriod,
                                     TimePhraseInfo& timePhraseInfo,
                                     bool useAlkaenPhrase) const
{
  Sentence sentence;
  Sentence timePhrase;
  bool specifyDay = false;
  WeatherPeriod actualPeriod = thePeriod;
  int forecastPeriodLength = get_period_length(theParameters.theForecastPeriod);
  part_of_the_day_id previousPartOfTheDay = timePhraseInfo.part_of_the_day;

  if (get_period_length(thePeriod) == 6 && thePeriod.localStartTime().GetHour() == 12 &&
      thePeriod.localEndTime().GetHour() == 18)
    useAlkaenPhrase = false;

  if (get_period_length(thePeriod) == 0)
  {
    timePhraseInfo.part_of_the_day = get_part_of_the_day_id_wind(thePeriod);
    //    timePhraseInfo.part_of_the_day = get_part_of_the_day_id_wind(thePeriod.localStartTime());
    if (timePhraseInfo.part_of_the_day == KESKIYO)
      specifyDay = false;
    else
      specifyDay = ((forecastPeriodLength > 6 &&
                     theParameters.theForecastPeriod.localStartTime().GetWeekday() !=
                         actualPeriod.localStartTime().GetWeekday()) &&
                    ((timePhraseInfo.day_number != actualPeriod.localStartTime().GetWeekday())));

    std::string plainTimePhrase =
        get_time_phrase_from_id(timePhraseInfo.part_of_the_day, theParameters.theVar, false);
    timePhrase << parse_time_phrase(
        thePeriod.localStartTime().GetWeekday(), specifyDay, plainTimePhrase);

    theParameters.theAlkaenPhraseUsed = false;
  }
  else
  {
    std::string dayPhasePhrase;
    theParameters.theAlkaenPhraseUsed = (get_period_length(thePeriod) >= 6 && useAlkaenPhrase &&
                                         !fit_into_narrow_day_part(thePeriod));

    // try to prevent tautology, like "iltapäivällä"... "iltapäivästä alkaen"
    if (timePhraseInfo.part_of_the_day == get_most_relevant_part_of_the_day_id_narrow(thePeriod) &&
        timePhraseInfo.day_number != MISSING_PART_OF_THE_DAY_ID && get_period_length(thePeriod) > 4)
    {
      TextGenPosixTime startTime(thePeriod.localStartTime());
      startTime.ChangeByHours(2);
      WeatherPeriod shortenedPeriod(startTime, thePeriod.localEndTime());
      theParameters.theAlkaenPhraseUsed =
          (get_period_length(shortenedPeriod) >= 6 && useAlkaenPhrase &&
           !fit_into_narrow_day_part(shortenedPeriod));

      actualPeriod = shortenedPeriod;
    }

    // dont specify day for keskiyö
    if (get_part_of_the_day_id_wind(thePeriod) == KESKIYO ||
        (theParameters.theAlkaenPhraseUsed &&
         get_part_of_the_day_id_wind(thePeriod.localStartTime()) == KESKIYO))
      specifyDay = false;
    else
      specifyDay = ((forecastPeriodLength > 6 &&
                     theParameters.theForecastPeriod.localStartTime().GetWeekday() !=
                         actualPeriod.localStartTime().GetWeekday()) &&
                    ((timePhraseInfo.day_number != actualPeriod.localStartTime().GetWeekday())));

    timePhrase << get_time_phrase_large(actualPeriod,
                                        specifyDay,
                                        theParameters.theVar,
                                        dayPhasePhrase,
                                        useAlkaenPhrase,
                                        timePhraseInfo.part_of_the_day);
  }

  if (timePhraseInfo.part_of_the_day == ILTA &&
      theParameters.theForecastPeriod.localEndTime().GetHour() == 18 &&
      abs(actualPeriod.localStartTime().DifferenceInHours(
          theParameters.theForecastPeriod.localEndTime())) < 6)
  {
    // if the whole forecast ends in the evening, the last phrase can not be 'illalla', but it
    // must
    // be 'iltapaivalla'
    timePhraseInfo.part_of_the_day = ILTAPAIVA;
    timePhrase.clear();
    timePhrase << get_time_phrase_from_id(
        timePhraseInfo.part_of_the_day, theParameters.theVar, useAlkaenPhrase);
  }
  else if (timePhraseInfo.part_of_the_day == AAMU &&
           theParameters.theForecastPeriod.localEndTime().GetHour() == 6 &&
           abs(actualPeriod.localStartTime().DifferenceInHours(
               theParameters.theForecastPeriod.localEndTime())) < 6)
  {
    // if the whole forecast ends in the morning, the last phrase can not be 'aamulla', but it
    // must
    // be 'aamuyolla'
    timePhraseInfo.part_of_the_day = AAMUYO;
    timePhrase.clear();
    timePhrase << get_time_phrase_from_id(
        timePhraseInfo.part_of_the_day, theParameters.theVar, useAlkaenPhrase);
  }

  timePhraseInfo.starttime = actualPeriod.localStartTime();
  timePhraseInfo.endtime = actualPeriod.localEndTime();

  std::string tps = as_string(timePhrase);
  boost::algorithm::to_lower(tps);
  // remove dot and line feed
  tps.erase(tps.end() - 2, tps.end());

  if (tps.find("keskiyo") == std::string::npos)
    timePhraseInfo.day_number = actualPeriod.localStartTime().GetWeekday();

  if (tps.find("aamuyo") != std::string::npos && timePhraseInfo.part_of_the_day != AAMUYO)
  {
    if (previousPartOfTheDay == ILTAYO)
    {
      short lastDayNumberOfPeriod = thePeriod.localStartTime().GetWeekday();

      short newDayNumber = (timePhraseInfo.day_number == 7 ? 1 : timePhraseInfo.day_number + 1);

      if (newDayNumber <= lastDayNumberOfPeriod &&
          !(lastDayNumberOfPeriod == 7 && newDayNumber == 1))
        timePhraseInfo.day_number = newDayNumber;
    }

    if (!isdigit(tps[0]) && specifyDay)
      tps.insert(0, (Fmi::to_string(timePhraseInfo.day_number) + "-"));
    timePhraseInfo.part_of_the_day = AAMUYO;
  }
  if (tps.find("aamuyolla ja aamulla") != std::string::npos)
  {
    tps.replace(tps.find("aamuyolla ja aamulla"), 20, "aamuyolla");
    timePhraseInfo.part_of_the_day = AAMUYO;
  }

  if (tps.find("keskiyo") != std::string::npos)
  {
    // replace keskiyo with aamuyo
    tps.replace(tps.find("keskiyo"), 7, "aamuyo");
    if (forecastPeriodLength > 6 && !isdigit(tps[0]))
    {
      timePhraseInfo.day_number = thePeriod.localEndTime().GetWeekday();
      tps.insert(0, (Fmi::to_string(timePhraseInfo.day_number) + "-"));
    }
    timePhraseInfo.part_of_the_day = AAMUYO;
  }

  sentence << tps;
  /*
  std::cout << "timePhrase: " << as_string(thePeriod) << " --> " << tps << " (" << specifyDay
            << " -> " << timePhraseInfo.day_number << ")"
            << " -> " << as_string(actualPeriod)
            << (useAlkaenPhrase ? " use alkaen phrase" : " dont use alkaen phrase") << " -> "
            << timePhraseInfo << std::endl;
  */
  return sentence;
}

vector<WeatherPeriod> WindForecast::getWindSpeedReportingPeriods(
    const WindEventPeriodDataItem& eventPeriodDataItem, bool firstSentenceInTheStory) const
{
  vector<WeatherPeriod> resultVector;

  WeatherPeriod speedEventPeriod(eventPeriodDataItem.theWindSpeedChangePeriod);

  theParameters.theLog << "Find out wind speed reporting points/period at period "
                       << as_string(speedEventPeriod) << std::endl;

  // find the point of time when max wind has changed theParameters.theWindSpeedThreshold from
  // the previous, take the period from there to the end of the speedEventPeriod
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
                               << as_string(previousWindDataItem.thePeriod) << std::endl;
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
          theParameters.theLog << "Reporting wind speed (case A) at "
                               << as_string(windDataItem.thePeriod) << std::endl;
        }
        previousTopWind = windDataItem.theEqualizedTopWind.value();
        previousTime = windDataItem.thePeriod.localStartTime();
      }
      else
      {
        // add reporting point when speed has changed enough
        if (windDataItem.theEqualizedTopWind.value() > WEAK_WIND_SPEED_UPPER_LIMIT ||
            previousTopWind > WEAK_WIND_SPEED_UPPER_LIMIT)
        {
          WeatherPeriod p(previousTime, windDataItem.thePeriod.localStartTime());
          if (wind_speed_differ_enough(theParameters, p))
          {
            reportingIndexes.push_back(i);
            previousTime = windDataItem.thePeriod.localStartTime();
            theParameters.theLog << "Reporting wind speed (case B) at "
                                 << as_string(windDataItem.thePeriod) << std::endl;
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
        resultVector.push_back(remainingPeriod);
        theParameters.theLog << "Only one reporting point, extend the period to the end "
                             << as_string(remainingPeriod) << std::endl;
      }
      else if (i < reportingIndexes.size() - 1)
      {
        // report wind speed at certain point of time during increasing/decreasing period
        resultVector.push_back(windDataItem.thePeriod);
        theParameters.theLog << "Report speed at certain point of time "
                             << as_string(windDataItem.thePeriod) << std::endl;
      }
      else
      {
        WeatherPeriod lastPeriod(windDataItem.thePeriod.localStartTime(),
                                 speedEventPeriod.localEndTime());
        resultVector.push_back(lastPeriod);
        theParameters.theLog << "Last reporting period " << as_string(lastPeriod) << std::endl;

#ifdef LATER
        // handle last reporting period when there are many reporting points:
        // last reporting point is at the end of the increasing/decreasing period
        // except when period ends in the evening or in the morning, in that case
        // last reporting period starts three hours before increasing/decreasing period ends
        // so that last time phrase in e.g 'iltaan asti'-forecast is 'iltapäivällä'
        TextGenPosixTime startTime(windDataItemLast.thePeriod.localStartTime());
        TextGenPosixTime endTime(windDataItemLast.thePeriod.localEndTime());
        if (endTime.GetHour() == 18 || endTime.GetHour() == 17 || endTime.GetHour() == 6)
        {
          startTime.ChangeByHours(-3);
          if (startTime < eventPeriodDataItem.thePeriod.localStartTime())
            startTime = eventPeriodDataItem.thePeriod.localStartTime();
        }
        WeatherPeriod lastReportingPeriod(startTime, endTime);

        resultVector.push_back(lastReportingPeriod);

        theParameters.theLog << "Last reporting period, special handling "
                             << as_string(windDataItemLast.thePeriod) << ", "
                             << as_string(lastReportingPeriod) << std::endl;
#endif
      }
    }
  }

  vector<WeatherPeriod> retVector;
  // max two intervals reported
  if (resultVector.size() > 0) retVector.push_back(resultVector.front());

  if (resultVector.size() > 1)
  {
    retVector.push_back(resultVector.back());
    if (!firstSentenceInTheStory)
    {
      WeatherPeriod firstPeriod = retVector.front();
      WeatherPeriod lastPeriod = retVector.back();
      if (lastPeriod.localStartTime().GetDay() == lastPeriod.localStartTime().GetDay())
      {
        TimePhraseInfo tpiFirst;
        TimePhraseInfo tpiLast;
        getTimePhrase(firstPeriod, tpiFirst, false);
        getTimePhrase(lastPeriod, tpiLast, false);
        if (tpiFirst == tpiLast)
        {
          retVector.erase(retVector.begin());
          theParameters.theLog
              << "Speed reporting points at the same part of the day -> report only "
                 "the latter period: "
              << as_string(*(retVector.begin())) << std::endl;
        }
      }
    }
  }

  // check the timePhrase of last reporting period:
  // - if forecast period end in the evening the last phrase can not be 'illalla'
  // - if forecast period end in the morning the last phrase can not be 'aamulla'
  if (retVector.size() > 0)
  {
    WeatherPeriod& lastPeriod = retVector.back();
    if (theParameters.theForecastPeriod.localEndTime() == lastPeriod.localEndTime() &&
        get_period_length(lastPeriod) == 1)
    {
      TextGenPosixTime startTime = lastPeriod.localStartTime();
      TextGenPosixTime endTime = lastPeriod.localEndTime();
      // roll back by one hour
      startTime.ChangeByHours(-1);
      theParameters.theLog << "Last speed reporting period changed from " << as_string(lastPeriod)
                           << " to " << as_string(WeatherPeriod(startTime, endTime)) << std::endl;
      lastPeriod = WeatherPeriod(startTime, endTime);
    }
  }
  /*
  std::cout << "speed reporting periods: " << std::endl;
  for (auto item : retVector)
    std::cout << as_string(item) << std::endl;
  */

  return retVector;
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
    theParameters.theLog << "Error: failed to get wind speed change parameters for period "
                         << as_string(changePeriod) << ": " << begLowerLimit << "..."
                         << begUpperLimit << " and " << endLowerLimit << "..." << endUpperLimit
                         << std::endl;
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
    else if (periodLength >= 10)
    {
      phraseStr = VAHITELLEN_WORD;
      gradualChange = true;
    }
  }

  return true;
}

interval_info WindForecast::windSpeedIntervalInfo(const WeatherPeriod& thePeriod) const
{
  interval_info ret;

  float intervalLowerLimit(kFloatMissing);
  float intervalUpperLimit(kFloatMissing);

  get_wind_speed_interval(
      thePeriod, theParameters, intervalLowerLimit, intervalUpperLimit, ret.peakWindTime);

  ret.startTime = thePeriod.localStartTime();
  ret.endTime = thePeriod.localEndTime();
  ret.lowerLimit = round(intervalLowerLimit);
  ret.upperLimit = round(intervalUpperLimit);

  /*
  theParameters.theLog << " rounded interval: " << ret.lowerLimit << "..." << ret.upperLimit
                       << std::endl;
  */

  ret.peakWind = ret.upperLimit;

  compose_wind_speed_range(theParameters, ret.lowerLimit, ret.upperLimit, ret.peakWind);

  theParameters.theLog << " wind speed interval for period " << as_string(thePeriod) << " is "
                       << ret.lowerLimit << "..." << ret.upperLimit << " peak: " << ret.peakWind
                       << std::endl;

  return ret;
}

Sentence WindForecast::speedRangeSentence(const WeatherPeriod& thePeriod,
                                          bool theUseAtItsStrongestPhrase) const
{
  Sentence sentence;

  interval_info intervalInfo = windSpeedIntervalInfo(thePeriod);

  return compose_wind_speed_sentence(theParameters,
                                     intervalInfo.lowerLimit,
                                     intervalInfo.upperLimit,
                                     intervalInfo.peakWind,
                                     theUseAtItsStrongestPhrase);
}

// returns true if id1 is later than id2 false otherwise
bool is_later(part_of_the_day_id id1, part_of_the_day_id id2)
{
  if (id1 == YO)
  {
    if (id1 != YO) return true;
  }
  else if (id1 == AAMUYO)
  {
    if (id2 == KESKIYO) return true;
  }
  else if (id1 == AAMU)
  {
    if (id2 == KESKIYO || id2 == AAMUYO) return true;
  }
  else if (id1 == AAMUPAIVA)
  {
    if (id2 == KESKIYO || id2 == AAMUYO || id2 == AAMU) return true;
  }
  else if (id1 == KESKIPAIVA)
  {
    if (id2 == KESKIYO || id2 == AAMUYO || id2 == AAMU || id2 == AAMUPAIVA) return true;
  }
  else if (id1 == ILTAPAIVA)
  {
    if (id2 == KESKIYO || id2 == AAMUYO || id2 == AAMU || id2 == AAMUPAIVA || id2 == KESKIPAIVA)
      return true;
  }
  else if (id1 == ILTA)
  {
    if (id2 == KESKIYO || id2 == AAMUYO || id2 == AAMU || id2 == AAMUPAIVA || id2 == KESKIPAIVA ||
        id2 == ILTAPAIVA)
      return true;
  }
  else if (id1 == PAIVA)
  {
    if (id2 == KESKIYO || id2 == AAMUYO || id2 == AAMU) return true;
  }

  return false;
}

Sentence WindForecast::speedRangeSentence2(const WeatherPeriod& thePeriod,
                                           TimePhraseInfo& timePhraseInfo,
                                           bool theUseAtItsStrongestPhrase) const
{
  Sentence sentence;

  interval_info intervalInfo = windSpeedIntervalInfo(thePeriod);

  Sentence timePhrase;
  TimePhraseInfo tpi;
  tpi.day_number = timePhraseInfo.day_number;
  tpi.part_of_the_day = timePhraseInfo.part_of_the_day;

  timePhrase << getTimePhrase(intervalInfo.peakWindTime, tpi, false);

  if (tpi != timePhraseInfo && theUseAtItsStrongestPhrase)
  {
    if (timePhraseInfo.starttime.GetDay() == intervalInfo.peakWindTime.GetDay() &&
        is_later(tpi.part_of_the_day, timePhraseInfo.part_of_the_day))
      timePhraseInfo = tpi;
    else
      timePhrase.clear();
  }
  else
    timePhrase.clear();

  return compose_wind_speed_sentence(
      theParameters, intervalInfo, timePhrase, theUseAtItsStrongestPhrase);
}

Sentence WindForecast::windSpeedIntervalSentence(const WeatherPeriod& thePeriod,
                                                 bool theUseAtItsStrongestPhrase /* = true*/) const
{
  Sentence sentence;

  sentence << speedRangeSentence(thePeriod, theUseAtItsStrongestPhrase);

  return sentence;
}

Sentence WindForecast::windSpeedIntervalSentence2(const WeatherPeriod& thePeriod,
                                                  TimePhraseInfo& tpi,
                                                  bool theUseAtItsStrongestPhrase /* = true*/) const
{
  Sentence sentence;

  sentence << speedRangeSentence2(thePeriod, tpi, theUseAtItsStrongestPhrase);

  return sentence;
}

Sentence WindForecast::reportWindDirectionChanges(
    const std::vector<WindDirectionInfo>& directionChanges, TimePhraseInfo& timePhraseInfo) const
{
  Sentence s;
  for (auto directionInfo : directionChanges)
  {
    if (directionInfo.id == MISSING_WIND_DIRECTION_ID) continue;
    s << Delimiter(COMMA_PUNCTUATION_MARK);
    s << getTimePhrase(
        directionInfo.period, timePhraseInfo, get_period_length(directionInfo.period) > 6);
    s << windDirectionSentence(directionInfo.id);
  }

  return s;
}

std::vector<sentence_parameter> WindForecast::reportWindDirectionChanges2(
    const std::vector<WindDirectionInfo>& directionChanges,
    TimePhraseInfo& timePhraseInfo,
    bool startWithComma /*= true*/) const
{
  std::vector<sentence_parameter> ret;

  for (auto directionInfo : directionChanges)
  {
    if (ret.empty() && startWithComma)
    {
      sentence_parameter sp;
      sp.sentence << Delimiter(COMMA_PUNCTUATION_MARK);
      ret.push_back(sp);
    }

    sentence_parameter sp2;
    sp2.sentence << getTimePhrase(
        directionInfo.period, timePhraseInfo, get_period_length(directionInfo.period) > 6);
    sp2.tpi = timePhraseInfo;
    ret.push_back(sp2);

    sentence_parameter sp3;
    sp3.sentence << windDirectionSentence(directionInfo);
    ret.push_back(sp3);
  }

  return ret;
}

}  // namespace TextGen
