// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::TemperatureStory::anomaly
 */
// ======================================================================

#include "TemperatureStory.h"

#include "ClimatologyTools.h"
#include "GridClimatology.h"

#include "AreaTools.h"
#include "Delimiter.h"
#include "Integer.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "SeasonTools.h"
#include "Sentence.h"
#include "TemperatureStoryTools.h"
#include "UnitFactory.h"
#include <calculator/DefaultAcceptor.h>
#include <calculator/GridForecaster.h>
#include <calculator/HourPeriodGenerator.h>
#include <calculator/MathTools.h>
#include <calculator/RangeAcceptor.h>
#include <calculator/Settings.h>
#include <calculator/WeatherPeriodTools.h>
#include <calculator/WeatherResult.h>
#include <macgyver/Exception.h>

#include <newbase/NFmiGrid.h>
#include <newbase/NFmiIndexMask.h>
#include <newbase/NFmiIndexMaskSource.h>
#include <newbase/NFmiIndexMaskTools.h>
#include <newbase/NFmiMetMath.h>
#include <newbase/NFmiQueryData.h>
#include <newbase/NFmiStringTools.h>

#include "WeatherForecast.h"
#include <calculator/WeatherArea.h>
#include <calculator/WeatherSource.h>
#include <map>

#include <boost/lexical_cast.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <filesystem>  // includes all needed Boost.Filesystem declarations
namespace fs = std::filesystem;

using namespace std;

namespace TextGen
{
namespace TemperatureAnomaly
{
using namespace TextGen;
using namespace TemperatureStoryTools;

using namespace Settings;
using namespace SeasonTools;
using namespace AreaTools;

#define MILD_TEMPERATURE_LOWER_LIMIT (-3.0)
#define MILD_TEMPERATURE_UPPER_LIMIT (+5.0)
#define HOT_WEATHER_LIMIT (+25.0)
#define GETTING_COOLER_NOTIFICATION_LIMIT (+20.0)
#define ZERO_DEGREES 0.0
#define VERY_COLD_TEMPERATURE_UPPER_LIMIT (-10.0)
#define SMALL_CHANGE_LOWER_LIMIT 2.0
#define SMALL_CHANGE_UPPER_LIMIT 3.0
#define MODERATE_CHANGE_LOWER_LIMIT 3.0
#define MODERATE_CHANGE_UPPER_LIMIT 5.0
#define SIGNIFIGANT_CHANGE_LOWER_LIMIT 5.0
#define NOTABLE_TEMPERATURE_CHANGE_LIMIT 2.5

#define SAA_LAUHTUU_PHRASE "saa lauhtuu"
#define SAA_WORD "saa"
#define ON_WORD "on"
#define POIKKEUKSELLISEN_WORD "poikkeuksellisen"
#define KYLMAA_WORD "kylmaa"
#define KOLEAA_WORD "koleaa"
#define HYVIN_WORD "hyvin"
#define VAHAN_WORD "vahan"
#define HUOMATTAVASTI_WORD "huomattavasti"
#define HARVINAISEN_WORD "harvinaisen"
#define LAMMINTA_WORD "lamminta"
#define LEUTOA_WORD "leutoa"

#define SAA_ON_EDELLEEN_LAUHAA_PHRASE "saa on edelleen lauhaa"
#define SAA_LAUHTUU_PHRASE "saa lauhtuu"
#define PAKKANEN_HEIKKENEE_PHRASE "pakkanen heikkenee"
#define KIREA_PAKKANEN_HEIKKENEE_PHRASE "kirea pakkanen heikkenee"
#define PAKKANEN_HELLITTAA_PHRASE "pakkanen hellittaa"
#define KIREA_PAKKANEN_JATKUU_PHRASE "kirea pakkanen jatkuu"
#define PAKKANEN_KIRISTYY_PHRASE "pakkanen kiristyy"
#define HELTEINEN_SAA_JATKUU_PHRASE "helteinen saa jatkuu"
#define VIILEA_SAA_JATKUU_PHRASE "viilea saa jatkuu"
#define KOLEA_SAA_JATKUU_PHRASE "kolea saa jatkuu"
#define SAA_MUUTTUU_HELTEISEKSI_PHRASE "saa muuttuu helteiseksi"
#define SAA_ON_HELTEISTA_PHRASE "saa on helteista"
#define SAA_LAMPENEE_PHRASE "saa lampenee"
#define SAA_LAMPENEE_VAHAN_PHRASE "saa lampenee vahan"
#define SAA_LAMPENEE_HUOMATTAVASTI_PHRASE "saa lampenee huomattavasti"
#define SAA_VIILENEE_PHRASE "saa viilenee"
#define SAA_VIILENEE_VAHAN_PHRASE "saa viilenee vahan"
#define SAA_VIILENEE_HUOMATTAVASTI_PHRASE "saa viilenee huomattavasti"

#define SAA_ON_AJANKOHTAAN_NAHDEN_TAVANOMAISTA_LAMPIMAMPAA_PHRASE \
  "saa on ajankohtaan nahden tavanomaista lampimampaa"
#define ALUEELLA_SAA_ON_AJANKOHTAAN_NAHDEN_TAVANOMAISTA_LAMPIMAMPAA_PHRASE \
  "[alueella] saa on ajankohtaan nahden tavanomaista lampimampaa"
#define MAANANTAINA_SAA_ON_AJANKOHTAAN_NAHDEN_TAVANOMAISTA_LAMPIMAMPAA_PHRASE \
  "[1-na] saa on ajankohtaan nahden tavanomaista lampimampaa"
#define MAANANTAINA_ALUEELLA_SAA_ON_AJANKOHTAAN_NAHDEN_TAVANOMAISTA_LAMPIMAMPAA_PHRASE \
  "[1-na] [alueella] saa on ajankohtaan nahden tavanomaista lampimampaa"

#define SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE "saa on [poikkeuksellisen] [kylmaa]"
#define ALUEELLA_SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE \
  "[alueella] saa on [poikkeuksellisen] [kylmaa]"
#define MAANANTAINA_SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE \
  "[1-na] saa on [poikkeuksellisen] [kylmaa]"
#define MAANANTAINA_ALUEELLA_SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE \
  "[1-na] [alueella] saa on [poikkeuksellisen] [kylmaa]"

#define MAANANTAINA_SAA_ON_EDELLEEN_LAUHAA_COMPOSITE_PHRASE "[1-na] saa on edelleen lauhaa"
#define MAANANTAINA_SAA_LAUHTUU_COMPOSITE_PHRASE "[1-na] saa lauhtuu"
#define MAANANTAINA_PAKKANEN_HEIKKENEE_COMPOSITE_PHRASE "[1-na] pakkanen heikkenee"
#define MAANANTAINA_KIREA_PAKKANEN_HEIKKENEE_COMPOSITE_PHRASE "[1-na] kirea pakkanen heikkenee"
#define MAANANTAINA_PAKKANEN_HELLITTAA_COMPOSITE_PHRASE "[1-na] pakkanen hellittaa"
#define MAANANTAINA_KIREA_PAKKANEN_JATKUU_COMPOSITE_PHRASE "[1-na] kirea pakkanen jatkuu"
#define MAANANTAINA_PAKKANEN_KIRISTYY_COMPOSITE_PHRASE "[1-na] pakkanen kiristyy"
#define MAANANTAINA_HELTEINEN_SAA_JATKUU_COMPOSITE_PHRASE "[1-na] helteinen saa jatkuu"
#define MAANANTAINA_VIILEA_SAA_JATKUU_COMPOSITE_PHRASE "[1-na] viilea saa jatkuu"
#define MAANANTAINA_KOLEA_SAA_JATKUU_COMPOSITE_PHRASE "[1-na] kolea saa jatkuu"
#define MAANANTAINA_SAA_MUUTTUU_HELTEISEKSI_COMPOSITE_PHRASE "[1-na] saa muuttuu helteiseksi"
#define MAANANTAINA_SAA_ON_HELTEISTA_COMPOSITE_PHRASE "[1-na] saa on helteista"
#define MAANANTAINA_SAA_LAMPENEE_COMPOSITE_PHRASE "[1-na] saa lampenee"
#define MAANANTAINA_SAA_LAMPENEE_VAHAN_COMPOSITE_PHRASE "[1-na] saa lampenee vahan"
#define MAANANTAINA_SAA_LAMPENEE_HUOMATTAVASTI_COMPOSITE_PHRASE "[1-na] saa lampenee huomattavasti"
#define MAANANTAINA_SAA_VIILENEE_COMPOSITE_PHRASE "[1-na] saa viilenee"
#define MAANANTAINA_SAA_VIILENEE_VAHAN_COMPOSITE_PHRASE "[1-na] saa viilenee vahan"
#define MAANANTAINA_SAA_VIILENEE_HUOMATTAVASTI_COMPOSITE_PHRASE "[1-na] saa viilenee huomattavasti"

#define ALUEELLA_SAA_ON_EDELLEEN_LAUHAA_COMPOSITE_PHRASE "[alueella] saa on edelleen lauhaa"
#define ALUEELLA_SAA_LAUHTUU_COMPOSITE_PHRASE "[alueella] saa lauhtuu"
#define ALUEELLA_PAKKANEN_HEIKKENEE_COMPOSITE_PHRASE "[alueella] pakkanen heikkenee"
#define ALUEELLA_KIREA_PAKKANEN_HEIKKENEE_COMPOSITE_PHRASE "[alueella] kirea pakkanen heikkenee"
#define ALUEELLA_PAKKANEN_HELLITTAA_COMPOSITE_PHRASE "[alueella] pakkanen hellittaa"
#define ALUEELLA_KIREA_PAKKANEN_JATKUU_COMPOSITE_PHRASE "[alueella] kirea pakkanen jatkuu"
#define ALUEELLA_PAKKANEN_KIRISTYY_COMPOSITE_PHRASE "[alueella] pakkanen kiristyy"
#define ALUEELLA_HELTEINEN_SAA_JATKUU_COMPOSITE_PHRASE "[alueella] helteinen saa jatkuu"
#define ALUEELLA_VIILEA_SAA_JATKUU_COMPOSITE_PHRASE "[alueella] viilea saa jatkuu"
#define ALUEELLA_KOLEA_SAA_JATKUU_COMPOSITE_PHRASE "[alueella] kolea saa jatkuu"
#define ALUEELLA_SAA_MUUTTUU_HELTEISEKSI_COMPOSITE_PHRASE "[alueella] saa muuttuu helteiseksi"
#define ALUEELLA_SAA_ON_HELTEISTA_COMPOSITE_PHRASE "[alueella] saa on helteista"
#define ALUEELLA_SAA_LAMPENEE_COMPOSITE_PHRASE "[alueella] saa lampenee"
#define ALUEELLA_SAA_LAMPENEE_VAHAN_COMPOSITE_PHRASE "[alueella] saa lampenee vahan"
#define ALUEELLA_SAA_LAMPENEE_HUOMATTAVASTI_COMPOSITE_PHRASE "[alueella] saa lampenee huomattavasti"
#define ALUEELLA_SAA_VIILENEE_COMPOSITE_PHRASE "[alueella] saa viilenee"
#define ALUEELLA_SAA_VIILENEE_VAHAN_COMPOSITE_PHRASE "[alueella] saa viilenee vahan"
#define ALUEELLA_SAA_VIILENEE_HUOMATTAVASTI_COMPOSITE_PHRASE "[alueella] saa viilenee huomattavasti"

#define MAANANTAINA_ALUEELLA_SAA_ON_EDELLEEN_LAUHAA_COMPOSITE_PHRASE \
  "[1-na] [alueella] saa on edelleen lauhaa"
#define MAANANTAINA_ALUEELLA_SAA_LAUHTUU_COMPOSITE_PHRASE "[1-na] [alueella] saa lauhtuu"
#define MAANANTAINA_ALUEELLA_PAKKANEN_HEIKKENEE_COMPOSITE_PHRASE \
  "[1-na] [alueella] pakkanen heikkenee"
#define MAANANTAINA_ALUEELLA_KIREA_PAKKANEN_HEIKKENEE_COMPOSITE_PHRASE \
  "[1-na] [alueella] kirea pakkanen heikkenee"
#define MAANANTAINA_ALUEELLA_PAKKANEN_HELLITTAA_COMPOSITE_PHRASE \
  "[1-na] [alueella] pakkanen hellittaa"
#define MAANANTAINA_ALUEELLA_KIREA_PAKKANEN_JATKUU_COMPOSITE_PHRASE \
  "[1-na] [alueella] kirea pakkanen jatkuu"
#define MAANANTAINA_ALUEELLA_PAKKANEN_KIRISTYY_COMPOSITE_PHRASE \
  "[1-na] [alueella] pakkanen kiristyy"
#define MAANANTAINA_ALUEELLA_HELTEINEN_SAA_JATKUU_COMPOSITE_PHRASE \
  "[1-na] [alueella] helteinen saa jatkuu"
#define MAANANTAINA_ALUEELLA_VIILEA_SAA_JATKUU_COMPOSITE_PHRASE \
  "[1-na] [alueella] viilea saa jatkuu"
#define MAANANTAINA_ALUEELLA_KOLEA_SAA_JATKUU_COMPOSITE_PHRASE "[1-na] [alueella] kolea saa jatkuu"
#define MAANANTAINA_ALUEELLA_SAA_MUUTTUU_HELTEISEKSI_COMPOSITE_PHRASE \
  "[1-na] [alueella] saa muuttuu helteiseksi"
#define MAANANTAINA_ALUEELLA_SAA_ON_HELTEISTA_COMPOSITE_PHRASE "[1-na] [alueella] saa on helteista"
#define MAANANTAINA_ALUEELLA_SAA_LAMPENEE_COMPOSITE_PHRASE "[1-na] [alueella] saa lampenee"
#define MAANANTAINA_ALUEELLA_SAA_LAMPENEE_VAHAN_COMPOSITE_PHRASE \
  "[1-na] [alueella] saa lampenee vahan"
#define MAANANTAINA_ALUEELLA_SAA_LAMPENEE_HUOMATTAVASTI_COMPOSITE_PHRASE \
  "[1-na] [alueella] saa lampenee huomattavasti"
#define MAANANTAINA_ALUEELLA_SAA_VIILENEE_COMPOSITE_PHRASE "[1-na] [alueella] saa viilenee"
#define MAANANTAINA_ALUEELLA_SAA_VIILENEE_VAHAN_COMPOSITE_PHRASE \
  "[1-na] [alueella] saa viilenee vahan"
#define MAANANTAINA_ALUEELLA_SAA_VIILENEE_HUOMATTAVASTI_COMPOSITE_PHRASE \
  "[1-na] [alueella] saa viilenee huomattavasti"

namespace
{
enum anomaly_phrase_id
{
  SAA_ON_POIKKEUKSELLISEN_KOLEAA,
  SAA_ON_POIKKEUKSELLISEN_KYLMAA,
  SAA_ON_KOLEAA,
  SAA_ON_HYVIN_KYLMAA,
  SAA_ON_HARVINAISEN_LAMMINTA,
  SAA_ON_HYVIN_LEUTOA,
  SAA_ON_POIKKEUKSLLISEN_LAMMINTA,
  SAA_ON_POIKKEUKSLLISEN_LEUTOA,
  UNDEFINED_ANOMALY_PHRASE_ID
};

enum shortrun_trend_id
{
  SAA_ON_EDELLEEN_LAUHAA,
  SAA_LAUHTUU,
  KIREA_PAKKANEN_HEIKKENEE,
  KIREA_PAKKANEN_HELLITTAA,
  PAKKANEN_HEIKKENEE,
  PAKKANEN_HELLITTAA,
  KIREA_PAKKANEN_JATKUU,
  PAKKANEN_KIRISTYY,
  HELTEINEN_SAA_JATKUU,
  KOLEA_SAA_JATKUU,
  VIILEA_SAA_JATKUU,
  SAA_MUUTTUU_HELTEISEKSI,
  SAA_ON_HELTEISTA,
  SAA_LAMPENEE_HUOMATTAVASTI,
  SAA_LAMPENEE,
  SAA_LAMPENEE_VAHAN,
  SAA_VIILENEE_HUOMATTAVASTI,
  SAA_VIILENEE,
  SAA_VIILENEE_VAHAN,
  UNDEFINED_SHORTRUN_TREND_ID
};

struct temperature_anomaly_params
{
  temperature_anomaly_params(const string& variable,
                             MessageLogger& log,
                             const AnalysisSources& sources,
                             const WeatherArea& area,
                             const WeatherPeriod& period,
                             const WeatherPeriod& dayBeforeDay1Period,
                             const WeatherPeriod& day1Period,
                             const WeatherPeriod& day2Period,
                             const WeatherPeriod& dayAfterDay2Period,
                             forecast_season_id season,
                             const TextGenPosixTime& forecastTime,
                             short periodLength)
      : theVariable(variable),
        theLog(log),
        theSources(sources),
        theArea(area),
        thePeriod(period),
        theDayBeforeDay1Period(dayBeforeDay1Period),
        theDay1Period(day1Period),
        theDay2Period(day2Period),
        theDayAfterDay2Period(dayAfterDay2Period),
        theSeason(season),
        theForecastTime(forecastTime),
        thePeriodLength(periodLength)
  {
  }

  const string& theVariable;
  MessageLogger& theLog;
  const AnalysisSources& theSources;
  const WeatherArea& theArea;
  const WeatherPeriod& thePeriod;
  const WeatherPeriod& theDayBeforeDay1Period;
  const WeatherPeriod& theDay1Period;
  const WeatherPeriod& theDay2Period;
  const WeatherPeriod& theDayAfterDay2Period;
  forecast_season_id theSeason;
  const TextGenPosixTime& theForecastTime;
  short thePeriodLength = 0;
  anomaly_phrase_id theAnomalyPhrase = UNDEFINED_ANOMALY_PHRASE_ID;
  shortrun_trend_id theShortrunTrend = UNDEFINED_SHORTRUN_TREND_ID;
  string theFakeVariable;
  bool theGrowingSeasonUnderway = false;
  WeatherResult theDayBeforeDay1TemperatureAreaAfternoonMinimum{kFloatMissing, 0};
  WeatherResult theDayBeforeDay1TemperatureAreaAfternoonMean{kFloatMissing, 0};
  WeatherResult theDayBeforeDay1TemperatureAreaAfternoonMaximum{kFloatMissing, 0};
  WeatherResult theDay1TemperatureAreaAfternoonMinimum{kFloatMissing, 0};
  WeatherResult theDay1TemperatureAreaAfternoonMean{kFloatMissing, 0};
  WeatherResult theDay1TemperatureAreaAfternoonMaximum{kFloatMissing, 0};
  WeatherResult theDay2TemperatureAreaAfternoonMinimum{kFloatMissing, 0};
  WeatherResult theDay2TemperatureAreaAfternoonMean{kFloatMissing, 0};
  WeatherResult theDay2TemperatureAreaAfternoonMaximum{kFloatMissing, 0};
  WeatherResult theDayAfterDay2TemperatureAreaAfternoonMinimum{kFloatMissing, 0};
  WeatherResult theDayAfterDay2TemperatureAreaAfternoonMean{kFloatMissing, 0};
  WeatherResult theDayAfterDay2TemperatureAreaAfternoonMaximum{kFloatMissing, 0};
};

void log_data(const temperature_anomaly_params& theParameters)
{
  theParameters.theLog << "theDayBeforeDay1TemperatureAreaAfternoonMinimum: "
                       << theParameters.theDayBeforeDay1TemperatureAreaAfternoonMinimum << '\n';
  theParameters.theLog << "theDayBeforeDay1TemperatureAreaAfternoonMean: "
                       << theParameters.theDayBeforeDay1TemperatureAreaAfternoonMean << '\n';
  theParameters.theLog << "theDayBeforeDay1TemperatureAreaAfternoonMaximum: "
                       << theParameters.theDayBeforeDay1TemperatureAreaAfternoonMaximum << '\n';
  theParameters.theLog << "theDay1TemperatureAreaAfternoonMinimum: "
                       << theParameters.theDay1TemperatureAreaAfternoonMinimum << '\n';
  theParameters.theLog << "theDay1TemperatureAreaAfternoonMean: "
                       << theParameters.theDay1TemperatureAreaAfternoonMean << '\n';
  theParameters.theLog << "theDay1TemperatureAreaAfternoonMaximum: "
                       << theParameters.theDay1TemperatureAreaAfternoonMaximum << '\n';

  theParameters.theLog << "theDay2TemperatureAreaAfternoonMinimum: "
                       << theParameters.theDay2TemperatureAreaAfternoonMinimum << '\n';
  theParameters.theLog << "theDay2TemperatureAreaAfternoonMean: "
                       << theParameters.theDay2TemperatureAreaAfternoonMean << '\n';
  theParameters.theLog << "theDay2TemperatureAreaAfternoonMaximum: "
                       << theParameters.theDay2TemperatureAreaAfternoonMaximum << '\n';
  theParameters.theLog << "theDayAfterDay2TemperatureAreaAfternoonMinimum: "
                       << theParameters.theDayAfterDay2TemperatureAreaAfternoonMinimum << '\n';
  theParameters.theLog << "theDayAfterDay2TemperatureAreaAfternoonMean: "
                       << theParameters.theDayAfterDay2TemperatureAreaAfternoonMean << '\n';
  theParameters.theLog << "theDayAfterDay2TemperatureAreaAfternoonMaximum: "
                       << theParameters.theDayAfterDay2TemperatureAreaAfternoonMaximum << '\n';
}

void log_start_time_and_end_time(MessageLogger& theLog,
                                 const std::string& theLogMessage,
                                 const WeatherPeriod& thePeriod)
{
  theLog << NFmiStringTools::Convert(theLogMessage) << thePeriod.localStartTime() << " ... "
         << thePeriod.localEndTime() << '\n';
}

std::string period2string(const WeatherPeriod& period)
{
  std::stringstream ss;

  ss << period.localStartTime() << "..." << period.localEndTime();

  return ss.str();
}

#if 0
void log_daily_factiles_for_period(MessageLogger& theLog,
                                   const string& theVariable,
                                   const AnalysisSources& theSources,
                                   const WeatherArea& theArea,
                                   const std::string& theLogMessage,
                                   const WeatherPeriod& thePeriod,
                                   fractile_type_id theFractileType)
{
  TextGenPosixTime startTime(thePeriod.localStartTime());
  TextGenPosixTime endTime(thePeriod.localStartTime());

  startTime.SetHour(6);
  startTime.SetMin(0);
  startTime.SetSec(0);
  endTime.SetHour(18);
  endTime.SetMin(0);
  endTime.SetSec(0);

  theLog << theLogMessage;

  while (startTime.GetJulianDay() < thePeriod.localEndTime().GetJulianDay())
  {
    WeatherPeriod fractilePeriod(startTime, endTime);

    WeatherResult fractile02Temperature = get_fractile_temperature(
        theVariable, FRACTILE_02, theSources, theArea, fractilePeriod, theFractileType);
    WeatherResult fractile12Temperature = get_fractile_temperature(
        theVariable, FRACTILE_12, theSources, theArea, fractilePeriod, theFractileType);
    WeatherResult fractile37Temperature = get_fractile_temperature(
        theVariable, FRACTILE_37, theSources, theArea, fractilePeriod, theFractileType);
    WeatherResult fractile50Temperature = get_fractile_temperature(
        theVariable, FRACTILE_50, theSources, theArea, fractilePeriod, theFractileType);
    WeatherResult fractile63Temperature = get_fractile_temperature(
        theVariable, FRACTILE_63, theSources, theArea, fractilePeriod, theFractileType);
    WeatherResult fractile88Temperature = get_fractile_temperature(
        theVariable, FRACTILE_88, theSources, theArea, fractilePeriod, theFractileType);
    WeatherResult fractile98Temperature = get_fractile_temperature(
        theVariable, FRACTILE_98, theSources, theArea, fractilePeriod, theFractileType);

    theLog << "date = " << startTime << "..." << endTime << '\n';
    theLog << "F02 = " << fractile02Temperature << '\n';
    theLog << "F12 = " << fractile12Temperature << '\n';
    theLog << "F37 = " << fractile37Temperature << '\n';
    theLog << "F50 = " << fractile50Temperature << '\n';
    theLog << "F63 = " << fractile63Temperature << '\n';
    theLog << "F88 = " << fractile88Temperature << '\n';
    theLog << "F98 = " << fractile98Temperature << '\n';

    startTime.ChangeByDays(1);
    endTime.ChangeByDays(1);
  }
}
#endif

// Build anomaly sentence with composite phrase + optional trailing words, dispatching on day/area
Sentence build_anomaly_sentence(const Sentence& theSpecifiedDay,
                                const Sentence& theAreaPhrase,
                                const std::string& noDay_noArea_phrase,
                                const std::string& noDay_area_phrase,
                                const std::string& day_noArea_phrase,
                                const std::string& day_area_phrase)
{
  Sentence sentence;
  const bool hasDay = !theSpecifiedDay.empty();
  const bool hasArea = !theAreaPhrase.empty();

  if (!hasDay && !hasArea)
    sentence << noDay_noArea_phrase;
  else if (!hasDay && hasArea)
    sentence << noDay_area_phrase << theAreaPhrase;
  else if (hasDay && !hasArea)
    sentence << day_noArea_phrase << theSpecifiedDay;
  else
    sentence << day_area_phrase << theSpecifiedDay << theAreaPhrase;
  return sentence;
}

// Build anomaly sentence for cold/hot fractile cases (with extra trailing words)
Sentence build_anomaly_sentence_with_suffix(const Sentence& theSpecifiedDay,
                                            const Sentence& theAreaPhrase,
                                            const std::string& noDay_noArea_phrase,
                                            const std::string& noDay_area_phrase,
                                            const std::string& day_noArea_phrase,
                                            const std::string& day_area_phrase,
                                            const std::string& modifierWord,
                                            const std::string& tempWord)
{
  Sentence sentence =
      build_anomaly_sentence(theSpecifiedDay, theAreaPhrase,
                             noDay_noArea_phrase, noDay_area_phrase,
                             day_noArea_phrase, day_area_phrase);
  sentence << modifierWord << tempWord;
  return sentence;
}

// Build area phrase based on area type
Sentence build_area_phrase(const WeatherArea& theArea)
{
  Sentence theAreaPhrase;
  if (theArea.type() == WeatherArea::Northern)
    theAreaPhrase << ALUEEN_POHJOISOSASSA_PHRASE;
  else if (theArea.type() == WeatherArea::Southern)
    theAreaPhrase << ALUEEN_ETELAOSASSA_PHRASE;
  else if (theArea.type() == WeatherArea::Eastern)
    theAreaPhrase << ALUEEN_ITAOSASSA_PHRASE;
  else if (theArea.type() == WeatherArea::Western)
    theAreaPhrase << ALUEEN_LANSIOSASSA_PHRASE;
  return theAreaPhrase;
}

// Handle fractile88 warm season case (summer/growing: lampimampaa phrase)
Sentence build_fractile88_warm(const Sentence& theSpecifiedDay, const Sentence& theAreaPhrase)
{
  return build_anomaly_sentence(theSpecifiedDay,
                                theAreaPhrase,
                                SAA_ON_AJANKOHTAAN_NAHDEN_TAVANOMAISTA_LAMPIMAMPAA_PHRASE,
                                ALUEELLA_SAA_ON_AJANKOHTAAN_NAHDEN_TAVANOMAISTA_LAMPIMAMPAA_PHRASE,
                                MAANANTAINA_SAA_ON_AJANKOHTAAN_NAHDEN_TAVANOMAISTA_LAMPIMAMPAA_PHRASE,
                                MAANANTAINA_ALUEELLA_SAA_ON_AJANKOHTAAN_NAHDEN_TAVANOMAISTA_LAMPIMAMPAA_PHRASE);
}

// Handle fractile88 winter season case (hyvin leutoa)
Sentence build_fractile88_cold(const Sentence& theSpecifiedDay, const Sentence& theAreaPhrase)
{
  return build_anomaly_sentence_with_suffix(
      theSpecifiedDay, theAreaPhrase,
      SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE,
      ALUEELLA_SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE,
      MAANANTAINA_SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE,
      MAANANTAINA_ALUEELLA_SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE,
      HYVIN_WORD, LEUTOA_WORD);
}

Sentence temperature_anomaly_sentence(temperature_anomaly_params& theParameters,
                                      float fractile02Share,
                                      float fractile12Share,
                                      float fractile88Share,
                                      float fractile98Share,
                                      const WeatherPeriod& thePeriod)
{
  Sentence theSpecifiedDay;
  if (theParameters.thePeriodLength > 24)
  {
    theSpecifiedDay << PeriodPhraseFactory::create("today",
                                                   theParameters.theVariable,
                                                   theParameters.theForecastTime,
                                                   thePeriod,
                                                   theParameters.theArea);
  }

  const Sentence theAreaPhrase = build_area_phrase(theParameters.theArea);

  const bool isSummer = (theParameters.theSeason == SUMMER_SEASON);
  const bool isWarmSeason = isSummer || theParameters.theGrowingSeasonUnderway;
  const float adequateShare(80.0);

  if (fractile02Share >= adequateShare)
  {
    theParameters.theAnomalyPhrase =
        (isSummer ? SAA_ON_POIKKEUKSELLISEN_KOLEAA : SAA_ON_POIKKEUKSELLISEN_KYLMAA);
    return build_anomaly_sentence_with_suffix(
        theSpecifiedDay, theAreaPhrase,
        SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE,
        ALUEELLA_SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE,
        MAANANTAINA_SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE,
        MAANANTAINA_ALUEELLA_SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE,
        POIKKEUKSELLISEN_WORD, isSummer ? KOLEAA_WORD : KYLMAA_WORD);
  }

  if (fractile12Share >= adequateShare)
  {
    theParameters.theAnomalyPhrase = (isSummer ? SAA_ON_KOLEAA : SAA_ON_HYVIN_KYLMAA);
    return build_anomaly_sentence_with_suffix(
        theSpecifiedDay, theAreaPhrase,
        SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE,
        ALUEELLA_SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE,
        MAANANTAINA_SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE,
        MAANANTAINA_ALUEELLA_SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE,
        EMPTY_STRING, isSummer ? KOLEAA_WORD : KYLMAA_WORD);
  }

  if (fractile98Share >= adequateShare)
  {
    theParameters.theAnomalyPhrase =
        (isWarmSeason ? SAA_ON_POIKKEUKSLLISEN_LAMMINTA : SAA_ON_POIKKEUKSLLISEN_LEUTOA);
    return build_anomaly_sentence_with_suffix(
        theSpecifiedDay, theAreaPhrase,
        SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE,
        ALUEELLA_SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE,
        MAANANTAINA_SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE,
        MAANANTAINA_ALUEELLA_SAA_ON_POIKKEUKSELLISEN_KYLMAA_COMPOSITE_PHRASE,
        POIKKEUKSELLISEN_WORD, isWarmSeason ? LAMMINTA_WORD : LEUTOA_WORD);
  }

  if (fractile88Share >= adequateShare)
  {
    theParameters.theAnomalyPhrase =
        (isSummer ? SAA_ON_HARVINAISEN_LAMMINTA : SAA_ON_HYVIN_LEUTOA);
    if (isWarmSeason)
      return build_fractile88_warm(theSpecifiedDay, theAreaPhrase);
    return build_fractile88_cold(theSpecifiedDay, theAreaPhrase);
  }

  return Sentence();
}

Sentence get_shortruntrend_sentence(const std::string& theDayAndAreaIncludedCompositePhrase,
                                    const std::string& theDayIncludedCompositePhrase,
                                    const std::string& theAreaIncludedCompositePhrase,
                                    const std::string& theTemperatureSentence,
                                    const Sentence& theSpecifiedDay,
                                    const Sentence& theAreaPhrase)
{
  Sentence sentence;

  if (!theSpecifiedDay.empty() && !theAreaPhrase.empty())
  {
    sentence << theDayAndAreaIncludedCompositePhrase << theSpecifiedDay << theAreaPhrase;
  }
  else if (!theSpecifiedDay.empty())
  {
    sentence << theDayIncludedCompositePhrase << theSpecifiedDay;
  }
  else if (!theAreaPhrase.empty())
  {
    sentence << theAreaIncludedCompositePhrase << theAreaPhrase;
  }
  else
  {
    sentence << theTemperatureSentence;
  }

  return sentence;
}

// Holds pre-computed temperature differences and change flags for shortrun trend
struct TrendTemps
{
  double dayBefore, day1, day2, dayAfter;
  float diff12, diff1After, diffBefore2, diffBeforeAfter;
  bool smallChange, moderateChange, signifigantChange, gettingLower;
};

// Build TrendTemps from the parameters
TrendTemps build_trend_temps(const temperature_anomaly_params& theParameters)
{
  const bool isWinter = (theParameters.theSeason == WINTER_SEASON);
  TrendTemps t;
  t.dayBefore = isWinter ? theParameters.theDayBeforeDay1TemperatureAreaAfternoonMean.value()
                         : theParameters.theDayBeforeDay1TemperatureAreaAfternoonMaximum.value();
  t.day1 = isWinter ? theParameters.theDay1TemperatureAreaAfternoonMean.value()
                    : theParameters.theDay1TemperatureAreaAfternoonMaximum.value();
  t.day2 = isWinter ? theParameters.theDay2TemperatureAreaAfternoonMean.value()
                    : theParameters.theDay2TemperatureAreaAfternoonMaximum.value();
  t.dayAfter = isWinter ? theParameters.theDayAfterDay2TemperatureAreaAfternoonMean.value()
                        : theParameters.theDayAfterDay2TemperatureAreaAfternoonMaximum.value();
  t.diff12 = abs(t.day2 - t.day1);
  t.diff1After = abs(t.dayAfter - t.day1);
  t.diffBefore2 = abs(t.day2 - t.dayBefore);
  t.diffBeforeAfter = abs(t.dayAfter - t.dayBefore);
  auto inRange = [](float d, float lo, float hi) { return d >= lo && d < hi; };
  t.smallChange = inRange(t.diff12, SMALL_CHANGE_LOWER_LIMIT, SMALL_CHANGE_UPPER_LIMIT) &&
                  inRange(t.diff1After, SMALL_CHANGE_LOWER_LIMIT, SMALL_CHANGE_UPPER_LIMIT) &&
                  inRange(t.diffBefore2, SMALL_CHANGE_LOWER_LIMIT, SMALL_CHANGE_UPPER_LIMIT) &&
                  inRange(t.diffBeforeAfter, SMALL_CHANGE_LOWER_LIMIT, SMALL_CHANGE_UPPER_LIMIT);
  t.moderateChange =
      inRange(t.diff12, MODERATE_CHANGE_LOWER_LIMIT, MODERATE_CHANGE_UPPER_LIMIT) &&
      inRange(t.diff1After, MODERATE_CHANGE_LOWER_LIMIT, MODERATE_CHANGE_UPPER_LIMIT) &&
      inRange(t.diffBefore2, MODERATE_CHANGE_LOWER_LIMIT, MODERATE_CHANGE_UPPER_LIMIT) &&
      inRange(t.diffBeforeAfter, MODERATE_CHANGE_LOWER_LIMIT, MODERATE_CHANGE_UPPER_LIMIT);
  t.signifigantChange = t.diff12 >= SIGNIFIGANT_CHANGE_LOWER_LIMIT &&
                        t.diff1After >= SIGNIFIGANT_CHANGE_LOWER_LIMIT &&
                        t.diffBefore2 >= SIGNIFIGANT_CHANGE_LOWER_LIMIT &&
                        t.diffBeforeAfter >= SIGNIFIGANT_CHANGE_LOWER_LIMIT;
  t.gettingLower = t.day2 < t.day1 && t.dayAfter < t.day1 &&
                   t.day2 < t.dayBefore && t.dayAfter < t.dayBefore;
  return t;
}

// Handle winter season shortrun trend
Sentence winter_shortruntrend_sentence(temperature_anomaly_params& theParameters,
                                       const TrendTemps& t,
                                       float veryColdTemperature,
                                       double fractile63Day2,
                                       double fractile63DayAfter,
                                       const Sentence& theSpecifiedDay,
                                       const Sentence& theAreaPhrase)
{
  Sentence sentence;
  // saa on edelleen lauhaa
  // saa lauhtuu
  // kirea pakkanen heikkenee / hellittaa
  // pakkanen heikkenee
  // kirea pakkanen jatkuu
  // pakkanen kiristyy

  if (!t.gettingLower)
  {
    if (t.day1 > MILD_TEMPERATURE_LOWER_LIMIT && t.day1 < MILD_TEMPERATURE_UPPER_LIMIT &&
        t.dayBefore > MILD_TEMPERATURE_LOWER_LIMIT && t.dayBefore < MILD_TEMPERATURE_UPPER_LIMIT &&
        t.day2 > MILD_TEMPERATURE_LOWER_LIMIT && t.day2 < MILD_TEMPERATURE_UPPER_LIMIT &&
        t.dayAfter > MILD_TEMPERATURE_LOWER_LIMIT && t.dayAfter < MILD_TEMPERATURE_UPPER_LIMIT &&
        t.day2 > fractile63Day2 && t.dayAfter > fractile63DayAfter &&
        !theParameters.theGrowingSeasonUnderway)
    {
      sentence << get_shortruntrend_sentence(
          MAANANTAINA_ALUEELLA_SAA_ON_EDELLEEN_LAUHAA_COMPOSITE_PHRASE,
          MAANANTAINA_SAA_ON_EDELLEEN_LAUHAA_COMPOSITE_PHRASE,
          ALUEELLA_SAA_ON_EDELLEEN_LAUHAA_COMPOSITE_PHRASE,
          SAA_ON_EDELLEEN_LAUHAA_PHRASE, theSpecifiedDay, theAreaPhrase);
      theParameters.theShortrunTrend = SAA_ON_EDELLEEN_LAUHAA;
    }
    else if (t.signifigantChange && t.dayBefore < MILD_TEMPERATURE_LOWER_LIMIT &&
             t.day1 < MILD_TEMPERATURE_LOWER_LIMIT &&
             t.day2 >= MILD_TEMPERATURE_LOWER_LIMIT && t.day2 < MILD_TEMPERATURE_UPPER_LIMIT &&
             t.dayAfter >= MILD_TEMPERATURE_LOWER_LIMIT && t.dayAfter < MILD_TEMPERATURE_UPPER_LIMIT &&
             !theParameters.theGrowingSeasonUnderway)
    {
      sentence << get_shortruntrend_sentence(MAANANTAINA_ALUEELLA_SAA_LAUHTUU_COMPOSITE_PHRASE,
                                             MAANANTAINA_SAA_LAUHTUU_COMPOSITE_PHRASE,
                                             ALUEELLA_SAA_LAUHTUU_COMPOSITE_PHRASE,
                                             SAA_LAUHTUU_PHRASE, theSpecifiedDay, theAreaPhrase);
      theParameters.theShortrunTrend = SAA_LAUHTUU;
    }
    else if (t.signifigantChange && t.day1 <= veryColdTemperature &&
             t.dayBefore <= veryColdTemperature &&
             t.day2 <= MILD_TEMPERATURE_LOWER_LIMIT && t.dayAfter <= MILD_TEMPERATURE_LOWER_LIMIT)
    {
      sentence << get_shortruntrend_sentence(
          MAANANTAINA_ALUEELLA_KIREA_PAKKANEN_HEIKKENEE_COMPOSITE_PHRASE,
          MAANANTAINA_KIREA_PAKKANEN_HEIKKENEE_COMPOSITE_PHRASE,
          ALUEELLA_KIREA_PAKKANEN_HEIKKENEE_COMPOSITE_PHRASE,
          KIREA_PAKKANEN_HEIKKENEE_PHRASE, theSpecifiedDay, theAreaPhrase);
      theParameters.theShortrunTrend = KIREA_PAKKANEN_HEIKKENEE;
    }
    else if (t.signifigantChange && t.day1 > veryColdTemperature &&
             t.dayBefore > veryColdTemperature &&
             t.day2 < MILD_TEMPERATURE_LOWER_LIMIT && t.dayAfter < MILD_TEMPERATURE_LOWER_LIMIT)
    {
      sentence << get_shortruntrend_sentence(
          MAANANTAINA_ALUEELLA_PAKKANEN_HEIKKENEE_COMPOSITE_PHRASE,
          MAANANTAINA_PAKKANEN_HEIKKENEE_COMPOSITE_PHRASE,
          ALUEELLA_PAKKANEN_HEIKKENEE_COMPOSITE_PHRASE,
          PAKKANEN_HEIKKENEE_PHRASE, theSpecifiedDay, theAreaPhrase);
      theParameters.theShortrunTrend = PAKKANEN_HEIKKENEE;
    }
    else if (t.signifigantChange && t.dayBefore > veryColdTemperature &&
             t.dayBefore < MILD_TEMPERATURE_LOWER_LIMIT &&
             t.day1 > veryColdTemperature && t.day1 < MILD_TEMPERATURE_LOWER_LIMIT &&
             t.day1 < ZERO_DEGREES && t.day2 >= MILD_TEMPERATURE_LOWER_LIMIT && t.day2 < ZERO_DEGREES)
    {
      // redundant: this will never happen, because "saa lauhtuu" is tested before
      sentence << get_shortruntrend_sentence(
          MAANANTAINA_ALUEELLA_PAKKANEN_HELLITTAA_COMPOSITE_PHRASE,
          MAANANTAINA_PAKKANEN_HELLITTAA_COMPOSITE_PHRASE,
          ALUEELLA_PAKKANEN_HELLITTAA_COMPOSITE_PHRASE,
          PAKKANEN_HELLITTAA_PHRASE, theSpecifiedDay, theAreaPhrase);
      theParameters.theShortrunTrend = PAKKANEN_HELLITTAA;
    }
    else if (t.dayBefore < veryColdTemperature && t.day1 < veryColdTemperature &&
             t.day2 < veryColdTemperature && t.dayAfter < veryColdTemperature)
    {
      sentence << get_shortruntrend_sentence(
          MAANANTAINA_ALUEELLA_KIREA_PAKKANEN_JATKUU_COMPOSITE_PHRASE,
          MAANANTAINA_KIREA_PAKKANEN_JATKUU_COMPOSITE_PHRASE,
          ALUEELLA_KIREA_PAKKANEN_JATKUU_COMPOSITE_PHRASE,
          KIREA_PAKKANEN_JATKUU_PHRASE, theSpecifiedDay, theAreaPhrase);
      theParameters.theShortrunTrend = KIREA_PAKKANEN_JATKUU;
    }
  }
  else  // gettingLower
  {
    if (t.signifigantChange && t.dayBefore < MILD_TEMPERATURE_LOWER_LIMIT &&
        t.day1 < MILD_TEMPERATURE_LOWER_LIMIT &&
        t.day2 <= veryColdTemperature && t.dayAfter <= veryColdTemperature)
    {
      sentence << get_shortruntrend_sentence(
          MAANANTAINA_ALUEELLA_PAKKANEN_KIRISTYY_COMPOSITE_PHRASE,
          MAANANTAINA_PAKKANEN_KIRISTYY_COMPOSITE_PHRASE,
          ALUEELLA_PAKKANEN_KIRISTYY_COMPOSITE_PHRASE,
          PAKKANEN_KIRISTYY_PHRASE, theSpecifiedDay, theAreaPhrase);
      theParameters.theShortrunTrend = PAKKANEN_KIRISTYY;
    }
    else if (t.dayBefore < veryColdTemperature && t.day1 < veryColdTemperature &&
             t.day2 < veryColdTemperature && t.dayAfter < veryColdTemperature)
    {
      sentence << get_shortruntrend_sentence(
          MAANANTAINA_ALUEELLA_KIREA_PAKKANEN_JATKUU_COMPOSITE_PHRASE,
          MAANANTAINA_KIREA_PAKKANEN_JATKUU_COMPOSITE_PHRASE,
          ALUEELLA_KIREA_PAKKANEN_JATKUU_COMPOSITE_PHRASE,
          KIREA_PAKKANEN_JATKUU_PHRASE, theSpecifiedDay, theAreaPhrase);
      theParameters.theShortrunTrend = KIREA_PAKKANEN_JATKUU;
    }
  }
  return sentence;
}

// Common check: all temps below f12 fractile thresholds (kolea jatkuu)
bool all_below_f12(const TrendTemps& t, double f12DayBefore, double f12Day1,
                   double f12Day2, double f12DayAfter)
{
  return t.dayBefore < f12DayBefore && t.day1 < f12Day1 &&
         t.day2 < f12Day2 && t.dayAfter < f12DayAfter;
}

// Common check: all temps below f37 fractile thresholds (viilea jatkuu)
bool all_below_f37(const TrendTemps& t, double f37DayBefore, double f37Day1,
                   double f37Day2, double f37DayAfter)
{
  return t.dayBefore < f37DayBefore && t.day1 < f37Day1 &&
         t.day2 < f37Day2 && t.dayAfter < f37DayAfter;
}

// Summer warming branch (temperature not getting lower)
Sentence summer_warming_sentence(temperature_anomaly_params& theParameters,
                                 const TrendTemps& t,
                                 float hot_weather_limit,
                                 double f12DayBefore, double f12Day1, double f12Day2, double f12DayAfter,
                                 double f37DayBefore, double f37Day1, double f37Day2, double f37DayAfter,
                                 const Sentence& theSpecifiedDay, const Sentence& theAreaPhrase)
{
  Sentence sentence;
  if (t.dayBefore >= hot_weather_limit && t.day1 >= hot_weather_limit &&
      t.day2 >= hot_weather_limit && t.dayAfter >= hot_weather_limit)
  {
    sentence << get_shortruntrend_sentence(
        MAANANTAINA_ALUEELLA_HELTEINEN_SAA_JATKUU_COMPOSITE_PHRASE,
        MAANANTAINA_HELTEINEN_SAA_JATKUU_COMPOSITE_PHRASE,
        ALUEELLA_HELTEINEN_SAA_JATKUU_COMPOSITE_PHRASE,
        HELTEINEN_SAA_JATKUU_PHRASE, theSpecifiedDay, theAreaPhrase);
    theParameters.theShortrunTrend = HELTEINEN_SAA_JATKUU;
  }
  else if (all_below_f12(t, f12DayBefore, f12Day1, f12Day2, f12DayAfter))
  {
    sentence << get_shortruntrend_sentence(
        MAANANTAINA_ALUEELLA_KOLEA_SAA_JATKUU_COMPOSITE_PHRASE,
        MAANANTAINA_KOLEA_SAA_JATKUU_COMPOSITE_PHRASE,
        ALUEELLA_KOLEA_SAA_JATKUU_COMPOSITE_PHRASE,
        KOLEA_SAA_JATKUU_PHRASE, theSpecifiedDay, theAreaPhrase);
    theParameters.theShortrunTrend = KOLEA_SAA_JATKUU;
  }
  else if (all_below_f37(t, f37DayBefore, f37Day1, f37Day2, f37DayAfter))
  {
    sentence << get_shortruntrend_sentence(
        MAANANTAINA_ALUEELLA_VIILEA_SAA_JATKUU_COMPOSITE_PHRASE,
        MAANANTAINA_VIILEA_SAA_JATKUU_COMPOSITE_PHRASE,
        ALUEELLA_VIILEA_SAA_JATKUU_COMPOSITE_PHRASE,
        VIILEA_SAA_JATKUU_PHRASE, theSpecifiedDay, theAreaPhrase);
    theParameters.theShortrunTrend = VIILEA_SAA_JATKUU;
  }
  else if (t.dayBefore < hot_weather_limit && t.day1 < hot_weather_limit &&
           t.day2 >= hot_weather_limit && t.dayAfter >= hot_weather_limit)
  {
    const bool notable = t.diff12 >= NOTABLE_TEMPERATURE_CHANGE_LIMIT &&
                         t.diff1After >= NOTABLE_TEMPERATURE_CHANGE_LIMIT &&
                         t.diffBefore2 >= NOTABLE_TEMPERATURE_CHANGE_LIMIT &&
                         t.diffBeforeAfter >= NOTABLE_TEMPERATURE_CHANGE_LIMIT;
    if (notable)
    {
      sentence << get_shortruntrend_sentence(
          MAANANTAINA_ALUEELLA_SAA_MUUTTUU_HELTEISEKSI_COMPOSITE_PHRASE,
          MAANANTAINA_SAA_MUUTTUU_HELTEISEKSI_COMPOSITE_PHRASE,
          ALUEELLA_SAA_MUUTTUU_HELTEISEKSI_COMPOSITE_PHRASE,
          SAA_MUUTTUU_HELTEISEKSI_PHRASE, theSpecifiedDay, theAreaPhrase);
      theParameters.theShortrunTrend = SAA_MUUTTUU_HELTEISEKSI;
    }
    else
    {
      sentence << get_shortruntrend_sentence(
          MAANANTAINA_ALUEELLA_SAA_ON_HELTEISTA_COMPOSITE_PHRASE,
          MAANANTAINA_SAA_ON_HELTEISTA_COMPOSITE_PHRASE,
          ALUEELLA_SAA_ON_HELTEISTA_COMPOSITE_PHRASE,
          SAA_ON_HELTEISTA_PHRASE, theSpecifiedDay, theAreaPhrase);
      theParameters.theShortrunTrend = SAA_ON_HELTEISTA;
    }
  }
  else if (t.signifigantChange && t.day2 < 25.0)
  {
    sentence << get_shortruntrend_sentence(
        MAANANTAINA_ALUEELLA_SAA_LAMPENEE_HUOMATTAVASTI_COMPOSITE_PHRASE,
        MAANANTAINA_SAA_LAMPENEE_HUOMATTAVASTI_COMPOSITE_PHRASE,
        ALUEELLA_SAA_LAMPENEE_HUOMATTAVASTI_COMPOSITE_PHRASE,
        SAA_LAMPENEE_HUOMATTAVASTI_PHRASE, theSpecifiedDay, theAreaPhrase);
    theParameters.theShortrunTrend = SAA_LAMPENEE_HUOMATTAVASTI;
  }
  else if (t.moderateChange && t.day2 < 25.0)
  {
    sentence << get_shortruntrend_sentence(MAANANTAINA_ALUEELLA_SAA_LAMPENEE_COMPOSITE_PHRASE,
                                           MAANANTAINA_SAA_LAMPENEE_COMPOSITE_PHRASE,
                                           ALUEELLA_SAA_LAMPENEE_COMPOSITE_PHRASE,
                                           SAA_LAMPENEE_PHRASE, theSpecifiedDay, theAreaPhrase);
    theParameters.theShortrunTrend = SAA_LAMPENEE;
  }
  else if (t.smallChange && t.day2 < 20.0)
  {
    sentence << get_shortruntrend_sentence(
        MAANANTAINA_ALUEELLA_SAA_LAMPENEE_VAHAN_COMPOSITE_PHRASE,
        MAANANTAINA_SAA_LAMPENEE_VAHAN_COMPOSITE_PHRASE,
        ALUEELLA_SAA_LAMPENEE_VAHAN_COMPOSITE_PHRASE,
        SAA_LAMPENEE_VAHAN_PHRASE, theSpecifiedDay, theAreaPhrase);
    theParameters.theShortrunTrend = SAA_LAMPENEE_VAHAN;
  }
  return sentence;
}

// Summer cooling branch (temperature getting lower, day2 > 0)
Sentence summer_cooling_sentence(temperature_anomaly_params& theParameters,
                                 const TrendTemps& t,
                                 float hot_weather_limit,
                                 double f12DayBefore, double f12Day1, double f12Day2, double f12DayAfter,
                                 double f37DayBefore, double f37Day1, double f37Day2, double f37DayAfter,
                                 const Sentence& theSpecifiedDay, const Sentence& theAreaPhrase)
{
  Sentence sentence;
  if (t.dayBefore >= hot_weather_limit && t.day1 >= hot_weather_limit &&
      t.day2 >= hot_weather_limit && t.dayAfter >= hot_weather_limit)
  {
    sentence << get_shortruntrend_sentence(
        MAANANTAINA_ALUEELLA_HELTEINEN_SAA_JATKUU_COMPOSITE_PHRASE,
        MAANANTAINA_HELTEINEN_SAA_JATKUU_COMPOSITE_PHRASE,
        ALUEELLA_HELTEINEN_SAA_JATKUU_COMPOSITE_PHRASE,
        HELTEINEN_SAA_JATKUU_PHRASE, theSpecifiedDay, theAreaPhrase);
    theParameters.theShortrunTrend = HELTEINEN_SAA_JATKUU;
  }
  else if (all_below_f12(t, f12DayBefore, f12Day1, f12Day2, f12DayAfter))
  {
    sentence << get_shortruntrend_sentence(
        MAANANTAINA_ALUEELLA_KOLEA_SAA_JATKUU_COMPOSITE_PHRASE,
        MAANANTAINA_KOLEA_SAA_JATKUU_COMPOSITE_PHRASE,
        ALUEELLA_KOLEA_SAA_JATKUU_COMPOSITE_PHRASE,
        KOLEA_SAA_JATKUU_PHRASE, theSpecifiedDay, theAreaPhrase);
    theParameters.theShortrunTrend = KOLEA_SAA_JATKUU;
  }
  else if (all_below_f37(t, f37DayBefore, f37Day1, f37Day2, f37DayAfter))
  {
    sentence << get_shortruntrend_sentence(
        MAANANTAINA_ALUEELLA_VIILEA_SAA_JATKUU_COMPOSITE_PHRASE,
        MAANANTAINA_VIILEA_SAA_JATKUU_COMPOSITE_PHRASE,
        ALUEELLA_VIILEA_SAA_JATKUU_COMPOSITE_PHRASE,
        VIILEA_SAA_JATKUU_PHRASE, theSpecifiedDay, theAreaPhrase);
    theParameters.theShortrunTrend = VIILEA_SAA_JATKUU;
  }
  else if (t.signifigantChange && t.day2 < 25.0)
  {
    sentence << get_shortruntrend_sentence(
        MAANANTAINA_ALUEELLA_SAA_VIILENEE_HUOMATTAVASTI_COMPOSITE_PHRASE,
        MAANANTAINA_SAA_VIILENEE_HUOMATTAVASTI_COMPOSITE_PHRASE,
        ALUEELLA_SAA_VIILENEE_HUOMATTAVASTI_COMPOSITE_PHRASE,
        SAA_VIILENEE_HUOMATTAVASTI_PHRASE, theSpecifiedDay, theAreaPhrase);
    theParameters.theShortrunTrend = SAA_VIILENEE_HUOMATTAVASTI;
  }
  else if (t.moderateChange && t.day2 < 25.0)
  {
    sentence << get_shortruntrend_sentence(MAANANTAINA_ALUEELLA_SAA_VIILENEE_COMPOSITE_PHRASE,
                                           MAANANTAINA_SAA_VIILENEE_COMPOSITE_PHRASE,
                                           ALUEELLA_SAA_VIILENEE_COMPOSITE_PHRASE,
                                           SAA_VIILENEE_PHRASE, theSpecifiedDay, theAreaPhrase);
    theParameters.theShortrunTrend = SAA_VIILENEE;
  }
  else if (t.smallChange && t.day2 < 20.0)
  {
    sentence << get_shortruntrend_sentence(
        MAANANTAINA_ALUEELLA_SAA_VIILENEE_VAHAN_COMPOSITE_PHRASE,
        MAANANTAINA_SAA_VIILENEE_VAHAN_COMPOSITE_PHRASE,
        ALUEELLA_SAA_VIILENEE_VAHAN_COMPOSITE_PHRASE,
        SAA_VIILENEE_VAHAN_PHRASE, theSpecifiedDay, theAreaPhrase);
    theParameters.theShortrunTrend = SAA_VIILENEE_VAHAN;
  }
  return sentence;
}

// Handle summer season shortrun trend
Sentence summer_shortruntrend_sentence(temperature_anomaly_params& theParameters,
                                       const TrendTemps& t,
                                       float hot_weather_limit,
                                       double f12DayBefore, double f12Day1,
                                       double f12Day2, double f12DayAfter,
                                       double f37DayBefore, double f37Day1,
                                       double f37Day2, double f37DayAfter,
                                       const Sentence& theSpecifiedDay,
                                       const Sentence& theAreaPhrase)
{
  if (!t.gettingLower)
    return summer_warming_sentence(theParameters, t, hot_weather_limit,
                                   f12DayBefore, f12Day1, f12Day2, f12DayAfter,
                                   f37DayBefore, f37Day1, f37Day2, f37DayAfter,
                                   theSpecifiedDay, theAreaPhrase);
  if (t.gettingLower && t.day2 > ZERO_DEGREES)
    return summer_cooling_sentence(theParameters, t, hot_weather_limit,
                                   f12DayBefore, f12Day1, f12Day2, f12DayAfter,
                                   f37DayBefore, f37Day1, f37Day2, f37DayAfter,
                                   theSpecifiedDay, theAreaPhrase);
  return Sentence();
}

Sentence temperature_shortruntrend_sentence(temperature_anomaly_params& theParameters,
                                            fractile_type_id theFractileType)
{
  const TrendTemps t = build_trend_temps(theParameters);

  // kova pakkanen: F12,5 fractile on 1. Feb 12:00
  TextGenPosixTime veryColdRefTime(theParameters.theForecastTime.GetYear(), 2, 1, 12, 0, 0);
  WeatherPeriod veryColdWeatherPeriod(veryColdRefTime, veryColdRefTime);

  WeatherResult wr = get_fractile_temperature(theParameters.theVariable,
                                              FRACTILE_12,
                                              theParameters.theSources,
                                              theParameters.theArea,
                                              veryColdWeatherPeriod,
                                              theFractileType);

  float veryColdTemperature = wr.value() < VERY_COLD_TEMPERATURE_UPPER_LIMIT
                                  ? wr.value()
                                  : VERY_COLD_TEMPERATURE_UPPER_LIMIT;

  WeatherPeriod fractileTemperatureDay1Period(get_afternoon_period(
      theParameters.theVariable, theParameters.theDay1Period.localStartTime()));
  WeatherPeriod fractileTemperatureDayBeforeDay1Period(get_afternoon_period(
      theParameters.theVariable, theParameters.theDayBeforeDay1Period.localStartTime()));
  WeatherPeriod fractileTemperatureDay2Period(get_afternoon_period(
      theParameters.theVariable, theParameters.theDay2Period.localStartTime()));
  WeatherPeriod fractileTemperatureDayAfterDay2Period(get_afternoon_period(
      theParameters.theVariable, theParameters.theDayAfterDay2Period.localStartTime()));

  auto getFractile = [&](fractile_id fid, const WeatherPeriod& period) {
    return get_fractile_temperature(
        theParameters.theVariable, fid, theParameters.theSources,
        theParameters.theArea, period, theFractileType);
  };

  const double f12DayBefore = getFractile(FRACTILE_12, fractileTemperatureDayBeforeDay1Period).value();
  const double f37DayBefore = getFractile(FRACTILE_37, fractileTemperatureDayBeforeDay1Period).value();
  const double f63DayBefore = getFractile(FRACTILE_63, fractileTemperatureDayBeforeDay1Period).value();
  const double f12Day1 = getFractile(FRACTILE_12, fractileTemperatureDay1Period).value();
  const double f37Day1 = getFractile(FRACTILE_37, fractileTemperatureDay1Period).value();
  const double f63Day1 = getFractile(FRACTILE_63, fractileTemperatureDay1Period).value();
  const double f12Day2 = getFractile(FRACTILE_12, fractileTemperatureDay2Period).value();
  const double f37Day2 = getFractile(FRACTILE_37, fractileTemperatureDay2Period).value();
  const double f63Day2 = getFractile(FRACTILE_63, fractileTemperatureDay2Period).value();
  const double f12DayAfter = getFractile(FRACTILE_12, fractileTemperatureDayAfterDay2Period).value();
  const double f37DayAfter = getFractile(FRACTILE_37, fractileTemperatureDayAfterDay2Period).value();
  const double f63DayAfter = getFractile(FRACTILE_63, fractileTemperatureDayAfterDay2Period).value();

  theParameters.theLog << "thePeriod: " << period2string(theParameters.thePeriod) << '\n';
  theParameters.theLog << "fractileTemperatureDayBeforeDay1Period: "
                       << period2string(fractileTemperatureDayBeforeDay1Period) << '\n';
  theParameters.theLog << "fractileTemperatureDay1Period: "
                       << period2string(fractileTemperatureDay1Period) << '\n';
  theParameters.theLog << "fractileTemperatureDay2Period: "
                       << period2string(fractileTemperatureDay2Period) << '\n';
  theParameters.theLog << "fractileTemperatureDayAfterDay2Period: "
                       << period2string(fractileTemperatureDayAfterDay2Period) << '\n';
  theParameters.theLog << "F12/F37/F63 dayBefore: " << f12DayBefore << "/" << f37DayBefore << "/" << f63DayBefore << '\n';
  theParameters.theLog << "F12/F37/F63 day1: " << f12Day1 << "/" << f37Day1 << "/" << f63Day1 << '\n';
  theParameters.theLog << "F12/F37/F63 day2: " << f12Day2 << "/" << f37Day2 << "/" << f63Day2 << '\n';
  theParameters.theLog << "F12/F37/F63 dayAfter: " << f12DayAfter << "/" << f37DayAfter << "/" << f63DayAfter << '\n';

  Sentence theSpecifiedDay;
  if (theParameters.thePeriodLength > 24)
  {
    theSpecifiedDay << PeriodPhraseFactory::create("today",
                                                   theParameters.theVariable,
                                                   theParameters.theForecastTime,
                                                   theParameters.theDay2Period,
                                                   theParameters.theArea);
  }

  const Sentence theAreaPhrase = build_area_phrase(theParameters.theArea);

  if (theParameters.theSeason == WINTER_SEASON)
  {
    return winter_shortruntrend_sentence(
        theParameters, t, veryColdTemperature, f63Day2, f63DayAfter,
        theSpecifiedDay, theAreaPhrase);
  }

  // summer season
  float hot_weather_limit = Settings::optional_double(
      theParameters.theVariable + "::hot_weather_limit", HOT_WEATHER_LIMIT);

  return summer_shortruntrend_sentence(
      theParameters, t, hot_weather_limit,
      f12DayBefore, f12Day1, f12Day2, f12DayAfter,
      f37DayBefore, f37Day1, f37Day2, f37DayAfter,
      theSpecifiedDay, theAreaPhrase);
}

Sentence handle_anomaly_and_shortrun_trend_sentences(
    const temperature_anomaly_params& theParameters,
    const Sentence& anomalySentence,
    const Sentence& shortrunTrendSentence)
{
  Sentence sentence;

  if (theParameters.theAnomalyPhrase == SAA_ON_POIKKEUKSELLISEN_KOLEAA ||
      theParameters.theAnomalyPhrase == SAA_ON_HARVINAISEN_LAMMINTA ||
      theParameters.theAnomalyPhrase == SAA_ON_POIKKEUKSLLISEN_LAMMINTA ||
      theParameters.theAnomalyPhrase == SAA_ON_HYVIN_LEUTOA)
  {
    sentence << anomalySentence;
  }
  else if (theParameters.theAnomalyPhrase == SAA_ON_KOLEAA)
  {
    switch (theParameters.theShortrunTrend)
    {
      case SAA_LAMPENEE_HUOMATTAVASTI:
      case SAA_VIILENEE_HUOMATTAVASTI:
        sentence << shortrunTrendSentence;
        break;
      default:
        sentence << anomalySentence;
    }
  }
  else if (theParameters.theAnomalyPhrase == SAA_ON_POIKKEUKSELLISEN_KYLMAA ||
           theParameters.theAnomalyPhrase == SAA_ON_HYVIN_KYLMAA)
  {
    switch (theParameters.theShortrunTrend)
    {
      case KIREA_PAKKANEN_HEIKKENEE:
      case PAKKANEN_HEIKKENEE:
      case SAA_LAUHTUU:
        sentence << shortrunTrendSentence;
        break;
      default:
        sentence << anomalySentence;
    }
  }
  else
  {
    sentence << shortrunTrendSentence;
  }

  return sentence;
}

Paragraph anomaly(const TextGen::WeatherArea& itsArea,
                  const TextGen::WeatherPeriod& itsPeriod,
                  const TextGen::AnalysisSources& itsSources,
                  const TextGenPosixTime& itsForecastTime,
                  const std::string& itsVar,
                  const std::string& theAreaName,
                  MessageLogger& theLog)
{
  using namespace TemperatureAnomaly;

  Paragraph paragraph;

  log_start_time_and_end_time(theLog, "the original period: ", itsPeriod);

  forecast_season_id theSeasonId =
      isSummerHalf(itsPeriod.localStartTime(), itsVar) ? SUMMER_SEASON : WINTER_SEASON;

  // the anomaly sentence relates always to the upcoming day,
  // so the period is defined to start in the previous day

  TextGenPosixTime periodStartTime = itsPeriod.localStartTime();
  const TextGenPosixTime& periodEndTime = itsPeriod.localEndTime();
  int periodLength = periodEndTime.DifferenceInHours(periodStartTime);

  int ndays = HourPeriodGenerator(itsPeriod, itsVar + "::day").size();

  theLog << "Period " << itsPeriod.localStartTime() << "..." << itsPeriod.localEndTime()
         << " covers " << ndays << " days\n";

  if (ndays <= 0)
  {
    theLog << paragraph;
    return paragraph;
  }
  if (ndays == 1)
  {
    periodStartTime.ChangeByDays(-1);
  }

  WeatherPeriod theExtendedPeriod(periodStartTime, periodEndTime);

  HourPeriodGenerator generator(theExtendedPeriod, itsVar + "::day");
  for (unsigned int i = 0; i < generator.size(); i++)
  {
    log_start_time_and_end_time(theLog, "The extended period: ", generator.period(i + 1));
  }

  WeatherPeriod day1Period(generator.period(1));
  WeatherPeriod day2Period(generator.period(2));

  TextGenPosixTime periodDayBeforeDay1StartTime;
  TextGenPosixTime periodDayBeforeDay1EndTime;
  TextGenPosixTime periodDayAfterDay2StartTime;
  TextGenPosixTime periodDayAfterDay2EndTime;

  periodDayBeforeDay1StartTime = day1Period.localStartTime();
  periodDayBeforeDay1StartTime.ChangeByHours(-24);
  periodDayBeforeDay1EndTime = day1Period.localEndTime();
  periodDayBeforeDay1EndTime.ChangeByHours(-24);

  periodDayAfterDay2StartTime = day2Period.localStartTime();
  periodDayAfterDay2StartTime.ChangeByHours(24);
  periodDayAfterDay2EndTime = day2Period.localEndTime();
  periodDayAfterDay2EndTime.ChangeByHours(24);

  WeatherPeriod dayBeforeDay1Period(periodDayBeforeDay1StartTime, periodDayBeforeDay1EndTime);
  WeatherPeriod dayAfterDay2Period(periodDayAfterDay2StartTime, periodDayAfterDay2EndTime);

  log_start_time_and_end_time(theLog, "day before day1: ", dayBeforeDay1Period);
  log_start_time_and_end_time(theLog, "day1: ", day1Period);
  log_start_time_and_end_time(theLog, "day2: ", day2Period);
  log_start_time_and_end_time(theLog, "day after day2: ", dayAfterDay2Period);

  bool report_day2_anomaly = true;

  temperature_anomaly_params parameters(itsVar,
                                        theLog,
                                        itsSources,
                                        itsArea,
                                        theExtendedPeriod,
                                        dayBeforeDay1Period,
                                        day1Period,
                                        day2Period,
                                        dayAfterDay2Period,
                                        theSeasonId,
                                        itsForecastTime,
                                        periodLength);

  afternoon_temperature(itsVar + "::fake::temperature::day_before_day1::afternoon::area",
                        itsSources,
                        itsArea,
                        dayBeforeDay1Period,
                        parameters.theDayBeforeDay1TemperatureAreaAfternoonMinimum,
                        parameters.theDayBeforeDay1TemperatureAreaAfternoonMaximum,
                        parameters.theDayBeforeDay1TemperatureAreaAfternoonMean);

  afternoon_temperature(itsVar + "::fake::temperature::day1::afternoon::area",
                        itsSources,
                        itsArea,
                        day1Period,
                        parameters.theDay1TemperatureAreaAfternoonMinimum,
                        parameters.theDay1TemperatureAreaAfternoonMaximum,
                        parameters.theDay1TemperatureAreaAfternoonMean);

  afternoon_temperature(itsVar + "::fake::temperature::day2::afternoon::area",
                        itsSources,
                        itsArea,
                        day2Period,
                        parameters.theDay2TemperatureAreaAfternoonMinimum,
                        parameters.theDay2TemperatureAreaAfternoonMaximum,
                        parameters.theDay2TemperatureAreaAfternoonMean);

  afternoon_temperature(itsVar + "::fake::temperature::day_after_day2::afternoon::area",
                        itsSources,
                        itsArea,
                        dayAfterDay2Period,
                        parameters.theDayAfterDay2TemperatureAreaAfternoonMinimum,
                        parameters.theDayAfterDay2TemperatureAreaAfternoonMaximum,
                        parameters.theDayAfterDay2TemperatureAreaAfternoonMean);

  fractile_type_id fractileType(MAX_FRACTILE);

  WeatherPeriod fractileTemperatureDay1Period(
      get_afternoon_period(parameters.theVariable, parameters.theDay1Period.localStartTime()));
  WeatherPeriod fractileTemperatureDay2Period(
      get_afternoon_period(parameters.theVariable, parameters.theDay2Period.localStartTime()));

  fractile_id theFractileDay1 =
      get_fractile(itsVar,
                   parameters.theDay1TemperatureAreaAfternoonMaximum.value(),
                   itsSources,
                   itsArea,
                   fractileTemperatureDay1Period,
                   fractileType);

  fractile_id theFractileDay2 =
      get_fractile(itsVar,
                   parameters.theDay2TemperatureAreaAfternoonMaximum.value(),
                   itsSources,
                   itsArea,
                   fractileTemperatureDay2Period,
                   fractileType);

  WeatherResult fractileTemperatureDay1 = get_fractile_temperature(
      itsVar, theFractileDay1, itsSources, itsArea, fractileTemperatureDay1Period, fractileType);

  WeatherResult fractileTemperatureDay2 = get_fractile_temperature(
      itsVar, theFractileDay2, itsSources, itsArea, fractileTemperatureDay2Period, fractileType);

  WeatherPeriod fractileTemperaturePeriod(report_day2_anomaly ? fractileTemperatureDay2Period
                                                              : fractileTemperatureDay1Period);

  theLog << "Day1 maximum temperature " << parameters.theDay1TemperatureAreaAfternoonMaximum;
  if (theFractileDay1 == FRACTILE_100)
  {
    WeatherResult f98Temperature = get_fractile_temperature(
        itsVar, FRACTILE_98, itsSources, itsArea, fractileTemperatureDay1Period, fractileType);

    theLog << "is higher than F98 temperature " << f98Temperature << '\n';
  }
  else
  {
    theLog << " falls into fractile " << fractile_name(theFractileDay1) << fractileTemperatureDay1
           << '\n';
  }

  theLog << "Day2 maximum temperature " << parameters.theDay2TemperatureAreaAfternoonMaximum;
  if (theFractileDay2 == FRACTILE_100)
  {
    WeatherResult f98Temperature = get_fractile_temperature(
        itsVar, FRACTILE_98, itsSources, itsArea, fractileTemperatureDay2Period, fractileType);
    theLog << "is higher than F98 temperature " << f98Temperature << '\n';
  }
  else
  {
    theLog << " falls into fractile " << fractile_name(theFractileDay2) << fractileTemperatureDay2
           << '\n';
  }

  if (parameters.theDay1TemperatureAreaAfternoonMaximum.value() == kFloatMissing ||
      parameters.theDay2TemperatureAreaAfternoonMaximum.value() == kFloatMissing)
  {
    theLog << "Maximum temperature value(s) missing, no story generated!\n";
    theLog << paragraph;
    return paragraph;
  }

  WeatherResult fractile02Temperature = get_fractile_temperature(
      itsVar, FRACTILE_02, itsSources, itsArea, fractileTemperaturePeriod, fractileType);

  WeatherResult fractile12Temperature = get_fractile_temperature(
      itsVar, FRACTILE_12, itsSources, itsArea, fractileTemperaturePeriod, fractileType);

  WeatherResult fractile88Temperature = get_fractile_temperature(
      itsVar, FRACTILE_88, itsSources, itsArea, fractileTemperaturePeriod, fractileType);

  WeatherResult fractile98Temperature = get_fractile_temperature(
      itsVar, FRACTILE_98, itsSources, itsArea, fractileTemperaturePeriod, fractileType);

  GridForecaster theForecaster;
  RangeAcceptor upperLimitF02Acceptor;
  upperLimitF02Acceptor.upperLimit(fractile02Temperature.value());
  RangeAcceptor upperLimitF12Acceptor;
  upperLimitF12Acceptor.upperLimit(fractile12Temperature.value());
  RangeAcceptor lowerLimitF88Acceptor;
  lowerLimitF88Acceptor.lowerLimit(fractile88Temperature.value());
  RangeAcceptor lowerLimitF98Acceptor;
  lowerLimitF98Acceptor.lowerLimit(fractile98Temperature.value());

  WeatherResult fractile02Share = theForecaster.analyze(itsVar + "::fake::fractile::share::F02",
                                                        itsSources,
                                                        Temperature,
                                                        Percentage,
                                                        Maximum,
                                                        itsArea,
                                                        fractileTemperaturePeriod,
                                                        DefaultAcceptor(),
                                                        DefaultAcceptor(),
                                                        upperLimitF02Acceptor);

  WeatherResult fractile12Share = theForecaster.analyze(itsVar + "::fake::fractile::share::F12",
                                                        itsSources,
                                                        Temperature,
                                                        Percentage,
                                                        Maximum,
                                                        itsArea,
                                                        fractileTemperaturePeriod,
                                                        DefaultAcceptor(),
                                                        DefaultAcceptor(),
                                                        upperLimitF12Acceptor);

  WeatherResult fractile88Share = theForecaster.analyze(itsVar + "::fake::fractile::share::F88",
                                                        itsSources,
                                                        Temperature,
                                                        Percentage,
                                                        Maximum,
                                                        itsArea,
                                                        fractileTemperaturePeriod,
                                                        DefaultAcceptor(),
                                                        DefaultAcceptor(),
                                                        lowerLimitF88Acceptor);

  WeatherResult fractile98Share = theForecaster.analyze(itsVar + "::fake::fractile::share::F98",
                                                        itsSources,
                                                        Temperature,
                                                        Percentage,
                                                        Maximum,
                                                        itsArea,
                                                        fractileTemperaturePeriod,
                                                        DefaultAcceptor(),
                                                        DefaultAcceptor(),
                                                        lowerLimitF98Acceptor);

  theLog << "Fractiles for period: " << period2string(fractileTemperaturePeriod) << '\n';
  theLog << "Fractile 02 temperature and share (" << theAreaName << "): " << fractile02Temperature
         << "; " << fractile02Share << '\n';
  theLog << "Fractile 12 temperature and share (" << theAreaName << "): " << fractile12Temperature
         << "; " << fractile12Share << '\n';
  theLog << "Fractile 88 temperature and share (" << theAreaName << "): " << fractile88Temperature
         << "; " << fractile88Share << '\n';
  theLog << "Fractile 98 temperature and share (" << theAreaName << "): " << fractile98Temperature
         << "; " << fractile98Share << '\n';

  if (fractile02Temperature.value() == kFloatMissing ||
      fractile12Temperature.value() == kFloatMissing ||
      fractile88Temperature.value() == kFloatMissing ||
      fractile98Temperature.value() == kFloatMissing)
  {
    theLog << "Fractile temperature value(s) missing, no story generated!\n";
    theLog << paragraph;
    return paragraph;
  }

  if (fractile02Share.value() == kFloatMissing || fractile12Share.value() == kFloatMissing ||
      fractile88Share.value() == kFloatMissing || fractile98Share.value() == kFloatMissing)
  {
    theLog << "Fractile share value(s) missing, no story generated!\n";
    theLog << paragraph;
    return paragraph;
  }

  const WeatherPeriod& thePeriod =
      (report_day2_anomaly ? parameters.theDay2Period : parameters.theDay1Period);

  parameters.theGrowingSeasonUnderway =
      growing_season_going_on(itsArea, itsSources, thePeriod, itsVar);
  Sentence temperatureAnomalySentence;
  Sentence shortrunTrendSentence;

  temperatureAnomalySentence << temperature_anomaly_sentence(
      parameters,
      fractile02Share.value(),
      fractile12Share.value(),
      fractile88Share.value(),
      fractile98Share.value(),
      report_day2_anomaly ? parameters.theDay2Period : parameters.theDay1Period);

  shortrunTrendSentence << temperature_shortruntrend_sentence(parameters, fractileType);

  log_data(parameters);

  paragraph << handle_anomaly_and_shortrun_trend_sentences(
      parameters, temperatureAnomalySentence, shortrunTrendSentence);

  theLog << paragraph;

  return paragraph;
}

}  // namespace

}  // namespace TemperatureAnomaly

// ----------------------------------------------------------------------
/*!
 * \brief Generate temperature anomaly story
 *
 * \return The generated paragraph
 */
// ----------------------------------------------------------------------

Paragraph TemperatureStory::anomaly() const
{
  using namespace TemperatureAnomaly;

  Paragraph paragraph;
  MessageLogger log("TemperatureStory::anomaly");

  std::string areaName;

  if (itsArea.isNamed())
  {
    areaName = itsArea.name();
    log << areaName << '\n';
  }

  fs::path climatologyFile(Settings::optional_string("textgen::tmax_climatology", ""));

  if (!fs::exists(climatologyFile))
  {
    log << "The climatology file " << climatologyFile << " does not exist!\n";
    return paragraph;
  }

  WeatherArea areaOne(itsArea);
  WeatherArea areaTwo(itsArea);
  split_method splitMethod =
      check_area_splitting(itsVar, itsArea, itsPeriod, itsSources, areaOne, areaTwo, log);

  if (NO_SPLITTING != splitMethod)
  {
    Paragraph paragraphAreaOne;
    Paragraph paragraphAreaTwo;

    std::string areaId =
        (areaName + (splitMethod == HORIZONTAL ? " - southern part" : " - western part"));
    log << areaId << '\n';

    paragraphAreaOne << TemperatureAnomaly::anomaly(
        areaOne, itsPeriod, itsSources, itsForecastTime, itsVar, areaId, log);

    areaId = (areaName + (splitMethod == HORIZONTAL ? " - northern part" : " - eastern part"));
    log << areaId << '\n';

    paragraphAreaTwo << TemperatureAnomaly::anomaly(
        areaTwo, itsPeriod, itsSources, itsForecastTime, itsVar, areaId, log);
    paragraph << paragraphAreaOne << paragraphAreaTwo;
  }
  else
  {
    paragraph << TemperatureAnomaly::anomaly(
        itsArea, itsPeriod, itsSources, itsForecastTime, itsVar, areaName, log);
  }

  return paragraph;
}

}  // namespace TextGen

// ======================================================================
