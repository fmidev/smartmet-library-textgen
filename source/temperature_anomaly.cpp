// ======================================================================
/*!
 * \file
 *†\brief Implementation of method TextGen::TemperatureStory::anomaly
 */
// ======================================================================

#include "TemperatureStory.h"

#include "ClimatologyTools.h"
#include "GridClimatology.h"

#include "DefaultAcceptor.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "NightAndDayPeriodGenerator.h"
#include "Integer.h"
#include "MathTools.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Settings.h"
#include "TemperatureStoryTools.h"
#include "AreaTools.h"
#include "TextGenError.h"
#include "UnitFactory.h"
#include "WeatherPeriodTools.h"
#include "SeasonTools.h"
#include "WeatherResult.h"
#include "PeriodPhraseFactory.h"

#include <newbase/NFmiStringTools.h>
#include <newbase/NFmiGrid.h>
#include <newbase/NFmiQueryData.h>
#include <newbase/NFmiIndexMask.h>
#include <newbase/NFmiIndexMaskSource.h>
#include <newbase/NFmiIndexMaskTools.h>
#include <newbase/NFmiSettings.h>
#include <newbase/NFmiMetMath.h>

#include "WeatherAnalysisError.h"
#include "WeatherArea.h"
#include "WeatherSource.h"

#include <boost/lexical_cast.hpp>

#include <map>

namespace TextGen
{

  namespace TemperatureAnomaly
  {
	using NFmiStringTools::Convert;
	using namespace WeatherAnalysis;
	using namespace TemperatureStoryTools;
	using namespace boost;
	using namespace std;
	using namespace Settings;
	using namespace SeasonTools;
	using namespace AreaTools;
	using MathTools::to_precision;
	using Settings::optional_int;
	using Settings::optional_bool;
	using Settings::optional_string;

#define WEAK_FROST_TEMPERATURE_LIMIT -5.0
#define HOT_WEATHER_LIMIT +25.0
#define GETTING_COOLER_NOTIFICATION_LIMIT +20.0
#define LOW_PLUS_TEMPARATURE +5.0
#define ZERO_DEGREES 0.0
#define VERY_COLD_TEMPERATURE_UPPER_LIMIT -10.0
#define SMALL_CHANGE_LOWER_LIMIT 2.0
#define SMALL_CHANGE_UPPER_LIMIT 3.0
#define MODERATE_CHANGE_LOWER_LIMIT 3.0
#define MODERATE_CHANGE_UPPER_LIMIT 5.0
#define SIGNIFIGANT_CHANGE_LOWER_LIMIT 5.0
#define NOTABLE_TEMPERATURE_CHANGE_LIMIT 2.5
#define WINDY_WEATER_LIMIT 7.0
#define EXTREMELY_WINDY_WEATHER_LIMIT 10.0
#define WIND_COOLING_THE_WEATHER_LIMIT 6.0
#define EXTREME_WINDCHILL_LIMIT -35.0
#define MILD_WINDCHILL_LIMIT -25.0

#define SAA_LAUHTUU_PHRASE "s‰‰ lauhtuu"
#define SAA_WORD "s‰‰"
#define ON_WORD "on"
#define POIKKEUKSELLISEN_WORD "poikkeuksellisen"
#define KYLMAA_WORD "kylm‰‰"
#define KOLEAA_WORD "koleaa"
#define HYVIN_WORD "hyvin"
#define VAHAN_WORD "v‰h‰n"
#define HUOMATTAVASTI_WORD "huomattavasti"
#define HARVINAISEN_WORD "harvinaisen"
#define LAMMINTA_WORD "l‰mmint‰"
#define LEUTOA_WORD "leutoa"
#define KIREA_WORD "kire‰"
#define SAA_ON_EDELLEEN_LAUHAA_PHRASE "s‰‰ on edelleen lauhaa"
#define SAA_LAUHTUU_PHRASE "s‰‰ lauhtuu"
#define PAKKANEN_HEIKKENEE_PHRASE "pakkanen heikkenee"
#define PAKKANEN_HELLITTAA_PHRASE "pakkanen hellitt‰‰"
#define KIREA_PAKKANEN_JATKUU_PHRASE "kire‰ pakkanen jatkuu"
#define PAKKANEN_KIRISTYY_PHRASE "pakkanen kiristyy"
#define HELTEINEN_SAA_JATKUU_PHRASE "helteinen s‰‰ jatkuu"
#define VIILEA_SAA_JATKUU_PHRASE "viile‰ s‰‰ jatkuu"
#define KOLEA_SAA_JATKUU_PHRASE "kolea s‰‰ jatkuu"
#define SAA_MUUTTUU_HELTEISEKSI_PHRASE "s‰‰ muuttuu helteiseksi"
#define SAA_ON_HELTEISTA_PHRASE "s‰‰ on helteist‰"
#define SAA_LAMPENEE_PHRASE "s‰‰ l‰mpenee"
#define SAA_VIILENEE_PHRASE "s‰‰ viilenee"
#define TUULINEN_WORD "tuulinen"
#define TUULISTA_WORD "tuulista"
#define AAMUPAIVALLA_WORD "aamup‰iv‰ll‰"
#define ILTAPAIVALLA_WORD "iltap‰iv‰ll‰"
#define ERITTAIN_WORD "eritt‰in"
#define SISAMAASSA_WORD "sis‰maassa"
#define RANNIKOLLA_WORD "rannikolla"
#define PAKKANEN_ON_PUREVAA_PHRASE "pakkanen on purevaa"
#define PAKKANEN_ON_ERITTAIN_PUREVAA_PHRASE "pakkanen on eritt‰in purevaa"
#define TUULI_VIILENTAA_SAATA_PHRASE "tuuli viilent‰‰ s‰‰t‰"
#define TUULI_KYLMENTAA_SAATA_PHRASE "tuuli kylment‰‰ s‰‰t‰"
#define TANAAN_WORD "t‰n‰‰n"
#define HUOMENNA_WORD "huomenna"

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
								 const WeatherPeriod& day1Period,
								 const WeatherPeriod& nightPeriod,
								 const WeatherPeriod& day2Period,
								 const forecast_season_id& season,
								 const NFmiTime& forecastTime) 
		: theVariable(variable),
		  theLog(log),
		  theSources(sources),
		  theArea(area),
		  thePeriod(period),
		  theDay1Period(day1Period),
		  theNightPeriod(nightPeriod),
		  theDay2Period(day2Period),
		  theSeason(season),
		  theForecastTime(forecastTime),
		  theAnomalyPhrase(UNDEFINED_ANOMALY_PHRASE_ID),
		  theShortrunTrend(UNDEFINED_SHORTRUN_TREND_ID),
		  theFakeVariable(""),
		  theDay1TemperatureAreaMorningMinimum(kFloatMissing, 0),
		  theDay1TemperatureAreaMorningMean(kFloatMissing, 0),
		  theDay1TemperatureAreaMorningMaximum(kFloatMissing, 0),
		  theDay1TemperatureAreaAfternoonMinimum(kFloatMissing, 0),
		  theDay1TemperatureAreaAfternoonMean(kFloatMissing, 0),
		  theDay1TemperatureAreaAfternoonMaximum(kFloatMissing, 0),
		  theDay1TemperatureInlandMorningMinimum(kFloatMissing, 0),
		  theDay1TemperatureInlandMorningMean(kFloatMissing, 0),
		  theDay1TemperatureInlandMorningMaximum(kFloatMissing, 0),
		  theDay1TemperatureInlandAfternoonMinimum(kFloatMissing, 0),
		  theDay1TemperatureInlandAfternoonMean(kFloatMissing, 0),
		  theDay1TemperatureInlandAfternoonMaximum(kFloatMissing, 0),
		  theDay1TemperatureCoastalMorningMinimum(kFloatMissing, 0),
		  theDay1TemperatureCoastalMorningMean(kFloatMissing, 0),
		  theDay1TemperatureCoastalMorningMaximum(kFloatMissing, 0),
		  theDay1TemperatureCoastalAfternoonMinimum(kFloatMissing, 0),
		  theDay1TemperatureCoastalAfternoonMean(kFloatMissing, 0),
		  theDay1TemperatureCoastalAfternoonMaximum(kFloatMissing, 0),
		  theNightTemperatureMinimum(kFloatMissing, 0),
		  theNightTemperatureMean(kFloatMissing, 0),
		  theNightTemperatureMaximum(kFloatMissing, 0),
		  theDay2TemperatureAreaMorningMinimum(kFloatMissing, 0),
		  theDay2TemperatureAreaMorningMean(kFloatMissing, 0),
		  theDay2TemperatureAreaMorningMaximum(kFloatMissing, 0),
		  theDay2TemperatureAreaAfternoonMinimum(kFloatMissing, 0),
		  theDay2TemperatureAreaAfternoonMean(kFloatMissing, 0),
		  theDay2TemperatureAreaAfternoonMaximum(kFloatMissing, 0),
		  theDay2TemperatureInlandMorningMinimum(kFloatMissing, 0),
		  theDay2TemperatureInlandMorningMean(kFloatMissing, 0),
		  theDay2TemperatureInlandMorningMaximum(kFloatMissing, 0),
		  theDay2TemperatureInlandAfternoonMinimum(kFloatMissing, 0),
		  theDay2TemperatureInlandAfternoonMean(kFloatMissing, 0),
		  theDay2TemperatureInlandAfternoonMaximum(kFloatMissing, 0),
		  theDay2TemperatureCoastalMorningMinimum(kFloatMissing, 0),
		  theDay2TemperatureCoastalMorningMean(kFloatMissing, 0),
		  theDay2TemperatureCoastalMorningMaximum(kFloatMissing, 0),
		  theDay2TemperatureCoastalAfternoonMinimum(kFloatMissing, 0),
		  theDay2TemperatureCoastalAfternoonMean(kFloatMissing, 0),
		  theDay2TemperatureCoastalAfternoonMaximum(kFloatMissing, 0),
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
	  {}

	  const string& theVariable;
	  MessageLogger& theLog;
	  const AnalysisSources& theSources;
	  const WeatherArea& theArea;
	  const WeatherPeriod& thePeriod;
	  const WeatherPeriod& theDay1Period;
	  const WeatherPeriod& theNightPeriod;
	  const WeatherPeriod& theDay2Period;
	  const forecast_season_id& theSeason;
	  const NFmiTime& theForecastTime;
	  anomaly_phrase_id theAnomalyPhrase;
	  shortrun_trend_id theShortrunTrend;
	  string theFakeVariable;
	  bool theIsToday; // indicates whether the day in question is today or tomorrow
	  WeatherResult theDay1TemperatureAreaMorningMinimum;
	  WeatherResult theDay1TemperatureAreaMorningMean;
	  WeatherResult theDay1TemperatureAreaMorningMaximum;
	  WeatherResult theDay1TemperatureAreaAfternoonMinimum;
	  WeatherResult theDay1TemperatureAreaAfternoonMean;
	  WeatherResult theDay1TemperatureAreaAfternoonMaximum;
	  WeatherResult theDay1TemperatureInlandMorningMinimum;
	  WeatherResult theDay1TemperatureInlandMorningMean;
	  WeatherResult theDay1TemperatureInlandMorningMaximum;
	  WeatherResult theDay1TemperatureInlandAfternoonMinimum;
	  WeatherResult theDay1TemperatureInlandAfternoonMean;
	  WeatherResult theDay1TemperatureInlandAfternoonMaximum;
	  WeatherResult theDay1TemperatureCoastalMorningMinimum;
	  WeatherResult theDay1TemperatureCoastalMorningMean;
	  WeatherResult theDay1TemperatureCoastalMorningMaximum;
	  WeatherResult theDay1TemperatureCoastalAfternoonMinimum;
	  WeatherResult theDay1TemperatureCoastalAfternoonMean;
	  WeatherResult theDay1TemperatureCoastalAfternoonMaximum;
	  WeatherResult theNightTemperatureMinimum;
	  WeatherResult theNightTemperatureMean;
	  WeatherResult theNightTemperatureMaximum;
	  WeatherResult theDay2TemperatureAreaMorningMinimum;
	  WeatherResult theDay2TemperatureAreaMorningMean;
	  WeatherResult theDay2TemperatureAreaMorningMaximum;
	  WeatherResult theDay2TemperatureAreaAfternoonMinimum;
	  WeatherResult theDay2TemperatureAreaAfternoonMean;
	  WeatherResult theDay2TemperatureAreaAfternoonMaximum;
	  WeatherResult theDay2TemperatureInlandMorningMinimum;
	  WeatherResult theDay2TemperatureInlandMorningMean;
	  WeatherResult theDay2TemperatureInlandMorningMaximum;
	  WeatherResult theDay2TemperatureInlandAfternoonMinimum;
	  WeatherResult theDay2TemperatureInlandAfternoonMean;
	  WeatherResult theDay2TemperatureInlandAfternoonMaximum;
	  WeatherResult theDay2TemperatureCoastalMorningMinimum;
	  WeatherResult theDay2TemperatureCoastalMorningMean;
	  WeatherResult theDay2TemperatureCoastalMorningMaximum;
	  WeatherResult theDay2TemperatureCoastalAfternoonMinimum;
	  WeatherResult theDay2TemperatureCoastalAfternoonMean;
	  WeatherResult theDay2TemperatureCoastalAfternoonMaximum;
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
	};

	void log_data(const temperature_anomaly_params& theParameters)
	{
	  theParameters.theLog << "theDay1TemperatureAreaMorningMinimum: " 
						   << theParameters.theDay1TemperatureAreaMorningMinimum << endl;
	  theParameters.theLog << "theDay1TemperatureAreaMorningMean: " 
						   << theParameters.theDay1TemperatureAreaMorningMean << endl;
	  theParameters.theLog << "theDay1TemperatureAreaMorningMaximum: " 
						   << theParameters.theDay1TemperatureAreaMorningMaximum << endl;
	  theParameters.theLog << "theDay1TemperatureAreaAfternoonMinimum: " 
						   << theParameters.theDay1TemperatureAreaAfternoonMinimum << endl;
	  theParameters.theLog << "theDay1TemperatureAreaAfternoonMean: " 
						   << theParameters.theDay1TemperatureAreaAfternoonMean << endl;
	  theParameters.theLog << "theDay1TemperatureAreaAfternoonMaximum: " 
						   << theParameters.theDay1TemperatureAreaAfternoonMaximum << endl;

	  if(theParameters.theDay1TemperatureInlandMorningMinimum.value() != kFloatMissing)
		{
		  theParameters.theLog << "theDay1TemperatureInlandMorningMinimum: " 
							   << theParameters.theDay1TemperatureInlandMorningMinimum << endl;
		  theParameters.theLog << "theDay1TemperatureInlandMorningMean: " 
							   << theParameters.theDay1TemperatureInlandMorningMean << endl;
		  theParameters.theLog << "theDay1TemperatureInlandMorningMaximum: " 
							   << theParameters.theDay1TemperatureInlandMorningMaximum << endl;
		  theParameters.theLog << "theDay1TemperatureInlandAfternoonMinimum: " 
							   << theParameters.theDay1TemperatureInlandAfternoonMinimum << endl;
		  theParameters.theLog << "theDay1TemperatureInlandAfternoonMean: " 
							   << theParameters.theDay1TemperatureInlandAfternoonMean << endl;
		  theParameters.theLog << "theDay1TemperatureInlandAfternoonMaximum: " 
							   << theParameters.theDay1TemperatureInlandAfternoonMaximum << endl;
		}

	  if(theParameters.theDay1TemperatureCoastalMorningMinimum.value() != kFloatMissing)
		{
		  theParameters.theLog << "theDay1TemperatureCoastalMorningMinimum: " 
							   << theParameters.theDay1TemperatureCoastalMorningMinimum << endl;
		  theParameters.theLog << "theDay1TemperatureCoastalMorningMean: " 
							   << theParameters.theDay1TemperatureCoastalMorningMean << endl;
		  theParameters.theLog << "theDay1TemperatureCoastalMorningMaximum: " 
							   << theParameters.theDay1TemperatureCoastalMorningMaximum << endl;
		  theParameters.theLog << "theDay1TemperatureCoastalAfternoonMinimum: " 
							   << theParameters.theDay1TemperatureCoastalAfternoonMinimum << endl;
		  theParameters.theLog << "theDay1TemperatureCoastalAfternoonMean: " 
							   << theParameters.theDay1TemperatureCoastalAfternoonMean << endl;
		  theParameters.theLog << "theDay1TemperatureCoastalAfternoonMaximum: " 
							   << theParameters.theDay1TemperatureCoastalAfternoonMaximum << endl;
		}

	  theParameters.theLog << "theDay2TemperatureAreaMorningMinimum: " 
						   << theParameters.theDay2TemperatureAreaMorningMinimum << endl;
	  theParameters.theLog << "theDay2TemperatureAreaMorningMean: " 
						   << theParameters.theDay2TemperatureAreaMorningMean << endl;
	  theParameters.theLog << "theDay2TemperatureAreaMorningMaximum: " 
						   << theParameters.theDay2TemperatureAreaMorningMaximum << endl;
	  theParameters.theLog << "theDay2TemperatureAreaAfternoonMinimum: " 
						   << theParameters.theDay2TemperatureAreaAfternoonMinimum << endl;
	  theParameters.theLog << "theDay2TemperatureAreaAfternoonMean: " 
						   << theParameters.theDay2TemperatureAreaAfternoonMean << endl;
	  theParameters.theLog << "theDay2TemperatureAreaAfternoonMaximum: " 
						   << theParameters.theDay2TemperatureAreaAfternoonMaximum << endl;

	  if(theParameters.theDay2TemperatureInlandMorningMinimum.value() != kFloatMissing)
		{
		  theParameters.theLog << "theDay2TemperatureInlandMorningMinimum: " 
							   << theParameters.theDay2TemperatureInlandMorningMinimum << endl;
		  theParameters.theLog << "theDay2TemperatureInlandMorningMean: " 
							   << theParameters.theDay2TemperatureInlandMorningMean << endl;
		  theParameters.theLog << "theDay2TemperatureInlandMorningMaximum: " 
							   << theParameters.theDay2TemperatureInlandMorningMaximum << endl;
		  theParameters.theLog << "theDay2TemperatureInlandAfternoonMinimum: " 
							   << theParameters.theDay2TemperatureInlandAfternoonMinimum << endl;
		  theParameters.theLog << "theDay2TemperatureInlandAfternoonMean: " 
							   << theParameters.theDay2TemperatureInlandAfternoonMean << endl;
		  theParameters.theLog << "theDay2TemperatureInlandAfternoonMaximum: " 
							   << theParameters.theDay2TemperatureInlandAfternoonMaximum << endl;
		}

	  if(theParameters.theDay2TemperatureCoastalMorningMinimum.value() != kFloatMissing)
		{
		  theParameters.theLog << "theDay2TemperatureCoastalMorningMinimum: " 
							   << theParameters.theDay2TemperatureCoastalMorningMinimum << endl;
		  theParameters.theLog << "theDay2TemperatureCoastalMorningMean: " 
							   << theParameters.theDay2TemperatureCoastalMorningMean << endl;
		  theParameters.theLog << "theDay2TemperatureCoastalMorningMaximum: " 
							   << theParameters.theDay2TemperatureCoastalMorningMaximum << endl;
		  theParameters.theLog << "theDay2TemperatureCoastalAfternoonMinimum: " 
							   << theParameters.theDay2TemperatureCoastalAfternoonMinimum << endl;
		  theParameters.theLog << "theDay2TemperatureCoastalAfternoonMean: " 
							   << theParameters.theDay2TemperatureCoastalAfternoonMean << endl;
		  theParameters.theLog << "theDay2TemperatureCoastalAfternoonMaximum: " 
							   << theParameters.theDay2TemperatureCoastalAfternoonMaximum << endl;
		}

#ifdef INCLUDE_WIND_ANOMALY
	  if(theParameters.theWindspeedInlandMorningMinimum.value() != kFloatMissing)
		{
		  theParameters.theLog << "theWindspeedInlandMorningMinimum: " 
							   << theParameters.theWindspeedInlandMorningMinimum << endl;
		  theParameters.theLog << "theWindspeedInlandMorningMean: " 
							   << theParameters.theWindspeedInlandMorningMean << endl;
		  theParameters.theLog << "theWindspeedInlandMorningMaximum: " 
							   << theParameters.theWindspeedInlandMorningMaximum << endl;
		  theParameters.theLog << "theWindspeedInlandAfternoonMinimum: " 
							   << theParameters.theWindspeedInlandAfternoonMinimum << endl;
		  theParameters.theLog << "theWindspeedInlandAfternoonMean: " 
							   << theParameters.theWindspeedInlandAfternoonMean << endl;
		  theParameters.theLog << "theWindspeedInlandAfternoonMaximum: " 
							   << theParameters.theWindspeedInlandAfternoonMaximum << endl;
		}

	  if(theParameters.theWindspeedCoastalMorningMinimum.value() != kFloatMissing)
		{
		  theParameters.theLog << "theWindspeedCoastalMorningMinimum: " 
							   << theParameters.theWindspeedCoastalMorningMinimum << endl;
		  theParameters.theLog << "theWindspeedCoastalMorningMean: " 
							   << theParameters.theWindspeedCoastalMorningMean << endl;
		  theParameters.theLog << "theWindspeedCoastalMorningMaximum: " 
							   << theParameters.theWindspeedCoastalMorningMaximum << endl;
		  theParameters.theLog << "theWindspeedCoastalAfternoonMinimum: " 
							   << theParameters.theWindspeedCoastalAfternoonMinimum << endl;
		  theParameters.theLog << "theWindspeedCoastalAfternoonMean: " 
							   << theParameters.theWindspeedCoastalAfternoonMean << endl;
		  theParameters.theLog << "theWindspeedCoastalAfternoonMaximum: " 
							   << theParameters.theWindspeedCoastalAfternoonMaximum << endl;
		}

	  if(theParameters.theWindchillInlandMorningMinimum.value() != kFloatMissing)
		{
		  theParameters.theLog << "theWindchillInlandMorningMinimum: " 
							   << theParameters.theWindchillInlandMorningMinimum << endl;
		  theParameters.theLog << "theWindchillInlandMorningMean: " 
							   << theParameters.theWindchillInlandMorningMean << endl;
		  theParameters.theLog << "theWindchillInlandMorningMaximum: " 
							   << theParameters.theWindchillInlandMorningMaximum << endl;
		  theParameters.theLog << "theWindchillInlandAfternoonMinimum: " 
							   << theParameters.theWindchillInlandAfternoonMinimum << endl;
		  theParameters.theLog << "theWindchillInlandAfternoonMean: " 
							   << theParameters.theWindchillInlandAfternoonMean << endl;
		  theParameters.theLog << "theWindchillInlandAfternoonMaximum: " 
							   << theParameters.theWindchillInlandAfternoonMaximum << endl;
		}

	  if(theParameters.theWindchillCoastalMorningMinimum.value() != kFloatMissing)
		{
		  theParameters.theLog << "theWindchillCoastalMorningMinimum: " 
							   << theParameters.theWindchillCoastalMorningMinimum << endl;
		  theParameters.theLog << "theWindchillCoastalMorningMean: " 
							   << theParameters.theWindchillCoastalMorningMean << endl;
		  theParameters.theLog << "theWindchillCoastalMorningMaximum: " 
							   << theParameters.theWindchillCoastalMorningMaximum << endl;
		  theParameters.theLog << "theWindchillCoastalAfternoonMinimum: " 
							   << theParameters.theWindchillCoastalAfternoonMinimum << endl;
		  theParameters.theLog << "theWindchillCoastalAfternoonMean: " 
							   << theParameters.theWindchillCoastalAfternoonMean << endl;
		  theParameters.theLog << "theWindchillCoastalAfternoonMaximum: " 
							   << theParameters.theWindchillCoastalAfternoonMaximum << endl;
		}
#endif
	}

	void calculate_night_temperature(temperature_anomaly_params& theParameters)
	{	  
	  GridForecaster theForecaster;

	  std::string theFakeVariable(theParameters.theVariable + "::fake::temperature::night::area");

	  theParameters.theNightTemperatureMinimum = theForecaster.analyze(theFakeVariable + "::min",
																	   theParameters.theSources,
																	   Temperature,
																	   Minimum,
																	   theParameters.theSeason == SUMMER_SEASON 
																	   ? Minimum : Mean,
																	   theParameters.theArea,
																	   theParameters.theNightPeriod);

	  theParameters.theNightTemperatureMaximum = theForecaster.analyze(theFakeVariable + "::max",
																	   theParameters.theSources,
																	   Temperature,
																	   Maximum,
																	   theParameters.theSeason == SUMMER_SEASON 
																	   ? Minimum : Mean,
																	   theParameters.theArea,
																	   theParameters.theNightPeriod);

	  theParameters.theNightTemperatureMean = theForecaster.analyze(theFakeVariable + "::mean",
																	theParameters.theSources,
																	Temperature,
																	Mean,
																	theParameters.theSeason == SUMMER_SEASON 
																	? Minimum : Mean,
																	theParameters.theArea,
																	theParameters.theNightPeriod);
	}

	void calculate_windspeed_and_chill(temperature_anomaly_params& theParameters, 
									   const bool& theInlandArea, 
									   const bool& theMorningPeriod,
									   const bool& theWindspeed)
	{
	  GridForecaster theForecaster;

	  WeatherArea theArea = theParameters.theArea;
	  theArea.type(theInlandArea ? WeatherArea::Inland : WeatherArea::Coast);

	  std::string theFakeVariable("");

	  if(theWindspeed)
		{
		  theFakeVariable = (theInlandArea ? 
							 (theMorningPeriod ? theParameters.theVariable + "::fake::windspeed::morning::inland" 
							  : theParameters.theVariable + "::fake::windspeed::afternoon::inland")
							 : (theMorningPeriod ? theParameters.theVariable + "::fake::windspeed::morning::coast" 
								: theParameters.theVariable + "::fake::windspeed::afternoon::coast"));
		}
	  else
		{
		  theFakeVariable = (theInlandArea ? 
							 (theMorningPeriod ? theParameters.theVariable + "::fake::windchill::morning::inland" 
							  : theParameters.theVariable + "::fake::windchill::afternoon::inland")
							 : (theMorningPeriod ? theParameters.theVariable + "::fake::windchill::morning::coast" 
								: theParameters.theVariable + "::fake::windchill::afternoon::coast"));
		}


	  WeatherPeriod theCompletePeriod(theParameters.theDay2Period);
								  
	  NFmiTime noonTime(theCompletePeriod.localStartTime().GetYear(),
						theCompletePeriod.localStartTime().GetMonth(),
						theCompletePeriod.localStartTime().GetDay(), 12, 0, 0);


	  NFmiTime theStartTime(theMorningPeriod ? theCompletePeriod.localStartTime() : noonTime);
	  NFmiTime theEndTime(theMorningPeriod ? noonTime : theCompletePeriod.localEndTime());

	  WeatherPeriod thePeriod(theStartTime, theEndTime);
	  WeatherResult* theMinimum = 0;
	  WeatherResult* theMean = 0;
	  WeatherResult* theMaximum = 0;

	  if(theInlandArea)
		{
		  if(theMorningPeriod)
			{
			  theMinimum = &(theWindspeed ? theParameters.theWindspeedInlandMorningMinimum : 
							 theParameters.theWindchillInlandMorningMinimum);
			  theMean = &(theWindspeed ? theParameters.theWindspeedInlandMorningMean : 
						  theParameters.theWindchillInlandMorningMean);
			  theMaximum = &(theWindspeed ? theParameters.theWindspeedInlandMorningMaximum : 
							 theParameters.theWindchillInlandMorningMaximum);

			}
		  else
			{
			  theMinimum = &(theWindspeed ? theParameters.theWindspeedInlandAfternoonMinimum : 
							 theParameters.theWindchillInlandAfternoonMinimum);
			  theMean = &(theWindspeed ? theParameters.theWindspeedInlandAfternoonMean : 
						  theParameters.theWindchillInlandAfternoonMean);
			  theMaximum = &(theWindspeed ? theParameters.theWindspeedInlandAfternoonMaximum : 
							 theParameters.theWindchillInlandAfternoonMaximum);
			}
		}
	  else
		{
		  if(theMorningPeriod)
			{
			  theMinimum = &(theWindspeed ? theParameters.theWindspeedCoastalMorningMinimum : 
							 theParameters.theWindchillCoastalMorningMinimum);
			  theMean = &(theWindspeed ? theParameters.theWindspeedCoastalMorningMean : 
						  theParameters.theWindchillCoastalMorningMean);
			  theMaximum = &(theWindspeed ? theParameters.theWindspeedCoastalMorningMaximum : 
							 theParameters.theWindchillCoastalMorningMaximum);

			}
		  else
			{
			  theMinimum = &(theWindspeed ? theParameters.theWindspeedCoastalAfternoonMinimum : 
							 theParameters.theWindchillCoastalAfternoonMinimum);
			  theMean = &(theWindspeed ? theParameters.theWindspeedCoastalAfternoonMean : 
						  theParameters.theWindchillCoastalAfternoonMean);
			  theMaximum = &(theWindspeed ? theParameters.theWindspeedCoastalAfternoonMaximum : 
							 theParameters.theWindchillCoastalAfternoonMaximum);
			}
		}

	  *theMinimum = theForecaster.analyze(theFakeVariable + "::min",
										  theParameters.theSources,
										  theWindspeed ? WindSpeed : WindChill,
										  Minimum,
										  Maximum,
										  theArea,
										  thePeriod);

	  *theMaximum = theForecaster.analyze(theFakeVariable + "::max",
										  theParameters.theSources,
										  theWindspeed ? WindSpeed : WindChill,
										  Maximum,
										  Maximum,
										  theArea,
										  thePeriod);

	  *theMean = theForecaster.analyze(theFakeVariable + "::mean",
									   theParameters.theSources,
									   theWindspeed ? WindSpeed : WindChill,
									   Mean,
									   Maximum,
									   theArea,
									   thePeriod);
	}



	const void log_start_time_and_end_time(MessageLogger& theLog, 
										   const std::string& theLogMessage, 
										   const WeatherPeriod& thePeriod)
	{
	  theLog << NFmiStringTools::Convert(theLogMessage)
			 << thePeriod.localStartTime()
			 << " ... "
			 << thePeriod.localEndTime()
			 << endl;
	}

	const Sentence temperature_anomaly_sentence(temperature_anomaly_params& theParameters, 
												const fractile_id& theFractile)
	{
	  Sentence retval;

	  std::string theSpecifiedDay(theParameters.theIsToday ? TANAAN_WORD : HUOMENNA_WORD);


	  switch(theFractile)
		{
		case FRACTILE_02:
		  {
			retval << SAA_WORD << ON_WORD << theSpecifiedDay << POIKKEUKSELLISEN_WORD;
			retval << (theParameters.theSeason == SUMMER_SEASON ? KOLEAA_WORD : KYLMAA_WORD);
			theParameters.theAnomalyPhrase = 
			  (theParameters.theSeason == SUMMER_SEASON ? SAA_ON_POIKKEUKSELLISEN_KOLEAA :SAA_ON_POIKKEUKSELLISEN_KYLMAA);
		  }
		  break;
		case FRACTILE_12:
		  {
			retval << SAA_WORD << ON_WORD << theSpecifiedDay;
			if(theParameters.theSeason == SUMMER_SEASON)
			  retval << KOLEAA_WORD;
			else
			  retval << HYVIN_WORD << KYLMAA_WORD;
			theParameters.theAnomalyPhrase = 
			  (theParameters.theSeason == SUMMER_SEASON ? SAA_ON_KOLEAA : SAA_ON_HYVIN_KYLMAA);
		  }
		  break;
		case FRACTILE_37:
		  break;
		case FRACTILE_50:
		  break;
		case FRACTILE_63:
		  break;
		case FRACTILE_88:
		  break;
		case FRACTILE_98:
		  {
			retval << SAA_WORD << ON_WORD << theSpecifiedDay;
			if(theParameters.theSeason == SUMMER_SEASON)
			  retval << HARVINAISEN_WORD << LAMMINTA_WORD;
			else
			  retval << HYVIN_WORD << LEUTOA_WORD;
			theParameters.theAnomalyPhrase = 
			  (theParameters.theSeason == SUMMER_SEASON ? SAA_ON_HARVINAISEN_LAMMINTA : SAA_ON_HYVIN_LEUTOA);
		  }
		  break;
		case FRACTILE_100:
		  {
			retval << SAA_WORD << ON_WORD << theSpecifiedDay << POIKKEUKSELLISEN_WORD;
			if(theParameters.theSeason == SUMMER_SEASON)
			  retval << LAMMINTA_WORD;
			else
			  retval << LEUTOA_WORD;
			theParameters.theAnomalyPhrase = 
			  (theParameters.theSeason == SUMMER_SEASON ? SAA_ON_POIKKEUKSLLISEN_LAMMINTA : SAA_ON_POIKKEUKSLLISEN_LEUTOA);
		  }
		  break;
		case FRACTILE_UNDEFINED:
		  break;		}

	  return retval;
	}

	const Sentence temperature_shortruntrend_sentence(temperature_anomaly_params& theParameters)
	{
	  Sentence sentence;

	  double period1Temperature = theParameters.theDay1TemperatureAreaAfternoonMean.value();
	  double period2Temperature = theParameters.theDay2TemperatureAreaAfternoonMean.value();

	  // kova pakkanen: F12,5 fractile on 1. Feb 12:00
	  NFmiTime veryColdRefTime(theParameters.theForecastTime.GetYear(), 2, 1, 12, 0, 0);
	  WeatherPeriod veryColdWeatherPeriod(veryColdRefTime, veryColdRefTime);

	  WeatherResult wr = get_fractile_temperature(theParameters.theVariable,
												  FRACTILE_12,
												  theParameters.theSources,
												  theParameters.theArea,
												  veryColdWeatherPeriod);

	  float veryColdTemperature = wr.value() < VERY_COLD_TEMPERATURE_UPPER_LIMIT ? 
		wr.value() : VERY_COLD_TEMPERATURE_UPPER_LIMIT;

	  WeatherPeriod forecastTimeWeatherPeriod(NFmiTime(theParameters.theForecastTime.GetYear(),
													   theParameters.theForecastTime.GetMonth(),
													   theParameters.theForecastTime.GetDay(),
													   6, 0, 0),
											  NFmiTime(theParameters.theForecastTime.GetYear(),
													   theParameters.theForecastTime.GetMonth(),
													   theParameters.theForecastTime.GetDay(),
													   18, 0, 0));

	  WeatherResult fractile37Temperature = get_fractile_temperature(theParameters.theVariable,
																	 FRACTILE_37,
																	 theParameters.theSources,
																	 theParameters.theArea,
																	 forecastTimeWeatherPeriod);

	  WeatherResult fractile12Temperature = get_fractile_temperature(theParameters.theVariable,
																	 FRACTILE_12,
																	 theParameters.theSources,
																	 theParameters.theArea,
																	 forecastTimeWeatherPeriod);


	  std::string theSpecifiedDay(theParameters.theIsToday ? TANAAN_WORD : HUOMENNA_WORD);
	
	  if(theParameters.theSeason == WINTER_SEASON)
		{
		  // s‰‰ on edelleen lauhaa
		  // s‰‰ lauhtuu
		  // kire‰ pakkanen heikkenee
		  // kire‰ pakkanen hellitt‰‰*
		  // pakkanen heikkenee
		  // pakkanen hellitt‰‰*
		  // kire‰ pakkanen jatkuu
		  // pakkanen kiristyy

		  float temperatureDifference = abs(period2Temperature - period1Temperature);
		  if(period2Temperature >= period1Temperature)
			{
			  if(period1Temperature > WEAK_FROST_TEMPERATURE_LIMIT && period1Temperature < LOW_PLUS_TEMPARATURE &&
				 period2Temperature > WEAK_FROST_TEMPERATURE_LIMIT && period2Temperature < LOW_PLUS_TEMPARATURE)
				{
				  sentence << SAA_ON_EDELLEEN_LAUHAA_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = SAA_ON_EDELLEEN_LAUHAA;
				}
			  else if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT 
					  && period1Temperature < WEAK_FROST_TEMPERATURE_LIMIT && period2Temperature >= WEAK_FROST_TEMPERATURE_LIMIT &&
					  period2Temperature < LOW_PLUS_TEMPARATURE)
				{
				  sentence << SAA_LAUHTUU_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = SAA_LAUHTUU;
				}
			  else if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT
					  && period1Temperature <= veryColdTemperature &&
					  period2Temperature <= WEAK_FROST_TEMPERATURE_LIMIT)
				{
				  sentence << KIREA_WORD << PAKKANEN_HEIKKENEE_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = KIREA_PAKKANEN_HEIKKENEE;
				}
			  else if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT
					  && period1Temperature <= veryColdTemperature &&
					  period2Temperature < ZERO_DEGREES && period2Temperature >= WEAK_FROST_TEMPERATURE_LIMIT)
				{
				  // reduntant: this will never happen, because "s‰‰ lauhtuu" is tested before
				  sentence << KIREA_WORD << PAKKANEN_HELLITTAA_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = KIREA_PAKKANEN_HELLITTAA;
				}
			  else if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT
					  && period1Temperature > veryColdTemperature &&
					  period2Temperature < WEAK_FROST_TEMPERATURE_LIMIT)
				{
				  sentence << PAKKANEN_HEIKKENEE_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = PAKKANEN_HEIKKENEE;
				}
			  else if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT
					  && period1Temperature > veryColdTemperature && period1Temperature < WEAK_FROST_TEMPERATURE_LIMIT && 
					  period2Temperature < ZERO_DEGREES && period2Temperature >= WEAK_FROST_TEMPERATURE_LIMIT)
				{
				  // redundant: this will never happen, because "s‰‰ lauhtuu" is tested before
				  sentence << PAKKANEN_HELLITTAA_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = PAKKANEN_HELLITTAA;
				}
			  else if(period1Temperature < veryColdTemperature &&
					  period2Temperature < veryColdTemperature)
				{
				  sentence << KIREA_PAKKANEN_JATKUU_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = KIREA_PAKKANEN_JATKUU;
				}

			}
		  else if(period2Temperature <= period1Temperature)
			{
			  if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT
				 && period1Temperature < WEAK_FROST_TEMPERATURE_LIMIT &&
				 period2Temperature <= veryColdTemperature)
				{
				  sentence << PAKKANEN_KIRISTYY_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = PAKKANEN_KIRISTYY;
				}
			  else if(period1Temperature < veryColdTemperature &&
					  period2Temperature < veryColdTemperature)
				{
				  sentence << KIREA_PAKKANEN_JATKUU_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = KIREA_PAKKANEN_JATKUU;
				}
			}
		}
	  else
		{
		  // helleraja
		  float hot_weather_limit = Settings::optional_double(theParameters.theVariable + 
															  "::hot_weather_limit", HOT_WEATHER_LIMIT);

		  // helteinen s‰‰ jatkuu
		  // viile‰ s‰‰ jatkuu
		  // kolea s‰‰ jatkuu
		  // s‰‰ muuttuu helteiseksi
		  // s‰‰ on helteist‰
		  // s‰‰ l‰mpenee v‰h‰n
		  // s‰‰ l‰mpenee
		  // s‰‰ l‰mpenee huomattavasti
		  // s‰‰ viilenee v‰h‰n
		  // s‰‰ viilenee
		  // s‰‰ viilenee huomattavasti

		  float temperatureDifference = abs(period2Temperature - period1Temperature);
		  bool smallChange =  temperatureDifference >= SMALL_CHANGE_LOWER_LIMIT && 
			temperatureDifference < SMALL_CHANGE_UPPER_LIMIT;
		  bool moderateChange =  temperatureDifference >= MODERATE_CHANGE_LOWER_LIMIT && 
			temperatureDifference < MODERATE_CHANGE_UPPER_LIMIT;
		  bool signifigantChange =  temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT;

		  if(period2Temperature >= period1Temperature)
			{
			  if(period1Temperature >= hot_weather_limit && period2Temperature >= hot_weather_limit)
				{
				  sentence << HELTEINEN_SAA_JATKUU_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = HELTEINEN_SAA_JATKUU;
				}
			  else if(period1Temperature < fractile12Temperature.value() && 
					  period2Temperature < fractile12Temperature.value())
				{
				  sentence << KOLEA_SAA_JATKUU_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = KOLEA_SAA_JATKUU;
				}
			  else if(period1Temperature < fractile37Temperature.value() && 
					  period2Temperature < fractile37Temperature.value())
				{
				  sentence << VIILEA_SAA_JATKUU_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = VIILEA_SAA_JATKUU;
				}
			  else if(period1Temperature < hot_weather_limit && period2Temperature >= hot_weather_limit)
				{
				  if(temperatureDifference >= NOTABLE_TEMPERATURE_CHANGE_LIMIT)
					{
					  sentence << SAA_MUUTTUU_HELTEISEKSI_PHRASE << theSpecifiedDay;
					  theParameters.theShortrunTrend = SAA_MUUTTUU_HELTEISEKSI;
					}
				  else
					{
					  sentence << SAA_ON_HELTEISTA_PHRASE << theSpecifiedDay;
					  theParameters.theShortrunTrend = SAA_ON_HELTEISTA;
					}
				}
			  else if(signifigantChange)
				{
				  sentence << SAA_LAMPENEE_PHRASE << theSpecifiedDay << HUOMATTAVASTI_WORD;
				  theParameters.theShortrunTrend = SAA_LAMPENEE_HUOMATTAVASTI;
				}
			  else if(moderateChange)
				{
				  sentence << SAA_LAMPENEE_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = SAA_LAMPENEE;
				}
			  else if(smallChange)
				{
				  sentence << SAA_LAMPENEE_PHRASE << theSpecifiedDay << VAHAN_WORD;
				  theParameters.theShortrunTrend = SAA_LAMPENEE_VAHAN;
				}
			}
		  else if(period2Temperature <= period1Temperature && period2Temperature > ZERO_DEGREES)
			{
			  if(period1Temperature >= hot_weather_limit && period2Temperature >= hot_weather_limit)
				{
				  sentence << HELTEINEN_SAA_JATKUU_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = HELTEINEN_SAA_JATKUU;
				}
			  else if(period1Temperature < fractile12Temperature.value() && 
					  period2Temperature < fractile12Temperature.value())
				{
				  sentence << KOLEA_SAA_JATKUU_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = KOLEA_SAA_JATKUU;
				}
			  else if(period1Temperature < fractile37Temperature.value() && 
					  period2Temperature < fractile37Temperature.value())
				{
				  sentence << VIILEA_SAA_JATKUU_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = VIILEA_SAA_JATKUU;
				}
			  else if(signifigantChange)
				{
				  sentence << SAA_VIILENEE_PHRASE << theSpecifiedDay << HUOMATTAVASTI_WORD;
				  theParameters.theShortrunTrend = SAA_VIILENEE_HUOMATTAVASTI;
				}
			  else if(moderateChange)
				{
				  sentence << SAA_VIILENEE_PHRASE << theSpecifiedDay;
				  theParameters.theShortrunTrend = SAA_VIILENEE;
				}
			  else if(smallChange && period2Temperature < GETTING_COOLER_NOTIFICATION_LIMIT)
				{
				  sentence << SAA_VIILENEE_PHRASE << theSpecifiedDay << VAHAN_WORD;
				  theParameters.theShortrunTrend = SAA_VIILENEE_VAHAN;
				}
			}
		}

	return sentence;
	}

	const Sentence test_windspeed(const float& theWindSpeed1, const float& theWindSpeed2, const char* theReturnString)
	{
	  Sentence retval;

	  if(abs(theWindSpeed1 - theWindSpeed2) > 1.0)
		retval << theReturnString;

	  return retval;
	}

	const Sentence windiness_sentence(const temperature_anomaly_params& theParameters)
	{
	  Sentence sentence;

	  float windy_weather_limit = Settings::optional_double(theParameters.theVariable + "::windy_weather_limit", WINDY_WEATER_LIMIT);
	  float extremely_windy_weather_limit = Settings::optional_double(theParameters.theVariable + "::extremely_windy_weather_limit", EXTREMELY_WINDY_WEATHER_LIMIT);
	  float wind_cooling_the_weather_limit = Settings::optional_double(theParameters.theVariable + "::wind_cooling_the_weather_limit", WIND_COOLING_THE_WEATHER_LIMIT);

	  bool inlandIncluded = theParameters.theWindspeedInlandMorningMinimum.value() != kFloatMissing;
	  bool coastIncluded = theParameters.theWindspeedCoastalMorningMinimum.value() != kFloatMissing;

	  bool windyMorningInland = 
		theParameters.theWindspeedInlandMorningMean.value() != kFloatMissing &&
		(static_cast<int>(theParameters.theWindspeedInlandMorningMean.value()) >= windy_weather_limit && 
		 static_cast<int>(theParameters.theWindspeedInlandMorningMean.value()) < extremely_windy_weather_limit);
	  bool windyAfternoonInland = 
		theParameters.theWindspeedInlandAfternoonMean.value() != kFloatMissing &&
		(static_cast<int>(theParameters.theWindspeedInlandAfternoonMean.value()) >= windy_weather_limit && 
		 static_cast<int>(theParameters.theWindspeedInlandAfternoonMean.value()) < extremely_windy_weather_limit);
	  bool windyMorningCoastal = 
		theParameters.theWindspeedCoastalMorningMean.value() != kFloatMissing &&
		(static_cast<int>(theParameters.theWindspeedCoastalMorningMean.value()) >= windy_weather_limit && 
		 static_cast<int>(theParameters.theWindspeedCoastalMorningMean.value()) < extremely_windy_weather_limit);
	  bool windyAfternoonCoastal = 
		theParameters.theWindspeedCoastalAfternoonMean.value() != kFloatMissing &&
		(static_cast<int>(theParameters.theWindspeedCoastalAfternoonMean.value()) >= windy_weather_limit && 
		 static_cast<int>(theParameters.theWindspeedCoastalAfternoonMean.value()) < extremely_windy_weather_limit);
	  bool extremelyWindyMorningInland = 
		theParameters.theWindspeedInlandMorningMean.value() != kFloatMissing &&
		static_cast<int>(theParameters.theWindspeedInlandMorningMean.value()) >= extremely_windy_weather_limit;
	  bool extremelyWindyAfternoonInland = 
		theParameters.theWindspeedInlandAfternoonMean.value() != kFloatMissing &&
		static_cast<int>(theParameters.theWindspeedInlandAfternoonMean.value()) >= extremely_windy_weather_limit;
	  bool extremelyWindyMorningCoastal = 
		theParameters.theWindspeedCoastalMorningMean.value() != kFloatMissing &&
		static_cast<int>(theParameters.theWindspeedCoastalMorningMean.value()) > extremely_windy_weather_limit;
	  bool extremelyWindyAfternoonCoastal = 
		theParameters.theWindspeedCoastalAfternoonMean.value() != kFloatMissing &&
		static_cast<int>(theParameters.theWindspeedCoastalAfternoonMean.value()) > extremely_windy_weather_limit;
	  float windspeedMorningInland = theParameters.theWindspeedInlandMorningMean.value();
	  float windspeedAfternoonInland = theParameters.theWindspeedInlandAfternoonMean.value();
	  float windspeedMorningCoastal = theParameters.theWindspeedCoastalMorningMean.value();
	  float windspeedAfternoonCoastal = theParameters.theWindspeedCoastalAfternoonMean.value();

	  std::string part_of_the_day("");
	  std::string areaString("");
	  std::string theSpecifiedDay(theParameters.theIsToday ? TANAAN_WORD : HUOMENNA_WORD);
	
	  Sentence varying_part;
	  if(inlandIncluded && coastIncluded)
		{
		  if(windyMorningInland && windyMorningCoastal && windyAfternoonInland && windyAfternoonCoastal)
			{
			  sentence << SAA_WORD << ON_WORD << theSpecifiedDay << TUULINEN_WORD;
			}
		  else if(windyMorningInland && !windyMorningCoastal && !windyAfternoonInland && !windyAfternoonCoastal)
			{
			  if(!extremelyWindyMorningCoastal && !extremelyWindyAfternoonInland && !extremelyWindyAfternoonCoastal)
				{
				  varying_part << test_windspeed(windspeedMorningInland, windspeedMorningCoastal, SISAMAASSA_WORD);

				  if(varying_part.empty())
					sentence << theSpecifiedDay << AAMUPAIVALLA_WORD << ON_WORD << TUULISTA_WORD;
				  else
					sentence << varying_part << ON_WORD << theSpecifiedDay << AAMUPAIVALLA_WORD << TUULISTA_WORD;
				}
			}
		  else if(!windyMorningInland && windyMorningCoastal && !windyAfternoonInland && !windyAfternoonCoastal)
			{
			  if(!extremelyWindyMorningInland && !extremelyWindyAfternoonInland && !extremelyWindyAfternoonCoastal)
				{
				  varying_part << test_windspeed(windspeedMorningCoastal, windspeedMorningInland, RANNIKOLLA_WORD);

				  if(varying_part.empty())
					sentence << theSpecifiedDay << AAMUPAIVALLA_WORD << ON_WORD << TUULISTA_WORD;
				  else
					sentence << varying_part << ON_WORD << theSpecifiedDay << AAMUPAIVALLA_WORD << TUULISTA_WORD;
				}
			}
		  else if(!windyMorningInland && !windyMorningCoastal && windyAfternoonInland && !windyAfternoonCoastal)
			{
			  if(!extremelyWindyMorningInland && !extremelyWindyMorningCoastal && !extremelyWindyAfternoonCoastal)
				{
				  varying_part << test_windspeed(windspeedMorningInland, windspeedMorningCoastal, SISAMAASSA_WORD);

				  if(varying_part.empty())
					sentence << theSpecifiedDay << ILTAPAIVALLA_WORD << ON_WORD << TUULISTA_WORD;
				  else
					sentence << varying_part << ON_WORD << theSpecifiedDay << ILTAPAIVALLA_WORD << TUULISTA_WORD;
				}
			}
		  else if(!windyMorningInland && !windyMorningCoastal && !windyAfternoonInland && windyAfternoonCoastal)
			{
			  if(!extremelyWindyMorningInland && !extremelyWindyMorningCoastal && !extremelyWindyAfternoonInland)
				{
				  varying_part << test_windspeed(windspeedAfternoonCoastal, windspeedAfternoonInland, RANNIKOLLA_WORD);

				  if(varying_part.empty())
					sentence << theSpecifiedDay << ILTAPAIVALLA_WORD << ON_WORD << TUULISTA_WORD;
				  else
					sentence << varying_part << ON_WORD << theSpecifiedDay << ILTAPAIVALLA_WORD << TUULISTA_WORD;
				}
			}
		  else if(windyMorningInland && !windyMorningCoastal && windyAfternoonInland && !windyAfternoonCoastal)
			{
			  if(!extremelyWindyMorningCoastal && !extremelyWindyAfternoonCoastal)
				{
				  varying_part << test_windspeed(windspeedMorningInland, windspeedMorningCoastal, SISAMAASSA_WORD);

				  if(varying_part.empty())
					sentence << SAA_WORD << ON_WORD << theSpecifiedDay << TUULINEN_WORD;
				  else
					sentence << varying_part << ON_WORD << theSpecifiedDay << TUULISTA_WORD;
				}
			}
		  else if(!windyMorningInland && windyMorningCoastal && windyAfternoonInland && windyAfternoonCoastal)
			{
			  if(!extremelyWindyMorningInland)
				sentence << SAA_WORD << ON_WORD << theSpecifiedDay << TUULINEN_WORD;
			}
		  else if(windyMorningInland && !windyMorningCoastal && windyAfternoonInland && windyAfternoonCoastal)
			{
			  if(!extremelyWindyMorningCoastal)
				sentence << SAA_WORD << ON_WORD << theSpecifiedDay << TUULINEN_WORD;
			}
		  else if(windyMorningInland && windyMorningCoastal && !windyAfternoonInland && windyAfternoonCoastal)
			{
			  if(!extremelyWindyAfternoonInland)
				sentence << SAA_WORD << ON_WORD << theSpecifiedDay << TUULINEN_WORD;
			}
		  else if(windyMorningInland && windyMorningCoastal && windyAfternoonInland && !windyAfternoonCoastal)
			{
			  if(!extremelyWindyAfternoonCoastal)
				sentence << SAA_WORD << ON_WORD << theSpecifiedDay << TUULINEN_WORD;
			}
		  else if(!windyMorningInland && windyMorningCoastal && !windyAfternoonInland && windyAfternoonCoastal)
			{
			  if(!extremelyWindyMorningInland && !extremelyWindyAfternoonInland)
				{
				  varying_part << test_windspeed(windspeedMorningCoastal, windspeedMorningInland, RANNIKOLLA_WORD); 

				  if(varying_part.empty())
					sentence << SAA_WORD << ON_WORD << theSpecifiedDay << TUULINEN_WORD;
				  else
					sentence << varying_part << ON_WORD << theSpecifiedDay << TUULISTA_WORD;
				}
			}
		  else if(extremelyWindyMorningInland && !extremelyWindyMorningCoastal && 
				  !extremelyWindyAfternoonInland && !extremelyWindyAfternoonCoastal)
			{
			  varying_part << test_windspeed(windspeedMorningInland, windspeedMorningCoastal, SISAMAASSA_WORD);

			  if(varying_part.empty())
				sentence << theSpecifiedDay << AAMUPAIVALLA_WORD << ON_WORD << HYVIN_WORD << TUULISTA_WORD;
			  else
				sentence << varying_part << ON_WORD << theSpecifiedDay << AAMUPAIVALLA_WORD << HYVIN_WORD << TUULISTA_WORD;
			}
		  else if(!extremelyWindyMorningInland && extremelyWindyMorningCoastal && 
				  !extremelyWindyAfternoonInland && !extremelyWindyAfternoonCoastal)
			{
			  varying_part << test_windspeed(windspeedMorningCoastal, windspeedMorningInland, RANNIKOLLA_WORD);

			  if(varying_part.empty())
				sentence << theSpecifiedDay << AAMUPAIVALLA_WORD << ON_WORD << HYVIN_WORD << TUULISTA_WORD;
			  else
				sentence << varying_part << ON_WORD << theSpecifiedDay << AAMUPAIVALLA_WORD << HYVIN_WORD << TUULISTA_WORD;
			}
		  else if(!extremelyWindyMorningInland && !extremelyWindyMorningCoastal && 
				  extremelyWindyAfternoonInland && !extremelyWindyAfternoonCoastal)
			{
			  varying_part << test_windspeed(windspeedAfternoonInland, windspeedAfternoonCoastal, SISAMAASSA_WORD);

			  if(varying_part.empty())
				sentence << theSpecifiedDay << ILTAPAIVALLA_WORD << ON_WORD << HYVIN_WORD << TUULISTA_WORD;
			  else
				sentence << varying_part << ON_WORD << theSpecifiedDay << ILTAPAIVALLA_WORD << HYVIN_WORD << TUULISTA_WORD;
			}
		  else if(!extremelyWindyMorningInland && !extremelyWindyMorningCoastal && 
				  !extremelyWindyAfternoonInland && extremelyWindyAfternoonCoastal)
			{
			  varying_part << test_windspeed(windspeedAfternoonCoastal, windspeedAfternoonInland, RANNIKOLLA_WORD);

			  if(varying_part.empty())
				sentence << theSpecifiedDay << ILTAPAIVALLA_WORD << ON_WORD << HYVIN_WORD << TUULISTA_WORD;
			  else
				sentence << varying_part << ON_WORD << theSpecifiedDay << ILTAPAIVALLA_WORD << HYVIN_WORD << TUULISTA_WORD;
			}
		  else if(!extremelyWindyMorningInland && extremelyWindyMorningCoastal && 
				  !extremelyWindyAfternoonInland && extremelyWindyAfternoonCoastal)
			{
			  varying_part << test_windspeed(windspeedMorningCoastal, windspeedMorningInland, RANNIKOLLA_WORD); 

			  if(varying_part.empty())
				sentence << SAA_WORD << ON_WORD << theSpecifiedDay << HYVIN_WORD << TUULINEN_WORD;
			  else
				sentence << varying_part << ON_WORD << theSpecifiedDay << HYVIN_WORD << TUULISTA_WORD;
			}
		  else if(extremelyWindyMorningInland && !extremelyWindyMorningCoastal && 
				  extremelyWindyAfternoonInland && !extremelyWindyAfternoonCoastal)
			{
			  varying_part <<  test_windspeed(windspeedMorningInland, windspeedMorningCoastal, SISAMAASSA_WORD);

			  if(varying_part.empty())
				sentence << SAA_WORD << ON_WORD << theSpecifiedDay << HYVIN_WORD << TUULINEN_WORD;
			  else
				sentence << varying_part << ON_WORD << theSpecifiedDay << HYVIN_WORD << TUULISTA_WORD;

			}
		  else if(extremelyWindyMorningInland && extremelyWindyMorningCoastal && 
				  extremelyWindyAfternoonInland && !extremelyWindyAfternoonCoastal)
			{
			  sentence << SAA_WORD << ON_WORD << theSpecifiedDay << HYVIN_WORD << TUULINEN_WORD;
			}
		  else if(extremelyWindyMorningInland && !extremelyWindyMorningCoastal && 
				  extremelyWindyAfternoonInland && extremelyWindyAfternoonCoastal)
			{
			  sentence << SAA_WORD << ON_WORD << theSpecifiedDay << HYVIN_WORD << TUULINEN_WORD;
			}
		  else if(!extremelyWindyMorningInland && extremelyWindyMorningCoastal && 
				  extremelyWindyAfternoonInland && extremelyWindyAfternoonCoastal)
			{
			  sentence << SAA_WORD << ON_WORD << theSpecifiedDay << HYVIN_WORD << TUULINEN_WORD;
			}
		  else if(extremelyWindyMorningInland && extremelyWindyMorningCoastal && 
				  !extremelyWindyAfternoonInland && extremelyWindyAfternoonCoastal)
			{
			  sentence << SAA_WORD << ON_WORD << theSpecifiedDay << HYVIN_WORD << TUULINEN_WORD;
			}
		  else if(extremelyWindyMorningInland && !extremelyWindyMorningCoastal && 
				  !extremelyWindyAfternoonInland && extremelyWindyAfternoonCoastal)
			{
			  sentence << SAA_WORD << ON_WORD << theSpecifiedDay << HYVIN_WORD << TUULINEN_WORD;
			}
		  else if(!extremelyWindyMorningInland && extremelyWindyMorningCoastal && 
				  extremelyWindyAfternoonInland && !extremelyWindyAfternoonCoastal)
			{
			  sentence << SAA_WORD << ON_WORD << theSpecifiedDay << HYVIN_WORD << TUULINEN_WORD;
			}
		  else if(extremelyWindyMorningInland && extremelyWindyMorningCoastal && 
				  extremelyWindyAfternoonInland && extremelyWindyAfternoonCoastal)
			{
			  sentence << SAA_WORD << ON_WORD << theSpecifiedDay << HYVIN_WORD << TUULINEN_WORD;
			}
		}
	  else if(inlandIncluded)
		{
		  if(windyMorningInland && !windyAfternoonInland)
			{
			  if(!extremelyWindyAfternoonInland)
				sentence << theSpecifiedDay  << AAMUPAIVALLA_WORD << ON_WORD << TUULISTA_WORD;
			}
		  else if(!windyMorningInland && windyAfternoonInland)
			{
			  if(!extremelyWindyMorningInland)
				sentence << theSpecifiedDay  << ILTAPAIVALLA_WORD << ON_WORD << TUULISTA_WORD;
			}
		  else if(windyMorningInland && windyAfternoonInland)
			{
			  sentence << SAA_WORD << ON_WORD << theSpecifiedDay << TUULINEN_WORD;
			}
		  else if(extremelyWindyMorningInland && !extremelyWindyAfternoonInland)
			{
			  sentence << theSpecifiedDay << AAMUPAIVALLA_WORD << ON_WORD << HYVIN_WORD << TUULISTA_WORD;
			}
		  else if(!extremelyWindyMorningInland && extremelyWindyAfternoonInland)
			{
			  sentence << theSpecifiedDay << ILTAPAIVALLA_WORD << ON_WORD << HYVIN_WORD << TUULISTA_WORD;
			}
		  else if(extremelyWindyMorningInland && extremelyWindyAfternoonInland)
			{
			  sentence << SAA_WORD << ON_WORD  << theSpecifiedDay << HYVIN_WORD << TUULINEN_WORD;
			}
		}
	  else if(coastIncluded)
		{
		  if(windyMorningCoastal && !windyAfternoonCoastal)
			{
			  if(!extremelyWindyAfternoonCoastal)
				sentence << theSpecifiedDay << AAMUPAIVALLA_WORD << ON_WORD << TUULISTA_WORD;
			}
		  else if(!windyMorningCoastal && windyAfternoonCoastal)
			{
			  if(!extremelyWindyMorningCoastal)
				sentence << theSpecifiedDay << ILTAPAIVALLA_WORD << ON_WORD << TUULISTA_WORD;
			}
		  else if(windyMorningCoastal && windyAfternoonCoastal)
			{
			  sentence << SAA_WORD << ON_WORD << theSpecifiedDay << TUULINEN_WORD;
			}
		  else if(extremelyWindyMorningCoastal && !extremelyWindyAfternoonCoastal)
			{
			  sentence << theSpecifiedDay << AAMUPAIVALLA_WORD << ON_WORD << HYVIN_WORD << TUULISTA_WORD;
			}
		  else if(!extremelyWindyMorningCoastal && extremelyWindyAfternoonCoastal)
			{
			  sentence << theSpecifiedDay << ILTAPAIVALLA_WORD << ON_WORD << HYVIN_WORD << TUULISTA_WORD;
			}
		  else if(extremelyWindyMorningCoastal && extremelyWindyAfternoonCoastal)
			{
			  sentence << SAA_WORD << ON_WORD << theSpecifiedDay << HYVIN_WORD << TUULINEN_WORD;
			}
		}

	  // handle the wind cooling effect
	  if(sentence.empty() && 
		 theParameters.theAnomalyPhrase == UNDEFINED_ANOMALY_PHRASE_ID &&
		 theParameters.theShortrunTrend != KOLEA_SAA_JATKUU && 
		 theParameters.theShortrunTrend != VIILEA_SAA_JATKUU &&
		 theParameters.theShortrunTrend != SAA_VIILENEE_HUOMATTAVASTI &&
		 theParameters.theShortrunTrend != SAA_VIILENEE &&
		 theParameters.theShortrunTrend != SAA_VIILENEE_VAHAN)
		{
		  areaString = "";
		  float temperature = -1.0;

		  bool windCoolingTheWeatherInlandMorning = 
			(theParameters.theWindspeedInlandMorningMean.value() != kFloatMissing &&
			 static_cast<int>(theParameters.theWindspeedInlandMorningMean.value()) >= wind_cooling_the_weather_limit);
		  bool windCoolingTheWeatherCoastalMorning = 
			(theParameters.theWindspeedCoastalMorningMean.value() != kFloatMissing &&
			 static_cast<int>(theParameters.theWindspeedCoastalMorningMean.value()) >= wind_cooling_the_weather_limit);
		  bool windCoolingTheWeatherInlandAfternoon = 
			(theParameters.theWindspeedInlandAfternoonMean.value() != kFloatMissing &&
			 static_cast<int>(theParameters.theWindspeedInlandAfternoonMean.value()) >= wind_cooling_the_weather_limit);
		  bool windCoolingTheWeatherCoastalAfternoon = 
			(theParameters.theWindspeedCoastalAfternoonMean.value() != kFloatMissing &&
			 static_cast<int>(theParameters.theWindspeedCoastalAfternoonMean.value()) >= wind_cooling_the_weather_limit);

			 float temperatureInlandMorning = theParameters.theDay2TemperatureInlandMorningMean.value();
			 float temperatureInlandAfternoon = theParameters.theDay2TemperatureInlandAfternoonMean.value();
			 float temperatureCoastalMorning = theParameters.theDay2TemperatureCoastalMorningMean.value();
			 float temperatureCoastalAfternoon = theParameters.theDay2TemperatureCoastalAfternoonMean.value();

			 inlandIncluded = windCoolingTheWeatherInlandMorning || windCoolingTheWeatherInlandAfternoon;
			 coastIncluded = windCoolingTheWeatherCoastalMorning || windCoolingTheWeatherCoastalAfternoon;

			 if(inlandIncluded && coastIncluded)
			   {
				 if(windCoolingTheWeatherInlandMorning && windCoolingTheWeatherCoastalMorning &&
					windCoolingTheWeatherInlandAfternoon && windCoolingTheWeatherCoastalAfternoon)
				   {
					 float morningTemperature = temperatureInlandMorning > temperatureCoastalMorning ?
					   temperatureInlandMorning : temperatureCoastalMorning;
					 float afternoonTemperature = temperatureInlandAfternoon > temperatureCoastalAfternoon ?
					   temperatureInlandAfternoon : temperatureCoastalAfternoon;
					 temperature = afternoonTemperature > morningTemperature ? afternoonTemperature : morningTemperature;
				   }
				 else if(!windCoolingTheWeatherInlandMorning && windCoolingTheWeatherCoastalMorning &&
						 !windCoolingTheWeatherInlandAfternoon && windCoolingTheWeatherCoastalAfternoon)
				   {
					 temperature = temperatureCoastalAfternoon > 
					   temperatureCoastalMorning ? temperatureCoastalAfternoon : temperatureCoastalMorning;
					 areaString = RANNIKOLLA_WORD;
				   }
				 else if(!windCoolingTheWeatherInlandMorning && windCoolingTheWeatherCoastalMorning &&
						 !windCoolingTheWeatherInlandAfternoon && !windCoolingTheWeatherCoastalAfternoon)
				   {
					 temperature = temperatureCoastalMorning; 
					 part_of_the_day = AAMUPAIVALLA_WORD;
					 areaString = RANNIKOLLA_WORD;
				   }
				 else if(!windCoolingTheWeatherInlandMorning && !windCoolingTheWeatherCoastalMorning &&
						 !windCoolingTheWeatherInlandAfternoon && windCoolingTheWeatherCoastalAfternoon)
				   {
					 temperature = temperatureCoastalAfternoon;
					 part_of_the_day = ILTAPAIVALLA_WORD;
					 areaString = RANNIKOLLA_WORD;
				   }
				 else if(windCoolingTheWeatherInlandMorning && !windCoolingTheWeatherCoastalMorning &&
						 windCoolingTheWeatherInlandAfternoon && !windCoolingTheWeatherCoastalAfternoon)
				   {
					 temperature = temperatureInlandAfternoon > 
					   temperatureInlandMorning ? temperatureInlandAfternoon : temperatureInlandMorning;
					 areaString = SISAMAASSA_WORD;
				   }
				 else if(windCoolingTheWeatherInlandMorning && !windCoolingTheWeatherCoastalMorning &&
						 !windCoolingTheWeatherInlandAfternoon && !windCoolingTheWeatherCoastalAfternoon)
				   {
					 temperature = temperatureInlandMorning;
					 part_of_the_day = AAMUPAIVALLA_WORD;
					 areaString = SISAMAASSA_WORD;
				   }
				 else if(!windCoolingTheWeatherInlandMorning && !windCoolingTheWeatherCoastalMorning &&
						 windCoolingTheWeatherInlandAfternoon && !windCoolingTheWeatherCoastalAfternoon)
				   {
					 temperature = temperatureInlandAfternoon;
					 part_of_the_day = ILTAPAIVALLA_WORD;
					 areaString = SISAMAASSA_WORD;
				   }
				 else if(windCoolingTheWeatherInlandMorning && windCoolingTheWeatherCoastalMorning)
				   {
					 temperature = temperatureCoastalMorning > 
					   temperatureInlandMorning ? temperatureCoastalMorning : temperatureInlandMorning;
					 part_of_the_day = AAMUPAIVALLA_WORD;
				   }
				 else if(windCoolingTheWeatherInlandAfternoon && windCoolingTheWeatherCoastalAfternoon)
				   {
					 temperature = temperatureCoastalAfternoon > 
					   temperatureInlandAfternoon ? temperatureCoastalAfternoon : temperatureInlandAfternoon;
					 part_of_the_day = ILTAPAIVALLA_WORD;
				   }
			   }
			 else if(!inlandIncluded && coastIncluded)
			   {
				 if(windCoolingTheWeatherCoastalMorning && !windCoolingTheWeatherCoastalAfternoon)
				   {
					 temperature = temperatureCoastalMorning;
					 part_of_the_day = AAMUPAIVALLA_WORD;
				   }
				 else if(!windCoolingTheWeatherCoastalMorning && windCoolingTheWeatherCoastalAfternoon)
				   {
					 temperature = temperatureCoastalAfternoon;
					 part_of_the_day = ILTAPAIVALLA_WORD;
				   }
				 else
				   {
					 temperature = temperatureCoastalAfternoon > 
					   temperatureCoastalMorning ? temperatureCoastalAfternoon : temperatureCoastalMorning;
				   }
			   }
			 else if(inlandIncluded && !coastIncluded)
			   {
				 if(windCoolingTheWeatherInlandMorning && !windCoolingTheWeatherInlandAfternoon)
				   {
					 temperature = temperatureInlandMorning;
					 part_of_the_day = AAMUPAIVALLA_WORD;
				   }
				 else if(!windCoolingTheWeatherInlandMorning && windCoolingTheWeatherInlandAfternoon)
				   {
					 temperature = temperatureInlandAfternoon;
					 part_of_the_day = ILTAPAIVALLA_WORD;
				   }
				 else
				   {
					 temperature = temperatureInlandAfternoon > 
					   temperatureInlandMorning ? temperatureInlandAfternoon : temperatureInlandMorning;
				   }
			   }

			 if(inlandIncluded || coastIncluded)
			   {
				 Sentence windCoolingSentence;

				 if(temperature > 0.0 && temperature <= 10.0)
				   windCoolingSentence << TUULI_KYLMENTAA_SAATA_PHRASE << theSpecifiedDay 
									   << areaString << part_of_the_day ;
				 else if(temperature > 10.0)
				   windCoolingSentence << TUULI_VIILENTAA_SAATA_PHRASE << theSpecifiedDay 
									   << areaString << part_of_the_day ;

				 sentence << windCoolingSentence;
			   }
		}

	  return sentence;
	}

	const Sentence windchill_sentence(const temperature_anomaly_params& theParameters)
	{
	  Sentence sentence;
	  
	  bool inlandIncluded = theParameters.theWindchillInlandMorningMinimum.value() != kFloatMissing;
	  bool coastIncluded = theParameters.theWindchillInlandMorningMinimum.value() != kFloatMissing;

	  WeatherResult windChillMorningMean = theParameters.theWindchillInlandMorningMinimum;
	  WeatherResult windChillAfternoonMean = theParameters.theWindchillInlandMorningMinimum;

	  forecast_area_id areaMorning = FULL_AREA;
	  forecast_area_id areaAfternoon = FULL_AREA;
	  std::string theSpecifiedDay(theParameters.theIsToday ? TANAAN_WORD : HUOMENNA_WORD);

	  if(inlandIncluded && coastIncluded)
		{
		  if(theParameters.theWindchillInlandMorningMean.value() > theParameters.theWindchillCoastalMorningMean.value())
			{
			  windChillMorningMean = theParameters.theWindchillInlandMorningMean;
			  areaMorning = INLAND_AREA;
			}
		  else
			{
			  windChillMorningMean = theParameters.theWindchillCoastalMorningMean;
			  areaMorning = COASTAL_AREA;
			}

		  if(theParameters.theWindchillInlandAfternoonMean.value() > theParameters.theWindchillCoastalAfternoonMean.value())
			{
			  windChillAfternoonMean = theParameters.theWindchillInlandAfternoonMean;
			  areaAfternoon = INLAND_AREA;
			}
		  else
			{
			  windChillAfternoonMean = theParameters.theWindchillCoastalAfternoonMean;
			  areaAfternoon = COASTAL_AREA;
			}
		}
	  else if(inlandIncluded)
		{
		  if(theParameters.theWindchillInlandMorningMean.value() > theParameters.theWindchillCoastalMorningMean.value())
			windChillMorningMean = theParameters.theWindchillInlandMorningMean;
		  else
			windChillMorningMean = theParameters.theWindchillCoastalMorningMean;
		}
	  else if(coastIncluded)
		{
		  if(theParameters.theWindchillInlandAfternoonMean.value() > theParameters.theWindchillCoastalAfternoonMean.value())
			windChillAfternoonMean = theParameters.theWindchillInlandAfternoonMean;
		  else
			windChillAfternoonMean = theParameters.theWindchillCoastalAfternoonMean;
		}

	  float windChill = windChillMorningMean.value() > windChillAfternoonMean.value() ? 
		windChillMorningMean.value() : windChillAfternoonMean.value();

	  bool windChillMorning = (windChillMorningMean.value() >= EXTREME_WINDCHILL_LIMIT && 
						   windChillMorningMean.value() <= MILD_WINDCHILL_LIMIT);
	  bool windChillAfternoon = (windChillAfternoonMean.value() >= EXTREME_WINDCHILL_LIMIT &&
							 windChillAfternoonMean.value() <= MILD_WINDCHILL_LIMIT);
	  bool extremelyWindChillMorning = windChillMorningMean.value() > EXTREME_WINDCHILL_LIMIT;
	  bool extremelyWindChillAfternoon = windChillAfternoonMean.value() > EXTREME_WINDCHILL_LIMIT;

	  std::string areaString((areaMorning == INLAND_AREA ? SISAMAASSA_WORD 
							  : (areaMorning == COASTAL_AREA ? RANNIKOLLA_WORD : "")));

	  if(windChill >= EXTREME_WINDCHILL_LIMIT && windChill <= MILD_WINDCHILL_LIMIT)
		{
		  sentence << PAKKANEN_ON_PUREVAA_PHRASE << theSpecifiedDay;

		  if(windChillMorning && !windChillAfternoon)
			{
			  sentence << AAMUPAIVALLA_WORD << areaString;
			}
		  else if(!windChillMorning && windChillAfternoon)
			{
			  sentence << ILTAPAIVALLA_WORD << areaString;
			}
		}
	  else if(windChill < EXTREME_WINDCHILL_LIMIT)
		{
		  sentence << PAKKANEN_ON_ERITTAIN_PUREVAA_PHRASE << theSpecifiedDay;

		  if(extremelyWindChillMorning && !extremelyWindChillAfternoon)
			{
			  sentence << AAMUPAIVALLA_WORD << areaString;
			}
		  else if(!extremelyWindChillMorning && extremelyWindChillAfternoon)
			{
			  sentence << ILTAPAIVALLA_WORD << areaString;
			}
		}

	  return sentence;
	}

	Sentence handle_anomaly_and_shortrun_trend_sentences(const temperature_anomaly_params& theParameters, 
														 const Sentence& anomalySentence, 
														 const Sentence& shortrunTrendSentence)
	{
	  Sentence sentence;

	  if(theParameters.theAnomalyPhrase == SAA_ON_POIKKEUKSELLISEN_KOLEAA ||
		 theParameters.theAnomalyPhrase == SAA_ON_HARVINAISEN_LAMMINTA ||
		 theParameters.theAnomalyPhrase == SAA_ON_POIKKEUKSLLISEN_LAMMINTA ||
		 theParameters.theAnomalyPhrase == SAA_ON_HYVIN_LEUTOA ||
		 theParameters.theAnomalyPhrase == SAA_ON_POIKKEUKSLLISEN_LAMMINTA)
		{
		  sentence << anomalySentence;
		}
	  else if(theParameters.theAnomalyPhrase == SAA_ON_KOLEAA)
		{
		  switch(theParameters.theShortrunTrend)
			{
			case SAA_LAMPENEE_HUOMATTAVASTI:
			case SAA_VIILENEE_HUOMATTAVASTI:
			  sentence << shortrunTrendSentence;
			  break;
			default:
			  sentence << anomalySentence;
			}
		}
	  else if(theParameters.theAnomalyPhrase == SAA_ON_POIKKEUKSELLISEN_KYLMAA ||
			  theParameters.theAnomalyPhrase == SAA_ON_HYVIN_KYLMAA)
		{
		  switch(theParameters.theShortrunTrend)
			{
			case KIREA_PAKKANEN_HEIKKENEE:
			case PAKKANEN_HEIKKENEE:
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

  } // namespace TemperatureAnomaly

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on temperature for the day and night
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  const Paragraph TemperatureStory::anomaly() const
  {
	using namespace TemperatureAnomaly;

	Paragraph paragraph;
	MessageLogger log("TemperatureStory::anomaly");

	if(itsArea.isNamed())
	  {
		std::string nimi(itsArea.name());
		log <<  nimi << endl;
	  }

	const int day_starthour = optional_hour(itsVar+"::day::starthour",6);
	const int day_maxstarthour = optional_hour(itsVar+"::day::maxstarthour",12);

	forecast_season_id theSeasonId = isSummerHalf(itsPeriod.localStartTime(), itsVar) ? SUMMER_SEASON : WINTER_SEASON;

	// the anomaly sentence relates always to the upcoming day,
	// so the period is defined to start in the previous day	

	log_start_time_and_end_time(log, 
								"the original period: ",
								itsPeriod);

	NFmiTime periodStartTime(itsPeriod.localStartTime());
	NFmiTime periodEndTime(itsPeriod.localEndTime());

	// Period generator
	NightAndDayPeriodGenerator generator00(itsPeriod, itsVar);

	if(periodStartTime.GetHour() >= day_starthour)
	  {
		periodStartTime.ChangeByHours(-1*(periodStartTime.GetHour() - day_starthour));
		//		if(generator00.size() == 0 || generator00.isday(1))
		if(itsForecastTime.GetHour() < day_maxstarthour)
		  periodStartTime.ChangeByHours(-24);
	  }
	else if(periodStartTime.GetHour() < day_starthour)
	  {
		periodStartTime.ChangeByHours(day_starthour - periodStartTime.GetHour());
		periodStartTime.ChangeByHours(-24);
	  }
	if(generator00.size() == 0)
	  periodEndTime.ChangeByHours(24);

	WeatherPeriod theExtendedPeriod(periodStartTime, periodEndTime);

	NightAndDayPeriodGenerator generator(theExtendedPeriod, itsVar);
	for(unsigned int i = 0; i < generator.size(); i++ )
	  log_start_time_and_end_time(log, 
								  "the extended period: ",
								  generator.period(i+1));

	WeatherPeriod day1Period(generator.period(1));
	WeatherPeriod nightPeriod(generator.period(2));
	WeatherPeriod day2Period(generator.period(3));

	log_start_time_and_end_time(log, 
								"day1: ",
								day1Period);
	log_start_time_and_end_time(log, 
								"night: ",
								nightPeriod);
	log_start_time_and_end_time(log, 
								"day2: ",
								day2Period);


	temperature_anomaly_params parameters(itsVar,
										  log,
										  itsSources,
										  itsArea,
										  theExtendedPeriod,
										  day1Period,
										  nightPeriod,
										  day2Period,
										  theSeasonId,
										  itsForecastTime);

	parameters.theIsToday = (itsForecastTime.GetHour() < day_maxstarthour);

	morning_temperature(itsVar + "::fake::temperature::day1::morning::area",
						  itsSources,
						  itsArea,
						  day1Period,
						  parameters.theDay1TemperatureAreaMorningMinimum,
						  parameters.theDay1TemperatureAreaMorningMaximum,
						  parameters.theDay1TemperatureAreaMorningMean);

	afternoon_temperature(itsVar + "::fake::temperature::day1::afternoon::area",
						  itsSources,
						  itsArea,
						  day1Period,
						  parameters.theDay1TemperatureAreaAfternoonMinimum,
						  parameters.theDay1TemperatureAreaAfternoonMaximum,
						  parameters.theDay1TemperatureAreaAfternoonMean);

	calculate_night_temperature(parameters);

	morning_temperature(itsVar + "::fake::temperature::day2::morning::area",
						  itsSources,
						  itsArea,
						  day2Period,
						  parameters.theDay2TemperatureAreaMorningMinimum,
						  parameters.theDay2TemperatureAreaMorningMaximum,
						  parameters.theDay2TemperatureAreaMorningMean);

	afternoon_temperature(itsVar + "::fake::temperature::day2::afternoon::area",
						  itsSources,
						  itsArea,
						  day2Period,
						  parameters.theDay2TemperatureAreaAfternoonMinimum,
						  parameters.theDay2TemperatureAreaAfternoonMaximum,
						  parameters.theDay2TemperatureAreaAfternoonMean);

	WeatherArea inlandArea = itsArea;
	inlandArea.type(WeatherArea::Inland);
	WeatherArea coastalArea = itsArea;
	coastalArea.type(WeatherArea::Coast);

	NFmiTime day1NoonTime(day1Period.localStartTime().GetYear(),
						  day1Period.localStartTime().GetMonth(),
						  day1Period.localStartTime().GetDay(), 12, 0, 0);
	WeatherPeriod day1MorningPeriod(day1Period.localStartTime(), day1NoonTime);
	WeatherPeriod day1AfternoonPeriod(day1NoonTime, day1Period.localEndTime());
	log_start_time_and_end_time(log, 
								"day1 morning: ",
								day1MorningPeriod);

	log_start_time_and_end_time(log, 
								"day1 afternoon: ",
								day1AfternoonPeriod);


	morning_temperature(itsVar + "::fake::temperature::day1::morning::inland",
						itsSources,
						inlandArea,
						day1MorningPeriod,
						parameters.theDay1TemperatureInlandMorningMinimum,
						parameters.theDay1TemperatureInlandMorningMaximum,
						parameters.theDay1TemperatureInlandMorningMean);

	afternoon_temperature(itsVar + "::fake::temperature::day1::afternoon::inland",
						  itsSources,
						  inlandArea,
						  day1MorningPeriod,
						  parameters.theDay1TemperatureInlandAfternoonMinimum,
						  parameters.theDay1TemperatureInlandAfternoonMaximum,
						  parameters.theDay1TemperatureInlandAfternoonMean);

	morning_temperature(itsVar + "::fake::temperature::day1::morning::coast",
						itsSources,
						coastalArea,
						day1MorningPeriod,
						parameters.theDay1TemperatureCoastalMorningMinimum,
						parameters.theDay1TemperatureCoastalMorningMaximum,
						parameters.theDay1TemperatureCoastalMorningMean);

	afternoon_temperature(itsVar + "::fake::temperature::day1::afternoon::coast",
						  itsSources,
						  coastalArea,
						  day1MorningPeriod,
						  parameters.theDay1TemperatureCoastalAfternoonMinimum,
						  parameters.theDay1TemperatureCoastalAfternoonMaximum,
						  parameters.theDay1TemperatureCoastalAfternoonMean);

	NFmiTime day2NoonTime(day2Period.localStartTime().GetYear(),
						  day2Period.localStartTime().GetMonth(),
						  day2Period.localStartTime().GetDay(), 12, 0, 0);
	WeatherPeriod day2MorningPeriod(day2Period.localStartTime(), day2NoonTime);
	WeatherPeriod day2AfternoonPeriod(day2NoonTime, day2Period.localEndTime());
	log_start_time_and_end_time(log, 
								"day2 morning: ",
								day2MorningPeriod);

	log_start_time_and_end_time(log, 
								"day2 afternoon: ",
								day2AfternoonPeriod);

	morning_temperature(itsVar + "::fake::temperature::day2::morning::inland",
						itsSources,
						inlandArea,
						day2MorningPeriod,
						parameters.theDay2TemperatureInlandMorningMinimum,
						parameters.theDay2TemperatureInlandMorningMaximum,
						parameters.theDay2TemperatureInlandMorningMean);

	afternoon_temperature(itsVar + "::fake::temperature::day2::afternoon::inland",
						  itsSources,
						  inlandArea,
						  day2MorningPeriod,
						  parameters.theDay2TemperatureInlandAfternoonMinimum,
						  parameters.theDay2TemperatureInlandAfternoonMaximum,
						  parameters.theDay2TemperatureInlandAfternoonMean);

	morning_temperature(itsVar + "::fake::temperature::day2::morning::coast",
						itsSources,
						coastalArea,
						day2MorningPeriod,
						parameters.theDay2TemperatureCoastalMorningMinimum,
						parameters.theDay2TemperatureCoastalMorningMaximum,
						parameters.theDay2TemperatureCoastalMorningMean);

	afternoon_temperature(itsVar + "::fake::temperature::day2::afternoon::coast",
						  itsSources,
						  coastalArea,
						  day2MorningPeriod,
						  parameters.theDay2TemperatureCoastalAfternoonMinimum,
						  parameters.theDay2TemperatureCoastalAfternoonMaximum,
						  parameters.theDay2TemperatureCoastalAfternoonMean);

#ifdef INCLUDE_WIND_ANOMALY
	bool handleWindchill = false;

	// inland, morning, windspeed
	calculate_windspeed_and_chill(parameters, true, true, true);
	// inland afternoon, windspeed
	calculate_windspeed_and_chill(parameters, true, false, true);
	// coastal morning, windspeed
	calculate_windspeed_and_chill(parameters, false, true, true);
	// coastal afternoon, windspeed
	calculate_windspeed_and_chill(parameters, false, false, true);

	if(parameters.theDay2TemperatureAreaMorningMean.value() < ZERO_DEGREES || 
	   parameters.theDay2TemperatureAreaAfternoonMean.value() < ZERO_DEGREES)
	  handleWindchill = true;

	if(handleWindchill)
	  {
		// inland, morning, windchill
		calculate_windspeed_and_chill(parameters, true, true, false);
		// inland afternoon, windchill
		calculate_windspeed_and_chill(parameters, true, false, false);
		// coastal morning, windchill
		calculate_windspeed_and_chill(parameters, false, true, false);
		// coastal afternoon, windchill
		calculate_windspeed_and_chill(parameters, false, false, false);
	  }
#endif

	fractile_id theFractileDay1 = get_fractile(itsVar,
											   parameters.theDay1TemperatureAreaAfternoonMean.value(),
											   itsSources,
											   itsArea,
											   day1Period);

	fractile_id theFractileDay2 = get_fractile(itsVar,
											   parameters.theDay2TemperatureAreaAfternoonMean.value(),
											   itsSources,
											   itsArea,
											   day2Period);

	WeatherResult fractileTemperatureDay1 = get_fractile_temperature(itsVar,
																	 theFractileDay1,
																	 itsSources,
																	 itsArea,  
																	 day1Period);

	WeatherResult fractileTemperatureDay2 = get_fractile_temperature(itsVar,
																	 theFractileDay2,
																	 itsSources,
																	 itsArea,  
																	 day2Period);

	WeatherResult fractile37Temperature = get_fractile_temperature(itsVar,
																   FRACTILE_37,
																   itsSources,
																   itsArea,
																   day2Period);

	WeatherResult fractile12Temperature = get_fractile_temperature(itsVar,
																   FRACTILE_12,
																   itsSources,
																   itsArea,
																   day2Period);
	
	Paragraph paragraphDev;
	Sentence temperatureAnomalySentence;
	Sentence shortrunTrendSentence;
#ifdef INCLUDE_WIND_ANOMALY
	Sentence windinessSentence;
	Sentence windChillSentence;
#endif

	temperatureAnomalySentence << temperature_anomaly_sentence(parameters, theFractileDay2);

	shortrunTrendSentence <<  temperature_shortruntrend_sentence(parameters);

#ifdef INCLUDE_WIND_ANOMALY
	windinessSentence << windiness_sentence(parameters);
	if(handleWindchill)
	  windChillSentence << windchill_sentence(parameters);
#endif

	log_data(parameters);

	log << "Day1 temperature " 
		<< parameters.theDay1TemperatureAreaAfternoonMean
		<< " falls into fractile " 
		<< fractile_name(theFractileDay1) << fractileTemperatureDay1 << endl;
	log << "Day2 temperature " 
		<< parameters.theDay2TemperatureAreaAfternoonMean
		<< " falls into fractile " 
		<< fractile_name(theFractileDay2) << fractileTemperatureDay2 << endl;


	log << "anomaly: ";
	log << temperatureAnomalySentence;
	log << "short run trend: ";
	log << shortrunTrendSentence;

#ifdef INCLUDE_WIND_ANOMALY
	log << "windiness: ";
	log << windinessSentence;
	log << "wind chill: ";
	log << windChillSentence;
#endif

	paragraph << handle_anomaly_and_shortrun_trend_sentences(parameters, 
															 temperatureAnomalySentence, 
															 shortrunTrendSentence);
#ifdef INCLUDE_WIND_ANOMALY
	paragraph << windinessSentence;
	paragraph << windChillSentence;
#endif

	log << paragraph;

	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================

  
