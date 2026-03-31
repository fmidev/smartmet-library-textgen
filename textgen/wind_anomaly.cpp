// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WindStory::anomaly
 */
// ======================================================================

#include "WindStory.h"

#include "AreaTools.h"
#include "Delimiter.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "SeasonTools.h"
#include "Sentence.h"
#include "TemperatureStoryTools.h"
#include "UnitFactory.h"
#include "WeatherForecast.h"
#include "WindStoryTools.h"
#include <calculator/GridForecaster.h>
#include <calculator/HourPeriodGenerator.h>
#include <calculator/Settings.h>
#include <calculator/WeatherPeriodTools.h>
#include <calculator/WeatherResult.h>
#include <calculator/WeatherResultTools.h>
#include <macgyver/Exception.h>

#include <newbase/NFmiGrid.h>
#include <newbase/NFmiIndexMask.h>
#include <newbase/NFmiIndexMaskSource.h>
#include <newbase/NFmiIndexMaskTools.h>
#include <newbase/NFmiMetMath.h>
#include <newbase/NFmiQueryData.h>
#include <newbase/NFmiStringTools.h>

#include <calculator/WeatherArea.h>
#include <calculator/WeatherSource.h>

#include <boost/lexical_cast.hpp>

#include <map>

using namespace std;

namespace TextGen
{
namespace WindAnomaly
{
using namespace TextGen;
using namespace WindStoryTools;
using namespace TemperatureStoryTools;

using namespace Settings;
using namespace SeasonTools;
using namespace AreaTools;

#define ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE \
  "[iltapaivalla] [rannikolla] on [tuulista]"
#define ILTAPAIVALLA_ON_TUULISTA_COMPOSITE_PHRASE "[iltapaivalla] on [tuulista]"
#define RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE "[rannikolla] on [tuulista]"

#define ILTAPAIVALLA_RANNIKOLLA_PAKKANEN_ON_PUREVAA_COMPOSITE_PHRASE \
  "[iltapaivalla] [rannikolla] pakkanen on purevaa"
#define ILTAPAIVALLA_PAKKANEN_ON_PUREVAA_COMPOSITE_PHRASE "[iltapaivalla] pakkanen on purevaa"
#define RANNIKOLLA_PAKKANEN_ON_PUREVAA_COMPOSITE_PHRASE "[rannikolla] pakkanen on purevaa"
#define ILTAPAIVALLA_RANNIKOLLA_PAKKANEN_ON_ERITTAIN_PUREVAA_COMPOSITE_PHRASE \
  "[iltapaivalla] [rannikolla] pakkanen on erittain purevaa"
#define ILTAPAIVALLA_PAKKANEN_ON_ERITTAIN_PUREVAA_COMPOSITE_PHRASE \
  "[iltapaivalla] pakkanen on erittain purevaa"
#define RANNIKOLLA_PAKKANEN_ON_ERITTAIN_PUREVAA_COMPOSITE_PHRASE \
  "[rannikolla] pakkanen on erittain purevaa"

#define ILTAPAIVALLA_RANNIKOLLA_TUULI_SAA_SAAN_TUNTUMAAN_KYLMEMMALTA_COMPOSITE_PHRASE \
  "[iltapaivalla] [rannikolla] tuuli saa saan tuntumaan kylmemmalta"
#define ILTAPAIVALLA_TUULI_SAA_SAAN_TUNTUMAAN_KYLMEMMALTA_COMPOSITE_PHRASE \
  "[iltapaivalla] tuuli saa saan tuntumaan kylmemmalta"
#define RANNIKOLLA_TUULI_SAA_SAAN_TUNTUMAAN_KYLMEMMALTA_COMPOSITE_PHRASE \
  "[rannikolla] tuuli saa saan tuntumaan kylmemmalta"
#define ILTAPAIVALLA_RANNIKOLLA_TUULI_SAA_SAAN_TUNTUMAAN_VIILEAMMALTA_COMPOSITE_PHRASE \
  "[iltapaivalla] [rannikolla] tuuli saa saan tuntumaan viileammalta"
#define ILTAPAIVALLA_TUULI_SAA_SAAN_TUNTUMAAN_VIILEAMMALTA_COMPOSITE_PHRASE \
  "[rannikolla] tuuli saa saan tuntumaan viileammalta"
#define RANNIKOLLA_TUULI_SAA_SAAN_TUNTUMAAN_VIILEAMMALTA_COMPOSITE_PHRASE \
  "[rannikolla] tuuli saa saan tuntumaan viileammalta"

#define WINDY_WEATER_LIMIT 7.0
#define EXTREMELY_WINDY_WEATHER_LIMIT 10.0
#define WIND_COOLING_THE_WEATHER_LIMIT 6.0
#define EXTREME_WINDCHILL_LIMIT (-35.0)
#define MILD_WINDCHILL_LIMIT (-25.0)
#define ZERO_DEGREES 0.0
#define TEMPERATURE_AND_WINDCHILL_DIFFERENCE_LIMIT 7.0

#define TUULI_KYLMENTAA_SAATA_LOWER_LIMIT 0.0
#define TUULI_KYLMENTAA_SAATA_UPPER_LIMIT 10.0
#define TUULI_VIILENTAA_SAATA_LOWER_LIMIT 10.0
#define TUULI_VIILENTAA_SAATA_UPPER_LIMIT 15.0

#define SAA_WORD "saa"
#define ON_WORD "on"
#define HYVIN_WORD "hyvin"
#define SAA_ON_TUULINEN_PHRASE "saa on tuulinen"
#define SAA_ON_HYVIN_TUULINEN_PHRASE "saa on hyvin tuulinen"
#define HYVIN_TUULINEN_PHRASE "hyvin tuulinen"
#define HYVIN_TUULISTA_PHRASE "hyvin tuulista"
#define TUULISTA_WORD "tuulista"
#define AAMUPAIVALLA_WORD "aamupaivalla"
#define ILTAPAIVALLA_WORD "iltapaivalla"
#define SISAMAASSA_WORD "sisamaassa"
#define RANNIKOLLA_WORD "rannikolla"
#define PAKKANEN_ON_PUREVAA_PHRASE "pakkanen on purevaa"
#define PAKKANEN_ON_ERITTAIN_PUREVAA_PHRASE "pakkanen on erittain purevaa"
#define TUULI_SAA_SAAN_TUNTUMAAN_VIILEAMMALTA_PHRASE "tuuli saa saan tuntumaan viileammalta"
#define TUULI_SAA_SAAN_TUNTUMAAN_KYLMEMMALTA_PHRASE "tuuli saa saan tuntumaan kylmemmalta"
#define TANAAN_WORD "tanaan"
#define HUOMENNA_WORD "huomenna"

namespace
{

struct wind_anomaly_params
{
  wind_anomaly_params(const string& variable,
                      MessageLogger& log,
                      const AnalysisSources& sources,
                      const WeatherArea& area,
                      const WeatherPeriod& windAnomalyPeriod,
                      const forecast_season_id& season,
                      const TextGenPosixTime& forecastTime,
                      const short& periodLength,
                      const bool& specifyPartOfTheDayFlag)
      : theVariable(variable),
        theLog(log),
        theSources(sources),
        theArea(area),
        thePeriod(windAnomalyPeriod),
        theSeason(season),
        theForecastTime(forecastTime),
        thePeriodLength(periodLength),
        theSpecifyPartOfTheDayFlag(specifyPartOfTheDayFlag),

        theTemperatureAreaMorningMinimum(kFloatMissing, 0),
        theTemperatureAreaMorningMean(kFloatMissing, 0),
        theTemperatureAreaMorningMaximum(kFloatMissing, 0),
        theTemperatureAreaAfternoonMinimum(kFloatMissing, 0),
        theTemperatureAreaAfternoonMean(kFloatMissing, 0),
        theTemperatureAreaAfternoonMaximum(kFloatMissing, 0),
        theTemperatureInlandMorningMinimum(kFloatMissing, 0),
        theTemperatureInlandMorningMean(kFloatMissing, 0),
        theTemperatureInlandMorningMaximum(kFloatMissing, 0),
        theTemperatureInlandAfternoonMinimum(kFloatMissing, 0),
        theTemperatureInlandAfternoonMean(kFloatMissing, 0),
        theTemperatureInlandAfternoonMaximum(kFloatMissing, 0),
        theTemperatureCoastalMorningMinimum(kFloatMissing, 0),
        theTemperatureCoastalMorningMean(kFloatMissing, 0),
        theTemperatureCoastalMorningMaximum(kFloatMissing, 0),
        theTemperatureCoastalAfternoonMinimum(kFloatMissing, 0),
        theTemperatureCoastalAfternoonMean(kFloatMissing, 0),
        theTemperatureCoastalAfternoonMaximum(kFloatMissing, 0),
        theWindspeedInlandMorningMinimum(kFloatMissing, 0),
        theWindspeedInlandMorningMean(kFloatMissing, 0),
        theWindspeedInlandMorningMaximum(kFloatMissing, 0),
        theWindspeedInlandAfternoonMinimum(kFloatMissing, 0),
        theWindspeedInlandAfternoonMean(kFloatMissing, 0),
        theWindspeedInlandAfternoonMaximum(kFloatMissing, 0),
        theWindspeedCoastalMorningMinimum(kFloatMissing, 0),
        theWindspeedCoastalMorningMean(kFloatMissing, 0),
        theWindspeedCoastalMorningMaximum(kFloatMissing, 0),
        theWindspeedCoastalAfternoonMinimum(kFloatMissing, 0),
        theWindspeedCoastalAfternoonMean(kFloatMissing, 0),
        theWindspeedCoastalAfternoonMaximum(kFloatMissing, 0),
        theWindchillInlandMorningMinimum(kFloatMissing, 0),
        theWindchillInlandMorningMean(kFloatMissing, 0),
        theWindchillInlandMorningMaximum(kFloatMissing, 0),
        theWindchillInlandAfternoonMinimum(kFloatMissing, 0),
        theWindchillInlandAfternoonMean(kFloatMissing, 0),
        theWindchillInlandAfternoonMaximum(kFloatMissing, 0),
        theWindchillCoastalMorningMinimum(kFloatMissing, 0),
        theWindchillCoastalMorningMean(kFloatMissing, 0),
        theWindchillCoastalMorningMaximum(kFloatMissing, 0),
        theWindchillCoastalAfternoonMinimum(kFloatMissing, 0),
        theWindchillCoastalAfternoonMean(kFloatMissing, 0),
        theWindchillCoastalAfternoonMaximum(kFloatMissing, 0)

  {
  }

  const string& theVariable;
  MessageLogger& theLog;
  const AnalysisSources& theSources;
  const WeatherArea& theArea;
  const WeatherPeriod& thePeriod;
  const forecast_season_id& theSeason;
  const TextGenPosixTime& theForecastTime;
  const short& thePeriodLength;
  bool theSpecifyPartOfTheDayFlag = false;
  string theFakeVariable;
  bool theCoastalAndInlandTogetherFlag = false;
  WeatherResult theTemperatureAreaMorningMinimum;
  WeatherResult theTemperatureAreaMorningMean;
  WeatherResult theTemperatureAreaMorningMaximum;
  WeatherResult theTemperatureAreaAfternoonMinimum;
  WeatherResult theTemperatureAreaAfternoonMean;
  WeatherResult theTemperatureAreaAfternoonMaximum;
  WeatherResult theTemperatureInlandMorningMinimum;
  WeatherResult theTemperatureInlandMorningMean;
  WeatherResult theTemperatureInlandMorningMaximum;
  WeatherResult theTemperatureInlandAfternoonMinimum;
  WeatherResult theTemperatureInlandAfternoonMean;
  WeatherResult theTemperatureInlandAfternoonMaximum;
  WeatherResult theTemperatureCoastalMorningMinimum;
  WeatherResult theTemperatureCoastalMorningMean;
  WeatherResult theTemperatureCoastalMorningMaximum;
  WeatherResult theTemperatureCoastalAfternoonMinimum;
  WeatherResult theTemperatureCoastalAfternoonMean;
  WeatherResult theTemperatureCoastalAfternoonMaximum;
  WeatherResult theWindspeedInlandMorningMinimum;
  WeatherResult theWindspeedInlandMorningMean;
  WeatherResult theWindspeedInlandMorningMaximum;
  WeatherResult theWindspeedInlandAfternoonMinimum;
  WeatherResult theWindspeedInlandAfternoonMean;
  WeatherResult theWindspeedInlandAfternoonMaximum;
  WeatherResult theWindspeedCoastalMorningMinimum;
  WeatherResult theWindspeedCoastalMorningMean;
  WeatherResult theWindspeedCoastalMorningMaximum;
  WeatherResult theWindspeedCoastalAfternoonMinimum;
  WeatherResult theWindspeedCoastalAfternoonMean;
  WeatherResult theWindspeedCoastalAfternoonMaximum;
  WeatherResult theWindchillInlandMorningMinimum;
  WeatherResult theWindchillInlandMorningMean;
  WeatherResult theWindchillInlandMorningMaximum;
  WeatherResult theWindchillInlandAfternoonMinimum;
  WeatherResult theWindchillInlandAfternoonMean;
  WeatherResult theWindchillInlandAfternoonMaximum;
  WeatherResult theWindchillCoastalMorningMinimum;
  WeatherResult theWindchillCoastalMorningMean;
  WeatherResult theWindchillCoastalMorningMaximum;
  WeatherResult theWindchillCoastalAfternoonMinimum;
  WeatherResult theWindchillCoastalAfternoonMean;
  WeatherResult theWindchillCoastalAfternoonMaximum;
  std::string theMorningWord;
  std::string theAfternoonWord;
};

void log_data(const wind_anomaly_params& theParameters)
{
  if (theParameters.theTemperatureAreaMorningMinimum.value() != kFloatMissing)
  {
    theParameters.theLog << "theTemperatureAreaMorningMinimum: "
                         << theParameters.theTemperatureAreaMorningMinimum << '\n';
    theParameters.theLog << "theTemperatureAreaMorningMean: "
                         << theParameters.theTemperatureAreaMorningMean << '\n';
    theParameters.theLog << "theTemperatureAreaMorningMaximum: "
                         << theParameters.theTemperatureAreaMorningMaximum << '\n';
  }

  if (theParameters.theTemperatureAreaAfternoonMinimum.value() != kFloatMissing)
  {
    theParameters.theLog << "theTemperatureAreaAfternoonMinimum: "
                         << theParameters.theTemperatureAreaAfternoonMinimum << '\n';
    theParameters.theLog << "theTemperatureAreaAfternoonMean: "
                         << theParameters.theTemperatureAreaAfternoonMean << '\n';
    theParameters.theLog << "theTemperatureAreaAfternoonMaximum: "
                         << theParameters.theTemperatureAreaAfternoonMaximum << '\n';
  }

  if (theParameters.theTemperatureInlandMorningMinimum.value() != kFloatMissing)
  {
    theParameters.theLog << "theTemperatureInlandMorningMinimum: "
                         << theParameters.theTemperatureInlandMorningMinimum << '\n';
    theParameters.theLog << "theTemperatureInlandMorningMean: "
                         << theParameters.theTemperatureInlandMorningMean << '\n';
    theParameters.theLog << "theTemperatureInlandMorningMaximum: "
                         << theParameters.theTemperatureInlandMorningMaximum << '\n';
  }

  if (theParameters.theTemperatureInlandAfternoonMinimum.value() != kFloatMissing)
  {
    theParameters.theLog << "theTemperatureInlandAfternoonMinimum: "
                         << theParameters.theTemperatureInlandAfternoonMinimum << '\n';
    theParameters.theLog << "theTemperatureInlandAfternoonMean: "
                         << theParameters.theTemperatureInlandAfternoonMean << '\n';
    theParameters.theLog << "theTemperatureInlandAfternoonMaximum: "
                         << theParameters.theTemperatureInlandAfternoonMaximum << '\n';
  }

  if (theParameters.theTemperatureCoastalMorningMinimum.value() != kFloatMissing)
  {
    theParameters.theLog << "theTemperatureCoastalMorningMinimum: "
                         << theParameters.theTemperatureCoastalMorningMinimum << '\n';
    theParameters.theLog << "theTemperatureCoastalMorningMean: "
                         << theParameters.theTemperatureCoastalMorningMean << '\n';
    theParameters.theLog << "theTemperatureCoastalMorningMaximum: "
                         << theParameters.theTemperatureCoastalMorningMaximum << '\n';
  }

  if (theParameters.theTemperatureCoastalAfternoonMinimum.value() != kFloatMissing)
  {
    theParameters.theLog << "theTemperatureCoastalAfternoonMinimum: "
                         << theParameters.theTemperatureCoastalAfternoonMinimum << '\n';
    theParameters.theLog << "theTemperatureCoastalAfternoonMean: "
                         << theParameters.theTemperatureCoastalAfternoonMean << '\n';
    theParameters.theLog << "theTemperatureCoastalAfternoonMaximum: "
                         << theParameters.theTemperatureCoastalAfternoonMaximum << '\n';
  }

  if (theParameters.theWindspeedInlandMorningMinimum.value() != kFloatMissing)
  {
    theParameters.theLog << "theWindspeedInlandMorningMinimum: "
                         << theParameters.theWindspeedInlandMorningMinimum << '\n';
    theParameters.theLog << "theWindspeedInlandMorningMean: "
                         << theParameters.theWindspeedInlandMorningMean << '\n';
    theParameters.theLog << "theWindspeedInlandMorningMaximum: "
                         << theParameters.theWindspeedInlandMorningMaximum << '\n';
  }

  if (theParameters.theWindspeedInlandAfternoonMinimum.value() != kFloatMissing)
  {
    theParameters.theLog << "theWindspeedInlandAfternoonMinimum: "
                         << theParameters.theWindspeedInlandAfternoonMinimum << '\n';
    theParameters.theLog << "theWindspeedInlandAfternoonMean: "
                         << theParameters.theWindspeedInlandAfternoonMean << '\n';
    theParameters.theLog << "theWindspeedInlandAfternoonMaximum: "
                         << theParameters.theWindspeedInlandAfternoonMaximum << '\n';
  }

  if (theParameters.theWindspeedCoastalMorningMinimum.value() != kFloatMissing)
  {
    theParameters.theLog << "theWindspeedCoastalMorningMinimum: "
                         << theParameters.theWindspeedCoastalMorningMinimum << '\n';
    theParameters.theLog << "theWindspeedCoastalMorningMean: "
                         << theParameters.theWindspeedCoastalMorningMean << '\n';
    theParameters.theLog << "theWindspeedCoastalMorningMaximum: "
                         << theParameters.theWindspeedCoastalMorningMaximum << '\n';
  }
  if (theParameters.theWindspeedCoastalAfternoonMinimum.value() != kFloatMissing)
  {
    theParameters.theLog << "theWindspeedCoastalAfternoonMinimum: "
                         << theParameters.theWindspeedCoastalAfternoonMinimum << '\n';
    theParameters.theLog << "theWindspeedCoastalAfternoonMean: "
                         << theParameters.theWindspeedCoastalAfternoonMean << '\n';
    theParameters.theLog << "theWindspeedCoastalAfternoonMaximum: "
                         << theParameters.theWindspeedCoastalAfternoonMaximum << '\n';
  }

  if (theParameters.theWindchillInlandMorningMinimum.value() != kFloatMissing)
  {
    theParameters.theLog << "theWindchillInlandMorningMinimum: "
                         << theParameters.theWindchillInlandMorningMinimum << '\n';
    theParameters.theLog << "theWindchillInlandMorningMean: "
                         << theParameters.theWindchillInlandMorningMean << '\n';
    theParameters.theLog << "theWindchillInlandMorningMaximum: "
                         << theParameters.theWindchillInlandMorningMaximum << '\n';
  }

  if (theParameters.theWindchillInlandAfternoonMinimum.value() != kFloatMissing)
  {
    theParameters.theLog << "theWindchillInlandAfternoonMinimum: "
                         << theParameters.theWindchillInlandAfternoonMinimum << '\n';
    theParameters.theLog << "theWindchillInlandAfternoonMean: "
                         << theParameters.theWindchillInlandAfternoonMean << '\n';
    theParameters.theLog << "theWindchillInlandAfternoonMaximum: "
                         << theParameters.theWindchillInlandAfternoonMaximum << '\n';
  }

  if (theParameters.theWindchillCoastalMorningMinimum.value() != kFloatMissing)
  {
    theParameters.theLog << "theWindchillCoastalMorningMinimum: "
                         << theParameters.theWindchillCoastalMorningMinimum << '\n';
    theParameters.theLog << "theWindchillCoastalMorningMean: "
                         << theParameters.theWindchillCoastalMorningMean << '\n';
    theParameters.theLog << "theWindchillCoastalMorningMaximum: "
                         << theParameters.theWindchillCoastalMorningMaximum << '\n';
  }

  if (theParameters.theWindchillCoastalAfternoonMinimum.value() != kFloatMissing)
  {
    theParameters.theLog << "theWindchillCoastalAfternoonMinimum: "
                         << theParameters.theWindchillCoastalAfternoonMinimum << '\n';
    theParameters.theLog << "theWindchillCoastalAfternoonMean: "
                         << theParameters.theWindchillCoastalAfternoonMean << '\n';
    theParameters.theLog << "theWindchillCoastalAfternoonMaximum: "
                         << theParameters.theWindchillCoastalAfternoonMaximum << '\n';
  }
}

struct WindResultTriple
{
  WeatherResult* minimum;
  WeatherResult* mean;
  WeatherResult* maximum;
};

WindResultTriple select_inland_wind_results(wind_anomaly_params& p,
                                            const bool& theMorningPeriod,
                                            const bool& theWindspeed)
{
  WindResultTriple r{nullptr, nullptr, nullptr};
  if (theMorningPeriod)
  {
    r.minimum =
        theWindspeed ? &p.theWindspeedInlandMorningMinimum : &p.theWindchillInlandMorningMinimum;
    r.mean =
        theWindspeed ? &p.theWindspeedInlandMorningMean : &p.theWindchillInlandMorningMean;
    r.maximum =
        theWindspeed ? &p.theWindspeedInlandMorningMaximum : &p.theWindchillInlandMorningMaximum;
  }
  else
  {
    r.minimum = theWindspeed ? &p.theWindspeedInlandAfternoonMinimum
                             : &p.theWindchillInlandAfternoonMinimum;
    r.mean =
        theWindspeed ? &p.theWindspeedInlandAfternoonMean : &p.theWindchillInlandAfternoonMean;
    r.maximum = theWindspeed ? &p.theWindspeedInlandAfternoonMaximum
                             : &p.theWindchillInlandAfternoonMaximum;
  }
  return r;
}

WindResultTriple select_coastal_wind_results(wind_anomaly_params& p,
                                             const bool& theMorningPeriod,
                                             const bool& theWindspeed)
{
  WindResultTriple r{nullptr, nullptr, nullptr};
  if (theMorningPeriod)
  {
    r.minimum = theWindspeed ? &p.theWindspeedCoastalMorningMinimum
                             : &p.theWindchillCoastalMorningMinimum;
    r.mean =
        theWindspeed ? &p.theWindspeedCoastalMorningMean : &p.theWindchillCoastalMorningMean;
    r.maximum = theWindspeed ? &p.theWindspeedCoastalMorningMaximum
                             : &p.theWindchillCoastalMorningMaximum;
  }
  else
  {
    r.minimum = theWindspeed ? &p.theWindspeedCoastalAfternoonMinimum
                             : &p.theWindchillCoastalAfternoonMinimum;
    r.mean =
        theWindspeed ? &p.theWindspeedCoastalAfternoonMean : &p.theWindchillCoastalAfternoonMean;
    r.maximum = theWindspeed ? &p.theWindspeedCoastalAfternoonMaximum
                             : &p.theWindchillCoastalAfternoonMaximum;
  }
  return r;
}

WindResultTriple select_wind_results(wind_anomaly_params& p,
                                     const TextGen::WeatherArea::Type& theType,
                                     const bool& theMorningPeriod,
                                     const bool& theWindspeed)
{
  if (theType == WeatherArea::Inland)
    return select_inland_wind_results(p, theMorningPeriod, theWindspeed);
  if (theType == WeatherArea::Coast)
    return select_coastal_wind_results(p, theMorningPeriod, theWindspeed);
  return WindResultTriple{nullptr, nullptr, nullptr};
}

std::string build_wind_postfix(const TextGen::WeatherArea::Type& theType,
                               const bool& theMorningPeriod,
                               const bool& theWindspeed)
{
  std::string postfix(theWindspeed ? "::fake::windspeed" : "::fake::windchill");
  postfix += (theMorningPeriod ? "::morning" : "::afternoon");
  if (theType == WeatherArea::Inland)
    postfix += "::inland";
  else if (theType == WeatherArea::Coast)
    postfix += "::coast";
  return postfix;
}

WeatherPeriod build_wind_period(const WeatherPeriod& theCompletePeriod,
                                const bool& theMorningPeriod)
{
  int noonHour = (theMorningPeriod ? (theCompletePeriod.localEndTime().GetHour() < 12
                                          ? theCompletePeriod.localEndTime().GetHour()
                                          : 12)
                                   : (theCompletePeriod.localStartTime().GetHour() < 12
                                          ? theCompletePeriod.localStartTime().GetHour()
                                          : 12));

  TextGenPosixTime noonTime(theCompletePeriod.localStartTime().GetYear(),
                            theCompletePeriod.localStartTime().GetMonth(),
                            theCompletePeriod.localStartTime().GetDay(),
                            noonHour,
                            0,
                            0);

  TextGenPosixTime theStartTime(theMorningPeriod ? theCompletePeriod.localStartTime() : noonTime);
  TextGenPosixTime theEndTime(theMorningPeriod ? noonTime : theCompletePeriod.localEndTime());

  return WeatherPeriod(
      theStartTime > theEndTime ? theCompletePeriod.localStartTime() : theStartTime,
      theStartTime > theEndTime ? theCompletePeriod.localEndTime() : theEndTime);
}

void analyze_wind_result(WeatherResult* result,
                         const std::string& fakeVar,
                         const std::string& statSuffix,
                         const wind_anomaly_params& theParameters,
                         const WeatherArea& theArea,
                         const WeatherPeriod& thePeriod,
                         const bool& theWindspeed,
                         WeatherFunction stat,
                         WeatherFunction areastat)
{
  if (result == nullptr)
    return;
  *result = GridForecaster().analyze(fakeVar + statSuffix,
                                     theParameters.theSources,
                                     theWindspeed ? WindSpeed : WindChill,
                                     stat,
                                     areastat,
                                     theArea,
                                     thePeriod);
  if (theWindspeed && theArea.type() == WeatherArea::Full)
    WeatherResultTools::checkMissingValue("wind_anomaly", WindSpeed, *result);
}

void calculate_windspeed_and_chill(wind_anomaly_params& theParameters,
                                   const TextGen::WeatherArea::Type& theType,
                                   const bool& theMorningPeriod,
                                   const bool& theWindspeed)
{
  WeatherArea theArea(theParameters.theArea);
  theArea.type(theParameters.theCoastalAndInlandTogetherFlag ? WeatherArea::Full : theType);

  const std::string fakeVariable =
      theParameters.theVariable + build_wind_postfix(theType, theMorningPeriod, theWindspeed);

  WeatherPeriod thePeriod = build_wind_period(theParameters.thePeriod, theMorningPeriod);

  WindResultTriple res = select_wind_results(theParameters, theType, theMorningPeriod, theWindspeed);

  analyze_wind_result(
      res.minimum, fakeVariable, "::min", theParameters, theArea, thePeriod, theWindspeed, Minimum, Maximum);
  analyze_wind_result(
      res.maximum, fakeVariable, "::max", theParameters, theArea, thePeriod, theWindspeed, Maximum, Maximum);
  analyze_wind_result(
      res.mean, fakeVariable, "::mean", theParameters, theArea, thePeriod, theWindspeed, Mean, Maximum);
}

void log_start_time_and_end_time(MessageLogger& theLog,
                                 const std::string& theLogMessage,
                                 const WeatherPeriod& thePeriod)
{
  theLog << NFmiStringTools::Convert(theLogMessage) << thePeriod.localStartTime() << " ... "
         << thePeriod.localEndTime() << '\n';
}

Sentence get_windiness_sentence(const Sentence& timeSpecifier,
                                const std::string& areaSpecifier,
                                const std::string& plainWindinessPhrase,
                                const std::string& windinessPhrase,
                                const bool& timeSpecifierEmpty,
                                const bool& areaSpecifierEmpty)
{
  Sentence sentence;

  if (timeSpecifierEmpty && areaSpecifierEmpty)
    sentence << plainWindinessPhrase;
  else if (!timeSpecifierEmpty && areaSpecifierEmpty)
    sentence << ILTAPAIVALLA_ON_TUULISTA_COMPOSITE_PHRASE << timeSpecifier << windinessPhrase;
  else if (timeSpecifierEmpty && !areaSpecifierEmpty)
    sentence << RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE << areaSpecifier << windinessPhrase;
  else
    sentence << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE << timeSpecifier
             << areaSpecifier << windinessPhrase;

  return sentence;
}

Sentence get_windiness_sentence(const std::string& timeSpecifier,
                                const std::string& areaSpecifier,
                                const std::string& plainWindinessPhrase,
                                const std::string& windinessPhrase,
                                const bool& timeSpecifierEmpty,
                                const bool& areaSpecifierEmpty)
{
  Sentence sentence;
  Sentence timeSentence;
  timeSentence << timeSpecifier;

  sentence << get_windiness_sentence(timeSentence,
                                     areaSpecifier,
                                     plainWindinessPhrase,
                                     windinessPhrase,
                                     timeSpecifierEmpty,
                                     areaSpecifierEmpty);
  return sentence;
}

Sentence construct_windiness_sentence_for_area(const float& windspeedMorning,
                                               const float& windspeedAfternoon,
                                               const float& windyWeatherLimit,
                                               const float& extremelyWindyWeatherLimit,
                                               const Sentence& specifiedDaySentence,
                                               const short& dayNumber,
                                               const std::string& areaString,
                                               const std::string& morningWord,
                                               const std::string& afternoonWord)
{
  Sentence sentence;

  bool morningIncluded = windspeedMorning != kFloatMissing;
  bool afternoonIncluded = windspeedAfternoon != kFloatMissing;

  Sentence specifiedDay;
  if (specifiedDaySentence.empty())
    specifiedDay << EMPTY_STRING;
  else
    specifiedDay << specifiedDaySentence;

  std::string weekdayMorningString(parse_weekday_phrase(dayNumber, morningWord));
  std::string weekdayAfternoonString(parse_weekday_phrase(dayNumber, afternoonWord));
  bool areaStringEmpty(areaString == EMPTY_STRING);
  bool specifiedDayEmpty(specifiedDaySentence.empty());
  bool weekdayMorningStringEmpty(weekdayMorningString == EMPTY_STRING);
  bool weekdayAfternoonStringEmpty(weekdayAfternoonString == EMPTY_STRING);

  if (morningIncluded && afternoonIncluded)
  {
    if (windspeedMorning >= extremelyWindyWeatherLimit &&
        windspeedAfternoon >= extremelyWindyWeatherLimit)
    {
      sentence << get_windiness_sentence(specifiedDay,
                                         areaString,
                                         SAA_ON_HYVIN_TUULINEN_PHRASE,
                                         HYVIN_TUULISTA_PHRASE,
                                         specifiedDayEmpty,
                                         areaStringEmpty);

      /*
if(areaStringEmpty && specifiedDayEmpty)
    sentence << SAA_ON_HYVIN_TUULINEN_PHRASE;
else if(areaStringEmpty && !specifiedDayEmpty)
    sentence << ILTAPAIVALLA_ON_TUULISTA_COMPOSITE_PHRASE
                     << specifiedDay
                     << HYVIN_TUULISTA_PHRASE;
else if(!areaStringEmpty && specifiedDayEmpty)
    sentence << RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
                     << areaString
                     << HYVIN_TUULISTA_PHRASE;
else
    sentence << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
                     << specifiedDay
                     << areaString
                     << HYVIN_TUULISTA_PHRASE;
      */
    }
    else if (windspeedMorning >= extremelyWindyWeatherLimit)
    {
      sentence << get_windiness_sentence(weekdayMorningString,
                                         areaString,
                                         SAA_ON_HYVIN_TUULINEN_PHRASE,
                                         HYVIN_TUULISTA_PHRASE,
                                         weekdayMorningStringEmpty,
                                         areaStringEmpty);

      /*
      if(areaStringEmpty && weekdayMorningStringEmpty)
            sentence << SAA_ON_HYVIN_TUULINEN_PHRASE;
      else
            sentence << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
                             << weekdayMorningString
                             << areaString
                             << HYVIN_TUULISTA_PHRASE;
      */
    }
    else if (windspeedAfternoon >= extremelyWindyWeatherLimit)
    {
      sentence << get_windiness_sentence(weekdayAfternoonString,
                                         areaString,
                                         SAA_ON_HYVIN_TUULINEN_PHRASE,
                                         HYVIN_TUULISTA_PHRASE,
                                         weekdayAfternoonStringEmpty,
                                         areaStringEmpty);

      /*
      if(areaStringEmpty && weekdayAfternoonStringEmpty)
            sentence << SAA_ON_HYVIN_TUULINEN_PHRASE;
      else
            sentence << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
                             << weekdayAfternoonString
                             << areaString
                             << HYVIN_TUULISTA_PHRASE;
      */
    }
    else if (windspeedMorning >= windyWeatherLimit && windspeedAfternoon >= windyWeatherLimit)
    {
      sentence << get_windiness_sentence(specifiedDay,
                                         areaString,
                                         SAA_ON_TUULINEN_PHRASE,
                                         TUULISTA_WORD,
                                         specifiedDayEmpty,
                                         areaStringEmpty);

      /*
      if(areaStringEmpty && specifiedDayEmpty)
            sentence << SAA_ON_TUULINEN_PHRASE;
      else
            sentence << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
                             << specifiedDay
                             << areaString
                             << TUULISTA_WORD;
      */
    }
    else if (windspeedMorning >= windyWeatherLimit)
    {
      sentence << get_windiness_sentence(weekdayMorningString,
                                         areaString,
                                         SAA_ON_TUULINEN_PHRASE,
                                         TUULISTA_WORD,
                                         weekdayMorningStringEmpty,
                                         areaStringEmpty);

      /*
      if(areaStringEmpty && weekdayMorningStringEmpty)
            sentence << SAA_ON_TUULINEN_PHRASE;
      else
            sentence << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
                             << weekdayMorningString
                             << areaString
                             << TUULISTA_WORD;
      */
    }
    else if (windspeedAfternoon >= windyWeatherLimit)
    {
      sentence << get_windiness_sentence(weekdayAfternoonString,
                                         areaString,
                                         SAA_ON_TUULINEN_PHRASE,
                                         TUULISTA_WORD,
                                         weekdayAfternoonStringEmpty,
                                         areaStringEmpty);
      /*
      if(areaStringEmpty && weekdayAfternoonStringEmpty)
            sentence << SAA_ON_TUULINEN_PHRASE;
      else
            sentence << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
                             << weekdayAfternoonString
                             << areaString
                             << TUULISTA_WORD;
      */
    }
  }
  else if (morningIncluded)
  {
    if (windspeedMorning >= extremelyWindyWeatherLimit)
    {
      sentence << get_windiness_sentence(specifiedDay,
                                         areaString,
                                         SAA_ON_HYVIN_TUULINEN_PHRASE,
                                         HYVIN_TUULISTA_PHRASE,
                                         specifiedDayEmpty,
                                         areaStringEmpty);
      /*
      if(areaStringEmpty && specifiedDayEmpty)
            sentence << SAA_ON_HYVIN_TUULINEN_PHRASE;
      else
            sentence << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
                             << specifiedDay
                             << areaString
                             << HYVIN_TUULISTA_PHRASE;
      */
    }
    else if (windspeedMorning >= windyWeatherLimit)
    {
      sentence << get_windiness_sentence(specifiedDay,
                                         areaString,
                                         SAA_ON_TUULINEN_PHRASE,
                                         TUULISTA_WORD,
                                         specifiedDayEmpty,
                                         areaStringEmpty);
      /*
      if(areaStringEmpty && specifiedDayEmpty)
            sentence << SAA_ON_TUULINEN_PHRASE;
      else
            sentence << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
                             << specifiedDay
                             << areaString
                             << TUULISTA_WORD;
      */
    }
  }
  else if (afternoonIncluded)
  {
    if (windspeedAfternoon >= extremelyWindyWeatherLimit)
    {
      sentence << get_windiness_sentence(specifiedDay,
                                         areaString,
                                         SAA_ON_HYVIN_TUULINEN_PHRASE,
                                         HYVIN_TUULISTA_PHRASE,
                                         specifiedDayEmpty,
                                         areaStringEmpty);
      /*
      if(areaStringEmpty && specifiedDayEmpty)
      if(areaStringEmpty && specifiedDayEmpty)
            sentence << SAA_ON_HYVIN_TUULINEN_PHRASE;
      else
            sentence << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
                             << specifiedDay
                             << areaString
                             << HYVIN_TUULISTA_PHRASE;
      */
    }
    else if (windspeedAfternoon >= windyWeatherLimit)
    {
      sentence << get_windiness_sentence(specifiedDay,
                                         areaString,
                                         SAA_ON_TUULINEN_PHRASE,
                                         TUULISTA_WORD,
                                         specifiedDayEmpty,
                                         areaStringEmpty);
      /*
      if(areaStringEmpty && specifiedDayEmpty)
            sentence << SAA_ON_TUULINEN_PHRASE;
      else
            sentence << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
                             << specifiedDay
                             << areaString
                             << TUULISTA_WORD;
      */
    }
  }

  return sentence;
}

// Helper: emit sentence based on presence of time/area specifiers
void emit_windy_sentence(Sentence& sentence,
                         bool specifiedDayEmpty,
                         const Sentence& specifiedDay,
                         const std::string& phrase_plain,
                         const std::string& phrase_windy)
{
  if (specifiedDayEmpty)
    sentence << phrase_plain;
  else
    sentence << ILTAPAIVALLA_ON_TUULISTA_COMPOSITE_PHRASE << specifiedDay << phrase_windy;
}

void emit_coastal_windy_sentence(Sentence& sentence,
                                 bool specifiedDayEmpty,
                                 const Sentence& specifiedDay,
                                 const std::string& phrase_windy)
{
  if (specifiedDayEmpty)
    sentence << RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE << RANNIKOLLA_WORD << phrase_windy;
  else
    sentence << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE << specifiedDay
             << RANNIKOLLA_WORD << phrase_windy;
}

// Helper for coast-morning-very-windy branch in both-areas morning+afternoon case
Sentence cws_coast_morning_extreme_afternoon_extreme(int windspeedMorningInland,
                                                     int windspeedAfternoonInland,
                                                     float ewl,
                                                     bool specifiedDayEmpty,
                                                     const Sentence& specifiedDay)
{
  Sentence s;
  if (windspeedMorningInland >= ewl - 1.0 || windspeedAfternoonInland >= ewl - 1.0)
    emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_HYVIN_TUULINEN_PHRASE, HYVIN_TUULINEN_PHRASE);
  else
    emit_coastal_windy_sentence(s, specifiedDayEmpty, specifiedDay, HYVIN_TUULISTA_PHRASE);
  return s;
}

Sentence cws_coast_morning_extreme_afternoon_windy(int windspeedMorningInland,
                                                   int windspeedAfternoonInland,
                                                   float wl,
                                                   float ewl,
                                                   bool specifiedDayEmpty,
                                                   const Sentence& specifiedDay,
                                                   short dayNumber,
                                                   const std::string& iltapaivalla)
{
  Sentence s;
  if (windspeedMorningInland >= ewl - 1.0 && windspeedAfternoonInland >= ewl - 1.0)
    emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_HYVIN_TUULINEN_PHRASE, HYVIN_TUULINEN_PHRASE);
  else if (windspeedMorningInland >= wl - 0.5 || windspeedAfternoonInland >= wl - 0.5)
    emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_TUULINEN_PHRASE, TUULISTA_WORD);
  else
    s << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
      << parse_weekday_phrase(dayNumber, iltapaivalla) << RANNIKOLLA_WORD << HYVIN_TUULISTA_PHRASE;
  return s;
}

Sentence cws_coast_morning_extreme_afternoon_calm(int windspeedMorningInland,
                                                  int windspeedAfternoonInland,
                                                  float ewl,
                                                  bool specifiedDayEmpty,
                                                  const Sentence& specifiedDay,
                                                  short dayNumber,
                                                  const std::string& aamupaivalla)
{
  Sentence s;
  if (windspeedMorningInland >= ewl - 1.0 && windspeedAfternoonInland >= ewl - 1.0)
    emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_HYVIN_TUULINEN_PHRASE, HYVIN_TUULINEN_PHRASE);
  else if (windspeedMorningInland >= ewl - 1.0)
    s << ILTAPAIVALLA_ON_TUULISTA_COMPOSITE_PHRASE
      << parse_weekday_phrase(dayNumber, aamupaivalla) << HYVIN_TUULINEN_PHRASE;
  else
    s << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
      << parse_weekday_phrase(dayNumber, aamupaivalla) << RANNIKOLLA_WORD << HYVIN_TUULISTA_PHRASE;
  return s;
}

Sentence cws_coast_morning_extreme(int windspeedMorningInland,
                                   int windspeedAfternoonInland,
                                   int windspeedAfternoonCoastal,
                                   float wl,
                                   float ewl,
                                   bool specifiedDayEmpty,
                                   const Sentence& specifiedDay,
                                   short dayNumber,
                                   const std::string& aamupaivalla,
                                   const std::string& iltapaivalla)
{
  if (windspeedAfternoonCoastal >= ewl)
    return cws_coast_morning_extreme_afternoon_extreme(
        windspeedMorningInland, windspeedAfternoonInland, ewl, specifiedDayEmpty, specifiedDay);
  if (windspeedAfternoonCoastal >= wl)
    return cws_coast_morning_extreme_afternoon_windy(windspeedMorningInland,
                                                     windspeedAfternoonInland,
                                                     wl,
                                                     ewl,
                                                     specifiedDayEmpty,
                                                     specifiedDay,
                                                     dayNumber,
                                                     iltapaivalla);
  return cws_coast_morning_extreme_afternoon_calm(windspeedMorningInland,
                                                  windspeedAfternoonInland,
                                                  ewl,
                                                  specifiedDayEmpty,
                                                  specifiedDay,
                                                  dayNumber,
                                                  aamupaivalla);
}

Sentence cws_coast_morning_windy_afternoon_extreme(int windspeedMorningInland,
                                                   int windspeedAfternoonInland,
                                                   float wl,
                                                   float ewl,
                                                   bool specifiedDayEmpty,
                                                   const Sentence& specifiedDay,
                                                   short dayNumber,
                                                   const std::string& iltapaivalla)
{
  Sentence s;
  if (windspeedMorningInland >= ewl - 1.0 || windspeedAfternoonInland >= ewl - 1.0)
    emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_HYVIN_TUULINEN_PHRASE, HYVIN_TUULINEN_PHRASE);
  else if (windspeedMorningInland >= wl - 0.5 || windspeedAfternoonInland >= wl - 0.5)
    emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_TUULINEN_PHRASE, TUULISTA_WORD);
  else
    s << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
      << parse_weekday_phrase(dayNumber, iltapaivalla) << RANNIKOLLA_WORD << HYVIN_TUULISTA_PHRASE;
  return s;
}

Sentence cws_coast_morning_windy_afternoon_windy(int windspeedMorningInland,
                                                 int windspeedAfternoonInland,
                                                 float wl,
                                                 float ewl,
                                                 bool specifiedDayEmpty,
                                                 const Sentence& specifiedDay)
{
  Sentence s;
  if (windspeedMorningInland >= ewl - 1.0 && windspeedAfternoonInland >= ewl - 1.0)
    emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_HYVIN_TUULINEN_PHRASE, HYVIN_TUULINEN_PHRASE);
  else if (windspeedMorningInland >= wl - 0.5 || windspeedAfternoonInland >= wl - 0.5)
    emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_TUULINEN_PHRASE, TUULISTA_WORD);
  else
    emit_coastal_windy_sentence(s, specifiedDayEmpty, specifiedDay, TUULISTA_WORD);
  return s;
}

Sentence cws_coast_morning_windy_afternoon_calm(int windspeedMorningInland,
                                                int windspeedAfternoonInland,
                                                float wl,
                                                float ewl,
                                                bool specifiedDayEmpty,
                                                const Sentence& specifiedDay,
                                                short dayNumber,
                                                const std::string& aamupaivalla)
{
  Sentence s;
  if (windspeedMorningInland >= ewl - 1.0 && windspeedAfternoonInland >= ewl - 1.0)
    emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_HYVIN_TUULINEN_PHRASE, HYVIN_TUULINEN_PHRASE);
  else if (windspeedMorningInland >= wl - 0.5 && windspeedAfternoonInland >= wl - 0.5)
    emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_TUULINEN_PHRASE, TUULISTA_WORD);
  else if (windspeedMorningInland >= wl - 0.5)
    s << ILTAPAIVALLA_ON_TUULISTA_COMPOSITE_PHRASE
      << parse_weekday_phrase(dayNumber, aamupaivalla) << TUULISTA_WORD;
  else
    s << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
      << parse_weekday_phrase(dayNumber, aamupaivalla) << RANNIKOLLA_WORD << TUULISTA_WORD;
  return s;
}

Sentence cws_coast_morning_windy(int windspeedMorningInland,
                                 int windspeedAfternoonInland,
                                 int windspeedAfternoonCoastal,
                                 float wl,
                                 float ewl,
                                 bool specifiedDayEmpty,
                                 const Sentence& specifiedDay,
                                 short dayNumber,
                                 const std::string& aamupaivalla,
                                 const std::string& iltapaivalla)
{
  if (windspeedAfternoonCoastal >= ewl)
    return cws_coast_morning_windy_afternoon_extreme(windspeedMorningInland,
                                                     windspeedAfternoonInland,
                                                     wl,
                                                     ewl,
                                                     specifiedDayEmpty,
                                                     specifiedDay,
                                                     dayNumber,
                                                     iltapaivalla);
  if (windspeedAfternoonCoastal >= wl)
    return cws_coast_morning_windy_afternoon_windy(windspeedMorningInland,
                                                   windspeedAfternoonInland,
                                                   wl,
                                                   ewl,
                                                   specifiedDayEmpty,
                                                   specifiedDay);
  return cws_coast_morning_windy_afternoon_calm(windspeedMorningInland,
                                                windspeedAfternoonInland,
                                                wl,
                                                ewl,
                                                specifiedDayEmpty,
                                                specifiedDay,
                                                dayNumber,
                                                aamupaivalla);
}

Sentence cws_coast_morning_calm_afternoon_extreme(int windspeedMorningInland,
                                                  int windspeedAfternoonInland,
                                                  float wl,
                                                  float ewl,
                                                  bool specifiedDayEmpty,
                                                  const Sentence& specifiedDay,
                                                  short dayNumber,
                                                  const std::string& iltapaivalla)
{
  Sentence s;
  if (windspeedMorningInland >= ewl - 1.0 && windspeedAfternoonInland >= ewl - 1.0)
    emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_HYVIN_TUULINEN_PHRASE, HYVIN_TUULINEN_PHRASE);
  else if (windspeedAfternoonInland >= ewl - 1.0)
    s << ILTAPAIVALLA_ON_TUULISTA_COMPOSITE_PHRASE
      << parse_weekday_phrase(dayNumber, iltapaivalla) << HYVIN_TUULINEN_PHRASE;
  else if (windspeedMorningInland >= wl - 0.5 && windspeedAfternoonInland >= wl - 0.5)
    emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_TUULINEN_PHRASE, TUULISTA_WORD);
  else
    s << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
      << parse_weekday_phrase(dayNumber, iltapaivalla) << RANNIKOLLA_WORD << HYVIN_TUULISTA_PHRASE;
  return s;
}

Sentence cws_coast_morning_calm_afternoon_windy(int windspeedMorningInland,
                                                int windspeedAfternoonInland,
                                                float wl,
                                                float ewl,
                                                bool specifiedDayEmpty,
                                                const Sentence& specifiedDay,
                                                short dayNumber,
                                                const std::string& iltapaivalla)
{
  Sentence s;
  if (windspeedMorningInland >= ewl - 1.0 && windspeedAfternoonInland >= ewl - 1.0)
    emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_HYVIN_TUULINEN_PHRASE, HYVIN_TUULINEN_PHRASE);
  else if (windspeedMorningInland >= wl - 0.5 && windspeedAfternoonInland >= wl - 0.5)
    emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_TUULINEN_PHRASE, TUULISTA_WORD);
  else if (windspeedAfternoonInland >= wl - 0.5)
    s << ILTAPAIVALLA_ON_TUULISTA_COMPOSITE_PHRASE
      << parse_weekday_phrase(dayNumber, iltapaivalla) << TUULISTA_WORD;
  else
    s << ILTAPAIVALLA_RANNIKOLLA_ON_TUULISTA_COMPOSITE_PHRASE
      << parse_weekday_phrase(dayNumber, iltapaivalla) << RANNIKOLLA_WORD << TUULISTA_WORD;
  return s;
}

Sentence cws_coast_morning_calm(int windspeedMorningInland,
                                int windspeedAfternoonInland,
                                int windspeedAfternoonCoastal,
                                float wl,
                                float ewl,
                                const Sentence& theSpecifiedDay,
                                bool specifiedDayEmpty,
                                const Sentence& specifiedDay,
                                short dayNumber,
                                const std::string& aamupaivalla,
                                const std::string& iltapaivalla)
{
  if (windspeedAfternoonCoastal >= ewl)
    return cws_coast_morning_calm_afternoon_extreme(windspeedMorningInland,
                                                    windspeedAfternoonInland,
                                                    wl,
                                                    ewl,
                                                    specifiedDayEmpty,
                                                    specifiedDay,
                                                    dayNumber,
                                                    iltapaivalla);
  if (windspeedAfternoonCoastal >= wl)
    return cws_coast_morning_calm_afternoon_windy(windspeedMorningInland,
                                                  windspeedAfternoonInland,
                                                  wl,
                                                  ewl,
                                                  specifiedDayEmpty,
                                                  specifiedDay,
                                                  dayNumber,
                                                  iltapaivalla);
  return construct_windiness_sentence_for_area(windspeedMorningInland,
                                               windspeedAfternoonInland,
                                               wl,
                                               ewl,
                                               theSpecifiedDay,
                                               dayNumber,
                                               EMPTY_STRING,
                                               aamupaivalla,
                                               iltapaivalla);
}

// Both inland and coast included, both morning and afternoon included
Sentence cws_both_morning_and_afternoon(int windspeedMorningInland,
                                        int windspeedAfternoonInland,
                                        int windspeedMorningCoastal,
                                        int windspeedAfternoonCoastal,
                                        float wl,
                                        float ewl,
                                        const Sentence& theSpecifiedDay,
                                        bool specifiedDayEmpty,
                                        const Sentence& specifiedDay,
                                        short dayNumber,
                                        const std::string& aamupaivalla,
                                        const std::string& iltapaivalla)
{
  if (windspeedMorningCoastal >= ewl)
    return cws_coast_morning_extreme(windspeedMorningInland,
                                     windspeedAfternoonInland,
                                     windspeedAfternoonCoastal,
                                     wl,
                                     ewl,
                                     specifiedDayEmpty,
                                     specifiedDay,
                                     dayNumber,
                                     aamupaivalla,
                                     iltapaivalla);
  if (windspeedMorningCoastal >= wl)
    return cws_coast_morning_windy(windspeedMorningInland,
                                   windspeedAfternoonInland,
                                   windspeedAfternoonCoastal,
                                   wl,
                                   ewl,
                                   specifiedDayEmpty,
                                   specifiedDay,
                                   dayNumber,
                                   aamupaivalla,
                                   iltapaivalla);
  return cws_coast_morning_calm(windspeedMorningInland,
                                windspeedAfternoonInland,
                                windspeedAfternoonCoastal,
                                wl,
                                ewl,
                                theSpecifiedDay,
                                specifiedDayEmpty,
                                specifiedDay,
                                dayNumber,
                                aamupaivalla,
                                iltapaivalla);
}

// Both inland and coast included, only morning included
Sentence cws_both_morning_only(int windspeedMorningInland,
                               int windspeedMorningCoastal,
                               float wl,
                               float ewl,
                               bool specifiedDayEmpty,
                               const Sentence& specifiedDay)
{
  Sentence s;
  if (windspeedMorningCoastal >= ewl)
  {
    if (windspeedMorningInland >= ewl - 1.0)
      emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_HYVIN_TUULINEN_PHRASE, HYVIN_TUULINEN_PHRASE);
    else
      emit_coastal_windy_sentence(s, specifiedDayEmpty, specifiedDay, HYVIN_TUULISTA_PHRASE);
  }
  else if (windspeedMorningCoastal >= wl)
  {
    if (windspeedMorningInland >= ewl)
      emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_HYVIN_TUULINEN_PHRASE, HYVIN_TUULINEN_PHRASE);
    else if (windspeedMorningInland >= wl - 0.5)
      emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_TUULINEN_PHRASE, TUULISTA_WORD);
    else
      emit_coastal_windy_sentence(s, specifiedDayEmpty, specifiedDay, TUULISTA_WORD);
  }
  else
  {
    if (windspeedMorningInland >= ewl)
      emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_HYVIN_TUULINEN_PHRASE, HYVIN_TUULINEN_PHRASE);
    else if (windspeedMorningInland >= wl - 0.5)
      emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_TUULINEN_PHRASE, TUULISTA_WORD);
  }
  return s;
}

// Both inland and coast included, only afternoon included
Sentence cws_both_afternoon_only(int windspeedAfternoonInland,
                                 int windspeedAfternoonCoastal,
                                 float wl,
                                 float ewl,
                                 bool specifiedDayEmpty,
                                 const Sentence& specifiedDay)
{
  Sentence s;
  if (windspeedAfternoonCoastal >= ewl)
  {
    if (windspeedAfternoonInland >= ewl - 1.0)
      emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_HYVIN_TUULINEN_PHRASE, HYVIN_TUULINEN_PHRASE);
    else
      emit_coastal_windy_sentence(s, specifiedDayEmpty, specifiedDay, HYVIN_TUULISTA_PHRASE);
  }
  else if (windspeedAfternoonCoastal >= wl)
  {
    if (windspeedAfternoonInland >= ewl)
      emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_HYVIN_TUULINEN_PHRASE, HYVIN_TUULINEN_PHRASE);
    else if (windspeedAfternoonInland >= wl - 0.5)
      emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_TUULINEN_PHRASE, TUULISTA_WORD);
    else
      emit_coastal_windy_sentence(s, specifiedDayEmpty, specifiedDay, TUULISTA_WORD);
  }
  else
  {
    if (windspeedAfternoonInland >= ewl)
      emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_HYVIN_TUULINEN_PHRASE, HYVIN_TUULINEN_PHRASE);
    else if (windspeedAfternoonInland >= wl - 0.5)
      emit_windy_sentence(s, specifiedDayEmpty, specifiedDay, SAA_ON_TUULINEN_PHRASE, TUULISTA_WORD);
  }
  return s;
}

Sentence construct_windiness_sentence(const wind_anomaly_params& theParameters,
                                      const Sentence& theSpecifiedDay,
                                      const short& dayNumber)
{
  Sentence sentence;

  bool inlandIncluded = theParameters.theWindspeedInlandMorningMinimum.value() != kFloatMissing ||
                        theParameters.theWindspeedInlandAfternoonMinimum.value() != kFloatMissing;
  bool coastIncluded = theParameters.theWindspeedCoastalMorningMinimum.value() != kFloatMissing ||
                       theParameters.theWindspeedCoastalAfternoonMinimum.value() != kFloatMissing;

  float wl = Settings::optional_double(
      theParameters.theVariable + "::windy_weather_limit", WINDY_WEATER_LIMIT);
  float ewl = Settings::optional_double(
      theParameters.theVariable + "::extremely_windy_weather_limit", EXTREMELY_WINDY_WEATHER_LIMIT);

  const std::string& aamupaivalla = theParameters.theMorningWord;
  const std::string& iltapaivalla = theParameters.theAfternoonWord;

  int windspeedMorningInland =
      static_cast<int>(round(theParameters.theWindspeedInlandMorningMean.value()));
  int windspeedAfternoonInland =
      static_cast<int>(round(theParameters.theWindspeedInlandAfternoonMean.value()));
  int windspeedMorningCoastal =
      static_cast<int>(round(theParameters.theWindspeedCoastalMorningMean.value()));
  int windspeedAfternoonCoastal =
      static_cast<int>(round(theParameters.theWindspeedCoastalAfternoonMean.value()));

  bool specifiedDayEmpty(theSpecifiedDay.empty());
  Sentence specifiedDay;
  if (specifiedDayEmpty)
    specifiedDay << EMPTY_STRING;
  else
    specifiedDay << theSpecifiedDay;

  if (inlandIncluded && coastIncluded)
  {
    bool morningIncluded = windspeedMorningInland != kFloatMissing;
    bool afternoonIncluded = windspeedAfternoonInland != kFloatMissing;

    if (morningIncluded && afternoonIncluded)
      sentence << cws_both_morning_and_afternoon(windspeedMorningInland,
                                                 windspeedAfternoonInland,
                                                 windspeedMorningCoastal,
                                                 windspeedAfternoonCoastal,
                                                 wl,
                                                 ewl,
                                                 theSpecifiedDay,
                                                 specifiedDayEmpty,
                                                 specifiedDay,
                                                 dayNumber,
                                                 aamupaivalla,
                                                 iltapaivalla);
    else if (morningIncluded)
      sentence << cws_both_morning_only(windspeedMorningInland,
                                        windspeedMorningCoastal,
                                        wl,
                                        ewl,
                                        specifiedDayEmpty,
                                        specifiedDay);
    else if (afternoonIncluded)
      sentence << cws_both_afternoon_only(windspeedAfternoonInland,
                                          windspeedAfternoonCoastal,
                                          wl,
                                          ewl,
                                          specifiedDayEmpty,
                                          specifiedDay);
  }
  else if (inlandIncluded)
  {
    sentence << construct_windiness_sentence_for_area(windspeedMorningInland,
                                                      windspeedAfternoonInland,
                                                      wl,
                                                      ewl,
                                                      theSpecifiedDay,
                                                      dayNumber,
                                                      EMPTY_STRING,
                                                      aamupaivalla,
                                                      iltapaivalla);
  }
  else if (coastIncluded)
  {
    sentence << construct_windiness_sentence_for_area(windspeedMorningCoastal,
                                                      windspeedAfternoonCoastal,
                                                      wl,
                                                      ewl,
                                                      theSpecifiedDay,
                                                      dayNumber,
                                                      EMPTY_STRING,
                                                      aamupaivalla,
                                                      iltapaivalla);
  }

  return sentence;
}

struct WindCoolingContext
{
  float temperature{-1.0};
  std::string areaString{EMPTY_STRING};
  std::string part_of_the_day;
};

WindCoolingContext determine_wind_cooling_both_areas(bool windCoolingInlandMorning,
                                                     bool windCoolingCoastalMorning,
                                                     bool windCoolingInlandAfternoon,
                                                     bool windCoolingCoastalAfternoon,
                                                     int tempInlandMorning,
                                                     int tempInlandAfternoon,
                                                     int tempCoastalMorning,
                                                     int tempCoastalAfternoon,
                                                     const std::string& aamupaivalla,
                                                     const std::string& iltapaivalla)
{
  WindCoolingContext ctx;
  if (windCoolingInlandMorning && windCoolingCoastalMorning && windCoolingInlandAfternoon &&
      windCoolingCoastalAfternoon)
  {
    float morningMax = std::max(tempInlandMorning, tempCoastalMorning);
    float afternoonMax = std::max(tempInlandAfternoon, tempCoastalAfternoon);
    ctx.temperature = std::max(morningMax, afternoonMax);
  }
  else if (!windCoolingInlandMorning && windCoolingCoastalMorning && !windCoolingInlandAfternoon &&
           windCoolingCoastalAfternoon)
  {
    ctx.temperature = std::max(tempCoastalAfternoon, tempCoastalMorning);
    ctx.areaString = RANNIKOLLA_WORD;
  }
  else if (!windCoolingInlandMorning && windCoolingCoastalMorning && !windCoolingInlandAfternoon &&
           !windCoolingCoastalAfternoon)
  {
    ctx.temperature = tempCoastalMorning;
    ctx.part_of_the_day = aamupaivalla;
    ctx.areaString = RANNIKOLLA_WORD;
  }
  else if (!windCoolingInlandMorning && !windCoolingCoastalMorning && !windCoolingInlandAfternoon &&
           windCoolingCoastalAfternoon)
  {
    ctx.temperature = tempCoastalAfternoon;
    ctx.part_of_the_day = iltapaivalla;
    ctx.areaString = RANNIKOLLA_WORD;
  }
  else if (windCoolingInlandMorning && !windCoolingCoastalMorning && windCoolingInlandAfternoon &&
           !windCoolingCoastalAfternoon)
  {
    ctx.temperature = std::max(tempInlandAfternoon, tempInlandMorning);
    ctx.areaString = SISAMAASSA_WORD;
  }
  else if (windCoolingInlandMorning && !windCoolingCoastalMorning && !windCoolingInlandAfternoon &&
           !windCoolingCoastalAfternoon)
  {
    ctx.temperature = tempInlandMorning;
    ctx.part_of_the_day = aamupaivalla;
    ctx.areaString = SISAMAASSA_WORD;
  }
  else if (!windCoolingInlandMorning && !windCoolingCoastalMorning && windCoolingInlandAfternoon &&
           !windCoolingCoastalAfternoon)
  {
    ctx.temperature = tempInlandAfternoon;
    ctx.part_of_the_day = iltapaivalla;
    ctx.areaString = SISAMAASSA_WORD;
  }
  else if (windCoolingInlandMorning && windCoolingCoastalMorning)
  {
    ctx.temperature = std::max(tempCoastalMorning, tempInlandMorning);
    ctx.part_of_the_day = aamupaivalla;
  }
  else if (windCoolingInlandAfternoon && windCoolingCoastalAfternoon)
  {
    ctx.temperature = std::max(tempCoastalAfternoon, tempInlandAfternoon);
    ctx.part_of_the_day = iltapaivalla;
  }
  return ctx;
}

WindCoolingContext determine_wind_cooling_coast_only(bool windCoolingCoastalMorning,
                                                     bool windCoolingCoastalAfternoon,
                                                     int tempCoastalMorning,
                                                     int tempCoastalAfternoon,
                                                     const std::string& aamupaivalla,
                                                     const std::string& iltapaivalla)
{
  WindCoolingContext ctx;
  if (windCoolingCoastalMorning && !windCoolingCoastalAfternoon)
  {
    ctx.temperature = tempCoastalMorning;
    ctx.part_of_the_day = aamupaivalla;
  }
  else if (!windCoolingCoastalMorning && windCoolingCoastalAfternoon)
  {
    ctx.temperature = tempCoastalAfternoon;
    ctx.part_of_the_day = iltapaivalla;
  }
  else
  {
    ctx.temperature = std::max(tempCoastalAfternoon, tempCoastalMorning);
  }
  return ctx;
}

WindCoolingContext determine_wind_cooling_inland_only(bool windCoolingInlandMorning,
                                                      bool windCoolingInlandAfternoon,
                                                      int tempInlandMorning,
                                                      int tempInlandAfternoon,
                                                      const std::string& aamupaivalla,
                                                      const std::string& iltapaivalla)
{
  WindCoolingContext ctx;
  if (windCoolingInlandMorning && !windCoolingInlandAfternoon)
  {
    ctx.temperature = tempInlandMorning;
    ctx.part_of_the_day = aamupaivalla;
  }
  else if (!windCoolingInlandMorning && windCoolingInlandAfternoon)
  {
    ctx.temperature = tempInlandAfternoon;
    ctx.part_of_the_day = iltapaivalla;
  }
  else
  {
    ctx.temperature = std::max(tempInlandAfternoon, tempInlandMorning);
  }
  return ctx;
}

void emit_wind_cooling_sentence(Sentence& sentence,
                                float temperature,
                                const std::string& areaString,
                                const std::string& timePhrase,
                                const std::string& part_of_the_day,
                                short dayNumber)
{
  bool hasArea = (areaString != EMPTY_STRING);
  bool hasTime = (timePhrase != EMPTY_STRING);

  if (temperature > TUULI_KYLMENTAA_SAATA_LOWER_LIMIT &&
      temperature <= TUULI_KYLMENTAA_SAATA_UPPER_LIMIT)
  {
    if (!hasArea && !hasTime)
      sentence << TUULI_SAA_SAAN_TUNTUMAAN_KYLMEMMALTA_PHRASE;
    else if (hasArea && !hasTime)
      sentence << RANNIKOLLA_TUULI_SAA_SAAN_TUNTUMAAN_KYLMEMMALTA_COMPOSITE_PHRASE << areaString;
    else if (!hasArea && hasTime)
      sentence << ILTAPAIVALLA_TUULI_SAA_SAAN_TUNTUMAAN_KYLMEMMALTA_COMPOSITE_PHRASE << timePhrase;
    else
      sentence << ILTAPAIVALLA_RANNIKOLLA_TUULI_SAA_SAAN_TUNTUMAAN_KYLMEMMALTA_COMPOSITE_PHRASE
               << parse_weekday_phrase(dayNumber, part_of_the_day) << areaString;
  }
  else if (temperature > TUULI_VIILENTAA_SAATA_LOWER_LIMIT &&
           temperature <= TUULI_VIILENTAA_SAATA_UPPER_LIMIT)
  {
    if (!hasArea && !hasTime)
      sentence << TUULI_SAA_SAAN_TUNTUMAAN_VIILEAMMALTA_PHRASE;
    else if (hasArea && !hasTime)
      sentence << RANNIKOLLA_TUULI_SAA_SAAN_TUNTUMAAN_VIILEAMMALTA_COMPOSITE_PHRASE << areaString;
    else if (!hasArea && hasTime)
      sentence << ILTAPAIVALLA_TUULI_SAA_SAAN_TUNTUMAAN_VIILEAMMALTA_COMPOSITE_PHRASE << timePhrase;
    else
      sentence << ILTAPAIVALLA_RANNIKOLLA_TUULI_SAA_SAAN_TUNTUMAAN_VIILEAMMALTA_COMPOSITE_PHRASE
               << parse_weekday_phrase(dayNumber, part_of_the_day) << areaString;
  }
}

Sentence windiness_sentence(const wind_anomaly_params& theParameters)
{
  Sentence sentence;

  bool generate_windiness_sentence =
      Settings::optional_bool(theParameters.theVariable + "::generate_windiness_sentence", true);
  bool generate_wind_cooling_sentence =
      Settings::optional_bool(theParameters.theVariable + "::generate_wind_cooling_sentence", true);

  const std::string& aamupaivalla = theParameters.theMorningWord;
  const std::string& iltapaivalla = theParameters.theAfternoonWord;

  Sentence theSpecifiedDay;
  short dayNumber = 0;
  if (theParameters.thePeriodLength > 24)
  {
    theSpecifiedDay << PeriodPhraseFactory::create("today",
                                                   theParameters.theVariable,
                                                   theParameters.theForecastTime,
                                                   theParameters.thePeriod,
                                                   theParameters.theArea);
    dayNumber = theParameters.thePeriod.localStartTime().GetWeekday();
  }

  float wind_cooling_the_weather_limit =
      Settings::optional_double(theParameters.theVariable + "::wind_cooling_the_weather_limit",
                                WIND_COOLING_THE_WEATHER_LIMIT);

  if (generate_windiness_sentence)
    sentence << construct_windiness_sentence(theParameters, theSpecifiedDay, dayNumber);

  if (!sentence.empty() || !generate_wind_cooling_sentence)
    return sentence;

  // handle the wind cooling effect
  bool windCoolingInlandMorning =
      (theParameters.theWindspeedInlandMorningMean.value() != kFloatMissing &&
       static_cast<int>(round(theParameters.theWindspeedInlandMorningMean.value())) >=
           wind_cooling_the_weather_limit);
  bool windCoolingCoastalMorning =
      (theParameters.theWindspeedCoastalMorningMean.value() != kFloatMissing &&
       static_cast<int>(round(theParameters.theWindspeedCoastalMorningMean.value())) >=
           wind_cooling_the_weather_limit);
  bool windCoolingInlandAfternoon =
      (theParameters.theWindspeedInlandAfternoonMean.value() != kFloatMissing &&
       static_cast<int>(round(theParameters.theWindspeedInlandAfternoonMean.value())) >=
           wind_cooling_the_weather_limit);
  bool windCoolingCoastalAfternoon =
      (theParameters.theWindspeedCoastalAfternoonMean.value() != kFloatMissing &&
       static_cast<int>(round(theParameters.theWindspeedCoastalAfternoonMean.value())) >=
           wind_cooling_the_weather_limit);

  bool inlandIncluded = windCoolingInlandMorning || windCoolingInlandAfternoon;
  bool coastIncluded = windCoolingCoastalMorning || windCoolingCoastalAfternoon;

  if (!inlandIncluded && !coastIncluded)
    return sentence;

  int tempInlandMorning =
      static_cast<int>(round(theParameters.theTemperatureInlandMorningMean.value()));
  int tempInlandAfternoon =
      static_cast<int>(round(theParameters.theTemperatureInlandAfternoonMean.value()));
  int tempCoastalMorning =
      static_cast<int>(round(theParameters.theTemperatureCoastalMorningMean.value()));
  int tempCoastalAfternoon =
      static_cast<int>(round(theParameters.theTemperatureCoastalAfternoonMean.value()));

  WindCoolingContext ctx;
  if (inlandIncluded && coastIncluded)
    ctx = determine_wind_cooling_both_areas(windCoolingInlandMorning,
                                            windCoolingCoastalMorning,
                                            windCoolingInlandAfternoon,
                                            windCoolingCoastalAfternoon,
                                            tempInlandMorning,
                                            tempInlandAfternoon,
                                            tempCoastalMorning,
                                            tempCoastalAfternoon,
                                            aamupaivalla,
                                            iltapaivalla);
  else if (coastIncluded)
    ctx = determine_wind_cooling_coast_only(windCoolingCoastalMorning,
                                            windCoolingCoastalAfternoon,
                                            tempCoastalMorning,
                                            tempCoastalAfternoon,
                                            aamupaivalla,
                                            iltapaivalla);
  else
    ctx = determine_wind_cooling_inland_only(windCoolingInlandMorning,
                                             windCoolingInlandAfternoon,
                                             tempInlandMorning,
                                             tempInlandAfternoon,
                                             aamupaivalla,
                                             iltapaivalla);

  std::string timePhrase(parse_weekday_phrase(dayNumber, ctx.part_of_the_day));
  emit_wind_cooling_sentence(
      sentence, ctx.temperature, ctx.areaString, timePhrase, ctx.part_of_the_day, dayNumber);

  return sentence;
}

std::string windchill_area_word(forecast_area_id area)
{
  if (area == INLAND_AREA)
    return SISAMAASSA_WORD;
  if (area == COASTAL_AREA)
    return RANNIKOLLA_WORD;
  return EMPTY_STRING;
}

struct windchill_context
{
  WeatherResult morningMean{kFloatMissing, 0};
  WeatherResult afternoonMean{kFloatMissing, 0};
  float tempDiffMorning = 0.0;
  float tempDiffAfternoon = 0.0;
  forecast_area_id areaMorning = FULL_AREA;
  forecast_area_id areaAfternoon = FULL_AREA;
};

windchill_context select_windchill_context_both(const wind_anomaly_params& p)
{
  windchill_context ctx;
  if (p.theWindchillInlandMorningMean.value() != kFloatMissing)
  {
    ctx.morningMean = p.theWindchillInlandMorningMean;
    ctx.areaMorning = INLAND_AREA;
    ctx.tempDiffMorning =
        abs(p.theTemperatureInlandMorningMean.value() - ctx.morningMean.value());
  }
  else
  {
    ctx.morningMean = p.theWindchillCoastalMorningMean;
    ctx.areaMorning = COASTAL_AREA;
    ctx.tempDiffMorning =
        abs(p.theTemperatureCoastalMorningMean.value() - ctx.morningMean.value());
  }

  if (p.theWindchillInlandAfternoonMean.value() > p.theWindchillCoastalAfternoonMean.value())
  {
    ctx.afternoonMean = p.theWindchillInlandAfternoonMean;
    ctx.areaAfternoon = INLAND_AREA;
    ctx.tempDiffAfternoon =
        abs(p.theTemperatureInlandAfternoonMean.value() - ctx.afternoonMean.value());
  }
  else
  {
    ctx.afternoonMean = p.theWindchillCoastalAfternoonMean;
    ctx.areaAfternoon = COASTAL_AREA;
    ctx.tempDiffAfternoon =
        abs(p.theTemperatureCoastalAfternoonMean.value() - ctx.afternoonMean.value());
  }
  return ctx;
}

windchill_context select_windchill_context(const wind_anomaly_params& p,
                                           bool inlandIncluded,
                                           bool coastIncluded)
{
  if (inlandIncluded && coastIncluded)
    return select_windchill_context_both(p);

  windchill_context ctx;
  if (inlandIncluded)
  {
    ctx.morningMean = p.theWindchillInlandMorningMean;
    ctx.afternoonMean = p.theWindchillInlandAfternoonMean;
    ctx.tempDiffMorning =
        abs(p.theTemperatureInlandMorningMean.value() - ctx.morningMean.value());
    ctx.tempDiffAfternoon =
        abs(p.theTemperatureInlandAfternoonMean.value() - ctx.afternoonMean.value());
  }
  else if (coastIncluded)
  {
    ctx.morningMean = p.theWindchillCoastalMorningMean;
    ctx.afternoonMean = p.theWindchillCoastalAfternoonMean;
    ctx.tempDiffMorning =
        abs(p.theTemperatureCoastalMorningMean.value() - ctx.morningMean.value());
    ctx.tempDiffAfternoon =
        abs(p.theTemperatureCoastalAfternoonMean.value() - ctx.afternoonMean.value());
  }
  return ctx;
}

void emit_windchill_partial_sentence(Sentence& sentence,
                                     bool extreme,
                                     forecast_area_id area,
                                     short dayNumber,
                                     const std::string& timeWord)
{
  const char* phraseWithArea = extreme
      ? ILTAPAIVALLA_RANNIKOLLA_PAKKANEN_ON_ERITTAIN_PUREVAA_COMPOSITE_PHRASE
      : ILTAPAIVALLA_RANNIKOLLA_PAKKANEN_ON_PUREVAA_COMPOSITE_PHRASE;
  const char* phraseNoArea = extreme
      ? ILTAPAIVALLA_PAKKANEN_ON_ERITTAIN_PUREVAA_COMPOSITE_PHRASE
      : ILTAPAIVALLA_PAKKANEN_ON_PUREVAA_COMPOSITE_PHRASE;

  std::string areaStr = windchill_area_word(area);
  if (areaStr.empty())
    sentence << phraseNoArea << parse_weekday_phrase(dayNumber, timeWord);
  else
    sentence << phraseWithArea << parse_weekday_phrase(dayNumber, timeWord) << areaStr;
}

void emit_windchill_mild_sentences(Sentence& sentence,
                                   bool wcMorning,
                                   bool wcAfternoon,
                                   const windchill_context& ctx,
                                   short dayNumber,
                                   const std::string& aamupaivalla,
                                   const std::string& iltapaivalla)
{
  if (wcMorning && wcAfternoon)
    sentence << PAKKANEN_ON_PUREVAA_PHRASE;
  else if (wcMorning)
    emit_windchill_partial_sentence(sentence, false, ctx.areaMorning, dayNumber, aamupaivalla);
  else if (wcAfternoon)
    emit_windchill_partial_sentence(sentence, false, ctx.areaAfternoon, dayNumber, iltapaivalla);
}

void emit_windchill_extreme_sentences(Sentence& sentence,
                                      bool wcMorning,
                                      bool wcAfternoon,
                                      bool extremeMorning,
                                      bool extremeAfternoon,
                                      const windchill_context& ctx,
                                      short dayNumber,
                                      const std::string& aamupaivalla,
                                      const std::string& iltapaivalla)
{
  if (wcMorning && wcAfternoon)
    sentence << PAKKANEN_ON_ERITTAIN_PUREVAA_PHRASE;
  if (extremeMorning && !extremeAfternoon)
    emit_windchill_partial_sentence(sentence, true, ctx.areaMorning, dayNumber, aamupaivalla);
  else if (!extremeMorning && extremeAfternoon)
    emit_windchill_partial_sentence(sentence, true, ctx.areaAfternoon, dayNumber, iltapaivalla);
}

Sentence windchill_sentence(const wind_anomaly_params& theParameters)
{
  Sentence sentence;

  bool inlandIncluded = theParameters.theWindchillInlandMorningMinimum.value() != kFloatMissing ||
                        theParameters.theWindchillInlandAfternoonMinimum.value() != kFloatMissing;
  bool coastIncluded = theParameters.theWindchillCoastalMorningMinimum.value() != kFloatMissing ||
                       theParameters.theWindchillCoastalAfternoonMinimum.value() != kFloatMissing;
  const std::string& aamupaivalla = theParameters.theMorningWord;
  const std::string& iltapaivalla = theParameters.theAfternoonWord;

  short dayNumber = 0;
  if (theParameters.thePeriodLength > 24)
    dayNumber = theParameters.thePeriod.localStartTime().GetWeekday();

  windchill_context ctx = select_windchill_context(theParameters, inlandIncluded, coastIncluded);

  bool morningIncluded = ctx.morningMean.value() != kFloatMissing;
  bool afternoonIncluded = ctx.afternoonMean.value() != kFloatMissing;

  float windChill = 0.0;
  if (morningIncluded && afternoonIncluded)
    windChill = std::min(ctx.morningMean.value(), ctx.afternoonMean.value());
  else if (morningIncluded)
    windChill = ctx.morningMean.value();
  else if (afternoonIncluded)
    windChill = ctx.afternoonMean.value();

  bool windChillMorning =
      morningIncluded && ctx.morningMean.value() >= EXTREME_WINDCHILL_LIMIT &&
      ctx.morningMean.value() <= MILD_WINDCHILL_LIMIT &&
      ctx.tempDiffMorning >= TEMPERATURE_AND_WINDCHILL_DIFFERENCE_LIMIT;
  bool windChillAfternoon =
      afternoonIncluded && ctx.afternoonMean.value() >= EXTREME_WINDCHILL_LIMIT &&
      ctx.afternoonMean.value() <= MILD_WINDCHILL_LIMIT &&
      ctx.tempDiffAfternoon >= TEMPERATURE_AND_WINDCHILL_DIFFERENCE_LIMIT;
  bool extremelyWindChillMorning =
      morningIncluded && ctx.morningMean.value() < EXTREME_WINDCHILL_LIMIT &&
      ctx.tempDiffMorning >= TEMPERATURE_AND_WINDCHILL_DIFFERENCE_LIMIT;
  bool extremelyWindChillAfternoon =
      afternoonIncluded && ctx.afternoonMean.value() < EXTREME_WINDCHILL_LIMIT &&
      ctx.tempDiffAfternoon >= TEMPERATURE_AND_WINDCHILL_DIFFERENCE_LIMIT;

  if (windChill >= EXTREME_WINDCHILL_LIMIT && windChill <= MILD_WINDCHILL_LIMIT)
    emit_windchill_mild_sentences(sentence, windChillMorning, windChillAfternoon,
                                  ctx, dayNumber, aamupaivalla, iltapaivalla);
  else if (windChill < EXTREME_WINDCHILL_LIMIT)
    emit_windchill_extreme_sentences(sentence, windChillMorning, windChillAfternoon,
                                     extremelyWindChillMorning, extremelyWindChillAfternoon,
                                     ctx, dayNumber, aamupaivalla, iltapaivalla);

  return sentence;
}

#if 0
void testWindiness(wind_anomaly_params& parameters, MessageLogger& log)
{
  Sentence theSpecifiedDay;
  short dayNumber = 0;
  if (parameters.thePeriodLength > 24)
  {
    theSpecifiedDay << PeriodPhraseFactory::create("today",
                                                   parameters.theVariable,
                                                   parameters.theForecastTime,
                                                   parameters.thePeriod,
                                                   parameters.theArea);
    dayNumber = parameters.thePeriod.localStartTime().GetWeekday();
  }

  parameters.theWindspeedInlandMorningMinimum = WeatherResult(1.0, 0.0);
  parameters.theWindspeedCoastalMorningMinimum = WeatherResult(1.0, 0.0);
  for (float i = 4.0; i < 13.0; i = i + 0.5)
  {
    parameters.theWindspeedInlandMorningMean = WeatherResult(i, 0.0);
    {
      for (float j = 4.0; j < 13.0; j = j + 0.5)
      {
        parameters.theWindspeedInlandAfternoonMean = WeatherResult(j, 0.0);
        for (float k = 4.0; k < 13.0; k = k + 0.5)
        {
          parameters.theWindspeedCoastalMorningMean = WeatherResult(k, 0.0);
          for (float l = 4.0; l < 13.0; l = l + 0.5)
          {
            parameters.theWindspeedCoastalAfternoonMean = WeatherResult(l, 0.0);
            Sentence sentence;
            sentence << construct_windiness_sentence(parameters, theSpecifiedDay, dayNumber);
            log << "Windspeed InlandM, InlandA, CoastalM, CoastalA: (";
            log << parameters.theWindspeedInlandMorningMean << ",";
            log << parameters.theWindspeedInlandAfternoonMean << ",";
            log << parameters.theWindspeedCoastalMorningMean << ",";
            log << parameters.theWindspeedCoastalAfternoonMean << "): \n";
            log << sentence;
          }
        }
      }
    }
  }
}
#endif

}  // namespace

}  // namespace WindAnomaly

// ----------------------------------------------------------------------
/*!
 * \brief Generate story on wind anomaly
 *
 * \return The generated paragraph
 */
// ----------------------------------------------------------------------

Paragraph WindStory::anomaly() const
{
  using namespace WindAnomaly;

  Paragraph paragraph;
  MessageLogger log("WindStory::anomaly");

  if (itsArea.isNamed())
  {
    std::string nimi(itsArea.name());
    /*
      if(nimi.compare("uusimaa") == 0)
      {
      log << "TEST WINDINESS: ";
      testWindiness(parameters, log);
      }
    */

    log << nimi << '\n';
  }

  forecast_season_id theSeasonId =
      isSummerHalf(itsPeriod.localStartTime(), itsVar) ? SUMMER_SEASON : WINTER_SEASON;

  // the anomaly sentence relates always to the upcoming day,
  // so the period is defined to start in the previous day
  log_start_time_and_end_time(log, "the original period: ", itsPeriod);

  TextGenPosixTime periodStartTime(itsPeriod.localStartTime());
  TextGenPosixTime periodEndTime(itsPeriod.localEndTime());
  int periodLength = periodEndTime.DifferenceInHours(periodStartTime);

  // Period generator
  const HourPeriodGenerator periodgenerator(itsPeriod, itsVar + "::day");
  const int ndays = periodgenerator.size();

  log << "Period " << itsPeriod.localStartTime() << "..." << itsPeriod.localEndTime() << " covers "
      << ndays << " days\n";

  if (ndays <= 0)
  {
    log << paragraph;
    return paragraph;
  }

  // the last day period
  WeatherPeriod windAnomalyPeriod(periodgenerator.period(ndays));

  log_start_time_and_end_time(log, "wind anomaly period: ", windAnomalyPeriod);

  bool specify_part_of_the_day =
      Settings::optional_bool(itsVar + "::specify_part_of_the_day", true);
  wind_anomaly_params parameters(itsVar,
                                 log,
                                 itsSources,
                                 itsArea,
                                 windAnomalyPeriod,
                                 theSeasonId,
                                 itsForecastTime,
                                 periodLength,
                                 specify_part_of_the_day);

  float coastalPercentage = get_area_percentage(
      itsVar + "::fake::area_percentage", itsArea, WeatherArea::Coast, itsSources, itsPeriod);

  float separate_coastal_area_percentage = Settings::optional_double(
      itsVar + "::separate_coastal_area_percentage", SEPARATE_COASTAL_AREA_PERCENTAGE);

  parameters.theCoastalAndInlandTogetherFlag =
      coastalPercentage > 0 && coastalPercentage < separate_coastal_area_percentage;

  if (parameters.theCoastalAndInlandTogetherFlag)
    log << "Inland and coastal area(" << coastalPercentage << ") not separated!\n";

  WeatherArea inlandArea = itsArea;
  inlandArea.type(!parameters.theCoastalAndInlandTogetherFlag ? WeatherArea::Inland
                                                              : WeatherArea::Full);
  WeatherArea coastalArea = itsArea;
  coastalArea.type(!parameters.theCoastalAndInlandTogetherFlag ? WeatherArea::Coast
                                                               : WeatherArea::Full);

  bool morningIncluded = windAnomalyPeriod.localStartTime().GetHour() < 10;
  bool afternoonIncluded = windAnomalyPeriod.localEndTime().GetHour() > 14;
  parameters.theMorningWord = (specify_part_of_the_day ? AAMUPAIVALLA_WORD : EMPTY_STRING);
  parameters.theAfternoonWord = (specify_part_of_the_day ? ILTAPAIVALLA_WORD : EMPTY_STRING);

  if (morningIncluded)
  {
    morning_temperature(itsVar + "::fake::temperature::morning::area",
                        itsSources,
                        itsArea,
                        windAnomalyPeriod,
                        parameters.theTemperatureAreaMorningMinimum,
                        parameters.theTemperatureAreaMorningMaximum,
                        parameters.theTemperatureAreaMorningMean);

    morning_temperature(itsVar + "::fake::temperature::morning::inland",
                        itsSources,
                        inlandArea,
                        windAnomalyPeriod,
                        parameters.theTemperatureInlandMorningMinimum,
                        parameters.theTemperatureInlandMorningMaximum,
                        parameters.theTemperatureInlandMorningMean);

    morning_temperature(itsVar + "::fake::temperature::morning::coast",
                        itsSources,
                        coastalArea,
                        windAnomalyPeriod,
                        parameters.theTemperatureCoastalMorningMinimum,
                        parameters.theTemperatureCoastalMorningMaximum,
                        parameters.theTemperatureCoastalMorningMean);

    // inland, morning, windspeed
    calculate_windspeed_and_chill(parameters, WeatherArea::Inland, true, true);
    // coastal morning, windspeed
    calculate_windspeed_and_chill(parameters, WeatherArea::Coast, true, true);

    // inland, morning, windchill
    calculate_windspeed_and_chill(parameters, WeatherArea::Inland, true, false);
    // coastal morning, windchill
    calculate_windspeed_and_chill(parameters, WeatherArea::Coast, true, false);
  }

  if (afternoonIncluded)
  {
    afternoon_temperature(itsVar + "::fake::temperature::afternoon::area",
                          itsSources,
                          itsArea,
                          windAnomalyPeriod,
                          parameters.theTemperatureAreaAfternoonMinimum,
                          parameters.theTemperatureAreaAfternoonMaximum,
                          parameters.theTemperatureAreaAfternoonMean);

    afternoon_temperature(itsVar + "::fake::temperature::afternoon::inland",
                          itsSources,
                          inlandArea,
                          windAnomalyPeriod,
                          parameters.theTemperatureInlandAfternoonMinimum,
                          parameters.theTemperatureInlandAfternoonMaximum,
                          parameters.theTemperatureInlandAfternoonMean);

    afternoon_temperature(itsVar + "::fake::temperature::afternoon::coast",
                          itsSources,
                          coastalArea,
                          windAnomalyPeriod,
                          parameters.theTemperatureCoastalAfternoonMinimum,
                          parameters.theTemperatureCoastalAfternoonMaximum,
                          parameters.theTemperatureCoastalAfternoonMean);

    // inland afternoon, windspeed
    calculate_windspeed_and_chill(parameters, WeatherArea::Inland, false, true);
    // coastal afternoon, windspeed
    calculate_windspeed_and_chill(parameters, WeatherArea::Coast, false, true);

    // inland afternoon, windchill
    calculate_windspeed_and_chill(parameters, WeatherArea::Inland, false, false);
    // coastal afternoon, windchill
    calculate_windspeed_and_chill(parameters, WeatherArea::Coast, false, false);
  }

  Paragraph paragraphDev;
  Sentence windinessSentence;
  Sentence windChillSentence;

  windinessSentence << windiness_sentence(parameters);

  windChillSentence << windchill_sentence(parameters);

  log_data(parameters);

  log << "windiness: ";
  log << windinessSentence;
  log << "wind chill: ";
  log << windChillSentence;

  paragraph << windinessSentence;
  paragraph << windChillSentence;

  log << paragraph;

  return paragraph;
}

}  // namespace TextGen

// ======================================================================
