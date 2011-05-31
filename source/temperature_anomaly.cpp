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
#include "HourPeriodGenerator.h"
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
#include "RangeAcceptor.h"

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
#include "WeatherForecast.h"

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

#define MILD_TEMPERATURE_LOWER_LIMIT -3.0
#define MILD_TEMPERATURE_UPPER_LIMIT +5.0
#define HOT_WEATHER_LIMIT +25.0
#define GETTING_COOLER_NOTIFICATION_LIMIT +20.0
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
#define SAA_ON_EDELLEEN_LAUHAA_PHRASE "s‰‰ on edelleen lauhaa"
#define SAA_LAUHTUU_PHRASE "s‰‰ lauhtuu"
#define PAKKANEN_HEIKKENEE_PHRASE "pakkanen heikkenee"
#define KIREA_PAKKANEN_HEIKKENEE_PHRASE "kire‰ pakkanen heikkenee"
#define PAKKANEN_HELLITTAA_PHRASE "pakkanen hellitt‰‰"
#define KIREA_PAKKANEN_JATKUU_PHRASE "kire‰ pakkanen jatkuu"
#define PAKKANEN_KIRISTYY_PHRASE "pakkanen kiristyy"
#define HELTEINEN_SAA_JATKUU_PHRASE "helteinen s‰‰ jatkuu"
#define VIILEA_SAA_JATKUU_PHRASE "viile‰ s‰‰ jatkuu"
#define KOLEA_SAA_JATKUU_PHRASE "kolea s‰‰ jatkuu"
#define SAA_MUUTTUU_HELTEISEKSI_PHRASE "s‰‰ muuttuu helteiseksi"
#define SAA_ON_HELTEISTA_PHRASE "s‰‰ on helteist‰"
#define SAA_LAMPENEE_PHRASE "s‰‰ l‰mpenee"
#define SAA_LAMPENEE_VAHAN_PHRASE "s‰‰ l‰mpenee v‰h‰n"
#define SAA_LAMPENEE_HUOMATTAVASTI_PHRASE "s‰‰ l‰mpenee huomattavasti"
#define SAA_VIILENEE_PHRASE "s‰‰ viilenee"
#define SAA_VIILENEE_VAHAN_PHRASE "s‰‰ viilenee v‰h‰n"
#define SAA_VIILENEE_HUOMATTAVASTI_PHRASE "s‰‰ viilenee huomattavasti"

#define TEMPERATURE_IS_SOMETHING_COMPOSITE_PHRASE "[1-na] s‰‰ on [poikkeuksellisen] [kylm‰‰]"
#define TEMPERATURE_IS_SOMETHING_PLAIN_COMPOSITE_PHRASE "s‰‰ on [poikkeuksellisen] [kylm‰‰]"
#define TEMPERATURE_CHANGES_COMPOSITE_PHRASE "[1-na] [s‰‰ l‰mpenee]"

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
		  theDay2TemperatureAreaAfternoonMinimum(kFloatMissing, 0),
		  theDay2TemperatureAreaAfternoonMean(kFloatMissing, 0),
		  theDay2TemperatureAreaAfternoonMaximum(kFloatMissing, 0)
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
	  WeatherResult theDay2TemperatureAreaAfternoonMinimum;
	  WeatherResult theDay2TemperatureAreaAfternoonMean;
	  WeatherResult theDay2TemperatureAreaAfternoonMaximum;
	};

	void log_data(const temperature_anomaly_params& theParameters)
	{
	  theParameters.theLog << "theDay1TemperatureAreaAfternoonMinimum: " 
						   << theParameters.theDay1TemperatureAreaAfternoonMinimum << endl;
	  theParameters.theLog << "theDay1TemperatureAreaAfternoonMean: " 
						   << theParameters.theDay1TemperatureAreaAfternoonMean << endl;
	  theParameters.theLog << "theDay1TemperatureAreaAfternoonMaximum: " 
						   << theParameters.theDay1TemperatureAreaAfternoonMaximum << endl;

	  theParameters.theLog << "theDay2TemperatureAreaAfternoonMinimum: " 
						   << theParameters.theDay2TemperatureAreaAfternoonMinimum << endl;
	  theParameters.theLog << "theDay2TemperatureAreaAfternoonMean: " 
						   << theParameters.theDay2TemperatureAreaAfternoonMean << endl;
	  theParameters.theLog << "theDay2TemperatureAreaAfternoonMaximum: " 
						   << theParameters.theDay2TemperatureAreaAfternoonMaximum << endl;
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

	const void log_daily_factiles_for_period(MessageLogger& theLog,
											 const string& theVariable,											
											 const AnalysisSources& theSources,
											 const WeatherArea& theArea,
											 const std::string& theLogMessage, 
											 const WeatherPeriod& thePeriod,
											 const fractile_type_id& theFractileType)
	{
	  NFmiTime startTime(thePeriod.localStartTime());
	  NFmiTime endTime(thePeriod.localStartTime());

	  startTime.SetHour(6);
	  startTime.SetMin(0);
	  startTime.SetSec(0);
	  endTime.SetHour(18);
	  endTime.SetMin(0);
	  endTime.SetSec(0);

	  theLog << theLogMessage;

	  while(startTime.GetJulianDay() < thePeriod.localEndTime().GetJulianDay())
		{
		  WeatherPeriod fractilePeriod(startTime, endTime);

		  WeatherResult fractile02Temperature = get_fractile_temperature(theVariable,
																		 FRACTILE_02,
																		 theSources,
																		 theArea,
																		 fractilePeriod,
																		 theFractileType);
		  WeatherResult fractile12Temperature = get_fractile_temperature(theVariable,
																		 FRACTILE_12,
																		 theSources,
																		 theArea,
																		 fractilePeriod,
																		 theFractileType);
		  WeatherResult fractile37Temperature = get_fractile_temperature(theVariable,
																		 FRACTILE_37,
																		 theSources,
																		 theArea,
																		 fractilePeriod,
																		 theFractileType);
		  WeatherResult fractile50Temperature = get_fractile_temperature(theVariable,
																		 FRACTILE_50,
																		 theSources,
																		 theArea,
																		 fractilePeriod,
																		 theFractileType);
		  WeatherResult fractile63Temperature = get_fractile_temperature(theVariable,
																		 FRACTILE_63,
																		 theSources,
																		 theArea,
																		 fractilePeriod,
																		 theFractileType);
		  WeatherResult fractile88Temperature = get_fractile_temperature(theVariable,
																		 FRACTILE_88,
																		 theSources,
																		 theArea,
																		 fractilePeriod,
																		 theFractileType);
		  WeatherResult fractile98Temperature = get_fractile_temperature(theVariable,
																		 FRACTILE_98,
																		 theSources,
																		 theArea,
																		 fractilePeriod,
																		 theFractileType);

		  theLog << "date = " << startTime << "..." << endTime << endl;
		  theLog << "F02 = " << fractile02Temperature << endl;
		  theLog << "F12 = " << fractile12Temperature << endl;
		  theLog << "F37 = " << fractile37Temperature << endl;
		  theLog << "F50 = " << fractile50Temperature << endl;
		  theLog << "F63 = " << fractile63Temperature << endl;
		  theLog << "F88 = " << fractile88Temperature << endl;
		  theLog << "F98 = " << fractile98Temperature << endl;

		  startTime.ChangeByDays(1);
		  endTime.ChangeByDays(1);
		}
	}

	const Sentence temperature_anomaly_sentence(temperature_anomaly_params& theParameters,
												const float& fractile02Share,
												const float& fractile12Share,
												const float& fractile88Share,
												const float& fractile98Share,
												const WeatherPeriod& thePeriod)
	{
	  Sentence sentence;

	  Sentence theSpecifiedDay;

	  if(theParameters.thePeriodLength > 24)
		{
		  theSpecifiedDay << PeriodPhraseFactory::create("today",
														 theParameters.theVariable,
														 theParameters.theForecastTime,
														 thePeriod,
														 theParameters.theArea);
		}
	  /*
	  if(theSpecifiedDay.size() == 0)
		theSpecifiedDay << EMPTY_STRING;
	  */
	  if(theSpecifiedDay.size() == 0)
		{
		  sentence << TEMPERATURE_IS_SOMETHING_PLAIN_COMPOSITE_PHRASE;
		}
	  else
		{
		  sentence << TEMPERATURE_IS_SOMETHING_COMPOSITE_PHRASE
				   << theSpecifiedDay;
		}

	  float adequateShare(80.0);

	  if(fractile02Share >= adequateShare)
		{
		  sentence << POIKKEUKSELLISEN_WORD;
		  sentence << (theParameters.theSeason == SUMMER_SEASON ? KOLEAA_WORD : KYLMAA_WORD);

		  theParameters.theAnomalyPhrase = 
			(theParameters.theSeason == SUMMER_SEASON ? SAA_ON_POIKKEUKSELLISEN_KOLEAA :SAA_ON_POIKKEUKSELLISEN_KYLMAA);
		}
	  else if(fractile12Share >= adequateShare)
		{
		  sentence << EMPTY_STRING;
		  if(theParameters.theSeason == SUMMER_SEASON)
			sentence << KOLEAA_WORD;
		  else
			sentence << KYLMAA_WORD;

		  theParameters.theAnomalyPhrase = 
			(theParameters.theSeason == SUMMER_SEASON ? SAA_ON_KOLEAA : SAA_ON_HYVIN_KYLMAA);
		}
	  else if(fractile98Share >= adequateShare)
		{
		  sentence << POIKKEUKSELLISEN_WORD;
		  if(theParameters.theSeason == SUMMER_SEASON)
			sentence << LAMMINTA_WORD;
		  else
			sentence << LEUTOA_WORD;

		  theParameters.theAnomalyPhrase = 
			(theParameters.theSeason == SUMMER_SEASON ? SAA_ON_POIKKEUKSLLISEN_LAMMINTA : SAA_ON_POIKKEUKSLLISEN_LEUTOA);		
		}
	  else if(fractile88Share >= adequateShare)
		{
		  if(theParameters.theSeason == SUMMER_SEASON)
			sentence << HARVINAISEN_WORD << LAMMINTA_WORD;
		  else
			sentence << HYVIN_WORD << LEUTOA_WORD;

		  theParameters.theAnomalyPhrase = 
			(theParameters.theSeason == SUMMER_SEASON ? SAA_ON_HARVINAISEN_LAMMINTA : SAA_ON_HYVIN_LEUTOA);
		}

	  return sentence;
	}

	const Sentence temperature_shortruntrend_sentence(temperature_anomaly_params& theParameters,
													  const fractile_type_id& theFractileType)
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
												  veryColdWeatherPeriod,
												  theFractileType);

	  float veryColdTemperature = wr.value() < VERY_COLD_TEMPERATURE_UPPER_LIMIT ? 
		wr.value() : VERY_COLD_TEMPERATURE_UPPER_LIMIT;


	  WeatherPeriod fractileTemperaturePeriod(get_afternoon_period(theParameters.theVariable,
																   theParameters.theDay1Period.localStartTime()));

	  WeatherResult fractile37Temperature = get_fractile_temperature(theParameters.theVariable,
																	 FRACTILE_37,
																	 theParameters.theSources,
																	 theParameters.theArea,
																	 fractileTemperaturePeriod,
																	 theFractileType);

	  WeatherResult fractile12Temperature = get_fractile_temperature(theParameters.theVariable,
																	 FRACTILE_12,
																	 theParameters.theSources,
																	 theParameters.theArea,
																	 fractileTemperaturePeriod,
																	 theFractileType);

	  WeatherResult fractile63Temperature = get_fractile_temperature(theParameters.theVariable,
																	 FRACTILE_63,
																	 theParameters.theSources,
																	 theParameters.theArea,
																	 fractileTemperaturePeriod,
																	 theFractileType);
	  Sentence theSpecifiedDay;

	  if(theParameters.thePeriodLength > 24)
		{
		  theSpecifiedDay << PeriodPhraseFactory::create("today",
														 theParameters.theVariable,
														 theParameters.theForecastTime,
														 theParameters.theDay2Period,
														 theParameters.theArea);
		}

	  if(theSpecifiedDay.size() == 0)
		theSpecifiedDay << EMPTY_STRING;

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
			  if(period1Temperature > MILD_TEMPERATURE_LOWER_LIMIT && period1Temperature < MILD_TEMPERATURE_UPPER_LIMIT &&
				 period2Temperature > MILD_TEMPERATURE_LOWER_LIMIT && period2Temperature < MILD_TEMPERATURE_UPPER_LIMIT &&
				 period2Temperature > fractile63Temperature.value())
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << SAA_ON_EDELLEEN_LAUHAA_PHRASE;
  
				  theParameters.theShortrunTrend = SAA_ON_EDELLEEN_LAUHAA;
				}
			  else if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT 
					  && period1Temperature < MILD_TEMPERATURE_LOWER_LIMIT && 
					  period2Temperature >= MILD_TEMPERATURE_LOWER_LIMIT && 
					  period2Temperature < MILD_TEMPERATURE_UPPER_LIMIT)
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << SAA_LAUHTUU_PHRASE;

				  theParameters.theShortrunTrend = SAA_LAUHTUU;
				}
			  else if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT
					  && period1Temperature <= veryColdTemperature &&
					  period2Temperature <= MILD_TEMPERATURE_LOWER_LIMIT)
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << KIREA_PAKKANEN_HEIKKENEE_PHRASE;

				  theParameters.theShortrunTrend = KIREA_PAKKANEN_HEIKKENEE;
				}
			  else if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT
					  && period1Temperature > veryColdTemperature &&
					  period2Temperature < MILD_TEMPERATURE_LOWER_LIMIT)
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << PAKKANEN_HEIKKENEE_PHRASE;

				  theParameters.theShortrunTrend = PAKKANEN_HEIKKENEE;
				}
			  else if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT
					  && period1Temperature > veryColdTemperature && period1Temperature < MILD_TEMPERATURE_LOWER_LIMIT && 
					  period2Temperature < ZERO_DEGREES && period2Temperature >= MILD_TEMPERATURE_LOWER_LIMIT)
				{
				  // redundant: this will never happen, because "s‰‰ lauhtuu" is tested before
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << PAKKANEN_HELLITTAA_PHRASE;

				  theParameters.theShortrunTrend = PAKKANEN_HELLITTAA;
				}
			  else if(period1Temperature < veryColdTemperature &&
					  period2Temperature < veryColdTemperature)
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << KIREA_PAKKANEN_JATKUU_PHRASE;

				  theParameters.theShortrunTrend = KIREA_PAKKANEN_JATKUU;
				}
			}
		  else if(period2Temperature <= period1Temperature)
			{
			  if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT
				 && period1Temperature < MILD_TEMPERATURE_LOWER_LIMIT &&
				 period2Temperature <= veryColdTemperature)
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << PAKKANEN_KIRISTYY_PHRASE;

				  theParameters.theShortrunTrend = PAKKANEN_KIRISTYY;
				}
			  else if(period1Temperature < veryColdTemperature &&
					  period2Temperature < veryColdTemperature)
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << KIREA_PAKKANEN_JATKUU_PHRASE;

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
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << HELTEINEN_SAA_JATKUU_PHRASE;

				  theParameters.theShortrunTrend = HELTEINEN_SAA_JATKUU;
				}
			  else if(period1Temperature < fractile12Temperature.value() && 
					  period2Temperature < fractile12Temperature.value())
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << KOLEA_SAA_JATKUU_PHRASE;

				  theParameters.theShortrunTrend = KOLEA_SAA_JATKUU;
				}
			  else if(period1Temperature < fractile37Temperature.value() && 
					  period2Temperature < fractile37Temperature.value())
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << VIILEA_SAA_JATKUU_PHRASE;

				  theParameters.theShortrunTrend = VIILEA_SAA_JATKUU;
				}
			  else if(period1Temperature < hot_weather_limit && period2Temperature >= hot_weather_limit)
				{
				  if(temperatureDifference >= NOTABLE_TEMPERATURE_CHANGE_LIMIT)
					{
					  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
							   << theSpecifiedDay 
							   << SAA_MUUTTUU_HELTEISEKSI_PHRASE;

					  theParameters.theShortrunTrend = SAA_MUUTTUU_HELTEISEKSI;
					}
				  else
					{
					  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
							   << theSpecifiedDay 
							   << SAA_ON_HELTEISTA_PHRASE;

					  theParameters.theShortrunTrend = SAA_ON_HELTEISTA;
					}
				}
			  else if(signifigantChange)
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << SAA_LAMPENEE_HUOMATTAVASTI_PHRASE;

				  theParameters.theShortrunTrend = SAA_LAMPENEE_HUOMATTAVASTI;
				}
			  else if(moderateChange)
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << SAA_LAMPENEE_PHRASE;

				  theParameters.theShortrunTrend = SAA_LAMPENEE;
				}
			  else if(smallChange)
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << SAA_LAMPENEE_VAHAN_PHRASE;

				  theParameters.theShortrunTrend = SAA_LAMPENEE_VAHAN;
				}
			}
		  else if(period2Temperature <= period1Temperature && period2Temperature > ZERO_DEGREES)
			{
			  if(period1Temperature >= hot_weather_limit && period2Temperature >= hot_weather_limit)
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << HELTEINEN_SAA_JATKUU_PHRASE;

				  theParameters.theShortrunTrend = HELTEINEN_SAA_JATKUU;
				}
			  else if(period1Temperature < fractile12Temperature.value() && 
					  period2Temperature < fractile12Temperature.value())
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << KOLEA_SAA_JATKUU_PHRASE;

				  theParameters.theShortrunTrend = KOLEA_SAA_JATKUU;
				}
			  else if(period1Temperature < fractile37Temperature.value() && 
					  period2Temperature < fractile37Temperature.value())
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << VIILEA_SAA_JATKUU_PHRASE;

				  theParameters.theShortrunTrend = VIILEA_SAA_JATKUU;
				}
			  else if(signifigantChange)
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << SAA_VIILENEE_HUOMATTAVASTI_PHRASE;

				  theParameters.theShortrunTrend = SAA_VIILENEE_HUOMATTAVASTI;
				}
			  else if(moderateChange)
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << SAA_VIILENEE_PHRASE;

				  theParameters.theShortrunTrend = SAA_VIILENEE;
				}
			  else if(smallChange && period2Temperature < GETTING_COOLER_NOTIFICATION_LIMIT)
				{
				  sentence << TEMPERATURE_CHANGES_COMPOSITE_PHRASE 
						   << theSpecifiedDay 
						   << SAA_VIILENEE_VAHAN_PHRASE;

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

	std::string areaName("");

	if(itsArea.isNamed())
	  {
		areaName = itsArea.name();
		log <<  areaName << endl;
	  }

	forecast_season_id theSeasonId = isSummerHalf(itsPeriod.localStartTime(), itsVar) ? SUMMER_SEASON : WINTER_SEASON;

	// the anomaly sentence relates always to the upcoming day,
	// so the period is defined to start in the previous day	

	log_start_time_and_end_time(log, 
								"the original period: ",
								itsPeriod);

	NFmiTime periodStartTime(itsPeriod.localStartTime());
	NFmiTime periodEndTime(itsPeriod.localEndTime());
	int periodLength = periodEndTime.DifferenceInHours(periodStartTime);

	int ndays = HourPeriodGenerator(itsPeriod, itsVar+"::day").size();

	log << "Period " << itsPeriod.localStartTime() << "..." 
		<< itsPeriod.localEndTime() << " covers " << ndays << " days" << endl;

	if(ndays <= 0)
	  {
		log << paragraph;
		return paragraph;
	  }
	else if(ndays == 1)
	  {
		periodStartTime.ChangeByDays(-1);
	  }


	WeatherPeriod theExtendedPeriod(periodStartTime, periodEndTime);

	HourPeriodGenerator generator(theExtendedPeriod, itsVar+"::day");
	for(unsigned int i = 0; i < generator.size(); i++ )
	  {
	  log_start_time_and_end_time(log, 
								  "The extended period: ",
								  generator.period(i+1));
	  }

	WeatherPeriod day1Period(generator.period(1));
	WeatherPeriod day2Period(generator.period(2));

	bool report_day2_anomaly = true;



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

	fractile_type_id fractileType(MAX_FRACTILE);

	WeatherPeriod fractileTemperatureDay1Period(get_afternoon_period(parameters.theVariable,
																	 parameters.theDay1Period.localStartTime()));
	WeatherPeriod fractileTemperatureDay2Period(get_afternoon_period(parameters.theVariable,
																	 parameters.theDay2Period.localStartTime()));



	fractile_id theFractileDay1 = get_fractile(itsVar,
											   parameters.theDay1TemperatureAreaAfternoonMaximum.value(),
											   itsSources,
											   itsArea,
											   fractileTemperatureDay1Period,
											   fractileType);

	fractile_id theFractileDay2 = get_fractile(itsVar,
											   parameters.theDay2TemperatureAreaAfternoonMaximum.value(),
											   itsSources,
											   itsArea,
											   fractileTemperatureDay2Period,
											   fractileType);

	WeatherResult fractileTemperatureDay1 = get_fractile_temperature(itsVar,
																	 theFractileDay1,
																	 itsSources,
																	 itsArea,  
																	 fractileTemperatureDay1Period,
																	 fractileType);

	WeatherResult fractileTemperatureDay2 = get_fractile_temperature(itsVar,
																	 theFractileDay2,
																	 itsSources,
																	 itsArea,  
																	 fractileTemperatureDay2Period,
																	 fractileType);

	WeatherPeriod fractileTemperaturePeriod(report_day2_anomaly ? fractileTemperatureDay2Period : fractileTemperatureDay1Period);

	log << "Day1 maximum temperature " 
		<< parameters.theDay1TemperatureAreaAfternoonMaximum
		<< " falls into fractile " 
		<< fractile_name(theFractileDay1) << fractileTemperatureDay1 << endl;
	log << "Day2 maximum temperature " 
		<< parameters.theDay2TemperatureAreaAfternoonMaximum
		<< " falls into fractile " 
		<< fractile_name(theFractileDay2) << fractileTemperatureDay2 << endl;

	
	WeatherResult fractile02Temperature = get_fractile_temperature(itsVar,
																   FRACTILE_02,
																   itsSources,
																   itsArea,
																   fractileTemperaturePeriod,
																   fractileType);

	WeatherResult fractile12Temperature = get_fractile_temperature(itsVar,
																   FRACTILE_12,
																   itsSources,
																   itsArea,
																   fractileTemperaturePeriod,
																   fractileType);

	WeatherResult fractile88Temperature = get_fractile_temperature(itsVar,
																   FRACTILE_88,
																   itsSources,
																   itsArea,
																   fractileTemperaturePeriod,
																   fractileType);

	WeatherResult fractile98Temperature = get_fractile_temperature(itsVar,
																   FRACTILE_98,
																   itsSources,
																   itsArea,
																   fractileTemperaturePeriod,
																   fractileType);

	GridForecaster theForecaster;
	RangeAcceptor upperLimitF02Acceptor;
	upperLimitF02Acceptor.upperLimit(fractile02Temperature.value());
	RangeAcceptor upperLimitF12Acceptor;
	upperLimitF12Acceptor.upperLimit(fractile12Temperature.value());
	RangeAcceptor lowerLimitF88Acceptor;
	lowerLimitF88Acceptor.lowerLimit(fractile88Temperature.value());
	RangeAcceptor lowerLimitF98Acceptor;
	lowerLimitF98Acceptor.lowerLimit(fractile98Temperature.value());

	/*
	  NFmiSettings::Set("anomaly::fake::fractile::share::F02", "0, 0");
	  NFmiSettings::Set("anomaly::fake::fractile::share::F12", "0, 0");
	  NFmiSettings::Set("anomaly::fake::fractile::share::F88", "0, 0");
	  NFmiSettings::Set("anomaly::fake::fractile::share::F98", "0, 0");
	*/

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

	log << "Fractile 02 temperature and share (" << areaName << "): " << fractile02Temperature << "; " << fractile02Share << endl;
	log << "Fractile 12 temperature and share (" << areaName << "): " << fractile12Temperature << "; " << fractile12Share << endl;
	log << "Fractile 88 temperature and share (" << areaName << "): " << fractile88Temperature << "; " << fractile88Share << endl;
	log << "Fractile 98 temperature and share (" << areaName << "): " << fractile98Temperature << "; " << fractile98Share << endl;

	//	Paragraph paragraphDev;
	Sentence temperatureAnomalySentence;
	Sentence shortrunTrendSentence;

	temperatureAnomalySentence << temperature_anomaly_sentence(parameters,
															   fractile02Share.value(),
															   fractile12Share.value(),
															   fractile88Share.value(),
															   fractile98Share.value(),
															   report_day2_anomaly ? 
															   parameters.theDay2Period : parameters.theDay1Period);

	shortrunTrendSentence <<  temperature_shortruntrend_sentence(parameters, fractileType);

	log_data(parameters);

	paragraph << handle_anomaly_and_shortrun_trend_sentences(parameters, 
															 temperatureAnomalySentence, 
															 shortrunTrendSentence);

	log << paragraph;

	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================

  
