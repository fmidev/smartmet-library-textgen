// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::TemperatureStory::day
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


	enum forecast_area_id{COASTAL_AREA = 0x1, 
						  INLAND_AREA = 0x2};

	enum forecast_period_id{DAY1_PERIOD = 0x1, 
							NIGHT_PERIOD = 0x2,
							DAY2_PERIOD = 0x4};

	enum forecast_season_id{SUMMER_SEASON, 
							WINTER_SEASON,
							UNDEFINED_SEASON};

	enum fractile_id{FRACTILE_02,
					 FRACTILE_12,
					 FRACTILE_37,
					 FRACTILE_50,
					 FRACTILE_63,
					 FRACTILE_88,
					 FRACTILE_98,
					 FRACTILE_100,
				     FRACTILE_UNDEFINED};

	
	enum weather_result_id{AREA_MEAN_MAX_DAY1 = 0x1,
						   AREA_MIN_MAX_DAY1,
						   AREA_MAX_MAX_DAY1,
						   AREA_MEAN_MIN_NIGHT,
						   AREA_MIN_MIN_NIGHT,
						   AREA_MAX_MIN_NIGHT,
						   AREA_MEAN_MAX_DAY2,
						   AREA_MIN_MAX_DAY2,
						   AREA_MAX_MAX_DAY2,
						   INLAND_MEAN_MAX_DAY1,
						   INLAND_MIN_MAX_DAY1,
						   INLAND_MAX_MAX_DAY1,
						   INLAND_MEAN_MIN_NIGHT,
						   INLAND_MIN_MIN_NIGHT,
						   INLAND_MAX_MIN_NIGHT,
						   INLAND_MEAN_MAX_DAY2,
						   INLAND_MIN_MAX_DAY2,
						   INLAND_MAX_MAX_DAY2,
						   COAST_MEAN_MAX_DAY1,
						   COAST_MIN_MAX_DAY1,
						   COAST_MAX_MAX_DAY1,
						   COAST_MEAN_MIN_NIGHT,
						   COAST_MIN_MIN_NIGHT,
						   COAST_MAX_MIN_NIGHT,
						   COAST_MEAN_MAX_DAY2,
						   COAST_MIN_MAX_DAY2,
						   COAST_MAX_MAX_DAY2};

	class Max36HoursResults
	{
	public:

	  Max36HoursResults(): m_weatherResult(new WeatherResult(kFloatMissing, 0.0)), 
						   m_fractile(FRACTILE_UNDEFINED)
	  {}

	  ~Max36HoursResults() { delete m_weatherResult; }

	  WeatherResult& weatherResult() { return *m_weatherResult; }
	  fractile_id& fractile() { return m_fractile; }
	  float value() const { return m_weatherResult->value(); }

	private:

	  WeatherResult* m_weatherResult;
	  fractile_id    m_fractile;
	};


	typedef map<int, Max36HoursResults*> weather_result_container_type;
	typedef weather_result_container_type::value_type value_type;



	fractile_id get_fractile(MessageLogger& theLog,
								  const float& theValue,
								  const std::string& theLogMessage, 
								  const std::string& theFakeVariable,
								  const WeatherArea& theArea,  
								  const WeatherPeriod& thePeriod,
								  const AnalysisSources& theSources)
	{
	  std::string dataName("textgen::fractiles");
	  
	  WeatherPeriod climateperiod = ClimatologyTools::getClimatologyPeriod(thePeriod, dataName, theSources);
	  
	  GridClimatology gc;
	  
	  WeatherResult result(kFloatMissing, 0.0);

	  result = gc.analyze(theFakeVariable,
						  theSources,
						  NormalMaxTemperatureF02,
						  Minimum,
						  Minimum,
						  theArea,
						  climateperiod);

	  if(result.value() != kFloatMissing && theValue <= result.value())
		return FRACTILE_02;
	  
	  result = gc.analyze(theFakeVariable,
						  theSources,
						  NormalMaxTemperatureF12,
						  Minimum,
						  Minimum,
						  theArea,
						  climateperiod);
	  
	  if(result.value() != kFloatMissing && theValue <= result.value())
		return FRACTILE_12;

	  result = gc.analyze(theFakeVariable,
						  theSources,
						  NormalMaxTemperatureF37,
						  Minimum,
						  Minimum,
						  theArea,
						  climateperiod);
	  
	  if(result.value() != kFloatMissing && theValue <= result.value())
		return FRACTILE_37;
	  
	  result = gc.analyze(theFakeVariable,
						  theSources,
						  NormalMaxTemperatureF50,
						  Minimum,
						  Minimum,
						  theArea,
						  climateperiod);

	  if(result.value() != kFloatMissing && theValue <= result.value())
		return FRACTILE_50;
	  
	  result = gc.analyze(theFakeVariable,
						  theSources,
						  NormalMaxTemperatureF63,
						  Maximum,
						  Maximum,
						  theArea,
						  climateperiod);
	  
	  if(result.value() != kFloatMissing && theValue <= result.value())
		return FRACTILE_63;
	  
	  result = gc.analyze(theFakeVariable,
						  theSources,
						  NormalMaxTemperatureF88,
						  Maximum,
						  Maximum,
						  theArea,
						  climateperiod);
	  
	  if(result.value() != kFloatMissing && theValue <= result.value())
		return FRACTILE_88;

	  result = gc.analyze(theFakeVariable,
						  theSources,
						  NormalMaxTemperatureF98,
						  Maximum,
						  Maximum,
						  theArea,
						  climateperiod);
		
		


	  if(result.value() != kFloatMissing && theValue <= result.value())
		return FRACTILE_98;
	  else if(result.value() != kFloatMissing)
		return FRACTILE_100;
	  else
		return FRACTILE_UNDEFINED;
	}



	// ----------------------------------------------------------------------
	/*!
	 * \brief Calculate area results
	 */
	// ----------------------------------------------------------------------

	void calculate_mean_min_max(MessageLogger& theLog,
								const WeatherForecaster& theForecaster,
								const string& theVar,
								const AnalysisSources& theSources,
								const WeatherArea& theArea,
								const WeatherPeriod& thePeriod,
								WeatherFunction theFunction,
								WeatherResult& theMean,
								WeatherResult& theMin,
								WeatherResult& theMax,
								fractile_id& theFractileMin,
								fractile_id& theFractileMax)
	{
	  theMean = theForecaster.analyze(theVar+"::mean",
									  theSources,
									  Temperature,
									  Mean,
									  theFunction,
									  theArea,
									  thePeriod);
	  
	  theMin = theForecaster.analyze(theVar+"::min",
									 theSources,
									 Temperature,
									 Minimum,
									 theFunction,
									 theArea,
									 thePeriod);
	  
	  theMax = theForecaster.analyze(theVar+"::max",
									 theSources,
									 Temperature,
									 Maximum,
									 theFunction,		
		

									 theArea,
									 thePeriod);


	  theFractileMin = get_fractile(theLog,
									theMin.value(),
									"", 
									theVar,
									theArea,  
									thePeriod,
									theSources);

	  theFractileMax = get_fractile(theLog,
									theMax.value(),
									"", 
									theVar,
									theArea,  
									thePeriod,
									theSources);


	  theLog << "Temperature Mean(Maximum) " << theMean << endl;
	  theLog << "Temperature Min(Maximum) " << theMin << endl;
	  theLog << "Temperature Max(Maximum) " << theMax << endl;
	  theLog << "Temperature Fractile(Min) " << Convert(static_cast<int>(theFractileMin)) << endl;
	  theLog << "Temperature Fractile(Max) " << Convert(static_cast<int>(theFractileMax)) << endl;
	  
	}


 
	void calculate_weather_results(MessageLogger& theLog,
								   const WeatherForecaster& theForecaster,
								   const string& theVar,
								   const AnalysisSources& theSources,
								   const WeatherArea& theArea,
								   const WeatherPeriod& thePeriod,
								   const forecast_period_id& forecast_period,
								   weather_result_container_type& weatherResults)
	{
	  WeatherArea full = theArea;
	  full.type(WeatherArea::Full);
	  
	  WeatherArea coast = theArea;
	  coast.type(WeatherArea::Coast);
	  
	  WeatherArea inland = theArea;
	  inland.type(WeatherArea::Inland);
	  
	  if(forecast_period == DAY1_PERIOD)
		theLog << "Day1 ";
	  else if(forecast_period == NIGHT_PERIOD)
		theLog << "Night ";
	  else if(forecast_period == DAY2_PERIOD)
		theLog << "Day2 ";

	  theLog << "results for area:" << endl;


	  unsigned short weather_id_offset = (forecast_period == DAY1_PERIOD ? 0 : (forecast_period == NIGHT_PERIOD ? 3 : 6));
	  unsigned short thePart = (forecast_period == DAY1_PERIOD ? 1 : (forecast_period == NIGHT_PERIOD ? 2 : 3));

	  calculate_mean_min_max(theLog,
							 theForecaster,
							 theVar+"::fake::day"+Convert(thePart)+"::area",
							 theSources,
							 full,
							 thePeriod,
							 forecast_period == NIGHT_PERIOD ? Minimum : Maximum,
							 weatherResults[AREA_MEAN_MAX_DAY1+weather_id_offset]->weatherResult(),
							 weatherResults[AREA_MIN_MAX_DAY1+weather_id_offset]->weatherResult(),
							 weatherResults[AREA_MAX_MAX_DAY1+weather_id_offset]->weatherResult(),
							 weatherResults[AREA_MIN_MAX_DAY1+weather_id_offset]->fractile(),
							 weatherResults[AREA_MAX_MAX_DAY1+weather_id_offset]->fractile());
	  
	  if(thePart == 1)
		theLog << "Day1 results for inland:" << endl;
	  else if(thePart == 2)
		theLog << "Night results for inland:" << endl;
	  else if(thePart == 3)
		theLog << "Day2 results for inland:" << endl;

	  calculate_mean_min_max(theLog,
							 theForecaster,
							 theVar+"::fake::day"+Convert(thePart)+"::inland",
							 theSources,
							 inland,
							 thePeriod,
							 forecast_period == NIGHT_PERIOD ? Minimum : Maximum,
							 weatherResults[INLAND_MEAN_MAX_DAY1+weather_id_offset]->weatherResult(),
							 weatherResults[INLAND_MIN_MAX_DAY1+weather_id_offset]->weatherResult(),
							 weatherResults[INLAND_MAX_MAX_DAY1+weather_id_offset]->weatherResult(),
							 weatherResults[INLAND_MIN_MAX_DAY1+weather_id_offset]->fractile(),
							 weatherResults[INLAND_MAX_MAX_DAY1+weather_id_offset]->fractile());
	  
	  if(thePart == 1)
		theLog << "Day1 results for coast:" << endl;
	  else if(thePart == 2)
		theLog << "Night results for coast:" << endl;
	  else if(thePart == 3)
		theLog << "Day2 results for coast:" << endl;

	  calculate_mean_min_max(theLog,
							 theForecaster,
							 theVar+"::fake::day"+Convert(thePart)+"::coast",
							 theSources,
							 coast,
							 thePeriod,
							 forecast_period == NIGHT_PERIOD ? Minimum : Maximum,
							 weatherResults[COAST_MEAN_MAX_DAY1+weather_id_offset]->weatherResult(),
							 weatherResults[COAST_MIN_MAX_DAY1+weather_id_offset]->weatherResult(),
							 weatherResults[COAST_MAX_MAX_DAY1+weather_id_offset]->weatherResult(),
							 weatherResults[COAST_MIN_MAX_DAY1+weather_id_offset]->fractile(),
							 weatherResults[COAST_MAX_MAX_DAY1+weather_id_offset]->fractile());
	  
	}
	

	const char* fractile_name(const fractile_id& id)
	{
	  const char* retval = "No value";

	  switch(id)
		{
		case FRACTILE_02:
		  retval = "F0,0-F2,5";
		  break;
		case FRACTILE_12:
		  retval = "F2,5-F12,5";
		  break;
		case FRACTILE_37:
		  retval = "F12,5-37,5";
		  break;
		case FRACTILE_50:
		  retval = "F37,5-F50,0";
		  break;
		case FRACTILE_63:
		  retval = "F50,0-F62,5";
		  break;
		case FRACTILE_88:
		  retval = "F62,5-F87,5";
		  break;
		case FRACTILE_98:
		  retval = "F87,5-F97,5";
		  break;
		case FRACTILE_100:
		  retval = "F97,5-F100,0";
		  break;
		case FRACTILE_UNDEFINED:
		  retval = "Undefined";
		  break;
		}

	  return retval;
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
	  for(int i = AREA_MEAN_MAX_DAY1; i <= COAST_MAX_MAX_DAY2; i++)
		{
		  theLog << weatherResults[i]->weatherResult() << endl;
		  theLog << "Fractile: " << fractile_name(weatherResults[i]->fractile()) << endl;
		} 
	}


	const forecast_season_id get_forecast_season(const NFmiTime& theTime)
	{
	  return (SeasonTools::isSummerHalf(theTime) ? SUMMER_SEASON : WINTER_SEASON);
	}


	const Sentence exceptional_temperature_sentence(const std::string theVariable,
													const forecast_season_id& season,
													const fractile_id& theFractile)
	{
	  Sentence retval;

	  switch(theFractile)
		{
		case FRACTILE_02:
		  retval << (season == SUMMER_SEASON ? "poikkeuksellisen koleaa" : "poikkeuksellisen kylmää");
		  break;
		case FRACTILE_12:
		  retval << (season == SUMMER_SEASON ? "koleaa" : "hyvin kylmää");
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
		  retval << (season == SUMMER_SEASON ? "harvinaisen lämmintä" : "hyvin lauhaa");
		  break;
		case FRACTILE_100:
		  retval << (season == SUMMER_SEASON ? "poikkeuksellisen lämmintä" : "poikkeuksellisen leutoa");
		  break;
		case FRACTILE_UNDEFINED:
		  break;
		}

	  return retval;
	}


	const Sentence value_dependent_sentence(const std::string theVariable,
											const double& min_temperature,
											const double& mean_temperature,
											const double& max_temperature)
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

	  return sentence;
	}

	const Sentence temperature_max36hours_sentence(MessageLogger& log,
												   const std::string & theVariable,
												   const NightAndDayPeriodGenerator& theGenerator,
												   const unsigned short& forecast_areas,
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
		  night_season = get_forecast_season(theGenerator.period(2).localStartTime());
		}
	  if(forecast_periods & DAY2_PERIOD)
		{
		  day2_season = get_forecast_season(theGenerator.period(3).localStartTime());
		}

	  if((forecast_areas & COASTAL_AREA) && (forecast_areas & INLAND_AREA))
		{
		  // both inland and coastal areas are included

		  // if the difference between coast and inland is less than 
		  // temperature_limit_coast_inland, handle them together

		  if(forecast_periods & DAY1_PERIOD)
			{
			  sentence << season_dependent_sentence(DAY1_PERIOD, day1_season);

			  float temp_diff_day1 = abs(round(weatherResults[INLAND_MEAN_MAX_DAY1]->value() - weatherResults[COAST_MEAN_MAX_DAY1]->value()));			  
			  if(temp_diff_day1 < temperature_limit_coast_inland)
				{
				  //				  sentence << day1_sentence_area(weatherResults);

				  sentence << value_dependent_sentence(theVariable,
													   weatherResults[AREA_MIN_MAX_DAY1]->value(),
													   weatherResults[AREA_MEAN_MAX_DAY1]->value(),
													   weatherResults[AREA_MAX_MAX_DAY1]->value());

				  sentence << exceptional_temperature_sentence(theVariable,
															   day1_season,
															   weatherResults[AREA_MAX_MAX_DAY1]->fractile());

				}
			  else
				{
				  sentence << value_dependent_sentence(theVariable,
													   weatherResults[INLAND_MIN_MAX_DAY1]->value(),
													   weatherResults[INLAND_MEAN_MAX_DAY1]->value(),
													   weatherResults[INLAND_MAX_MAX_DAY1]->value());

				  sentence << exceptional_temperature_sentence(theVariable,
															   day1_season,
															   weatherResults[INLAND_MAX_MAX_DAY1]->fractile());

				  sentence << value_dependent_sentence(theVariable,
													   weatherResults[COAST_MIN_MAX_DAY1]->value(),
													   weatherResults[COAST_MEAN_MAX_DAY1]->value(),
													   weatherResults[COAST_MAX_MAX_DAY1]->value());

				  sentence << exceptional_temperature_sentence(theVariable,
															   day1_season,
															   weatherResults[COAST_MAX_MAX_DAY1]->fractile());

				  // sentence << day1_sentence_inland_and_coast(weatherResults);
				}
			}

		  if(forecast_periods & NIGHT_PERIOD)
			{
			  sentence << season_dependent_sentence(NIGHT_PERIOD, night_season);

			  float temp_diff_night = abs(weatherResults[INLAND_MEAN_MIN_NIGHT]->value() - weatherResults[COAST_MEAN_MIN_NIGHT]->value());
			  
			  if(temp_diff_night < temperature_limit_coast_inland)
				{
				  sentence << value_dependent_sentence(theVariable,
													   weatherResults[AREA_MIN_MIN_NIGHT]->value(),
													   weatherResults[AREA_MEAN_MIN_NIGHT]->value(),
													   weatherResults[AREA_MAX_MIN_NIGHT]->value());
				  //				  sentence << night_sentence_area(weatherResults);
				}
			  else
				{
				  sentence << value_dependent_sentence(theVariable,
													   weatherResults[INLAND_MIN_MIN_NIGHT]->value(),
													   weatherResults[INLAND_MEAN_MIN_NIGHT]->value(),
													   weatherResults[INLAND_MAX_MIN_NIGHT]->value());

				  sentence << value_dependent_sentence(theVariable,
													   weatherResults[COAST_MIN_MIN_NIGHT]->value(),
													   weatherResults[COAST_MEAN_MIN_NIGHT]->value(),
													   weatherResults[COAST_MAX_MIN_NIGHT]->value());


				  //				  sentence << day1_sentence_inland_and_coast(weatherResults);
				}
			}

		  if(forecast_periods & DAY2_PERIOD)
			{
			  sentence << season_dependent_sentence(DAY2_PERIOD, day2_season);

			  float temp_diff_day2 = abs(weatherResults[INLAND_MEAN_MAX_DAY2]->value() - weatherResults[COAST_MEAN_MAX_DAY2]->value());
			  
			  if(temp_diff_day2 < temperature_limit_coast_inland)
				{
				  sentence << value_dependent_sentence(theVariable,
													   weatherResults[AREA_MIN_MAX_DAY2]->value(),
													   weatherResults[AREA_MEAN_MAX_DAY2]->value(),
													   weatherResults[AREA_MAX_MAX_DAY2]->value());

				  //				  sentence << day2_sentence_area(weatherResults);
				}
			  else
				{
				  sentence << value_dependent_sentence(theVariable,
													   weatherResults[INLAND_MIN_MAX_DAY2]->value(),
													   weatherResults[INLAND_MEAN_MAX_DAY2]->value(),
													   weatherResults[INLAND_MAX_MAX_DAY2]->value());

				  sentence << value_dependent_sentence(theVariable,
													   weatherResults[COAST_MIN_MAX_DAY2]->value(),
													   weatherResults[COAST_MEAN_MAX_DAY2]->value(),
													   weatherResults[COAST_MAX_MAX_DAY2]->value());

				  //				  sentence << day2_sentence_inland_and_coast(weatherResults);
				}
			}
		}
	  else if(forecast_areas & INLAND_AREA)
		{
		  // only inland area is included


		  if(forecast_periods & DAY1_PERIOD)
			{
			  sentence << season_dependent_sentence(DAY1_PERIOD, day1_season);

			  sentence << value_dependent_sentence(theVariable,
												   weatherResults[INLAND_MIN_MAX_DAY1]->value(),
												   weatherResults[INLAND_MEAN_MAX_DAY1]->value(),
												   weatherResults[INLAND_MAX_MAX_DAY1]->value());

			  sentence << exceptional_temperature_sentence(theVariable,
														   day1_season,
														   weatherResults[INLAND_MAX_MAX_DAY1]->fractile());

			}
		  
		  if(forecast_periods & NIGHT_PERIOD)
			{
			  if(forecast_periods & DAY1_PERIOD)
				sentence << Delimiter(",");

			  sentence << season_dependent_sentence(NIGHT_PERIOD, night_season);
			  
			  sentence << value_dependent_sentence(theVariable,
												   weatherResults[INLAND_MIN_MIN_NIGHT]->value(),
												   weatherResults[INLAND_MEAN_MIN_NIGHT]->value(),
												   weatherResults[INLAND_MAX_MIN_NIGHT]->value());
			}

		  if(forecast_periods & DAY2_PERIOD)
			{
			  if(forecast_periods & (DAY1_PERIOD | NIGHT_PERIOD))
				sentence << Delimiter(",");

			  //			  sentence << season_dependent_sentence(DAY1_PERIOD, day2_season);

			  sentence << "huomenna";

			  
			  sentence << value_dependent_sentence(theVariable,
												   weatherResults[INLAND_MIN_MAX_DAY2]->value(),
												   weatherResults[INLAND_MEAN_MAX_DAY2]->value(),
												   weatherResults[INLAND_MAX_MAX_DAY2]->value());
			}
		}
	  else
		{
		  // only coastal area is included

		  if(forecast_periods & DAY1_PERIOD)
			{
			  sentence << season_dependent_sentence(DAY1_PERIOD, day1_season);

			  sentence << value_dependent_sentence(theVariable,
												   weatherResults[COAST_MIN_MAX_DAY1]->value(),
												   weatherResults[COAST_MEAN_MAX_DAY1]->value(),
												   weatherResults[COAST_MAX_MAX_DAY1]->value());

			  sentence << exceptional_temperature_sentence(theVariable,
														   day1_season,
														   weatherResults[COAST_MAX_MAX_DAY1]->fractile());
			}
		  
		  if(forecast_periods & NIGHT_PERIOD)
			{
			  if(forecast_periods & DAY1_PERIOD)
				sentence << Delimiter(",");

			  sentence << season_dependent_sentence(NIGHT_PERIOD, night_season);
			  
			  sentence << value_dependent_sentence(theVariable,
												   weatherResults[COAST_MIN_MIN_NIGHT]->value(),
												   weatherResults[COAST_MEAN_MIN_NIGHT]->value(),
												   weatherResults[COAST_MAX_MIN_NIGHT]->value());
			}
		  
		  if(forecast_periods & DAY2_PERIOD)
			{
			  if(forecast_periods & (DAY1_PERIOD | NIGHT_PERIOD))
				sentence << Delimiter(",");

			  sentence << "huomenna";
			  
			  sentence << value_dependent_sentence(theVariable,
												   weatherResults[COAST_MIN_MAX_DAY2]->value(),
												   weatherResults[COAST_MEAN_MAX_DAY2]->value(),
												   weatherResults[COAST_MAX_MAX_DAY2]->value());
			}
		}

	  return sentence;
	}


  } // namespace TemperatureMax36Hours

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

	MessageLogger log("TemperatureStory::max36h");
	unsigned short forecast_areas = 0x0;
	unsigned short forecast_periods = 0x0;
		      
	NFmiSettings::Set("textgen::part1::story::temperature_max36hours::day::starthour","21");
	NFmiSettings::Set("textgen::part1::story::temperature_max36hours::day::endhour","6");
	//Settings::require_hour(itsVar+"::night::starthour");

	Paragraph paragraph;

	// Period generator

	NightAndDayPeriodGenerator generator(itsPeriod, itsVar);

	// Too late? Return empty story then

	if(generator.size() == 0)
	  {
		log << paragraph;
		return paragraph;
	  }


	log << "Period contains " << generator.size() << " days or nights" << endl;


	// Calculate the results

	weather_result_container_type weatherResults;
	//weather_result_container_type2 weatherResults2;

	GridForecaster forecaster;

	if(generator.isday(1))
	  {
		// when the first period is day, and the second period exists 
		// it must be night, and if third period exists it must be day
		forecast_periods |= DAY1_PERIOD;
		forecast_periods |= (generator.size() > 1 ? NIGHT_PERIOD : 0x0);
		forecast_periods |= (generator.size() > 2 ? DAY2_PERIOD : 0x0);
	  }
	else
	  {
		// if the first period is not day, it must be night, and
		// if second period exists it must be day
		forecast_periods |= NIGHT_PERIOD;
		forecast_periods |= (generator.size() > 1 ? DAY2_PERIOD : 0x0);
	  }


	// Initialize the container for WeatherResult variables
	for(int i = AREA_MEAN_MAX_DAY1; i <= COAST_MAX_MAX_DAY2; i++)
	  {
		weatherResults.insert(make_pair(i, new Max36HoursResults()));
		//		weatherResults2.insert(make_pair(i, new Max36HoursResults()));
	  } 





	WeatherPeriod period = generator.period(1);
	if(forecast_periods & DAY1_PERIOD)
	  {
		// calculate results for day1
		log_start_time_and_end_time(log, "Day1: ", period);
		calculate_weather_results(log, forecaster, itsVar, itsSources,
								itsArea, period, DAY1_PERIOD, weatherResults);

		// day1 period exists, so
		// if the area is included, it must have valid values
		forecast_areas |= (weatherResults[COAST_MEAN_MAX_DAY1]->value() != kFloatMissing ? COASTAL_AREA : 0x0); 
		forecast_areas |= (weatherResults[INLAND_MEAN_MAX_DAY1]->value() != kFloatMissing ? INLAND_AREA : 0x0); 

		if(forecast_periods & NIGHT_PERIOD)
		  {
			// calculate results for night
			period = generator.period(2);
			log_start_time_and_end_time(log, "Night: ", period);
			calculate_weather_results(log, forecaster, itsVar, itsSources,
									itsArea, period, NIGHT_PERIOD, weatherResults);
		  }

		if(forecast_periods & DAY2_PERIOD)
		  {
			// calculate results for day2
			period = generator.period(3);
			log_start_time_and_end_time(log, "Day2: ", period);
			calculate_weather_results(log, forecaster, itsVar, itsSources,
									itsArea, period, DAY2_PERIOD, weatherResults);
		  }
	  }
	else
	  {
		// calculate results for night
		log_start_time_and_end_time(log, "Night: ", period);
		calculate_weather_results(log, forecaster, itsVar, itsSources,
									itsArea, period, NIGHT_PERIOD, weatherResults);

		// night period exists, so
		// if the area is included, it must have valid values
		forecast_areas |= (weatherResults[COAST_MEAN_MIN_NIGHT]->value() != kFloatMissing ? COASTAL_AREA : 0x0); 
		forecast_areas |= (weatherResults[INLAND_MEAN_MIN_NIGHT]->value() != kFloatMissing ? INLAND_AREA : 0x0); 


		if(forecast_periods & DAY2_PERIOD)
		  {
			// calculate results for day2
			period = generator.period(2);
			log_start_time_and_end_time(log, "Day2: ", period);
			calculate_weather_results(log, forecaster, itsVar, itsSources,
									itsArea, period, DAY2_PERIOD, weatherResults);
		  }

	  }

	paragraph << temperature_max36hours_sentence(log, itsVar, generator, forecast_areas, forecast_periods, weatherResults);

	 log_weather_results(log, weatherResults);


	// delete the allocated WeatherResult-objects
	for(int i = AREA_MEAN_MAX_DAY1; i <= COAST_MAX_MAX_DAY2; i++)
	  {
		delete weatherResults[i];
	  } 


	log << paragraph;
	return paragraph;






























#ifdef REFERENCE_CODE

	// The options

	const int day_night_limit = optional_int(itsVar+"::day_night_limit",3);
	const int coast_numeric_limit = optional_int(itsVar+"::coast_numeric_limit",8);
	const int coast_not_below = optional_int(itsVar+"::coast_not_below",-15);
	const int coast_limit = optional_int(itsVar+"::coast_limit",2);

    const int mininterval = optional_int(itsVar+"::mininterval",2);
    const bool interval_zero = optional_bool(itsVar+"::always_interval_zero",false);

	const string rangeseparator = optional_string(itsVar+"::rangeseparator","...");

	WeatherArea coast = itsArea;
	coast.type(WeatherArea::Coast);

	WeatherArea inland = itsArea;
	inland.type(WeatherArea::Inland);

	// Calculate the results

	GridForecaster forecaster;

	unsigned int periodnum = 1;
	unsigned int part = 0;
	while(periodnum <= generator.size())
	  {
		++part;

		log << "Sentence " << part << ":" << endl;

		WeatherResult areameanday(kFloatMissing,0);
		WeatherResult areaminday(kFloatMissing,0);
		WeatherResult areamaxday(kFloatMissing,0);

		WeatherResult coastmeanday(kFloatMissing,0);
		WeatherResult coastminday(kFloatMissing,0);
		WeatherResult coastmaxday(kFloatMissing,0);
		
		WeatherResult inlandmeanday(kFloatMissing,0);
		WeatherResult inlandminday(kFloatMissing,0);
		WeatherResult inlandmaxday(kFloatMissing,0);
		
		WeatherResult areameannight(kFloatMissing,0);
		WeatherResult areaminnight(kFloatMissing,0);
		WeatherResult areamaxnight(kFloatMissing,0);
		
		WeatherResult coastmeannight(kFloatMissing,0);
		WeatherResult coastminnight(kFloatMissing,0);
		WeatherResult coastmaxnight(kFloatMissing,0);
		
		WeatherResult inlandmeannight(kFloatMissing,0);
		WeatherResult inlandminnight(kFloatMissing,0);
		WeatherResult inlandmaxnight(kFloatMissing,0);
		
		// Daytime results

		bool hasday = false;
		const int daynum = periodnum;
		if(generator.isday(periodnum))
		  {
			hasday = true;

			WeatherPeriod period = generator.period(periodnum++);
			log << "Day: "
				<< period.localStartTime()
				<< " ... "
				<< period.localEndTime()
				<< endl;

			log << "Day results for area:" << endl;
			calculate_mean_min_max(log,forecaster,
								   itsVar+"::fake::day"+Convert(part)+"::area",
								   itsSources,itsArea,period,Maximum,
								   areameanday,areaminday,areamaxday);

			log << "Day results for coast:" << endl;
			calculate_mean_min_max(log,forecaster,
								   itsVar+"::fake::day"+Convert(part)+"::coast",
								   itsSources,coast,period,Maximum,
								   coastmeanday,coastminday,coastmaxday);

			log << "Day results for inland:" << endl;
			calculate_mean_min_max(log,forecaster,
								   itsVar+"::fake::day"+Convert(part)+"::inland",
								   itsSources,inland,period,Maximum,
								   inlandmeanday,inlandminday,inlandmaxday);

		  }
		
		// Nighttime results
		
		bool hasnight = false;
		const int nightnum = periodnum;
		if(periodnum <= generator.size() && generator.isnight(periodnum))
		  {
			hasnight = true;
			WeatherPeriod period = generator.period(periodnum++);

			log << "Night: "
				<< period.localStartTime()
				<< " ... "
				<< period.localEndTime()
				<< endl;

			log << "Night results for area:" << endl;
			calculate_mean_min_max(log,forecaster,
								   itsVar+"::fake::night"+Convert(part)+"::area",
								   itsSources,itsArea,period,Minimum,
								   areameannight,areaminnight,areamaxnight);
			
			log << "Night results for coast:" << endl;
			calculate_mean_min_max(log,forecaster,
								   itsVar+"::fake::night"+Convert(part)+"::coast",
								   itsSources,coast,period,Minimum,
								   coastmeannight,coastminnight,coastmaxnight);
			
			log << "Night results for inland:" << endl;
			calculate_mean_min_max(log,forecaster,
								   itsVar+"::fake::night"+Convert(part)+"::inland",itsSources,inland,period,Minimum,
								   inlandmeannight,inlandminnight,inlandmaxnight);
		  }

		// Rounded values

		const int aminday = static_cast<int>(round(areaminday.value()));
		const int ameanday = static_cast<int>(round(areameanday.value()));
		const int amaxday = static_cast<int>(round(areamaxday.value()));
		
		const int cminday = static_cast<int>(round(coastminday.value()));
		const int cmeanday = static_cast<int>(round(coastmeanday.value()));
		const int cmaxday = static_cast<int>(round(coastmaxday.value()));
		
		const int iminday = static_cast<int>(round(inlandminday.value()));
		const int imeanday = static_cast<int>(round(inlandmeanday.value()));
		const int imaxday = static_cast<int>(round(inlandmaxday.value()));
		
		const int aminnight = static_cast<int>(round(areaminnight.value()));
		const int ameannight = static_cast<int>(round(areameannight.value()));
		const int amaxnight = static_cast<int>(round(areamaxnight.value()));
		
		const int cminnight = static_cast<int>(round(coastminnight.value()));
		const int cmeannight = static_cast<int>(round(coastmeannight.value()));
		const int cmaxnight = static_cast<int>(round(coastmaxnight.value()));
		
		const int iminnight = static_cast<int>(round(inlandminnight.value()));
		const int imeannight = static_cast<int>(round(inlandmeannight.value()));
		const int imaxnight = static_cast<int>(round(inlandmaxnight.value()));
		
		const int bad = static_cast<int>(kFloatMissing);
		
		const bool hascoast = ( (cmeanday!=bad && imeanday!=bad) || (cmeannight!=bad && imeannight!=bad) );
		
		// See if we can report on day and night simultaneously
		// (when the difference is very small)

		const bool canmerge = (hasday && hasnight && abs(ameanday-ameannight) < day_night_limit);

		if(canmerge)
		  {
			Sentence sentence;
			sentence << "lämpötila"
					 << "on"
					 << PeriodPhraseFactory::create("today",
													itsVar,
													itsForecastTime,
													generator.period(daynum));
			if(!hascoast ||
			   ameanday < coast_not_below ||
			   (abs(cmeanday - imeanday) < coast_limit && abs(cmeannight - imeannight) < coast_limit))
			  {
				sentence << temperature_sentence(min(aminday,aminnight),
												 static_cast<int>(round((ameanday+ameannight)/2)),
												 max(amaxday,amaxnight),
												 mininterval,
												 interval_zero,
												 rangeseparator);
			  }
			else
			  {
				const int imean = static_cast<int>(round((inlandmeanday.value()+inlandmeannight.value())/2));
				const int cmean = static_cast<int>(round((coastmeanday.value()+coastmeannight.value())/kFloatMissing,0)));2));
				sentence << temperature_sentence(min(iminday,iminnight),
												 imean,
												 max(imaxday,imaxnight),
												 mininterval,
												 interval_zero,
												 rangeseparator)
						 << Delimiter(",")
						 << "rannikolla"
						 << temperature_comparison_phrase(imean,cmean,itsVar);
			  }
			paragraph << sentence;
		  }
		
		// Report daytime
		
		if(hasday && !canmerge)
		  {
			Sentence sentence;
			
			sentence << "päivän ylin lämpötila"
					 << "on"
					 << PeriodPhraseFactory::create("today",
													itsVar,
													itsForecastTime,
													generator.period(daynum));
		
			// Do not report on the coast separately if there's no coast,
			// the mean temperature is very low (atleast freezing), or if the
			// difference between inland and the coast is small

			if(!hascoast ||
			   ameanday < coast_not_below ||
			   abs(cmeanday - imeanday) < coast_limit)
			  {
				sentence << temperature_sentence(aminday,ameanday,amaxday,mininterval,interval_zero,rangeseparator);
			  }
			else
			  {
				sentence << temperature_sentence(iminday,imeanday,imaxday,mininterval,interval_zero,rangeseparator)
						 << Delimiter(",")
						 << "rannikolla";
				
				if(abs(imeanday-cmeanday) >= coast_numeric_limit)
				  sentence << temperature_sentence(cminday,cmeanday,cmaxday,mininterval,interval_zero,rangeseparator);
				else
				  sentence << temperature_comparison_phrase(imeanday,cmeanday,itsVar);
			  }
			paragraph << sentence;
		  }

		// Report nighttime
		
		if(hasnight && !canmerge)
		  {
			Sentence sentence;
			
			sentence << "yön alin lämpötila"
					 << "on"
					 << PeriodPhraseFactory::create("tonight",
													itsVar,
													itsForecastTime,
													generator.period(nightnum));
		
			// Do not report on the coast separately if there's no coast,
			// the mean temperature is very low (atleast freezing), or if the
			// difference between inland and the coast is small
			
			if(!hascoast ||
			   ameannight < coast_not_below ||
			   abs(cmeannight - imeannight) < coast_limit)
			  {
				sentence << temperature_sentence(aminnight,ameannight,amaxnight,mininterval,interval_zero,rangeseparator);
			  }
			else
			  {
				sentence << temperature_sentence(iminnight,imeannight,imaxnight,mininterval,interval_zero,rangeseparator)
						 << Delimiter(",")
						 << "rannikolla";
				
				if(abs(imeannight-cmeannight) >= coast_numeric_limit)
				  sentence << temperature_sentence(cminnight,cmeannight,cmaxnight,mininterval,interval_zero,rangeseparator);
				else
				  sentence << temperature_comparison_phrase(imeannight,cmeannight,itsVar);
			  }
			paragraph << sentence;
		  }
	  }

#endif

  }

} // namespace TextGen
  
// ======================================================================
  
