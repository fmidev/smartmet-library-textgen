// ======================================================================
/*!
 * \file
 * \brief Implementation of PrecipitationForecast class
 */
// ======================================================================

#include "PrecipitationForecast.h"

#include "AreaTools.h"
#include "CloudinessForecast.h"
#include "CloudinessStory.h"
#include "CloudinessStoryTools.h"
#include "DebugTextFormatter.h"
#include "Delimiter.h"
#include "MessageLogger.h"
#include "NightAndDayPeriodGenerator.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "PrecipitationStoryTools.h"
#include "SeasonTools.h"
#include "Sentence.h"
#include "SubMaskExtractor.h"
#include "ThunderForecast.h"
#include "ValueAcceptor.h"
#include "WeatherStory.h"
#include "WeekdayTools.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <calculator/GridForecaster.h>
#include <calculator/HourPeriodGenerator.h>
#include <calculator/MathTools.h>
#include <calculator/NullPeriodGenerator.h>
#include <calculator/RangeAcceptor.h>
#include <calculator/Settings.h>
#include <calculator/TimeTools.h>
#include <calculator/WeatherPeriodTools.h>
#include <calculator/WeatherResult.h>
#include <macgyver/Exception.h>
#include <newbase/NFmiAreaTools.h>
#include <iomanip>
#include <map>
#include <vector>

using namespace std;

namespace TextGen
{
using namespace Settings;
using namespace TextGen;
using namespace AreaTools;
using namespace TimeTools;

// pouta
#define HUOMENNA_SISAMAASSA_SAA_ON_POUTAINEN_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] saa on poutainen"
#define SISAMAASSA_SAA_ON_POUTAINEN_COMPOSITE_PHRASE "[sisamaassa] saa on poutainen"
#define HUOMENNA_SAA_ON_POUTAINEN_COMPOSITE_PHRASE "[huomenna] saa on poutainen"
#define HUOMENNA_SISAMAASSA_SAA_ON_ENIMMAKSEEN_POUTAINEN_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] saa on enimmakseen poutainen, [yksittaiset sadekuurot mahdollisia]"

// enimmakseen pouta, yksittaiset kuurot mahdollisia
#define SISAMAASSA_SAA_ON_ENIMMAKSEEN_POUTAINEN_COMPOSITE_PHRASE \
  "[sisamaassa] saa on enimmakseen poutainen, [yksittaiset sadekuurot mahdollisia]"
#define HUOMENNA_SAA_ON_ENIMMAKSEEN_POUTAINEN_COMPOSITE_PHRASE \
  "[huomenna] saa on enimmakseen poutainen, [yksittaiset sadekuurot mahdollisia]"
// sade
#define ILTAPAIVALLA_KUUROT_VOIVAT_OLLA_VOIMAKKAITA "[iltapaivalla] kuurot voivat olla voimakkaita"
#define ILTAPAIVALLA_LUMIKUUROT_VOIVAT_OLLA_SAKEITA "[iltapaivalla] lumikuurot voivat olla sakeita"
#define ILTAPAIVALLA_SADE_VOI_OLLA_RUNSASTA "[iltapaivalla] sade voi olla runsasta"
#define ILTAPAIVALLA_LUMISADE_VOI_OLLA_SAKEAA "[iltapaivalla] lumisade voi olla sakeaa"
#define HUOMENNA_SISAMAASSA_PAIKOIN_HEIKKOA_SADETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [paikoin] [heikkoa] [sadetta]"
#define HUOMENNA_SISAMAASSA_HEIKKOA_SADETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [heikkoa] [sadetta]"
#define HUOMENNA_SISAMAASSA_PAIKOIN_SADETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [paikoin] [sadetta]"

#define SISAMAASSA_PAIKOIN_HEIKKOA_SADETTA_COMPOSITE_PHRASE \
  "[sisamaassa] [paikoin] [heikkoa] [sadetta]"
#define SISAMAASSA_PAIKOIN_SADETTA_COMPOSITE_PHRASE "[sisamaassa] [paikoin] [sadetta]"
#define SISAMAASSA_HEIKKOA_SADETTA_COMPOSITE_PHRASE "[sisamaassa] [heikkoa] [sadetta]"
#define SISAMAASSA_SADETTA_COMPOSITE_PHRASE "[sisamaassa] [sadetta]"

#define HUOMENNA_PAIKOIN_HEIKKOA_SADETTA_COMPOSITE_PHRASE "[huomenna] [paikoin] [heikkoa] [sadetta]"
#define HUOMENNA_PAIKOIN_SADETTA_COMPOSITE_PHRASE "[huomenna] [paikoin] [sadetta]"
#define HUOMENNA_HEIKKOA_SADETTA_COMPOSITE_PHRASE "[huomenna] [heikkoa] [sadetta]"
#define HUOMENNA_SADETTA_COMPOSITE_PHRASE "[huomenna] [sadetta]"

#define PAIKOIN_HEIKKOA_SADETTA_COMPOSITE_PHRASE "[paikoin] [heikkoa] [sadetta]"
#define PAIKOIN_SADETTA_COMPOSITE_PHRASE "[paikoin] [sadetta]"
#define HEIKKOA_SADETTA_COMPOSITE_PHRASE "[heikkoa] [sadetta]"

#define HUOMENNA_YKSITTAISET_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[huomenna] yksittaiset vesikuurot mahdollisia"
#define HUOMENNA_YKSITTAISET_VESI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[huomenna] yksittaiset vesi- tai rantakuurot mahdollisia"
#define HUOMENNA_YKSITTAISET_VESI_TAI_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[huomenna] yksittaiset vesi- tai lumikuurot mahdollisia"
#define HUOMENNA_YKSITTAISET_SADEKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[huomenna] yksittaiset sadekuurot mahdollisia"
#define HUOMENNA_YKSITTAISET_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[huomenna] yksittaiset rantakuurot mahdollisia"
#define HUOMENNA_YKSITTAISET_RANTA_TAI_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[huomenna] yksittaiset ranta- tai vesikuurot mahdollisia"
#define HUOMENNA_YKSITTAISET_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[huomenna] yksittaiset lumikuurot mahdollisia"
#define HUOMENNA_YKSITTAISET_LUMI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[huomenna] yksittaiset lumi- tai rantakuurot mahdollisia"

#define SISAMAASSA_YKSITTAISET_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[sisamaassa] yksittaiset vesikuurot mahdollisia"
#define SISAMAASSA_YKSITTAISET_VESI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[sisamaassa] yksittaiset vesi- tai rantakuurot mahdollisia"
#define SISAMAASSA_YKSITTAISET_VESI_TAI_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[sisamaassa] yksittaiset vesi- tai lumikuurot mahdollisia"
#define SISAMAASSA_YKSITTAISET_SADEKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[sisamaassa] yksittaiset sadekuurot mahdollisia"
#define SISAMAASSA_YKSITTAISET_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[sisamaassa] yksittaiset rantakuurot mahdollisia"
#define SISAMAASSA_YKSITTAISET_RANTA_TAI_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[sisamaassa] yksittaiset ranta- tai vesikuurot mahdollisia"
#define SISAMAASSA_YKSITTAISET_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[sisamaassa] yksittaiset lumikuurot mahdollisia"
#define SISAMAASSA_YKSITTAISET_LUMI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[sisamaassa] yksittaiset lumi- tai rantakuurot mahdollisia"

#define HUOMENNA_SISAMAASSA_YKSITTAISET_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] yksittaiset vesikuurot mahdollisia"
#define HUOMENNA_SISAMAASSA_YKSITTAISET_VESI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] yksittaiset vesi- tai rantakuurot mahdollisia"
#define HUOMENNA_SISAMAASSA_YKSITTAISET_VESI_TAI_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] yksittaiset vesi- tai lumikuurot mahdollisia"
#define HUOMENNA_SISAMAASSA_YKSITTAISET_SADEKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] yksittaiset sadekuurot mahdollisia"
#define HUOMENNA_SISAMAASSA_YKSITTAISET_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] yksittaiset rantakuurot mahdollisia"
#define HUOMENNA_SISAMAASSA_YKSITTAISET_RANTA_TAI_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] yksittaiset ranta- tai vesikuurot mahdollisia"
#define HUOMENNA_SISAMAASSA_YKSITTAISET_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] yksittaiset lumikuurot mahdollisia"
#define HUOMENNA_SISAMAASSA_YKSITTAISET_LUMI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] yksittaiset lumi- tai rantakuurot mahdollisia"

#define HUOMENNA_SISAMAASSA_SADETTA_COMPOSITE_PHRASE "[huomenna] [sisamaassa] [sadetta]"

#define HUOMENNA_SISAMAASSA_PAIKOIN_HEIKKOA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [paikoin] [heikkoa] [sadetta], joka voi olla jaatavaa"
#define HUOMENNA_SISAMAASSA_PAIKOIN_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [paikoin] [sadetta], joka voi olla jaatavaa"
#define HUOMENNA_SISAMAASSA_HEIKKOA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [heikkoa] [sadetta], joka voi olla jaatavaa"
#define HUOMENNA_SISAMAASSA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [sadetta], joka voi olla jaatavaa"

#define HUOMENNA_PAIKOIN_HEIKKOA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE \
  "[huomenna] [paikoin] [heikkoa] [sadetta], joka voi olla jaatavaa"
#define HUOMENNA_PAIKOIN_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE \
  "[huomenna] [paikoin] [sadetta], joka voi olla jaatavaa"
#define HUOMENNA_HEIKKOA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE \
  "[huomenna] [heikkoa] [sadetta], joka voi olla jaatavaa"
#define HUOMENNA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE \
  "[huomenna] [sadetta], joka voi olla jaatavaa"
#define SISAMAASSA_PAIKOIN_HEIKKOA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE \
  "[sisamaassa] [paikoin] [heikkoa] [sadetta], joka voi olla jaatavaa"
#define SISAMAASSA_PAIKOIN_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE \
  "[sisamaassa] [paikoin] [sadetta], joka voi olla jaatavaa"
#define SISAMAASSA_HEIKKOA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE \
  "[sisamaassa] [heikkoa] [sadetta], joka voi olla jaatavaa"
#define SISAMAASSA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE \
  "[sisamaassa] [sadetta], joka voi olla jaatavaa"
#define PAIKOIN_HEIKKOA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE \
  "[paikoin] [heikkoa] [sadetta], joka voi olla jaatavaa"
#define PAIKOIN_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE \
  "[paikoin] [sadetta], joka voi olla jaatavaa"
#define HEIKKOA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE \
  "[heikkoa] [sadetta], joka voi olla jaatavaa"
#define SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE "[sadetta], joka voi olla jaatavaa"

#define HUOMENNA_SISAMAASSA_PAIKOIN_HEIKKOJA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [paikoin] [heikkoja] [sadekuuroja], jotka voivat olla jaatavia"
#define HUOMENNA_SISAMAASSA_HEIKKOJA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [heikkoja] [sadekuuroja], jotka voivat olla jaatavia"
#define HUOMENNA_SISAMAASSA_PAIKOIN_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [paikoin] [sadekuuroja], jotka voivat olla jaatavia"

#define HUOMENNA_SISAMAASSA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [sadekuuroja], jotka voivat olla jaatavia"

#define HUOMENNA_PAIKOIN_HEIKKOJA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE \
  "[huomenna] [paikoin] [heikkoja] [sadekuuroja], jotka voivat olla jaatavia"
#define HUOMENNA_PAIKOIN_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE \
  "[huomenna] [paikoin] [sadekuuroja], jotka voivat olla jaatavia"
#define HUOMENNA_HEIKKOJA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE \
  "[huomenna] [heikkoja] [sadekuuroja], jotka voivat olla jaatavia"
#define HUOMENNA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE \
  "[huomenna] [sadekuuroja], jotka voivat olla jaatavia"
#define SISAMAASSA_PAIKOIN_HEIKKOJA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE \
  "[sisamaassa] [paikoin] [heikkoja] [sadekuuroja], jotka voivat olla jaatavia"
#define SISAMAASSA_PAIKOIN_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE \
  "[sisamaassa] [paikoin] [sadekuuroja], jotka voivat olla jaatavia"
#define SISAMAASSA_HEIKKOJA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE \
  "[sisamaassa] [heikkoja] [sadekuuroja], jotka voivat olla jaatavia"
#define SISAMAASSA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE \
  "[sisamaassa] [sadekuuroja], jotka voivat olla jaatavia"
#define PAIKOIN_HEIKKOJA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE \
  "[paikoin] [heikkoja] [sadekuuroja], jotka voivat olla jaatavia"
#define PAIKOIN_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE \
  "[paikoin] [sadekuuroja], jotka voivat olla jaatavia"
#define HEIKKOJA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE \
  "[heikkoja] [sadekuuroja], jotka voivat olla jaatavia"
#define SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE \
  "[sadekuuroja], jotka voivat olla jaatavia"

// #define SAA_POUTAANTUU_COMPOSITE_PHRASE "[iltapaivalla] saa poutaantuu"

#define SAA_POUTAANTUU_VESISATEEN_JALKEEN_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu vesisateen jalkeen"
#define SAA_POUTAANTUU_RANTASATEEN_JALKEEN_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu rantasateen jalkeen"
#define SAA_POUTAANTUU_LUMISATEEN_JALKEEN_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu lumisateen jalkeen"
#define SAA_POUTAANTUU_TIHKUSATEEN_JALKEEN_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu tihkusateen jalkeen"

// #define SAA_POUTAANTUU_JA_ON_SELKEA_COMPOSITE_PHRASE "[iltapaivalla] saa poutaantuu ja on
//[selkea]"

#define SAA_POUTAANTUU_VESISATEEN_JALKEEN_JA_ON_SELKEA_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu vesisateen jalkeen ja on selkea"
#define SAA_POUTAANTUU_RANTASATEEN_JALKEEN_JA_ON_SELKEA_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu rantasateen jalkeen ja on selkea"
#define SAA_POUTAANTUU_LUMISATEEN_JALKEEN_JA_ON_SELKEA_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu lumisateen jalkeen ja on selkea"
#define SAA_POUTAANTUU_TIHKUSATEEN_JALKEEN_JA_ON_SELKEA_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu tihkusateen jalkeen ja on selkea"

#define SAA_POUTAANTUU_VESISATEEN_JALKEEN_JA_ON_MELKO_SELKEA_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu vesisateen jalkeen ja on melko selkea"
#define SAA_POUTAANTUU_RANTASATEEN_JALKEEN_JA_ON_MELKO_SELKEA_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu rantasateen jalkeen ja on melko selkea"
#define SAA_POUTAANTUU_LUMISATEEN_JALKEEN_JA_ON_MELKO_SELKEA_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu lumisateen jalkeen ja on melko selkea"
#define SAA_POUTAANTUU_TIHKUSATEEN_JALKEEN_JA_ON_MELKO_SELKEA_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu tihkusateen jalkeen ja on melko selkea"

#define SAA_POUTAANTUU_VESISATEEN_JALKEEN_JA_ON_PUOLIPILVINEN_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu vesisateen jalkeen ja on puolipilvinen"
#define SAA_POUTAANTUU_RANTASATEEN_JALKEEN_JA_ON_PUOLIPILVINEN_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu rantasateen jalkeen ja on puolipilvinen"
#define SAA_POUTAANTUU_LUMISATEEN_JALKEEN_JA_ON_PUOLIPILVINEN_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu lumisateen jalkeen ja on puolipilvinen"
#define SAA_POUTAANTUU_TIHKUSATEEN_JALKEEN_JA_ON_PUOLIPILVINEN_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu tihkusateen jalkeen ja on puolipilvinen"

#define SAA_POUTAANTUU_VESISATEEN_JALKEEN_JA_ON_VERRATTAIN_PILVINEN_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu vesisateen jalkeen ja on verrattain pilvinen"
#define SAA_POUTAANTUU_RANTASATEEN_JALKEEN_JA_ON_VERRATTAIN_PILVINEN_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu rantasateen jalkeen ja on verrattain pilvinen"
#define SAA_POUTAANTUU_LUMISATEEN_JALKEEN_JA_ON_VERRATTAIN_PILVINEN_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu lumisateen jalkeen ja on verrattain pilvinen"
#define SAA_POUTAANTUU_TIHKUSATEEN_JALKEEN_JA_ON_VERRATTAIN_PILVINEN_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu tihkusateen jalkeen ja on verrattain pilvinen"

#define SAA_POUTAANTUU_VESISATEEN_JALKEEN_JA_ON_PILVINEN_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu vesisateen jalkeen ja on pilvinen"
#define SAA_POUTAANTUU_RANTASATEEN_JALKEEN_JA_ON_PILVINEN_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu rantasateen jalkeen ja on pilvinen"
#define SAA_POUTAANTUU_LUMISATEEN_JALKEEN_JA_ON_PILVINEN_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu lumisateen jalkeen ja on pilvinen"
#define SAA_POUTAANTUU_TIHKUSATEEN_JALKEEN_JA_ON_PILVINEN_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu tihkusateen jalkeen ja on pilvinen"

// #define SAA_POUTAANTUU_JA_VAIHTELEE_COMPOSITE_PHRASE "[iltapaivalla] saa poutaantuu ja vaihtelee
//  puolipilvisesta pilviseen"

#define SAA_POUTAANTUU_VESISATEEN_JALKEEN_JA_VAIHTELEE_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu vesisateen jalkeen ja vaihtelee puolipilvisesta pilviseen"
#define SAA_POUTAANTUU_RANTASATEEN_JALKEEN_JA_VAIHTELEE_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu rantasateen jalkeen ja vaihtelee puolipilvisesta pilviseen"
#define SAA_POUTAANTUU_LUMISATEEN_JALKEEN_JA_VAIHTELEE_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu lumisateen jalkeen ja vaihtelee puolipilvisesta pilviseen"
#define SAA_POUTAANTUU_TIHKUSATEEN_JALKEEN_JA_VAIHTELEE_COMPOSITE_PHRASE \
  "[iltapaivalla] saa poutaantuu tihkusateen jalkeen ja vaihtelee puolipilvisesta pilviseen"

namespace
{
InPlacesPhrase& get_in_places_phrase()
{
  static boost::thread_specific_ptr<InPlacesPhrase> tls;

  if (!tls.get())
    tls.reset(new InPlacesPhrase);

  return *tls;
}

std::ostream& operator<<(std::ostream& theOutput,
                         const PrecipitationDataItemData& thePrecipitationDataItemData)
{
  theOutput << thePrecipitationDataItemData.theObservationTime << ": ";
  theOutput << setw(8) << setfill(' ') << fixed << setprecision(3)
            << thePrecipitationDataItemData.thePrecipitationIntensity;
  theOutput << setw(8) << setfill(' ') << fixed << setprecision(3)
            << thePrecipitationDataItemData.thePrecipitationMaxIntensity;
  theOutput << setw(8) << setfill(' ') << fixed << setprecision(1)
            << thePrecipitationDataItemData.thePrecipitationExtent;
  if (thePrecipitationDataItemData.thePrecipitationIntensity > 0)
  {
    theOutput << " ";
    if (thePrecipitationDataItemData.thePrecipitationFormWater > 0.0)
      theOutput << "    water=" << setw(1) << setfill(' ') << fixed << setprecision(0)
                << thePrecipitationDataItemData.thePrecipitationFormWater;
    if (thePrecipitationDataItemData.thePrecipitationFormDrizzle > 0.0)
      theOutput << "    drizzle=" << setw(1) << setfill(' ') << fixed << setprecision(0)
                << thePrecipitationDataItemData.thePrecipitationFormDrizzle;
    if (thePrecipitationDataItemData.thePrecipitationFormSleet > 0.0)
      theOutput << "    sleet=" << setw(1) << setfill(' ') << fixed << setprecision(0)
                << thePrecipitationDataItemData.thePrecipitationFormSleet;
    if (thePrecipitationDataItemData.thePrecipitationFormSnow > 0.0)
      theOutput << "    snow=" << setw(1) << setfill(' ') << fixed << setprecision(0)
                << thePrecipitationDataItemData.thePrecipitationFormSnow;
    if (thePrecipitationDataItemData.thePrecipitationFormFreezingRain > 0.0)
      theOutput << "    freezing rain=" << setw(1) << setfill(' ') << fixed << setprecision(0)
                << thePrecipitationDataItemData.thePrecipitationFormFreezingRain;
    if (thePrecipitationDataItemData.thePrecipitationFormFreezingDrizzle > 0.0)
      theOutput << "    freezing drizzle=" << setw(1) << setfill(' ') << fixed << setprecision(0)
                << thePrecipitationDataItemData.thePrecipitationFormFreezingDrizzle;
    if (thePrecipitationDataItemData.thePrecipitationTypeShowers > 0.0)
      theOutput << "    showers=" << setw(1) << setfill(' ') << fixed << setprecision(0)
                << thePrecipitationDataItemData.thePrecipitationTypeShowers;
    theOutput << '\n';

    /*
    theOutput << " ne=" << fixed << setprecision(0)
                      << thePrecipitationDataItemData.thePrecipitationPercentageNorthEast << " "
                      << "se=" << fixed << setprecision(0)
                      << thePrecipitationDataItemData.thePrecipitationPercentageSouthEast << " "
                      << "sw=" << fixed << setprecision(0)
                      << thePrecipitationDataItemData.thePrecipitationPercentageSouthWest << " "
                      << "nw=" << fixed << setprecision(0)
                      << thePrecipitationDataItemData.thePrecipitationPercentageNorthWest << " ";

    theOutput << " coord=" << setw(3) << setfill(' ') << fixed << setprecision(2)
                      << thePrecipitationDataItemData.thePrecipitationPoint.X()
                      << ", "
                      << setw(3) << setfill(' ') << fixed << setprecision(2)
                      << thePrecipitationDataItemData.thePrecipitationPoint.Y() << '\n';
    */
  }
  else
  {
    theOutput << '\n';
  }

  return theOutput;
}

weather_result_data_item_vector* get_data_vector(wf_story_params& theParameters,
                                                 weather_result_data_id theDataId)
{
  weather_result_data_item_vector* retval = nullptr;

  forecast_area_id areaId(NO_AREA);
  if (theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
    areaId = FULL_AREA;
  else if (theParameters.theForecastArea & COASTAL_AREA)
    areaId = COASTAL_AREA;
  else if (theParameters.theForecastArea & INLAND_AREA)
    areaId = INLAND_AREA;

  if (areaId != NO_AREA)
    retval = (*theParameters.theCompleteData[areaId])[theDataId].get();

  return retval;
}

bool get_period_start_end_index(const WeatherPeriod& thePeriod,
                                const weather_result_data_item_vector& theDataVector,
                                unsigned int& theStartIndex,
                                unsigned int& theEndIndex)
{
  theStartIndex = 0;
  theEndIndex = 0;

  bool startFound = false;
  for (unsigned int i = 0; i < theDataVector.size(); i++)
  {
    // Note: in weather result vector start and end times are same
    if (theDataVector.at(i)->thePeriod.localStartTime() >= thePeriod.localStartTime() &&
        theDataVector.at(i)->thePeriod.localStartTime() <= thePeriod.localEndTime())
    {
      if (!startFound)
      {
        startFound = true;
        theStartIndex = i;
      }
      if (startFound)
        theEndIndex = i;
    }
  }
  return startFound;
}

void can_be_freezing_phrase(bool theUseIcingPhraseFlag,
                            bool theCanBeFreezingFlag,
                            map<string, Sentence>& theCompositePhraseElements,
                            bool thePluralFlag)
{
  if (theUseIcingPhraseFlag && theCanBeFreezingFlag)
  {
    if (thePluralFlag)
    {
      theCompositePhraseElements[JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE] << SAA_WORD;
    }
    else
    {
      theCompositePhraseElements[JOKA_VOI_OLLA_JAATAVAA_PHRASE] << SAA_WORD;
    }
  }
}

bool is_dry_weather(const wf_story_params& theParameters,
                    precipitation_form_id thePrecipitationForm,
                    float thePrecipitationIntensity,
                    float thePrecipitationExtent)
{
  if (thePrecipitationExtent == 0.0 || thePrecipitationExtent == kFloatMissing ||
      thePrecipitationIntensity == kFloatMissing)
    return true;

  bool dry_weather = false;

  switch (thePrecipitationForm)
  {
    case WATER_FREEZING_FORM:
    case FREEZING_FORM:
    case WATER_FORM:
    case SLEET_FREEZING_FORM:
    case SLEET_FORM:
    case WATER_DRIZZLE_FREEZING_FORM:
    case WATER_DRIZZLE_FORM:
    case WATER_DRIZZLE_SLEET_FORM:
    case WATER_SLEET_FREEZING_FORM:
    case WATER_SLEET_FORM:
    case WATER_SNOW_FREEZING_FORM:
    case WATER_SNOW_FORM:
    case WATER_DRIZZLE_SNOW_FORM:
    case WATER_SLEET_SNOW_FORM:
      if (thePrecipitationIntensity < theParameters.theDryWeatherLimitWater)
        dry_weather = true;
      break;
    case SNOW_FORM:
    case SNOW_FREEZING_FORM:
      if (thePrecipitationIntensity < theParameters.theDryWeatherLimitSnow)
        dry_weather = true;
      break;
    case DRIZZLE_FORM:
    case DRIZZLE_FREEZING_FORM:
    case DRIZZLE_SNOW_FREEZING_FORM:
    case DRIZZLE_SNOW_FORM:
    case DRIZZLE_SLEET_SNOW_FORM:
      if (thePrecipitationIntensity < theParameters.theDryWeatherLimitDrizzle)
        dry_weather = true;
      break;
    case DRIZZLE_SLEET_FORM:
    case DRIZZLE_SLEET_FREEZING_FORM:
      if (thePrecipitationIntensity < theParameters.theDryWeatherLimitDrizzle &&
          thePrecipitationIntensity < theParameters.theDryWeatherLimitSleet)
        dry_weather = true;
      break;
    case SLEET_SNOW_FREEZING_FORM:
    case SLEET_SNOW_FORM:
      if (thePrecipitationIntensity < theParameters.theDryWeatherLimitSleet)
        dry_weather = true;
      break;
    case MISSING_PRECIPITATION_FORM:
      dry_weather = true;
      break;
  }

  return dry_weather;
}

}  // namespace

InPlacesPhrase::InPlacesPhrase() = default;

Sentence InPlacesPhrase::getInPlacesPhrase(PhraseType thePhraseType, bool /*useOllaVerbFlag*/)
{
  Sentence sentence;

  if (thePreventTautologyFlag && thePreviousPhrase == thePhraseType)
  {
    sentence << EMPTY_STRING;
    return sentence;
  }

  thePreventTautologyFlag = false;

  if (thePhraseType != NONEXISTENT_PHRASE)
  {
    /*
    if(useOllaVerbFlag)
      {
            stringVector.push_back(SAADAAN_WORD);
      }
    */
    thePreviousPhrase = thePhraseType;
    sentence << (thePhraseType == IN_SOME_PLACES_PHRASE ? PAIKOIN_WORD : MONIN_PAIKOIN_WORD);
  }
  else
  {
    sentence << EMPTY_STRING;
  }

  return sentence;
}

void PrecipitationForecast::waterAndSnowShowersPhrase(
    float thePrecipitationIntensity,
    float /*thePrecipitationIntensityAbsoluteMax*/,
    float theWaterDrizzleSleetShare,
    bool theCanBeFreezingFlag,
    map<string, Sentence>& theCompositePhraseElements) const
{
  // when showers, dont use sleet, use water and snow instead (Sari Hartonen)
  // and if there is no heavy rain
  /*
        // Annakaisa 8.5.2017: Olisiko mahdollista poistaa kuuro-tyypin kanssa sana heikko? Kuurot
  ovat usein pienialaisia tai niitä on vähän, mutta jos kohdalle sattuu, niin kastuu varmasti.
  if (thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow &&
      thePrecipitationIntensityAbsoluteMax < theParameters.theHeavyPrecipitationLimitSnow)
  {
    theCompositePhraseElements[INTENSITY_PARAMETER] << HEIKKOJA_WORD;
  }
  else
  */
  if (thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
  {
    theCompositePhraseElements[INTENSITY_PARAMETER] << RUNSAITA_WORD;
  }
  else
  {
    theCheckHeavyIntensityFlag = SHOWERS;
  }

  if (theCanBeFreezingFlag)
  {
    can_be_freezing_phrase(
        theUseIcingPhraseFlag, theCanBeFreezingFlag, theCompositePhraseElements, true);

    theCompositePhraseElements[PRECIPITATION_PARAMETER] << LUMI_TAI_VESIKUUROJA_PHRASE;
  }
  else
  {
    if (theWaterDrizzleSleetShare > MAJORITY_LIMIT)
    {
      theCompositePhraseElements[PRECIPITATION_PARAMETER] << VESI_TAI_LUMIKUUROJA_PHRASE;
    }
    else
    {
      theCompositePhraseElements[PRECIPITATION_PARAMETER] << LUMI_TAI_VESIKUUROJA_PHRASE;
    }
  }
}

void PrecipitationForecast::mostlyDryWeatherPhrase(
    bool /*theIsShowersFlag*/,
    const WeatherPeriod& /*thePeriod*/,
    const char* thePhrase,
    map<string, Sentence>& theCompositePhraseElements) const
{
  if (!theDryPeriodTautologyFlag)
  {
    theCompositePhraseElements[HUOMENNA_SISAMAASSA_SAA_ON_ENIMMAKSEEN_POUTAINEN_COMPOSITE_PHRASE]
        << SAA_WORD;
  }
  else
  {
    if (strcmp(thePhrase, YKSITTAISET_SADEKUUROT_MAHDOLLISIA_PHRASE) == 0)
      theCompositePhraseElements[HUOMENNA_YKSITTAISET_SADEKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE]
          << SAA_WORD;
    else if (strcmp(thePhrase, YKSITTAISET_VESIKUUROT_MAHDOLLISIA_PHRASE) == 0)
      theCompositePhraseElements[HUOMENNA_YKSITTAISET_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE]
          << SAA_WORD;
    else if (strcmp(thePhrase, YKSITTAISET_RANTAKUUROT_MAHDOLLISIA_PHRASE) == 0)
      theCompositePhraseElements[HUOMENNA_YKSITTAISET_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE]
          << SAA_WORD;
    else if (strcmp(thePhrase, YKSITTAISET_LUMIKUUROT_MAHDOLLISIA_PHRASE) == 0)
      theCompositePhraseElements[HUOMENNA_YKSITTAISET_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE]
          << SAA_WORD;
    else if (strcmp(thePhrase, YKSITTAISET_VESI_RANTA_KUUROT_MAHDOLLISIA_PHRASE) == 0)
      theCompositePhraseElements
              [HUOMENNA_YKSITTAISET_VESI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE]
          << SAA_WORD;
    else if (strcmp(thePhrase, YKSITTAISET_RANTA_VESI_KUUROT_MAHDOLLISIA_PHRASE) == 0)
      theCompositePhraseElements
              [HUOMENNA_YKSITTAISET_RANTA_TAI_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE]
          << SAA_WORD;
    else if (strcmp(thePhrase, YKSITTAISET_LUMI_RANTA_KUUROT_MAHDOLLISIA_PHRASE) == 0)
      theCompositePhraseElements
              [HUOMENNA_YKSITTAISET_LUMI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE]
          << SAA_WORD;
    else if (strcmp(thePhrase, YKSITTAISET_VESI_LUMI_KUUROT_MAHDOLLISIA_PHRASE) == 0)
      theCompositePhraseElements
              [HUOMENNA_YKSITTAISET_VESI_TAI_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE]
          << SAA_WORD;
    else
      theCompositePhraseElements[PAIKOIN_HEIKKOA_SADETTA_COMPOSITE_PHRASE] << SAA_WORD;
  }

  theCompositePhraseElements[PRECIPITATION_PARAMETER] << thePhrase;

  theDryPeriodTautologyFlag = true;
}

namespace
{
struct TransformPhrases
{
  const char* startWord;
  const char* jokaMuuttuu;
};

// Returns phrase pair for a given transformation, or nullptr if special case / unknown
const TransformPhrases* getTransformPhrases(precipitation_form_transformation_id id)
{
  static const std::map<precipitation_form_transformation_id, TransformPhrases> table = {
      {WATER_TO_SNOW, {VESISADETTA_WORD, JOKA_MUUTTUU_LUMISATEEKSI_PHRASE}},
      {WATER_TO_DRIZZLE, {VESISADETTA_WORD, JOKA_MUUTTUU_TIHKUSATEEKSI_PHRASE}},
      {WATER_TO_SLEET, {VESISADETTA_WORD, JOKA_MUUTTUU_RANTASATEEKSI_PHRASE}},
      {SNOW_TO_WATER, {LUMISADETTA_WORD, JOKA_MUUTTUU_VESISATEEKSI_PHRASE}},
      {SNOW_TO_DRIZZLE, {LUMISADETTA_WORD, JOKA_MUUTTUU_TIHKUSATEEKSI_PHRASE}},
      {SNOW_TO_SLEET, {LUMISADETTA_WORD, JOKA_MUUTTUU_RANTASATEEKSI_PHRASE}},
      {DRIZZLE_TO_WATER, {TIHKUSADETTA_WORD, JOKA_MUUTTUU_VESISATEEKSI_PHRASE}},
      {DRIZZLE_TO_SNOW, {TIHKUSADETTA_WORD, JOKA_MUUTTUU_LUMISATEEKSI_PHRASE}},
      {DRIZZLE_TO_SLEET, {TIHKUSADETTA_WORD, JOKA_MUUTTUU_RANTASATEEKSI_PHRASE}},
      {SLEET_TO_WATER, {RANTASADETTA_WORD, JOKA_MUUTTUU_VESISATEEKSI_PHRASE}},
      {SLEET_TO_DRIZZLE, {RANTASADETTA_WORD, JOKA_MUUTTUU_TIHKUSATEEKSI_PHRASE}},
      {SLEET_TO_SNOW, {RANTASADETTA_WORD, JOKA_MUUTTUU_LUMISATEEKSI_PHRASE}},
  };
  auto it = table.find(id);
  return (it != table.end()) ? &it->second : nullptr;
}
}  // anonymous namespace

void PrecipitationForecast::getTransformationPhraseElements(
    const WeatherPeriod& /*thePeriod*/,
    float thePrecipitationExtent,
    precipitation_type thePrecipitationType,
    precipitation_form_transformation_id theTransformId,
    map<string, Sentence>& theCompositePhraseElements) const
{
  const bool in_some_places = thePrecipitationExtent > theParameters.theInSomePlacesLowerLimit &&
                              thePrecipitationExtent <= theParameters.theInSomePlacesUpperLimit;
  const bool in_many_places = thePrecipitationExtent > theParameters.theInManyPlacesLowerLimit &&
                              thePrecipitationExtent <= theParameters.theInManyPlacesUpperLimit;

  // check if intensity is heavy
  theCheckHeavyIntensityFlag = CONTINUOUS;

  InPlacesPhrase& inPlacesPhraseMaker = get_in_places_phrase();
  InPlacesPhrase::PhraseType phraseType(InPlacesPhrase::NONEXISTENT_PHRASE);
  if (in_some_places)
    phraseType = InPlacesPhrase::IN_SOME_PLACES_PHRASE;
  else if (in_many_places)
    phraseType = InPlacesPhrase::IN_MANY_PLACES_PHRASE;

  theCompositePhraseElements[IN_PLACES_PARAMETER]
      << inPlacesPhraseMaker.getInPlacesPhrase(phraseType, theUseOllaVerbFlag);

  // Special shower cases: snow/sleet transitioning to sleet/snow showers
  if (theTransformId == SNOW_TO_SLEET && thePrecipitationType == SHOWERS)
  {
    theCompositePhraseElements[PLAIN_PRECIPITATION_PHRASE]
        << LUMIKUUROJA_WORD << Delimiter(COMMA_PUNCTUATION_MARK)
        << JOTKA_MUUTTUVAT_RANTAKUUROIKSI_PHRASE;
    return;
  }
  if (theTransformId == SLEET_TO_SNOW && thePrecipitationType == SHOWERS)
  {
    theCompositePhraseElements[PLAIN_PRECIPITATION_PHRASE]
        << RANTAKUUROJA_WORD << Delimiter(COMMA_PUNCTUATION_MARK)
        << JOTKA_MUUTTUVAT_LUMIKUUROIKSI_PHRASE;
    return;
  }

  const TransformPhrases* tp = getTransformPhrases(theTransformId);
  if (tp)
  {
    theCompositePhraseElements[PLAIN_PRECIPITATION_PHRASE]
        << tp->startWord << Delimiter(COMMA_PUNCTUATION_MARK) << tp->jokaMuuttuu;
  }
}

void PrecipitationForecast::setPhraseElementsDry(map<string, Sentence>& elems) const
{
  elems[SAA_ON_POUTAINEN_PHRASE] << SAA_ON_POUTAINEN_PHRASE;
  theDryPeriodTautologyFlag = true;
}

void PrecipitationForecast::handleWaterFormShowersPhrase(const PrecipPhraseContext& ctx,
                                                          map<string, Sentence>& elems) const
{
  if (ctx.intensity >= theParameters.theHeavyPrecipitationLimitWater)
    elems[PRECIPITATION_PARAMETER]
        << (ctx.use_summer_phrase ? VOIMAKKAITA_SADEKUUROJA_PHRASE : VOIMAKKAITA_VESIKUUROJA_PHRASE);
  else
  {
    elems[PRECIPITATION_PARAMETER]
        << (ctx.use_summer_phrase ? SADEKUUROJA_WORD : VESIKUUROJA_WORD);
    theCheckHeavyIntensityFlag = SHOWERS;
  }
  can_be_freezing_phrase(theUseIcingPhraseFlag, ctx.can_be_freezing, elems, true);
}

void PrecipitationForecast::handleWaterFormContinuousPhrase(const PrecipPhraseContext& ctx,
                                                             map<string, Sentence>& elems) const
{
  if (ctx.intensity < theParameters.theWeakPrecipitationLimitWater &&
      ctx.intensityAbsoluteMax < theParameters.theHeavyPrecipitationLimitWater)
    elems[PRECIPITATION_PARAMETER]
        << (ctx.use_summer_phrase ? HEIKKOA_SADETTA_PHRASE : HEIKKOA_VESISADETTA_PHRASE);
  else if (ctx.intensity >= theParameters.theHeavyPrecipitationLimitWater)
    elems[PRECIPITATION_PARAMETER]
        << (ctx.use_summer_phrase ? RUNSASTA_SADETTA_PHRASE : RUNSASTA_VESISADETTA_PHRASE);
  else
  {
    elems[PRECIPITATION_PARAMETER] << (ctx.use_summer_phrase ? SADETTA_WORD : VESISADETTA_WORD);
    theCheckHeavyIntensityFlag = CONTINUOUS;
  }
  can_be_freezing_phrase(theUseIcingPhraseFlag, ctx.can_be_freezing, elems, false);
}

void PrecipitationForecast::handleWaterFormPhrase(const PrecipPhraseContext& ctx,
                                                  map<string, Sentence>& elems) const
{
  if (ctx.intensity < theParameters.theDryWeatherLimitWater)
  {
    setPhraseElementsDry(elems);
    return;
  }
  if (ctx.mostly_dry_weather && ctx.is_showers)
  {
    mostlyDryWeatherPhrase(ctx.is_showers,
                           ctx.period,
                           (ctx.use_summer_phrase ? YKSITTAISET_SADEKUUROT_MAHDOLLISIA_PHRASE
                                                  : YKSITTAISET_VESIKUUROT_MAHDOLLISIA_PHRASE),
                           elems);
    return;
  }
  elems[IN_PLACES_PARAMETER]
      << get_in_places_phrase().getInPlacesPhrase(ctx.phraseType, theUseOllaVerbFlag);
  if (ctx.is_showers)
    handleWaterFormShowersPhrase(ctx, elems);
  else
    handleWaterFormContinuousPhrase(ctx, elems);
  theDryPeriodTautologyFlag = false;
}

void PrecipitationForecast::handleSleetFormPhrase(const PrecipPhraseContext& ctx,
                                                  map<string, Sentence>& elems) const
{
  if (ctx.intensity < theParameters.theDryWeatherLimitSleet)
  {
    setPhraseElementsDry(elems);
    return;
  }
  if (ctx.mostly_dry_weather)
  {
    mostlyDryWeatherPhrase(
        ctx.is_showers, ctx.period, YKSITTAISET_RANTAKUUROT_MAHDOLLISIA_PHRASE, elems);
    return;
  }
  elems[IN_PLACES_PARAMETER]
      << get_in_places_phrase().getInPlacesPhrase(ctx.phraseType, theUseOllaVerbFlag);
  if (ctx.is_showers)
  {
    elems[PRECIPITATION_PARAMETER] << RANTAKUUROJA_WORD;
    theCheckHeavyIntensityFlag = SHOWERS;
  }
  else
  {
    elems[PRECIPITATION_PARAMETER] << RANTASADETTA_WORD;
    theCheckHeavyIntensityFlag = CONTINUOUS;
    if (ctx.can_be_freezing)
      elems[PRECIPITATION_PARAMETER]
          << JA_WORD
          << (ctx.freezing_form == WATER_FREEZING_FORM ? JAATAVAA_VESISADETTA_PHRASE
                                                       : JAATAVAA_TIHKUSADETTA_PHRASE);
  }
  theDryPeriodTautologyFlag = false;
}

void PrecipitationForecast::handleSnowFormPhrase(const PrecipPhraseContext& ctx,
                                                 map<string, Sentence>& elems) const
{
  if (ctx.intensity < theParameters.theDryWeatherLimitSnow)
  {
    setPhraseElementsDry(elems);
    return;
  }
  if (ctx.mostly_dry_weather)
  {
    mostlyDryWeatherPhrase(
        ctx.is_showers, ctx.period, YKSITTAISET_LUMIKUUROT_MAHDOLLISIA_PHRASE, elems);
    return;
  }
  elems[IN_PLACES_PARAMETER]
      << get_in_places_phrase().getInPlacesPhrase(ctx.phraseType, theUseOllaVerbFlag);
  if (ctx.is_showers)
  {
    if (ctx.intensity >= theParameters.theHeavyPrecipitationLimitSnow)
      elems[INTENSITY_PARAMETER] << SAKEITA_WORD;
    else
      theCheckHeavyIntensityFlag = SHOWERS;
    elems[PRECIPITATION_PARAMETER] << LUMIKUUROJA_WORD;
  }
  else
  {
    if (ctx.intensity < theParameters.theWeakPrecipitationLimitSnow &&
        ctx.intensityAbsoluteMax < theParameters.theHeavyPrecipitationLimitSnow)
      elems[INTENSITY_PARAMETER] << HEIKKOA_WORD;
    else if (ctx.intensity >= theParameters.theHeavyPrecipitationLimitSnow)
      elems[INTENSITY_PARAMETER] << SAKEAA_WORD;
    else
      theCheckHeavyIntensityFlag = CONTINUOUS;
    elems[PRECIPITATION_PARAMETER] << LUMISADETTA_WORD;
    if (ctx.can_be_freezing)
      elems[PRECIPITATION_PARAMETER]
          << JA_WORD
          << (ctx.freezing_form == WATER_FREEZING_FORM ? JAATAVAA_VESISADETTA_PHRASE
                                                       : JAATAVAA_TIHKUSADETTA_PHRASE);
  }
  theDryPeriodTautologyFlag = false;
}

void PrecipitationForecast::handleDrizzleFormPhrase(const PrecipPhraseContext& ctx,
                                                    map<string, Sentence>& elems) const
{
  if (ctx.intensity < theParameters.theDryWeatherLimitDrizzle)
  {
    setPhraseElementsDry(elems);
    return;
  }
  if (ctx.mostly_dry_weather)
  {
    mostlyDryWeatherPhrase(ctx.is_showers,
                           ctx.period,
                           (ctx.use_summer_phrase ? YKSITTAISET_SADEKUUROT_MAHDOLLISIA_PHRASE
                                                  : YKSITTAISET_VESIKUUROT_MAHDOLLISIA_PHRASE),
                           elems);
    return;
  }
  elems[IN_PLACES_PARAMETER]
      << get_in_places_phrase().getInPlacesPhrase(ctx.phraseType, theUseOllaVerbFlag);
  if (ctx.is_showers)
  {
    elems[PRECIPITATION_PARAMETER]
        << (ctx.use_summer_phrase ? SADEKUUROJA_WORD : VESIKUUROJA_WORD);
    theCheckHeavyIntensityFlag = SHOWERS;
    can_be_freezing_phrase(theUseIcingPhraseFlag, ctx.can_be_freezing, elems, true);
  }
  else
  {
    elems[PRECIPITATION_PARAMETER] << TIHKUSADETTA_WORD;
    can_be_freezing_phrase(theUseIcingPhraseFlag, ctx.can_be_freezing, elems, false);
  }
  theDryPeriodTautologyFlag = false;
}

void PrecipitationForecast::handleWaterDrizzleFormPhrase(const PrecipPhraseContext& ctx,
                                                         map<string, Sentence>& elems) const
{
  if (ctx.intensity < theParameters.theDryWeatherLimitDrizzle)
  {
    setPhraseElementsDry(elems);
    return;
  }
  if (ctx.mostly_dry_weather)
  {
    mostlyDryWeatherPhrase(ctx.is_showers,
                           ctx.period,
                           (ctx.use_summer_phrase ? YKSITTAISET_SADEKUUROT_MAHDOLLISIA_PHRASE
                                                  : YKSITTAISET_VESIKUUROT_MAHDOLLISIA_PHRASE),
                           elems);
    return;
  }
  elems[IN_PLACES_PARAMETER]
      << get_in_places_phrase().getInPlacesPhrase(ctx.phraseType, theUseOllaVerbFlag);
  if (ctx.is_showers)
  {
    elems[PRECIPITATION_PARAMETER]
        << (ctx.use_summer_phrase ? SADEKUUROJA_WORD : VESIKUUROJA_WORD);
    theCheckHeavyIntensityFlag = SHOWERS;
    can_be_freezing_phrase(theUseIcingPhraseFlag, ctx.can_be_freezing, elems, true);
  }
  else
  {
    if (ctx.formDrizzle < 70.0)
      elems[PRECIPITATION_PARAMETER] << (ctx.use_summer_phrase ? SADETTA_WORD : VESISADETTA_WORD);
    else
      elems[PRECIPITATION_PARAMETER] << TIHKUSADETTA_WORD;
    theCheckHeavyIntensityFlag = CONTINUOUS;
    can_be_freezing_phrase(theUseIcingPhraseFlag, ctx.can_be_freezing, elems, false);
  }
  theDryPeriodTautologyFlag = false;
}

void PrecipitationForecast::handleWaterSleetFormPhrase(const PrecipPhraseContext& ctx,
                                                       map<string, Sentence>& elems) const
{
  if (ctx.intensity < theParameters.theDryWeatherLimitDrizzle)
  {
    setPhraseElementsDry(elems);
    return;
  }
  if (ctx.mostly_dry_weather)
  {
    mostlyDryWeatherPhrase(ctx.is_showers,
                           ctx.period,
                           (ctx.formWater >= ctx.formSleet
                                ? YKSITTAISET_VESI_RANTA_KUUROT_MAHDOLLISIA_PHRASE
                                : YKSITTAISET_RANTA_VESI_KUUROT_MAHDOLLISIA_PHRASE),
                           elems);
    return;
  }
  elems[IN_PLACES_PARAMETER]
      << get_in_places_phrase().getInPlacesPhrase(ctx.phraseType, theUseOllaVerbFlag);
  if (ctx.is_showers)
  {
    if (ctx.formWater >= ctx.formSleet && !ctx.can_be_freezing)
      elems[PRECIPITATION_PARAMETER] << VESI_TAI_RANTAKUUROJA_PHRASE;
    else
      elems[PRECIPITATION_PARAMETER] << RANTA_TAI_VESIKUUROJA_PHRASE;
    theCheckHeavyIntensityFlag = SHOWERS;
    can_be_freezing_phrase(theUseIcingPhraseFlag, ctx.can_be_freezing, elems, true);
  }
  else
  {
    if (ctx.formWater >= ctx.formSleet)
      elems[PRECIPITATION_PARAMETER] << VESI_TAI_RANTASADETTA_PHRASE;
    else
      elems[PRECIPITATION_PARAMETER] << RANTA_TAI_VESISADETTA_PHRASE;
    theCheckHeavyIntensityFlag = CONTINUOUS;
    can_be_freezing_phrase(theUseIcingPhraseFlag, ctx.can_be_freezing, elems, false);
  }
  theDryPeriodTautologyFlag = false;
}

void PrecipitationForecast::handleWaterSnowFormPhrase(const PrecipPhraseContext& ctx,
                                                      map<string, Sentence>& elems) const
{
  if (ctx.intensity < theParameters.theDryWeatherLimitDrizzle)
  {
    setPhraseElementsDry(elems);
    return;
  }
  if (ctx.mostly_dry_weather)
  {
    mostlyDryWeatherPhrase(
        ctx.is_showers, ctx.period, YKSITTAISET_VESI_LUMI_KUUROT_MAHDOLLISIA_PHRASE, elems);
    return;
  }
  elems[IN_PLACES_PARAMETER]
      << get_in_places_phrase().getInPlacesPhrase(ctx.phraseType, theUseOllaVerbFlag);
  if (ctx.is_showers)
  {
    waterAndSnowShowersPhrase(ctx.intensity,
                              ctx.intensityAbsoluteMax,
                              ctx.formWater + ctx.formDrizzle + ctx.formSleet,
                              ctx.can_be_freezing,
                              elems);
  }
  else
  {
    // IF vesi on suunnilleen yhta paljon kuin lumi AND ranta < vesi+lumi, THEN vesi- tai lumisadetta
    if (ctx.intensity < theParameters.theWeakPrecipitationLimitSnow &&
        ctx.intensityAbsoluteMax < theParameters.theHeavyPrecipitationLimitSnow)
      elems[INTENSITY_PARAMETER] << HEIKKOA_WORD;
    else if (ctx.intensity >= theParameters.theHeavyPrecipitationLimitSnow)
      elems[INTENSITY_PARAMETER] << KOVAA_WORD;
    else
      theCheckHeavyIntensityFlag = CONTINUOUS;

    if (ctx.formWater >= ctx.formSnow)
    {
      elems[PRECIPITATION_PARAMETER] << VESI_TAI_LUMISADETTA_PHRASE;
    }
    else if (ctx.can_be_freezing)
    {
      elems[PRECIPITATION_PARAMETER]
          << (ctx.freezing_form == WATER_FREEZING_FORM ? LUMI_TAI_VESISADETTA_PHRASE
                                                       : LUMI_TAI_TIHKUSADETTA_PHRASE);
      can_be_freezing_phrase(theUseIcingPhraseFlag, ctx.can_be_freezing, elems, false);
    }
    else
    {
      elems[PRECIPITATION_PARAMETER] << LUMI_TAI_VESISADETTA_PHRASE;
    }
  }
  theDryPeriodTautologyFlag = false;
}

void PrecipitationForecast::handleSleetSnowFormPhrase(const PrecipPhraseContext& ctx,
                                                      map<string, Sentence>& elems) const
{
  if (ctx.intensity < theParameters.theDryWeatherLimitSleet)
  {
    setPhraseElementsDry(elems);
    return;
  }
  if (ctx.mostly_dry_weather)
  {
    mostlyDryWeatherPhrase(
        ctx.is_showers, ctx.period, YKSITTAISET_LUMI_RANTA_KUUROT_MAHDOLLISIA_PHRASE, elems);
    return;
  }
  elems[IN_PLACES_PARAMETER]
      << get_in_places_phrase().getInPlacesPhrase(ctx.phraseType, theUseOllaVerbFlag);
  if (ctx.is_showers)
  {
    waterAndSnowShowersPhrase(ctx.intensity,
                              ctx.intensityAbsoluteMax,
                              ctx.formWater + ctx.formDrizzle + ctx.formSleet,
                              ctx.can_be_freezing,
                              elems);
  }
  else
  {
    if (ctx.intensity < theParameters.theWeakPrecipitationLimitSnow &&
        ctx.intensityAbsoluteMax < theParameters.theHeavyPrecipitationLimitSnow)
      elems[INTENSITY_PARAMETER] << HEIKKOA_WORD;
    else if (ctx.intensity >= theParameters.theHeavyPrecipitationLimitSnow)
      elems[INTENSITY_PARAMETER] << SAKEAA_WORD;
    else
      theCheckHeavyIntensityFlag = CONTINUOUS;

    if (ctx.formSnow >= ctx.formSleet)
    {
      elems[PRECIPITATION_PARAMETER] << LUMI_TAI_RANTASADETTA_PHRASE;
      can_be_freezing_phrase(theUseIcingPhraseFlag, ctx.can_be_freezing, elems, false);
    }
    else
    {
      elems[PRECIPITATION_PARAMETER] << RANTA_TAI_LUMISADETTA_PHRASE;
    }
  }
  theDryPeriodTautologyFlag = false;
}

void PrecipitationForecast::getPrecipitationPhraseElements(
    const WeatherPeriod& thePeriod,
    precipitation_form_id thePrecipitationForm,
    float thePrecipitationIntensity,
    float thePrecipitationIntensityAbsoluteMax,
    float thePrecipitationExtent,
    float thePrecipitationFormWater,
    float thePrecipitationFormDrizzle,
    float thePrecipitationFormSleet,
    float thePrecipitationFormSnow,
    float thePrecipitationFormFreezingRain,
    float thePrecipitationFormFreezingDrizzle,
    precipitation_type thePrecipitationType,
    const TextGenPosixTime& /*theTypeChangeTime*/,
    map<string, Sentence>& theCompositePhraseElements) const
{
  theCheckHeavyIntensityFlag = MISSING_PRECIPITATION_TYPE;

  bool dry_weather = is_dry_weather(
      theParameters, thePrecipitationForm, thePrecipitationIntensity, thePrecipitationExtent);

  if (dry_weather)
  {
    theCompositePhraseElements[SAA_ON_POUTAINEN_PHRASE] << SAA_ON_POUTAINEN_PHRASE;
    theDryPeriodTautologyFlag = true;
    return;
  }

  if (thePrecipitationForm == MISSING_PRECIPITATION_FORM)
    return;

  // use the summer phrase if it is summertime and no more than one precipitation form is involved
  const bool use_summer_phrase =
      SeasonTools::isSummerHalf(thePeriod.localStartTime(), theParameters.theVariable) &&
      theSinglePrecipitationFormFlag;

  const bool is_showers = thePrecipitationType == SHOWERS;
  const bool mostly_dry_weather = thePrecipitationExtent <= theParameters.theMostlyDryWeatherLimit;
  const bool in_some_places = thePrecipitationExtent > theParameters.theInSomePlacesLowerLimit &&
                              thePrecipitationExtent <= theParameters.theInSomePlacesUpperLimit;
  const bool in_many_places = thePrecipitationExtent > theParameters.theInManyPlacesLowerLimit &&
                              thePrecipitationExtent <= theParameters.theInManyPlacesUpperLimit;

  InPlacesPhrase::PhraseType phraseType(InPlacesPhrase::NONEXISTENT_PHRASE);
  if (in_some_places)
    phraseType = InPlacesPhrase::IN_SOME_PLACES_PHRASE;
  else if (in_many_places)
    phraseType = InPlacesPhrase::IN_MANY_PLACES_PHRASE;

  const bool can_be_freezing =
      thePrecipitationFormFreezingRain > theParameters.theFreezingPrecipitationLimit ||
      thePrecipitationFormFreezingDrizzle > theParameters.theFreezingPrecipitationLimit;

  precipitation_form_id freezing_form = MISSING_PRECIPITATION_FORM;
  if (thePrecipitationFormFreezingRain > theParameters.theFreezingPrecipitationLimit &&
      thePrecipitationFormFreezingDrizzle > theParameters.theFreezingPrecipitationLimit)
    freezing_form = (thePrecipitationFormFreezingRain > thePrecipitationFormFreezingDrizzle
                         ? WATER_FREEZING_FORM
                         : DRIZZLE_FREEZING_FORM);
  else if (thePrecipitationFormFreezingRain > theParameters.theFreezingPrecipitationLimit)
    freezing_form = WATER_FREEZING_FORM;
  else if (thePrecipitationFormFreezingDrizzle > theParameters.theFreezingPrecipitationLimit)
    freezing_form = DRIZZLE_FREEZING_FORM;

  theParameters.theLog << "Precipitation form is "
                       << precipitation_form_string(thePrecipitationForm) << '\n';
  theParameters.theLog << "Precipitation extent is " << thePrecipitationExtent << '\n';
  theParameters.theLog << "Precipitation type is "
                       << (thePrecipitationType == 1
                               ? "'continuous'"
                               : (thePrecipitationType == 2 ? "'showers'" : "'missing'"))
                       << '\n';

  if (!(thePrecipitationIntensity >= theParameters.theDryWeatherLimitSnow && mostly_dry_weather))
    theCompositePhraseElements[PAIKOIN_HEIKKOA_SADETTA_COMPOSITE_PHRASE] << SAA_WORD;

  PrecipPhraseContext ctx{thePeriod,
                          thePrecipitationIntensity,
                          thePrecipitationIntensityAbsoluteMax,
                          thePrecipitationExtent,
                          thePrecipitationFormWater,
                          thePrecipitationFormDrizzle,
                          thePrecipitationFormSleet,
                          thePrecipitationFormSnow,
                          thePrecipitationFormFreezingRain,
                          thePrecipitationFormFreezingDrizzle,
                          use_summer_phrase,
                          is_showers,
                          mostly_dry_weather,
                          can_be_freezing,
                          freezing_form,
                          phraseType};

  switch (thePrecipitationForm)
  {
    case WATER_FREEZING_FORM:
    case FREEZING_FORM:
    case WATER_FORM:
      handleWaterFormPhrase(ctx, theCompositePhraseElements);
      break;
    case SLEET_FREEZING_FORM:
    case SLEET_FORM:
      handleSleetFormPhrase(ctx, theCompositePhraseElements);
      break;
    case SNOW_FORM:
    case SNOW_FREEZING_FORM:
      handleSnowFormPhrase(ctx, theCompositePhraseElements);
      break;
    case DRIZZLE_FORM:
    case DRIZZLE_FREEZING_FORM:
      handleDrizzleFormPhrase(ctx, theCompositePhraseElements);
      break;
    case WATER_DRIZZLE_FREEZING_FORM:
    case WATER_DRIZZLE_FORM:
      handleWaterDrizzleFormPhrase(ctx, theCompositePhraseElements);
      break;
    case DRIZZLE_SLEET_FORM:
    case DRIZZLE_SLEET_FREEZING_FORM:
    case WATER_DRIZZLE_SLEET_FORM:
    case WATER_SLEET_FREEZING_FORM:
    case WATER_SLEET_FORM:
      handleWaterSleetFormPhrase(ctx, theCompositePhraseElements);
      break;
    case WATER_SNOW_FORM:
    case DRIZZLE_SNOW_FORM:
    case WATER_DRIZZLE_SNOW_FORM:
    case DRIZZLE_SLEET_SNOW_FORM:
    case WATER_SLEET_SNOW_FORM:
    case DRIZZLE_SNOW_FREEZING_FORM:
    case WATER_SNOW_FREEZING_FORM:
      handleWaterSnowFormPhrase(ctx, theCompositePhraseElements);
      break;
    case SLEET_SNOW_FREEZING_FORM:
    case SLEET_SNOW_FORM:
      handleSleetSnowFormPhrase(ctx, theCompositePhraseElements);
      break;
    default:
      break;
  }
}

void PrecipitationForecast::selectPrecipitationSentence(
    const WeatherPeriod& thePeriod,
    precipitation_form_id thePrecipitationForm,
    float thePrecipitationIntensity,
    float thePrecipitationAbsoluteMax,
    float thePrecipitationExtent,
    float thePrecipitationFormWater,
    float thePrecipitationFormDrizzle,
    float thePrecipitationFormSleet,
    float thePrecipitationFormSnow,
    float thePrecipitationFormFreezingRain,
    float thePrecipitationFormFreezingDrizzle,
    precipitation_type thePrecipitationType,
    const TextGenPosixTime& theTypeChangeTime,
    precipitation_form_transformation_id theTransformationId,
    map<string, Sentence>& theCompositePhraseElements) const
{
  if (theTransformationId == NO_FORM_TRANSFORMATION)
  {
    getPrecipitationPhraseElements(thePeriod,
                                   thePrecipitationForm,
                                   thePrecipitationIntensity,
                                   thePrecipitationAbsoluteMax,
                                   thePrecipitationExtent,
                                   thePrecipitationFormWater,
                                   thePrecipitationFormDrizzle,
                                   thePrecipitationFormSleet,
                                   thePrecipitationFormSnow,
                                   thePrecipitationFormFreezingRain,
                                   thePrecipitationFormFreezingDrizzle,
                                   thePrecipitationType,
                                   theTypeChangeTime,
                                   theCompositePhraseElements);
  }
  else
  {
    getTransformationPhraseElements(thePeriod,
                                    thePrecipitationExtent,
                                    thePrecipitationType,
                                    theTransformationId,
                                    theCompositePhraseElements);
  }
}

namespace
{
// Helper: classify and add a dry gap between two consecutive precipitation periods
void addDryGapPeriod(const TextGenPosixTime& prevEnd,
                     const TextGenPosixTime& pStart,
                     const TextGenPosixTime& srcStart,
                     const TextGenPosixTime& srcEnd,
                     vector<WeatherPeriod>& theDryPeriods)
{
  WeatherPeriod dryPeriod(prevEnd, pStart);
  if (dryPeriod.localStartTime() < srcStart || dryPeriod.localStartTime() >= srcEnd)
    return;
  if (dryPeriod.localEndTime() > srcStart && dryPeriod.localEndTime() <= srcEnd)
    theDryPeriods.push_back(dryPeriod);
  else
    theDryPeriods.emplace_back(dryPeriod.localStartTime(), srcEnd);
}
}  // anonymous namespace

bool PrecipitationForecast::getDryPeriods(const WeatherPeriod& theSourcePeriod,
                                          vector<WeatherPeriod>& theDryPeriods) const
{
  const vector<WeatherPeriod>* precipitationPeriods = nullptr;

  if (theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
    precipitationPeriods = &thePrecipitationPeriodsFull;
  else if (theParameters.theForecastArea & INLAND_AREA)
    precipitationPeriods = &thePrecipitationPeriodsInland;
  else if (theParameters.theForecastArea & COASTAL_AREA)
    precipitationPeriods = &thePrecipitationPeriodsCoastal;

  if (!precipitationPeriods || precipitationPeriods->empty())
  {
    theDryPeriods.emplace_back(theSourcePeriod.localStartTime(), theSourcePeriod.localEndTime());
    return true;
  }

  theParameters.theLog << "START TIME: " << theSourcePeriod.localStartTime() << '\n';
  theParameters.theLog << "END TIME: " << theSourcePeriod.localEndTime() << '\n';

  const TextGenPosixTime& srcStart = theSourcePeriod.localStartTime();
  const TextGenPosixTime& srcEnd = theSourcePeriod.localEndTime();

  bool overlappingPeriods = false;

  for (unsigned i = 0; i < precipitationPeriods->size(); i++)
  {
    const TextGenPosixTime& pStart = precipitationPeriods->at(i).localStartTime();
    const TextGenPosixTime& pEnd = precipitationPeriods->at(i).localEndTime();

    theParameters.theLog << "precipitation period i start: " << pStart << '\n';
    theParameters.theLog << "precipitation period i end: " << pEnd << '\n';

    if ((pStart >= srcStart && pStart <= srcEnd) || (pEnd >= srcStart && pEnd <= srcStart))
      overlappingPeriods = true;

    if (i == 0)
    {
      if (pStart >= srcStart && pStart <= srcEnd)
        theDryPeriods.emplace_back(srcStart, pStart);
      continue;
    }

    addDryGapPeriod(
        precipitationPeriods->at(i - 1).localEndTime(), pStart, srcStart, srcEnd, theDryPeriods);
  }

  if (!overlappingPeriods)
    theDryPeriods.push_back(theSourcePeriod);

  return !theDryPeriods.empty();
}

bool PrecipitationForecast::getPrecipitationPeriods(
    const WeatherPeriod& theSourcePeriod, vector<WeatherPeriod>& thePrecipitationPeriods) const
{
  const vector<WeatherPeriod>* precipitationPeriods = nullptr;

  if (theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
    precipitationPeriods = &thePrecipitationPeriodsFull;
  else if (theParameters.theForecastArea & INLAND_AREA)
    precipitationPeriods = &thePrecipitationPeriodsInland;
  else if (theParameters.theForecastArea & COASTAL_AREA)
    precipitationPeriods = &thePrecipitationPeriodsCoastal;

  if (!precipitationPeriods || precipitationPeriods->empty())
  {
    return false;
  }

  for (const auto& precipitationPeriod : *precipitationPeriods)
  {
    WeatherPeriod notDryPeriod(precipitationPeriod.localStartTime(),
                               precipitationPeriod.localEndTime());

    if (notDryPeriod.localStartTime() >= theSourcePeriod.localStartTime() &&
        notDryPeriod.localStartTime() <= theSourcePeriod.localEndTime())
    {
      if (notDryPeriod.localEndTime() >= theSourcePeriod.localStartTime() &&
          notDryPeriod.localEndTime() <= theSourcePeriod.localEndTime())
      {
        thePrecipitationPeriods.push_back(notDryPeriod);
      }
      else
      {
        thePrecipitationPeriods.emplace_back(notDryPeriod.localStartTime(),
                                             theSourcePeriod.localEndTime());
      }
    }
    else if (notDryPeriod.localStartTime() < theSourcePeriod.localStartTime() &&
             notDryPeriod.localEndTime() > theSourcePeriod.localStartTime())
    {
      if (notDryPeriod.localEndTime() <= theSourcePeriod.localEndTime())
      {
        thePrecipitationPeriods.emplace_back(theSourcePeriod.localStartTime(),
                                             notDryPeriod.localEndTime());
      }
      else
      {
        thePrecipitationPeriods.push_back(theSourcePeriod);
      }
    }
  }

  return !thePrecipitationPeriods.empty();
}

bool PrecipitationForecast::getIntensityFormExtent(const WeatherPeriod& theWeatherPeriod,
                                                   const unsigned short theForecastArea,
                                                   float& theIntensity,
                                                   precipitation_form_id& theForm,
                                                   float& theExtent) const
{
  const precipitation_data_vector& dataVector = getPrecipitationDataVector(theForecastArea);

  precipitation_form_id precipitationForm = MISSING_PRECIPITATION_FORM;
  theIntensity = getMean(dataVector, PRECIPITATION_MEAN_DATA, theWeatherPeriod);

  for (const auto& i : dataVector)
    if (i->theObservationTime >= theWeatherPeriod.localStartTime() &&
        i->theObservationTime <= theWeatherPeriod.localEndTime() &&
        i->thePrecipitationForm != MISSING_PRECIPITATION_FORM)
    {
      if ((precipitationForm == MISSING_PRECIPITATION_FORM) ||
          (i->thePrecipitationForm < precipitationForm))
      {
        precipitationForm = i->thePrecipitationForm;
      }
    }

  theForm = precipitationForm;

  theExtent = getMean(dataVector, PRECIPITATION_EXTENT_DATA, theWeatherPeriod);

  return theForm != MISSING_PRECIPITATION_FORM;
}

bool PrecipitationForecast::isMostlyDryPeriod(const WeatherPeriod& theWeatherPeriod,
                                              const unsigned short theForecastArea) const
{
  const precipitation_data_vector& dataVector = getPrecipitationDataVector(theForecastArea);

  return (getMean(dataVector, PRECIPITATION_EXTENT_DATA, theWeatherPeriod) <=
          theParameters.theMostlyDryWeatherLimit);
}

bool PrecipitationForecast::isDryPeriod(const WeatherPeriod& theWeatherPeriod,
                                        const unsigned short theForecastArea) const
{
  float precipitationIntensity(0.0);
  precipitation_form_id precipitationForm = MISSING_PRECIPITATION_FORM;
  float precipitationExtent(0.0);
  bool dry_weather = true;

  if (getIntensityFormExtent(theWeatherPeriod,
                             theForecastArea,
                             precipitationIntensity,
                             precipitationForm,
                             precipitationExtent))
  {
    dry_weather = is_dry_weather(
        theParameters, precipitationForm, precipitationIntensity, precipitationExtent);
  }

  return dry_weather;
}

namespace
{
// Returns the value from a data item for the given data id, or kFloatMissing if not applicable.
float getDataItemValue(const PrecipitationDataItemData& item, weather_result_data_id dataId)
{
  switch (dataId)
  {
    case PRECIPITATION_MAX_DATA:
      return item.thePrecipitationMaxIntensity;
    case PRECIPITATION_MEAN_DATA:
      return item.thePrecipitationIntensity;
    case PRECIPITATION_EXTENT_DATA:
      return item.thePrecipitationExtent;
    case PRECIPITATION_FORM_WATER_DATA:
      return item.thePrecipitationFormWater;
    case PRECIPITATION_FORM_DRIZZLE_DATA:
      return item.thePrecipitationFormDrizzle;
    case PRECIPITATION_FORM_SLEET_DATA:
      return item.thePrecipitationFormSleet;
    case PRECIPITATION_FORM_SNOW_DATA:
      return item.thePrecipitationFormSnow;
    case PRECIPITATION_FORM_FREEZING_RAIN_DATA:
      return item.thePrecipitationFormFreezingRain;
    case PRECIPITATION_FORM_FREEZING_DRIZZLE_DATA:
      return item.thePrecipitationFormFreezingDrizzle;
    case PRECIPITATION_TYPE_DATA:
      return item.thePrecipitationTypeShowers;
    default:
      return kFloatMissing;
  }
}
}  // anonymous namespace

float PrecipitationForecast::getStat(const precipitation_data_vector& theData,
                                     weather_result_data_id theDataId,
                                     const WeatherPeriod& theWeatherPeriod,
                                     stat_func_id theStatFunc)
{
  float cumulativeSum = 0.0;
  float minValue = 0.0;
  float maxValue = 0.0;
  int count = 0;

  for (const auto& i : theData)
  {
    if (i->theObservationTime < theWeatherPeriod.localStartTime())
      continue;
    if (i->theObservationTime > theWeatherPeriod.localEndTime())
      break;

    float val = getDataItemValue(*i, theDataId);
    if (val == kFloatMissing)
      continue;

    if (count == 0 || minValue > val)
      minValue = val;
    if (count == 0 || maxValue < val)
      maxValue = val;
    cumulativeSum += val;
    count++;
  }

  if (theStatFunc == MIN)
    return minValue;
  if (theStatFunc == MAX)
    return maxValue;
  // MEAN
  return (count > 0) ? (cumulativeSum / count) : 0.0f;
}

float PrecipitationForecast::getMin(const precipitation_data_vector& theData,
                                    weather_result_data_id theDataId,
                                    const WeatherPeriod& theWeatherPeriod)
{
  return getStat(theData, theDataId, theWeatherPeriod, MIN);
}

float PrecipitationForecast::getMax(const precipitation_data_vector& theData,
                                    weather_result_data_id theDataId,
                                    const WeatherPeriod& theWeatherPeriod)
{
  return getStat(theData, theDataId, theWeatherPeriod, MAX);
}

float PrecipitationForecast::getMean(const precipitation_data_vector& theData,
                                     weather_result_data_id theDataId,
                                     const WeatherPeriod& theWeatherPeriod)
{
  return getStat(theData, theDataId, theWeatherPeriod, MEAN);
}

precipitation_type PrecipitationForecast::getPrecipitationType(
    const WeatherPeriod& thePeriod, const unsigned short theForecastArea) const
{
  const precipitation_data_vector& theData = getPrecipitationDataVector(theForecastArea);

  unsigned int showers_counter(0);
  unsigned int continuous_counter(0);

  for (const auto& i : theData)
  {
    if (i->theObservationTime < thePeriod.localStartTime())
      continue;
    if (i->theObservationTime > thePeriod.localEndTime())
      break;

    if (i->thePrecipitationType != MISSING_PRECIPITATION_TYPE)
    {
      if (i->thePrecipitationType == SHOWERS)
        showers_counter++;
      else
        continuous_counter++;
    }
  }

  return (continuous_counter >= showers_counter ? CONTINUOUS : SHOWERS);
}

// Logic:
namespace
{
// Returns true if type stays constant from index i to end of data
bool typeIsConstantFrom(const precipitation_data_vector& theData,
                        unsigned int i,
                        precipitation_type expectedType)
{
  for (unsigned int k = i; k < theData.size(); k++)
  {
    if (theData[k]->thePrecipitationType != expectedType)
      return false;
  }
  return true;
}
}  // anonymous namespace

// 1) save the type of the first hour
// 2) Iterate the precipitation period through hour by hour and check
// if the type changes
// 3) If the type changes (i.e. is different from the original type), check if the new type
// is the prevailing type for the rest of the period
// returns 0, if there is no change, index in the data vector otherwise
unsigned int PrecipitationForecast::getPrecipitationTypeChange(
    const precipitation_data_vector& theData, const WeatherPeriod& thePeriod) const
{
  WeatherPeriod periodToCheck(thePeriod.localStartTime(),
                              thePeriod.localEndTime().GetYear() == 2037
                                  ? theParameters.theForecastPeriod.localEndTime()
                                  : thePeriod.localEndTime());

  bool firstValue = true;
  precipitation_type originalPrecipitationType(MISSING_PRECIPITATION_TYPE);
  for (unsigned int i = 0; i < theData.size(); i++)
  {
    if (theData[i]->theObservationTime < periodToCheck.localStartTime())
      continue;
    if (theData[i]->theObservationTime > periodToCheck.localEndTime())
      break;

    if (firstValue)
    {
      originalPrecipitationType = theData[i]->thePrecipitationType;
      firstValue = false;
      continue;
    }

    if (originalPrecipitationType == theData[i]->thePrecipitationType)
      continue;

    // type changed - check if it stays constant for the rest of the period
    if (!typeIsConstantFrom(theData, i, theData[i]->thePrecipitationType))
      return 0;

    // ignore changes too close to start or end
    if (abs(theData[i]->theObservationTime.DifferenceInHours(periodToCheck.localStartTime())) < 3 ||
        abs(theData[i]->theObservationTime.DifferenceInHours(periodToCheck.localEndTime())) < 3)
      return 0;

    return i;
  }

  return 0;
}

precipitation_form_id PrecipitationForecast::getPrecipitationForm(
    const WeatherPeriod& thePeriod, const unsigned short theForecastArea) const
{
  const precipitation_data_vector& theDataVector = getPrecipitationDataVector(theForecastArea);

  float precipitationFormWater = getMean(theDataVector, PRECIPITATION_FORM_WATER_DATA, thePeriod);
  float precipitationFormDrizzle =
      getMean(theDataVector, PRECIPITATION_FORM_DRIZZLE_DATA, thePeriod);
  float precipitationFormSleet = getMean(theDataVector, PRECIPITATION_FORM_SLEET_DATA, thePeriod);
  float precipitationFormSnow = getMean(theDataVector, PRECIPITATION_FORM_SNOW_DATA, thePeriod);
  float precipitationFormFreezingRain =
      getMean(theDataVector, PRECIPITATION_FORM_FREEZING_RAIN_DATA, thePeriod);
  float precipitationFormFreezingDrizzle =
      getMean(theDataVector, PRECIPITATION_FORM_FREEZING_DRIZZLE_DATA, thePeriod);

  return get_complete_precipitation_form(theParameters.theVariable,
                                         precipitationFormWater,
                                         precipitationFormDrizzle,
                                         precipitationFormSleet,
                                         precipitationFormSnow,
                                         precipitationFormFreezingRain,
                                         precipitationFormFreezingDrizzle);
}

unsigned int PrecipitationForecast::getPrecipitationCategory(float thePrecipitation,
                                                             unsigned int theType) const
{
  unsigned int retval(DRY_WEATHER_CATEGORY);

  switch (theType)
  {
    case WATER_FORM:
    case WATER_DRIZZLE_FORM:
    case WATER_SLEET_FORM:
    case WATER_SNOW_FORM:
    case WATER_FREEZING_FORM:
    case WATER_DRIZZLE_SLEET_FORM:
    case WATER_DRIZZLE_SNOW_FORM:
    case WATER_DRIZZLE_FREEZING_FORM:
    case WATER_SLEET_SNOW_FORM:
    case WATER_SLEET_FREEZING_FORM:
    case WATER_SNOW_FREEZING_FORM:
    case DRIZZLE_FORM:
    case DRIZZLE_SLEET_FORM:
    case DRIZZLE_SLEET_SNOW_FORM:
    case DRIZZLE_SLEET_FREEZING_FORM:
    case DRIZZLE_FREEZING_FORM:
    case DRIZZLE_SNOW_FORM:
    case DRIZZLE_SNOW_FREEZING_FORM:
    case FREEZING_FORM:
    {
      if (thePrecipitation <= theParameters.theDryWeatherLimitWater)
        retval = DRY_WEATHER_CATEGORY;
      else if (thePrecipitation >= theParameters.theWeakPrecipitationLimitWater &&
               thePrecipitation < theParameters.theHeavyPrecipitationLimitWater)
        retval = MODERATE_WATER_PRECIPITATION;
      else if (thePrecipitation > theParameters.theHeavyPrecipitationLimitWater)
        retval = HEAVY_WATER_PRECIPITATION;
    }
    break;
    case SLEET_FORM:
    case SLEET_FREEZING_FORM:
    case SLEET_SNOW_FREEZING_FORM:
    {
      if (thePrecipitation <= theParameters.theDryWeatherLimitSleet)
        retval = DRY_WEATHER_CATEGORY;
      else if (thePrecipitation >= theParameters.theWeakPrecipitationLimitSleet &&
               thePrecipitation < theParameters.theHeavyPrecipitationLimitSleet)
        retval = MODERATE_WATER_PRECIPITATION;
      else if (thePrecipitation > theParameters.theHeavyPrecipitationLimitSleet)
        retval = HEAVY_WATER_PRECIPITATION;
    }
    break;
    case SNOW_FORM:
    case SNOW_FREEZING_FORM:
    {
      if (thePrecipitation <= theParameters.theDryWeatherLimitSnow)
        retval = DRY_WEATHER_CATEGORY;
      else if (thePrecipitation >= theParameters.theWeakPrecipitationLimitSnow &&
               thePrecipitation < theParameters.theHeavyPrecipitationLimitSnow)
        retval = MODERATE_WATER_PRECIPITATION;
      else if (thePrecipitation > theParameters.theHeavyPrecipitationLimitSnow)
        retval = HEAVY_WATER_PRECIPITATION;
    }
    break;
    default:
      break;
  }

  return retval;
}

// this should be more detailed
bool PrecipitationForecast::reportPrecipitationFormsSeparately(precipitation_form_id form1,
                                                               precipitation_form_id form2)
{
  if ((form1 == WATER_FORM &&
       (form2 == SLEET_FORM || form2 == SNOW_FORM || form2 == FREEZING_FORM)) ||
      (form2 == WATER_FORM &&
       (form1 == SLEET_FORM || form1 == SNOW_FORM || form1 == FREEZING_FORM)))
    return true;
  if ((form1 == SLEET_FORM &&
       (form2 == WATER_FORM || form2 == SNOW_FORM || form2 == FREEZING_FORM)) ||
      (form2 == SLEET_FORM &&
       (form1 == WATER_FORM || form1 == SNOW_FORM || form1 == FREEZING_FORM)))
    return true;
  if ((form1 == SNOW_FORM &&
       (form2 == WATER_FORM || form2 == SLEET_FORM || form2 == FREEZING_FORM)) ||
      (form2 == SNOW_FORM &&
       (form1 == WATER_FORM || form1 == SLEET_FORM || form1 == FREEZING_FORM)))
    return true;
  if ((form1 == FREEZING_FORM &&
       (form2 == WATER_FORM || form2 == SLEET_FORM || form2 == SNOW_FORM)) ||
      (form2 == FREEZING_FORM &&
       (form1 == WATER_FORM || form1 == SLEET_FORM || form1 == SNOW_FORM)))
    return true;

  return false;
}

bool PrecipitationForecast::separateCoastInlandPrecipitation(
    const WeatherPeriod& theWeatherPeriod) const
{
  if (!(theParameters.theForecastArea & INLAND_AREA &&
        theParameters.theForecastArea & COASTAL_AREA) ||
      theParameters.theCoastalAndInlandTogetherFlag)
    return false;

  float coastalPrecipitation = getMean(theCoastalData, PRECIPITATION_MEAN_DATA, theWeatherPeriod);
  float inlandPrecipitation = getMean(theInlandData, PRECIPITATION_MEAN_DATA, theWeatherPeriod);

  precipitation_form_id coastalPrecipitationForm =
      getPrecipitationForm(theWeatherPeriod, COASTAL_AREA);
  precipitation_form_id inlandPrecipitationForm =
      getPrecipitationForm(theWeatherPeriod, INLAND_AREA);

  bool precipitationFormsDiffer =
      reportPrecipitationFormsSeparately(inlandPrecipitationForm, coastalPrecipitationForm);
  bool retval;

  if (((coastalPrecipitation < 0.04 && inlandPrecipitation > 0.4) ||
       (coastalPrecipitation > 0.4 && inlandPrecipitation < 0.04)) ||
      ((inlandPrecipitation > 0.04 && coastalPrecipitation > 0.04) && precipitationFormsDiffer))
  {
    retval = true;
    theParameters.theLog << "Coastal and Inland precipitation reported separately:\n";
  }
  else
  {
    retval = false;
    theParameters.theLog << "Coastal and Inland precipitation reported together:\n";
  }

  theParameters.theLog << "Coastal: " << coastalPrecipitation << "("
                       << precipitation_form_string(coastalPrecipitationForm) << ")"
                       << "; Inland: " << inlandPrecipitation << "("
                       << precipitation_form_string(inlandPrecipitationForm) << ")\n";

  return retval;
}

void PrecipitationForecast::printOutPrecipitationVector(
    std::ostream& theOutput, const precipitation_data_vector& thePrecipitationDataVector)
{
  for (const auto& i : thePrecipitationDataVector)
  {
    theOutput << *i;
  }
}

void PrecipitationForecast::printOutPrecipitationData(std::ostream& theOutput) const
{
  theOutput << "** PRECIPITATION DATA **\n";
  theOutput << "time:**mean intensity**max "
               "intensity**extent**water**drizzle**sleet**snow**freezing rain**freezing "
               "drizzle**showers**northeast**"
               "southeast**southwest**northwest)"
            << '\n';

  if (theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
  {
    theOutput << "Full area precipitation: \n";
    printOutPrecipitationVector(theOutput, theFullData);
  }
  if (theParameters.theForecastArea & COASTAL_AREA)
  {
    theOutput << "Coastal precipitation: \n";
    printOutPrecipitationVector(theOutput, theCoastalData);
  }
  if (theParameters.theForecastArea & INLAND_AREA)
  {
    theOutput << "Inland precipitation: \n";
    printOutPrecipitationVector(theOutput, theInlandData);
  }
}

void PrecipitationForecast::gatherPrecipitationData()
{
  if (theParameters.theForecastArea & COASTAL_AREA)
    fillInPrecipitationDataVector(COASTAL_AREA);
  if (theParameters.theForecastArea & INLAND_AREA)
    fillInPrecipitationDataVector(INLAND_AREA);
  if (theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
    fillInPrecipitationDataVector(FULL_AREA);
}

void PrecipitationForecast::fillInPrecipitationDataVector(forecast_area_id theAreaId)
{
  std::shared_ptr<weather_result_data_item_vector> precipitationMaxHourly =
      (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_MAX_DATA];
  std::shared_ptr<weather_result_data_item_vector> precipitationMeanHourly =
      (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_MEAN_DATA];
  std::shared_ptr<weather_result_data_item_vector> precipitationExtentHourly =
      (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_EXTENT_DATA];
  std::shared_ptr<weather_result_data_item_vector> precipitationFormWaterHourly =
      (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_WATER_DATA];
  std::shared_ptr<weather_result_data_item_vector> precipitationFormDrizzleHourly =
      (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_DRIZZLE_DATA];
  std::shared_ptr<weather_result_data_item_vector> precipitationFormSleetHourly =
      (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_SLEET_DATA];
  std::shared_ptr<weather_result_data_item_vector> precipitationFormSnowHourly =
      (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_SNOW_DATA];
  std::shared_ptr<weather_result_data_item_vector> precipitationFormFreezingRainHourly =
      (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_FREEZING_RAIN_DATA];
  std::shared_ptr<weather_result_data_item_vector> precipitationFormFreezingDrizzleHourly =
      (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_FREEZING_DRIZZLE_DATA];
  std::shared_ptr<weather_result_data_item_vector> precipitationTypeHourly =
      (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_TYPE_DATA];
  std::shared_ptr<weather_result_data_item_vector> precipitationNorthEastShareHourly =
      (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_NORTHEAST_SHARE_DATA];
  std::shared_ptr<weather_result_data_item_vector> precipitationSouthEastShareHourly =
      (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_SOUTHEAST_SHARE_DATA];
  std::shared_ptr<weather_result_data_item_vector> precipitationSouthWestShareHourly =
      (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_SOUTHWEST_SHARE_DATA];
  std::shared_ptr<weather_result_data_item_vector> precipitationNorthWestShareHourly =
      (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_NORTHWEST_SHARE_DATA];
  std::shared_ptr<weather_result_data_item_vector> precipitationPointHourly =
      (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_POINT_DATA];

  precipitation_data_vector& dataVector =
      (theAreaId == COASTAL_AREA ? theCoastalData
                                 : (theAreaId == INLAND_AREA ? theInlandData : theFullData));

  for (unsigned int i = 0; i < precipitationMaxHourly->size(); i++)
  {
    float precipitationMaxIntesity = (*precipitationMaxHourly)[i]->theResult.value();
    float precipitationMeanIntesity = (*precipitationMeanHourly)[i]->theResult.value();
    float precipitationExtent = (*precipitationExtentHourly)[i]->theResult.value();
    float precipitationFormWater = (*precipitationFormWaterHourly)[i]->theResult.value();
    float precipitationFormDrizzle = (*precipitationFormDrizzleHourly)[i]->theResult.value();
    float precipitationFormSleet = (*precipitationFormSleetHourly)[i]->theResult.value();
    float precipitationFormSnow = (*precipitationFormSnowHourly)[i]->theResult.value();
    float precipitationFormFreezingDrizzle =
        (*precipitationFormFreezingDrizzleHourly)[i]->theResult.value();
    float precipitationFormFreezingRain =
        (*precipitationFormFreezingRainHourly)[i]->theResult.value();
    float precipitationTypeShowers = (*precipitationTypeHourly)[i]->theResult.value();

    const precipitation_form_id precipitationForm =
        get_complete_precipitation_form(theParameters.theVariable,
                                        precipitationFormWater,
                                        precipitationFormDrizzle,
                                        precipitationFormSleet,
                                        precipitationFormSnow,
                                        precipitationFormFreezingRain,
                                        precipitationFormFreezingDrizzle);

    auto item = std::make_shared<PrecipitationDataItemData>(
        theParameters,
        precipitationForm,
        (precipitationMaxIntesity + precipitationMeanIntesity) / 2.0,
        precipitationMaxIntesity,
        precipitationExtent,
        precipitationFormWater,
        precipitationFormDrizzle,
        precipitationFormSleet,
        precipitationFormSnow,
        precipitationFormFreezingRain,
        precipitationFormFreezingDrizzle,
        precipitationTypeShowers,
        MISSING_WEATHER_EVENT,
        0.0,
        (*precipitationMaxHourly)[i]->thePeriod.localStartTime());

    item->thePrecipitationPercentageNorthEast =
        (*precipitationNorthEastShareHourly)[i]->theResult.value();
    item->thePrecipitationPercentageSouthEast =
        (*precipitationSouthEastShareHourly)[i]->theResult.value();
    item->thePrecipitationPercentageSouthWest =
        (*precipitationSouthWestShareHourly)[i]->theResult.value();
    item->thePrecipitationPercentageNorthWest =
        (*precipitationNorthWestShareHourly)[i]->theResult.value();
    item->thePrecipitationPoint.X((*precipitationPointHourly)[i]->theResult.value());
    item->thePrecipitationPoint.Y((*precipitationPointHourly)[i]->theResult.error());

    dataVector.push_back(item);
  }
}

void PrecipitationForecast::findOutPrecipitationPeriods()
{
  if (theParameters.theForecastArea & COASTAL_AREA)
    findOutPrecipitationPeriods(COASTAL_AREA);
  if (theParameters.theForecastArea & INLAND_AREA)
    findOutPrecipitationPeriods(INLAND_AREA);
  if (theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
    findOutPrecipitationPeriods(FULL_AREA);
}

// join periods if there is only one hour "dry" period between
// and if type stays the same
void PrecipitationForecast::joinPrecipitationPeriods(
    vector<WeatherPeriod>& thePrecipitationPeriodVector)
{
  if (thePrecipitationPeriodVector.size() < 2)
    return;

  for (unsigned int i = thePrecipitationPeriodVector.size() - 1; i > 0; i--)
  {
    TextGenPosixTime currentPeriodStartTime(thePrecipitationPeriodVector.at(i).localStartTime());
    TextGenPosixTime previousPeriodEndTime(thePrecipitationPeriodVector.at(i - 1).localEndTime());

    if (currentPeriodStartTime.DifferenceInHours(previousPeriodEndTime) <= 2)
    {
      WeatherPeriod joinedPeriod(thePrecipitationPeriodVector.at(i - 1).localStartTime(),
                                 thePrecipitationPeriodVector.at(i).localEndTime());

      if (isDryPeriod(joinedPeriod, theParameters.theForecastArea))
        continue;

      theParameters.theLog << "Joining precipitation periods: "
                           << thePrecipitationPeriodVector.at(i - 1).localStartTime() << "..."
                           << thePrecipitationPeriodVector.at(i - 1).localEndTime() << " and "
                           << thePrecipitationPeriodVector.at(i).localStartTime() << "..."
                           << thePrecipitationPeriodVector.at(i).localEndTime();

      thePrecipitationPeriodVector.erase(thePrecipitationPeriodVector.begin() + i);
      thePrecipitationPeriodVector.erase(thePrecipitationPeriodVector.begin() + i - 1);
      thePrecipitationPeriodVector.insert(thePrecipitationPeriodVector.begin() + i - 1,
                                          joinedPeriod);
    }
  }
}

void PrecipitationForecast::appendFinalPrecipitationPeriod(
    const precipitation_data_vector& data,
    unsigned int periodStartIndex,
    vector<WeatherPeriod>& dest) const
{
  TextGenPosixTime startTime(data[periodStartIndex]->theObservationTime);
  if (periodStartIndex == 0)
    startTime.SetDate(1970, 1, 1);  // precipitation starts before forecast period
  TextGenPosixTime endTime(data[data.size() - 1]->theObservationTime);
  if (endTime == theParameters.theForecastPeriod.localEndTime())
    endTime.SetDate(2037, 1, 1);  // precipitation continues when forecast period ends

  WeatherPeriod precipitationPeriod(startTime, endTime);
  unsigned int typeChangeIndex = getPrecipitationTypeChange(data, precipitationPeriod);
  if (typeChangeIndex == 0)
  {
    dest.push_back(precipitationPeriod);
    return;
  }

  // Split one period into two because type changes
  theParameters.theLog << "Split one precipitation period into two, because type changes at ";
  theParameters.theLog << data[typeChangeIndex]->theObservationTime;
  if (data[typeChangeIndex]->thePrecipitationType == SHOWERS)
    theParameters.theLog << " from continuous to showers";
  else
    theParameters.theLog << " from showers to continuous";

  dest.emplace_back(startTime, data[typeChangeIndex - 1]->theObservationTime);
  dest.emplace_back(data[typeChangeIndex]->theObservationTime, endTime);
}

void PrecipitationForecast::findOutPrecipitationPeriods(forecast_area_id theAreaId)
{
  precipitation_data_vector* dataSourceVector = nullptr;
  vector<WeatherPeriod>* dataDestinationVector = nullptr;

  if (theAreaId & FULL_AREA)
  {
    dataSourceVector = &theFullData;
    dataDestinationVector = &thePrecipitationPeriodsFull;
  }
  else if (theAreaId & INLAND_AREA)
  {
    dataSourceVector = &theInlandData;
    dataDestinationVector = &thePrecipitationPeriodsInland;
  }
  else if (theAreaId & COASTAL_AREA)
  {
    dataSourceVector = &theCoastalData;
    dataDestinationVector = &thePrecipitationPeriodsCoastal;
  }

  if (!dataSourceVector)
    return;

  bool isDryPrevious = true;
  bool isDryCurrent = true;

  unsigned int periodStartIndex = 0;
  for (unsigned int i = 1; i < dataSourceVector->size(); i++)
  {
    isDryPrevious = is_dry_weather(theParameters,
                                   (*dataSourceVector)[i - 1]->thePrecipitationForm,
                                   (*dataSourceVector)[i - 1]->thePrecipitationIntensity,
                                   (*dataSourceVector)[i - 1]->thePrecipitationExtent);
    isDryCurrent = is_dry_weather(theParameters,
                                  (*dataSourceVector)[i]->thePrecipitationForm,
                                  (*dataSourceVector)[i]->thePrecipitationIntensity,
                                  (*dataSourceVector)[i]->thePrecipitationExtent);
    if (isDryPrevious != isDryCurrent)
    {
      if (!isDryPrevious)
      {
        TextGenPosixTime startTime((*dataSourceVector)[periodStartIndex]->theObservationTime);
        if (periodStartIndex == 0)
          startTime.SetDate(1970, 1, 1);  // precipitation starts before forecast period
        TextGenPosixTime endTime((*dataSourceVector)[i - 1]->theObservationTime);
        dataDestinationVector->emplace_back(startTime, endTime);
      }
      periodStartIndex = i;
    }
  }
  if (!isDryPrevious && periodStartIndex != dataSourceVector->size() - 1)
    appendFinalPrecipitationPeriod(*dataSourceVector, periodStartIndex, *dataDestinationVector);
  //	joinPrecipitationPeriods(*dataDestinationVector);
}

void PrecipitationForecast::findOutPrecipitationWeatherEvents()
{
  if (theParameters.theForecastArea & COASTAL_AREA)
    findOutPrecipitationWeatherEvents(
        thePrecipitationPeriodsCoastal, COASTAL_AREA, thePrecipitationWeatherEventsCoastal);
  if (theParameters.theForecastArea & INLAND_AREA)
    findOutPrecipitationWeatherEvents(
        thePrecipitationPeriodsInland, INLAND_AREA, thePrecipitationWeatherEventsInland);
  if (theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
    findOutPrecipitationWeatherEvents(
        thePrecipitationPeriodsFull, FULL_AREA, thePrecipitationWeatherEventsFull);
}

void PrecipitationForecast::addPoutaantuuEvent(
    const vector<WeatherPeriod>& thePrecipitationPeriods,
    unsigned int i,
    const TextGenPosixTime& precipitationEndTime,
    unsigned short theForecastArea,
    weather_event_id_vector& thePrecipitationWeatherEvents) const
{
  // Check if next period starts close enough to suppress poutaantuu
  if (i < thePrecipitationPeriods.size() - 1)
  {
    if (thePrecipitationPeriods.at(i + 1).localStartTime().DifferenceInHours(
            precipitationEndTime) <= 2)
      return;
  }

  weather_event_id event =
      (getPrecipitationExtent(thePrecipitationPeriods.at(i), theForecastArea) >
       MOSTLY_DRY_WEATHER_LIMIT)
          ? POUTAANTUU
          : POUTAANTUU_WHEN_EXTENT_SMALL;

  thePrecipitationWeatherEvents.emplace_back(precipitationEndTime, event);
}

void PrecipitationForecast::findOutPrecipitationWeatherEvents(
    const vector<WeatherPeriod>& thePrecipitationPeriods,
    const unsigned short theForecastArea,
    weather_event_id_vector& thePrecipitationWeatherEvents)
{
  const TextGenPosixTime& forecastStart = theParameters.theForecastPeriod.localStartTime();
  const TextGenPosixTime& forecastEnd = theParameters.theForecastPeriod.localEndTime();

  for (unsigned int i = 0; i < thePrecipitationPeriods.size(); i++)
  {
    const TextGenPosixTime precipitationStartTime(thePrecipitationPeriods.at(i).localStartTime());
    const TextGenPosixTime precipitationEndTime(thePrecipitationPeriods.at(i).localEndTime());

    if (precipitationEndTime.DifferenceInHours(precipitationStartTime) < 6)
      continue;

    // start year 1970 indicates that rain starts on previous period
    if (precipitationStartTime.GetYear() != 1970 && precipitationStartTime >= forecastStart &&
        precipitationStartTime <= forecastEnd)
    {
      thePrecipitationWeatherEvents.emplace_back(precipitationStartTime, SADE_ALKAA);
    }

    if (precipitationEndTime.GetYear() != 2037 && precipitationEndTime >= forecastStart &&
        precipitationEndTime <= forecastEnd)
    {
      addPoutaantuuEvent(
          thePrecipitationPeriods, i, precipitationEndTime, theForecastArea,
          thePrecipitationWeatherEvents);
    }
  }
}

float PrecipitationForecast::getPrecipitationExtent(const WeatherPeriod& thePeriod,
                                                    const unsigned short theForecastArea) const
{
  const precipitation_data_vector& dataVector = getPrecipitationDataVector(theForecastArea);

  float extent = getMean(dataVector, PRECIPITATION_EXTENT_DATA, thePeriod);

  return extent;
}

float PrecipitationForecast::getMaxIntensity(const WeatherPeriod& thePeriod,
                                             const unsigned short theForecastArea) const
{
  const precipitation_data_vector& dataVector = getPrecipitationDataVector(theForecastArea);

  float intensity = getMean(dataVector, PRECIPITATION_MAX_DATA, thePeriod);

  return intensity;
}

float PrecipitationForecast::getMeanIntensity(const WeatherPeriod& thePeriod,
                                              const unsigned short theForecastArea) const
{
  const precipitation_data_vector& dataVector = getPrecipitationDataVector(theForecastArea);

  float intensity = getMean(dataVector, PRECIPITATION_MEAN_DATA, thePeriod);

  return intensity;
}

void PrecipitationForecast::removeRedundantWeatherEvents(
    weather_event_id_vector& thePrecipitationWeatherEvents, const vector<int>& theRemoveIndexes)
{
  if (!theRemoveIndexes.empty())
  {
    for (unsigned int i = theRemoveIndexes.size(); i > 0; i--)
    {
      thePrecipitationWeatherEvents.erase(thePrecipitationWeatherEvents.begin() +
                                          theRemoveIndexes[i - 1]);
    }
  }
}

void PrecipitationForecast::removeDuplicatePrecipitationWeatherEvents(
    weather_event_id_vector& thePrecipitationWeatherEvents)
{
  vector<int> removeIndexes;

  for (unsigned int i = 1; i < thePrecipitationWeatherEvents.size(); i++)
  {
    weather_event_id previousPeriodWeatherEventId(thePrecipitationWeatherEvents[i - 1].second);
    weather_event_id currentPeriodWeatherEventId(thePrecipitationWeatherEvents[i].second);
    if (previousPeriodWeatherEventId == currentPeriodWeatherEventId)
      removeIndexes.push_back(i);
  }
  removeRedundantWeatherEvents(thePrecipitationWeatherEvents, removeIndexes);
}

// take into account the continuous precipitation periods
void PrecipitationForecast::cleanUpPrecipitationWeatherEvents(
    weather_event_id_vector& thePrecipitationWeatherEvents)
{
  vector<int> removeIndexes;

  for (unsigned int i = 1; i < thePrecipitationWeatherEvents.size(); i++)
  {
    TextGenPosixTime previousPeriodEndTime(thePrecipitationWeatherEvents[i - 1].first);
    TextGenPosixTime currentPeriodStartTime(thePrecipitationWeatherEvents[i].first);
    weather_event_id previousPeriodWeatherEventId(thePrecipitationWeatherEvents[i - 1].second);
    weather_event_id currentPeriodWeatherEventId(thePrecipitationWeatherEvents[i].second);
    if (abs(previousPeriodEndTime.DifferenceInHours(currentPeriodStartTime)) < 2 &&
        previousPeriodWeatherEventId == SADE_ALKAA &&
        (currentPeriodWeatherEventId == POUTAANTUU ||
         currentPeriodWeatherEventId == POUTAANTUU_WHEN_EXTENT_SMALL))
      removeIndexes.push_back(i - 1);
  }
  removeRedundantWeatherEvents(thePrecipitationWeatherEvents, removeIndexes);
  removeDuplicatePrecipitationWeatherEvents(thePrecipitationWeatherEvents);
}

void PrecipitationForecast::printOutPrecipitationDistribution(std::ostream& theOutput) const
{
  theOutput << "** PRECIPITATION DISTRIBUTION **\n";
  if (!theCoastalData.empty())
  {
    theOutput << "Coastal distribution: \n";
    printOutPrecipitationDistribution(theOutput, theCoastalData);
  }
  if (!theInlandData.empty())
  {
    theOutput << "Inland distribution: \n";
    printOutPrecipitationDistribution(theOutput, theInlandData);
  }
  if (!theFullData.empty())
  {
    theOutput << "Full area distribution: \n";
    printOutPrecipitationDistribution(theOutput, theFullData);
  }
}

void PrecipitationForecast::printOutPrecipitationDistribution(
    std::ostream& theOutput, const precipitation_data_vector& theDataVector)
{
  for (const auto& i : theDataVector)
  {
    theOutput << "distribution(ne,se,sw,nw,n,s,e,w): " << i->theObservationTime << ", "
              << i->thePrecipitationPercentageNorthEast << ", "
              << i->thePrecipitationPercentageSouthEast << ", "
              << i->thePrecipitationPercentageSouthWest << ", "
              << i->thePrecipitationPercentageNorthWest << ", " << i->precipitationPercentageNorth()
              << ", " << i->precipitationPercentageSouth() << ", "
              << i->precipitationPercentageEast() << ", " << i->precipitationPercentageWest()
              << '\n';
  }
}

void PrecipitationForecast::printOutPrecipitationWeatherEvents(std::ostream& theOutput) const
{
  theOutput << "** PRECIPITATION WEATHER EVENTS **\n";
  bool isWeatherEvents = false;
  if (!thePrecipitationWeatherEventsCoastal.empty())
  {
    theOutput << "Coastal precipitation weather events: \n";
    print_out_weather_event_vector(theOutput, thePrecipitationWeatherEventsCoastal);
    isWeatherEvents = true;
  }
  if (!thePrecipitationWeatherEventsInland.empty())
  {
    theOutput << "Inland precipitation weather events: \n";
    print_out_weather_event_vector(theOutput, thePrecipitationWeatherEventsInland);
    isWeatherEvents = true;
  }
  if (!thePrecipitationWeatherEventsFull.empty())
  {
    theOutput << "Full area precipitation weather events: \n";
    print_out_weather_event_vector(theOutput, thePrecipitationWeatherEventsFull);
    isWeatherEvents = true;
  }

  if (!isWeatherEvents)
    theOutput << "No weather events!\n";
}

void PrecipitationForecast::printOutPrecipitationPeriods(std::ostream& theOutput,
                                                         bool isPoint) const
{
  theOutput << "** PRECIPITATION PERIODS **\n";
  bool found = false;
  if (!thePrecipitationPeriodsCoastal.empty())
  {
    theOutput << "Coastal precipitation periods: \n";
    found = printOutPrecipitationPeriods(
        theOutput, thePrecipitationPeriodsCoastal, theCoastalData, isPoint);
  }
  if (!thePrecipitationPeriodsInland.empty())
  {
    theOutput << "Inland precipitation periods: \n";
    found = printOutPrecipitationPeriods(
        theOutput, thePrecipitationPeriodsInland, theInlandData, isPoint);
  }
  if (!thePrecipitationPeriodsFull.empty())
  {
    theOutput << "Full precipitation periods: \n";
    found =
        printOutPrecipitationPeriods(theOutput, thePrecipitationPeriodsFull, theFullData, isPoint);
  }

  if (!found)
    theOutput << "No precipitation periods!\n";
}

bool PrecipitationForecast::printOutPrecipitationPeriods(
    std::ostream& theOutput,
    const vector<WeatherPeriod>& thePrecipitationPeriods,
    const precipitation_data_vector& theDataVector,
    bool isPoint) const
{
  bool retval = false;

  for (const auto& thePrecipitationPeriod : thePrecipitationPeriods)
  {
    bool intersectionPeriodFound(false);
    WeatherPeriod period = get_intersection_period(
        thePrecipitationPeriod, theParameters.theForecastPeriod, intersectionPeriodFound);

    if (!intersectionPeriodFound)
    {
      continue;
    }
    theOutput << period.localStartTime() << "..." << period.localEndTime() << '\n';

    theOutput  //<< ": "
        << " min_of_max = " << setw(7) << setfill(' ') << setprecision(3)
        << getMin(theDataVector, PRECIPITATION_MAX_DATA, period) << '\n'
        << " min_of_mean = " << setw(7) << setfill(' ') << setprecision(3)
        << getMin(theDataVector, PRECIPITATION_MEAN_DATA, period) << '\n'
        << " mean_of_max = " << setw(7) << setfill(' ') << setprecision(3)
        << getMean(theDataVector, PRECIPITATION_MAX_DATA, period) << '\n'
        << " mean_of_mean = " << setw(7) << setfill(' ') << setprecision(3)
        << getMean(theDataVector, PRECIPITATION_MEAN_DATA, period) << '\n'
        << " max_of_max = " << setprecision(3)
        << getMax(theDataVector, PRECIPITATION_MAX_DATA, period) << '\n'
        << " max_of_mean = " << setprecision(3)
        << getMax(theDataVector, PRECIPITATION_MEAN_DATA, period) << '\n';

    if (!isPoint)
    {
      precipitation_traverse_id traverseId = getPrecipitationTraverseId(period);
      if (traverseId != MISSING_TRAVERSE_ID)
      {
        theOutput << " movement=" << precipitation_traverse_string(traverseId);
      }
    }

    theOutput << '\n';
    retval = true;
  }

  return retval;
}

void PrecipitationForecast::printOutPrecipitationDistribution(std::ostream& theOutput,
                                                              const WeatherPeriod& thePeriod)
{
  //	if(!isDryPeriod(thePeriod, theParameters.theForecastArea))
  {
    float northPercentage(0.0);
    float southPercentage(0.0);
    float eastPercentage(0.0);
    float westPercentage(0.0);
    float northEastPercentage(0.0);
    float southEastPercentage(0.0);
    float southWestPercentage(0.0);
    float northWestPercentage(0.0);

    getPrecipitationDistribution(thePeriod,
                                 northPercentage,
                                 southPercentage,
                                 eastPercentage,
                                 westPercentage,
                                 northEastPercentage,
                                 southEastPercentage,
                                 southWestPercentage,
                                 northWestPercentage);

    theOutput << "precipitation north: " << northPercentage << '\n';
    theOutput << "precipitation south: " << southPercentage << '\n';
    theOutput << "precipitation east: " << eastPercentage << '\n';
    theOutput << "precipitation west: " << westPercentage << '\n';
    theOutput << "precipitation northeast: " << northEastPercentage << '\n';
    theOutput << "precipitation southeast: " << southEastPercentage << '\n';
    theOutput << "precipitation southwest: " << southWestPercentage << '\n';
    theOutput << "precipitation northwest: " << northWestPercentage << '\n';
  }
}

bool PrecipitationForecast::getPrecipitationPeriod(const TextGenPosixTime& theTimestamp,
                                                   TextGenPosixTime& theStartTime,
                                                   TextGenPosixTime& theEndTime) const
{
  const vector<WeatherPeriod>* precipitationPeriodVector = nullptr;

  if (theParameters.theForecastArea & FULL_AREA)
    precipitationPeriodVector = &thePrecipitationPeriodsFull;
  else if (theParameters.theForecastArea & COASTAL_AREA)
    precipitationPeriodVector = &thePrecipitationPeriodsCoastal;
  else if (theParameters.theForecastArea & INLAND_AREA)
    precipitationPeriodVector = &thePrecipitationPeriodsInland;

  if (!precipitationPeriodVector)
    return false;

  for (const auto& i : *precipitationPeriodVector)
  {
    if (is_inside(theTimestamp, i))
    {
      theStartTime = i.localStartTime();
      theEndTime = i.localEndTime();
      //			theWeatherPeriod = precipitationPeriodVector->at(i);
      return true;
    }
  }
  return false;
}
Sentence PrecipitationForecast::precipitationPoutaantuuAndCloudiness(
    const Sentence& thePeriodPhrase, cloudiness_id theCloudinessId) const
{
  Sentence sentence;

  precipitation_form_id previousPrecipitationForm = getPoutaantuuPrecipitationForm();

  if (theCloudinessId == PUOLIPILVINEN_JA_PILVINEN)
  {
    switch (previousPrecipitationForm)
    {
      case DRIZZLE_FORM:
        sentence << SAA_POUTAANTUU_TIHKUSATEEN_JALKEEN_JA_VAIHTELEE_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      case SLEET_FORM:
        sentence << SAA_POUTAANTUU_RANTASATEEN_JALKEEN_JA_VAIHTELEE_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      case SNOW_FORM:
        sentence << SAA_POUTAANTUU_LUMISATEEN_JALKEEN_JA_VAIHTELEE_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      default:
        sentence << SAA_POUTAANTUU_VESISATEEN_JALKEEN_JA_VAIHTELEE_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
    }
  }
  else if (theCloudinessId == PILVINEN)
  {
    switch (previousPrecipitationForm)
    {
      case DRIZZLE_FORM:
        sentence << SAA_POUTAANTUU_TIHKUSATEEN_JALKEEN_JA_ON_PILVINEN_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      case SLEET_FORM:
        sentence << SAA_POUTAANTUU_RANTASATEEN_JALKEEN_JA_ON_PILVINEN_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      case SNOW_FORM:
        sentence << SAA_POUTAANTUU_LUMISATEEN_JALKEEN_JA_ON_PILVINEN_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      default:
        sentence << SAA_POUTAANTUU_VESISATEEN_JALKEEN_JA_ON_PILVINEN_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
    }
  }
  else if (theCloudinessId == VERRATTAIN_PILVINEN)
  {
    switch (previousPrecipitationForm)
    {
      case DRIZZLE_FORM:
        sentence << SAA_POUTAANTUU_TIHKUSATEEN_JALKEEN_JA_ON_VERRATTAIN_PILVINEN_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      case SLEET_FORM:
        sentence << SAA_POUTAANTUU_RANTASATEEN_JALKEEN_JA_ON_VERRATTAIN_PILVINEN_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      case SNOW_FORM:
        sentence << SAA_POUTAANTUU_LUMISATEEN_JALKEEN_JA_ON_VERRATTAIN_PILVINEN_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      default:
        sentence << SAA_POUTAANTUU_VESISATEEN_JALKEEN_JA_ON_VERRATTAIN_PILVINEN_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
    }
  }
  else if (theCloudinessId == PUOLIPILVINEN)
  {
    switch (previousPrecipitationForm)
    {
      case DRIZZLE_FORM:
        sentence << SAA_POUTAANTUU_TIHKUSATEEN_JALKEEN_JA_ON_PUOLIPILVINEN_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      case SLEET_FORM:
        sentence << SAA_POUTAANTUU_RANTASATEEN_JALKEEN_JA_ON_PUOLIPILVINEN_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      case SNOW_FORM:
        sentence << SAA_POUTAANTUU_LUMISATEEN_JALKEEN_JA_ON_PUOLIPILVINEN_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      default:
        sentence << SAA_POUTAANTUU_VESISATEEN_JALKEEN_JA_ON_PUOLIPILVINEN_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
    }
  }
  else if (theCloudinessId == MELKO_SELKEA)
  {
    switch (previousPrecipitationForm)
    {
      case DRIZZLE_FORM:
        sentence << SAA_POUTAANTUU_TIHKUSATEEN_JALKEEN_JA_ON_MELKO_SELKEA_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      case SLEET_FORM:
        sentence << SAA_POUTAANTUU_RANTASATEEN_JALKEEN_JA_ON_MELKO_SELKEA_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      case SNOW_FORM:
        sentence << SAA_POUTAANTUU_LUMISATEEN_JALKEEN_JA_ON_MELKO_SELKEA_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      default:
        sentence << SAA_POUTAANTUU_VESISATEEN_JALKEEN_JA_ON_MELKO_SELKEA_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
    }
  }
  else if (theCloudinessId == SELKEA)
  {
    switch (previousPrecipitationForm)
    {
      case DRIZZLE_FORM:
        sentence << SAA_POUTAANTUU_TIHKUSATEEN_JALKEEN_JA_ON_SELKEA_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      case SLEET_FORM:
        sentence << SAA_POUTAANTUU_RANTASATEEN_JALKEEN_JA_ON_SELKEA_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      case SNOW_FORM:
        sentence << SAA_POUTAANTUU_LUMISATEEN_JALKEEN_JA_ON_SELKEA_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
      default:
        sentence << SAA_POUTAANTUU_VESISATEEN_JALKEEN_JA_ON_SELKEA_COMPOSITE_PHRASE
                 << thePeriodPhrase;
        break;
    }
  }

  setDryPeriodTautologyFlag(true);

  return sentence;
}

Sentence PrecipitationForecast::precipitationChangeSentence(
    const WeatherPeriod& thePeriod,
    const Sentence& thePeriodPhrase,
    weather_event_id theWeatherEvent,
    std::vector<std::pair<WeatherPeriod, Sentence>>& theAdditionalSentences) const
{
  Sentence sentence;

  if (theWeatherEvent == POUTAANTUU || theWeatherEvent == POUTAANTUU_WHEN_EXTENT_SMALL)
  {
    switch (getPoutaantuuPrecipitationForm())
    {
      case DRIZZLE_FORM:
        sentence << SAA_POUTAANTUU_TIHKUSATEEN_JALKEEN_COMPOSITE_PHRASE << thePeriodPhrase;
        break;
      case SLEET_FORM:
        sentence << SAA_POUTAANTUU_RANTASATEEN_JALKEEN_COMPOSITE_PHRASE << thePeriodPhrase;
        break;
      case SNOW_FORM:
        sentence << SAA_POUTAANTUU_LUMISATEEN_JALKEEN_COMPOSITE_PHRASE << thePeriodPhrase;
        break;
      default:
        sentence << SAA_POUTAANTUU_VESISATEEN_JALKEEN_COMPOSITE_PHRASE << thePeriodPhrase;
        break;
    }

    theDryPeriodTautologyFlag = true;
  }
  else  // sade alkaa
  {
    sentence << precipitationSentence(thePeriod, thePeriodPhrase, theAdditionalSentences);
  }

  return sentence;
}

void PrecipitationForecast::getPrecipitationDistribution(const WeatherPeriod& /*thePeriod*/,
                                                         float& theNorthPercentage,
                                                         float& theSouthPercentage,
                                                         float& theEastPercentage,
                                                         float& theWestPercentage,
                                                         float& theNorthEastPercentage,
                                                         float& theSouthEastPercentage,
                                                         float& theSouthWestPercentage,
                                                         float& theNorthWestPercentage) const
{
  const precipitation_data_vector* precipitationDataVector =
      ((theParameters.theForecastArea & FULL_AREA)
           ? &theFullData
           : ((theParameters.theForecastArea & INLAND_AREA) ? &theInlandData : &theCoastalData));

  theNorthPercentage = 0.0;
  theSouthPercentage = 0.0;
  theEastPercentage = 0.0;
  theWestPercentage = 0.0;
  theNorthEastPercentage = 0.0;
  theSouthEastPercentage = 0.0;
  theSouthWestPercentage = 0.0;
  theNorthWestPercentage = 0.0;

  unsigned int count = 0;
  for (const auto& i : *precipitationDataVector)
  {
    if (i->thePrecipitationIntensity > 0)
    {
      theNorthEastPercentage += i->thePrecipitationPercentageNorthEast;
      theSouthEastPercentage += i->thePrecipitationPercentageSouthEast;
      theSouthWestPercentage += i->thePrecipitationPercentageSouthWest;
      theNorthWestPercentage += i->thePrecipitationPercentageNorthWest;
      count++;
    }
  }
  if (count > 0)
  {
    theNorthEastPercentage /= count;
    theSouthEastPercentage /= count;
    theSouthWestPercentage /= count;
    theNorthWestPercentage /= count;
    theNorthPercentage = (theNorthEastPercentage + theNorthWestPercentage);
    theSouthPercentage = (theSouthEastPercentage + theSouthWestPercentage);
    theEastPercentage = (theNorthEastPercentage + theSouthEastPercentage);
    theWestPercentage = (theSouthEastPercentage + theSouthWestPercentage);
  }
}

namespace
{
// Returns the dominant precipitation form when one form >= 90%, else MISSING
precipitation_form_id dominantForm(float water, float drizzle, float sleet, float snow)
{
  if (water >= 90)
    return WATER_FORM;
  if (drizzle >= 90)
    return DRIZZLE_FORM;
  if (sleet >= 90)
    return SLEET_FORM;
  if (snow >= 90)
    return SNOW_FORM;
  return MISSING_PRECIPITATION_FORM;
}

// Look up transformation id from (beg, end) form pair + matching pearson coefficients
precipitation_form_transformation_id lookupFormTransformation(precipitation_form_id beg,
                                                              precipitation_form_id end,
                                                              double co_water,
                                                              double co_drizzle,
                                                              double co_sleet,
                                                              double co_snow)
{
  using fid = precipitation_form_id;
  // Each row: {beg, end, co_decreasing, co_increasing, result}
  struct Row
  {
    fid b, e;
    double dec, inc;
    precipitation_form_transformation_id result;
  };
  static const Row table[] = {
      {WATER_FORM, SNOW_FORM, 0, 0, WATER_TO_SNOW},
      {WATER_FORM, DRIZZLE_FORM, 0, 0, WATER_TO_DRIZZLE},
      {WATER_FORM, SLEET_FORM, 0, 0, WATER_TO_SLEET},
      {DRIZZLE_FORM, SNOW_FORM, 0, 0, DRIZZLE_TO_SNOW},
      {DRIZZLE_FORM, WATER_FORM, 0, 0, DRIZZLE_TO_WATER},
      {DRIZZLE_FORM, SLEET_FORM, 0, 0, DRIZZLE_TO_SLEET},
      {SLEET_FORM, SNOW_FORM, 0, 0, SLEET_TO_SNOW},
      {SLEET_FORM, DRIZZLE_FORM, 0, 0, SLEET_TO_DRIZZLE},
      {SLEET_FORM, WATER_FORM, 0, 0, SLEET_TO_WATER},
      {SNOW_FORM, WATER_FORM, 0, 0, SNOW_TO_WATER},
      {SNOW_FORM, DRIZZLE_FORM, 0, 0, SNOW_TO_DRIZZLE},
      {SNOW_FORM, SLEET_FORM, 0, 0, SNOW_TO_SLEET},
  };

  // Map form -> its pearson coefficient
  auto coeff = [&](fid f) -> double {
    if (f == WATER_FORM)
      return co_water;
    if (f == DRIZZLE_FORM)
      return co_drizzle;
    if (f == SLEET_FORM)
      return co_sleet;
    if (f == SNOW_FORM)
      return co_snow;
    return 0.0;
  };

  for (const auto& row : table)
  {
    if (row.b != beg || row.e != end)
      continue;
    if (coeff(beg) <= -PEARSON_CO_FORM_TRANSFORM && coeff(end) >= PEARSON_CO_FORM_TRANSFORM)
      return row.result;
    break;
  }
  return NO_FORM_TRANSFORMATION;
}
}  // namespace

precipitation_form_transformation_id PrecipitationForecast::getPrecipitationTransformationId(
    const WeatherPeriod& thePeriod, const unsigned short theForecastArea) const
{
  if (thePeriod.localEndTime().DifferenceInHours(thePeriod.localStartTime()) < 5)
    return NO_FORM_TRANSFORMATION;

  TextGenPosixTime atStartBeg(thePeriod.localStartTime());
  if (atStartBeg.GetYear() == 1970)
    atStartBeg = theParameters.theForecastPeriod.localStartTime();
  TextGenPosixTime atStartEnd(atStartBeg);
  atStartEnd.ChangeByHours(2);
  WeatherPeriod atStartPeriod(atStartBeg, atStartEnd);
  TextGenPosixTime atEndBeg(thePeriod.localEndTime());
  if (atEndBeg.GetYear() == 2037)
    atEndBeg = theParameters.theForecastPeriod.localEndTime();
  TextGenPosixTime atEndEnd(atEndBeg);
  atEndBeg.ChangeByHours(-2);
  WeatherPeriod atEndPeriod(atEndBeg, atEndEnd);

  const precipitation_data_vector& theDataVector = getPrecipitationDataVector(theForecastArea);

  float precipitationFormWaterBeg =
      getMean(theDataVector, PRECIPITATION_FORM_WATER_DATA, WeatherPeriod(atStartBeg, atStartBeg));
  float precipitationFormDrizzleBeg = getMean(
      theDataVector, PRECIPITATION_FORM_DRIZZLE_DATA, WeatherPeriod(atStartBeg, atStartBeg));
  float precipitationFormSleetBeg =
      getMean(theDataVector, PRECIPITATION_FORM_SLEET_DATA, WeatherPeriod(atStartBeg, atStartBeg));
  float precipitationFormSnowBeg =
      getMean(theDataVector, PRECIPITATION_FORM_SNOW_DATA, WeatherPeriod(atStartBeg, atStartBeg));
  float precipitationFormWaterEnd =
      getMean(theDataVector, PRECIPITATION_FORM_WATER_DATA, WeatherPeriod(atEndEnd, atEndEnd));
  float precipitationFormDrizzleEnd =
      getMean(theDataVector, PRECIPITATION_FORM_DRIZZLE_DATA, WeatherPeriod(atEndEnd, atEndEnd));
  float precipitationFormSleetEnd =
      getMean(theDataVector, PRECIPITATION_FORM_SLEET_DATA, WeatherPeriod(atEndEnd, atEndEnd));
  float precipitationFormSnowEnd =
      getMean(theDataVector, PRECIPITATION_FORM_SNOW_DATA, WeatherPeriod(atEndEnd, atEndEnd));

  precipitation_form_id precipitationFormBeg =
      dominantForm(precipitationFormWaterBeg,
                   precipitationFormDrizzleBeg,
                   precipitationFormSleetBeg,
                   precipitationFormSnowBeg);
  precipitation_form_id precipitationFormEnd =
      dominantForm(precipitationFormWaterEnd,
                   precipitationFormDrizzleEnd,
                   precipitationFormSleetEnd,
                   precipitationFormSnowEnd);

  if (precipitationFormBeg == precipitationFormEnd)
    return NO_FORM_TRANSFORMATION;

  const weather_result_data_item_vector* precipitationFormWaterHourly =
      get_data_vector(theParameters, PRECIPITATION_FORM_WATER_DATA);
  const weather_result_data_item_vector* precipitationFormSnowHourly =
      get_data_vector(theParameters, PRECIPITATION_FORM_SNOW_DATA);
  const weather_result_data_item_vector* precipitationFormDrizzleHourly =
      get_data_vector(theParameters, PRECIPITATION_FORM_DRIZZLE_DATA);
  const weather_result_data_item_vector* precipitationFormSleetHourly =
      get_data_vector(theParameters, PRECIPITATION_FORM_SLEET_DATA);

  unsigned int startIndex;
  unsigned int endIndex;
  get_period_start_end_index(thePeriod, *precipitationFormWaterHourly, startIndex, endIndex);

  double pearson_co_water =
      get_pearson_coefficient(*precipitationFormWaterHourly, startIndex, endIndex);
  double pearson_co_snow =
      get_pearson_coefficient(*precipitationFormSnowHourly, startIndex, endIndex);
  double pearson_co_drizzle =
      get_pearson_coefficient(*precipitationFormDrizzleHourly, startIndex, endIndex);
  double pearson_co_sleet =
      get_pearson_coefficient(*precipitationFormSleetHourly, startIndex, endIndex);

  return lookupFormTransformation(precipitationFormBeg,
                                  precipitationFormEnd,
                                  pearson_co_water,
                                  pearson_co_drizzle,
                                  pearson_co_sleet,
                                  pearson_co_snow);
}

namespace
{
// Merge adjacent heavy precipitation periods that are close enough
vector<WeatherPeriod> mergeHeavyPeriods(vector<WeatherPeriod> periods)
{
  vector<WeatherPeriod> merged;
  for (int i = static_cast<int>(periods.size()) - 2; i >= 0; i--)
  {
    auto& cur = periods[i];
    auto& nxt = periods[i + 1];
    if (abs(nxt.localStartTime().DifferenceInHours(cur.localEndTime())) <= 2 &&
        (get_period_length(cur) >= 2 || get_period_length(nxt) >= 2))
    {
      cur = WeatherPeriod(cur.localStartTime(), nxt.localEndTime());
    }
    else
    {
      merged.push_back(nxt);
    }
  }
  merged.push_back(periods[0]);
  sort(merged.begin(), merged.end());
  return merged;
}

// Return index of the longest period, or -1 if empty
int longestPeriodIndex(const vector<WeatherPeriod>& periods)
{
  if (periods.empty())
    return -1;
  int best = 0;
  for (int i = 1; i < static_cast<int>(periods.size()); i++)
    if (get_period_length(periods[i]) > get_period_length(periods[best]))
      best = i;
  return best;
}
}  // namespace

WeatherPeriod PrecipitationForecast::getHeavyPrecipitationPeriod(
    const WeatherPeriod& thePeriod, const precipitation_data_vector& theDataVector) const
{
  vector<WeatherPeriod> heavyPrecipitationPeriods;

  // First find out all heavy precipitation periods
  int heavyPrecipitationStartIndex(-1);
  int heavyPrecipitationEndIndex(-1);
  for (unsigned int i = 0; i < theDataVector.size(); i++)
  {
    const PrecipitationDataItemData* precipitationItem(theDataVector[i].get());

    if (precipitationItem->theObservationTime < thePeriod.localStartTime())
      continue;
    if (precipitationItem->theObservationTime > thePeriod.localEndTime())
      break;

    precipitation_form_id precipitationForm(
        get_complete_precipitation_form(theParameters.theVariable,
                                        precipitationItem->thePrecipitationFormWater,
                                        precipitationItem->thePrecipitationFormDrizzle,
                                        precipitationItem->thePrecipitationFormSleet,
                                        precipitationItem->thePrecipitationFormSnow,
                                        precipitationItem->thePrecipitationFormFreezingRain,
                                        precipitationItem->thePrecipitationFormFreezingDrizzle));
    float lowerLimit(kFloatMissing);
    float upperLimit(kFloatMissing);
    get_precipitation_limit_value(
        theParameters, precipitationForm, HEAVY_PRECIPITATION, lowerLimit, upperLimit);

    if (theDataVector[i]->thePrecipitationMaxIntensity >= lowerLimit)
    {
      if (heavyPrecipitationStartIndex == -1)
        heavyPrecipitationStartIndex = i;  // heavy precipitation starts
      heavyPrecipitationEndIndex = i;
    }
    else if (heavyPrecipitationStartIndex != -1)
    {
      heavyPrecipitationPeriods.emplace_back(
          theDataVector[heavyPrecipitationStartIndex]->theObservationTime,
          theDataVector[heavyPrecipitationEndIndex]->theObservationTime);
      heavyPrecipitationStartIndex = -1;
      heavyPrecipitationEndIndex = -1;
    }
  }
  if (heavyPrecipitationStartIndex != -1 && heavyPrecipitationPeriods.empty())
  {
    heavyPrecipitationPeriods.emplace_back(
        theDataVector[heavyPrecipitationStartIndex]->theObservationTime,
        theDataVector[heavyPrecipitationEndIndex]->theObservationTime);
  }

  if (heavyPrecipitationPeriods.empty())
    return {thePeriod.localStartTime(), thePeriod.localStartTime()};

  // Merge periods with small gaps between them, then return the longest
  vector<WeatherPeriod> mergedPeriods = mergeHeavyPeriods(heavyPrecipitationPeriods);
  int heavyIndex = longestPeriodIndex(mergedPeriods);

  if (heavyIndex >= 0)
    return mergedPeriods[heavyIndex];
  return {thePeriod.localStartTime(), thePeriod.localStartTime()};
}

void PrecipitationForecast::calculatePrecipitationParameters(
    const WeatherPeriod& thePeriod,
    const precipitation_data_vector& theDataVector,
    float& thePrecipitationIntensity,
    float& thePrecipitationAbsoluteMaxIntensity,
    float& thePrecipitationExtent,
    float& thePrecipitationFormWater,
    float& thePrecipitationFormDrizzle,
    float& thePrecipitationFormSleet,
    float& thePrecipitationFormSnow,
    float& thePrecipitationFormFreezingRain,
    float& thePrecipitationFormFreezingDrizzle)
{
  // thePrecipitationIntensity contains mean of the maximum and mean precipitation
  thePrecipitationIntensity = getMean(theDataVector, PRECIPITATION_MEAN_DATA, thePeriod);
  thePrecipitationAbsoluteMaxIntensity = getMax(theDataVector, PRECIPITATION_MAX_DATA, thePeriod);
  thePrecipitationExtent = getMean(theDataVector, PRECIPITATION_EXTENT_DATA, thePeriod);
  thePrecipitationFormWater = getMean(theDataVector, PRECIPITATION_FORM_WATER_DATA, thePeriod);
  thePrecipitationFormDrizzle = getMean(theDataVector, PRECIPITATION_FORM_DRIZZLE_DATA, thePeriod);
  thePrecipitationFormSleet = getMean(theDataVector, PRECIPITATION_FORM_SLEET_DATA, thePeriod);
  thePrecipitationFormSnow = getMean(theDataVector, PRECIPITATION_FORM_SNOW_DATA, thePeriod);
  thePrecipitationFormFreezingRain =
      getMean(theDataVector, PRECIPITATION_FORM_FREEZING_RAIN_DATA, thePeriod);
  thePrecipitationFormFreezingDrizzle =
      getMean(theDataVector, PRECIPITATION_FORM_FREEZING_DRIZZLE_DATA, thePeriod);
}

Sentence PrecipitationForecast::parsePlainPrecipitationPhrase(
    const FinalSentenceContext& ctx, const Sentence& plainPrecipitation) const
{
  Sentence s;
  if (ctx.periodPhraseEmpty && ctx.areaPhraseEmpty)
  {
    s << PAIKOIN_SADETTA_COMPOSITE_PHRASE << ctx.inPlacesPhrase << plainPrecipitation;
  }
  else if (ctx.periodPhraseEmpty && !ctx.areaPhraseEmpty)
  {
    if (ctx.inPlacesPhraseEmpty)
      s << SISAMAASSA_SADETTA_COMPOSITE_PHRASE << ctx.areaPhrase << plainPrecipitation;
    else
      s << SISAMAASSA_PAIKOIN_SADETTA_COMPOSITE_PHRASE << ctx.areaPhrase << ctx.inPlacesPhrase
        << plainPrecipitation;
  }
  else if (!ctx.periodPhraseEmpty && ctx.areaPhraseEmpty)
  {
    if (ctx.inPlacesPhraseEmpty)
      s << HUOMENNA_SADETTA_COMPOSITE_PHRASE << ctx.periodPhrase << plainPrecipitation;
    else
      s << HUOMENNA_PAIKOIN_SADETTA_COMPOSITE_PHRASE << ctx.periodPhrase << ctx.inPlacesPhrase
        << plainPrecipitation;
  }
  else
  {
    if (ctx.inPlacesPhraseEmpty)
      s << HUOMENNA_SISAMAASSA_SADETTA_COMPOSITE_PHRASE << ctx.periodPhrase << ctx.areaPhrase
        << plainPrecipitation;
    else
      s << HUOMENNA_SISAMAASSA_PAIKOIN_SADETTA_COMPOSITE_PHRASE << ctx.periodPhrase
        << ctx.areaPhrase << ctx.inPlacesPhrase << plainPrecipitation;
  }
  return s;
}

Sentence PrecipitationForecast::parseMostlyDryPhrase(const FinalSentenceContext& ctx,
                                                     map<string, Sentence>& elems) const
{
  // Table: {key, sisamaassa_variant, huomenna_variant, huomenna_sisamaassa_variant}
  struct Row
  {
    const char* key;
    const char* areaOnly;
    const char* periodOnly;
    const char* both;
  };
  static const Row table[] = {
      {HUOMENNA_YKSITTAISET_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       SISAMAASSA_YKSITTAISET_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       HUOMENNA_YKSITTAISET_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       HUOMENNA_SISAMAASSA_YKSITTAISET_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE},
      {HUOMENNA_YKSITTAISET_VESI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       SISAMAASSA_YKSITTAISET_VESI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       HUOMENNA_YKSITTAISET_VESI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       HUOMENNA_SISAMAASSA_YKSITTAISET_VESI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE},
      {HUOMENNA_YKSITTAISET_VESI_TAI_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       SISAMAASSA_YKSITTAISET_VESI_TAI_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       HUOMENNA_YKSITTAISET_VESI_TAI_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       HUOMENNA_SISAMAASSA_YKSITTAISET_VESI_TAI_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE},
      {HUOMENNA_YKSITTAISET_SADEKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       SISAMAASSA_YKSITTAISET_SADEKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       HUOMENNA_YKSITTAISET_SADEKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       HUOMENNA_SISAMAASSA_YKSITTAISET_SADEKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE},
      {HUOMENNA_YKSITTAISET_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       SISAMAASSA_YKSITTAISET_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       HUOMENNA_YKSITTAISET_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       HUOMENNA_SISAMAASSA_YKSITTAISET_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE},
      {HUOMENNA_YKSITTAISET_RANTA_TAI_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       SISAMAASSA_YKSITTAISET_RANTA_TAI_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       HUOMENNA_YKSITTAISET_RANTA_TAI_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       HUOMENNA_SISAMAASSA_YKSITTAISET_RANTA_TAI_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE},
      {HUOMENNA_YKSITTAISET_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       SISAMAASSA_YKSITTAISET_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       HUOMENNA_YKSITTAISET_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       HUOMENNA_SISAMAASSA_YKSITTAISET_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE},
      {HUOMENNA_YKSITTAISET_LUMI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       SISAMAASSA_YKSITTAISET_LUMI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       HUOMENNA_YKSITTAISET_LUMI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
       HUOMENNA_SISAMAASSA_YKSITTAISET_LUMI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE},
  };

  Sentence s;
  if (ctx.periodPhraseEmpty && ctx.areaPhraseEmpty)
  {
    s << ctx.precipitation;
    return s;
  }

  for (const auto& row : table)
  {
    if (elems.find(row.key) == elems.end())
      continue;
    if (!ctx.periodPhraseEmpty && !ctx.areaPhraseEmpty)
      s << row.both << ctx.periodPhrase << ctx.areaPhrase;
    else if (ctx.periodPhraseEmpty)
      s << row.areaOnly << ctx.areaPhrase;
    else
      s << row.periodOnly << ctx.periodPhrase;
    break;
  }
  return s;
}

Sentence PrecipitationForecast::parseDryPhrase(const FinalSentenceContext& ctx) const
{
  Sentence s;
  if (ctx.periodPhraseEmpty && ctx.areaPhraseEmpty)
    s << SAA_ON_POUTAINEN_PHRASE;
  else if (ctx.periodPhraseEmpty)
    s << SISAMAASSA_SAA_ON_POUTAINEN_COMPOSITE_PHRASE << ctx.areaPhrase;
  else if (ctx.areaPhraseEmpty)
    s << HUOMENNA_SAA_ON_POUTAINEN_COMPOSITE_PHRASE << ctx.periodPhrase;
  else
    s << HUOMENNA_SISAMAASSA_SAA_ON_POUTAINEN_COMPOSITE_PHRASE << ctx.periodPhrase
      << ctx.areaPhrase;
  return s;
}

namespace
{
// Build a regular precipitation sentence selecting the right composite phrase
// based on period/area/inPlaces/intensity flags
// Phrase table index: bit0=!periodEmpty, bit1=!areaEmpty, bit2=!inPlacesEmpty, bit3=!intensityEmpty
// Order of 16 entries: none_none, none_none_i, places_none, places_none_i,
//                      area_none, area_none_i, area_places, area_places_i,
//                      period_none, period_none_i, period_places, period_places_i,
//                      both_none, both_none_i, both_places, both_places_i
struct RegularSentenceArgs
{
  const char* phrases[16];
};

Sentence buildRegularSentence(const PrecipitationForecast::FinalSentenceContext& ctx,
                              const RegularSentenceArgs& a)
{
  // Compute index from flags. Bit meanings:
  // bit0: period present, bit1: area present, bit2: inPlaces present, bit3: intensity present
  const int idx = (ctx.periodPhraseEmpty ? 0 : 1) | (ctx.areaPhraseEmpty ? 0 : 2) |
                  (ctx.inPlacesPhraseEmpty ? 0 : 4) | (ctx.intensityEmpty ? 0 : 8);
  const char* phrase = a.phrases[idx];

  Sentence s;
  if (!phrase)
  {
    // none_none with no phrase just outputs precipitation directly
    s << ctx.precipitation;
    return s;
  }

  s << phrase;
  if (!ctx.periodPhraseEmpty)
    s << ctx.periodPhrase;
  if (!ctx.areaPhraseEmpty)
    s << ctx.areaPhrase;
  if (!ctx.inPlacesPhraseEmpty)
    s << ctx.inPlacesPhrase;
  if (!ctx.intensityEmpty)
    s << ctx.intensity;
  s << ctx.precipitation;
  return s;
}
}  // namespace

// Check if any "mostly dry" shower phrase key exists in the composite phrase elements
bool hasMostlyDryPhrase(const map<string, Sentence>& elems)
{
  static const char* const keys[] = {
      HUOMENNA_YKSITTAISET_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
      HUOMENNA_YKSITTAISET_VESI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
      HUOMENNA_YKSITTAISET_VESI_TAI_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
      HUOMENNA_YKSITTAISET_SADEKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
      HUOMENNA_YKSITTAISET_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
      HUOMENNA_YKSITTAISET_RANTA_TAI_VESIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
      HUOMENNA_YKSITTAISET_LUMIKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
      HUOMENNA_YKSITTAISET_LUMI_TAI_RANTAKUUROT_MAHDOLLISIA_COMPOSITE_PHRASE,
  };
  for (const auto* key : keys)
    if (elems.find(key) != elems.end())
      return true;
  return false;
}

// Bit-index order: bit0=periodPresent, bit1=areaPresent, bit2=inPlacesPresent, bit3=intensityPresent
// So: [0]=none, [1]=period, [2]=area, [3]=period+area,
//     [4]=inPlaces, [5]=period+inPlaces, [6]=area+inPlaces, [7]=period+area+inPlaces,
//     [8]=intensity, [9]=period+intensity, [10]=area+intensity, [11]=period+area+intensity,
//     [12]=inPlaces+intensity, [13]=period+inPlaces+intensity,
//     [14]=area+inPlaces+intensity, [15]=period+area+inPlaces+intensity

Sentence PrecipitationForecast::parseFreezingRainPhrase(const FinalSentenceContext& ctx) const
{
  static const RegularSentenceArgs a = {{
      SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE,            // 0
      HUOMENNA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE,   // 1: period
      SISAMAASSA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE, // 2: area
      HUOMENNA_SISAMAASSA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE, // 3: period+area
      PAIKOIN_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE,    // 4: inPlaces
      HUOMENNA_PAIKOIN_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE,    // 5: period+inPlaces
      SISAMAASSA_PAIKOIN_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE,  // 6: area+inPlaces
      HUOMENNA_SISAMAASSA_PAIKOIN_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE, // 7
      HEIKKOA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE,    // 8: intensity
      HUOMENNA_HEIKKOA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE,    // 9: period+intensity
      SISAMAASSA_HEIKKOA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE,  // 10: area+intensity
      HUOMENNA_SISAMAASSA_HEIKKOA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE, // 11
      PAIKOIN_HEIKKOA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE,     // 12: inPlaces+intensity
      HUOMENNA_PAIKOIN_HEIKKOA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE,    // 13
      SISAMAASSA_PAIKOIN_HEIKKOA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE,  // 14
      HUOMENNA_SISAMAASSA_PAIKOIN_HEIKKOA_SADETTA_JOKA_VOI_OLLA_JAATAVAA_COMPOSITE_PHRASE, // 15
  }};
  return buildRegularSentence(ctx, a);
}

Sentence PrecipitationForecast::parseFreezingShowersPhrase(const FinalSentenceContext& ctx) const
{
  static const RegularSentenceArgs a = {{
      SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE,            // 0
      HUOMENNA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE,   // 1: period
      SISAMAASSA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE, // 2: area
      HUOMENNA_SISAMAASSA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE, // 3
      PAIKOIN_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE,    // 4: inPlaces
      HUOMENNA_PAIKOIN_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE,    // 5
      SISAMAASSA_PAIKOIN_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE,  // 6
      HUOMENNA_SISAMAASSA_PAIKOIN_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE, // 7
      HEIKKOJA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE,             // 8: intensity
      HUOMENNA_HEIKKOJA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE,   // 9
      SISAMAASSA_HEIKKOJA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE, // 10
      HUOMENNA_SISAMAASSA_HEIKKOJA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE, // 11
      PAIKOIN_HEIKKOJA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE,             // 12
      HUOMENNA_PAIKOIN_HEIKKOJA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE,    // 13
      SISAMAASSA_PAIKOIN_HEIKKOJA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE,  // 14
      HUOMENNA_SISAMAASSA_PAIKOIN_HEIKKOJA_SADEKUUROJA_JOTKA_VOIVAT_OLLA_JAATAVIA_COMPOSITE_PHRASE, // 15
  }};
  return buildRegularSentence(ctx, a);
}

Sentence PrecipitationForecast::parseRegularPrecipitationPhrase(
    const FinalSentenceContext& ctx) const
{
  static const RegularSentenceArgs a = {{
      nullptr,                                               // 0: no phrase, just precipitation
      HUOMENNA_SADETTA_COMPOSITE_PHRASE,                    // 1: period
      SISAMAASSA_SADETTA_COMPOSITE_PHRASE,                  // 2: area
      HUOMENNA_SISAMAASSA_SADETTA_COMPOSITE_PHRASE,         // 3: period+area
      PAIKOIN_SADETTA_COMPOSITE_PHRASE,                     // 4: inPlaces
      HUOMENNA_PAIKOIN_SADETTA_COMPOSITE_PHRASE,            // 5: period+inPlaces
      SISAMAASSA_PAIKOIN_SADETTA_COMPOSITE_PHRASE,          // 6: area+inPlaces
      HUOMENNA_SISAMAASSA_PAIKOIN_SADETTA_COMPOSITE_PHRASE, // 7: period+area+inPlaces
      HEIKKOA_SADETTA_COMPOSITE_PHRASE,                     // 8: intensity
      HUOMENNA_HEIKKOA_SADETTA_COMPOSITE_PHRASE,            // 9: period+intensity
      SISAMAASSA_HEIKKOA_SADETTA_COMPOSITE_PHRASE,          // 10: area+intensity
      HUOMENNA_SISAMAASSA_HEIKKOA_SADETTA_COMPOSITE_PHRASE, // 11: period+area+intensity
      PAIKOIN_HEIKKOA_SADETTA_COMPOSITE_PHRASE,             // 12: inPlaces+intensity
      HUOMENNA_PAIKOIN_HEIKKOA_SADETTA_COMPOSITE_PHRASE,    // 13: period+inPlaces+intensity
      SISAMAASSA_PAIKOIN_HEIKKOA_SADETTA_COMPOSITE_PHRASE,  // 14: area+inPlaces+intensity
      HUOMENNA_SISAMAASSA_PAIKOIN_HEIKKOA_SADETTA_COMPOSITE_PHRASE, // 15: all
  }};
  return buildRegularSentence(ctx, a);
}

Sentence PrecipitationForecast::parseFinalSentence(
    map<string, Sentence>& theCompositePhraseElements,
    const Sentence& thePeriodPhrase,
    const std::string& theAreaPhrase) const
{
  const bool periodPhraseEmpty = thePeriodPhrase.empty();
  const bool areaPhraseEmpty = theAreaPhrase.empty() || theAreaPhrase == EMPTY_STRING;
  const bool inPlacesPhraseEmpty =
      theCompositePhraseElements.find(IN_PLACES_PARAMETER) == theCompositePhraseElements.end();
  const bool intensityEmpty =
      theCompositePhraseElements.find(INTENSITY_PARAMETER) == theCompositePhraseElements.end();

  Sentence periodPhrase;
  periodPhrase << (thePeriodPhrase.empty() ? EMPTY_STRING : "") << thePeriodPhrase;

  Sentence areaPhrase;
  areaPhrase << theAreaPhrase;

  Sentence inPlacesPhrase;
  if (!inPlacesPhraseEmpty)
    inPlacesPhrase << theCompositePhraseElements[IN_PLACES_PARAMETER];

  Sentence intensity;
  if (!intensityEmpty)
    intensity << theCompositePhraseElements[INTENSITY_PARAMETER];

  Sentence precipitation;
  if (theCompositePhraseElements.find(PRECIPITATION_PARAMETER) !=
      theCompositePhraseElements.end())
    precipitation << theCompositePhraseElements[PRECIPITATION_PARAMETER];

  FinalSentenceContext ctx{periodPhrase,
                           areaPhrase,
                           inPlacesPhrase,
                           intensity,
                           precipitation,
                           periodPhraseEmpty,
                           areaPhraseEmpty,
                           inPlacesPhraseEmpty,
                           intensityEmpty};

  Sentence sentence;

  if (theCompositePhraseElements.find(PLAIN_PRECIPITATION_PHRASE) !=
      theCompositePhraseElements.end())
  {
    sentence << parsePlainPrecipitationPhrase(
        ctx, theCompositePhraseElements[PLAIN_PRECIPITATION_PHRASE]);
  }
  else
  {
    theParameters.theLog << "periodPhrase: ";
    theParameters.theLog << periodPhrase;
    theParameters.theLog << "areaPhrase: ";
    theParameters.theLog << areaPhrase;
    theParameters.theLog << "intensity: ";
    theParameters.theLog << intensity;
    theParameters.theLog << "precipitation: ";
    theParameters.theLog << precipitation;
    theParameters.theLog << "inPlacesPhrase: ";
    theParameters.theLog << inPlacesPhrase;
    theParameters.theLog << "periodPhraseEmpty: ";
    theParameters.theLog << (periodPhraseEmpty ? "yes" : "no") << '\n';
    theParameters.theLog << "areaPhraseEmpty: ";
    theParameters.theLog << (areaPhraseEmpty ? "yes" : "no") << '\n';

    if (theCompositePhraseElements.find(
            HUOMENNA_SISAMAASSA_SAA_ON_ENIMMAKSEEN_POUTAINEN_COMPOSITE_PHRASE) !=
        theCompositePhraseElements.end())
    {
      if (periodPhraseEmpty && areaPhraseEmpty)
        sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE << Delimiter(COMMA_PUNCTUATION_MARK)
                 << precipitation;
      else if (periodPhraseEmpty)
        sentence << SISAMAASSA_SAA_ON_ENIMMAKSEEN_POUTAINEN_COMPOSITE_PHRASE << areaPhrase
                 << precipitation;
      else if (areaPhraseEmpty)
        sentence << HUOMENNA_SAA_ON_ENIMMAKSEEN_POUTAINEN_COMPOSITE_PHRASE << periodPhrase
                 << precipitation;
      else
        sentence << HUOMENNA_SISAMAASSA_SAA_ON_ENIMMAKSEEN_POUTAINEN_COMPOSITE_PHRASE
                 << periodPhrase << areaPhrase << precipitation;
    }
    else if (hasMostlyDryPhrase(theCompositePhraseElements))
    {
      sentence << parseMostlyDryPhrase(ctx, theCompositePhraseElements);
    }
    else if (theCompositePhraseElements.find(SAA_ON_POUTAINEN_PHRASE) !=
             theCompositePhraseElements.end())
    {
      sentence << parseDryPhrase(ctx);
    }
    else if (theCompositePhraseElements.find(JOKA_VOI_OLLA_JAATAVAA_PHRASE) !=
             theCompositePhraseElements.end())
    {
      sentence << parseFreezingRainPhrase(ctx);
    }
    else if (theCompositePhraseElements.find(JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE) !=
             theCompositePhraseElements.end())
    {
      sentence << parseFreezingShowersPhrase(ctx);
    }
    else
    {
      sentence << parseRegularPrecipitationPhrase(ctx);
    }
  }

  theParameters.theLog << "FINAL SENTENCE: ";
  theParameters.theLog << sentence;

  return sentence;
}

void PrecipitationForecast::appendHeavyPrecipitationSentence(
    const WeatherPeriod& thePeriod,
    const WeatherPeriod& heavyPrecipitationPeriod,
    precipitation_form_id precipitationForm,
    std::vector<std::pair<WeatherPeriod, Sentence>>& theAdditionalSentences) const
{
  if (get_period_length(heavyPrecipitationPeriod) < 2)
    return;

  part_of_the_day_id partOfTheDayId = get_part_of_the_day_id_large(heavyPrecipitationPeriod);
  time_phrase_format timePhraseFormat = AT_FORMAT;

  if (partOfTheDayId != MISSING_PART_OF_THE_DAY_ID)
  {
    timePhraseFormat = AT_FORMAT;
  }
  else if (heavyPrecipitationPeriod.localStartTime() == thePeriod.localStartTime())
  {
    partOfTheDayId = get_part_of_the_day_id(heavyPrecipitationPeriod.localEndTime());
    timePhraseFormat = TILL_FORMAT;
  }
  else if (heavyPrecipitationPeriod.localEndTime() == thePeriod.localEndTime())
  {
    partOfTheDayId = get_part_of_the_day_id(heavyPrecipitationPeriod.localStartTime());
    timePhraseFormat = FROM_FORMAT;
  }

  if (partOfTheDayId == MISSING_PART_OF_THE_DAY_ID)
    return;

  Sentence heavyPrecipitationSentence;
  const bool isSnow = (precipitationForm == SNOW_FORM || precipitationForm == SNOW_FREEZING_FORM);
  if (isSnow)
  {
    if (theCheckHeavyIntensityFlag == SHOWERS)
      heavyPrecipitationSentence << ILTAPAIVALLA_LUMIKUUROT_VOIVAT_OLLA_SAKEITA;
    else
      heavyPrecipitationSentence << ILTAPAIVALLA_LUMISADE_VOI_OLLA_SAKEAA;
  }
  else
  {
    if (theCheckHeavyIntensityFlag == SHOWERS)
      heavyPrecipitationSentence << ILTAPAIVALLA_KUUROT_VOIVAT_OLLA_VOIMAKKAITA;
    else
      heavyPrecipitationSentence << ILTAPAIVALLA_SADE_VOI_OLLA_RUNSASTA;
  }

  heavyPrecipitationSentence << getTimePhrase(partOfTheDayId, timePhraseFormat);
  theAdditionalSentences.emplace_back(heavyPrecipitationPeriod, heavyPrecipitationSentence);

  theParameters.theLog << "HEAVY PRECIPITATION PERIOD: "
                       << heavyPrecipitationPeriod.localStartTime() << "..."
                       << heavyPrecipitationPeriod.localEndTime() << " ("
                       << getTimePhrase(partOfTheDayId, timePhraseFormat) << ")\n";
}

Sentence PrecipitationForecast::buildSingleAreaPrecipitationSentence(
    const WeatherPeriod& thePeriod,
    const Sentence& thePeriodPhrase,
    unsigned short theForecastAreaId,
    const std::string& theAreaPhrase,
    const precipitation_data_vector& dataVector,
    std::vector<std::pair<WeatherPeriod, Sentence>>& theAdditionalSentences) const
{
  float precipitationIntensity = kFloatMissing;
  float precipitationAbsoluteMaxIntensity = kFloatMissing;
  float precipitationExtent = kFloatMissing;
  float precipitationFormWater = kFloatMissing;
  float precipitationFormDrizzle = kFloatMissing;
  float precipitationFormSleet = kFloatMissing;
  float precipitationFormSnow = kFloatMissing;
  float precipitationFormFreezingRain = kFloatMissing;
  float precipitationFormFreezingDrizzle = kFloatMissing;

  calculatePrecipitationParameters(thePeriod,
                                   dataVector,
                                   precipitationIntensity,
                                   precipitationAbsoluteMaxIntensity,
                                   precipitationExtent,
                                   precipitationFormWater,
                                   precipitationFormDrizzle,
                                   precipitationFormSleet,
                                   precipitationFormSnow,
                                   precipitationFormFreezingRain,
                                   precipitationFormFreezingDrizzle);

  theParameters.theLog << "Period: " << thePeriod.localStartTime() << "..."
                       << thePeriod.localEndTime() << '\n';
  theParameters.theLog << "Mean intensity (max+mean/2.0): " << precipitationIntensity << '\n';
  theParameters.theLog << "Maximum intensity: " << precipitationAbsoluteMaxIntensity << '\n';

  precipitation_type precipitationType(
      getPrecipitationType(thePeriod, theParameters.theForecastArea));
  unsigned int typeChangeIndex = getPrecipitationTypeChange(dataVector, thePeriod);
  precipitation_form_id precipitationForm =
      get_complete_precipitation_form(theParameters.theVariable,
                                      precipitationFormWater,
                                      precipitationFormDrizzle,
                                      precipitationFormSleet,
                                      precipitationFormSnow,
                                      precipitationFormFreezingRain,
                                      precipitationFormFreezingDrizzle);

  thePrecipitationFormBeforeDryPeriod = precipitationForm;

  map<string, Sentence> compositePhraseElements;
  selectPrecipitationSentence(thePeriod,
                              precipitationForm,
                              precipitationIntensity,
                              precipitationAbsoluteMaxIntensity,
                              precipitationExtent,
                              precipitationFormWater,
                              precipitationFormDrizzle,
                              precipitationFormSleet,
                              precipitationFormSnow,
                              precipitationFormFreezingRain,
                              precipitationFormFreezingDrizzle,
                              precipitationType,
                              dataVector.at(typeChangeIndex)->theObservationTime,
                              getPrecipitationTransformationId(thePeriod, theForecastAreaId),
                              compositePhraseElements);

  Sentence sentence;
  sentence << parseFinalSentence(compositePhraseElements, thePeriodPhrase, theAreaPhrase);

  bool dry_weather =
      is_dry_weather(theParameters, precipitationForm, precipitationIntensity, precipitationExtent);
  if (dry_weather)
    return sentence;

  Sentence thunderSentence;
  thunderSentence << getThunderSentence(thePeriod, theForecastAreaId, theParameters.theVariable);
  if (!thunderSentence.empty())
    sentence << thunderSentence;

  if (theCheckHeavyIntensityFlag != MISSING_PRECIPITATION_TYPE)
  {
    WeatherPeriod heavyPrecipitationPeriod(getHeavyPrecipitationPeriod(thePeriod, dataVector));
    appendHeavyPrecipitationSentence(
        thePeriod, heavyPrecipitationPeriod, precipitationForm, theAdditionalSentences);
  }

  return sentence;
}

Sentence PrecipitationForecast::constructPrecipitationSentence(
    const WeatherPeriod& thePeriod,
    const Sentence& thePeriodPhrase,
    unsigned short theForecastAreaId,
    const std::string& theAreaPhrase,
    std::vector<std::pair<WeatherPeriod, Sentence>>& theAdditionalSentences) const
{
  Sentence sentence;

  if (theForecastAreaId & INLAND_AREA && theForecastAreaId & COASTAL_AREA)
  {
    if (separateCoastInlandPrecipitation(thePeriod))
    {
      sentence << constructPrecipitationSentence(
          thePeriod, thePeriodPhrase, INLAND_AREA, INLAND_PHRASE, theAdditionalSentences);

      // ARE 22.02.2011: this is to prevent tautology e.g. sisamaassa moinin paikoin rantasadetta,
      // rannikolla monin paikoin vesisadetta
      get_in_places_phrase().preventTautology(true);
      sentence << Delimiter(COMMA_PUNCTUATION_MARK);
      sentence << constructPrecipitationSentence(
          thePeriod, thePeriodPhrase, COASTAL_AREA, COAST_PHRASE, theAdditionalSentences);
      get_in_places_phrase().preventTautology(false);
      return sentence;
    }
    return buildSingleAreaPrecipitationSentence(
        thePeriod, thePeriodPhrase, theForecastAreaId, theAreaPhrase, theFullData, theAdditionalSentences);
  }

  if (theForecastAreaId & INLAND_AREA)
    return buildSingleAreaPrecipitationSentence(
        thePeriod, thePeriodPhrase, theForecastAreaId, theAreaPhrase, theInlandData, theAdditionalSentences);

  if (theForecastAreaId & COASTAL_AREA)
    return buildSingleAreaPrecipitationSentence(
        thePeriod, thePeriodPhrase, theForecastAreaId, theAreaPhrase, theCoastalData, theAdditionalSentences);

  return sentence;
}

string PrecipitationForecast::getTimePhrase(part_of_the_day_id thePartOfTheDayId,
                                            time_phrase_format theTimePhraseFormat)
{
  // Each entry: {from_phrase, till_phrase, at_phrase}
  using T3 = std::array<const char*, 3>;
  static const std::map<part_of_the_day_id, T3> table = {
      {AAMU, {AAMUSTA_ALKAEN_PHRASE, AAMUUN_ASTI_PHRASE, AAMULLA_WORD}},
      {AAMUPAIVA, {AAMUPAIVASTA_ALKAEN_PHRASE, AAMUPAIVAAN_ASTI_PHRASE, AAMUPAIVALLA_WORD}},
      {KESKIPAIVA, {ILTAPAIVASTA_ALKAEN_PHRASE, ILTAPAIVAAN_ASTI_PHRASE, ILTAPAIVALLA_WORD}},
      {ILTAPAIVA, {ILTAPAIVASTA_ALKAEN_PHRASE, ILTAPAIVAAN_ASTI_PHRASE, ILTAPAIVALLA_WORD}},
      {ILTA, {ILLASTA_ALKAEN_PHRASE, ILTAAN_ASTI_PHRASE, ILLALLA_WORD}},
      {ILTAYO, {ILTAYOSTA_ALKAEN_PHRASE, ILTAYOHON_ASTI_PHRASE, ILTAYOLLA_WORD}},
      {KESKIYO, {KESKIYOSTA_ALKAEN_PHRASE, KESKIYOHON_ASTI_PHRASE, KESKIYOLLA_WORD}},
      {AAMUYO, {AAMUYOSTA_ALKAEN_PHRASE, AAMUYOHON_ASTI_PHRASE, AAMUYOLLA_WORD}},
      {PAIVA, {nullptr, nullptr, PAIVALLA_WORD}},
      {YO, {nullptr, nullptr, YOLLA_WORD}},
      {AAMU_JA_AAMUPAIVA,
       {AAMUSTA_ALKAEN_PHRASE, AAMUPAIVAAN_ASTI_PHRASE, AAMULLA_JA_AAMUPAIVALLA_PHRASE}},
      {ILTAPAIVA_JA_ILTA,
       {ILTAPAIVASTA_ALKAEN_PHRASE, ILTAAN_ASTI_PHRASE, ILTAPAIVALLA_JA_ILLALLA_PHRASE}},
      {ILTA_JA_ILTAYO,
       {ILLASTA_ALKAEN_PHRASE, ILTAYOHON_ASTI_PHRASE, ILLALLA_JA_ILTAYOLLA_PHRASE}},
      {ILTAYO_JA_KESKIYO,
       {ILTAYOSTA_ALKAEN_PHRASE, KESKIYOHON_ASTI_PHRASE, ILTAYOLLA_JA_KESKIYOLLA_PHRASE}},
      {KESKIYO_JA_AAMUYO,
       {KESKIYOSTA_ALKAEN_PHRASE, AAMUYOHON_ASTI_PHRASE, KESKIYOLLA_JA_AAMUYOLLA_PHRASE}},
      {AAMUYO_JA_AAMU,
       {AAMUYOSTA_ALKAEN_PHRASE, AAMUUN_ASTI_PHRASE, AAMUYOLLA_JA_AAMULLA_PHRASE}},
  };

  auto it = table.find(thePartOfTheDayId);
  if (it != table.end())
  {
    const T3& phrases = it->second;
    if (theTimePhraseFormat == FROM_FORMAT && phrases[0])
      return phrases[0];
    if (theTimePhraseFormat == TILL_FORMAT && phrases[1])
      return phrases[1];
    if (theTimePhraseFormat == AT_FORMAT && phrases[2])
      return phrases[2];
  }

  return "missing part of the day";
}

bool PrecipitationForecast::shortTermPrecipitationExists(const WeatherPeriod& thePeriod) const
{
  const vector<WeatherPeriod>* precipitationPeriodVector = nullptr;

  if (theParameters.theForecastArea & FULL_AREA)
    precipitationPeriodVector = &thePrecipitationPeriodsFull;
  else if (theParameters.theForecastArea & COASTAL_AREA)
    precipitationPeriodVector = &thePrecipitationPeriodsCoastal;
  else if (theParameters.theForecastArea & INLAND_AREA)
    precipitationPeriodVector = &thePrecipitationPeriodsInland;

  if (precipitationPeriodVector)
  {
    for (const auto& i : *precipitationPeriodVector)
    {
      const TextGenPosixTime& startTime = i.localStartTime();
      const TextGenPosixTime& endTime = i.localEndTime();

      if (endTime.DifferenceInHours(startTime) < 6 && is_inside(startTime, thePeriod) &&
          is_inside(endTime, thePeriod))
      {
        return true;
      }
    }
  }

  return false;
}

Sentence PrecipitationForecast::shortTermPrecipitationSentence(
    const WeatherPeriod& thePeriod, const Sentence& thePeriodPhrase) const
{
  Sentence sentence;

  std::vector<std::pair<WeatherPeriod, Sentence>> theAdditionalSentences;

  sentence << constructPrecipitationSentence(thePeriod,
                                             thePeriodPhrase,
                                             theParameters.theForecastArea,
                                             EMPTY_STRING,
                                             theAdditionalSentences);

  theParameters.theLog << "Short term precipitation sentence: ";
  theParameters.theLog << sentence;

  return sentence;
}

Sentence PrecipitationForecast::precipitationSentence(
    const WeatherPeriod& thePeriod,
    const Sentence& thePeriodPhrase,
    std::vector<std::pair<WeatherPeriod, Sentence>>& theAdditionalSentences) const
{
  Sentence sentence;

  sentence << constructPrecipitationSentence(thePeriod,
                                             thePeriodPhrase,
                                             theParameters.theForecastArea,
                                             EMPTY_STRING,
                                             theAdditionalSentences);

  return sentence;
}

Sentence PrecipitationForecast::areaSpecificSentence(const WeatherPeriod& thePeriod) const
{
  Sentence sentence;

  // If the area contains both coast and inland, we don't use area specific sentence
  if (theParameters.theForecastArea & FULL_AREA)
    return sentence;

  float north(0.0);
  float south(0.0);
  float east(0.0);
  float west(0.0);
  float northEast(0.0);
  float southEast(0.0);
  float southWest(0.0);
  float northWest(0.0);

  getPrecipitationDistribution(
      thePeriod, north, south, east, west, northEast, southEast, southWest, northWest);

  area_specific_sentence_id sentenceId = get_area_specific_sentence_id(
      north, south, east, west, northEast, southEast, southWest, northWest);

  Rect areaRect(theParameters.theArea);
  std::unique_ptr<NFmiArea> mercatorArea(
      NFmiAreaTools::CreateLegacyMercatorArea(areaRect.getBottomLeft(), areaRect.getTopRight()));

  float areaHeightWidthRatio =
      mercatorArea->WorldRect().Height() / mercatorArea->WorldRect().Width();

  Sentence areaSpecificSentence;
  areaSpecificSentence << area_specific_sentence(
      north, south, east, west, northEast, southEast, southWest, northWest);

  // If the area is too cigar-shaped, we can use only north-south/east-west specifier
  if ((areaHeightWidthRatio >= 0.6 && areaHeightWidthRatio <= 1.5) ||
      (areaHeightWidthRatio < 0.6 &&
       (sentenceId == ALUEEN_ITAOSASSA || sentenceId == ALUEEN_LANSIOSASSA ||
        sentenceId == ENIMMAKSEEN_ALUEEN_ITAOSASSA ||
        sentenceId == ENIMMAKSEEN_ALUEEN_LANSIOSASSA)) ||
      (areaHeightWidthRatio > 1.5 &&
       (sentenceId == ALUEEN_POHJOISOSASSA || sentenceId == ALUEEN_ETELAOSASSA ||
        sentenceId == ENIMMAKSEEN_ALUEEN_POHJOISOSASSA ||
        sentenceId == ENIMMAKSEEN_ALUEEN_ETELAOSASSA)))
  {
    sentence << areaSpecificSentence;
  }

  return sentence;
}

void PrecipitationForecast::getWeatherEventIdVector(
    weather_event_id_vector& thePrecipitationWeatherEvents) const
{
  const weather_event_id_vector* vectorToRefer = nullptr;

  if (theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
    vectorToRefer = &thePrecipitationWeatherEventsFull;
  else if (theParameters.theForecastArea & COASTAL_AREA)
    vectorToRefer = &thePrecipitationWeatherEventsCoastal;
  else if (theParameters.theForecastArea & INLAND_AREA)
    vectorToRefer = &thePrecipitationWeatherEventsInland;

  if (vectorToRefer)
    thePrecipitationWeatherEvents = *vectorToRefer;
}

Rect PrecipitationForecast::getPrecipitationRect(const TextGenPosixTime& theTimestamp,
                                                 float theLowerLimit,
                                                 float theUpperLimit) const
{
  NFmiIndexMask indexMask;
  RangeAcceptor precipitationlimits;
  precipitationlimits.lowerLimit(theLowerLimit);
  precipitationlimits.upperLimit(theUpperLimit);

  // precipitation in the beginning
  ExtractMask(theParameters.theSources,
              Precipitation,
              theParameters.theArea,
              WeatherPeriod(theTimestamp, theTimestamp),
              precipitationlimits,
              indexMask);

  return {theParameters.theSources, Precipitation, indexMask};
}

direction_id PrecipitationForecast::getPrecipitationLeavingDirection(
    const WeatherPeriod& thePeriod) const
{
  direction_id retval = NO_DIRECTION;

  TextGenPosixTime startTime(thePeriod.localEndTime());
  TextGenPosixTime endTime(thePeriod.localEndTime());
  endTime.ChangeByHours(1);
  startTime.ChangeByHours(1);
  WeatherPeriod checkPeriod(startTime, endTime);

  unsigned int startIndex = 0;
  unsigned int endIndex = 0;
  weather_result_data_item_vector* northeast_data =
      get_data_vector(theParameters, PRECIPITATION_NORTHEAST_SHARE_DATA);
  weather_result_data_item_vector* southeast_data =
      get_data_vector(theParameters, PRECIPITATION_SOUTHEAST_SHARE_DATA);
  weather_result_data_item_vector* southwest_data =
      get_data_vector(theParameters, PRECIPITATION_SOUTHWEST_SHARE_DATA);
  weather_result_data_item_vector* northwest_data =
      get_data_vector(theParameters, PRECIPITATION_NORTHWEST_SHARE_DATA);

  if (!get_period_start_end_index(checkPeriod, *northeast_data, startIndex, endIndex))
    return retval;

  float northeast = northeast_data->at(startIndex)->theResult.value();
  float southeast = southeast_data->at(startIndex)->theResult.value();
  float southwest = southwest_data->at(startIndex)->theResult.value();
  float northwest = northwest_data->at(startIndex)->theResult.value();
  float north = northeast + northwest;
  float south = southeast + southwest;
  float east = northeast + southeast;
  float west = southwest + northwest;

  if (north >= 80)
    retval = SOUTH;
  else if (south >= 80)
    retval = NORTH;
  else if (east >= 80)
    retval = WEST;
  else if (west >= 80)
    retval = EAST;

  /*
  Rect areaRect(theParameters.theArea);
  Rect precipitationRect(getPrecipitationRect(thePeriod.localEndTime(), 0.0, 0.02));

  retval = getDirection(areaRect, precipitationRect);

  */

  return retval;
}

direction_id PrecipitationForecast::getPrecipitationArrivalDirection(
    const WeatherPeriod& thePeriod) const
{
  direction_id retval(NO_DIRECTION);

  TextGenPosixTime startTime = thePeriod.localStartTime();
  startTime.ChangeByHours(-1);
  const TextGenPosixTime& endTime = thePeriod.localStartTime();
  WeatherPeriod checkPeriod(startTime, endTime);

  unsigned int startIndex;
  unsigned int endIndex;
  weather_result_data_item_vector* northeast_data =
      get_data_vector(theParameters, PRECIPITATION_NORTHEAST_SHARE_DATA);
  weather_result_data_item_vector* southeast_data =
      get_data_vector(theParameters, PRECIPITATION_SOUTHEAST_SHARE_DATA);
  weather_result_data_item_vector* southwest_data =
      get_data_vector(theParameters, PRECIPITATION_SOUTHWEST_SHARE_DATA);
  weather_result_data_item_vector* northwest_data =
      get_data_vector(theParameters, PRECIPITATION_NORTHWEST_SHARE_DATA);

  if (!get_period_start_end_index(checkPeriod, *northeast_data, startIndex, endIndex))
    return retval;

  float northeast = northeast_data->at(startIndex)->theResult.value();
  float southeast = southeast_data->at(startIndex)->theResult.value();
  float southwest = southwest_data->at(startIndex)->theResult.value();
  float northwest = northwest_data->at(startIndex)->theResult.value();
  float north = northeast + northwest;
  float south = southeast + southwest;
  float east = northeast + southeast;
  float west = southwest + northwest;

  if (northeast >= 80)
    retval = NORTHEAST;
  else if (southeast >= 80)
    retval = SOUTHEAST;
  else if (southwest >= 80)
    retval = SOUTHWEST;
  else if (northwest >= 80)
    retval = NORTHWEST;
  else if (north >= 80)
    retval = NORTH;
  else if (south >= 80)
    retval = SOUTH;
  else if (east >= 80)
    retval = EAST;
  else if (west >= 80)
    retval = WEST;

  /*
  Rect areaRect(theParameters.theArea);
  Rect precipitationRect(getPrecipitationRect(thePeriod.localStartTime(), 0.02, 100.0));

  retval = getDirection(areaRect, precipitationRect);

  if(retval != NO_DIRECTION)
    {
          theParameters.theLog << "Whole area: " << (string)areaRect << '\n';
          theParameters.theLog << "Precipitation area: " << (string)precipitationRect << '\n';
          theParameters.theLog << "West: " <<	(string)areaRect.subRect(WEST) << '\n';
          theParameters.theLog << "East: " <<	(string)areaRect.subRect(EAST) << '\n';

          theParameters.theLog << "Forecast period: "
                                                   <<
  theParameters.theForecastPeriod.localStartTime()
                                                   << " ... "
                                                   << theParameters.theForecastPeriod.localEndTime()
                                                   << '\n';
          theParameters.theLog << "Rain direction check period: "
                                                   << thePeriod.localStartTime()
                                                   << " ... "
                                                   << thePeriod.localStartTime()
                                                   << '\n';
          theParameters.theLog << getDirectionString(retval) << '\n';
    }
  */
  return retval;
}

NFmiPoint PrecipitationForecast::getPrecipitationRepresentativePoint(
    const WeatherPeriod& thePeriod) const
{
  NFmiPoint retval(kFloatMissing, kFloatMissing);

  unsigned int startIndex;
  unsigned int endIndex;
  weather_result_data_item_vector* coordinates =
      get_data_vector(theParameters, PRECIPITATION_POINT_DATA);
  if (!get_period_start_end_index(thePeriod, *coordinates, startIndex, endIndex))
    return retval;

  float lon = 0.0;
  float lat = 0.0;
  unsigned int count = 0;
  for (unsigned int i = startIndex; i <= endIndex; i++)
  {
    if (coordinates->at(i)->theResult.value() != kFloatMissing)
    {
      lon += coordinates->at(i)->theResult.value();
      lat += coordinates->at(i)->theResult.error();
      count++;
    }
  }

  if (count > 0)
  {
    retval.X(lon / count);
    retval.Y(lat / count);
  }

  return retval;
}

precipitation_traverse_id PrecipitationForecast::getPrecipitationTraverseId(
    const WeatherPeriod& thePeriod) const
{
  precipitation_traverse_id retval(MISSING_TRAVERSE_ID);

  const weather_result_data_item_vector* dataVector =
      get_data_vector(theParameters, PRECIPITATION_MEAN_DATA);

  unsigned int startIndex;
  unsigned int endIndex;

  if (!get_period_start_end_index(thePeriod, *dataVector, startIndex, endIndex))
    return retval;

  NFmiIndexMask begIndexMask;
  NFmiIndexMask endIndexMask;
  RangeAcceptor precipitationlimits;
  precipitationlimits.lowerLimit(0.02);

  ExtractMask(theParameters.theSources,
              Precipitation,
              theParameters.theArea,
              WeatherPeriod(thePeriod.localStartTime(), thePeriod.localStartTime()),
              precipitationlimits,
              begIndexMask);
  ExtractMask(theParameters.theSources,
              Precipitation,
              theParameters.theArea,
              WeatherPeriod(thePeriod.localEndTime(), thePeriod.localEndTime()),
              precipitationlimits,
              endIndexMask);

  Rect areaRect(theParameters.theArea);
  Rect begRect(theParameters.theSources, Precipitation, begIndexMask);
  Rect endRect(theParameters.theSources, Precipitation, endIndexMask);

  direction_id begDirection = getDirection(areaRect, begRect);
  direction_id endDirection = getDirection(areaRect, endRect);

  weather_result_data_item_vector* coordinates =
      get_data_vector(theParameters, PRECIPITATION_POINT_DATA);
  get_period_start_end_index(thePeriod, *coordinates, startIndex, endIndex);

  double pearson_co_lon = get_pearson_coefficient(*coordinates, startIndex, endIndex, false);
  double pearson_co_lat = get_pearson_coefficient(*coordinates, startIndex, endIndex, true);

  if (begDirection == EAST && endDirection == WEST && pearson_co_lon <= -0.85)
  {
    retval = FROM_EAST_TO_WEST;
  }
  else if (begDirection == WEST && endDirection == EAST && pearson_co_lon >= 0.85)
  {
    retval = FROM_WEST_TO_EAST;
  }
  else if (begDirection == SOUTH && endDirection == NORTH && pearson_co_lat >= 0.85)
  {
    retval = FROM_SOUTH_TO_NORTH;
  }
  else if (begDirection == NORTH && endDirection == SOUTH && pearson_co_lat <= -0.85)
  {
    retval = FROM_NORTH_TO_SOUTH;
  }
  else if (begDirection == NORTHEAST && endDirection == SOUTHWEST && pearson_co_lon <= -0.85 &&
           pearson_co_lat <= -0.85)
  {
    retval = FROM_NORTHEAST_TO_SOUTHWEST;
  }
  else if (begDirection == SOUTHWEST && endDirection == NORTHEAST && pearson_co_lon >= 0.85 &&
           pearson_co_lat >= 0.85)
  {
    retval = FROM_SOUTHWEST_TO_NORTHEAST;
  }
  else if (begDirection == NORTHWEST && endDirection == SOUTHEAST && pearson_co_lon >= 0.85 &&
           pearson_co_lat <= -0.85)
  {
    retval = FROM_NORTHWEST_TO_SOUTHEAST;
  }
  else if (begDirection == SOUTHEAST && endDirection == NORTHWEST && pearson_co_lon <= -0.85 &&
           pearson_co_lat >= 0.85)
  {
    retval = FROM_SOUTHEAST_TO_NORTHWEST;
  }

  if (retval != MISSING_TRAVERSE_ID)
  {
    theParameters.theLog << "Sadealue kulkee " << precipitation_traverse_string(retval) << '\n';
  }

  return retval;
}

const precipitation_data_vector& PrecipitationForecast::getPrecipitationDataVector(
    const unsigned short theForecastArea) const
{
  if (theForecastArea & INLAND_AREA && theForecastArea & COASTAL_AREA)
  {
    return theFullData;
  }
  if (theForecastArea & COASTAL_AREA)
  {
    return theCoastalData;
  }

  return theInlandData;
}

PrecipitationForecast::PrecipitationForecast(wf_story_params& parameters)
    : theParameters(parameters)
{
  gatherPrecipitationData();
  findOutPrecipitationPeriods();
  findOutPrecipitationWeatherEvents();
}

PrecipitationForecast::~PrecipitationForecast()
{
  theCoastalData.clear();
  theInlandData.clear();
  theFullData.clear();
}

bool PrecipitationForecast::singleForm(precipitation_form_id thePrecipitationForm)
{
  return thePrecipitationForm == WATER_FORM || thePrecipitationForm == WATER_FREEZING_FORM ||
         thePrecipitationForm == DRIZZLE_FORM || thePrecipitationForm == DRIZZLE_FREEZING_FORM ||
         thePrecipitationForm == SLEET_FORM || thePrecipitationForm == SNOW_FORM ||
         thePrecipitationForm == FREEZING_FORM ||
         thePrecipitationForm == WATER_DRIZZLE_FREEZING_FORM ||
         thePrecipitationForm == WATER_DRIZZLE_FORM ||
         thePrecipitationForm == MISSING_PRECIPITATION_FORM;
}

// this function is set up because of language versions, for eample in swedish: regnet/snöfallet
// upphör
precipitation_form_id PrecipitationForecast::getPoutaantuuPrecipitationForm() const
{
  precipitation_form_id retval(MISSING_PRECIPITATION_FORM);

  switch (thePrecipitationFormBeforeDryPeriod)
  {
    case MISSING_PRECIPITATION_FORM:
    case WATER_SLEET_SNOW_FORM:
    case WATER_SNOW_FREEZING_FORM:
    case WATER_SNOW_FORM:
    case WATER_DRIZZLE_SNOW_FORM:
    case WATER_SLEET_FREEZING_FORM:
    case WATER_SLEET_FORM:
    case WATER_DRIZZLE_SLEET_FORM:
    case WATER_DRIZZLE_FREEZING_FORM:
    case WATER_DRIZZLE_FORM:
    case WATER_FREEZING_FORM:
    case FREEZING_FORM:
    case WATER_FORM:
      retval = WATER_FORM;
      break;
    case SLEET_SNOW_FREEZING_FORM:
    case SLEET_SNOW_FORM:
    case SLEET_FREEZING_FORM:
    case SLEET_FORM:
      retval = SLEET_FORM;
      break;
    case SNOW_FORM:
    case SNOW_FREEZING_FORM:
      retval = SNOW_FORM;
      break;
    case DRIZZLE_SNOW_FREEZING_FORM:
    case DRIZZLE_SNOW_FORM:
    case DRIZZLE_SLEET_SNOW_FORM:
    case DRIZZLE_FORM:
    case DRIZZLE_FREEZING_FORM:
    case DRIZZLE_SLEET_FORM:
    case DRIZZLE_SLEET_FREEZING_FORM:
      retval = DRIZZLE_FORM;
      break;
  }

  return retval;
}

Sentence PrecipitationForecast::getThunderSentence(const WeatherPeriod& thePeriod,
                                                   unsigned short theForecastAreaId,
                                                   const string& theVariable) const
{
  Sentence thunderSentence;

  forecast_area_id theAreaId = NO_AREA;

  if (theForecastAreaId & FULL_AREA)
    theAreaId = FULL_AREA;
  else if (theForecastAreaId & INLAND_AREA)
    theAreaId = INLAND_AREA;
  else if (theForecastAreaId & COASTAL_AREA)
    theAreaId = COASTAL_AREA;

  thunderSentence << theParameters.theThunderForecast->thunderSentence(
      thePeriod, theAreaId, theVariable);

  return thunderSentence;
}

bool PrecipitationForecast::thunderExists(const WeatherPeriod& thePeriod,
                                          unsigned short theForecastAreaId,
                                          const string& theVariable) const
{
  Sentence thunderSentence;

  thunderSentence << getThunderSentence(thePeriod, theForecastAreaId, theVariable);

  return (!thunderSentence.empty());
}

// return hours of intensityId during forecast period
unsigned int PrecipitationForecast::getPrecipitationHours(precipitation_intesity_id intensityId,
                                                          const WeatherPeriod& period) const
{
  const precipitation_data_vector& dataVector =
      getPrecipitationDataVector(theParameters.theForecastArea);

  std::map<precipitation_intesity_id, unsigned int> hours;
  hours.insert(make_pair(DRY_WEATHER, 0));
  hours.insert(make_pair(WEAK_PRECIPITATION, 0));
  hours.insert(make_pair(MODERATE_PRECIPITATION, 0));
  hours.insert(make_pair(HEAVY_PRECIPITATION, 0));
  hours.insert(make_pair(MISSING_INTENSITY_ID, 0));
  for (const auto& i : dataVector)
  {
    if (i->theObservationTime >= period.localStartTime() &&
        i->theObservationTime <= period.localEndTime())
    {
      precipitation_intesity_id intensityId = get_precipitation_intensity_id(
          i->thePrecipitationForm, i->thePrecipitationMaxIntensity, theParameters);
      (hours[intensityId])++;
    }
  }

  return hours[intensityId];
}

}  // namespace TextGen

// ======================================================================
