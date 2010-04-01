// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::TemperatureStory::max36hours
 */
// ======================================================================

#include "TemperatureStory.h"
#include "TemperatureTools.h"
#include "SeasonTools.h"

#include "ClimatologyTools.h"
#include "GridClimatology.h"

#include "DefaultAcceptor.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "NightAndDayPeriodGenerator.h"
#include "Integer.h"
#include "IntegerRange.h"
#include "MathTools.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "UnitFactory.h"
#include "WeatherPeriodTools.h"
#include "WeatherResult.h"
#include "WeatherResultTools.h"
#include "PeriodPhraseFactory.h"

#include <newbase/NFmiStringTools.h>
#include <newbase/NFmiGrid.h>
#include <newbase/NFmiQueryData.h>
#include <newbase/NFmiIndexMask.h>
#include <newbase/NFmiIndexMaskSource.h>
#include <newbase/NFmiIndexMaskTools.h>
#include <newbase/NFmiSettings.h>
#include <newbase/NFmiGlobals.h>

#include "WeatherAnalysisError.h"
#include "WeatherArea.h"
#include "WeatherSource.h"

#include <boost/lexical_cast.hpp>

#include <map>
#include <vector>

namespace TextGen
{

  namespace TemperatureMax36Hours
  {
	using NFmiStringTools::Convert;
	using namespace WeatherAnalysis;
	using namespace TemperatureTools;
	//using namespace TextGen::TemperatureStoryTools;
	using namespace boost;
	using namespace std;
	using namespace Settings;
	using MathTools::to_precision;
	using Settings::optional_int;
	using Settings::optional_bool;
	using Settings::optional_string;


	enum forecast_area_id{NO_AREA = 0x0,
						  COASTAL_AREA = 0x1, 
						  INLAND_AREA = 0x2,
						  FULL_AREA = 0x4};

	enum forecast_period_id{DAY1_PERIOD = 0x1, 
							NIGHT_PERIOD = 0x2,
							DAY2_PERIOD = 0x4};

	enum forecast_subperiod_id{DAY1_MORNING_PERIOD,
							   DAY1_AFTERNOON_PERIOD,
							   DAY2_MORNING_PERIOD,
							   DAY2_AFTERNOON_PERIOD,
							   UNDEFINED_SUBPERIOD};

	enum forecast_season_id{SUMMER_SEASON, 
							WINTER_SEASON,
							UNDEFINED_SEASON};

	enum processing_order{DAY1_DAY2_NIGHT,
						  DAY1_NIGHT,
						  NIGHT_DAY2};

	enum forecast_period_area_id{DAY1_INLAND,
								 DAY1_COASTAL,
								 DAY1_FULL,
								 NIGHT_INLAND,
								 NIGHT_COASTAL,
								 NIGHT_FULL,
								 DAY2_INLAND,
								 DAY2_COASTAL,
								 DAY2_FULL};

	/*
	enum weather_result_id{AREA_MIN_DAY1 = 0x1,
						   AREA_MAX_DAY1,
						   AREA_MEAN_DAY1,
						   AREA_MIN_NIGHT,
						   AREA_MAX_NIGHT,
						   AREA_MEAN_NIGHT,
						   AREA_MIN_DAY2,
						   AREA_MAX_DAY2,
						   AREA_MEAN_DAY2,
						   INLAND_MIN_DAY1,
						   INLAND_MAX_DAY1,
						   INLAND_MEAN_DAY1,
						   INLAND_MIN_NIGHT,
						   INLAND_MAX_NIGHT,
						   INLAND_MEAN_NIGHT,
						   INLAND_MIN_DAY2,
						   INLAND_MAX_DAY2,
						   INLAND_MEAN_DAY2,
						   COAST_MIN_DAY1 ,
						   COAST_MAX_DAY1,
						   COAST_MEAN_DAY1,
						   COAST_MIN_NIGHT,
						   COAST_MAX_NIGHT,
						   COAST_MEAN_NIGHT,
						   COAST_MIN_DAY2,
						   COAST_MAX_DAY2,
						   COAST_MEAN_DAY2,
						   WEATHER_RESULT_ID_UNDEFINED};
	*/
	
	enum weather_result_id{AREA_MIN_DAY1 = 0x1,
						   AREA_MAX_DAY1,
						   AREA_MEAN_DAY1,
						   AREA_MIN_NIGHT,
						   AREA_MAX_NIGHT,
						   AREA_MEAN_NIGHT,
						   AREA_MIN_DAY2,
						   AREA_MAX_DAY2,
						   AREA_MEAN_DAY2,
						   AREA_MIN_DAY1_MORNING,
						   AREA_MAX_DAY1_MORNING,
						   AREA_MEAN_DAY1_MORNING,
						   AREA_MIN_DAY2_MORNING,
						   AREA_MAX_DAY2_MORNING,
						   AREA_MEAN_DAY2_MORNING,
						   AREA_MIN_DAY1_AFTERNOON,
						   AREA_MAX_DAY1_AFTERNOON,
						   AREA_MEAN_DAY1_AFTERNOON,
						   AREA_MIN_DAY2_AFTERNOON,
						   AREA_MAX_DAY2_AFTERNOON,
						   AREA_MEAN_DAY2_AFTERNOON,
						   INLAND_MIN_DAY1,
						   INLAND_MAX_DAY1,
						   INLAND_MEAN_DAY1,
						   INLAND_MIN_NIGHT,
						   INLAND_MAX_NIGHT,
						   INLAND_MEAN_NIGHT,
						   INLAND_MIN_DAY2,
						   INLAND_MAX_DAY2,
						   INLAND_MEAN_DAY2,
						   INLAND_MIN_DAY1_MORNING,
						   INLAND_MAX_DAY1_MORNING,
						   INLAND_MEAN_DAY1_MORNING,
						   INLAND_MIN_DAY2_MORNING,
						   INLAND_MAX_DAY2_MORNING,
						   INLAND_MEAN_DAY2_MORNING,
						   INLAND_MIN_DAY1_AFTERNOON,
						   INLAND_MAX_DAY1_AFTERNOON,
						   INLAND_MEAN_DAY1_AFTERNOON,
						   INLAND_MIN_DAY2_AFTERNOON,
						   INLAND_MAX_DAY2_AFTERNOON,
						   INLAND_MEAN_DAY2_AFTERNOON,
						   COAST_MIN_DAY1 ,
						   COAST_MAX_DAY1,
						   COAST_MEAN_DAY1,
						   COAST_MIN_NIGHT,
						   COAST_MAX_NIGHT,
						   COAST_MEAN_NIGHT,
						   COAST_MIN_DAY2,
						   COAST_MAX_DAY2,
						   COAST_MEAN_DAY2,
						   COAST_MIN_DAY1_MORNING,
						   COAST_MAX_DAY1_MORNING,
						   COAST_MEAN_DAY1_MORNING,
						   COAST_MIN_DAY2_MORNING,
						   COAST_MAX_DAY2_MORNING,
						   COAST_MEAN_DAY2_MORNING,
						   COAST_MIN_DAY1_AFTERNOON,
						   COAST_MAX_DAY1_AFTERNOON,
						   COAST_MEAN_DAY1_AFTERNOON,
						   COAST_MIN_DAY2_AFTERNOON,
						   COAST_MAX_DAY2_AFTERNOON,
						   COAST_MEAN_DAY2_AFTERNOON,
						   UNDEFINED_WEATHER_RESULT_ID};


	typedef map<int, WeatherResult*> weather_result_container_type;
	typedef weather_result_container_type::value_type value_type;

	WeatherResult do_calculation(const string& theVar,
								 const AnalysisSources& theSources,
								 const WeatherFunction& theAreaFunction,
								 const WeatherFunction& theTimeFunction,
								 const WeatherArea& theArea,
								 const WeatherPeriod& thePeriod)
	{
	  GridForecaster theForecaster;

	  return theForecaster.analyze(theVar,
								   theSources,
								   Temperature,
								   theAreaFunction,
								   theTimeFunction,
								   theArea,
								   thePeriod);
	}

	void calculate_results2(MessageLogger& theLog,
						   const string& theVar,
						   const AnalysisSources& theSources,
						   const WeatherArea& theArea,
						   const WeatherPeriod& thePeriod,
						   const forecast_period_id& thePeriodId,
						   const forecast_season_id& theSeasonId,
						   const forecast_area_id& theAreaId,
						   weather_result_container_type& theWeatherResults)
	{
	  weather_result_id min_id_full, max_id_full, mean_id_full;
	  weather_result_id min_id_morning, max_id_morning, mean_id_morning;
	  weather_result_id min_id_afternoon, max_id_afternoon, mean_id_afternoon;

	  std::string fakeVarFull("::fake");
	  std::string fakeVarMorning("::fake");
	  std::string fakeVarAfternoon("::fake");

	  min_id_full = max_id_full = mean_id_full = UNDEFINED_WEATHER_RESULT_ID;
	  min_id_morning = max_id_morning = mean_id_morning = UNDEFINED_WEATHER_RESULT_ID;
	  min_id_afternoon = max_id_afternoon = mean_id_afternoon = UNDEFINED_WEATHER_RESULT_ID;
	  
	  if(thePeriodId == DAY1_PERIOD)
		{
		  min_id_full = (theAreaId == INLAND_AREA ? INLAND_MIN_DAY1 : 
					(theAreaId == COASTAL_AREA ? COAST_MIN_DAY1 : AREA_MIN_DAY1));
		  max_id_full = (theAreaId == INLAND_AREA ? INLAND_MAX_DAY1 : 
					(theAreaId == COASTAL_AREA ? COAST_MAX_DAY1 : AREA_MAX_DAY1));
		  mean_id_full = (theAreaId == INLAND_AREA ? INLAND_MEAN_DAY1 : 
					 (theAreaId == COASTAL_AREA ? COAST_MEAN_DAY1 : AREA_MEAN_DAY1));

		  if(theSeasonId == SUMMER_SEASON)
			{
			  min_id_morning = (theAreaId == INLAND_AREA ? INLAND_MIN_DAY1_MORNING : 
								(theAreaId == COASTAL_AREA ? COAST_MIN_DAY1_MORNING : AREA_MIN_DAY1_MORNING));
			  max_id_morning = (theAreaId == INLAND_AREA ? INLAND_MAX_DAY1_MORNING : 
								(theAreaId == COASTAL_AREA ? COAST_MAX_DAY1_MORNING : AREA_MAX_DAY1_MORNING));
			  mean_id_morning = (theAreaId == INLAND_AREA ? INLAND_MEAN_DAY1_MORNING : 
								 (theAreaId == COASTAL_AREA ? COAST_MEAN_DAY1_MORNING : AREA_MEAN_DAY1_MORNING));
			  min_id_afternoon = (theAreaId == INLAND_AREA ? INLAND_MIN_DAY1_AFTERNOON : 
							 (theAreaId == COASTAL_AREA ? COAST_MIN_DAY1_AFTERNOON : AREA_MIN_DAY1_AFTERNOON));
			  max_id_afternoon = (theAreaId == INLAND_AREA ? INLAND_MAX_DAY1_AFTERNOON : 
							 (theAreaId == COASTAL_AREA ? COAST_MAX_DAY1_AFTERNOON : AREA_MAX_DAY1_AFTERNOON));
			  mean_id_afternoon = (theAreaId == INLAND_AREA ? INLAND_MEAN_DAY1_AFTERNOON : 
							  (theAreaId == COASTAL_AREA ? COAST_MEAN_DAY1_AFTERNOON : AREA_MEAN_DAY1_AFTERNOON));
			}

		  fakeVarFull += "::day1";
		  fakeVarMorning += "::day1::morning";
		  fakeVarAfternoon += "::day1::afternoon";
		}
	  else if(thePeriodId == NIGHT_PERIOD)
		{
		  min_id_full = (theAreaId == INLAND_AREA ? INLAND_MIN_NIGHT : 
					(theAreaId == COASTAL_AREA ? COAST_MIN_NIGHT : AREA_MIN_NIGHT));
		  max_id_full = (theAreaId == INLAND_AREA ? INLAND_MAX_NIGHT : 
					(theAreaId == COASTAL_AREA ? COAST_MAX_NIGHT : AREA_MAX_NIGHT));
		  mean_id_full = (theAreaId == INLAND_AREA ? INLAND_MEAN_NIGHT : 
					 (theAreaId == COASTAL_AREA ? COAST_MEAN_NIGHT : AREA_MEAN_NIGHT));
		  fakeVarFull += "::night";
		}
	  else if(thePeriodId == DAY2_PERIOD)
		{
		  min_id_full = (theAreaId == INLAND_AREA ? INLAND_MIN_DAY2 : 
					(theAreaId == COASTAL_AREA ? COAST_MIN_DAY2 : AREA_MIN_DAY2));
		  max_id_full = (theAreaId == INLAND_AREA ? INLAND_MAX_DAY2 : 
					(theAreaId == COASTAL_AREA ? COAST_MAX_DAY2 : AREA_MAX_DAY2));
		  mean_id_full = (theAreaId == INLAND_AREA ? INLAND_MEAN_DAY2 : 
					 (theAreaId == COASTAL_AREA ? COAST_MEAN_DAY2 : AREA_MEAN_DAY2));

		  if(theSeasonId == SUMMER_SEASON)
			{
			  min_id_morning = (theAreaId == INLAND_AREA ? INLAND_MIN_DAY2_MORNING : 
								(theAreaId == COASTAL_AREA ? COAST_MIN_DAY2_MORNING : AREA_MIN_DAY2_MORNING));
			  max_id_morning = (theAreaId == INLAND_AREA ? INLAND_MAX_DAY2_MORNING : 
								(theAreaId == COASTAL_AREA ? COAST_MAX_DAY2_MORNING : AREA_MAX_DAY2_MORNING));
			  mean_id_morning = (theAreaId == INLAND_AREA ? INLAND_MEAN_DAY2_MORNING : 
								 (theAreaId == COASTAL_AREA ? COAST_MEAN_DAY2_MORNING : AREA_MEAN_DAY2_MORNING));
			  min_id_afternoon = (theAreaId == INLAND_AREA ? INLAND_MIN_DAY2_AFTERNOON : 
							 (theAreaId == COASTAL_AREA ? COAST_MIN_DAY2_AFTERNOON : AREA_MIN_DAY2_AFTERNOON));
			  max_id_afternoon = (theAreaId == INLAND_AREA ? INLAND_MAX_DAY2_AFTERNOON : 
							 (theAreaId == COASTAL_AREA ? COAST_MAX_DAY2_AFTERNOON : AREA_MAX_DAY2_AFTERNOON));
			  mean_id_afternoon = (theAreaId == INLAND_AREA ? INLAND_MEAN_DAY2_AFTERNOON : 
							  (theAreaId == COASTAL_AREA ? COAST_MEAN_DAY2_AFTERNOON : AREA_MEAN_DAY2_AFTERNOON));
			}

		  fakeVarFull += "::day2";
		  fakeVarMorning += "::day2::morning";
		  fakeVarAfternoon += "::day2::afternoon";
		}
	  else
		{
		  // TODO: error handling
		  return;
		}

	  WeatherArea theActualArea = theArea;

	  if(theAreaId == INLAND_AREA) {
		theActualArea.type(WeatherArea::Inland);
		fakeVarFull += "::inland";
		fakeVarMorning += "::inland";
		fakeVarAfternoon += "::inland";
	  }
	  else if(theAreaId == COASTAL_AREA)
		{
		  theActualArea.type(WeatherArea::Coast);
		  fakeVarFull += "::coast";
		  fakeVarMorning += "::coast";
		  fakeVarAfternoon += "::coast";
		}
	  else
		{
		  theActualArea.type(WeatherArea::Full);
		  fakeVarFull += "::area";		  
		  fakeVarMorning += "::area";
		  fakeVarAfternoon += "::area";
		}

	  WeatherResult& minResultFull = *theWeatherResults[min_id_full];
	  WeatherResult& maxResultFull = *theWeatherResults[max_id_full];
	  WeatherResult& meanResultFull = *theWeatherResults[mean_id_full];
	  WeatherResult& minResultMorning = *theWeatherResults[min_id_morning];
	  WeatherResult& maxResultMorning = *theWeatherResults[max_id_morning];
	  WeatherResult& meanResultMorning = *theWeatherResults[mean_id_morning];
	  WeatherResult& minResultAfternoon = *theWeatherResults[min_id_afternoon];
	  WeatherResult& maxResultAfternoon = *theWeatherResults[max_id_afternoon];
	  WeatherResult& meanResultAfternoon = *theWeatherResults[mean_id_afternoon];
	  
	  if(theSeasonId == SUMMER_SEASON)
		{
		  // In summertime we calculate maximum (of area maximums) at daytime and 
		  // minimum (of area minimums) at nighttime
		  WeatherFunction theAreaFunction = (thePeriodId == NIGHT_PERIOD ? Minimum : Maximum);

		  minResultFull = do_calculation(theVar + fakeVarFull + "::min",
									 theSources,
									 theAreaFunction,
									 Minimum,
									 theActualArea,
									 thePeriod);

		  maxResultFull = do_calculation(theVar + fakeVarFull + "::max",
									 theSources,
									 theAreaFunction,
									 Maximum,
									 theActualArea,
									 thePeriod);

		  meanResultFull = do_calculation(theVar + fakeVarFull + "::mean",
									  theSources,
									  theAreaFunction,
									  Mean,
									  theActualArea,
									  thePeriod);

		  if(thePeriodId == DAY1_PERIOD || thePeriodId == DAY2_PERIOD)
			{
			  morning_temperature(theVar + fakeVarMorning,
								  theSources,
								  theActualArea,
								  thePeriod,
								  minResultMorning,
								  maxResultMorning,
								  meanResultMorning);

			  afternoon_temperature(theVar + fakeVarAfternoon,
									theSources,
									theActualArea,
									thePeriod,
									minResultAfternoon,
									maxResultAfternoon,
									meanResultAfternoon);
			}

		  theLog << "Temperature ";
		  theLog << (theAreaFunction == Minimum ? "Minimum(Min) " : "Maximum(Min) ") << minResultFull << endl;
		  theLog << "Temperature ";
		  theLog << (theAreaFunction == Minimum ? "Minimum(Max) " : "Maximum(Max) ") << maxResultFull << endl;
		  theLog << "Temperature ";
		  theLog << (theAreaFunction == Minimum ? "Minimum(Mean) " : "Maximum(Mean) ") << meanResultFull << endl;
		}
	  else
		{
		  // In wintertime we calculate Mean temperature of areas Maximum, Minimum and Mean temperatures

		  minResultFull = do_calculation(theVar + fakeVarFull + "::min",
									 theSources,
									 Minimum,
									 Mean   ,
									 theActualArea,								
									 thePeriod);

		  maxResultFull = do_calculation(theVar + fakeVarFull + "::max",
									 theSources,
									 Maximum,
									 Mean,
									 theActualArea,
									 thePeriod);

		  meanResultFull = do_calculation(theVar + fakeVarFull + "::mean",
									  theSources,
									  Mean,
									  Mean,
									  theActualArea,
									  thePeriod);

		  theLog << "Temperature ";
		  theLog << "Minimum(Mean) " << minResultFull << endl;
		  theLog << "Temperature ";
		  theLog << "Maximum(Mean) " << maxResultFull << endl;
		  theLog << "Temperature ";
		  theLog << "Mean(Mean) " << meanResultFull << endl;
		}

	}

	void calculate_results(MessageLogger& theLog,
						   const string& theVar,
						   const AnalysisSources& theSources,
						   const WeatherArea& theArea,
						   const WeatherPeriod& thePeriod,
						   const forecast_period_id& thePeriodId,
						   const forecast_season_id& theSeasonId,
						   const forecast_area_id& theAreaId,
						   weather_result_container_type& theWeatherResults)
	{
	  calculate_results2(theLog,
						 theVar,
						 theSources,
						 theArea,
						 thePeriod,
						 thePeriodId,
						 theSeasonId,
						 theAreaId,
						 theWeatherResults);
#ifdef KUKKUU
	  weather_result_id min_id, max_id, mean_id;
	  std::string fakeVar("::fake");

	  min_id = max_id = mean_id = WEATHER_RESULT_ID_UNDEFINED;
	  
	  if(thePeriodId == DAY1_PERIOD)
		{
		  min_id = (theAreaId == INLAND_AREA ? INLAND_MIN_DAY1 : 
					(theAreaId == COASTAL_AREA ? COAST_MIN_DAY1 : AREA_MIN_DAY1));
		  max_id = (theAreaId == INLAND_AREA ? INLAND_MAX_DAY1 : 
					(theAreaId == COASTAL_AREA ? COAST_MAX_DAY1 : AREA_MAX_DAY1));
		  mean_id = (theAreaId == INLAND_AREA ? INLAND_MEAN_DAY1 : 
					 (theAreaId == COASTAL_AREA ? COAST_MEAN_DAY1 : AREA_MEAN_DAY1));
		  fakeVar += "::day1";
		}
	  else if(thePeriodId == NIGHT_PERIOD)
		{
		  min_id = (theAreaId == INLAND_AREA ? INLAND_MIN_NIGHT : 
					(theAreaId == COASTAL_AREA ? COAST_MIN_NIGHT : AREA_MIN_NIGHT));
		  max_id = (theAreaId == INLAND_AREA ? INLAND_MAX_NIGHT : 
					(theAreaId == COASTAL_AREA ? COAST_MAX_NIGHT : AREA_MAX_NIGHT));
		  mean_id = (theAreaId == INLAND_AREA ? INLAND_MEAN_NIGHT : 
					 (theAreaId == COASTAL_AREA ? COAST_MEAN_NIGHT : AREA_MEAN_NIGHT));
		  fakeVar += "::night";
		}
	  else if(thePeriodId == DAY2_PERIOD)
		{
		  min_id = (theAreaId == INLAND_AREA ? INLAND_MIN_DAY2 : 
					(theAreaId == COASTAL_AREA ? COAST_MIN_DAY2 : AREA_MIN_DAY2));
		  max_id = (theAreaId == INLAND_AREA ? INLAND_MAX_DAY2 : 
					(theAreaId == COASTAL_AREA ? COAST_MAX_DAY2 : AREA_MAX_DAY2));
		  mean_id = (theAreaId == INLAND_AREA ? INLAND_MEAN_DAY2 : 
					 (theAreaId == COASTAL_AREA ? COAST_MEAN_DAY2 : AREA_MEAN_DAY2));
		  fakeVar += "::day2";
		}
	  else
		{
		  // TODO: error handling
		  return;
		}

	  WeatherArea theActualArea = theArea;

	  if(theAreaId == INLAND_AREA) {
		theActualArea.type(WeatherArea::Inland);
		fakeVar += "::inland";
	  }
	  else if(theAreaId == COASTAL_AREA)
		{
		  theActualArea.type(WeatherArea::Coast);
		  fakeVar += "::coast";
		}
	  else
		{
		  theActualArea.type(WeatherArea::Full);
		  fakeVar += "::area";		  
		}

	  WeatherResult& minResult = *theWeatherResults[min_id];
	  WeatherResult& maxResult = *theWeatherResults[max_id];
	  WeatherResult& meanResult = *theWeatherResults[mean_id];
	  
	  if(theSeasonId == SUMMER_SEASON)
		{
		  // In summertime we calculate maximum (of area maximums) at daytime and 
		  // minimum (of area minimums) at nighttime
		  WeatherFunction theAreaFunction = (thePeriodId == NIGHT_PERIOD ? Minimum : Maximum);

		  minResult = do_calculation(theVar + fakeVar + "::min",
									 theSources,
									 theAreaFunction,
									 Minimum,
									 theActualArea,
									 thePeriod);

		  maxResult = do_calculation(theVar + fakeVar + "::max",
									 theSources,
									 theAreaFunction,
									 Maximum,
									 theActualArea,
									 thePeriod);

		  meanResult = do_calculation(theVar + fakeVar + "::mean",
									  theSources,
									  theAreaFunction,
									  Mean,
									  theActualArea,
									  thePeriod);

		  theLog << "Temperature ";
		  theLog << (theAreaFunction == Minimum ? "Minimum(Min) " : "Maximum(Min) ") << minResult << endl;
		  theLog << "Temperature ";
		  theLog << (theAreaFunction == Minimum ? "Minimum(Max) " : "Maximum(Max) ") << maxResult << endl;
		  theLog << "Temperature ";
		  theLog << (theAreaFunction == Minimum ? "Minimum(Mean) " : "Maximum(Mean) ") << meanResult << endl;
		}
	  else
		{
		  // In wintertime we calculate Mean temperature of areas Maximum, Minimum and Mean temperatures

		  minResult = do_calculation(theVar + fakeVar + "::min",
									 theSources,
									 Minimum,
									 Mean   ,
									 theActualArea,								
									 thePeriod);

		  maxResult = do_calculation(theVar + fakeVar + "::max",
									 theSources,
									 Maximum,
									 Mean,
									 theActualArea,
									 thePeriod);

		  meanResult = do_calculation(theVar + fakeVar + "::mean",
									  theSources,
									  Mean,
									  Mean,
									  theActualArea,
									  thePeriod);

		  theLog << "Temperature ";
		  theLog << "Minimum(Mean) " << minResult << endl;
		  theLog << "Temperature ";
		  theLog << "Maximum(Mean) " << maxResult << endl;
		  theLog << "Temperature ";
		  theLog << "Mean(Mean) " << meanResult << endl;
		}
#endif
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


	const void log_weather_results(MessageLogger& theLog, weather_result_container_type& weatherResults)
	{
	  theLog << "Weather results: " << endl;

	  // Iterate and print out the WeatherResult variables
	  for(int i = AREA_MIN_DAY1; i <= COAST_MEAN_DAY2; i++)
		{
		  theLog << *weatherResults[i] << endl;
		} 
	}


	const forecast_season_id get_forecast_season(const NFmiTime& theTime, const std::string theVariable)
	{
	  return (SeasonTools::isSummerHalf(theTime, theVariable) ? SUMMER_SEASON : WINTER_SEASON);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return temperature sentence
	 *
	 * Possible sentences are
	 *
	 *  - "noin x astetta"
	 *  - "x...y astetta";
	 *
	 * \param theMinimum The minimum temperature
	 * \param theMean The mean temperature
	 * \param theMaximum The maximum temperature
	 * \param theMinInterval The minimum interval limit
	 * \param theRangeSeparator String separating the numbers
	 * \return The sentence
	 */
	// ----------------------------------------------------------------------
	
	const TextGen::Sentence temperature_sentence(const std::string theVariable,
												 const double& theMinimum,
												 const double& theMaximum)
	{
	  Sentence sentence;
	  

	  const string range_separator = optional_string(theVariable + "::rangeseparator", "...");
	  const int min_interval = optional_int(theVariable + "::min_interval", 2);
	  
	  int min_value = static_cast<int>(round(theMinimum));
	  int max_value = static_cast<int>(round(theMaximum));
	  //int mean_value = static_cast<int>(round(theMean));

	  if(max_value - min_value > 5)
		min_value = max_value - 5;

	  bool range = true;
	  
	  if(min_value == max_value)
		range = false;
	  else if(max_value - min_value >= min_interval)
		range = true;
	  else if(min_value <= 0 && max_value >= 0)
		range = true;
	  else
		range = false;
	  
	  if(range)
		{
		  if(max_value + min_value == 0 && max_value <= 2.0)
			sentence << "nollan tienoilla";
		  else
			sentence << IntegerRange(min_value, max_value, range_separator)
					 << *UnitFactory::create(DegreesCelsius);
		}
	  else
		{
		  sentence << "noin"
				   << Integer(max_value)
				   << *UnitFactory::create(DegreesCelsius);
		}
	  
	  return sentence;
	}

	const Sentence temperature_phrase(const forecast_season_id& theSeasonId,
									  const forecast_period_id& thePeriodId,
									  const forecast_subperiod_id& theSubperiodId,
									  const unsigned short& theForecastPeriods,
									  const forecast_area_id& theAreaId,
									  const unsigned short& theForecastAreas) 
	{
	  Sentence sentence;
	  
	  /*
	  bool useAreaPhrase = (thePeriodId == DAY1_PERIOD || 
							(thePeriodId == NIGHT_PERIOD && !(theForecastPeriods & DAY1_PERIOD)) ||
							(thePeriodId == DAY2_PERIOD && !(theForecastPeriods & DAY1_PERIOD)));

	  if(theAreaId == INLAND_AREA && (theForecastAreas & COASTAL_AREA) && useAreaPhrase)
		sentence << "sisämaassa";
	  else if(theAreaId == COASTAL_AREA && (theForecastAreas & INLAND_AREA) && useAreaPhrase)
		sentence << "rannikolla";
	  */
	  if(theAreaId == INLAND_AREA && (theForecastAreas & COASTAL_AREA && theForecastAreas & INLAND_AREA))
		sentence << "sisämaassa";
	  else if(theAreaId == COASTAL_AREA && (theForecastAreas & COASTAL_AREA && theForecastAreas & INLAND_AREA))
		sentence << "rannikolla";


	  bool useDayTemperaturePhrase = (thePeriodId == DAY1_PERIOD || 
									  (thePeriodId == DAY2_PERIOD && !(theForecastPeriods & DAY1_PERIOD)));
	  if(theSeasonId == SUMMER_SEASON)
		{
		  if(thePeriodId == NIGHT_PERIOD)
			{
			  sentence << "yön alin lämpötila";
			}
		  else if(useDayTemperaturePhrase)
			{
			  if(theSubperiodId == UNDEFINED_SUBPERIOD)
				sentence << "päivän ylin lämpötila";
			  else
				if(theSubperiodId == DAY1_MORNING_PERIOD || theSubperiodId == DAY2_MORNING_PERIOD)
				  sentence << "lämpötila" << "aamulla";
			  else
				  sentence << "iltapäivällä";
			}
		}
	  else
		{
		  if(thePeriodId == NIGHT_PERIOD)
			{
			  sentence << "yölämpötila";
			}
		  else if(useDayTemperaturePhrase)
			{
			  sentence << "päivälämpötila";
			}
		}

	  return sentence;
	}

	const Sentence night_sentence(const std::string theVariable,
								  const forecast_season_id& theSeasonId,
								  const forecast_area_id& theAreaId,
								  const unsigned short& theForecastAreas,
								  const unsigned short& theForecastPeriods,
								  const double& theMaxTemperatureDay1,
								  const double& theMeanTemperatureDay1,
								  const double& theMinimum,
								  const double& theMaximum,
								  const double& theMean)
	{
	  Sentence sentence;

	  double temperatureDifferenceDay1Night = 100.0;
	  bool day1PeriodIncluded = theMaxTemperatureDay1 != kFloatMissing;
	  bool nightlyMinHigherThanDailyMax = (theMaxTemperatureDay1 - theMinimum < 0);

	  if(day1PeriodIncluded)
		{
		  temperatureDifferenceDay1Night =  theMean - theMeanTemperatureDay1;
		}

	  bool smallChangeBetweenDay1AndNight = day1PeriodIncluded && 
		abs(temperatureDifferenceDay1Night) <= 2.0;
	  bool moderateChangeBetweenDay1AndNight = day1PeriodIncluded && 
		abs(temperatureDifferenceDay1Night) > 2.0 && abs(temperatureDifferenceDay1Night) <= 5.0;


	  //	  if(theSeasonId == SUMMER_SEASON && moderateChangeBetweenDay1AndNight || nightlyMinHigherThanDailyMax)
	  //	sentence << "yöllä";
	  //else

	  sentence << temperature_phrase(theSeasonId, NIGHT_PERIOD, UNDEFINED_SUBPERIOD, theForecastPeriods, theAreaId, theForecastAreas);

	  if(smallChangeBetweenDay1AndNight)
		{
		  // no change
		  sentence << "suunnilleen sama";
		}
	  else if(moderateChangeBetweenDay1AndNight)
		{
		  // small change
		  if(temperatureDifferenceDay1Night > 0)
			sentence << "hieman korkeampi";
		  else
			sentence << "hieman alempi";
		}
	  else if(nightlyMinHigherThanDailyMax)
		{
			sentence << "lämpötila nousee";
		}
	  /*
	  else if(moderateChangeBetweenDay1AndNight)
		{
		  if(temperatureDifferenceDay1Night < 0)
			sentence << "hieman viileämpää";
		  else
			sentence << "hieman lämpimämpää";
		}
	  */
	  else
		{
		  sentence << temperature_sentence(theVariable,
										   theMinimum,
										   theMaximum);
		}

	  return sentence;
	}

	const Sentence day2_sentence(const std::string theVariable,
								 const forecast_season_id& theSeasonId,
								 const forecast_area_id& theAreaId,
								 const forecast_subperiod_id& theSubperiodId,
								 const unsigned short& theForecastAreas,
								 const unsigned short& theForecastPeriods,
								 const double& theMeanTemperatureDay1,
								 const double& theMinimum,
								 const double& theMaximum,
								 const double& theMean)
	{
	  Sentence sentence;

	  double temperatureDifferenceDay1Day2 = 100.0;

	  if(theForecastPeriods & DAY1_PERIOD)
		{
			temperatureDifferenceDay1Day2 =  theMean - theMeanTemperatureDay1;
		}

	  bool day1PeriodIncluded = theMeanTemperatureDay1 != kFloatMissing;
	  bool smallChangeBetweenDay1AndDay2 = day1PeriodIncluded && 
		abs(temperatureDifferenceDay1Day2) <= 2.0;
	  bool moderateChangeBetweenDay1AndDay2 = day1PeriodIncluded && 
		abs(temperatureDifferenceDay1Day2) > 2.0 && abs(temperatureDifferenceDay1Day2) <= 5.0;

	  sentence << "huomenna";

	  sentence << temperature_phrase(theSeasonId, DAY2_PERIOD, UNDEFINED_SUBPERIOD, theForecastPeriods, theAreaId, theForecastAreas);

	  if(smallChangeBetweenDay1AndDay2)
		{

		  sentence << "suunnilleen sama";
		}
	  else if(moderateChangeBetweenDay1AndDay2)
		{
		  // small change
		  if(temperatureDifferenceDay1Day2 > 0)
			sentence << "hieman korkeampi";
		  else
			sentence << "hieman alempi";
		}
	  else
		{
		  sentence << temperature_sentence(theVariable,
										   theMinimum,
										   theMaximum);
		}

	  return sentence;
	}

	const Sentence day1_sentence(const std::string theVariable,
								 const forecast_season_id& theSeasonId,
								 const forecast_area_id& theAreaId,
								 const forecast_subperiod_id& theSubperiodId,
								 const unsigned short& theForecastAreas,
								 const unsigned short& theForecastPeriods,
								 const double& theMinimum,
								 const double& theMaximum,
								 const double& theMean)
	{
	  Sentence sentence;

	  sentence << temperature_phrase(theSeasonId, DAY1_PERIOD, theSubperiodId, theForecastPeriods, theAreaId, theForecastAreas);

	  sentence << "on";
	  
	  sentence << temperature_sentence(theVariable,
									   theMinimum,
									   theMaximum);

	  return sentence;
	}

	const Sentence construct_sentence(const std::string theVariable,
									  const forecast_area_id& theAreaId,
									  const unsigned short& theIncludedForecastArea,
									  const unsigned short& theForecastPeriod,
									  const forecast_period_id& thePeriodId, 
									  const forecast_season_id& theSeasonId,
									  weather_result_container_type& weatherResults)
	{
	  Sentence sentence;

	  int areaMin = (thePeriodId == DAY1_PERIOD ? AREA_MIN_DAY1 : 
					 (thePeriodId == NIGHT_PERIOD ? AREA_MIN_NIGHT : AREA_MIN_DAY2));
	  int areaMax = (thePeriodId == DAY1_PERIOD ? AREA_MAX_DAY1 : 
					 (thePeriodId == NIGHT_PERIOD ? AREA_MAX_NIGHT : AREA_MAX_DAY2));
	  int areaMean = (thePeriodId == DAY1_PERIOD ? AREA_MEAN_DAY1 : 
					  (thePeriodId == NIGHT_PERIOD ? AREA_MEAN_NIGHT : AREA_MEAN_DAY2));
	  int coastMin = (thePeriodId == DAY1_PERIOD ? COAST_MIN_DAY1 : 
					  (thePeriodId == NIGHT_PERIOD ? COAST_MIN_NIGHT : COAST_MIN_DAY2));
	  int coastMax = (thePeriodId == DAY1_PERIOD ? COAST_MAX_DAY1 : 
					  (thePeriodId == NIGHT_PERIOD ? COAST_MAX_NIGHT : COAST_MAX_DAY2));
	  int coastMean = (thePeriodId == DAY1_PERIOD ? COAST_MEAN_DAY1 : 
					   (thePeriodId == NIGHT_PERIOD ? COAST_MEAN_NIGHT : COAST_MEAN_DAY2));
	  int inlandMin = (thePeriodId == DAY1_PERIOD ? INLAND_MIN_DAY1 : 
					   (thePeriodId == NIGHT_PERIOD ? INLAND_MIN_NIGHT : INLAND_MIN_DAY2));
	  int inlandMax = (thePeriodId == DAY1_PERIOD ? INLAND_MAX_DAY1 : 
					   (thePeriodId == NIGHT_PERIOD ? INLAND_MAX_NIGHT : INLAND_MAX_DAY2));
	  int inlandMean = (thePeriodId == DAY1_PERIOD ? INLAND_MEAN_DAY1 : 
						(thePeriodId == NIGHT_PERIOD ? INLAND_MEAN_NIGHT : INLAND_MEAN_DAY2));

	  if(theAreaId == FULL_AREA)
		{
		  if(thePeriodId == DAY1_PERIOD)
			{
			  if(theSeasonId == SUMMER_SEASON && 
				 weatherResults[AREA_MAX_DAY1_MORNING]->value() > weatherResults[AREA_MAX_DAY1_AFTERNOON]->value())
				{
				  sentence << day1_sentence(theVariable,
											theSeasonId,
											FULL_AREA,
											DAY1_MORNING_PERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[AREA_MIN_DAY1_MORNING]->value(),
											weatherResults[AREA_MAX_DAY1_MORNING]->value(),
											weatherResults[AREA_MEAN_DAY1_MORNING]->value());

				  sentence << day1_sentence(theVariable,
											theSeasonId,
											FULL_AREA,
											DAY1_AFTERNOON_PERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[AREA_MIN_DAY1_AFTERNOON]->value(),
											weatherResults[AREA_MAX_DAY1_AFTERNOON]->value(),
											weatherResults[AREA_MEAN_DAY1_AFTERNOON]->value());
				}
			  else
				{
				  sentence << day1_sentence(theVariable,
											theSeasonId,
											FULL_AREA,
											UNDEFINED_SUBPERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[areaMin]->value(),
											weatherResults[areaMax]->value(),
											weatherResults[areaMean]->value());
				}

			}
		  else if(thePeriodId == NIGHT_PERIOD)
			{
			  sentence << night_sentence(theVariable,
										 theSeasonId,
										 FULL_AREA,
										 theIncludedForecastArea,
										 theForecastPeriod,
										 weatherResults[AREA_MAX_DAY1]->value(),
										 weatherResults[AREA_MEAN_DAY1]->value(),
										 weatherResults[areaMin]->value(),
										 weatherResults[areaMax]->value(),
										 weatherResults[areaMean]->value());
			}
		  else if(thePeriodId == DAY2_PERIOD)
			{
			  if(theSeasonId == SUMMER_SEASON && 
				 weatherResults[INLAND_MAX_DAY2_MORNING]->value() > weatherResults[INLAND_MAX_DAY2_AFTERNOON]->value())
				{
				  sentence << day2_sentence(theVariable,
											theSeasonId,
											FULL_AREA,
											DAY2_MORNING_PERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[AREA_MEAN_DAY1]->value(),
											weatherResults[AREA_MIN_DAY2_MORNING]->value(),
											weatherResults[AREA_MAX_DAY2_MORNING]->value(),
											weatherResults[AREA_MEAN_DAY2_MORNING]->value());

				  sentence << day2_sentence(theVariable,
											theSeasonId,
											FULL_AREA,
											DAY2_AFTERNOON_PERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[AREA_MEAN_DAY1]->value(),
											weatherResults[AREA_MIN_DAY2_AFTERNOON]->value(),
											weatherResults[AREA_MAX_DAY2_AFTERNOON]->value(),
											weatherResults[AREA_MEAN_DAY2_AFTERNOON]->value());
				}
			  else
				{
				  sentence << day2_sentence(theVariable,
											theSeasonId,
											FULL_AREA,
											UNDEFINED_SUBPERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[AREA_MEAN_DAY1]->value(),
											weatherResults[areaMin]->value(),
											weatherResults[areaMax]->value(),
											weatherResults[areaMean]->value());
				}
			}
		}
	  else if(theAreaId == INLAND_AREA)
		{
		  if(thePeriodId == DAY1_PERIOD)
			{
			  if(theSeasonId == SUMMER_SEASON && 
				 weatherResults[INLAND_MAX_DAY1_MORNING]->value() > weatherResults[INLAND_MAX_DAY1_AFTERNOON]->value())
				{
				  sentence << day1_sentence(theVariable,
											theSeasonId,
											INLAND_AREA,
											DAY1_MORNING_PERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[INLAND_MIN_DAY1_MORNING]->value(),
											weatherResults[INLAND_MAX_DAY1_MORNING]->value(),
											weatherResults[INLAND_MEAN_DAY1_MORNING]->value());

				  sentence << day1_sentence(theVariable,
											theSeasonId,
											INLAND_AREA,
											DAY1_AFTERNOON_PERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[INLAND_MIN_DAY1_AFTERNOON]->value(),
											weatherResults[INLAND_MAX_DAY1_AFTERNOON]->value(),
											weatherResults[INLAND_MEAN_DAY1_AFTERNOON]->value());
				}
			  else
				{
				  sentence << day1_sentence(theVariable,
											theSeasonId,
											INLAND_AREA,
											UNDEFINED_SUBPERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[inlandMin]->value(),
											weatherResults[inlandMax]->value(),
											weatherResults[inlandMean]->value());
				}
			}
		  else if(thePeriodId == NIGHT_PERIOD)
			{
			  sentence << night_sentence(theVariable,
										 theSeasonId,
										 INLAND_AREA,
										 theIncludedForecastArea,
										 UNDEFINED_SUBPERIOD,
										 weatherResults[INLAND_MAX_DAY1]->value(),
										 weatherResults[INLAND_MEAN_DAY1]->value(),
										 weatherResults[inlandMin]->value(),
										 weatherResults[inlandMax]->value(),
										 weatherResults[inlandMean]->value());
			}
		  else if(thePeriodId == DAY2_PERIOD)
			{
			  if(theSeasonId == SUMMER_SEASON && 
				 weatherResults[INLAND_MAX_DAY2_MORNING]->value() > weatherResults[INLAND_MAX_DAY2_AFTERNOON]->value())
				{
				  sentence << day2_sentence(theVariable,
											theSeasonId,
											FULL_AREA,
											DAY2_MORNING_PERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[INLAND_MEAN_DAY1]->value(),
											weatherResults[INLAND_MIN_DAY2_MORNING]->value(),
											weatherResults[INLAND_MAX_DAY2_MORNING]->value(),
											weatherResults[INLAND_MEAN_DAY2_MORNING]->value());

				  sentence << day2_sentence(theVariable,
											theSeasonId,
											FULL_AREA,
											DAY2_AFTERNOON_PERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[INLAND_MEAN_DAY1]->value(),
											weatherResults[INLAND_MIN_DAY2_AFTERNOON]->value(),
											weatherResults[INLAND_MAX_DAY2_AFTERNOON]->value(),
											weatherResults[INLAND_MEAN_DAY2_AFTERNOON]->value());
				}
			  else
				{
				  sentence << day2_sentence(theVariable,
											theSeasonId,
											INLAND_AREA,
											UNDEFINED_SUBPERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[INLAND_MEAN_DAY1]->value(),
											weatherResults[inlandMin]->value(),
											weatherResults[inlandMax]->value(),
											weatherResults[inlandMean]->value());
				}
			}
		}
	  else if(theAreaId == COASTAL_AREA)
		{
		  if(thePeriodId == DAY1_PERIOD)
			{
			  if(theSeasonId == SUMMER_SEASON && 
				 weatherResults[COAST_MAX_DAY1_MORNING]->value() > weatherResults[COAST_MAX_DAY1_AFTERNOON]->value())
				{
				  sentence << day1_sentence(theVariable,
											theSeasonId,
											COASTAL_AREA,
											DAY1_MORNING_PERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[COAST_MIN_DAY1_MORNING]->value(),
											weatherResults[COAST_MAX_DAY1_MORNING]->value(),
											weatherResults[COAST_MEAN_DAY1_MORNING]->value());

				  sentence << day1_sentence(theVariable,
											theSeasonId,
											COASTAL_AREA,
											DAY1_AFTERNOON_PERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[COAST_MIN_DAY1_AFTERNOON]->value(),
											weatherResults[COAST_MAX_DAY1_AFTERNOON]->value(),
											weatherResults[COAST_MEAN_DAY1_AFTERNOON]->value());

				}
			  else
				{
				  sentence << day1_sentence(theVariable,
											theSeasonId,
											COASTAL_AREA,
											UNDEFINED_SUBPERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[coastMin]->value(),
											weatherResults[coastMax]->value(),
											weatherResults[coastMean]->value());
				}
			}
		  else if(thePeriodId == NIGHT_PERIOD)
			{
			  sentence << night_sentence(theVariable,
										 theSeasonId,
										 COASTAL_AREA,
										 theIncludedForecastArea,
										 theForecastPeriod,
										 weatherResults[COAST_MAX_DAY1]->value(),
										 weatherResults[COAST_MEAN_DAY1]->value(),
										 weatherResults[coastMin]->value(),
										 weatherResults[coastMax]->value(),
										 weatherResults[coastMean]->value());
			}
		  else if(thePeriodId == DAY2_PERIOD)
			{
			  if(theSeasonId == SUMMER_SEASON && 
				 weatherResults[COAST_MAX_DAY2_MORNING]->value() > weatherResults[COAST_MAX_DAY2_AFTERNOON]->value())
				{
				  sentence << day2_sentence(theVariable,
											theSeasonId,
											FULL_AREA,
											DAY2_MORNING_PERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[COAST_MEAN_DAY1]->value(),
											weatherResults[COAST_MIN_DAY2_MORNING]->value(),
											weatherResults[COAST_MAX_DAY2_MORNING]->value(),
											weatherResults[COAST_MEAN_DAY2_MORNING]->value());

				  sentence << day2_sentence(theVariable,
											theSeasonId,
											FULL_AREA,
											DAY2_AFTERNOON_PERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[COAST_MEAN_DAY1]->value(),
											weatherResults[COAST_MIN_DAY2_AFTERNOON]->value(),
											weatherResults[COAST_MAX_DAY2_AFTERNOON]->value(),
											weatherResults[COAST_MEAN_DAY2_AFTERNOON]->value());
				}
			  else
				{
				  sentence << day2_sentence(theVariable,
											theSeasonId,
											COASTAL_AREA,
											UNDEFINED_SUBPERIOD,
											theIncludedForecastArea,
											theForecastPeriod,
											weatherResults[COAST_MEAN_DAY1]->value(),
											weatherResults[coastMin]->value(),
											weatherResults[coastMax]->value(),
											weatherResults[coastMean]->value());
				}
			}
		}

	  return sentence;
	}

	const Sentence temperature_max36hours_sentence(MessageLogger& log,
												   const std::string & theVariable,
												   const NightAndDayPeriodGenerator& theGenerator,
												   const unsigned short& forecast_area,
												   const unsigned short& theForecastPeriod,
												   weather_result_container_type& weatherResults)
	{
	  Sentence sentence;

	  const int temperature_limit_coast_inland = optional_int(theVariable + "::temperature_limit_coast_inland", 2);

	  forecast_season_id forecast_season(UNDEFINED_SEASON);

	  // find out seasons of the forecast periods
	  if(theForecastPeriod & DAY1_PERIOD)
		{
		  forecast_season = get_forecast_season(theGenerator.period(1).localStartTime(), theVariable);
		}
	  else if(theForecastPeriod & NIGHT_PERIOD)
		{
		  forecast_season = get_forecast_season(theGenerator.period(1).localStartTime(), theVariable);
		}

	  // Day1, Night, Day2
	  // 1. Day1 inland 
	  // 2. Day2 inland
	  // 3. Night inland
	  // 4. Day1 coastal
	  // 5. Day2 coastal
	  // 6. Night coastal

	  // Day1, Night
	  // 1. Day1 inland
	  // 2. Day1 coastal
	  // 3. Night inland
	  // 4. Night coastal

	  // Night, Day2
	  // 1. Night inland
	  // 2. Night coastal
	  // 3. Day2 inland
	  // 4. Day2 coastal

	  vector<int> periodAreas;
	  processing_order processingOrder;

	  if(theForecastPeriod & DAY1_PERIOD && theForecastPeriod & NIGHT_PERIOD && theForecastPeriod & DAY2_PERIOD)
		{
		  processingOrder = DAY1_DAY2_NIGHT;
		}
	  else if(theForecastPeriod & NIGHT_PERIOD && theForecastPeriod & DAY2_PERIOD)
		{
		  processingOrder = NIGHT_DAY2;
		}
	  else if(theForecastPeriod & DAY1_PERIOD && theForecastPeriod & NIGHT_PERIOD)
		{
		  processingOrder = DAY1_NIGHT;
		}

	  float temperature_diff_day1 = abs(round(weatherResults[INLAND_MEAN_DAY1]->value() - 
											  weatherResults[COAST_MEAN_DAY1]->value()));
		  
	  bool separate_inland_and_coast_day1 = (temperature_diff_day1 >= temperature_limit_coast_inland);

	  float temperature_diff_day2 = abs(weatherResults[INLAND_MEAN_DAY2]->value() - 
										weatherResults[COAST_MEAN_DAY2]->value());
		  
	  bool separate_inland_and_coast_day2 = (temperature_diff_day2 >= temperature_limit_coast_inland);
			  
	  float temperature_diff_night = abs(weatherResults[INLAND_MEAN_NIGHT]->value() - 
										 weatherResults[COAST_MEAN_NIGHT]->value());
		  
	  bool separate_inland_and_coast_night = (temperature_diff_night >= temperature_limit_coast_inland);


	  if(processingOrder == DAY1_DAY2_NIGHT)
		{
		  if(separate_inland_and_coast_day1)
			  periodAreas.push_back(DAY1_INLAND);
		  else
			  periodAreas.push_back(DAY1_FULL);

		  if(separate_inland_and_coast_day2)
			  periodAreas.push_back(DAY2_INLAND);
		  else
			  periodAreas.push_back(DAY2_FULL);

		  if(separate_inland_and_coast_night)
			  periodAreas.push_back(NIGHT_INLAND);
		  else
			  periodAreas.push_back(NIGHT_FULL);

		  if(separate_inland_and_coast_day1)
			  periodAreas.push_back(DAY1_COASTAL);

		  if(separate_inland_and_coast_day2)
			  periodAreas.push_back(DAY2_COASTAL);

		  if(separate_inland_and_coast_night)
			  periodAreas.push_back(NIGHT_COASTAL);
		}
	  else if(processingOrder == DAY1_NIGHT)
		{
		  if(separate_inland_and_coast_day1)
			{
			  periodAreas.push_back(DAY1_INLAND);
			  periodAreas.push_back(DAY1_COASTAL);
			}
		  else
			{
			  periodAreas.push_back(DAY1_FULL);
			}

		  if(separate_inland_and_coast_night)
			{
			  periodAreas.push_back(NIGHT_INLAND);
			  periodAreas.push_back(NIGHT_COASTAL);
			}
		  else
			{
			  periodAreas.push_back(NIGHT_FULL);
			}
		}
	  else if(processingOrder == NIGHT_DAY2)
		{
		  if(separate_inland_and_coast_night)
			{
			  periodAreas.push_back(NIGHT_INLAND);
			  periodAreas.push_back(NIGHT_COASTAL);
			}
		  else
			{
			  periodAreas.push_back(NIGHT_FULL);
			}

		  if(separate_inland_and_coast_day2)
			{
			  periodAreas.push_back(DAY2_INLAND);
			  periodAreas.push_back(DAY2_COASTAL);
			}
		  else
			{
			  periodAreas.push_back(DAY2_FULL);
			}
		}

	  for(unsigned int i = 0; i < periodAreas.size(); i++)
		{
		  unsigned short story_forecast_areas = 0x0;
		  forecast_period_id period_id;
		  forecast_area_id area_id;

		  if(periodAreas[i] == DAY1_INLAND || periodAreas[i] == DAY1_COASTAL)
			{
			  period_id = DAY1_PERIOD;

			  if(separate_inland_and_coast_day1)
				{
				  area_id = periodAreas[i] == DAY1_INLAND ? INLAND_AREA : COASTAL_AREA;
				  story_forecast_areas |= COASTAL_AREA;
				  story_forecast_areas |= INLAND_AREA;
				}
			  else
				{
				  area_id = FULL_AREA;
				  story_forecast_areas |= FULL_AREA;
				}
			}
		  else if(periodAreas[i] == NIGHT_INLAND || periodAreas[i] == NIGHT_COASTAL)
			{	
			  period_id = NIGHT_PERIOD;
			  if(separate_inland_and_coast_night)
				{
				  area_id = periodAreas[i] == NIGHT_INLAND ? INLAND_AREA : COASTAL_AREA;
				  story_forecast_areas |= COASTAL_AREA;
				  story_forecast_areas |= INLAND_AREA;
				}
			  else
				{
				  area_id = FULL_AREA;
				  story_forecast_areas |= FULL_AREA;
				}
			}
		  else
			{
			  period_id = DAY2_PERIOD;
			  if(separate_inland_and_coast_day2)
				{
				  area_id = periodAreas[i] == DAY2_INLAND ? INLAND_AREA : COASTAL_AREA;
				  story_forecast_areas |= COASTAL_AREA;
				  story_forecast_areas |= INLAND_AREA;
				}
			  else
				{
				  area_id = FULL_AREA;
				  story_forecast_areas |= FULL_AREA;
				}
			}

		  Sentence addThisSentence;
		  addThisSentence << construct_sentence(theVariable,
												area_id,
												story_forecast_areas,
												theForecastPeriod,
												period_id,
												forecast_season,
												weatherResults);

		  if(!addThisSentence.empty())
			{
			  if(!sentence.empty())
				sentence << Delimiter(",");

			  sentence << addThisSentence;
			}
		}

	  return sentence;
	}

  } // namespace TemperatureMax36Hours


  bool valid_value_period_check(const float& value, unsigned short& forecast_period, const unsigned short& mask)
  {
	bool retval = (value != kFloatMissing);

	if(!retval)
	  forecast_period &= ~mask;

	return retval;
  }

  
  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on temperature for the day and night
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  const Paragraph TemperatureStory::max36hours() const
  {
	using namespace TemperatureMax36Hours;

	Paragraph paragraph;
	MessageLogger log("TemperatureStory::max36h");

	//	NFmiSettings::Set("textgen::part1::story::temperature_max36hours::day::starthour","6");
	//NFmiSettings::Set("textgen::part1::story::temperature_max36hours::day::endhour","18");

	/*

	WeatherResult theMin(kFloatMissing, 0);
	WeatherResult theMax(kFloatMissing, 0);
	WeatherResult theMean(kFloatMissing, 0);

	morning_temperature(itsVar,
	itsSources,
	itsArea,
						itsPeriod,
						theMin,
						theMax,
						theMean);
		day_temperature(itsVar,
						  itsSources,
						  itsArea,
						  itsPeriod,
						  theMin,
						  theMax,
						  theMean);
	*/

	// Period generator
	NightAndDayPeriodGenerator generator(itsPeriod, itsVar);

	unsigned short forecast_area = 0x0;
	unsigned short forecast_period = 0x0;
	forecast_season_id forecast_season = get_forecast_season(generator.period(1).localStartTime(), itsVar);
		      
	// Too late? Return empty story then
	if(generator.size() == 0)
	  {
		log << paragraph;
		return paragraph;
	  }


	log << "Period contains " << generator.size() << " days or nights" << endl;


	// Calculate the results

	weather_result_container_type weatherResults;

	GridForecaster forecaster;

	if(generator.isday(1))
	  {
		// when the first period is day, and the second period exists 
		// it must be night, and if third period exists it must be day
		forecast_period |= DAY1_PERIOD;
		forecast_period |= (generator.size() > 1 ? NIGHT_PERIOD : 0x0);
		forecast_period |= (generator.size() > 2 ? DAY2_PERIOD : 0x0);
	  }
	else
	  {
		// if the first period is not day, it must be night, and
		// if second period exists it must be day
		forecast_period |= NIGHT_PERIOD;
		forecast_period |= (generator.size() > 1 ? DAY2_PERIOD : 0x0);
	  }


	// Initialize the container for WeatherResult variables
	for(int i = AREA_MIN_DAY1; i < UNDEFINED_WEATHER_RESULT_ID; i++)
	  {
		weatherResults.insert(make_pair(i, new WeatherResult(kFloatMissing, 0)));
	  } 

	WeatherPeriod period = generator.period(1);
	if(forecast_period & DAY1_PERIOD)
	  {
		// calculate results for day1
		log_start_time_and_end_time(log, "Day1: ", period);

		calculate_results(log, itsVar, itsSources, itsArea,
						  period, DAY1_PERIOD, forecast_season, INLAND_AREA, weatherResults);
		//if(weatherResults[INLAND_MIN_DAY1]->value() == kFloatMissing)
		//forecast_period &= ~DAY1_PERIOD;
		if(forecast_period & DAY1_PERIOD)
		  {
			calculate_results(log, itsVar, itsSources, itsArea,
							  period, DAY1_PERIOD, forecast_season, COASTAL_AREA, weatherResults);
			//if(weatherResults[COAST_MIN_DAY1]->value() == kFloatMissing)
			//forecast_period &= ~DAY1_PERIOD;
		  }
		if(forecast_period & DAY1_PERIOD)
		  {
			calculate_results(log, itsVar, itsSources, itsArea,
						  period, DAY1_PERIOD, forecast_season, FULL_AREA, weatherResults);
			//if(weatherResults[AREA_MIN_DAY1]->value() == kFloatMissing)
			//forecast_period &= ~DAY1_PERIOD;
		  }

		// day1 period exists, so
		// if the area is included, it must have valid values
		forecast_area |= (weatherResults[COAST_MIN_DAY1]->value() != kFloatMissing ? COASTAL_AREA : 0x0); 
		forecast_area |= (weatherResults[INLAND_MIN_DAY1]->value() != kFloatMissing ? INLAND_AREA : 0x0);

		if(forecast_area == NO_AREA)
		  {
			log << "Something wrong, NO Coastal area NOR Inland area is included! " << endl;
		  }
		else
		  {
			if(forecast_period & NIGHT_PERIOD)
			  {
				// calculate results for night
				period = generator.period(2);
				log_start_time_and_end_time(log, "Night: ", period);
				
				if(forecast_area & INLAND_AREA)
				  {
					calculate_results(log, itsVar, itsSources, itsArea,
									  period, NIGHT_PERIOD, forecast_season, INLAND_AREA, weatherResults);
					valid_value_period_check(weatherResults[INLAND_MIN_NIGHT]->value(), forecast_period, NIGHT_PERIOD);
				  }
				if(forecast_area & COASTAL_AREA && (forecast_period & NIGHT_PERIOD))
				  {
					calculate_results(log, itsVar, itsSources, itsArea,
									  period, NIGHT_PERIOD, forecast_season, COASTAL_AREA, weatherResults);
					valid_value_period_check(weatherResults[COAST_MIN_NIGHT]->value(), forecast_period, NIGHT_PERIOD);
				  }
				if((forecast_area & INLAND_AREA && forecast_area & COASTAL_AREA) && (forecast_period & NIGHT_PERIOD))
				  {
					calculate_results(log, itsVar, itsSources, itsArea,
									  period, NIGHT_PERIOD, forecast_season, FULL_AREA, weatherResults);
					valid_value_period_check(weatherResults[AREA_MIN_NIGHT]->value(), forecast_period, NIGHT_PERIOD);
				  }
			  }
			
			if(forecast_period & DAY2_PERIOD)
			  {
				// calculate results for day2
				period = generator.period(3);
				log_start_time_and_end_time(log, "Day2: ", period);

				if(forecast_area & INLAND_AREA)
				  {
					calculate_results(log, itsVar, itsSources, itsArea,
									  period, DAY2_PERIOD, forecast_season, INLAND_AREA, weatherResults);
					valid_value_period_check(weatherResults[INLAND_MIN_NIGHT]->value(), forecast_period, DAY2_PERIOD);
				  }
				if(forecast_area & COASTAL_AREA && (forecast_period & DAY2_PERIOD))
				  {
					calculate_results(log, itsVar, itsSources, itsArea,
									  period, DAY2_PERIOD, forecast_season, COASTAL_AREA, weatherResults);
					valid_value_period_check(weatherResults[COAST_MIN_NIGHT]->value(), forecast_period, DAY2_PERIOD);
				  }
				if((forecast_area & INLAND_AREA && forecast_area & COASTAL_AREA) && (forecast_period & NIGHT_PERIOD))
				  {
					calculate_results(log, itsVar, itsSources, itsArea,
									  period, DAY2_PERIOD, forecast_season, FULL_AREA, weatherResults);
					valid_value_period_check(weatherResults[AREA_MIN_NIGHT]->value(), forecast_period, DAY2_PERIOD);
				  }
			  }
		  }
	  }
	else
	  {
		// calculate results for night
		log_start_time_and_end_time(log, "Night: ", period);

		calculate_results(log, itsVar, itsSources, itsArea,
						  period, NIGHT_PERIOD, forecast_season, INLAND_AREA, weatherResults);
		calculate_results(log, itsVar, itsSources, itsArea,
						  period, NIGHT_PERIOD, forecast_season, COASTAL_AREA, weatherResults);
		calculate_results(log, itsVar, itsSources, itsArea,
						  period, NIGHT_PERIOD, forecast_season, FULL_AREA, weatherResults);

		// night period exists, so
		// if the area is included, it must have valid values
		forecast_area |= (weatherResults[COAST_MIN_NIGHT]->value() != kFloatMissing ? COASTAL_AREA : 0x0); 
		forecast_area |= (weatherResults[INLAND_MIN_NIGHT]->value() != kFloatMissing ? INLAND_AREA : 0x0); 

		if(forecast_area == NO_AREA)
		  {
			log << "Something wrong, NO Coastal area NOR Inland area is included! " << endl;
		  }
		else
		  {
			if(forecast_period & DAY2_PERIOD)
			  {
				// calculate results for day2
				period = generator.period(2);
				log_start_time_and_end_time(log, "Day2: ", period);
				
				if(forecast_area & INLAND_AREA)
				  {
					calculate_results(log, itsVar, itsSources, itsArea,
									  period, DAY2_PERIOD, forecast_season, INLAND_AREA, weatherResults);
					valid_value_period_check(weatherResults[INLAND_MIN_DAY2]->value(), forecast_period, DAY2_PERIOD);
				  }

				if(forecast_area & COASTAL_AREA && (forecast_period & DAY2_PERIOD))
				  {
					calculate_results(log, itsVar, itsSources, itsArea,
									  period, DAY2_PERIOD, forecast_season, COASTAL_AREA, weatherResults);
					valid_value_period_check(weatherResults[INLAND_MIN_DAY2]->value(), forecast_period, DAY2_PERIOD);
				  }

				if(forecast_area & INLAND_AREA && forecast_area & COASTAL_AREA&& (forecast_period & DAY2_PERIOD))
				  calculate_results(log, itsVar, itsSources, itsArea,
									period, DAY2_PERIOD, forecast_season, FULL_AREA, weatherResults);
			  }
		  }
	  }

	bool res = forecast_area | (INLAND_AREA & COASTAL_AREA);
	res = forecast_area & (INLAND_AREA | COASTAL_AREA);
	res = (forecast_area & INLAND_AREA && forecast_area & COASTAL_AREA);

	if(forecast_area != NO_AREA)
	  paragraph << temperature_max36hours_sentence(log, itsVar, generator, forecast_area, forecast_period, weatherResults);

	log_weather_results(log, weatherResults);


	// delete the allocated WeatherResult-objects
	for(int i = AREA_MIN_DAY1; i < UNDEFINED_WEATHER_RESULT_ID; i++)
	  {
		delete weatherResults[i];
	  } 


	log << paragraph;
	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================
  
