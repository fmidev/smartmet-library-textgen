// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::TemperatureStory::anomaly
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
			retval << "sää" << "on" << "poikkeuksellisen";
			retval << (theSeasonId == SUMMER_SEASON ? "koleaa" : "kylmää");
		  }
		  break;
		case FRACTILE_12:
		  {
			retval << "sää" << "on";
			if(theSeasonId == SUMMER_SEASON)
			  retval << "koleaa";
			else
			  retval << "hyvin" << "kylmää";
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
			retval << "sää" << "on";
			if(theSeasonId == SUMMER_SEASON)
			  retval << "harvinaisen" << "lämmintä";
			else
			  retval << "hyvin" << "leutoa";
		  }
		  break;
		case FRACTILE_100:
		  {
			retval << "sää" << "on" << "poikkeuksellisen";
			if(theSeasonId == SUMMER_SEASON)
			  retval << "lämmintä";
			else
			  retval << "leutoa";
		  }
		  break;
		case FRACTILE_UNDEFINED:
		  break;		}

	  return retval;
	}

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

	  float veryColdTemperature = wr.value() < -10.0 ? wr.value() : -10.0;

	  WeatherPeriod forecastTimeWeatherPeriod(NFmiTime(theForecastTime.GetYear(),
													   theForecastTime.GetMonth(),
													   theForecastTime.GetDay(),
													   6, 0, 0),
											  NFmiTime(theForecastTime.GetYear(),
													   theForecastTime.GetMonth(),
													   theForecastTime.GetDay(),
													   18, 0, 0));

	  WeatherResult fractile12Temperature = get_fractile_temperature(theVariable,
																	 FRACTILE_12,
																	 theAnalysisSources,
																	 theWeatherArea,
																	 forecastTimeWeatherPeriod);

	  // heikko pakkanen: -5 astetta
	  float moderatelyColdTemperature = -5.0;
	  // helle: +25 astetta
	  float veryHotTemperature = +25.0;
	  float zeroDegrees = 0.0;

	  if(theSeasonId == WINTER_SEASON)
		{
		  // pakkanen kiristyy
		  // kireä pakkanen jatkuu
		  // kireä pakkanen heikkenee
		  // pakkanen heikkenee
		  // pakkanen hellittää
		  // sää lauhtuu

		  float temperatureDifference = abs(period2Temperature - period1Temperature);
		  if(period2Temperature >= period1Temperature)
			{
			  if(temperatureDifference >= 5 && period1Temperature < moderatelyColdTemperature &&
				 period2Temperature > moderatelyColdTemperature)
				{
				  sentence << "sää lauhtuu";
				}
			  else if(temperatureDifference >= 5 && period1Temperature <= veryColdTemperature &&
					  period2Temperature <= moderatelyColdTemperature)
				{
				  sentence << "kireä" << "pakkanen heikkenee";
				}
			  else if(temperatureDifference >= 5 && period1Temperature <= veryColdTemperature &&
					  period2Temperature < zeroDegrees && period2Temperature > moderatelyColdTemperature)
				{
				  sentence << "kireä" << "pakkanen hellittää";
				}
			  else if(temperatureDifference >= 5 && period1Temperature > veryColdTemperature &&
					  period2Temperature < moderatelyColdTemperature)
				{
				  sentence << "pakkanen heikkenee";
				}
			  else if(temperatureDifference >= 5 && period1Temperature > veryColdTemperature &&
					  period1Temperature < moderatelyColdTemperature && period2Temperature < zeroDegrees &&
					  period2Temperature > moderatelyColdTemperature)
				{
				  sentence << "pakkanen hellittää";
				}
			  else if(period1Temperature < veryColdTemperature &&
					  period2Temperature < veryColdTemperature)
				{
				  sentence << "kireä pakkanen jatkuu";
				}

			}
		  else if(period2Temperature <= period1Temperature)
			{
			  if(temperatureDifference >= 5 && period1Temperature < moderatelyColdTemperature &&
				 period2Temperature <= veryColdTemperature)
				{
				  sentence << "pakkanen kiristyy";
				}
			  else if(period1Temperature < veryColdTemperature &&
					  period2Temperature < veryColdTemperature)
				{
				  sentence << "kireä pakkanen jatkuu";
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

		  // sää muuttuu helteiseksi
		  // helteinen sää jatkuu
		  // viileä sää jatkuu
		  // sää viilenee vähän
		  // sää viilenee
		  // sää viilenee huomattavasti
		  // sää lämpenee vähän
		  // sää lämpenee
		  // sää lämpenee huomattavasti

		  float temperatureDifference = abs(period2Temperature - period1Temperature);
		  bool smallChange =  temperatureDifference >= 2.0 && temperatureDifference < 3.0;
		  bool moderateChange =  temperatureDifference >= 3.0 && temperatureDifference <= 5.0;
		  bool signifigantChange =  temperatureDifference > 5.0;

		  if(period2Temperature >= period1Temperature)
			{
			  if(period1Temperature >= veryHotTemperature && period2Temperature >= veryHotTemperature)
				{
				  sentence << "helteinen sää jatkuu";
				}
			  else if(period1Temperature < fractile12Temperature.value() && 
					  period2Temperature < fractile12Temperature.value())
				{
				  sentence << "viileä sää jatkuu";
				}
			  else if(period1Temperature < veryHotTemperature && period2Temperature >= veryHotTemperature &&
					  temperatureDifference > 2.5)
				{
					sentence << "sää muuttuu helteiseksi";
				}
			  else if(smallChange)
				{
				  sentence << "sää lämpenee" << "vähän";
				}
			  else if(moderateChange)
				{
				  sentence << "sää lämpenee";
				}
			  else if(signifigantChange)
				{
				  sentence << "sää lämpenee" << "huomattavasti";
				}
			}
		  else if(period2Temperature <= period1Temperature && period2Temperature > 0)
			{
			  if(period1Temperature >= veryHotTemperature && period2Temperature >= veryHotTemperature)
				{
				  sentence << "helteinen sää jatkuu";
				}
			  else if(period1Temperature < fractile12Temperature.value() && 
					  period2Temperature < fractile12Temperature.value())
				{
				  sentence << "viileä sää jatkuu";
				}
			  else if(smallChange)
				{
				  sentence << "sää viilenee" << "vähän";
				}
			  else if(moderateChange)
				{
				  sentence << "sää viilenee";
				}
			  else if(signifigantChange)
				{
				  sentence << "sää viilenee" << "huomattavasti";
				}
			}
		}
	  
	return sentence;
	}

	const Sentence windy_sentence(const std::string theVariable,
								  const WeatherResult& windSpeedMinimum,
								  const WeatherResult& windSpeedMean,
								  const WeatherResult& windSpeedMaximum)
	{
	  Sentence sentence;

	  if(windSpeedMean.value() > 8.0)
		sentence << "sää on erittäin tuulista";
	  
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
	WeatherResult day2WindspeedMinimum(kFloatMissing, 0);
	WeatherResult day2WindspeedMean(kFloatMissing, 0);
	WeatherResult day2WindspeedMaximum(kFloatMissing, 0);
	WeatherResult day2WindchillMinimum(kFloatMissing, 0);
	WeatherResult day2WindchillMean(kFloatMissing, 0);
	WeatherResult day2WindchillMaximum(kFloatMissing, 0);

	forecast_season_id theSeasonId = isSummerHalf(itsPeriod.localStartTime(), itsVar) ? SUMMER_SEASON : WINTER_SEASON;

	// the anomaly sentence relates always the the upcoming day,
	// so the period is defined to start in the previous day

	NFmiTime newStartTime(itsPeriod.localStartTime());
	newStartTime.ChangeByHours(-24);
	//itsForecastTime.ChangeByHours(-24);

	WeatherPeriod theExtendedPeriod(newStartTime, itsPeriod.localEndTime());

	// Period generator
	NightAndDayPeriodGenerator generator(theExtendedPeriod, itsVar);
	WeatherPeriod day1Period(generator.isday(1) ? generator.period(1) : generator.period(2));
	WeatherPeriod nightPeriod(generator.isday(1) ? generator.period(2) : generator.period(3));
	WeatherPeriod day2Period(generator.isday(1) ? generator.period(3) : generator.period(4));


	for(unsigned int i = 0; i < generator.size(); i++ )
	  log_start_time_and_end_time(log, 
								  "perioodi: ",
								  generator.period(i+1));

	log_start_time_and_end_time(log, 
								"alkuperäinen periodi: ",
								itsPeriod);
	log_start_time_and_end_time(log, 
								"day1: ",
								day1Period);
	log_start_time_and_end_time(log, 
								"night: ",
								nightPeriod);
	log_start_time_and_end_time(log, 
								"day2: ",
								day2Period);

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
						itsVar + "::fake::windspeed",
						itsSources,
						itsArea,
						day2Period,
						day2WindspeedMinimum,
						day2WindspeedMean,
						day2WindspeedMaximum);

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
	
	log << "Day2 windspeed min: " 
		<< day2WindspeedMinimum << " mean: " 
		<< day2WindspeedMean << " max: " 
		<< day2WindspeedMaximum << endl;

	log << "Day2 windchill min: " 
		<< day2WindchillMinimum << " mean: " 
		<< day2WindchillMean << " max: " 
		<< day2WindchillMaximum << endl;


	Paragraph paragraphDev;
	Sentence temperatureAnomalySentence;
	Sentence shortrunTrendSentence;
	Sentence windySentence;
	Sentence bitingFrostSentence;

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
									 day2WindspeedMinimum,
									 day2WindspeedMean,
									 day2WindspeedMaximum);

	/*

	if(day1TemperatureMean.value() != kFloatMissing &&
	   day1WindspeedMean.value() != kFloatMissing &&
	   day1TemperatureMean.value() < -15.0 &&
	   day1WindspeedMean.value() > 6.0)
	  bitingFrostSentence << "pakkanen on purevaa";
	*/



	log << "Anomaly sentence: ";
	log << temperatureAnomalySentence;
	log << shortrunTrendSentence;
	log << bitingFrostSentence;
	log << windySentence;

	paragraph << temperatureAnomalySentence;
	paragraph << shortrunTrendSentence;
	paragraph << bitingFrostSentence;
	paragraph << windySentence;

	log << paragraph;

	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================

  
