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
	using MathTools::to_precision;
	using Settings::optional_int;
	using Settings::optional_bool;
	using Settings::optional_string;

#define WEAK_FROST_TEMPERATURE -5.0
#define VERY_HOT_TEMPERATURE +25.0
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

	enum sentence_id {
	  SAA_ON_POIKKEUKSELLISEN_KOLEAA_SENTENCE = 0x1,
	  SAA_ON_POIKKEUKSELLISEN_KYLMAA_SENTENCE,
	  SAA_ON_KOLEAA_SENTENCE,
	  SAA_ON_HYVIN_KYLMAA_SENTENCE,
	  SAA_ON_HARVINAISEN_LAMMINTA_SENTENCE,
	  SAA_ON_HYVIN_LEUTOA_SENTENCE,
	  SAA_ON_POIKKEUKSELLISEN_LAMMINTA_SENTENCE,
	  SAA_ON_POIKKEUKSELLISEN_LEUTOA_SENTENCE,
	  SAA_ON_EDELLEEN_LAUHAA_SENTENCE,
	  SAA_LAUHTUU_SENTENCE,
	  KIREA_PAKKANEN_HEIKKENEE_SENTENCE,
	  KIREA_PAKKANEN_HELLITTAA_SENTENCE,
	  PAKKANEN_HEIKKENEE_SENTENCE,
	  PAKKANEN_HELLITTAA_SENTENCE,
	  KIREA_PAKKANEN_JATKUU_SENTENCE,
	  PAKKANEN_KIRISTYY_SENTENCE,
	  HELTEINEN_SAA_JATKUU_SENTENCE,
	  VIILEA_SAA_JATKUU_SENTENCE,
	  KOLEA_SAA_JATKUU_SENTENCE,
	  SAA_MUUTTUU_HELTEISEKSI_SENTENCE,
	  SAA_ON_HELTEISTA_SENTENCE,
	  SAA_LAMPENEE_VAHAN_SENTENCE,
	  SAA_LAMPENEE_SENTENCE,
	  SAA_LAMPENEE_HUOMATTAVASTI_SENTENCE,
	  SAA_VIILENEE_VAHAN_SENTENCE,
	  SAA_VIILENEE_SENTENCE,
	  SAA_VIILENEE_HUOMATTAVASTI_SENTENCE,
	  UNDEFINED_SENTENCE_ID
	};


	typedef map<int, Sentence*> anomaly_container_type;
	typedef anomaly_container_type::value_type value_type;


	struct anomaly_temperature_params
	{
	  anomaly_temperature_params(const string& variable,
								 const AnalysisSources& analysisSources,
								 const WeatherArea& weatherArea,
								 const WeatherPeriod& weatherPeriod,
								 const forecast_season_id& seasonId,
								 const fractile_id& fractile,
								 const WeatherResult& day1Temperature,
								 const WeatherResult& day2Temperature,
								 anomaly_container_type& sentences,
								 MessageLogger& log) :
		theVariable(variable),
		theAnalysisSources(analysisSources),
		theWeatherArea(weatherArea),
		theWeatherPeriod(weatherPeriod),
		theSeasonId(seasonId),
		theFractile(fractile),
		theDay1Temperature(day1Temperature),
		theDay2Temperature(day2Temperature),
		theSentences(sentences),
		theLog(log)
	  {}
	  const string& theVariable;
	  const AnalysisSources& theAnalysisSources;
	  const WeatherArea& theWeatherArea;
	  const WeatherPeriod& theWeatherPeriod;
	  const forecast_season_id& theSeasonId;
	  const fractile_id& theFractile;
	  const WeatherResult& theDay1Temperature;
	  const WeatherResult& theDay2Temperature;
	  anomaly_container_type& theSentences;
	  MessageLogger& theLog;
	};

	void calculate_temperature(MessageLogger& theLog,
						   const string& theVar,
						   const AnalysisSources& theSources,
						   const WeatherArea& theArea,
						   const WeatherPeriod& thePeriod,
						   const forecast_season_id& theSeasonId,
						   WeatherResult& theMinimum,
						   WeatherResult& theMean,
						   WeatherResult& theMaximum)
	{	  
	  GridForecaster theForecaster;

	  if(theSeasonId == SUMMER_SEASON)
		{
		  theMinimum = theForecaster.analyze(theVar + "::min",
											 theSources,
											 Temperature,
											 Minimum,
											 Maximum,
											 theArea,
											 thePeriod);

		  theMaximum = theForecaster.analyze(theVar + "::max",
											 theSources,
											 Temperature,
											 Maximum,
											 Maximum,
											 theArea,
											 thePeriod);

		  theMean = theForecaster.analyze(theVar + "::mean",
											 theSources,
											 Temperature,
											 Mean,
											 Maximum,
											 theArea,
											 thePeriod);
		}
	  else
		{
		  theMinimum = theForecaster.analyze(theVar + "::min",
											 theSources,
											 Temperature,
											 Minimum,
											 Mean,
											 theArea,
											 thePeriod);

		  theMaximum = theForecaster.analyze(theVar + "::max",
											 theSources,
											 Temperature,
											 Maximum,
											 Mean,
											 theArea,
											 thePeriod);
		  
		  theMean = theForecaster.analyze(theVar + "::mean",
											 theSources,
											 Temperature,
											 Mean,
											 Mean,
											 theArea,
											 thePeriod);
		}
	}

	void calculate_windspeed(MessageLogger& theLog,
							 const string& theVariable,
							 const AnalysisSources& theSources,
							 const WeatherArea& theArea,
							 const WeatherPeriod& thePeriod,
							 WeatherResult& theMinimum,
							 WeatherResult& theMean,
							 WeatherResult& theMaximum)
	{
	  GridForecaster theForecaster;
	  theMinimum = theForecaster.analyze(theVariable+"::min",
										 theSources,
										 WindSpeed,
										 Minimum,
										 Maximum,
										 theArea,
										 thePeriod);
	  theMaximum = theForecaster.analyze(theVariable+"::max",
										 theSources,
										 WindSpeed,
										 Maximum,
										 Maximum,
										 theArea,
										 thePeriod);
	  theMean = theForecaster.analyze(theVariable+"::mean",
									  theSources,
									  WindSpeed,
									  Mean,
									  Maximum,
									  theArea,
									  thePeriod);
	}

	void calculate_windchill(MessageLogger& theLog,
							 const string& theVariable,
							 const AnalysisSources& theSources,
							 const WeatherArea& theArea,
							 const WeatherPeriod& thePeriod,
							 WeatherResult& theMinimum,
							 WeatherResult& theMean,
							 WeatherResult& theMaximum)
	{
	  GridForecaster theForecaster;
	  theMinimum = theForecaster.analyze(theVariable+"::min",
										 theSources,
										 WindChill,
										 Minimum,
										 Maximum,
										 theArea,
										 thePeriod);
	  theMaximum = theForecaster.analyze(theVariable+"::max",
										 theSources,
										 WindChill,
										 Maximum,
										 Maximum,
										 theArea,
										 thePeriod);
	  theMean = theForecaster.analyze(theVariable+"::mean",
									  theSources,
									  WindChill,
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

	const Sentence temperature_anomaly_sentence(const std::string theVariable,
												const forecast_season_id& theSeasonId,
												const fractile_id& theFractile)
	{
	  Sentence retval;

	  switch(theFractile)
		{
		case FRACTILE_02:
		  {
			retval << SAA_WORD << ON_WORD << POIKKEUKSELLISEN_WORD;
			retval << (theSeasonId == SUMMER_SEASON ? KOLEAA_WORD : KYLMAA_WORD);
		  }
		  break;
		case FRACTILE_12:
		  {
			retval << SAA_WORD << ON_WORD;
			if(theSeasonId == SUMMER_SEASON)
			  retval << KOLEAA_WORD;
			else
			  retval << HYVIN_WORD << KYLMAA_WORD;
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
			retval << SAA_WORD << ON_WORD;
			if(theSeasonId == SUMMER_SEASON)
			  retval << POIKKEUKSELLISEN_WORD << LAMMINTA_WORD;
			else
			  retval << HYVIN_WORD << LEUTOA_WORD;
		  }
		  break;
		case FRACTILE_100:
		  {
			retval << SAA_WORD << ON_WORD << POIKKEUKSELLISEN_WORD;
			if(theSeasonId == SUMMER_SEASON)
			  retval << LAMMINTA_WORD;
			else
			  retval << LEUTOA_WORD;
		  }
		  break;
		case FRACTILE_UNDEFINED:
		  break;		}

	  return retval;
	}

	/*
	const Sentence temperature_anomaly_sentence(anomaly_temperature_params& theParameters)
	{
	  Sentence retval;

	  switch(theParameters.theFractile)
		{
		case FRACTILE_02:
		  {
			if(theParameters.theSeasonId == SUMMER_SEASON)
			  {
				*(theParameters.theSentences[SAA_ON_POIKKEUKSELLISEN_KOLEAA_SENTENCE]) << 
				  SAA_WORD << 
				  ON_WORD << 
				  POIKKEUKSELLISEN_WORD << 
				  KOLEAA_WORD;
			  }
			else
			  {
				*(theParameters.theSentences[SAA_ON_POIKKEUKSELLISEN_KYLMAA_SENTENCE]) << 
				  SAA_WORD << 
				  ON_WORD << 
				  POIKKEUKSELLISEN_WORD << 
				  KYLMAA_WORD;
			  }
		  }
		  break;
		case FRACTILE_12:
		  {
			if(theParameters.theSeasonId == SUMMER_SEASON)
			  {
				*(theParameters.theSentences[SAA_ON_KOLEAA_SENTENCE]) << 
				  SAA_WORD << 
				  ON_WORD << 
				  KOLEAA_WORD;
			  }
			else
			  {
				*(theParameters.theSentences[SAA_ON_HYVIN_KYLMAA_SENTENCE]) << 
				  SAA_WORD << 
				  ON_WORD << 
				  HYVIN_WORD <<
				  KYLMAA_WORD;
			  }
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
			if(theParameters.theSeasonId == SUMMER_SEASON)
			  {
				*(theParameters.theSentences[SAA_ON_HARVINAISEN_LAMMINTA_SENTENCE]) << 
				  SAA_WORD << 
				  ON_WORD << 
				  HARVINAISEN_WORD <<
				  LAMMINTA_WORD;
			  }
			else
			  {
				*(theParameters.theSentences[SAA_ON_HYVIN_LEUTOA_SENTENCE]) << 
				  SAA_WORD << 
				  ON_WORD << 
				  HYVIN_WORD <<
				  LEUTOA_WORD;
			  }
		  }
		  break;
		case FRACTILE_100:
		  {
			if(theParameters.theSeasonId == SUMMER_SEASON)
			  {
				*(theParameters.theSentences[SAA_ON_POIKKEUKSELLISEN_LAMMINTA_SENTENCE]) << 
				  SAA_WORD << 
				  ON_WORD << 
				  POIKKEUKSELLISEN_WORD <<
				  LAMMINTA_WORD;
			  }
			else
			  {
				*(theParameters.theSentences[SAA_ON_POIKKEUKSELLISEN_LEUTOA_SENTENCE]) << 
				  SAA_WORD << 
				  ON_WORD << 
				  POIKKEUKSELLISEN_WORD <<
				  LEUTOA_WORD;
			  }
		  }
		  break;
		case FRACTILE_UNDEFINED:
		  break;		}

	  return retval;
	}
	*/
	const Sentence temperature_shortruntrend_sentence(const std::string theVariable,
													  const NFmiTime& theForecastTime,
													  const AnalysisSources& theAnalysisSources,
													  const WeatherArea& theWeatherArea,
													  const WeatherResult& thePeriod1Temperature,
													  const WeatherResult& thePeriod2Temperature,
													  const forecast_season_id& theSeasonId)
	{
	  Sentence sentence;

	  double period1Temperature = thePeriod1Temperature.value();
	  double period2Temperature = thePeriod2Temperature.value();


	  // kova pakkanen: F12,5 fractile on 1. Feb 12:00
	  NFmiTime veryColdRefTime(theForecastTime.GetYear(), 2, 1, 12, 0, 0);
	  WeatherPeriod veryColdWeatherPeriod(veryColdRefTime, veryColdRefTime);

	  WeatherResult wr = get_fractile_temperature(theVariable,
												  FRACTILE_12,
												  theAnalysisSources,
												  theWeatherArea,
												  veryColdWeatherPeriod);

	  float veryColdTemperature = wr.value() < VERY_COLD_TEMPERATURE_UPPER_LIMIT ? 
		wr.value() : VERY_COLD_TEMPERATURE_UPPER_LIMIT;

	  WeatherPeriod forecastTimeWeatherPeriod(NFmiTime(theForecastTime.GetYear(),
													   theForecastTime.GetMonth(),
													   theForecastTime.GetDay(),
													   6, 0, 0),
											  NFmiTime(theForecastTime.GetYear(),
													   theForecastTime.GetMonth(),
													   theForecastTime.GetDay(),
													   18, 0, 0));

	  WeatherResult fractile37Temperature = get_fractile_temperature(theVariable,
																	 FRACTILE_37,
																	 theAnalysisSources,
																	 theWeatherArea,
																	 forecastTimeWeatherPeriod);
	  WeatherResult fractile12Temperature = get_fractile_temperature(theVariable,
																	 FRACTILE_12,
																	 theAnalysisSources,
																	 theWeatherArea,
																	 forecastTimeWeatherPeriod);

	  if(theSeasonId == WINTER_SEASON)
		{
		  // pakkanen kiristyy
		  // kire‰ pakkanen jatkuu
		  // kire‰ pakkanen heikkenee
		  // pakkanen heikkenee
		  // pakkanen hellitt‰‰
		  // s‰‰ lauhtuu


		  float temperatureDifference = abs(period2Temperature - period1Temperature);
		  if(period2Temperature >= period1Temperature)
			{
			  if(period1Temperature > WEAK_FROST_TEMPERATURE && period1Temperature < LOW_PLUS_TEMPARATURE &&
				 period2Temperature > WEAK_FROST_TEMPERATURE && period2Temperature < LOW_PLUS_TEMPARATURE)
				{
				  sentence << SAA_ON_EDELLEEN_LAUHAA_PHRASE;
				}
			  else if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT 
					  && period1Temperature < WEAK_FROST_TEMPERATURE &&
				 period2Temperature > WEAK_FROST_TEMPERATURE)
				{
				  sentence << SAA_LAUHTUU_PHRASE;
				}
			  else if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT
					  && period1Temperature <= veryColdTemperature &&
					  period2Temperature <= WEAK_FROST_TEMPERATURE)
				{
				  sentence << KIREA_WORD << PAKKANEN_HEIKKENEE_PHRASE;
				}
			  else if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT
					  && period1Temperature <= veryColdTemperature &&
					  period2Temperature < ZERO_DEGREES && period2Temperature > WEAK_FROST_TEMPERATURE)
				{
				  sentence << KIREA_WORD << PAKKANEN_HELLITTAA_PHRASE;
				}
			  else if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT
					  && period1Temperature > veryColdTemperature &&
					  period2Temperature < WEAK_FROST_TEMPERATURE)
				{
				  sentence << PAKKANEN_HEIKKENEE_PHRASE;
				}
			  else if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT
					  && period1Temperature > veryColdTemperature &&
					  period1Temperature < WEAK_FROST_TEMPERATURE && period2Temperature < ZERO_DEGREES &&
					  period2Temperature > WEAK_FROST_TEMPERATURE)
				{
				  sentence << PAKKANEN_HELLITTAA_PHRASE;
				}
			  else if(period1Temperature < veryColdTemperature &&
					  period2Temperature < veryColdTemperature)
				{
				  sentence << KIREA_PAKKANEN_JATKUU_PHRASE;
				}

			}
		  else if(period2Temperature <= period1Temperature)
			{
			  if(temperatureDifference >= SIGNIFIGANT_CHANGE_LOWER_LIMIT
				 && period1Temperature < WEAK_FROST_TEMPERATURE &&
				 period2Temperature <= veryColdTemperature)
				{
				  sentence << PAKKANEN_KIRISTYY_PHRASE;
				}
			  else if(period1Temperature < veryColdTemperature &&
					  period2Temperature < veryColdTemperature)
				{
				  sentence << KIREA_PAKKANEN_JATKUU_PHRASE;
				}
			}
		}
	  else
		{
		  double hot_weather_limit = Settings::optional_double(theVariable+"::hot_weather_limit::default", 25);

		  if(theWeatherArea.isNamed())
			{
			  std::string parameter_name(theVariable + "::hot_weather_limit::" + theWeatherArea.name());
			  if(Settings::isset(parameter_name))
				hot_weather_limit = Settings::require_double(parameter_name);
			}

		  // s‰‰ muuttuu helteiseksi
		  // helteinen s‰‰ jatkuu
		  // viile‰ s‰‰ jatkuu
		  // kolea s‰‰ jatkuu
		  // s‰‰ viilenee v‰h‰n
		  // s‰‰ viilenee
		  // s‰‰ viilenee huomattavasti
		  // s‰‰ l‰mpenee v‰h‰n
		  // s‰‰ l‰mpenee
		  // s‰‰ l‰mpenee huomattavasti


		  float temperatureDifference = abs(period2Temperature - period1Temperature);
		  bool smallChange =  temperatureDifference >= SMALL_CHANGE_LOWER_LIMIT && 
			temperatureDifference < SMALL_CHANGE_UPPER_LIMIT;
		  bool moderateChange =  temperatureDifference >= MODERATE_CHANGE_LOWER_LIMIT && 
			temperatureDifference <= MODERATE_CHANGE_UPPER_LIMIT;
		  bool signifigantChange =  temperatureDifference > SIGNIFIGANT_CHANGE_LOWER_LIMIT;

		  if(period2Temperature >= period1Temperature)
			{
			  if(period1Temperature >= VERY_HOT_TEMPERATURE && period2Temperature >= VERY_HOT_TEMPERATURE)
				{
				  sentence << HELTEINEN_SAA_JATKUU_PHRASE;
				}
			  else if(period1Temperature < fractile37Temperature.value() && 
					  period2Temperature < fractile37Temperature.value())
				{
				  sentence << VIILEA_SAA_JATKUU_PHRASE;
				}
			  else if(period1Temperature < fractile12Temperature.value() && 
					  period2Temperature < fractile12Temperature.value())
				{
				  sentence << KOLEA_SAA_JATKUU_PHRASE;
				}
			  else if(period1Temperature < VERY_HOT_TEMPERATURE && period2Temperature >= VERY_HOT_TEMPERATURE)
				{
				  if(temperatureDifference > NOTABLE_TEMPERATURE_CHANGE_LIMIT)
					sentence << SAA_MUUTTUU_HELTEISEKSI_PHRASE;
				  else
					sentence << SAA_ON_HELTEISTA_PHRASE;
				}
			  else if(smallChange)
				{
				  sentence << SAA_LAMPENEE_PHRASE << VAHAN_WORD;
				}
			  else if(moderateChange)
				{
				  sentence << SAA_LAMPENEE_PHRASE;
				}
			  else if(signifigantChange)
				{
				  sentence << SAA_LAMPENEE_PHRASE << HUOMATTAVASTI_WORD;
				}
			}
		  else if(period2Temperature <= period1Temperature && period2Temperature > ZERO_DEGREES)
			{
			  if(period1Temperature >= VERY_HOT_TEMPERATURE && period2Temperature >= VERY_HOT_TEMPERATURE)
				{
				  sentence << HELTEINEN_SAA_JATKUU_PHRASE;
				}
			  else if(period1Temperature < fractile37Temperature.value() && 
					  period2Temperature < fractile37Temperature.value())
				{
				  sentence << VIILEA_SAA_JATKUU_PHRASE;
				}
			  else if(period1Temperature < fractile12Temperature.value() && 
					  period2Temperature < fractile12Temperature.value())
				{
				  sentence << KOLEA_SAA_JATKUU_PHRASE;
				}
			  else if(smallChange)
				{
				  sentence << SAA_VIILENEE_PHRASE << VAHAN_WORD;
				}
			  else if(moderateChange)
				{
				  sentence << SAA_VIILENEE_PHRASE;
				}
			  else if(signifigantChange)
				{
				  sentence << SAA_VIILENEE_PHRASE << HUOMATTAVASTI_WORD;
				}
			}
		}

	return sentence;
	}

	const Sentence windy_sentence(const std::string theVariable,
								  const WeatherResult& windSpeedMorningMinimum,
								  const WeatherResult& windSpeedMorningMean,
								  const WeatherResult& windSpeedMorningMaximum,
								  const WeatherResult& windSpeedAfternoonMinimum,
								  const WeatherResult& windSpeedAfternoonMean,
								  const WeatherResult& windSpeedAfternoonMaximum)
	{
	  Sentence sentence;

	  float windSpeed = windSpeedMorningMean.value() > windSpeedAfternoonMean.value() ? 
		windSpeedMorningMean.value() : windSpeedAfternoonMean.value();

	  bool windyMorning = windSpeedMorningMean.value() >= 8.0;
	  bool windyAfternoon = windSpeedAfternoonMean.value() >= 8.0;

	  if(windSpeed >= 8.0 && windSpeed <= 10.0)
		{
		  if(windyMorning && !windyAfternoon)
			sentence << "s‰‰ on aamup‰iv‰ll‰ tuulista";
		  else if(!windyMorning && windyAfternoon)
			sentence << "s‰‰ on iltap‰iv‰ll‰ tuulista";
		  else
			sentence << "s‰‰ on tuulista";
		}
	  else if(windSpeed > 10)
		{
		  if(windyMorning && !windyAfternoon)
			sentence << "s‰‰ on aamup‰iv‰ll‰ eritt‰in tuulista";
		  else if(!windyMorning && windyAfternoon)
			sentence << "s‰‰ on iltap‰iv‰ll‰ eritt‰in tuulista";
		  else
			sentence << "s‰‰ on eritt‰in tuulista";
		}

	  // TODO: voiko sanoa s‰‰ muuttuua tuuliseksi
	  
	  return sentence;
	}

	const Sentence windchill_sentence(const std::string theVariable, const WeatherResult& day2WindchillMean)
	{
	  Sentence sentence;
	  
	  float windChillValue = day2WindchillMean.value();

	  if(windChillValue <= -25 && windChillValue >= -35)
		sentence << "pakkanen on purevaa";
	  else if(windChillValue <= -35 && windChillValue <= -60)
		sentence << "pakkanen on eritt‰in purevaa";
	  else if(windChillValue < -60)
		sentence << "pakkanen on vaarallisen purevaa";
	  
	  return sentence;
	}


	int count_dayperiods(const string& theVar, const WeatherPeriod& theWeatherPeriod, NFmiTime& theDayperiodStart)
	{	  
	  NightAndDayPeriodGenerator generator(theWeatherPeriod, theVar);

	  int iDayPeriods = generator.size();
	  iDayPeriods = 0;
	  for(unsigned int i = 0; i < generator.size(); i++ )
		{
		  if(generator.isday(i+1))
			{
			  // set starttime of the first day period
			  if(iDayPeriods == 0)
				theDayperiodStart = generator.period(i+1).localStartTime();
			  iDayPeriods++;
			}
		}

	  return iDayPeriods;
	}

	WeatherPeriod extend_period(const string& theVar, const WeatherPeriod& theWeatherPeriod,MessageLogger& log)
	{
	  log_start_time_and_end_time(log, 
								  "original period: ",
								  theWeatherPeriod);
	  
	  NFmiTime dayPeriodStartTime;
	  int iDayPeriods = count_dayperiods(theVar, theWeatherPeriod, dayPeriodStartTime);

	  if(iDayPeriods >= 2)
		{
		  WeatherPeriod retval(theWeatherPeriod);
		  return retval;
		}
	  else
		{
		  const int starthour  = Settings::require_hour(theVar+"::day::starthour");

		  dayPeriodStartTime.ChangeByHours(-24);
		  while(dayPeriodStartTime.GetHour() != starthour)
			dayPeriodStartTime.ChangeByHours(-1);
		  WeatherPeriod retval(dayPeriodStartTime, theWeatherPeriod.localEndTime());
		  return retval;
		}
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

	/*
	  NFmiSettings::Set("textgen::part1::story::temperature_anomaly::day::starthour","21");
	  NFmiSettings::Set("textgen::part1::story::temperature_anomaly::day::endhour","6");
	*/

	WeatherResult day1TemperatureMinimum(kFloatMissing, 0);
	WeatherResult day1TemperatureMean(kFloatMissing, 0);
	WeatherResult day1TemperatureMaximum(kFloatMissing, 0);	
	WeatherResult nightTemperatureMinimum(kFloatMissing, 0);
	WeatherResult nightTemperatureMean(kFloatMissing, 0);
	WeatherResult nightTemperatureMaximum(kFloatMissing, 0);
	WeatherResult day2TemperatureMinimum(kFloatMissing, 0);
	WeatherResult day2TemperatureMean(kFloatMissing, 0);
	WeatherResult day2TemperatureMaximum(kFloatMissing, 0);
	WeatherResult day2WindspeedMorningMinimum(kFloatMissing, 0);
	WeatherResult day2WindspeedMorningMean(kFloatMissing, 0);
	WeatherResult day2WindspeedMorningMaximum(kFloatMissing, 0);
	WeatherResult day2WindspeedAfternoonMinimum(kFloatMissing, 0);
	WeatherResult day2WindspeedAfternoonMean(kFloatMissing, 0);
	WeatherResult day2WindspeedAfternoonMaximum(kFloatMissing, 0);
	WeatherResult day2WindchillMinimum(kFloatMissing, 0);
	WeatherResult day2WindchillMean(kFloatMissing, 0);
	WeatherResult day2WindchillMaximum(kFloatMissing, 0);
	bool handleWindchill = false;

	forecast_season_id theSeasonId = isSummerHalf(itsPeriod.localStartTime(), itsVar) ? SUMMER_SEASON : WINTER_SEASON;

	// the anomaly sentence relates always the the upcoming day,
	// so the period is defined to start in the previous day	

	log_start_time_and_end_time(log, 
								"the original period: ",
								itsPeriod);

	/*
	WeatherPeriod theExtendedPeriod(extend_period(itsVar, itsPeriod, log);

	if(theExtendedPeriod.localStartTime().DifferenceInMinutes(itsPeriod.localStartTime()) != 0)
	  {
		log_start_time_and_end_time(log, 
									"the extended period: ",
									theExtendedPeriod);

		char date_str[15];
		NFmiTime forecastTime(theExtendedPeriod.localStartTime());
		forecastTime.ChangeByMinutes(5);
		sprintf(date_str, "%04d%02d%02d%02d%02d%02d", 
				forecastTime.GetYear(),
				forecastTime.GetMonth(),
				forecastTime.GetDay(),
				forecastTime.GetHour(),
				forecastTime.GetMin(),
				forecastTime.GetSec());
		
		NFmiSettings::Set("qdtext::forecasttime", date_str);
	  }
	*/
	// Period generator


	NFmiTime periodStartTime(itsPeriod.localStartTime());
	periodStartTime.ChangeByHours(-24);

	WeatherPeriod theExtendedPeriod(periodStartTime, itsPeriod.localEndTime());
	NightAndDayPeriodGenerator generator(theExtendedPeriod, itsVar);
	for(unsigned int i = 0; i < generator.size(); i++ )
	  log_start_time_and_end_time(log, 
								  "laajennettu perioodi: ",
								  generator.period(i+1));

	WeatherPeriod day1Period(generator.isday(1) ? generator.period(1) : generator.period(2));
	WeatherPeriod nightPeriod(generator.isday(1) ? generator.period(2) : generator.period(3));
	WeatherPeriod day2Period(generator.isday(1) ? generator.period(3) : generator.period(4));

	NFmiTime noonTime(day2Period.localStartTime().GetYear(),
					  day2Period.localStartTime().GetMonth(),
					  day2Period.localStartTime().GetDay(), 12, 0, 0);
	WeatherPeriod day2MorningPeriod(day2Period.localStartTime(), noonTime);
	WeatherPeriod day2AfternoonPeriod(noonTime, day2Period.localEndTime());

	log_start_time_and_end_time(log, 
								"day1: ",
								day1Period);
	log_start_time_and_end_time(log, 
								"night: ",
								nightPeriod);
	log_start_time_and_end_time(log, 
								"day2: ",
								day2Period);

 	log_start_time_and_end_time(log, 
								"day2 morning: ",
								day2MorningPeriod);

  	log_start_time_and_end_time(log, 
								"day2 afternoon: ",
								day2AfternoonPeriod);

	log << "forecast time: "
	   << itsForecastTime << endl;



	calculate_temperature(log,
						  itsVar + "::fake::temperature::day1::area",
						  itsSources,
						  itsArea,
						  day1Period,
						  theSeasonId,
						  day1TemperatureMinimum,
						  day1TemperatureMean,
						  day1TemperatureMaximum);
	calculate_temperature(log,
						  itsVar + "::fake::temperature::night::area",
						  itsSources,
						  itsArea,
						  nightPeriod,
						  theSeasonId,
						  nightTemperatureMinimum,
						  nightTemperatureMean,
						  nightTemperatureMaximum);
	calculate_temperature(log,
						  itsVar + "::fake::temperature::day2::area",
						  itsSources,
						  itsArea,
						  day2Period,
						  theSeasonId,
						  day2TemperatureMinimum,
						  day2TemperatureMean,
						  day2TemperatureMaximum);

	calculate_windspeed(log,
						itsVar + "::fake::windspeed::morning",
						itsSources,
						itsArea,
						day2MorningPeriod,
						day2WindspeedMorningMinimum,
						day2WindspeedMorningMean,
						day2WindspeedMorningMaximum);

	calculate_windspeed(log,
						itsVar + "::fake::windspeed::afternoon",
						itsSources,
						itsArea,
						day2AfternoonPeriod,
						day2WindspeedAfternoonMinimum,
						day2WindspeedAfternoonMean,
						day2WindspeedAfternoonMaximum);

	if(day2TemperatureMean.value() < ZERO_DEGREES)
	  handleWindchill = true;

	if(handleWindchill)
	  calculate_windchill(log,
						  itsVar + "::fake::windchill",
						  itsSources,
						  itsArea,
						  day2Period,
						  day2WindchillMinimum,
						  day2WindchillMean,
						  day2WindchillMaximum);


	fractile_id theFractileDay1 = get_fractile(itsVar,
											   day1TemperatureMean.value(),
											   itsSources,
											   itsArea,
											   itsPeriod);
	fractile_id theFractileNight = get_fractile(itsVar,
												nightTemperatureMean.value(),
												itsSources,
												itsArea,
												itsPeriod);
	fractile_id theFractileDay2 = get_fractile(itsVar,
											   day2TemperatureMean.value(),
											   itsSources,
											   itsArea,
											   itsPeriod);


	WeatherResult fractileTemperatureDay1 = get_fractile_temperature(itsVar,
																	 theFractileDay1,
																	 itsSources,
																	 itsArea,  
																	 itsPeriod);
	WeatherResult fractileTemperatureNight = get_fractile_temperature(itsVar,
																	  theFractileNight,
																	  itsSources,
																	  itsArea,  
																	  itsPeriod);
	WeatherResult fractileTemperatureDay2 = get_fractile_temperature(itsVar,
																	 theFractileDay2,
																	 itsSources,
																	 itsArea,  
																	 itsPeriod);



	log << "Day1 temperature " 
		<< day1TemperatureMean
		<< " falls into fractile " 
		<< fractile_name(theFractileDay1) 
		<< fractileTemperatureDay1 << endl;
	log << "Night temperature " 
		<< nightTemperatureMean
		<< " falls into fractile " 
		<< fractile_name(theFractileNight) << fractileTemperatureNight << endl;
	log << "Day2 temperature " 
		<< day2TemperatureMean
		<< " falls into fractile " 
		<< fractile_name(theFractileDay2) << fractileTemperatureDay2 << endl;
	
	log << "Day2 windspeed morning min: " 
		<< day2WindspeedMorningMinimum << " mean: " 
		<< day2WindspeedMorningMean << " max: " 
		<< day2WindspeedMorningMaximum << endl;

	log << "Day2 windspeed afternoon min: " 
		<< day2WindspeedAfternoonMinimum << " mean: " 
		<< day2WindspeedAfternoonMean << " max: " 
		<< day2WindspeedAfternoonMaximum << endl;

	if(handleWindchill)
	  {
		log << "Day2 windchill min: " 
			<< day2WindchillMinimum << " mean: " 
			<< day2WindchillMean << " max: " 
			<< day2WindchillMaximum << endl;
	  }

	Paragraph paragraphDev;
	Sentence temperatureAnomalySentence;
	Sentence shortrunTrendSentence;
	Sentence windySentence;
	Sentence windChillSentence;


	anomaly_container_type theSentences;

	temperatureAnomalySentence << temperature_anomaly_sentence(itsVar,
															   theSeasonId,
															   theFractileDay2);

	shortrunTrendSentence << temperature_shortruntrend_sentence(itsVar,
																itsPeriod.localStartTime(),
																itsSources,
																itsArea,
																day1TemperatureMean,
																day2TemperatureMean,
																theSeasonId);

	windySentence <<  windy_sentence(itsVar,
									 day2WindspeedMorningMinimum,
									 day2WindspeedMorningMean,
									 day2WindspeedMorningMaximum,
									 day2WindspeedAfternoonMinimum,
									 day2WindspeedAfternoonMean,
									 day2WindspeedAfternoonMaximum);

	if(handleWindchill)
	  windChillSentence << windchill_sentence(itsVar,
											  day2WindchillMean);


	log << "Anomaly sentence: ";
	log << temperatureAnomalySentence;
	log << shortrunTrendSentence;
	log << windySentence;
	log << windChillSentence;

	paragraph << temperatureAnomalySentence;
	paragraph << shortrunTrendSentence;
	paragraph << windySentence;
	paragraph << windChillSentence;

	log << paragraph;

	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================

  
