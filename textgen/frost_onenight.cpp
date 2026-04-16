// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::FrostStory::one night
 */
// ======================================================================

#include "AreaTools.h"
#include "ComparativeAcceptor.h"
#include "Delimiter.h"
#include "FrostStory.h"
#include "FrostStoryTools.h"
#include "Integer.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "PositiveValueAcceptor.h"
#include "SeasonTools.h"
#include "Sentence.h"
#include "UnitFactory.h"
#include "WeatherForecast.h"
#include <calculator/GridForecaster.h>
#include <calculator/HourPeriodGenerator.h>
#include <calculator/MathTools.h>
#include <calculator/RangeAcceptor.h>
#include <calculator/Settings.h>
#include <calculator/WeatherPeriodTools.h>
#include <calculator/WeatherResult.h>
#include <macgyver/Exception.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <newbase/NFmiStringTools.h>
#include <iomanip>
#include <map>
#define BOOST_FILESYSTEM_VERSION 3
#include <filesystem>  // includes all needed Boost.Filesystem declarations
namespace boostfs = std::filesystem;

using namespace std;

namespace TextGen
{
namespace FrostOnenight
{
using namespace TextGen;
using namespace AreaTools;
using namespace SeasonTools;

#define HALLAN_VAARA_COMPOSITE_PHRASE "[sisamaassa] hallan vaara"
#define ALAVILLA_MAILLA_HALLAN_VAARA_COMPOSITE_PHRASE "[sisamaassa] alavilla mailla hallan vaara"
#define MAHDOLLISESTI_HALLAA_COMPOSITE_PHRASE "[rannikolla] mahdollisesti hallaa"
#define PAIKOIN_HALLAA_COMPOSITE_PHRASE "[rannikolla] [paikoin] hallaa"
#define PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE \
  "[rannikolla] [paikoin] hallaa, joka voi olla ankaraa"

namespace
{
enum frost_category
{
  CAT_NA,
  CAT_0010,
  CAT_1020,
  CAT_3040,
  CAT_5060,
  CAT_7080,
  CAT_90100,
  CAT_FROST
};

enum frost_phrase_id
{
  EMPTY_STORY,
  ALAVILLA_MAILLA_HALLAN_VAARA,
  MAHDOLLISESTI_HALLAA,
  PAIKOIN_HALLAA,
  MONIN_PAIKOIN_HALLAA,
  HALLAA_YLEISESTI,
  YOPAKKASTA,
  RANNIKOLLA_HALLAN_VAARA,
  RANNIKOLLA_MAHDOLLISESTI_HALLAA,
  RANNIKOLLA_PAIKOIN_HALLAA,
  RANNIKOLLA_MONIN_PAIKOIN_HALLAA,
  RANNIKOLLA_HALLAA,
  SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA,
  SISAMAASSA_MAHDOLLISESTI_HALLAA,
  SISAMAASSA_PAIKOIN_HALLAA,
  SISAMAASSA_MONIN_PAIKOIN_HALLAA,
  SISAMAASSA_YLEISESTI_HALLAA
};

frost_category get_frost_category(const double& frostProbability,
                                  const unsigned short& forecast_areas,
                                  const unsigned short& growing_season_started,
                                  const unsigned short& night_frost,
                                  const forecast_area_id& area_id)
{
  // area not included or growing season not yet started
  if (!(forecast_areas & area_id) || !(growing_season_started & area_id))
    return CAT_NA;
  if (night_frost & area_id)  // night frost
    return CAT_FROST;

  if (frostProbability >= 0.0 && frostProbability < 10.0)
    return CAT_0010;
  if (frostProbability >= 10.0 && frostProbability < 25.0)
    return CAT_1020;
  if (frostProbability >= 25.0 && frostProbability < 45.0)
    return CAT_3040;
  if (frostProbability >= 45.0 && frostProbability < 65.0)
    return CAT_5060;
  if (frostProbability >= 65.0 && frostProbability < 85.0)
    return CAT_7080;
  if (frostProbability >= 85.0 && frostProbability <= 100.0)
    return CAT_90100;
  return CAT_NA;
}

int get_frost_onenight_phrase_id(const double& coastalFrostProbability,
                                 const double& inlandFrostProbability,
                                 const unsigned short& forecast_areas,
                                 const unsigned short& growing_season_started,
                                 const unsigned short& night_frost)
{
  frost_category categoryCoastal = get_frost_category(
      coastalFrostProbability, forecast_areas, growing_season_started, night_frost, COASTAL_AREA);
  frost_category categoryInland = get_frost_category(
      inlandFrostProbability, forecast_areas, growing_season_started, night_frost, INLAND_AREA);

  // if coastal or inland area is missing, read story from story selection table diagonal
  if (!(forecast_areas & COASTAL_AREA))
    categoryCoastal = categoryInland;
  if (!(forecast_areas & INLAND_AREA))
    categoryInland = categoryCoastal;

  // Lookup table: rows = coastal category [NA,0010,1020,3040,5060,7080,90100,FROST]
  //               cols = inland category  [NA,0010,1020,3040,5060,7080,90100,FROST]
  // Values are phrase IDs (simple) or composite (primary*100+secondary)
  static const int table[8][8] = {
      // NA                    0010                   1020                        3040 5060 7080
      // 90100                               FROST
      {EMPTY_STORY,
       EMPTY_STORY,
       ALAVILLA_MAILLA_HALLAN_VAARA,
       MAHDOLLISESTI_HALLAA,
       PAIKOIN_HALLAA,
       MONIN_PAIKOIN_HALLAA,
       HALLAA_YLEISESTI,
       EMPTY_STORY},  // CAT_NA
      {EMPTY_STORY,
       EMPTY_STORY,
       ALAVILLA_MAILLA_HALLAN_VAARA,
       MAHDOLLISESTI_HALLAA,
       SISAMAASSA_PAIKOIN_HALLAA,
       SISAMAASSA_MONIN_PAIKOIN_HALLAA,
       SISAMAASSA_YLEISESTI_HALLAA,
       EMPTY_STORY},  // CAT_0010
      {RANNIKOLLA_MAHDOLLISESTI_HALLAA,
       ALAVILLA_MAILLA_HALLAN_VAARA,
       ALAVILLA_MAILLA_HALLAN_VAARA,
       MAHDOLLISESTI_HALLAA,
       PAIKOIN_HALLAA,
       SISAMAASSA_MONIN_PAIKOIN_HALLAA,
       SISAMAASSA_YLEISESTI_HALLAA,
       EMPTY_STORY},  // CAT_1020
      {RANNIKOLLA_MAHDOLLISESTI_HALLAA,
       RANNIKOLLA_MAHDOLLISESTI_HALLAA,
       MAHDOLLISESTI_HALLAA,
       MAHDOLLISESTI_HALLAA,
       PAIKOIN_HALLAA,
       (SISAMAASSA_MONIN_PAIKOIN_HALLAA * 100) + RANNIKOLLA_MAHDOLLISESTI_HALLAA,
       (SISAMAASSA_YLEISESTI_HALLAA * 100) + RANNIKOLLA_MAHDOLLISESTI_HALLAA,
       EMPTY_STORY},  // CAT_3040
      {RANNIKOLLA_PAIKOIN_HALLAA,
       RANNIKOLLA_PAIKOIN_HALLAA,
       PAIKOIN_HALLAA,
       PAIKOIN_HALLAA,
       PAIKOIN_HALLAA,
       MONIN_PAIKOIN_HALLAA,
       HALLAA_YLEISESTI,
       EMPTY_STORY},  // CAT_5060
      {RANNIKOLLA_MONIN_PAIKOIN_HALLAA,
       RANNIKOLLA_MONIN_PAIKOIN_HALLAA,
       (SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA * 100) + RANNIKOLLA_MONIN_PAIKOIN_HALLAA,
       (RANNIKOLLA_MONIN_PAIKOIN_HALLAA * 100) + SISAMAASSA_MAHDOLLISESTI_HALLAA,
       PAIKOIN_HALLAA,
       MONIN_PAIKOIN_HALLAA,
       HALLAA_YLEISESTI,
       EMPTY_STORY},  // CAT_7080
      {RANNIKOLLA_HALLAA,
       RANNIKOLLA_HALLAA,
       (SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA * 100) + RANNIKOLLA_HALLAA,
       (RANNIKOLLA_HALLAA * 100) + SISAMAASSA_MAHDOLLISESTI_HALLAA,
       (RANNIKOLLA_HALLAA * 100) + SISAMAASSA_PAIKOIN_HALLAA,
       HALLAA_YLEISESTI,
       HALLAA_YLEISESTI,
       EMPTY_STORY},  // CAT_90100
      {EMPTY_STORY,
       EMPTY_STORY,
       EMPTY_STORY,
       EMPTY_STORY,
       EMPTY_STORY,
       EMPTY_STORY,
       EMPTY_STORY,
       EMPTY_STORY},  // CAT_FROST
  };

  if (categoryCoastal < CAT_NA || categoryCoastal > CAT_FROST)
    return EMPTY_STORY;
  if (categoryInland < CAT_NA || categoryInland > CAT_FROST)
    return EMPTY_STORY;

  return table[categoryCoastal][categoryInland];
}

// Helper: emit paikoin-hallaa phrase, possibly severe
void emit_frost_phrase(Sentence& s, bool severe)
{
  s << (severe ? PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE
               : PAIKOIN_HALLAA_COMPOSITE_PHRASE);
}

Sentence get_frost_onenight_phrase_simple(const int& phraseId)
{
  Sentence sentence;
  switch (phraseId)
  {
    case ALAVILLA_MAILLA_HALLAN_VAARA:
      sentence << ALAVILLA_MAILLA_HALLAN_VAARA_COMPOSITE_PHRASE << EMPTY_STRING;
      break;
    case MAHDOLLISESTI_HALLAA:
      sentence << MAHDOLLISESTI_HALLAA_COMPOSITE_PHRASE << EMPTY_STRING;
      break;
    case PAIKOIN_HALLAA:
      sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE << EMPTY_STRING << PAIKOIN_WORD;
      break;
    case RANNIKOLLA_HALLAN_VAARA:
      sentence << HALLAN_VAARA_COMPOSITE_PHRASE << COAST_PHRASE;
      break;
    case RANNIKOLLA_MAHDOLLISESTI_HALLAA:
      sentence << MAHDOLLISESTI_HALLAA_COMPOSITE_PHRASE << COAST_PHRASE;
      break;
    case RANNIKOLLA_PAIKOIN_HALLAA:
      sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE << COAST_PHRASE << PAIKOIN_WORD;
      break;
    case SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA:
      sentence << ALAVILLA_MAILLA_HALLAN_VAARA_COMPOSITE_PHRASE << INLAND_PHRASE;
      break;
    case SISAMAASSA_MAHDOLLISESTI_HALLAA:
      sentence << MAHDOLLISESTI_HALLAA_COMPOSITE_PHRASE << INLAND_PHRASE;
      break;
    case SISAMAASSA_PAIKOIN_HALLAA:
      sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE << INLAND_PHRASE << PAIKOIN_WORD;
      break;
    default:
      break;
  }
  return sentence;
}

Sentence get_frost_onenight_phrase_severe(const int& phraseId, bool severe)
{
  Sentence sentence;
  switch (phraseId)
  {
    case MONIN_PAIKOIN_HALLAA:
      emit_frost_phrase(sentence, severe);
      sentence << EMPTY_STRING << MONIN_PAIKOIN_WORD;
      break;
    case HALLAA_YLEISESTI:
      emit_frost_phrase(sentence, severe);
      sentence << EMPTY_STRING << EMPTY_STRING;
      break;
    case RANNIKOLLA_MONIN_PAIKOIN_HALLAA:
      emit_frost_phrase(sentence, severe);
      sentence << COAST_PHRASE << MONIN_PAIKOIN_WORD;
      break;
    case RANNIKOLLA_HALLAA:
      emit_frost_phrase(sentence, severe);
      sentence << COAST_PHRASE << EMPTY_STRING;
      break;
    case SISAMAASSA_MONIN_PAIKOIN_HALLAA:
      emit_frost_phrase(sentence, severe);
      sentence << INLAND_PHRASE << MONIN_PAIKOIN_WORD;
      break;
    case SISAMAASSA_YLEISESTI_HALLAA:
      emit_frost_phrase(sentence, severe);
      sentence << INLAND_PHRASE << EMPTY_STRING;
      break;
    default:
      break;
  }
  return sentence;
}

Sentence get_frost_onenight_phrase_composite(const int& phraseId, bool severe)
{
  Sentence sentence;
  switch (phraseId)
  {
    case (SISAMAASSA_MONIN_PAIKOIN_HALLAA * 100) + RANNIKOLLA_MAHDOLLISESTI_HALLAA:
      emit_frost_phrase(sentence, severe);
      sentence << INLAND_PHRASE << MONIN_PAIKOIN_WORD << Delimiter(COMMA_PUNCTUATION_MARK)
               << MAHDOLLISESTI_HALLAA_COMPOSITE_PHRASE << COAST_PHRASE;
      break;
    case (SISAMAASSA_YLEISESTI_HALLAA * 100) + RANNIKOLLA_MAHDOLLISESTI_HALLAA:
      emit_frost_phrase(sentence, severe);
      sentence << INLAND_PHRASE << EMPTY_STRING << Delimiter(COMMA_PUNCTUATION_MARK)
               << MAHDOLLISESTI_HALLAA_COMPOSITE_PHRASE << COAST_PHRASE;
      break;
    case (SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA * 100) + RANNIKOLLA_PAIKOIN_HALLAA:
      sentence << ALAVILLA_MAILLA_HALLAN_VAARA_COMPOSITE_PHRASE << INLAND_PHRASE
               << Delimiter(COMMA_PUNCTUATION_MARK) << PAIKOIN_HALLAA_COMPOSITE_PHRASE
               << COAST_PHRASE << PAIKOIN_WORD;
      break;
    case (SISAMAASSA_YLEISESTI_HALLAA * 100) + RANNIKOLLA_PAIKOIN_HALLAA:
      emit_frost_phrase(sentence, severe);
      sentence << INLAND_PHRASE << EMPTY_STRING << Delimiter(COMMA_PUNCTUATION_MARK)
               << PAIKOIN_HALLAA_COMPOSITE_PHRASE << COAST_PHRASE << MONIN_PAIKOIN_WORD;
      break;
    case (SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA * 100) + RANNIKOLLA_MONIN_PAIKOIN_HALLAA:
      sentence << ALAVILLA_MAILLA_HALLAN_VAARA_COMPOSITE_PHRASE << INLAND_PHRASE
               << Delimiter(COMMA_PUNCTUATION_MARK);
      emit_frost_phrase(sentence, severe);
      sentence << COAST_PHRASE << MONIN_PAIKOIN_WORD;
      break;
    case (RANNIKOLLA_MONIN_PAIKOIN_HALLAA * 100) + SISAMAASSA_MAHDOLLISESTI_HALLAA:
      emit_frost_phrase(sentence, severe);
      sentence << COAST_PHRASE << MONIN_PAIKOIN_WORD << Delimiter(COMMA_PUNCTUATION_MARK)
               << MAHDOLLISESTI_HALLAA_COMPOSITE_PHRASE << INLAND_PHRASE;
      break;
    case (SISAMAASSA_PAIKOIN_HALLAA * 100) + RANNIKOLLA_MONIN_PAIKOIN_HALLAA:
      sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE << INLAND_PHRASE << PAIKOIN_WORD
               << Delimiter(COMMA_PUNCTUATION_MARK);
      emit_frost_phrase(sentence, severe);
      sentence << COAST_PHRASE << MONIN_PAIKOIN_WORD;
      break;
    case (SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA * 100) + RANNIKOLLA_HALLAA:
      sentence << ALAVILLA_MAILLA_HALLAN_VAARA_COMPOSITE_PHRASE << INLAND_PHRASE
               << Delimiter(COMMA_PUNCTUATION_MARK);
      emit_frost_phrase(sentence, severe);
      sentence << COAST_PHRASE << EMPTY_STRING;
      break;
    case (RANNIKOLLA_HALLAA * 100) + SISAMAASSA_MAHDOLLISESTI_HALLAA:
      emit_frost_phrase(sentence, severe);
      sentence << COAST_PHRASE << EMPTY_STRING << Delimiter(COMMA_PUNCTUATION_MARK)
               << MAHDOLLISESTI_HALLAA_COMPOSITE_PHRASE << INLAND_PHRASE;
      break;
    case (RANNIKOLLA_HALLAA * 100) + SISAMAASSA_PAIKOIN_HALLAA:
      emit_frost_phrase(sentence, severe);
      sentence << COAST_PHRASE << EMPTY_STRING << Delimiter(COMMA_PUNCTUATION_MARK)
               << PAIKOIN_HALLAA_COMPOSITE_PHRASE << INLAND_PHRASE << PAIKOIN_WORD;
      break;
    default:
      break;
  }
  return sentence;
}

Sentence get_frost_onenight_phrase(const int& phraseId, const bool& tellSevereFrostStory)
{
  if (phraseId == EMPTY_STORY || phraseId == YOPAKKASTA)
    return {};

  Sentence s = get_frost_onenight_phrase_simple(phraseId);
  if (!s.empty())
    return s;

  s = get_frost_onenight_phrase_severe(phraseId, tellSevereFrostStory);
  if (!s.empty())
    return s;

  return get_frost_onenight_phrase_composite(phraseId, tellSevereFrostStory);
}

Sentence frost_onenight_sentence(const double& coastalFrostProbability,
                                 const double& inlandFrostProbability,
                                 const bool& severeFrostCoastal,
                                 const bool& severeFrostInland,
                                 const unsigned short& forecast_areas,
                                 const unsigned short& growing_season_started,
                                 const unsigned short& night_frost)
{
  Sentence sentence;

  int phraseId = get_frost_onenight_phrase_id(coastalFrostProbability,
                                              inlandFrostProbability,
                                              forecast_areas,
                                              growing_season_started,
                                              night_frost);

  sentence << get_frost_onenight_phrase(phraseId, severeFrostCoastal || severeFrostInland);

  return sentence;
}

bool is_night_frost(MessageLogger& log,
                    const std::string& theLogMessage,
                    const std::string& theFakeVariable,
                    const double& theNightFrostLimit,
                    const GridForecaster& forecaster,
                    const AnalysisSources& theSources,
                    const WeatherArea& theArea,
                    const WeatherPeriod& thePeriod)
{
  ComparativeAcceptor comparativeAcceptor(0.0, LESS_THAN);
  WeatherResult nightFrostPercentage = forecaster.analyze(theFakeVariable,
                                                          theSources,
                                                          Temperature,
                                                          Percentage,
                                                          Minimum,
                                                          theArea,
                                                          thePeriod,
                                                          DefaultAcceptor(),
                                                          DefaultAcceptor(),
                                                          comparativeAcceptor);

  log << NFmiStringTools::Convert(theLogMessage) << nightFrostPercentage << '\n';

  // At least 20% of the area has night frost
  bool retval = nightFrostPercentage.value() != kFloatMissing &&
                nightFrostPercentage.value() >= theNightFrostLimit;

  return retval;
}

bool growing_season_going_on(const WeatherArea& theArea,
                             const AnalysisSources& theSources,
                             const WeatherPeriod& thePeriod,
                             const std::string& theVariable,
                             MessageLogger& log,
                             const std::string& theLogMessage)
{
  float growingSeasonPercentage =
      SeasonTools::growing_season_percentage(theArea, theSources, thePeriod, theVariable);

  bool growingSeasonGoingOn =
      SeasonTools::growing_season_going_on(theArea, theSources, thePeriod, theVariable);

  if (growingSeasonPercentage != kFloatMissing)
  {
    log << NFmiStringTools::Convert(theLogMessage) << fixed << setprecision(0)
        << (theArea.isPoint() ? growingSeasonPercentage * 100.0 : growingSeasonPercentage) << '\n';
  }
  const int notBeforeDate = Settings::optional_int(theVariable + "::not_before_date", 0);
  const int notAfterDate = Settings::optional_int(theVariable + "::not_after_date", 0);
  const int periodStartDate =
      (thePeriod.localStartTime().GetMonth() * 100) + thePeriod.localStartTime().GetDay();

  bool acceptedDate(false);

  if (notBeforeDate > notAfterDate)
  {
    acceptedDate = ((notBeforeDate == 0) || periodStartDate >= notBeforeDate) ||
                   ((notAfterDate == 0) || periodStartDate <= notAfterDate);
  }
  else
  {
    acceptedDate = ((notBeforeDate == 0) || periodStartDate >= notBeforeDate) &&
                   ((notAfterDate == 0) || periodStartDate <= notAfterDate);
  }

  if (growingSeasonGoingOn && !acceptedDate)
  {
    if (notBeforeDate != 0)
    {
      log << "No frost warnings will be issued before "
          << Settings::optional_string(theVariable + "::not_before_date", "**") << '\n';
    }
    if (notAfterDate != 0)
    {
      log << "No frost warnings are issued after "
          << Settings::optional_string(theVariable + "::not_after_date", "**") << '\n';
    }

    growingSeasonGoingOn = false;
  }

  return growingSeasonGoingOn;
}
bool should_ignore_coastal_area(const std::string& theVar,
                                const WeatherArea& theArea,
                                float coastalPercentage,
                                float separateCoastalAreaPercentage)
{
  if (coastalPercentage < separateCoastalAreaPercentage)
    return true;
  std::string coastal_areas_to_ignore =
      Settings::optional_string(theVar + "::dont_report_coastal_area", "");
  if (coastal_areas_to_ignore.empty())
    return false;
  std::vector<std::string> areas;
  boost::algorithm::split(areas, coastal_areas_to_ignore, boost::algorithm::is_any_of(","));
  for (const auto& area : areas)
  {
    if (theArea.name() == boost::trim_copy(area))
      return true;
  }
  return false;
}

unsigned short compute_night_frost(MessageLogger& log,
                                   const std::string& theVar,
                                   const GridForecaster& forecaster,
                                   const AnalysisSources& sources,
                                   const WeatherArea& coastalArea,
                                   const WeatherArea& inlandArea,
                                   const WeatherPeriod& night1,
                                   unsigned short forecast_areas,
                                   unsigned short growing_season_started,
                                   double required_night_frost_percentage)
{
  unsigned short night_frost = 0x0;
  if ((forecast_areas & COASTAL_AREA) && (growing_season_started & COASTAL_AREA))
  {
    bool isNightFrost = is_night_frost(log,
                                       "actual night frost percentage, coastal: ",
                                       theVar + "::fake::night_frost_percentage::coastal",
                                       required_night_frost_percentage,
                                       forecaster,
                                       sources,
                                       coastalArea,
                                       night1);
    night_frost |= (isNightFrost ? COASTAL_AREA : 0x0);
  }
  if ((forecast_areas & INLAND_AREA) && (growing_season_started & INLAND_AREA))
  {
    bool isNightFrost = is_night_frost(log,
                                       "actual night frost percentage, inland: ",
                                       theVar + "::fake::night_frost_percentage::inland",
                                       required_night_frost_percentage,
                                       forecaster,
                                       sources,
                                       inlandArea,
                                       night1);
    night_frost |= (isNightFrost ? INLAND_AREA : 0x0);
  }
  return night_frost;
}

struct FrostProbabilities
{
  WeatherResult frostCoastal{kFloatMissing, 0.0};
  WeatherResult severeFrostCoastal{kFloatMissing, 0.0};
  WeatherResult frostInland{kFloatMissing, 0.0};
  WeatherResult severeFrostInland{kFloatMissing, 0.0};
};

FrostProbabilities compute_frost_probabilities(MessageLogger& log,
                                               const std::string& theVar,
                                               GridForecaster& forecaster,
                                               const AnalysisSources& sources,
                                               const WeatherArea& coastalArea,
                                               const WeatherArea& inlandArea,
                                               const WeatherPeriod& night1,
                                               unsigned short forecast_areas,
                                               unsigned short growing_season_started,
                                               unsigned short night_frost)
{
  FrostProbabilities fp;
  if ((forecast_areas & COASTAL_AREA) && (growing_season_started & COASTAL_AREA) &&
      !(night_frost & COASTAL_AREA))
  {
    fp.frostCoastal = forecaster.analyze(theVar + "::fake::frost_probability::coastal",
                                         sources,
                                         Frost,
                                         Maximum,
                                         Maximum,
                                         coastalArea,
                                         night1);
    log << "Frost_Maximum_Maximum Coastal: " << fp.frostCoastal << '\n';
    if (fp.frostCoastal.value() >= 70.0)
    {
      fp.severeFrostCoastal =
          forecaster.analyze(theVar + "::fake::severe_frost_probability::coastal",
                             sources,
                             SevereFrost,
                             Maximum,
                             Maximum,
                             coastalArea,
                             night1);
      log << "Severe_Frost_Maximum_Maximum Coastal: " << fp.severeFrostCoastal << '\n';
    }
  }
  if ((forecast_areas & INLAND_AREA) && (growing_season_started & INLAND_AREA) &&
      !(night_frost & INLAND_AREA))
  {
    fp.frostInland = forecaster.analyze(theVar + "::fake::frost_probability::inland",
                                        sources,
                                        Frost,
                                        Maximum,
                                        Maximum,
                                        inlandArea,
                                        night1);
    log << "Frost_Maximum_Maximum Inland: " << fp.frostInland << '\n';
    if (fp.frostInland.value() >= 70.0)
    {
      fp.severeFrostInland = forecaster.analyze(theVar + "::fake::severe_frost_probability::inland",
                                                sources,
                                                SevereFrost,
                                                Maximum,
                                                Maximum,
                                                inlandArea,
                                                night1);
      log << "Severe_Frost_Maximum_Maximum Inland: " << fp.severeFrostInland << '\n';
    }
  }
  return fp;
}

}  // namespace
}  // namespace FrostOnenight

// ----------------------------------------------------------------------
/*!
 * \brief Generate story on one night frost
 *
 * \return The generated paragraph
 */
// ----------------------------------------------------------------------

Paragraph FrostStory::onenight() const
{
  using namespace FrostOnenight;

  MessageLogger log("FrostStory::onenight");

  log << "forecasttime: " << itsForecastTime << '\n';

  Paragraph paragraph;

  boostfs::path temperatureSumFile(
      Settings::optional_string("textgen::temperaturesum_forecast", ""));
  bool temperatureSumFileExists(boostfs::exists(temperatureSumFile));
  bool resressionTestRun(Settings::optional_bool(itsVar + "::regression_test", false));

  if (!temperatureSumFileExists && !resressionTestRun)
  {
    log << "The temperature sum file " << temperatureSumFile << " does not exist!\n";
    return paragraph;
  }

  GridForecaster forecaster;

  const int starthour = Settings::require_hour(itsVar + "::night::starthour");
  const int endhour = Settings::require_hour(itsVar + "::night::endhour");
  const int maxstarthour = Settings::optional_hour(itsVar + "::night::maxstarthour", starthour);
  const int minendhour = Settings::optional_hour(itsVar + "::night::minendhour", endhour);

  const double required_night_frost_percentage =
      Settings::optional_double(itsVar + "::required_night_frost_percentage", 20);
  const double required_severe_frost_probability =
      Settings::optional_double(itsVar + "::required_severe_frost_probability", 20);
  std::string parameter_name(itsVar + "::required_growing_season_percentage::default");
  if (itsArea.isNamed() &&
      (Settings::isset(itsVar + "::required_growing_season_percentage::" + itsArea.name())))
    parameter_name = itsVar + "::required_growing_season_percentage::" + itsArea.name();

  const double required_growing_season_percentage =
      Settings::optional_double(parameter_name, 33.33);

  log << "starthour " << starthour << '\n';
  log << "endhour " << endhour << '\n';
  log << "required_growing_season_percentage " << required_growing_season_percentage << '\n';
  log << "required_night_frost_percentage " << required_night_frost_percentage << '\n';
  log << "required_severe_frost_probability " << required_severe_frost_probability << '\n';
  log << "maxstarthour " << maxstarthour << '\n';
  log << "minendhour " << minendhour << '\n';

  HourPeriodGenerator generator(itsPeriod, itsVar + "::night");

  // Too late for this night? Return empty story then
  if (generator.size() == 0)
  {
    log << "Too late for frost-story for this night!\n";
    log << paragraph;
    return paragraph;
  }

  WeatherPeriod night1 =
      WeatherPeriodTools::getPeriod(itsPeriod, 1, starthour, endhour, maxstarthour, minendhour);

  RangeAcceptor precipitationlimits;
  precipitationlimits.lowerLimit(DRY_WEATHER_LIMIT_DRIZZLE);

  WeatherResult precipitationResult = forecaster.analyze(itsVar + "::fake::precipitation",
                                                         itsSources,
                                                         Precipitation,
                                                         Maximum,
                                                         Sum,
                                                         itsArea,
                                                         night1,
                                                         DefaultAcceptor(),
                                                         precipitationlimits);

  if (precipitationResult.value() > 0)
  {
    log << "There is some rain during period " << night1.localStartTime() << "..."
        << night1.localEndTime() << " -> frost is not reported!\n";
    return paragraph;
  }

  // Calculate values for coastal and inland area separately
  WeatherArea coastalArea = itsArea;
  coastalArea.type(WeatherArea::Coast);

  WeatherArea inlandArea = itsArea;
  inlandArea.type(WeatherArea::Inland);

  bool growingSeasonCoastal = growing_season_going_on(
      coastalArea, itsSources, night1, itsVar, log, "Actual growing season percentage, coastal: ");

  bool growingSeasonInland = growing_season_going_on(
      inlandArea, itsSources, night1, itsVar, log, "Actual growing season percentage, inland: ");

  if (!growingSeasonInland)
  {
    log << (growingSeasonCoastal
                ? "Growing season not started on inland area, coastal area is not reported alone!\n"
                : "Growing season not started, frost will not be reported!\n");
    return paragraph;
  }

  float coastalPercentage = get_area_percentage(
      itsVar + "::fake::area_percentage", itsArea, WeatherArea::Coast, itsSources, itsPeriod);

  float separate_coastal_area_percentage = Settings::optional_double(
      itsVar + "::separate_coastal_area_percentage", SEPARATE_COASTAL_AREA_PERCENTAGE);

  bool ignoreCoastalArea = should_ignore_coastal_area(
      itsVar, itsArea, coastalPercentage, separate_coastal_area_percentage);

  float inlandPercentage = get_area_percentage(
      itsVar + "::fake::area_percentage", itsArea, WeatherArea::Inland, itsSources, itsPeriod);

  if (ignoreCoastalArea && coastalPercentage > 0.0)
  {
    log << "Coastal proportion: " << coastalPercentage << " is smaller than "
        << SEPARATE_COASTAL_AREA_PERCENTAGE << " and thus will be ignored!\n";
  }

  unsigned short growing_season_started = 0x0;
  unsigned short forecast_areas = 0x0;
  unsigned short night_frost = 0x0;

  growing_season_started |= (growingSeasonCoastal ? COASTAL_AREA : 0x0);
  growing_season_started |= (growingSeasonInland ? INLAND_AREA : 0x0);
  forecast_areas |= (growingSeasonCoastal ? COASTAL_AREA : 0x0);
  forecast_areas |= (inlandPercentage > 0.0 ? INLAND_AREA : 0x0);
  if (forecast_areas & COASTAL_AREA && ignoreCoastalArea)
  {
    forecast_areas ^= COASTAL_AREA;
    if (growing_season_started & COASTAL_AREA)
      growing_season_started ^= COASTAL_AREA;
  }

  if (!(forecast_areas & (COASTAL_AREA | INLAND_AREA)))
  {
    log << "Neither Coastal nor Inland area is included! \n";
    return paragraph;
  }
  if (!(forecast_areas & COASTAL_AREA))
  {
    log << "Coastal area is not included\n";
  }
  if (!(forecast_areas & INLAND_AREA))
  {
    log << "Inland area is not included\n";
  }

  WeatherResult temperatureMinMinCoastal(kFloatMissing, 0.0);
  WeatherResult temperatureMinMinInland(kFloatMissing, 0.0);

  night_frost = compute_night_frost(log,
                                    itsVar,
                                    forecaster,
                                    itsSources,
                                    coastalArea,
                                    inlandArea,
                                    night1,
                                    forecast_areas,
                                    growing_season_started,
                                    required_night_frost_percentage);

  // night frost at both areas
  if (night_frost == (COASTAL_AREA | INLAND_AREA))
  {
    log << "Night frost both on coastal and inland area!\n";
    return paragraph;
  }

  FrostProbabilities fp = compute_frost_probabilities(log,
                                                      itsVar,
                                                      forecaster,
                                                      itsSources,
                                                      coastalArea,
                                                      inlandArea,
                                                      night1,
                                                      forecast_areas,
                                                      growing_season_started,
                                                      night_frost);

  const WeatherResult& frostMaxMaxCoastal = fp.frostCoastal;
  const WeatherResult& severeFrostMaxMaxCoastal = fp.severeFrostCoastal;
  const WeatherResult& frostMaxMaxInland = fp.frostInland;
  const WeatherResult& severeFrostMaxMaxInland = fp.severeFrostInland;

  log << "forecast_areas:  " << forecast_areas << '\n';
  log << "growing_season_started:  " << growing_season_started << '\n';
  log << "night_frost:  " << night_frost << '\n';

  bool is_severe_frost_coastal =
      (severeFrostMaxMaxCoastal.value() > required_severe_frost_probability &&
       severeFrostMaxMaxCoastal.value() <= 100);
  bool is_severe_frost_inland =
      (severeFrostMaxMaxInland.value() > required_severe_frost_probability &&
       severeFrostMaxMaxInland.value() <= 100);

  paragraph << frost_onenight_sentence(frostMaxMaxCoastal.value(),
                                       frostMaxMaxInland.value(),
                                       is_severe_frost_coastal,
                                       is_severe_frost_inland,
                                       forecast_areas,
                                       growing_season_started,
                                       night_frost);

  log << paragraph;
  return paragraph;
}

}  // namespace TextGen

// ======================================================================
