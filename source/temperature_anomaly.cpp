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
								 const WeatherPeriod& day2Period,
								 const forecast_season_id& season,
								 const NFmiTime& forecastTime,
								 const short& periodLength) 
		: theVariable(variable),
		  theLog(log),
		  theSources(sources),
		  theArea(area),
		  thePeriod(period),
		  theDay1Period(day1Period),
		  theDay2Period(day2Period),
		  theSeason(season),
		  theForecastTime(forecastTime),
		  thePeriodLength(periodLength),
		  theAnomalyPhrase(UNDEFINED_ANOMALY_PHRASE_ID),
		  theShortrunTrend(UNDEFINED_SHORTRUN_TREND_ID),
		  theFakeVariable(""),
		  theDay1TemperatureAreaAfternoonMinimum(kFloatMissing, 0),
		  theDay1TemperatureAreaAfternoonMean(kFloatMissing, 0),
		  theDay1TemperatureAreaAfternoonMaximum(kFloatMissing, 0),
		  theDay1TemperatureInlandAfternoonMinimum(kFloatMissing, 0),
		  theDay1TemperatureInlandAfternoonMean(kFloatMissing, 0),
		  theDay1TemperatureInlandAfternoonMaximum(kFloatMissing, 0),
		  theDay1TemperatureCoastalAfternoonMinimum(kFloatMissing, 0),
		  theDay1TemperatureCoastalAfternoonMean(kFloatMissing, 0),
		  theDay1TemperatureCoastalAfternoonMaximum(kFloatMissing, 0),
		  theDay2TemperatureAreaAfternoonMinimum(kFloatMissing, 0),
		  theDay2TemperatureAreaAfternoonMean(kFloatMissing, 0),
		  theDay2TemperatureAreaAfternoonMaximum(kFloatMissing, 0),
		  theDay2TemperatureInlandAfternoonMinimum(kFloatMissing, 0),
		  theDay2TemperatureInlandAfternoonMean(kFloatMissing, 0),
		  theDay2TemperatureInlandAfternoonMaximum(kFloatMissing, 0),
		  theDay2TemperatureCoastalAfternoonMinimum(kFloatMissing, 0),
		  theDay2TemperatureCoastalAfternoonMean(kFloatMissing, 0),
		  theDay2TemperatureCoastalAfternoonMaximum(kFloatMissing, 0)
	  {}

	  const string& theVariable;
	  MessageLogger& theLog;
	  const AnalysisSources& theSources;
	  const WeatherArea& theArea;
	  const WeatherPeriod& thePeriod;
	  const WeatherPeriod& theDay1Period;
	  const WeatherPeriod& theDay2Period;
	  const forecast_season_id& theSeason;
	  const NFmiTime& theForecastTime;
	  const short& thePeriodLength;
	  anomaly_phrase_id theAnomalyPhrase;
	  shortrun_trend_id theShortrunTrend;
	  string theFakeVariable;
	  WeatherResult theDay1TemperatureAreaAfternoonMinimum;
	  WeatherResult theDay1TemperatureAreaAfternoonMean;
	  WeatherResult theDay1TemperatureAreaAfternoonMaximum;
	  WeatherResult theDay1TemperatureInlandAfternoonMinimum;
	  WeatherResult theDay1TemperatureInlandAfternoonMean;
	  WeatherResult theDay1TemperatureInlandAfternoonMaximum;
	  WeatherResult theDay1TemperatureCoastalAfternoonMinimum;
	  WeatherResult theDay1TemperatureCoastalAfternoonMean;
	  WeatherResult theDay1TemperatureCoastalAfternoonMaximum;
	  WeatherResult theDay2TemperatureAreaAfternoonMinimum;
	  WeatherResult theDay2TemperatureAreaAfternoonMean;
	  WeatherResult theDay2TemperatureAreaAfternoonMaximum;
	  WeatherResult theDay2TemperatureInlandAfternoonMinimum;
	  WeatherResult theDay2TemperatureInlandAfternoonMean;
	  WeatherResult theDay2TemperatureInlandAfternoonMaximum;
	  WeatherResult theDay2TemperatureCoastalAfternoonMinimum;
	  WeatherResult theDay2TemperatureCoastalAfternoonMean;
	  WeatherResult theDay2TemperatureCoastalAfternoonMaximum;
	};

	void log_data(const temperature_anomaly_params& theParameters)
	{
	  theParameters.theLog << "theDay1TemperatureAreaAfternoonMinimum: " 
						   << theParameters.theDay1TemperatureAreaAfternoonMinimum << endl;
	  theParameters.theLog << "theDay1TemperatureAreaAfternoonMean: " 
						   << theParameters.theDay1TemperatureAreaAfternoonMean << endl;
	  theParameters.theLog << "theDay1TemperatureAreaAfternoonMaximum: " 
						   << theParameters.theDay1TemperatureAreaAfternoonMaximum << endl;

	  if(theParameters.theDay1TemperatureInlandAfternoonMinimum.value() != kFloatMissing)
		{
		  theParameters.theLog << "theDay1TemperatureInlandAfternoonMinimum: " 
							   << theParameters.theDay1TemperatureInlandAfternoonMinimum << endl;
		  theParameters.theLog << "theDay1TemperatureInlandAfternoonMean: " 
							   << theParameters.theDay1TemperatureInlandAfternoonMean << endl;
		  theParameters.theLog << "theDay1TemperatureInlandAfternoonMaximum: " 
							   << theParameters.theDay1TemperatureInlandAfternoonMaximum << endl;
		}

	  if(theParameters.theDay1TemperatureCoastalAfternoonMinimum.value() != kFloatMissing)
		{
		  theParameters.theLog << "theDay1TemperatureCoastalAfternoonMinimum: " 
							   << theParameters.theDay1TemperatureCoastalAfternoonMinimum << endl;
		  theParameters.theLog << "theDay1TemperatureCoastalAfternoonMean: " 
							   << theParameters.theDay1TemperatureCoastalAfternoonMean << endl;
		  theParameters.theLog << "theDay1TemperatureCoastalAfternoonMaximum: " 
							   << theParameters.theDay1TemperatureCoastalAfternoonMaximum << endl;
		}

	  theParameters.theLog << "theDay2TemperatureAreaAfternoonMinimum: " 
						   << theParameters.theDay2TemperatureAreaAfternoonMinimum << endl;
	  theParameters.theLog << "theDay2TemperatureAreaAfternoonMean: " 
						   << theParameters.theDay2TemperatureAreaAfternoonMean << endl;
	  theParameters.theLog << "theDay2TemperatureAreaAfternoonMaximum: " 
						   << theParameters.theDay2TemperatureAreaAfternoonMaximum << endl;

	  if(theParameters.theDay2TemperatureInlandAfternoonMinimum.value() != kFloatMissing)
		{
		  theParameters.theLog << "theDay2TemperatureInlandAfternoonMinimum: " 
							   << theParameters.theDay2TemperatureInlandAfternoonMinimum << endl;
		  theParameters.theLog << "theDay2TemperatureInlandAfternoonMean: " 
							   << theParameters.theDay2TemperatureInlandAfternoonMean << endl;
		  theParameters.theLog << "theDay2TemperatureInlandAfternoonMaximum: " 
							   << theParameters.theDay2TemperatureInlandAfternoonMaximum << endl;
		}

	  if(theParameters.theDay2TemperatureCoastalAfternoonMinimum.value() != kFloatMissing)
		{
		  theParameters.theLog << "theDay2TemperatureCoastalAfternoonMinimum: " 
							   << theParameters.theDay2TemperatureCoastalAfternoonMinimum << endl;
		  theParameters.theLog << "theDay2TemperatureCoastalAfternoonMean: " 
							   << theParameters.theDay2TemperatureCoastalAfternoonMean << endl;
		  theParameters.theLog << "theDay2TemperatureCoastalAfternoonMaximum: " 
							   << theParameters.theDay2TemperatureCoastalAfternoonMaximum << endl;
		}
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
												const fractile_id& theFractile,
												const bool& theDay1Anomaly)
	{
	  Sentence retval;

	  Sentence theSpecifiedDay;

	  if(theParameters.thePeriodLength > 24)
		{
		  theSpecifiedDay << PeriodPhraseFactory::create("today",
														 theParameters.theVariable,
														 theParameters.theForecastTime,
														 theDay1Anomaly ? theParameters.theDay1Period : theParameters.theDay2Period,
														 theParameters.theArea);
		}

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

	  WeatherResult fractile63Temperature = get_fractile_temperature(theParameters.theVariable,
																	 FRACTILE_63,
																	 theParameters.theSources,
																	 theParameters.theArea,
																	 forecastTimeWeatherPeriod);
	  Sentence theSpecifiedDay;
	  theSpecifiedDay << PeriodPhraseFactory::create("today",
													 theParameters.theVariable,
													 theParameters.theForecastTime,
													 theParameters.theDay2Period,
													 theParameters.theArea);

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
				 period2Temperature > WEAK_FROST_TEMPERATURE_LIMIT && period2Temperature < LOW_PLUS_TEMPARATURE &&
				 period2Temperature > fractile63Temperature.value())
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
   * \brief Generate temperature anomaly story
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

	//	const int day_starthour = optional_hour(itsVar+"::day::starthour",6);
	//	const int day_maxstarthour = optional_hour(itsVar+"::day::maxstarthour",12);

	forecast_season_id theSeasonId = isSummerHalf(itsPeriod.localStartTime(), itsVar) ? SUMMER_SEASON : WINTER_SEASON;

	// the anomaly sentence relates always to the upcoming day,
	// so the period is defined to start in the previous day	

	log_start_time_and_end_time(log, 
								"the original period: ",
								itsPeriod);

	NFmiTime periodStartTime(itsPeriod.localStartTime());
	NFmiTime periodEndTime(itsPeriod.localEndTime());
	int periodLength = periodEndTime.DifferenceInHours(periodStartTime);

	// Period generator
	NightAndDayPeriodGenerator generator00(itsPeriod, itsVar);

	if(generator00.size() == 0)
	  {
		log << "No weather periods available!" << endl;
		log << paragraph;
		return paragraph;
	  }

	log << "period contains ";
	bool report_day2_anomaly = false;

	if(generator00.isday(1))
	  {
		if(generator00.size() > 2)
		  {
			log << "day, night and next day" << endl;
		  }
		else if(generator00.size() == 2)
		  {
			log << "day and night" << endl;
			// if forecast time is not today, extend period from the start
			if(abs(itsForecastTime.DifferenceInHours(generator00.period(1).localStartTime())) > 12)
			  periodStartTime.ChangeByHours(-24);
			else
			  periodEndTime.ChangeByHours(12);
		  }
		else
		  {
			log << "day" << endl;
			// if forecast time is not today, extend period from the start
			if(abs(itsForecastTime.DifferenceInHours(generator00.period(1).localStartTime())) > 12)
			  periodStartTime.ChangeByHours(-24);
			else
			  periodEndTime.ChangeByHours(24);
		  }
		// if forecast time is not today, report temperature anomaly for tomorrow
		if(abs(itsForecastTime.DifferenceInHours(generator00.period(1).localStartTime())) > 12)
		  {
			report_day2_anomaly = true;
		  }
	  }
	else
	  {
		if(generator00.size() == 1)
		  {
			log << "one night" << endl;
			periodStartTime.ChangeByHours(-12);
			periodEndTime.ChangeByHours(12);
			report_day2_anomaly = true;
		  }
		else
		  {
			log << "night and next day" << endl;		  
			periodStartTime.ChangeByHours(-12);
			report_day2_anomaly = true;
		  }
	  }

	/*
	if(periodStartTime.GetHour() >= day_starthour)
	  {
		periodStartTime.ChangeByHours(-1*(periodStartTime.GetHour() - day_starthour));
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

	int diffInHours = abs(periodStartTime.DifferenceInHours(periodEndTime));
	if(diffInHours < 48)
	  periodEndTime.ChangeByHours(48 - diffInHours);
	*/

	/*
	// Too late? Return empty story then
	if(generator.size() == 0)
	  {
		log << "No weather periods available!" << endl;
		log << paragraph;
		return paragraph;
	  }

	log << "period contains ";

	if(generator.isday(1))
	  {
		if(generator.size() > 2)
		  log << "today, night and tomorrow" << endl;
		else if(generator.size() == 2)
		  log << "today and night" << endl;
		else
		  log << "today" << endl;
	  }
	else
	  {
		if(generator.size() == 1)
		  log << "one night" << endl;		  
		else
		  log << "night and tomorrow" << endl;		  
	  }

	 */


	WeatherPeriod theExtendedPeriod(periodStartTime, periodEndTime);

	NightAndDayPeriodGenerator generator(theExtendedPeriod, itsVar);
	for(unsigned int i = 0; i < generator.size(); i++ )
	  {
	  log_start_time_and_end_time(log, 
								  "the extended period: ",
								  generator.period(i+1));
	  }

	WeatherPeriod day1Period(generator.period(1));
	WeatherPeriod day2Period(generator.period(3));

	log_start_time_and_end_time(log, 
								"day1: ",
								day1Period);
	log_start_time_and_end_time(log, 
								"day2: ",
								day2Period);

	temperature_anomaly_params parameters(itsVar,
										  log,
										  itsSources,
										  itsArea,
										  theExtendedPeriod,
										  day1Period,
										  day2Period,
										  theSeasonId,
										  itsForecastTime,
										  periodLength);

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

	WeatherArea inlandArea = itsArea;
	inlandArea.type(WeatherArea::Inland);
	WeatherArea coastalArea = itsArea;
	coastalArea.type(WeatherArea::Coast);

	NFmiTime day1NoonTime(day1Period.localStartTime().GetYear(),
						  day1Period.localStartTime().GetMonth(),
						  day1Period.localStartTime().GetDay(), 12, 0, 0);
	WeatherPeriod day1AfternoonPeriod(day1NoonTime, day1Period.localEndTime());

	log_start_time_and_end_time(log, 
								"day1 afternoon: ",
								day1AfternoonPeriod);

	afternoon_temperature(itsVar + "::fake::temperature::day1::afternoon::inland",
						  itsSources,
						  inlandArea,
						  day1AfternoonPeriod,
						  parameters.theDay1TemperatureInlandAfternoonMinimum,
						  parameters.theDay1TemperatureInlandAfternoonMaximum,
						  parameters.theDay1TemperatureInlandAfternoonMean);

	afternoon_temperature(itsVar + "::fake::temperature::day1::afternoon::coast",
						  itsSources,
						  coastalArea,
						  day1AfternoonPeriod,
						  parameters.theDay1TemperatureCoastalAfternoonMinimum,
						  parameters.theDay1TemperatureCoastalAfternoonMaximum,
						  parameters.theDay1TemperatureCoastalAfternoonMean);

	NFmiTime day2NoonTime(day2Period.localStartTime().GetYear(),
						  day2Period.localStartTime().GetMonth(),
						  day2Period.localStartTime().GetDay(), 12, 0, 0);
	WeatherPeriod day2AfternoonPeriod(day2NoonTime, day2Period.localEndTime());

	log_start_time_and_end_time(log, 
								"day2 afternoon: ",
								day2AfternoonPeriod);

	afternoon_temperature(itsVar + "::fake::temperature::day2::afternoon::inland",
						  itsSources,
						  inlandArea,
						  day2AfternoonPeriod,
						  parameters.theDay2TemperatureInlandAfternoonMinimum,
						  parameters.theDay2TemperatureInlandAfternoonMaximum,
						  parameters.theDay2TemperatureInlandAfternoonMean);

	afternoon_temperature(itsVar + "::fake::temperature::day2::afternoon::coast",
						  itsSources,
						  coastalArea,
						  day2AfternoonPeriod,
						  parameters.theDay2TemperatureCoastalAfternoonMinimum,
						  parameters.theDay2TemperatureCoastalAfternoonMaximum,
						  parameters.theDay2TemperatureCoastalAfternoonMean);

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

	if(report_day2_anomaly)
	  temperatureAnomalySentence << temperature_anomaly_sentence(parameters, theFractileDay2, false);
	else
	  temperatureAnomalySentence << temperature_anomaly_sentence(parameters, theFractileDay1, true);

	shortrunTrendSentence <<  temperature_shortruntrend_sentence(parameters);

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

	paragraph << handle_anomaly_and_shortrun_trend_sentences(parameters, 
															 temperatureAnomalySentence, 
															 shortrunTrendSentence);
	log << paragraph;

	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================

  
