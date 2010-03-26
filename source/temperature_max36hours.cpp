// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::TemperatureStory::max36hours
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

  namespace TemperatureMax36Hours
  {
	using NFmiStringTools::Convert;
	using namespace WeatherAnalysis;
	using namespace TextGen::TemperatureStoryTools;
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

	enum forecast_season_id{SUMMER_SEASON, 
							WINTER_SEASON,
							UNDEFINED_SEASON};

	// COAST_MEAN_DAY2
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

		  minResult = do_calculation(theVar+"::min",
									 theSources,
									 Minimum,
									 Mean   ,
									 theActualArea,								
									 thePeriod);

		  maxResult = do_calculation(theVar+"::max",
									 theSources,
									 Maximum,
									 Mean,
									 theActualArea,
									 thePeriod);

		  meanResult = do_calculation(theVar+"::mean",
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


	const forecast_season_id get_forecast_season(const NFmiTime& theTime)
	{
	  return (SeasonTools::isSummerHalf(theTime) ? SUMMER_SEASON : WINTER_SEASON);
	}

	const Sentence value_dependent_sentence(const std::string theVariable,
											const double& min_temperature,
											const double& max_temperature,
											const double& mean_temperature)
	{
	  Sentence retval;

	  const string range_separator = optional_string(theVariable + "::rangeseparator", "...");
	  const int min_interval = optional_int(theVariable + "::min_interval", 2);
	  
	  retval = temperature_sentence(static_cast<int>(round(min_temperature)),
									static_cast<int>(round(mean_temperature)),
									static_cast<int>(round(max_temperature)),
									min_interval,
									false,
									range_separator);
	  
	  return retval;
	}

	
	const Sentence season_dependent_sentence(const forecast_period_id& period_id, const forecast_season_id& season_id)
	{
	  Sentence sentence;

	  if(season_id == SUMMER_SEASON)
		{
		  if(period_id == NIGHT_PERIOD)
			sentence << "yön alin lämpötila";
		  else
			sentence << "päivän ylin lämpötila";
		}
	  else
		{
		  if(period_id == NIGHT_PERIOD)
			sentence << "yölämpötila";
		  else
			sentence << "päivälämpötila";
		}

		sentence << "on";

	  return sentence;
	}

	const Sentence construct_sentence(const std::string theVariable,
									  const unsigned short& included_forecast_area,
									  const forecast_period_id& period_id, 
									  const forecast_season_id& season_id,
									  weather_result_container_type& weatherResults)
	{
	  Sentence sentence;

	  if(season_id == SUMMER_SEASON)
		{
		  if(period_id == NIGHT_PERIOD)
			sentence << "yön alin lämpötila";
		  else
			sentence << "päivän ylin lämpötila";
		}
	  else
		{
		  if(period_id == NIGHT_PERIOD)
			sentence << "yölämpötila";
		  else
			sentence << "päivälämpötila";
		}

	  sentence << "on";

	  if(period_id == DAY1_PERIOD)
		{
		  sentence << "tänään";
		}
	  else if(period_id == DAY2_PERIOD)
		{
		  sentence << "huomenna";
		}

	  int areaMin = (period_id == DAY1_PERIOD ? AREA_MIN_DAY1 : 
					 (period_id == NIGHT_PERIOD ? AREA_MIN_NIGHT : AREA_MIN_DAY2));
	  int areaMax = (period_id == DAY1_PERIOD ? AREA_MAX_DAY1 : 
					 (period_id == NIGHT_PERIOD ? AREA_MAX_NIGHT : AREA_MAX_DAY2));
	  int areaMean = (period_id == DAY1_PERIOD ? AREA_MEAN_DAY1 : 
					  (period_id == NIGHT_PERIOD ? AREA_MEAN_NIGHT : AREA_MEAN_DAY2));
	  int coastMin = (period_id == DAY1_PERIOD ? COAST_MIN_DAY1 : 
					  (period_id == NIGHT_PERIOD ? COAST_MIN_NIGHT : COAST_MIN_DAY2));
	  int coastMax = (period_id == DAY1_PERIOD ? COAST_MAX_DAY1 : 
					  (period_id == NIGHT_PERIOD ? COAST_MAX_NIGHT : COAST_MAX_DAY2));
	  int coastMean = (period_id == DAY1_PERIOD ? COAST_MEAN_DAY1 : 
					   (period_id == NIGHT_PERIOD ? COAST_MEAN_NIGHT : COAST_MEAN_DAY2));
	  int inlandMin = (period_id == DAY1_PERIOD ? INLAND_MIN_DAY1 : 
					   (period_id == NIGHT_PERIOD ? INLAND_MIN_NIGHT : INLAND_MIN_DAY2));
	  int inlandMax = (period_id == DAY1_PERIOD ? INLAND_MAX_DAY1 : 
					   (period_id == NIGHT_PERIOD ? INLAND_MAX_NIGHT : INLAND_MAX_DAY2));
	  int inlandMean = (period_id == DAY1_PERIOD ? INLAND_MEAN_DAY1 : 
						(period_id == NIGHT_PERIOD ? INLAND_MEAN_NIGHT : INLAND_MEAN_DAY2));


	  if(included_forecast_area & FULL_AREA)
		{
		  sentence << value_dependent_sentence(theVariable,
											   weatherResults[areaMin]->value(),
											   weatherResults[areaMax]->value(),
											   weatherResults[areaMean]->value());
		}
	  else if(included_forecast_area & COASTAL_AREA  && included_forecast_area & INLAND_AREA)
		{
		  sentence << value_dependent_sentence(theVariable,
											   weatherResults[inlandMin]->value(),
											   weatherResults[inlandMax]->value(),
											   weatherResults[inlandMean]->value());

		  sentence << Delimiter(",");
		  sentence << "rannikolla";

		  sentence << value_dependent_sentence(theVariable,
											   weatherResults[coastMin]->value(),
											   weatherResults[coastMax]->value(),
											   weatherResults[coastMean]->value());
		}
	  else if(included_forecast_area & INLAND_AREA)
		{
		  sentence << value_dependent_sentence(theVariable,
											   weatherResults[inlandMin]->value(),
											   weatherResults[inlandMax]->value(),
											   weatherResults[inlandMean]->value());
		}
	  else if(included_forecast_area & COASTAL_AREA)
		{
		  sentence << value_dependent_sentence(theVariable,
											   weatherResults[coastMin]->value(),
											   weatherResults[coastMax]->value(),
											   weatherResults[coastMean]->value());
		}

	  return sentence;
	}

	const Sentence temperature_max36hours_sentence(MessageLogger& log,
												   const std::string & theVariable,
												   const NightAndDayPeriodGenerator& theGenerator,
												   const unsigned short& forecast_area,
												   const unsigned short& forecast_periods,
												   weather_result_container_type& weatherResults)
	{
	  Sentence sentence;

	  const int temperature_limit_coast_inland = optional_int(theVariable + "::temperature_limit_coast_inland", 2);

	  forecast_season_id day1_season(UNDEFINED_SEASON);
	  forecast_season_id night_season(UNDEFINED_SEASON);
	  forecast_season_id day2_season(UNDEFINED_SEASON);

	  // find out seasons of the forecast periods
	  if(forecast_periods & DAY1_PERIOD)
		{
		  day1_season = get_forecast_season(theGenerator.period(1).localStartTime());
		}
	  if(forecast_periods & NIGHT_PERIOD)
		{
		  night_season = get_forecast_season(theGenerator.period((forecast_periods & DAY1_PERIOD ? 2 : 1)).localStartTime());
		}
	  if(forecast_periods & DAY2_PERIOD)
		{
		  day2_season = get_forecast_season(theGenerator.period((forecast_periods & DAY1_PERIOD ? 3 : 2)).localStartTime());
		}

	  if(forecast_area & COASTAL_AREA && forecast_area & INLAND_AREA)
		{
		  // both inland and coastal areas are included

		  if(forecast_periods & DAY1_PERIOD)
			{
			  float temperature_diff_day1 = abs(round(weatherResults[INLAND_MEAN_DAY1]->value() - 
													  weatherResults[COAST_MEAN_DAY1]->value()));

			  unsigned short story_forecast_areas = 0x0;
			  if(temperature_diff_day1 < temperature_limit_coast_inland)
				{
				  story_forecast_areas |= FULL_AREA;
				}
			  else
				{
				  story_forecast_areas |= COASTAL_AREA;
				  story_forecast_areas |= INLAND_AREA;
				}

			  sentence << construct_sentence(theVariable,
											 story_forecast_areas,
											 DAY1_PERIOD,
											 day1_season,
											 weatherResults);
			}

		  if(forecast_periods & NIGHT_PERIOD)
			{

			  float temperature_diff_night = abs(weatherResults[INLAND_MEAN_NIGHT]->value() - 
												 weatherResults[COAST_MEAN_NIGHT]->value());

			  unsigned short story_forecast_areas = 0x0;
			  if(temperature_diff_night < temperature_limit_coast_inland)
				{
				  story_forecast_areas |= FULL_AREA;
				}
			  else
				{
				  story_forecast_areas |= COASTAL_AREA;
				  story_forecast_areas |= INLAND_AREA;
				}

			  if(forecast_periods & DAY1_PERIOD)
				sentence << Delimiter(",");

			  sentence << construct_sentence(theVariable,
											 story_forecast_areas,
											 NIGHT_PERIOD,
											 day1_season,
											 weatherResults);
			}

		  if(forecast_periods & DAY2_PERIOD)
			{
			  float temperature_diff_day2 = abs(weatherResults[INLAND_MEAN_DAY2]->value() - 
												weatherResults[COAST_MEAN_DAY2]->value());
			  
			  unsigned short story_forecast_areas = 0x0;
			  if(temperature_diff_day2 < temperature_limit_coast_inland)
				{
				  story_forecast_areas |= FULL_AREA;
				}
			  else
				{
				  story_forecast_areas |= COASTAL_AREA;
				  story_forecast_areas |= INLAND_AREA;
				}

			  if(forecast_periods & NIGHT_PERIOD)
				sentence << Delimiter(",");

			  sentence << construct_sentence(theVariable,
											 story_forecast_areas,
											 DAY2_PERIOD,
											 day1_season,
											 weatherResults);
			}
		}
	  else
		{
		  // either inland or coastal area is included

		  unsigned short story_forecast_areas = (forecast_area & INLAND_AREA ? INLAND_AREA : COASTAL_AREA);

		  if(forecast_periods & DAY1_PERIOD)
			{
			  sentence << construct_sentence(theVariable,
											 story_forecast_areas,
											 DAY1_PERIOD,
											 day1_season,
											 weatherResults);
			}
		  
		  if(forecast_periods & NIGHT_PERIOD)
			{
			  if(forecast_periods & DAY1_PERIOD)
				sentence << Delimiter(",");

			  sentence << construct_sentence(theVariable,
											 story_forecast_areas,
											 NIGHT_PERIOD,
											 day1_season,
											 weatherResults);
			}

		  if(forecast_periods & DAY2_PERIOD)
			{
			  sentence << Delimiter(",");

			  sentence << construct_sentence(theVariable,
											 story_forecast_areas,
											 DAY2_PERIOD,
											 day1_season,
											 weatherResults);
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

	// Period generator
	NightAndDayPeriodGenerator generator(itsPeriod, itsVar);

	unsigned short forecast_area = 0x0;
	unsigned short forecast_period = 0x0;
	forecast_season_id forecast_season = get_forecast_season(generator.period(1).localStartTime());
		      
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
	for(int i = AREA_MIN_DAY1; i < WEATHER_RESULT_ID_UNDEFINED; i++)
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
	for(int i = AREA_MIN_DAY1; i < WEATHER_RESULT_ID_UNDEFINED; i++)
	  {
		delete weatherResults[i];
	  } 


	log << paragraph;
	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================
  
