// ======================================================================
/*!
 * \file
 * \brief Implementation of WindForecast class
 */
// ======================================================================

#include "WindForecast.h"
#include "Delimiter.h"
#include "PositiveRange.h"
#include "UnitFactory.h"
#include <boost/algorithm/string/case_conv.hpp>
#include <calculator/Settings.h>
#include <macgyver/StringConversion.h>
#include <algorithm>

// #define BOOST_STACKTRACE_USE_ADDR2LINE
#include <boost/stacktrace.hpp>

namespace TextGen
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

part_of_the_day_id get_most_relevant_part_of_the_day_id_narrow(const WeatherPeriod& thePeriod);

part_of_the_day_id get_part_of_the_day_id_wind(const TextGenPosixTime& theTime)
{
  int hour = theTime.GetHour();

  // Note the reversed order. The periods overlap, and we prefer the later period name

  if (hour >= ILTA_START && hour <= YO_START)
    return ILTA;
  if (hour >= ILTAPAIVA_START && hour <= ILTAPAIVA_END)
    return ILTAPAIVA;
  if (hour >= AAMUPAIVA_START && hour < ILTAPAIVA_START)
    return AAMUPAIVA;
  if (hour >= AAMU_START && hour < AAMUPAIVA_START)
    return AAMU;
  if (hour >= KESKIYO_START && hour < AAMU_START)
    return AAMUYO;
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
  std::array<const char*, 7> weekdays = {
      "maanantai", "tiistai", "keskiviikko", "rorstai", "perjantai", "lauantai", "sunnuntai"};

  os << weekdays[tpi.day_number - 1] << part_of_the_day_string(tpi.part_of_the_day) << '\n';

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

  for (auto* i : theWindDataVector)
  {
    WindDataItemUnit& item = i->getDataItem(theArea.type());
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
  for (const WindDataItemsByArea* item : theParameters.theWindDataVector)
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
          if (!upper_index_updated || k > upper_index)
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
                                           const std::string& /*var*/)
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
                   const std::string& /*var*/)
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
                   const std::string& /*var*/)
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
                                     const WindDirectionInfo* /*thePreviousWindDirection*/ /*= 0*/)
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
                                     const WindDirectionInfo* /*thePreviousWindDirection*/ /*= 0*/)
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
  // Lookup table: maps direction id to {basic_form, partitive_form}
  static const std::map<WindDirectionId, std::pair<const char*, const char*>> directionPhrases = {
      {POHJOINEN, {POHJOINEN_TUULI, POHJOINEN_TUULI_P}},
      {POHJOISEN_PUOLEINEN, {POHJOISEN_PUOLEINEN_TUULI, POHJOISEN_PUOLEINEN_TUULI_P}},
      {POHJOINEN_KOILLINEN, {POHJOINEN_KOILLINEN_TUULI, POHJOINEN_KOILLINEN_TUULI_P}},
      {KOILLINEN, {KOILLINEN_TUULI, KOILLINEN_TUULI_P}},
      {KOILLISEN_PUOLEINEN, {KOILLISEN_PUOLEINEN_TUULI, KOILLISEN_PUOLEINEN_TUULI_P}},
      {ITA_KOILLINEN, {ITA_KOILLINEN_TUULI, ITA_KOILLINEN_TUULI_P}},
      {ITA, {ITA_TUULI, ITA_TUULI_P}},
      {IDAN_PUOLEINEN, {IDAN_PUOLEINEN_TUULI, IDAN_PUOLEINEN_TUULI_P}},
      {ITA_KAAKKO, {ITA_KAAKKO_TUULI, ITA_KAAKKO_TUULI_P}},
      {KAAKKO, {KAAKKO_TUULI, KAAKKO_TUULI_P}},
      {KAAKON_PUOLEINEN, {KAAKON_PUOLEINEN_TUULI, KAAKON_PUOLEINEN_TUULI_P}},
      {ETELA_KAAKKO, {ETELA_KAAKKO_TUULI, ETELA_KAAKKO_TUULI_P}},
      {ETELA, {ETELA_TUULI, ETELA_TUULI_P}},
      {ETELAN_PUOLEINEN, {ETELAN_PUOLEINEN_TUULI, ETELAN_PUOLEINEN_TUULI_P}},
      {ETELA_LOUNAS, {ETELA_LOUNAS_TUULI, ETELA_LOUNAS_TUULI_P}},
      {LOUNAS, {LOUNAS_TUULI, LOUNAS_TUULI_P}},
      {LOUNAAN_PUOLEINEN, {LOUNAAN_PUOLEINEN_TUULI, LOUNAAN_PUOLEINEN_TUULI_P}},
      {LANSI_LOUNAS, {LANSI_LOUNAS_TUULI, LANSI_LOUNAS_TUULI_P}},
      {LANSI, {LANSI_TUULI, LANSI_TUULI_P}},
      {LANNEN_PUOLEINEN, {LANNEN_PUOLEINEN_TUULI, LANNEN_PUOLEINEN_TUULI_P}},
      {LANSI_LUODE, {LANSI_LUODE_TUULI, LANSI_LUODE_TUULI_P}},
      {LUODE, {LUODE_TUULI, LUODE_TUULI_P}},
      {LUOTEEN_PUOLEINEN, {LUOTEEN_PUOLEINEN_TUULI, LUOTEEN_PUOLEINEN_TUULI_P}},
      {POHJOINEN_LUODE, {POHJOINEN_LUODE_TUULI, POHJOINEN_LUODE_TUULI_P}},
      {VAIHTELEVA, {VAIHTELEVA_TUULI, VAIHTELEVA_TUULI_P}},
  };

  Sentence sentence;
  auto it = directionPhrases.find(theWindDirectionId);
  if (it != directionPhrases.end())
    sentence << (theBasicForm ? it->second.first : it->second.second);
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
        minValue = std::min(windDataItem.theEqualizedMedianWind.value(), minValue);
        topValue = std::max(windDataItem.theEqualizedTopWind.value(), topValue);
        maxValue = std::max(windDataItem.theEqualizedMaxWind.value(), maxValue);
      }
    }
  }
  lowerLimit = minValue;
  upperLimit = (topValue > maxValue ? topValue : maxValue);
  // if top wind is smaller than configuration value, use maximum wind as upper limit
  if (topValue < theParameter.theWindSpeedWarningThreshold)
  {
    // Annakaisa / Punkka 06.06.2016: pienennetään maksimikoko neljään
    theParameter.theLog << "Top wind speed (" << topValue
                        << ") is smaller than theWindSpeedWarningThreshold ("
                        << theParameter.theWindSpeedWarningThreshold
                        << ") setting interval maximum size to 4.\n";
    theParameter.theContextualMaxIntervalSize = 4;
    upperLimit = maxValue;
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
  if (lowerLimit == kFloatMissing || upperLimit == kFloatMissing)
    return;

  value_distribution_data_vector windSpeedDistributionVector;

  // first calculte the sum of the distribution values for the period for each wind speed
  unsigned int counter(0);
  for (unsigned int i = 0; i < theParameter.theWindDataVector.size(); i++)
  {
    const WindDataItemUnit& windDataItem =
        (*theParameter.theWindDataVector[i])(theParameter.theArea.type());

    if (is_inside(windDataItem.thePeriod, thePeriod))
    {
      if (windSpeedDistributionVector.empty())
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
    if (i > 0 && windSpeedDistributionVector[mostTypicalWindSpeed].second.value() <
                     windSpeedDistributionVector[i].second.value())
      mostTypicalWindSpeed = i;
  }

  // Make sure that there is enough values (coverage) inside the interval
  int lower_index = std::lround(upperLimit);
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
  // std::stringstream ss_log;

  //  ss_log << " wind_speed_interval:\n period: " << as_string(thePeriod) << '\n';

  // first get plain interval: smallest median wind on period, highest top wind or maximum wind on
  // period
  float plainLowerLimit = lowerLimit;
  float plainUpperLimit = upperLimit;
  get_plain_wind_speed_interval(thePeriod, theParameter, plainLowerLimit, plainUpperLimit);

  //  ss_log << " plain interval: " << plainLowerLimit << "..." << plainUpperLimit << '\n';

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
         << distributionUpperLimit67 << " most typical: " << mostTypicalWindSpeed << '\n';
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
       << distributionUpperLimit50 << " most typical: " << mostTypicalWindSpeed << '\n';
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
         << " peak: " << peakWind << '\n';
  */

  upperLimit = std::min<float>(peakWind, upperLimit);

  /*
  ss_log << " interval after peak check: " << lowerLimit << "..." << upperLimit << '\n';

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
  double medianWindWeight = 1.0 - topWindWeight;

  return (upperLimit * topWindWeight + lowerLimit * medianWindWeight);
}

bool wind_speed_differ_enough(wo_story_params& theParameters, const WeatherPeriod& thePeriod)
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

  return (lowerLimitDifference + upperLimitDifference > 3);
}

bool wind_speed_differ_enough(const interval_info& firstInterval,
                              const interval_info& secondInterval,
                              int contextualMaxIntervalSize,
                              bool useAtItsStrongestPhrase)
{
  if (firstInterval.upperLimit == firstInterval.peakWind &&
      secondInterval.upperLimit == secondInterval.peakWind)
    return wind_speed_differ_enough(firstInterval, secondInterval);

  unsigned int firstStartValue = 0;
  unsigned int firstEndValue = 0;
  unsigned int firstTopValue = 0;
  unsigned int secondStartValue = 0;
  unsigned int secondEndValue = 0;
  unsigned int secondTopValue = 0;

  if (firstInterval.peakWind > firstInterval.upperLimit)
  {
    if (useAtItsStrongestPhrase)
    {
      firstStartValue = firstInterval.lowerLimit;
      firstEndValue = firstInterval.upperLimit;
      firstTopValue = firstInterval.peakWind;
    }
    else
    {
      firstStartValue = firstInterval.peakWind - contextualMaxIntervalSize;
      firstEndValue = firstInterval.peakWind;
    }
  }
  else
  {
    firstStartValue = firstInterval.lowerLimit;
    firstEndValue = firstInterval.upperLimit;
  }

  if (secondInterval.peakWind > secondInterval.upperLimit)
  {
    if (useAtItsStrongestPhrase)
    {
      secondStartValue = secondInterval.lowerLimit;
      secondEndValue = secondInterval.upperLimit;
      secondTopValue = secondInterval.peakWind;
    }
    else
    {
      secondStartValue = secondInterval.peakWind - contextualMaxIntervalSize;
      secondEndValue = secondInterval.peakWind;
    }
  }
  else
  {
    secondStartValue = secondInterval.lowerLimit;
    secondEndValue = secondInterval.upperLimit;
  }

  auto lowerLimitDifference = (firstStartValue - secondStartValue);
  auto upperLimitDifference = (firstEndValue - secondEndValue);

  if (firstTopValue == 0 && secondTopValue == 0)
    return (lowerLimitDifference + upperLimitDifference > 3);
  return (firstStartValue + firstEndValue + firstTopValue !=
          secondStartValue + secondEndValue + secondTopValue);
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

  if (directionIdBeg == directionIdEnd)
    return false;

  return wind_direction_differ_enough(
      windDirection1, windDirection2, theParameter.theWindDirectionThreshold);
}

void compose_wind_speed_range(const wo_story_params& theParameters,
                              int& intervalLowerLimit,
                              int& intervalUpperLimit,
                              int& peakWind)
{
  int actualIntervalSize(intervalUpperLimit - intervalLowerLimit);
  std::stringstream ss_log;
  ss_log << '\n';

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

    ss_log << intervalLowerLimit << "..." << intervalUpperLimit << '\n';
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
    ss_log << intervalLowerLimit << "..." << intervalUpperLimit << '\n';
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
           << intervalLowerLimit << "..." << intervalUpperLimit << '\n';
  }

  if (ss_log.str().size() > 1)
    theParameters.theLog << ss_log.str();
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
      sentence << PositiveRange(intervalInfo.lowerLimit,
                               intervalInfo.upperLimit,
                               theParameters.theRangeSeparator)
               << *UnitFactory::create(MetersPerSecond);
      sentence << Delimiter(COMMA_PUNCTUATION_MARK) << timePhrase << KOVIMMILLAAN_PHRASE
               << intervalInfo.peakWind << *UnitFactory::create(MetersPerSecond);
    }
    else
    {
      sentence << PositiveRange(intervalInfo.peakWind - theParameters.theContextualMaxIntervalSize,
                               intervalInfo.peakWind,
                               theParameters.theRangeSeparator)
               << *UnitFactory::create(MetersPerSecond);
    }
  }
  else
  {
    sentence << PositiveRange(intervalInfo.lowerLimit,
                             intervalInfo.upperLimit,
                             theParameters.theRangeSeparator)
             << *UnitFactory::create(MetersPerSecond);
  }

  return sentence;
}

WindForecast::WindForecast(wo_story_params& parameters) : theParameters(parameters) {}

// Build a simple direction+speed sentence for weak or missing-speed periods
sentence_info WindForecast::buildSimpleSentenceInfo(
    const WeatherPeriod& period,
    const WindDirectionPeriodInfo& firstDirectionPeriodInfo,
    bool firstSentence) const
{
  sentence_info sentenceInfo;
  sentenceInfo.period = period;
  sentenceInfo.directionChange = get_wind_direction(
      theParameters, (firstSentence ? firstDirectionPeriodInfo.period : period));
  interval_sentence_info isi;
  isi.period = period;
  isi.intervalInfo = windSpeedIntervalInfo(period);
  sentenceInfo.intervalSentences.push_back(isi);
  sentenceInfo.sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE << EMPTY_STRING;
  sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::WIND_DIRECTION);
  sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::WIND_SPEED_INTERVAL);
  return sentenceInfo;
}

// Compute the reporting periods for a strengthening/weakening event, combining with the next
// period if it has no wind speed event of its own.
std::vector<WeatherPeriod> WindForecast::computeReportingPeriods(
    const WindEventPeriodDataItem* windSpeedItem,
    const WindEventPeriodDataItem* nextWindSpeedItem,
    bool firstSentence,
    const WeatherPeriod& windSpeedEventPeriod) const
{
  bool nextPeriodIsMissing =
      (nextWindSpeedItem && nextWindSpeedItem->theWindEvent == MISSING_WIND_SPEED_EVENT);

  std::vector<WeatherPeriod> periods;
  if (nextPeriodIsMissing)
  {
    WeatherPeriod combinedPeriod(windSpeedItem->thePeriod.localStartTime(),
                                 nextWindSpeedItem->thePeriod.localEndTime());
    theParameters.theLog << "Reporting next period here also, combined period is: "
                         << as_string(combinedPeriod) << '\n';
    WindEventPeriodDataItem combinedItem(combinedPeriod, windSpeedItem->theWindEvent,
                                        windSpeedItem->thePeriodBeginDataItem,
                                        nextWindSpeedItem->thePeriodEndDataItem);
    periods = getWindSpeedReportingPeriods(combinedItem, firstSentence);
  }
  else
  {
    periods = getWindSpeedReportingPeriods(*windSpeedItem, firstSentence);
  }

  if (periods.empty())
  {
    TextGenPosixTime sTime = windSpeedEventPeriod.localStartTime();
    TextGenPosixTime eTime = windSpeedEventPeriod.localEndTime();
    if (get_period_length(windSpeedEventPeriod) > 3)
    {
      sTime = eTime;
      sTime.ChangeByHours(-3);
    }
    WeatherPeriod p(sTime, eTime);
    periods.push_back(p);
    theParameters.theLog << "No wind speed reporting periods found -> reporting at end: "
                         << as_string(p) << '\n';
  }
  return periods;
}

void WindForecast::constructWindSentence(const WindEventPeriodDataItem* windSpeedItem,
                                         const WindEventPeriodDataItem* nextWindSpeedItem,
                                         const WindDirectionPeriodInfo& firstDirectionPeriodInfo,
                                         WindDirectionInfo& thePreviousWindDirection,
                                         WindSpeedSentenceInfo& sentenceInfoVector) const
{
  const WeatherPeriod& windSpeedEventPeriod = windSpeedItem->thePeriod;
  bool firstSentence = (sentenceInfoVector.empty());

  theParameters.theLog << "This is #" << sentenceInfoVector.size() << " sentence.\n";

  // we dont report speed and direction changes on weak period
  if (is_weak_period(theParameters, windSpeedEventPeriod))
  {
    bool middleNonFullPeriod = !firstSentence && get_period_length(windSpeedEventPeriod) !=
                                                     get_period_length(theParameters.theForecastPeriod);
    if (middleNonFullPeriod)
    {
      theParameters.theLog << "Wind is weak on period " << as_string(windSpeedEventPeriod)
                           << " -> period is not reported\n";
    }
    else
    {
      theParameters.theLog << "Wind is weak on period " << as_string(windSpeedEventPeriod)
                           << " -> reporting the whole period at once!\n";
      sentenceInfoVector.push_back(
          buildSimpleSentenceInfo(windSpeedEventPeriod, firstDirectionPeriodInfo, firstSentence));
    }
    return;
  }

  WindEventId windSpeedEventId = windSpeedItem->theWindEvent;
  bool windStrenghtens = (windSpeedEventId == TUULI_VOIMISTUU);
  bool windWeakens = (windSpeedEventId == TUULI_HEIKKENEE);

  if (windSpeedEventId == MISSING_WIND_SPEED_EVENT)
  {
    theParameters.theLog << "Processing MISSING_WIND_SPEED_EVENT at "
                         << as_string(windSpeedEventPeriod) << firstSentence << "\n";
    if (firstSentence && get_period_length(windSpeedEventPeriod) > 3)
      sentenceInfoVector.push_back(
          buildSimpleSentenceInfo(windSpeedEventPeriod, firstDirectionPeriodInfo, firstSentence));
    return;
  }

  if (!windStrenghtens && !windWeakens)
    return;

  theParameters.theLog << (windStrenghtens ? "Processing TUULI_VOIMISTUU event at "
                                           : "Processing TUULI_HEIKKENEE event at ")
                       << as_string(windSpeedEventPeriod) << '\n';

  std::vector<WeatherPeriod> windSpeedReportingPeriods = computeReportingPeriods(
      windSpeedItem, nextWindSpeedItem, firstSentence, windSpeedEventPeriod);

  bool smallChange = false;
  bool gradualChange = false;
  bool fastChange = false;
  sentence_info sentenceInfo;
  sentenceInfo.changeSpeed = EMPTY_STRING;
  getWindSpeedChangeAttribute(
      windSpeedEventPeriod, sentenceInfo.changeSpeed, smallChange, gradualChange, fastChange);
  if (windWeakens && sentenceInfo.changeSpeed == VAHAN_WORD)
    sentenceInfo.changeSpeed = EMPTY_STRING;
  sentenceInfo.period = windSpeedEventPeriod;

  for (unsigned int i = 0; i < windSpeedReportingPeriods.size(); i++)
  {
    const WeatherPeriod& period = windSpeedReportingPeriods[i];
    interval_sentence_info isi;
    isi.period = period;
    isi.intervalInfo = windSpeedIntervalInfo(period);
    if (i == 0)
      sentenceInfo.intervalSentences.push_back(isi);
    else
      sentenceInfoVector.back().intervalSentences.push_back(isi);

    theParameters.theLog << "Reporting "
                         << (windStrenghtens ? "strenghtening wind at " : "weakening wind at ")
                         << as_string(period) << '\n';

    if (i == 0)
    {
      populateFirstReportingPointSentence(sentenceInfo, firstSentence, windStrenghtens,
                                          windSpeedItem, firstDirectionPeriodInfo,
                                          thePreviousWindDirection, windSpeedEventPeriod);
      sentenceInfoVector.push_back(sentenceInfo);
    }
  }
}

void WindForecast::populateFirstReportingPointSentence(
    sentence_info& sentenceInfo,
    bool firstSentence,
    bool windStrenghtens,
    const WindEventPeriodDataItem* windSpeedItem,
    const WindDirectionPeriodInfo& firstDirectionPeriodInfo,
    WindDirectionInfo& thePreviousWindDirection,
    const WeatherPeriod& windSpeedEventPeriod) const
{
  auto addTimePeriodDirectionSpeed = [&]() {
    sentenceInfo.sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE;
    sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::TIME_PERIOD);
    sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::CHANGE_SPEED);
    sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::CHANGE_TYPE);
    sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::WIND_DIRECTION);
  };

  if (firstSentence)
  {
    sentenceInfo.directionChange = firstDirectionPeriodInfo.info;
    theParameters.theLog << "First sentence -> period is " << as_string(windSpeedEventPeriod)
                         << '\n';
    WindDirectionInfo windDirection = firstDirectionPeriodInfo.info;
    negotiateWindDirection(windDirection, thePreviousWindDirection);

    if (windDirection.id == VAIHTELEVA)
    {
      // varying wind can not weaken
      if (windStrenghtens)
      {
        sentenceInfo.sentence << POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_COMPOSITE_PHRASE;
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
      addTimePeriodDirectionSpeed();
    }
  }
  else if (sentenceInfo.changeSpeed == EMPTY_STRING)
  {
    addTimePeriodDirectionSpeed();
  }
  else if (windSpeedItem->theSuccessiveEventFlag)
  {
    sentenceInfo.sentence << ILTAPAIVALLA_EDELLEEN_HEIKKENEVAA_POHJOISTUULTA_COMPOSITE_PHRASE;
    sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::TIME_PERIOD);
    sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::CHANGE_TYPE);
    sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::WIND_DIRECTION);
  }
  else
  {
    addTimePeriodDirectionSpeed();
  }

  sentenceInfo.changeType = (windStrenghtens ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD);
  sentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::WIND_SPEED_INTERVAL);
}

void logDirectionList(std::ostream& theOutput,
                      const char* label,
                      const std::vector<WindDirectionInfo>& directions)
{
  if (directions.empty())
    return;
  theOutput << label;
  for (const auto& direction : directions)
    theOutput << "   " << direction.startTime() << " (" << wind_direction_string(direction.id)
              << ")\n";
}

void logIntervalSentenceInfo(std::ostream& theOutput, const interval_sentence_info& isi,
                             unsigned int index)
{
  theOutput << "  interval #" << index << '\n'
            << "   sentence: " << as_string(isi.sentence) << '\n'
            << "   period: " << as_string(isi.period) << '\n'
            << "   interval_info: " << isi.intervalInfo << '\n'
            << "   alkaen: " << isi.useAlkaenPhrase << '\n'
            << "   skip: " << isi.skip << '\n';
  logDirectionList(theOutput, "   direction changes before interval at: \n",
                   isi.directionChangesBefore);
  if (isi.directionChange)
    theOutput << "   direction changes same time as interval at: "
              << isi.directionChange->startTime() << " ("
              << wind_direction_string(isi.directionChange->id) << ")\n";
  logDirectionList(theOutput, "   direction changes after interval at: \n",
                   isi.directionChangesAfter);
}

void logSentenceParameter(std::ostream& theOutput,
                          SentenceParameterType parameterType,
                          const sentence_info& sentenceInfo)
{
  switch (parameterType)
  {
    case TIME_PERIOD:
      theOutput << "  TIME_PERIOD: " << sentenceInfo.period.localStartTime() << "..."
                << sentenceInfo.period.localEndTime() << '\n';
      break;
    case WIND_DIRECTION:
      if (sentenceInfo.directionChange)
        theOutput << "  WIND_DIRECTION: "
                  << sentenceInfo.directionChange->period.localStartTime() << " ("
                  << wind_direction_string(sentenceInfo.directionChange->id) << ")\n";
      else
        theOutput << "  WIND_DIRECTION: "
                  << (sentenceInfo.useWindBasicForm ? "use basic form: tuuli"
                                                    : "use partitive form: tuulta")
                  << '\n';
      break;
    case CHANGE_TYPE:
      theOutput << "  CHANGE_TYPE: " << sentenceInfo.changeType << '\n';
      break;
    case CHANGE_SPEED:
      theOutput << "  CHANGE_SPEED: " << sentenceInfo.changeSpeed << '\n';
      break;
    case WIND_SPEED_INTERVAL:
    {
      theOutput << "  WIND_SPEED_INTERVAL: \n";
      unsigned int index = 0;
      for (const auto& isi : sentenceInfo.intervalSentences)
        logIntervalSentenceInfo(theOutput, isi, index++);
      break;
    }
    case VOID_TYPE:
      theOutput << "  VOID_TYPE: \n";
      break;
  }
}

std::ostream& operator<<(std::ostream& theOutput, const sentence_info& sentenceInfo)
{
  theOutput << "   " << as_string(sentenceInfo.sentence) << '\n';

  if (sentenceInfo.directionChange)
    theOutput << "  direction changes same time as wind speed starts to change: "
              << sentenceInfo.directionChange->startTime() << " ~ "
              << sentenceInfo.period.localStartTime() << '\n';

  theOutput << "  sentenceInfo.skip: " << sentenceInfo.skip << '\n';

  for (SentenceParameterType parameterType : sentenceInfo.sentenceParameterTypes)
    logSentenceParameter(theOutput, parameterType, sentenceInfo);

  return theOutput;
}

void log_sentence_info(MessageLogger& theLog,
                       const std::string& theHeader,
                       const WindSpeedSentenceInfo& sentenceInfoVector)
{
  theLog << theHeader << '\n';
  for (unsigned int i = 0; i < sentenceInfoVector.size(); i++)
    theLog << " sentence #" << i << ": \n" << sentenceInfoVector[i] << '\n';
}

// originally story contains just wind speed changes
// this function injects wind direction changes into story
// Assign a single direction change (h) to the correct place within an interval sentence
void WindForecast::assignDirectionToInterval(interval_sentence_info& intervalSentenceInfo,
                                             unsigned int h,
                                             unsigned int& directionChangeStartIndex,
                                             bool firstInterval,
                                             bool firstSentence,
                                             bool lastInterval,
                                             bool lastSentence) const
{
  WeatherPeriod windDirectionPeriod = theParameters.theWindDirectionPeriods[h];
  int directionPeriodLen = get_period_length(windDirectionPeriod);

  if (directionPeriodLen < 3)
  {
    theParameters.theLog << "Direction change #" << h
                         << " period is short: " << directionPeriodLen << ", skipping!\n";
    return;
  }

  WindDirectionInfo windDirectionInfo = get_wind_direction(theParameters, windDirectionPeriod);
  theParameters.theLog << "Processing direction #" << h << "(" << as_string(windDirectionPeriod)
                       << ", " << wind_direction_string(windDirectionInfo.id)
                       << ") during interval (" << as_string(intervalSentenceInfo.period) << ", "
                       << intervalSentenceInfo.intervalInfo << ")\n";

  bool same_part_of_day =
      (get_part_of_the_day_id_wind(windDirectionPeriod.localStartTime()) ==
       get_part_of_the_day_id_wind(intervalSentenceInfo.period.localStartTime()));
  unsigned int differenceInHours = abs(intervalSentenceInfo.period.localStartTime()
                                           .DifferenceInHours(windDirectionPeriod.localStartTime()));

  bool isBeforeInterval = windDirectionPeriod.localStartTime() <
                          intervalSentenceInfo.period.localStartTime();
  bool separateBefore = isBeforeInterval &&
                        ((differenceInHours > 3 && !same_part_of_day) ||
                         (differenceInHours > 3 &&
                          windDirectionPeriod.localStartTime().GetDay() !=
                              intervalSentenceInfo.period.localStartTime().GetDay()));

  bool sameTime = (differenceInHours <= 3 ||
                   (differenceInHours < 8 && same_part_of_day) ||
                   (differenceInHours < 6 && lastInterval && lastSentence)) &&
                  !(firstInterval && firstSentence);

  if (separateBefore)
  {
    theParameters.theLog << "Direction changes more than 3 hours before speed reporting point -> "
                            "separate sentence: "
                         << windDirectionPeriod.localStartTime() << " < "
                         << intervalSentenceInfo.period.localStartTime() << "("
                         << intervalSentenceInfo.intervalInfo << ") "
                         << wind_direction_string(windDirectionInfo.id) << "\n";
    intervalSentenceInfo.directionChangesBefore.push_back(windDirectionInfo);
    directionChangeStartIndex = h + 1;
  }
  else if (sameTime)
  {
    theParameters.theLog << "Direction changes about same time as speed reporting point -> "
                            "report along with interval: "
                         << windDirectionPeriod.localStartTime() << " ~ "
                         << intervalSentenceInfo.period.localStartTime() << "("
                         << intervalSentenceInfo.intervalInfo << ") "
                         << wind_direction_string(windDirectionInfo.id) << "\n";
    intervalSentenceInfo.directionChange = windDirectionInfo;
    directionChangeStartIndex = h + 1;
  }
  else if (lastInterval && lastSentence)
  {
    theParameters.theLog << "Direction changes after last interval (report in separate sentence): "
                         << windDirectionPeriod.localStartTime() << " > "
                         << intervalSentenceInfo.period.localStartTime() << "("
                         << intervalSentenceInfo.intervalInfo << ") "
                         << wind_direction_string(windDirectionInfo.id) << "\n";
    intervalSentenceInfo.directionChangesAfter.push_back(windDirectionInfo);
    directionChangeStartIndex = h + 1;
  }
  else
  {
    theParameters.theLog << "Direction period " << as_string(windDirectionPeriod)
                         << " could not be attached to interval "
                         << as_string(intervalSentenceInfo.period) << '\n';
  }
}

// Inject direction changes into interval sentences of a single sentence_info
void WindForecast::injectDirectionsIntoIntervals(sentence_info& sentenceInfo,
                                                 unsigned int& directionChangeStartIndex,
                                                 unsigned int directionChanges,
                                                 bool firstSentence,
                                                 bool lastSentence,
                                                 WindSpeedSentenceInfo& separateDirectionSentences,
                                                 unsigned int j,
                                                 const WeatherPeriod& windDirectionPeriod,
                                                 const WindDirectionInfo& windDirectionInfo) const
{
  if (sentenceInfo.intervalSentences.empty())
  {
    if (lastSentence)
    {
      sentenceInfo.directionChange = windDirectionInfo;
      theParameters.theLog << "No interval sentences for direction change #" << j
                           << " -> set direction change time to "
                           << sentenceInfo.directionChange->startTime() << '\n';
      sentence_info separateSentenceInfo;
      separateSentenceInfo.period = windDirectionPeriod;
      separateSentenceInfo.directionChange = windDirectionInfo;
      separateSentenceInfo.sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE;
      separateSentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::TIME_PERIOD);
      separateSentenceInfo.sentenceParameterTypes.push_back(SentenceParameterType::WIND_DIRECTION);
      separateDirectionSentences.push_back(separateSentenceInfo);
      directionChangeStartIndex = j + 1;
    }
    return;
  }

  theParameters.theLog << "Wind direction changes during strenghtening/weakening period:\n";
  for (unsigned int k = 0; k < sentenceInfo.intervalSentences.size(); k++)
  {
    interval_sentence_info& intervalSentenceInfo = sentenceInfo.intervalSentences[k];
    bool firstInterval = (k == 0);
    bool lastInterval = (k == sentenceInfo.intervalSentences.size() - 1);

    if (intervalSentenceInfo.skip)
      theParameters.theLog << "Interval sentence #" << k
                           << " does not differ enough -> dont report interval, but report "
                              "direction change "
                           << intervalSentenceInfo.intervalInfo << '\n';

    theParameters.theLog << "Interval sentence #" << k << " -> "
                         << as_string(intervalSentenceInfo.period) << ", "
                         << directionChangeStartIndex << '\n';

    for (unsigned int h = directionChangeStartIndex; h < directionChanges; h++)
      assignDirectionToInterval(intervalSentenceInfo, h, directionChangeStartIndex, firstInterval,
                                firstSentence, lastInterval, lastSentence);
  }
}

// Process a single direction change period (j) for one speed-change sentence
// Returns false if the outer loop should break, true to continue.
bool WindForecast::processOneDirectionChange(unsigned int j,
                                             sentence_info& sentenceInfo,
                                             const WeatherPeriod& windSpeedChangePeriod,
                                             WindDirectionInfo& previousWindDirectionInfo,
                                             unsigned int& directionChangeStartIndex,
                                             unsigned int directionChanges,
                                             bool firstSentence,
                                             bool lastSentence,
                                             WindSpeedSentenceInfo& sentenceInfoBefore,
                                             WindSpeedSentenceInfo& separateDirectionSentences) const
{
  WeatherPeriod windDirectionPeriod = theParameters.theWindDirectionPeriods[j];
  int directionPeriodLen = get_period_length(windDirectionPeriod);
  if (directionPeriodLen < 3)
  {
    theParameters.theLog << "Direction change #" << j
                         << " period is short: " << as_string(windDirectionPeriod)
                         << ", skipping!\n";
    return true;
  }

  WindDirectionInfo windDirectionInfo = get_wind_direction(theParameters, windDirectionPeriod);
  theParameters.theLog << "Direction change #" << j << " -> "
                       << as_string(windDirectionInfo.period) << " ("
                       << wind_direction_string(windDirectionInfo.id) << ")\n";

  if ((windDirectionInfo.id == VAIHTELEVA && directionPeriodLen < 2) ||
      (windDirectionInfo.id != VAIHTELEVA && directionPeriodLen < 3))
  {
    theParameters.theLog << "Period " << as_string(windDirectionPeriod) << "("
                         << wind_direction_string(windDirectionInfo.id) << ") shorter than "
                         << directionPeriodLen << " hours -> skip it.\n";
    return true;
  }

  if (windDirectionPeriod.localStartTime() > windSpeedChangePeriod.localEndTime())
  {
    theParameters.theLog << "Direction changes after wind speed change ends "
                         << as_string(windDirectionPeriod) << " < "
                         << as_string(windSpeedChangePeriod) << '\n';
    return false;  // break outer loop
  }

  bool directionIsBefore =
      windDirectionPeriod.localStartTime() < windSpeedChangePeriod.localStartTime() &&
      (abs(windSpeedChangePeriod.localStartTime().DifferenceInHours(
           windDirectionPeriod.localStartTime())) > 2 ||
       (windDirectionInfo.id == VAIHTELEVA && directionPeriodLen > 2));

  if (directionIsBefore)
  {
    sentence_info si;
    negotiateWindDirection(windDirectionInfo, previousWindDirectionInfo);
    if (windDirectionInfo.id == VAIHTELEVA)
    {
      si.sentence << (directionPeriodLen <= 5
                          ? ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE
                          : ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE);
      si.sentenceParameterTypes.push_back(SentenceParameterType::TIME_PERIOD);
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
                         << windSpeedChangePeriod.localStartTime() << '\n';
    return true;
  }

  if (abs(windSpeedChangePeriod.localStartTime().DifferenceInHours(
          windDirectionPeriod.localStartTime())) < 4)
  {
    if (windDirectionInfo.id != VAIHTELEVA)
      theParameters.theLog
          << "Direction changes about same time as wind speed starts to change: "
          << windDirectionPeriod.localStartTime() << " ~ "
          << windSpeedChangePeriod.localStartTime() << ", "
          << wind_direction_string(windDirectionInfo.id) << '\n';

    if (!sentenceInfo.intervalSentences.empty())
    {
      if (windDirectionInfo.id != VAIHTELEVA)
        sentenceInfo.directionChange = windDirectionInfo;
      directionChangeStartIndex = j + 1;
    }
  }
  else if (is_inside(windDirectionPeriod.localStartTime(), windSpeedChangePeriod))
  {
    injectDirectionsIntoIntervals(sentenceInfo, directionChangeStartIndex, directionChanges,
                                  firstSentence, lastSentence, separateDirectionSentences, j,
                                  windDirectionPeriod, windDirectionInfo);
  }
  return true;
}

void WindForecast::injectWindDirections(WindSpeedSentenceInfo& sentenceInfoVector) const
{
  WindSpeedSentenceInfo resultVector;
  WindDirectionInfo previousWindDirectionInfo;
  unsigned int directionChanges = theParameters.theWindDirectionPeriods.size();
  unsigned int directionChangeStartIndex = 1;  // direction in beginning already reported

  theParameters.theLog << "Injecting wind directions into story\n";

  WindSpeedSentenceInfo separateDirectionSentences;
  for (unsigned int i = 0; i < sentenceInfoVector.size(); i++)
  {
    WindSpeedSentenceInfo sentenceInfoBefore;
    WindSpeedSentenceInfo sentenceInfoAfter;

    sentence_info& sentenceInfo = sentenceInfoVector[i];
    WeatherPeriod windSpeedChangePeriod = sentenceInfo.period;
    bool firstSentence = (i == 0);
    bool lastSentence = (i == sentenceInfoVector.size() - 1);

    if (!sentenceInfo.intervalSentences.empty() &&
        sentenceInfo.intervalSentences.back().period.localEndTime() >
            windSpeedChangePeriod.localEndTime())
    {
      windSpeedChangePeriod =
          WeatherPeriod(windSpeedChangePeriod.localStartTime(),
                        sentenceInfo.intervalSentences.back().period.localEndTime());
    }

    theParameters.theLog << "Sentence #" << i << " -> " << as_string(sentenceInfo.period)
                         << " extended to " << as_string(windSpeedChangePeriod) << '\n';

    for (unsigned int j = directionChangeStartIndex; j < directionChanges; j++)
    {
      if (!processOneDirectionChange(j, sentenceInfo, windSpeedChangePeriod,
                                     previousWindDirectionInfo, directionChangeStartIndex,
                                     directionChanges, firstSentence, lastSentence,
                                     sentenceInfoBefore, separateDirectionSentences))
        break;
    }

    if (lastSentence && directionChanges - directionChangeStartIndex > 1 &&
        directionChangeStartIndex > 0)
    {
      WeatherPeriod prevReported =
          theParameters.theWindDirectionPeriods[directionChangeStartIndex - 1];
      WeatherPeriod lastDirPeriod = theParameters.theWindDirectionPeriods[directionChanges - 1];
      if (get_period_length(lastDirPeriod) > 2 &&
          get_part_of_the_day_id_wind(prevReported) !=
              get_part_of_the_day_id_wind(lastDirPeriod))
      {
        WindDirectionInfo lastDirInfo = get_wind_direction(theParameters, lastDirPeriod);
        sentence_info si;
        si.sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE;
        si.sentenceParameterTypes.push_back(SentenceParameterType::TIME_PERIOD);
        si.sentenceParameterTypes.push_back(SentenceParameterType::WIND_DIRECTION);
        si.directionChange = lastDirInfo;
        si.period = lastDirPeriod;
        sentenceInfoAfter.push_back(si);
        theParameters.theLog << "Reporting last short direction period: "
                             << as_string(lastDirPeriod) << '\n';
      }
    }

    if (sentenceInfo.directionChange)
      theParameters.theLog << "No interval sentences for direction change2 "
                           << as_string(sentenceInfo.directionChange->period) << '\n';
    else
      theParameters.theLog << "No sentenceInfo.directionChange #\n";

    resultVector.insert(resultVector.end(), sentenceInfoBefore.begin(), sentenceInfoBefore.end());
    resultVector.insert(resultVector.end(), sentenceInfo);
    resultVector.insert(resultVector.end(), sentenceInfoAfter.begin(), sentenceInfoAfter.end());
  }

  sentenceInfoVector = resultVector;
  if (!separateDirectionSentences.empty())
  {
    sentenceInfoVector.insert(sentenceInfoVector.end(),
                              separateDirectionSentences.begin(),
                              separateDirectionSentences.end());
  }
}

// merge first two sentences if wind speed intervals don't differ enough
void WindForecast::mergeRedundantFirstSentences(WindSpeedSentenceInfo& sentenceInfoVector) const
{
  if (sentenceInfoVector.size() <= 1)
    return;
  sentence_info& sentenceInfoFirst = sentenceInfoVector[0];
  sentence_info& sentenceInfoSecond = sentenceInfoVector[1];
  if (!sentenceInfoFirst.intervalSentences.empty() ||
      sentenceInfoSecond.intervalSentences.size() != 1)
    return;

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

// mark intervals to skip when they don't differ enough from each other
void WindForecast::markSkippedIntervals(sentence_info& sentenceInfo,
                                        unsigned int sentenceIndex,
                                        interval_info& intervalInfo,
                                        interval_info& intervalInfoPrevious) const
{
  for (unsigned int k = 0; k < sentenceInfo.intervalSentences.size(); k++)
  {
    interval_sentence_info& isi = sentenceInfo.intervalSentences[k];

    if (sentenceIndex == 0 || get_period_length(sentenceInfo.period) <= 6)
      isi.useAlkaenPhrase = (get_period_length(isi.period) > 6);

    interval_info intervalInfoIsi = windSpeedIntervalInfo(isi.period);

    if (k == 0 && sentenceIndex > 0 && intervalInfo.lowerLimit != kFloatMissing)
    {
      intervalInfoPrevious.peakWind = intervalInfo.upperLimit;
      interval_info intervalInfoIsi2 = intervalInfoIsi;
      intervalInfoIsi2.peakWind = intervalInfoIsi.upperLimit;
      if (!wind_speed_differ_enough(intervalInfoIsi2, intervalInfoPrevious))
      {
        if (k == sentenceInfo.intervalSentences.size() - 1)
        {
          sentenceInfo.sentence.clear();
          sentenceInfo.sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE;
          sentenceInfo.changeType = "";
        }
        theParameters.theLog << "Interval1 " << intervalInfoIsi
                             << " does not differ enough from " << intervalInfoPrevious
                             << " -> not reported\n";
        isi.skip = true;
        continue;
      }
    }

    if (k < sentenceInfo.intervalSentences.size() - 1)
    {
      interval_sentence_info& isiNext = sentenceInfo.intervalSentences[k + 1];
      interval_info intervalInfoIsiNext = windSpeedIntervalInfo(isiNext.period);
      bool differEnough =
          wind_speed_differ_enough(intervalInfoIsi, intervalInfoIsiNext,
                                   theParameters.theContextualMaxIntervalSize,
                                   sentenceInfo.changeType == VOIMISTUVAA_WORD);
      if (!differEnough)
      {
        isi.skip = true;
        theParameters.theLog << "Interval2 " << intervalInfoIsiNext
                             << " does not differ enough from " << intervalInfoIsi
                             << " -> not reported\n";
        continue;
      }
    }

    intervalInfoPrevious = intervalInfoIsi;
  }
}

// check the consecutive intervals and check that same or almost same interval is not reported twice
void WindForecast::checkWindSpeedIntervals(WindSpeedSentenceInfo& sentenceInfoVector) const
{
  mergeRedundantFirstSentences(sentenceInfoVector);

  interval_info intervalInfo;
  interval_info intervalInfoPrevious;
  intervalInfo.lowerLimit = kFloatMissing;

  for (unsigned int i = 0; i < sentenceInfoVector.size(); i++)
  {
    sentence_info& sentenceInfo = sentenceInfoVector[i];

    if (i == 0 && sentenceInfo.intervalSentences.empty())
    {
      intervalInfo = windSpeedIntervalInfo(sentenceInfo.period);
      intervalInfoPrevious = intervalInfo;
    }

    markSkippedIntervals(sentenceInfo, i, intervalInfo, intervalInfoPrevious);

    if (!sentenceInfo.intervalSentences.empty())
    {
      bool anyNotSkipped = false;
      for (const auto& isi : sentenceInfo.intervalSentences)
      {
        if (!isi.skip)
        {
          anyNotSkipped = true;
          break;
        }
      }
      sentenceInfo.skip = !anyNotSkipped;
      if (sentenceInfo.skip)
        theParameters.theLog << "Skipping sentence at period " << as_string(sentenceInfo.period)
                             << " since no interval is present\n";
    }
  }
}

// Search for a moved period by sliding the whole period backward or forward
// until the part-of-day changes. Returns hours moved (0 if unchanged).
WeatherPeriod WindForecast::findMovedPeriod(part_of_the_day_id partOfTheDay,
                                            const WeatherPeriod& period,
                                            const WeatherPeriod& guardPeriod,
                                            int& hours_moved,
                                            bool backwards) const
{
  WeatherPeriod result = period;
  hours_moved = 0;
  TextGenPosixTime startTime = period.localStartTime();
  TextGenPosixTime endTime = period.localEndTime();
  while (true)
  {
    startTime.ChangeByHours(backwards ? -1 : 1);
    endTime.ChangeByHours(backwards ? -1 : 1);
    if (backwards && startTime <= guardPeriod.localEndTime())
      break;
    if (!backwards && endTime >= guardPeriod.localStartTime())
      break;
    WeatherPeriod phrasePeriod(startTime, endTime);
    TimePhraseInfo tpi;
    getTimePhrase(phrasePeriod, tpi, get_period_length(phrasePeriod) > 6);
    if (tpi.part_of_the_day != partOfTheDay)
    {
      result = WeatherPeriod(startTime, endTime);
      hours_moved = abs(startTime.DifferenceInHours(period.localStartTime()));
      break;
    }
  }
  return result;
}

// Search for a shrunk period by trimming one end of the period until the part-of-day
// changes. Returns hours shrunk (0 if unchanged).
WeatherPeriod WindForecast::findShrunkPeriod(part_of_the_day_id partOfTheDay,
                                             const WeatherPeriod& period,
                                             const WeatherPeriod& guardPeriod,
                                             int& hours_shrunk,
                                             bool backwards) const
{
  WeatherPeriod result = period;
  hours_shrunk = 0;
  unsigned int period_len = get_period_length(period);
  unsigned int max_shrink = std::max(period_len * 0.9, 1.0);
  TextGenPosixTime startTime = period.localStartTime();
  TextGenPosixTime endTime = period.localEndTime();
  for (unsigned int i = 0; i < max_shrink; i++)
  {
    if (backwards)
    {
      endTime.ChangeByHours(-1);
      if (endTime <= guardPeriod.localEndTime())
        break;
    }
    else
    {
      startTime.ChangeByHours(1);
      if (startTime >= guardPeriod.localStartTime())
        break;
    }
    WeatherPeriod phrasePeriod(startTime, endTime);
    TimePhraseInfo tpi;
    getTimePhrase(phrasePeriod, tpi, get_period_length(phrasePeriod) > 6);
    if (tpi.part_of_the_day != partOfTheDay)
    {
      result = WeatherPeriod(startTime, endTime);
      hours_shrunk = backwards ? abs(endTime.DifferenceInHours(period.localEndTime()))
                               : abs(startTime.DifferenceInHours(period.localStartTime()));
      break;
    }
  }
  return result;
}

// move or shrink the period till part_of_the_day is different from original
WeatherPeriod WindForecast::getNewPeriod(part_of_the_day_id& partOfTheDay,
                                         const WeatherPeriod& period,
                                         const WeatherPeriod& guardPeriod,
                                         unsigned int& hours,
                                         bool backwards) const
{
  hours = 0;
  int hours_moved = 0;
  WeatherPeriod movedPeriod = findMovedPeriod(partOfTheDay, period, guardPeriod, hours_moved,
                                              backwards);

  if (get_period_length(period) < 3)
  {
    hours = static_cast<unsigned int>(hours_moved);
    return movedPeriod;
  }

  int hours_shrunk = 0;
  WeatherPeriod shrunkPeriod = findShrunkPeriod(partOfTheDay, period, guardPeriod, hours_shrunk,
                                                backwards);

  if (hours_moved == 0 && hours_shrunk == 0)
    return period;
  if (hours_moved == hours_shrunk || hours_moved == 0)
  {
    hours = static_cast<unsigned int>(hours_shrunk);
    return shrunkPeriod;
  }
  if (hours_shrunk == 0)
  {
    hours = static_cast<unsigned int>(hours_moved);
    return movedPeriod;
  }
  hours = static_cast<unsigned int>(std::min(hours_moved, hours_shrunk));
  return (hours_moved < hours_shrunk ? movedPeriod : shrunkPeriod);
}

// update either period1 (change it to earlier) or period2 (change it to later)
//
// TODO: when moving periods check borders
bool WindForecast::negotiateNewPeriod(part_of_the_day_id& partOfTheDay,
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

  if (hoursChanged1 == 0 && hoursChanged2 == 0)
    return false;

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

// collect pointers to all periods that need a time phrase, in sentence order
std::vector<WeatherPeriod*> collectTimePhrasePeriods(WindSpeedSentenceInfo& sentenceInfoVector)
{
  std::vector<WeatherPeriod*> result;
  for (unsigned int i = 0; i < sentenceInfoVector.size(); i++)
  {
    sentence_info& si = sentenceInfoVector[i];
    if (i > 0)
      result.push_back(&si.period);

    for (unsigned int j = 0; j < si.intervalSentences.size(); j++)
    {
      interval_sentence_info& isi = si.intervalSentences[j];
      for (auto& isi_dcb : isi.directionChangesBefore)
        result.push_back(&isi_dcb.period);
      if (i == 0 && j == 0)
        continue;
      if (j > 0)
        result.push_back(&isi.period);
    }
  }
  return result;
}

void WindForecast::logTautologyResolution(const WeatherPeriod& original1,
                                          const WeatherPeriod& original2,
                                          const TimePhraseInfo& currentTPI,
                                          const TimePhraseInfo& previousTPI,
                                          WeatherPeriod* previousPeriod,
                                          WeatherPeriod* currentPeriod) const
{
  if (original1.localStartTime() == previousPeriod->localStartTime() &&
      original1.localEndTime() == previousPeriod->localEndTime())
  {
    theParameters.theLog << "Tautology resolved, period " << as_string(original2) << " ("
                         << part_of_the_day_string(currentTPI.part_of_the_day) << ")";
    TimePhraseInfo tpi;
    getTimePhrase(*currentPeriod, tpi, get_period_length(*currentPeriod) > 6);
    theParameters.theLog << " changed to " << as_string(*currentPeriod) << " ("
                         << part_of_the_day_string(tpi.part_of_the_day) << ")\n";
  }
  else
  {
    theParameters.theLog << "Tautology resolved, period " << as_string(original1) << " ("
                         << part_of_the_day_string(previousTPI.part_of_the_day) << ")";
    TimePhraseInfo tpi;
    getTimePhrase(*previousPeriod, tpi, get_period_length(*previousPeriod) > 6);
    theParameters.theLog << " changed to " << as_string(*previousPeriod) << " ("
                         << part_of_the_day_string(tpi.part_of_the_day) << ")\n";
  }
}

// check time phrases, so that same phrase is not used in successive sentences
// if same phrase is in successive sentences -> change either of the phrase to the nearest fitting
// phrase
void WindForecast::checkTimePhrases(WindSpeedSentenceInfo& sentenceInfoVector) const
{
  theParameters.theLog << "Checking for time phrase tautology\n";

  std::vector<WeatherPeriod*> timePhrasePeriods = collectTimePhrasePeriods(sentenceInfoVector);

  int n = static_cast<int>(timePhrasePeriods.size());
  for (int i = 1; i < n; i++)
  {
    WeatherPeriod* currentPeriod = timePhrasePeriods[i];
    WeatherPeriod* previousPeriod = timePhrasePeriods[i - 1];
    TimePhraseInfo previousTPI;
    TimePhraseInfo currentTPI;
    getTimePhrase(*previousPeriod, previousTPI, get_period_length(*previousPeriod) > 6);
    getTimePhrase(*currentPeriod, currentTPI, get_period_length(*currentPeriod) > 6);

    if (previousTPI.part_of_the_day == MISSING_PART_OF_THE_DAY_ID || previousTPI != currentTPI)
      continue;

    WeatherPeriod period1 = *previousPeriod;
    WeatherPeriod period2 = *currentPeriod;
    theParameters.theLog << "Tautology found between " << as_string(*previousPeriod) << " and "
                         << as_string(*currentPeriod) << " ("
                         << part_of_the_day_string(currentTPI.part_of_the_day) << ")\n";

    TextGenPosixTime beforeStart = theParameters.theForecastPeriod.localStartTime();
    beforeStart.ChangeByHours(-1);
    TextGenPosixTime afterEnd = theParameters.theForecastPeriod.localEndTime();
    afterEnd.ChangeByHours(1);
    WeatherPeriod beforePreviousPeriod(beforeStart, beforeStart);
    WeatherPeriod afterCurrentPeriod(afterEnd, afterEnd);

    if (i > 1)
      beforePreviousPeriod = *(timePhrasePeriods[i - 2]);
    if (i < n - 1)
      afterCurrentPeriod = *(timePhrasePeriods[i + 1]);

    if (negotiateNewPeriod(
            currentTPI.part_of_the_day, *previousPeriod, *currentPeriod,
            beforePreviousPeriod, afterCurrentPeriod))
    {
      logTautologyResolution(period1, period2, currentTPI, previousTPI, previousPeriod,
                             currentPeriod);
    }
    else
    {
      theParameters.theLog << "Tautology COULD NOT BE resolved between periods "
                           << as_string(*previousPeriod) << " and " << as_string(*currentPeriod)
                           << "\n";
    }
  }
}

// collect all direction change pointers from a sentence_info into a flat list
std::vector<WindDirectionInfo*> collectDirectionChanges(sentence_info& si)
{
  std::vector<WindDirectionInfo*> result;
  if (si.directionChange)
    result.push_back(&(*(si.directionChange)));
  for (auto& isi : si.intervalSentences)
  {
    for (auto& dc : isi.directionChangesBefore)
      result.push_back(&dc);
    if (isi.directionChange)
      result.push_back(&(*(isi.directionChange)));
    for (auto& dc : isi.directionChangesAfter)
      result.push_back(&dc);
  }
  return result;
}

// suppress redundant middle direction when three consecutive directions form a monotone arc
// of 90..180 degrees
void suppressRedundantDirections(std::vector<WindDirectionInfo*>& directionChanges)
{
  unsigned int step = 3;
  for (unsigned int i = 0; i < directionChanges.size(); i += step)
  {
    if (directionChanges.size() - i <= 3)
      break;

    // ignore varying wind
    if (directionChanges[i]->id == VAIHTELEVA || directionChanges[i + 1]->id == VAIHTELEVA ||
        directionChanges[i + 2]->id == VAIHTELEVA)
    {
      step = 1;
      continue;
    }

    step = 3;

    double d1 = directionChanges[i]->direction.value();
    double d2 = directionChanges[i + 1]->direction.value();
    double d3 = directionChanges[i + 2]->direction.value();
    double totalChange = std::abs(d3 - d1);

    bool monotone = (d1 > d2 && d2 > d3) || (d1 < d2 && d2 < d3);
    if (monotone && totalChange >= 90.0 && totalChange < 180.0)
      *(directionChanges[i + 1]) = WindDirectionInfo();  // reset
  }
}

// check that wind direction changes  are not reported too often if direction changes
// a lot at once
void WindForecast::checkWindDirections(WindSpeedSentenceInfo& sentenceInfoVector)
{
  for (auto& si : sentenceInfoVector)
  {
    std::vector<WindDirectionInfo*> directionChanges = collectDirectionChanges(si);
    if (directionChanges.size() >= 3)
      suppressRedundantDirections(directionChanges);
  }
}

// get paragraph info
// Add time-phrase sentence parameter for a sentence (first sentence gets empty delimiter)
void WindForecast::addTimePeriodParam(paragraph_info& pi,
                                      const sentence_info& sentenceInfo,
                                      TimePhraseInfo& timePhraseInfo,
                                      bool isFirstSentence) const
{
  sentence_parameter sp(TIME_PERIOD);
  if (isFirstSentence)
  {
    sp.sentence << Delimiter("");  // dummy empty string for [1] [2] pattern substitutions
  }
  else
  {
    WeatherPeriod period = sentenceInfo.period;
    sp.sentence << getTimePhrase(period, timePhraseInfo, get_period_length(period) > 6);
  }
  sp.tpi = timePhraseInfo;
  pi.sentenceParameters.push_back(sp);
}

// Add wind-direction sentence parameter
void WindForecast::addWindDirectionParam(paragraph_info& pi,
                                         const sentence_info& sentenceInfo) const
{
  sentence_parameter sp(WIND_DIRECTION);
  if (sentenceInfo.directionChange)
    sp.sentence << windDirectionSentence(sentenceInfo.directionChange->id,
                                         sentenceInfo.useWindBasicForm);
  else
    sp.sentence << (sentenceInfo.useWindBasicForm ? TUULI_WORD : TUULTA_WORD);
  pi.sentenceParameters.push_back(sp);
}

// Add skipped-but-peaked interval parameters (kovimmillaan phrase)
void WindForecast::addPeakWindParam(paragraph_info& pi,
                                    const interval_sentence_info& isi,
                                    TimePhraseInfo& timePhraseInfo) const
{
  interval_info intervalInfoIsi = windSpeedIntervalInfo(isi.period);
  if (intervalInfoIsi.upperLimit >= intervalInfoIsi.peakWind)
    return;
  sentence_parameter sp1;
  sp1.sentence << Delimiter(COMMA_PUNCTUATION_MARK);
  sentence_parameter sp2(TIME_PERIOD);
  sp2.sentence << getTimePhrase(isi.period, timePhraseInfo, isi.useAlkaenPhrase);
  sp2.tpi = timePhraseInfo;
  sentence_parameter sp3(WIND_SPEED_INTERVAL);
  sp3.sentence << KOVIMMILLAAN_PHRASE << intervalInfoIsi.peakWind << "m/s";
  pi.sentenceParameters.push_back(sp1);
  pi.sentenceParameters.push_back(sp2);
  pi.sentenceParameters.push_back(sp3);
}

// Process one non-skipped interval sentence and append parameters to pi
void WindForecast::processNonSkippedInterval(paragraph_info& pi,
                                              const interval_sentence_info& isi,
                                              const sentence_info& sentenceInfo,
                                              TimePhraseInfo& timePhraseInfo,
                                              unsigned int k,
                                              bool isFirstOfAll) const
{
  WeatherPeriod period = isi.period;
  bool samePeriod = false;

  if (isFirstOfAll)
  {
    // First interval of first sentence: add "aluksi" unless special phrase is used
    if (as_string(sentenceInfo.sentence)
            .substr(0, strlen(POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_COMPOSITE_PHRASE)) !=
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
      sentence_parameter sp(TIME_PERIOD);
      sp.sentence << Delimiter(COMMA_PUNCTUATION_MARK) << timePhrase;
      timePhraseInfo = tpi;
      sp.tpi = tpi;
      pi.sentenceParameters.push_back(sp);
    }
    else
    {
      samePeriod = true;
    }
  }

  // direction change may happen same time as interval
  if (isi.directionChange)
  {
    sentence_parameter sp(WIND_DIRECTION);
    sp.sentence << windDirectionSentence(*(isi.directionChange));
    if (!pi.sentenceParameters.empty() && pi.sentenceParameters.back().type == WIND_DIRECTION)
      pi.sentenceParameters.back() = sp;
    else
      pi.sentenceParameters.push_back(sp);
  }

  sentence_parameter sp1(WIND_SPEED_INTERVAL);
  sp1.sentence << windSpeedIntervalSentence(period, isi.intervalInfo, timePhraseInfo,
                                             sentenceInfo.changeType == VOIMISTUVAA_WORD);
  if (!sp1.sentence.empty())
  {
    sp1.tpi = timePhraseInfo;
    if (samePeriod && k > 0)
      pi.sentenceParameters.back().sentence = sp1.sentence;
    else
      pi.sentenceParameters.push_back(sp1);
  }
}

// Process one skipped interval that may still have a direction change to report
void WindForecast::processSkippedInterval(paragraph_info& pi,
                                           const interval_sentence_info& isi,
                                           TimePhraseInfo& timePhraseInfo) const
{
  if (!isi.directionChange)
    return;
  Sentence timePhrase;
  TimePhraseInfo tpi = timePhraseInfo;
  timePhrase << getTimePhrase(isi.period, tpi, isi.useAlkaenPhrase);
  if (tpi != timePhraseInfo)
  {
    sentence_parameter sp(TIME_PERIOD);
    sp.sentence << Delimiter(COMMA_PUNCTUATION_MARK) << timePhrase;
    timePhraseInfo = tpi;
    sp.tpi = tpi;
    pi.sentenceParameters.push_back(sp);
  }
  sentence_parameter sp(WIND_DIRECTION);
  sp.sentence << windDirectionSentence(*(isi.directionChange));
  pi.sentenceParameters.push_back(sp);
}

// Process the WIND_SPEED_INTERVAL parameter for a sentence
void WindForecast::processWindSpeedIntervalParam(paragraph_info& pi,
                                                  paragraph_info& piAfterLastInterval,
                                                  const sentence_info& sentenceInfo,
                                                  TimePhraseInfo& timePhraseInfo,
                                                  bool isFirstOutputSentence) const
{
  // simple case: first output sentence with exactly one interval
  if (isFirstOutputSentence && sentenceInfo.intervalSentences.size() == 1)
  {
    const interval_sentence_info& isi = sentenceInfo.intervalSentences[0];
    sentence_parameter sp(WIND_SPEED_INTERVAL);
    sp.sentence << windSpeedIntervalSentence(isi.period, isi.intervalInfo, timePhraseInfo,
                                              USE_AT_ITS_STRONGEST_PHRASE);
    if (!sp.sentence.empty())
    {
      sp.tpi = timePhraseInfo;
      pi.sentenceParameters.push_back(sp);
    }
    auto sps2 = reportWindDirectionChanges(isi.directionChangesAfter, timePhraseInfo, false);
    piAfterLastInterval.sentenceParameters.insert(piAfterLastInterval.sentenceParameters.end(),
                                                   sps2.begin(), sps2.end());
    return;
  }

  // general case: multiple intervals
  for (unsigned int k = 0; k < sentenceInfo.intervalSentences.size(); k++)
  {
    const interval_sentence_info& isi = sentenceInfo.intervalSentences[k];
    // report direction changes before interval
    auto sps = reportWindDirectionChanges(isi.directionChangesBefore, timePhraseInfo);
    pi.sentenceParameters.insert(pi.sentenceParameters.end(), sps.begin(), sps.end());

    // kovimmillaan must be reported if this is the last skipped interval
    if (isi.skip && k == sentenceInfo.intervalSentences.size() - 1)
      addPeakWindParam(pi, isi, timePhraseInfo);

    bool isFirstOfAll = (isFirstOutputSentence && k == 0);
    if (!isi.skip)
      processNonSkippedInterval(pi, isi, sentenceInfo, timePhraseInfo, k, isFirstOfAll);
    else
      processSkippedInterval(pi, isi, timePhraseInfo);

    // report direction changes after interval
    auto sps2 = reportWindDirectionChanges(isi.directionChangesAfter, timePhraseInfo, false);
    piAfterLastInterval.sentenceParameters.insert(piAfterLastInterval.sentenceParameters.end(),
                                                   sps2.begin(), sps2.end());
  }
}

ParagraphInfoVector WindForecast::getParagraphInfo(
    const WeatherPeriod& /*thePeriod*/, const WindSpeedSentenceInfo& sentenceInfoVector) const
{
  ParagraphInfoVector ret;
  TimePhraseInfo timePhraseInfo;

  for (unsigned int i = 0; i < sentenceInfoVector.size(); i++)
  {
    theParameters.theLog << "** Processing sentence part " << i << "\n";
    bool firstOutputSentence = ret.empty();
    paragraph_info pi;
    paragraph_info piAfterLastInterval;
    const sentence_info& sentenceInfo = sentenceInfoVector[i];

    if (sentenceInfo.skip)
      continue;
    if (i == sentenceInfoVector.size() - 1 && get_period_length(sentenceInfo.period) < 3 &&
        !sentenceInfo.changeType.empty())
      continue;

    pi.sentence << sentenceInfo.sentence;
    for (SentenceParameterType parameterType : sentenceInfo.sentenceParameterTypes)
    {
      switch (parameterType)
      {
        case TIME_PERIOD:
          addTimePeriodParam(pi, sentenceInfo, timePhraseInfo, i == 0);
          break;
        case WIND_DIRECTION:
          addWindDirectionParam(pi, sentenceInfo);
          break;
        case CHANGE_TYPE:
        {
          sentence_parameter sp(CHANGE_TYPE);
          sp.sentence << sentenceInfo.changeType;
          pi.sentenceParameters.push_back(sp);
          break;
        }
        case CHANGE_SPEED:
        {
          sentence_parameter sp(CHANGE_SPEED);
          sp.sentence << sentenceInfo.changeSpeed;
          pi.sentenceParameters.push_back(sp);
          break;
        }
        case WIND_SPEED_INTERVAL:
          processWindSpeedIntervalParam(pi, piAfterLastInterval, sentenceInfo, timePhraseInfo,
                                        firstOutputSentence);
          break;
        case VOID_TYPE:
          std::cout << "VOID_TYPE\n";
          break;
      }
    }
    ret.push_back(pi);
    if (!piAfterLastInterval.sentenceParameters.empty())
      ret.push_back(piAfterLastInterval);
  }

  return ret;
}

Paragraph WindForecast::getWindStory(const WeatherPeriod& thePeriod) const
{
  Paragraph paragraph;
  std::vector<Sentence> sentences;

  WindDirectionInfo previouslyReportedWindDirection;

  theParameters.theLog << "*** WIND DIRECTION REPORTING PERIODS ***\n";

  for (const WeatherPeriod& period : theParameters.theWindDirectionPeriods)
    theParameters.theLog << as_string(period) << " - "
                         << wind_direction_string(get_wind_direction(theParameters, period).id)
                         << '\n';

  theParameters.theLog << "*** WIND SPEED REPORTING PERIODS ***\n";

  for (const WindEventPeriodDataItem* item : theParameters.theWindSpeedEventPeriodVector)
    theParameters.theLog << as_string(item->thePeriod) << " - "
                         << get_wind_event_string(item->theWindEvent) << '\n';

  std::vector<WindDirectionPeriodInfo> windDirectionPeriodInfo;
  windDirectionPeriodInfo.reserve(theParameters.theWindDirectionPeriods.size());

  for (const auto& p : theParameters.theWindDirectionPeriods)
    windDirectionPeriodInfo.emplace_back(p, get_wind_direction(theParameters, p));

  WindSpeedSentenceInfo sentenceInfoVector;
  WindDirectionInfo previousWindDirection;

  for (unsigned int i = 0; i < theParameters.theWindSpeedEventPeriodVector.size(); i++)
  {
    const WindEventPeriodDataItem* windSpeedEventPeriodDataItem =
        theParameters.theWindSpeedEventPeriodVector[i];

    theParameters.theLog << "** Processing event period " << i << " "
                         << windSpeedEventPeriodDataItem->thePeriod.localStartTime() << " - "
                         << windSpeedEventPeriodDataItem->thePeriod.localEndTime() << "\n";

    const WindEventPeriodDataItem* nextWindSpeedEventPeriodDataItem =
        (i < theParameters.theWindSpeedEventPeriodVector.size() - 1
             ? theParameters.theWindSpeedEventPeriodVector[i + 1]
             : nullptr);

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

  if (!sentenceInfoVector.empty())
    sentenceInfoVector[0].firstSentence = true;

  checkWindSpeedIntervals(sentenceInfoVector);

  log_sentence_info(
      theParameters.theLog, "SENTENCE INFO BEFORE INJECTING DIRECTIONS", sentenceInfoVector);

  injectWindDirections(sentenceInfoVector);

  log_sentence_info(
      theParameters.theLog, "SENTENCE INFO AFTER INJECTING DIRECTIONS", sentenceInfoVector);

  checkTimePhrases(sentenceInfoVector);

  checkWindDirections(sentenceInfoVector);

  Paragraph newParagraph;
  newParagraph << getParagraphInfo(thePeriod, sentenceInfoVector);

  /*
  for(const Sentence & s : sentences)
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

// Build time phrase for a zero-length period (point in time)
Sentence WindForecast::buildPointTimePhrase(const WeatherPeriod& thePeriod,
                                            TimePhraseInfo& timePhraseInfo,
                                            bool& specifyDay,
                                            int forecastPeriodLength,
                                            const WeatherPeriod& actualPeriod) const
{
  Sentence timePhrase;
  timePhraseInfo.part_of_the_day = get_part_of_the_day_id_wind(thePeriod);
  if (timePhraseInfo.part_of_the_day == KESKIYO)
    specifyDay = false;
  else
    specifyDay =
        ((forecastPeriodLength > 6 &&
          theParameters.theForecastPeriod.localStartTime().GetWeekday() !=
              actualPeriod.localStartTime().GetWeekday()) &&
         (timePhraseInfo.day_number != actualPeriod.localStartTime().GetWeekday()));
  specifyDay &= theParameters.theWeekdaysUsed;
  std::string plainTimePhrase =
      get_time_phrase_from_id(timePhraseInfo.part_of_the_day, theParameters.theVar, false);
  timePhrase << parse_time_phrase(thePeriod.localStartTime().GetWeekday(), specifyDay,
                                  plainTimePhrase);
  theParameters.theAlkaenPhraseUsed = false;
  return timePhrase;
}

// Build time phrase for a non-zero-length period
Sentence WindForecast::buildRangeTimePhrase(const WeatherPeriod& thePeriod,
                                            TimePhraseInfo& timePhraseInfo,
                                            bool useAlkaenPhrase,
                                            bool& specifyDay,
                                            int forecastPeriodLength,
                                            WeatherPeriod& actualPeriod) const
{
  Sentence timePhrase;
  std::string dayPhasePhrase;
  theParameters.theAlkaenPhraseUsed =
      (get_period_length(thePeriod) >= 6 && useAlkaenPhrase && !fit_into_narrow_day_part(thePeriod));

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
    specifyDay =
        ((forecastPeriodLength > 6 &&
          theParameters.theForecastPeriod.localStartTime().GetWeekday() !=
              actualPeriod.localStartTime().GetWeekday()) &&
         (timePhraseInfo.day_number != actualPeriod.localStartTime().GetWeekday()));
  specifyDay &= theParameters.theWeekdaysUsed;

  timePhrase << get_time_phrase_large(actualPeriod, specifyDay, theParameters.theVar,
                                      dayPhasePhrase, useAlkaenPhrase,
                                      timePhraseInfo.part_of_the_day);
  return timePhrase;
}

// adjust "illalla" -> "iltapäivällä" or "aamulla" -> "aamuyolla" when forecast boundary forces it
void WindForecast::adjustEndOfForecastPhrase(TimePhraseInfo& timePhraseInfo,
                                             Sentence& timePhrase,
                                             const WeatherPeriod& actualPeriod,
                                             bool useAlkaenPhrase) const
{
  auto endHour = theParameters.theForecastPeriod.localEndTime().GetHour();
  auto diffHours =
      abs(actualPeriod.localStartTime().DifferenceInHours(
          theParameters.theForecastPeriod.localEndTime()));
  if (timePhraseInfo.part_of_the_day == ILTA && endHour == 18 && diffHours < 6)
  {
    timePhraseInfo.part_of_the_day = ILTAPAIVA;
    timePhrase.clear();
    timePhrase << get_time_phrase_from_id(
        timePhraseInfo.part_of_the_day, theParameters.theVar, useAlkaenPhrase);
  }
  else if (timePhraseInfo.part_of_the_day == AAMU && endHour == 6 && diffHours < 6)
  {
    timePhraseInfo.part_of_the_day = AAMUYO;
    timePhrase.clear();
    timePhrase << get_time_phrase_from_id(
        timePhraseInfo.part_of_the_day, theParameters.theVar, useAlkaenPhrase);
  }
}

// normalise "aamuyo", "aamuyolla ja aamulla", and "keskiyo" in the time phrase string
void WindForecast::normalizeTimePhraseString(std::string& tps,
                                             TimePhraseInfo& timePhraseInfo,
                                             const WeatherPeriod& thePeriod,
                                             part_of_the_day_id previousPartOfTheDay,
                                             bool specifyDay,
                                             int forecastPeriodLength) const
{
  if (tps.find("aamuyo") != std::string::npos && timePhraseInfo.part_of_the_day != AAMUYO)
  {
    if (previousPartOfTheDay == ILTAYO)
    {
      short lastDay = thePeriod.localStartTime().GetWeekday();
      short newDay = (timePhraseInfo.day_number == 7 ? 1 : timePhraseInfo.day_number + 1);
      if (newDay <= lastDay && !(lastDay == 7 && newDay == 1))
        timePhraseInfo.day_number = newDay;
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
    tps.replace(tps.find("keskiyo"), 7, "aamuyo");
    if (specifyDay && forecastPeriodLength > 6 && !isdigit(tps[0]))
    {
      timePhraseInfo.day_number = thePeriod.localEndTime().GetWeekday();
      tps.insert(0, (Fmi::to_string(timePhraseInfo.day_number) + "-"));
    }
    timePhraseInfo.part_of_the_day = AAMUYO;
  }
}

Sentence WindForecast::getTimePhrase(const WeatherPeriod& thePeriod,
                                     TimePhraseInfo& timePhraseInfo,
                                     bool useAlkaenPhrase) const
{
  bool specifyDay = false;
  WeatherPeriod actualPeriod = thePeriod;
  int forecastPeriodLength = get_period_length(theParameters.theForecastPeriod);
  part_of_the_day_id previousPartOfTheDay = timePhraseInfo.part_of_the_day;

  if (get_period_length(thePeriod) == 6 && thePeriod.localStartTime().GetHour() == 12 &&
      thePeriod.localEndTime().GetHour() == 18)
    useAlkaenPhrase = false;

  Sentence timePhrase;
  if (get_period_length(thePeriod) == 0)
    timePhrase = buildPointTimePhrase(thePeriod, timePhraseInfo, specifyDay, forecastPeriodLength,
                                      actualPeriod);
  else
    timePhrase = buildRangeTimePhrase(thePeriod, timePhraseInfo, useAlkaenPhrase, specifyDay,
                                      forecastPeriodLength, actualPeriod);

  adjustEndOfForecastPhrase(timePhraseInfo, timePhrase, actualPeriod, useAlkaenPhrase);

  timePhraseInfo.starttime = actualPeriod.localStartTime();
  timePhraseInfo.endtime = actualPeriod.localEndTime();

  std::string tps = as_string(timePhrase);
  boost::algorithm::to_lower(tps);
  // remove dot and line feed
  tps.erase(tps.end() - 2, tps.end());

  if (tps.find("keskiyo") == std::string::npos)
    timePhraseInfo.day_number = actualPeriod.localStartTime().GetWeekday();

  normalizeTimePhraseString(tps, timePhraseInfo, thePeriod, previousPartOfTheDay, specifyDay,
                            forecastPeriodLength);

  Sentence sentence;
  sentence << tps;

  theParameters.theLog << "timePhrase: " << as_string(thePeriod) << " --> " << tps << " ("
                       << specifyDay << " -> " << timePhraseInfo.day_number << ")"
                       << " -> " << as_string(actualPeriod)
                       << (useAlkaenPhrase ? " use alkaen phrase" : " dont use alkaen phrase")
                       << " -> " << timePhraseInfo << "\n";

  return sentence;
}

// Adjust a weakening period to end at the weakest wind point when varying wind is present
WeatherPeriod WindForecast::findWeakeningLastPeriod(
    const WeatherPeriod& lastPeriod,
    const TextGenPosixTime& startTime,
    const WeatherPeriod& speedEventPeriod) const
{
  interval_info minIntervalInfo = windSpeedIntervalInfo(WeatherPeriod(startTime, startTime));
  interval_info maxIntervalInfo = windSpeedIntervalInfo(WeatherPeriod(startTime, startTime));
  unsigned int varyingHours = 0;
  TextGenPosixTime t = startTime;
  while (t < speedEventPeriod.localEndTime())
  {
    if (get_wind_direction(theParameters, t).id == VAIHTELEVA)
      varyingHours++;
    if (t > startTime)
    {
      interval_info iInfo = windSpeedIntervalInfo(WeatherPeriod(t, t));
      if (iInfo.lowerLimit <= minIntervalInfo.lowerLimit &&
          iInfo.upperLimit <= minIntervalInfo.upperLimit)
        minIntervalInfo = iInfo;
      else if (iInfo.lowerLimit >= maxIntervalInfo.lowerLimit &&
               iInfo.upperLimit >= maxIntervalInfo.upperLimit)
        maxIntervalInfo = iInfo;
    }
    t.ChangeByHours(1);
  }

  if (varyingHours <= 2)
    return lastPeriod;

  float lowerDiff = abs(minIntervalInfo.lowerLimit - maxIntervalInfo.lowerLimit);
  float upperDiff = abs(minIntervalInfo.upperLimit - maxIntervalInfo.upperLimit);
  minIntervalInfo.startTime = lastPeriod.localStartTime();
  minIntervalInfo.endTime = (lowerDiff + upperDiff > 5.0) ? lastPeriod.localStartTime()
                                                           : lastPeriod.localEndTime();
  return WeatherPeriod(minIntervalInfo.startTime, minIntervalInfo.endTime);
}

// Build the "last" reporting period for the last reporting index
WeatherPeriod WindForecast::buildLastReportingPeriod(
    const WindDataItemUnit& windDataItem,
    const WeatherPeriod& speedEventPeriod,
    WindEventId windEvent) const
{
  WeatherPeriod lastPeriod(windDataItem.thePeriod.localStartTime(), speedEventPeriod.localEndTime());

  if (windEvent == TUULI_HEIKKENEE)
    lastPeriod = findWeakeningLastPeriod(lastPeriod, windDataItem.thePeriod.localStartTime(),
                                         speedEventPeriod);

  theParameters.theLog << "Last reporting period " << as_string(lastPeriod) << '\n';
  return lastPeriod;
}

// Collect reporting indexes within the speed event period
std::vector<unsigned int> WindForecast::collectReportingIndexes(
    unsigned int begIndex,
    unsigned int& endIndex,
    const WeatherPeriod& speedEventPeriod,
    bool firstSentenceInTheStory) const
{
  std::vector<unsigned int> reportingIndexes;
  float previousTopWind = kFloatMissing;
  TextGenPosixTime previousTime;

  for (unsigned int i = begIndex; i < theParameters.theWindDataVector.size(); i++)
  {
    const WindDataItemUnit& windDataItem =
        (*theParameters.theWindDataVector[i])(theParameters.theArea.type());

    if (!is_inside(windDataItem.thePeriod.localStartTime(), speedEventPeriod))
    {
      if ((firstSentenceInTheStory && reportingIndexes.size() == 1) || reportingIndexes.empty())
      {
        const WindDataItemUnit& prev =
            (*theParameters.theWindDataVector[i - 1])(theParameters.theArea.type());
        reportingIndexes.push_back(i - 1);
        previousTopWind = prev.theEqualizedTopWind.value();
        previousTime = prev.thePeriod.localStartTime();
        theParameters.theLog << "Reporting wind speed (case C) at "
                             << as_string(prev.thePeriod) << '\n';
      }
      break;
    }

    if (i == begIndex)
    {
      if (firstSentenceInTheStory)
      {
        reportingIndexes.push_back(i);
        theParameters.theLog << "Reporting wind speed (case A) at "
                             << as_string(windDataItem.thePeriod) << '\n';
      }
      previousTopWind = windDataItem.theEqualizedTopWind.value();
      previousTime = windDataItem.thePeriod.localStartTime();
    }
    else
    {
      if (windDataItem.theEqualizedTopWind.value() > WEAK_WIND_SPEED_UPPER_LIMIT ||
          previousTopWind > WEAK_WIND_SPEED_UPPER_LIMIT)
      {
        WeatherPeriod p(previousTime, windDataItem.thePeriod.localStartTime());
        if (wind_speed_differ_enough(theParameters, p))
        {
          reportingIndexes.push_back(i);
          previousTime = windDataItem.thePeriod.localStartTime();
          theParameters.theLog << "Reporting wind speed (case B) at "
                               << as_string(windDataItem.thePeriod) << '\n';
        }
      }
    }
    endIndex = i;
  }
  return reportingIndexes;
}

// Select the second period from a list of candidates (highest strengthening / lowest weakening)
WeatherPeriod WindForecast::selectSecondPeriod(const std::vector<WeatherPeriod>& candidates,
                                               WindEventId windEvent) const
{
  unsigned int selectedIndex = 1;
  interval_info selectedInfo = windSpeedIntervalInfo(candidates[selectedIndex]);
  for (unsigned int i = 2; i < candidates.size(); i++)
  {
    interval_info cmpInfo = windSpeedIntervalInfo(candidates[i]);
    bool betterStrengthening = windEvent == TUULI_VOIMISTUU &&
                               cmpInfo.lowerLimit > selectedInfo.lowerLimit &&
                               cmpInfo.upperLimit > selectedInfo.upperLimit;
    bool betterWeakening = windEvent == TUULI_HEIKKENEE &&
                           cmpInfo.lowerLimit < selectedInfo.lowerLimit &&
                           cmpInfo.upperLimit < selectedInfo.upperLimit;
    if (betterStrengthening || betterWeakening)
    {
      selectedIndex = i;
      selectedInfo = cmpInfo;
    }
  }
  const WeatherPeriod& result = candidates[selectedIndex];
  theParameters.theLog << "Actual last reporting period " << as_string(result) << '\n';
  return result;
}

std::vector<WeatherPeriod> WindForecast::getWindSpeedReportingPeriods(
    const WindEventPeriodDataItem& eventPeriodDataItem, bool firstSentenceInTheStory) const
{
  WeatherPeriod speedEventPeriod(eventPeriodDataItem.theWindSpeedChangePeriod);

  theParameters.theLog << "Find out wind speed reporting points/period at period "
                       << as_string(speedEventPeriod) << '\n';

  // Find the beginning index inside the speed event period
  unsigned int begIndex = 0;
  do
  {
    const WindDataItemUnit& windDataItem =
        (*theParameters.theWindDataVector[begIndex])(theParameters.theArea.type());
    if (is_inside(windDataItem.thePeriod.localStartTime(), speedEventPeriod) ||
        begIndex == theParameters.theWindDataVector.size() - 1)
      break;
    begIndex++;
  } while (true);

  std::vector<WeatherPeriod> resultVector;

  if (begIndex >= theParameters.theWindDataVector.size())
    return resultVector;

  unsigned int endIndex = begIndex;
  std::vector<unsigned int> reportingIndexes =
      collectReportingIndexes(begIndex, endIndex, speedEventPeriod, firstSentenceInTheStory);

  const WindDataItemUnit& windDataItemLast =
      (*theParameters.theWindDataVector[endIndex])(theParameters.theArea.type());

  for (unsigned int i = 0; i < reportingIndexes.size(); i++)
  {
    unsigned int index = reportingIndexes[i];
    const WindDataItemUnit& windDataItem =
        (*theParameters.theWindDataVector[index])(theParameters.theArea.type());

    if (reportingIndexes.size() == 1)
    {
      WeatherPeriod remainingPeriod(windDataItem.thePeriod.localStartTime(),
                                    windDataItemLast.thePeriod.localEndTime());
      resultVector.push_back(remainingPeriod);
      theParameters.theLog << "Only one reporting point, extend the period to the end "
                           << as_string(remainingPeriod) << '\n';
    }
    else if (i < reportingIndexes.size() - 1)
    {
      resultVector.push_back(windDataItem.thePeriod);
      theParameters.theLog << "Report speed at certain point of time "
                           << as_string(windDataItem.thePeriod) << '\n';
    }
    else
    {
      resultVector.push_back(buildLastReportingPeriod(windDataItem, speedEventPeriod,
                                                       eventPeriodDataItem.theWindEvent));
    }
  }

  // max two intervals reported; select the best second period
  std::vector<WeatherPeriod> retVector;
  if (!resultVector.empty())
    retVector.push_back(resultVector.front());

  if (resultVector.size() == 2)
    retVector.push_back(resultVector.back());
  else if (resultVector.size() > 2)
    retVector.push_back(selectSecondPeriod(resultVector, eventPeriodDataItem.theWindEvent));

  // adjust last period if it's a single-hour period at the end of the forecast
  if (!retVector.empty())
  {
    WeatherPeriod& lastPeriod = retVector.back();
    if (theParameters.theForecastPeriod.localEndTime() == lastPeriod.localEndTime() &&
        get_period_length(lastPeriod) == 1)
    {
      TextGenPosixTime startTime = lastPeriod.localStartTime();
      TextGenPosixTime endTime = lastPeriod.localEndTime();
      startTime.ChangeByHours(-1);
      theParameters.theLog << "Last speed reporting period changed from " << as_string(lastPeriod)
                           << " to " << as_string(WeatherPeriod(startTime, endTime)) << '\n';
      lastPeriod = WeatherPeriod(startTime, endTime);
    }
  }

  return retVector;
}

bool WindForecast::getWindSpeedChangeAttribute(const WeatherPeriod& changePeriod,
                                               std::string& phraseStr,
                                               bool& smallChange,
                                               bool& gradualChange,
                                               bool& fastChange) const
{
  float begLowerLimit = kFloatMissing;
  float begUpperLimit = kFloatMissing;
  float endLowerLimit = kFloatMissing;
  float endUpperLimit = kFloatMissing;

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
                         << '\n';
    return false;
  }

  // begLowerLimit = std::round(begLowerLimit);
  begUpperLimit = std::round(begUpperLimit);
  // endLowerLimit = std::round(endLowerLimit);
  endUpperLimit = std::round(endUpperLimit);

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

  float intervalLowerLimit = kFloatMissing;
  float intervalUpperLimit = kFloatMissing;

  get_wind_speed_interval(
      thePeriod, theParameters, intervalLowerLimit, intervalUpperLimit, ret.peakWindTime);

  ret.startTime = thePeriod.localStartTime();
  ret.endTime = thePeriod.localEndTime();
  ret.lowerLimit = std::round(intervalLowerLimit);
  ret.upperLimit = std::round(intervalUpperLimit);

  /*
  theParameters.theLog << " rounded interval: " << ret.lowerLimit << "..." << ret.upperLimit
                       << " peak time: " << ret.peakWindTime << '\n';
  */

  ret.peakWind = ret.upperLimit;

  compose_wind_speed_range(theParameters, ret.lowerLimit, ret.upperLimit, ret.peakWind);

  theParameters.theLog << " wind speed interval for period " << as_string(thePeriod) << " is "
                       << ret.lowerLimit << "..." << ret.upperLimit << " peak: " << ret.peakWind
                       << '\n';

  return ret;
}

// returns true if id1 is later than id2 false otherwise
bool is_later(part_of_the_day_id id1, part_of_the_day_id id2)
{
  // Assign a day-order priority to each part-of-day id.
  // Returns true if id1 occurs strictly later in the day than id2.
  static const std::map<part_of_the_day_id, int> priority = {
      {YO, 0},
      {KESKIYO, 0},
      {AAMUYO, 1},
      {AAMU, 2},
      {AAMUPAIVA, 3},
      {PAIVA, 3},
      {KESKIPAIVA, 4},
      {ILTAPAIVA, 5},
      {ILTA, 6},
  };
  auto it1 = priority.find(id1);
  auto it2 = priority.find(id2);
  if (it1 == priority.end() || it2 == priority.end())
    return false;
  return it1->second > it2->second;
}

Sentence WindForecast::windSpeedIntervalSentence(const WeatherPeriod& /*thePeriod*/,
                                                 const interval_info& intervalInfo,
                                                 TimePhraseInfo& timePhraseInfo,
                                                 bool theUseAtItsStrongestPhrase) const
{
  Sentence sentence;

  Sentence timePhrase;
  if (!timePhraseInfo.empty())
  {
    TimePhraseInfo tpi;
    tpi.day_number = timePhraseInfo.day_number;
    tpi.part_of_the_day = timePhraseInfo.part_of_the_day;

    timePhrase << getTimePhrase(intervalInfo.peakWindTime, tpi, false);

    if (tpi != timePhraseInfo && theUseAtItsStrongestPhrase)
    {
      bool different_days =
          (timePhraseInfo.starttime.GetDay() != intervalInfo.peakWindTime.GetDay());
      if (different_days || is_later(tpi.part_of_the_day, timePhraseInfo.part_of_the_day))
        timePhraseInfo = tpi;
      else
        timePhrase.clear();
    }
    else
      timePhrase.clear();
  }

  return compose_wind_speed_sentence(
      theParameters, intervalInfo, timePhrase, theUseAtItsStrongestPhrase);
}

Sentence WindForecast::windSpeedIntervalSentence(const WeatherPeriod& thePeriod,
                                                 TimePhraseInfo& timePhraseInfo,
                                                 bool theUseAtItsStrongestPhrase) const
{
  Sentence sentence;

  interval_info intervalInfo = windSpeedIntervalInfo(thePeriod);

  Sentence timePhrase;
  if (!timePhraseInfo.empty())
  {
    TimePhraseInfo tpi;
    tpi.day_number = timePhraseInfo.day_number;
    tpi.part_of_the_day = timePhraseInfo.part_of_the_day;

    timePhrase << getTimePhrase(intervalInfo.peakWindTime, tpi, false);

    if (tpi != timePhraseInfo && theUseAtItsStrongestPhrase)
    {
      bool different_days =
          (timePhraseInfo.starttime.GetDay() != intervalInfo.peakWindTime.GetDay());
      if (different_days || is_later(tpi.part_of_the_day, timePhraseInfo.part_of_the_day))
        timePhraseInfo = tpi;
      else
        timePhrase.clear();
    }
    else
      timePhrase.clear();
  }

  return compose_wind_speed_sentence(
      theParameters, intervalInfo, timePhrase, theUseAtItsStrongestPhrase);
}

std::vector<sentence_parameter> WindForecast::reportWindDirectionChanges(
    const std::vector<WindDirectionInfo>& directionChanges,
    TimePhraseInfo& timePhraseInfo,
    bool startWithComma /*= true*/) const
{
  std::vector<sentence_parameter> ret;

  for (const auto& directionInfo : directionChanges)
  {
    if (directionInfo.empty())
      continue;

    if (!ret.empty() || (ret.empty() && startWithComma))
    {
      sentence_parameter sp;
      sp.sentence << Delimiter(COMMA_PUNCTUATION_MARK);
      ret.push_back(sp);
    }

    sentence_parameter sp2(TIME_PERIOD);
    sp2.sentence << getTimePhrase(
        directionInfo.period, timePhraseInfo, get_period_length(directionInfo.period) > 6);

    sp2.tpi = timePhraseInfo;
    ret.push_back(sp2);

    sentence_parameter sp3(WIND_DIRECTION);
    sp3.sentence << windDirectionSentence(directionInfo);
    ret.push_back(sp3);
  }

  return ret;
}

}  // namespace TextGen
