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

	enum forecast_subperiod_id{DAY1_MORNING_PERIOD = 0x1,
							   DAY1_AFTERNOON_PERIOD = 0x2,
							   DAY2_MORNING_PERIOD = 0x4,
							   DAY2_AFTERNOON_PERIOD = 0x8,
							   UNDEFINED_SUBPERIOD = 0x0};

	enum forecast_season_id{SUMMER_SEASON, 
							WINTER_SEASON,
							UNDEFINED_SEASON};

	enum processing_order{DAY1_DAY2_NIGHT,
						  DAY1_NIGHT,
						  NIGHT_DAY2};

	enum sentence_part_id{DAY1_INLAND,
						  DAY1_COASTAL,
						  DAY1_FULL,
						  NIGHT_INLAND,
						  NIGHT_COASTAL,
						  NIGHT_FULL,
						  DAY2_INLAND,
						  DAY2_COASTAL,
						  DAY2_FULL,
						  DELIMITER_COMMA,
						  DELIMITER_DOT};

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


	struct t36hparams
	{
	  t36hparams(const string& variable,
				 MessageLogger& log,
				 const NightAndDayPeriodGenerator& generator,
				 forecast_season_id& seasonId,
				 forecast_area_id& areaId,
				 forecast_period_id& periodId,
				 forecast_subperiod_id& subPeriodId,
				 const unsigned short& forecastArea,
				 unsigned short& forecastAreaDay1,
				 unsigned short& forecastAreaNight,
				 unsigned short& forecastAreaDay2,
				 const unsigned short& forecastPeriod,
				 unsigned short& forecastSubPeriod,
				 weather_result_container_type& weatherResults,
				 double& maxTemperatureDay1,
				 double& meanTemperatureDay1,
				 double& minimum,
				 double& maximum,
				 double& mean):
		theVariable(variable),
		theLog(log),
		theGenerator(generator),
		theSeasonId(seasonId),
		theAreaId(areaId),
		thePeriodId(periodId),
		theSubPeriodId(subPeriodId),
		theForecastArea(forecastArea),
		theForecastAreaDay1(forecastAreaDay1),
		theForecastAreaNight(forecastAreaNight),
		theForecastAreaDay2(forecastAreaDay2),
		theForecastPeriod(forecastPeriod),
		theForecastSubPeriod(forecastSubPeriod),
		theWeatherResults(weatherResults),
		theMaxTemperatureDay1(maxTemperatureDay1),
		theMeanTemperatureDay1(meanTemperatureDay1),
		theMinimum(minimum),
		theMaximum(maximum),
		theMean(mean),
		theNightPeriodTautologyFlag(false),
		theDayPeriodTautologyFlag(false),
		theTomorrowTautologyFlag(false),
		theOnCoastalAreaTautologyFlag(false)
	  {}

	  const string& theVariable;
	  MessageLogger& theLog;
	  const NightAndDayPeriodGenerator& theGenerator;
	  forecast_season_id& theSeasonId;
	  forecast_area_id& theAreaId;
	  forecast_period_id& thePeriodId;
	  forecast_subperiod_id& theSubPeriodId;
	  const unsigned short& theForecastArea;
	  unsigned short& theForecastAreaDay1;
	  unsigned short& theForecastAreaNight;
	  unsigned short& theForecastAreaDay2;
	  const unsigned short& theForecastPeriod;
	  unsigned short& theForecastSubPeriod;
	  weather_result_container_type& theWeatherResults;
	  double& theMaxTemperatureDay1;
	  double& theMeanTemperatureDay1;
	  double& theMinimum;
	  double& theMaximum;
	  double& theMean;
	  bool theNightPeriodTautologyFlag;
	  bool theDayPeriodTautologyFlag;
	  bool theTomorrowTautologyFlag;
	  bool theOnCoastalAreaTautologyFlag;
	};
	
	std::string weather_result_string(const std::string& areaName, const weather_result_id& id)
	{
	  std::string retval;

	  switch(id)
		{
		case AREA_MIN_DAY1:
		  retval = areaName + " - area day1 minimum: ";
		  break;
		case AREA_MAX_DAY1:
		  retval = areaName + " - area day1 maximum: ";
		  break;
		case AREA_MEAN_DAY1:
		  retval = areaName + " - area day1 mean: ";
		  break;
		case AREA_MIN_NIGHT:
		  retval = areaName + " - area night minimum: ";
		  break;
		case AREA_MAX_NIGHT:
		  retval = areaName + " - area night maximum: ";
		  break;
		case AREA_MEAN_NIGHT:
		  retval = areaName + " - area night mean: ";
		  break;
		case AREA_MIN_DAY2:
		  retval = areaName + " - area day2 minimum: ";
		  break;
		case AREA_MAX_DAY2:
		  retval = areaName + " - area day2 maximum: ";
		  break;
		case AREA_MEAN_DAY2:
		  retval = areaName + " - area day2 mean: ";
		  break;
		case AREA_MIN_DAY1_MORNING:
		  retval = areaName + " - area day1 morning minimum: ";
		  break;
		case AREA_MAX_DAY1_MORNING:
		  retval = areaName + " - area day1 morning maximum: ";
		  break;
		case AREA_MEAN_DAY1_MORNING:
		  retval = areaName + " - area day1  morning mean: ";
		  break;
		case AREA_MIN_DAY2_MORNING:
		  retval = areaName + " - area day2  morning minimum: ";
		  break;
		case AREA_MAX_DAY2_MORNING:
		  retval = areaName + " - area day2 morning maximum: ";
		  break;
		case AREA_MEAN_DAY2_MORNING:
		  retval = areaName + " - area day2 morning mean: ";
		  break;
		case AREA_MIN_DAY1_AFTERNOON:
		  retval = areaName + " - area day1 afternoon minimum: ";
		  break;
		case AREA_MAX_DAY1_AFTERNOON:
		  retval = areaName + " - area day1 afternoon maximum: ";
		  break;
		case AREA_MEAN_DAY1_AFTERNOON:
		  retval = areaName + " - area day1 afternoon mean: ";
		  break;
		case AREA_MIN_DAY2_AFTERNOON:
		  retval = areaName + " - area day2 afternoon minimum: ";
		  break;
		case AREA_MAX_DAY2_AFTERNOON:
		  retval = areaName + " - area day2 afternoon maximum: ";
		  break;
		case AREA_MEAN_DAY2_AFTERNOON:
		  retval = areaName + " - area day2 afternoon mean: ";
		  break;
		case INLAND_MIN_DAY1:
		  retval = areaName + " - inland day1 minimum: ";
		  break;
		case INLAND_MAX_DAY1:
		  retval = areaName + " - inland day1 maximum: ";
		  break;
		case INLAND_MEAN_DAY1:
		  retval = areaName + " - inland day1 mean: ";
		  break;
		case INLAND_MIN_NIGHT:
		  retval = areaName + " - inland night minimum: ";
		  break;
		case INLAND_MAX_NIGHT:
		  retval = areaName + " - inland night maximum: ";
		  break;
		case INLAND_MEAN_NIGHT:
		  retval = areaName + " - inland night mean: ";
		  break;
		case INLAND_MIN_DAY2:
		  retval = areaName + " - inland day2 minimum: ";
		  break;
		case INLAND_MAX_DAY2:
		  retval = areaName + " - inland day2 maximum: ";
		  break;
		case INLAND_MEAN_DAY2:
		  retval = areaName + " - inland day2 mean: ";
		  break;
		case INLAND_MIN_DAY1_MORNING:
		  retval = areaName + " - inland day1 morning minimum: ";
		  break;
		case INLAND_MAX_DAY1_MORNING:
		  retval = areaName + " - inland day1 morning maximum: ";
		  break;
		case INLAND_MEAN_DAY1_MORNING:
		  retval = areaName + " - inland day1  morning mean: ";
		  break;
		case INLAND_MIN_DAY2_MORNING:
		  retval = areaName + " - inland day2  morning minimum: ";
		  break;
		case INLAND_MAX_DAY2_MORNING:
		  retval = areaName + " - inland day2 morning maximum: ";
		  break;
		case INLAND_MEAN_DAY2_MORNING:
		  retval = areaName + " - inland day2 morning mean: ";
		  break;
		case INLAND_MIN_DAY1_AFTERNOON:
		  retval = areaName + " - inland day1 afternoon minimum: ";
		  break;
		case INLAND_MAX_DAY1_AFTERNOON:
		  retval = areaName + " - inland day1 afternoon maximum: ";
		  break;
		case INLAND_MEAN_DAY1_AFTERNOON:
		  retval = areaName + " - inland day1 afternoon mean: ";
		  break;
		case INLAND_MIN_DAY2_AFTERNOON:
		  retval = areaName + " - inland day2 afternoon minimum: ";
		  break;
		case INLAND_MAX_DAY2_AFTERNOON:
		  retval = areaName + " - inland day2 afternoon maximum: ";
		  break;
		case INLAND_MEAN_DAY2_AFTERNOON:
		  retval = areaName + " - inland day2 afternoon mean: ";
		  break;
		case COAST_MIN_DAY1:
		  retval = areaName + " - coast day1 minimum: ";
		  break;
		case COAST_MAX_DAY1:
		  retval = areaName + " - coast day1 maximum: ";
		  break;
		case COAST_MEAN_DAY1:
		  retval = areaName + " - coast day1 mean: ";
		  break;
		case COAST_MIN_NIGHT:
		  retval = areaName + " - coast night minimum: ";
		  break;
		case COAST_MAX_NIGHT:
		  retval = areaName + " - coast night maximum: ";
		  break;
		case COAST_MEAN_NIGHT:
		  retval = areaName + " - coast night mean: ";
		  break;
		case COAST_MIN_DAY2:
		  retval = areaName + " - coast day2 minimum: ";
		  break;
		case COAST_MAX_DAY2:
		  retval = areaName + " - coast day2 maximum: ";
		  break;
		case COAST_MEAN_DAY2:
		  retval = areaName + " - coast day2 mean: ";
		  break;
		case COAST_MIN_DAY1_MORNING:
		  retval = areaName + " - coast day1 morning minimum: ";
		  break;
		case COAST_MAX_DAY1_MORNING:
		  retval = areaName + " - coast day1 morning maximum: ";
		  break;
		case COAST_MEAN_DAY1_MORNING:
		  retval = areaName + " - coast day1  morning mean: ";
		  break;
		case COAST_MIN_DAY2_MORNING:
		  retval = areaName + " - coast day2  morning minimum: ";
		  break;
		case COAST_MAX_DAY2_MORNING:
		  retval = areaName + " - coast day2 morning maximum: ";
		  break;
		case COAST_MEAN_DAY2_MORNING:
		  retval = areaName + " - coast day2 morning mean: ";
		  break;
		case COAST_MIN_DAY1_AFTERNOON:
		  retval = areaName + " - coast day1 afternoon minimum: ";
		  break;
		case COAST_MAX_DAY1_AFTERNOON:
		  retval = areaName + " - coast day1 afternoon maximum: ";
		  break;
		case COAST_MEAN_DAY1_AFTERNOON:
		  retval = areaName + " - coast day1 afternoon mean: ";
		  break;
		case COAST_MIN_DAY2_AFTERNOON:
		  retval = areaName + " - coast day2 afternoon minimum: ";
		  break;
		case COAST_MAX_DAY2_AFTERNOON:
		  retval = areaName + " - coast day2 afternoon maximum: ";
		  break;
		case COAST_MEAN_DAY2_AFTERNOON:
		  retval = areaName + " - coast day2 afternoon mean: ";
		  break;
		case UNDEFINED_WEATHER_RESULT_ID:
		  break;
		}

	  return retval;
	}

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


	const void log_weather_results(MessageLogger& theLog, 
								   weather_result_container_type& theWeatherResults, 
								   const WeatherArea& theArea)
	{
	  theLog << "Weather results: " << endl;

	  // Iterate and print out the WeatherResult variables
	  for(int i = AREA_MIN_DAY1; i < UNDEFINED_WEATHER_RESULT_ID; i++)
		{
		  if(theWeatherResults[i]->value() != kFloatMissing)
			{
			  theLog <<  weather_result_string(theArea.isNamed() ? theArea.name() : "", static_cast<weather_result_id>(i));
			  theLog << *theWeatherResults[i] << endl;
			}
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
									  const unsigned short& theForecastPeriod,
									  const unsigned short& theForecastSubPeriod,
									  const forecast_area_id& theAreaId,
									  const unsigned short& theForecastArea) 
	{
	  Sentence sentence;
	  
	  bool inlandAndCoastSeparated = (theForecastArea & COASTAL_AREA && theForecastArea & INLAND_AREA);
	  bool useAreaPhrase = ((thePeriodId == DAY1_PERIOD && theSubperiodId == UNDEFINED_SUBPERIOD) ||
							(thePeriodId == DAY2_PERIOD && !(theForecastPeriod & DAY1_PERIOD)) ||
							(thePeriodId == NIGHT_PERIOD) || // && !(theForecastPeriod & DAY1_PERIOD)) ||
							(theSubperiodId == DAY1_MORNING_PERIOD));

	  bool useDayTemperaturePhrase = (thePeriodId == DAY1_PERIOD || 
									  (thePeriodId == DAY2_PERIOD && !(theForecastPeriod & DAY1_PERIOD)));
	  if(theSeasonId == SUMMER_SEASON)
		{
		  if(thePeriodId == NIGHT_PERIOD && 
			 ((theAreaId != COASTAL_AREA) ||
			  //(theAreaId == COASTAL_AREA && (theForecastPeriod & DAY2_PERIOD) && inlandAndCoastSeparated) || 
			  (theAreaId == COASTAL_AREA && (theForecastPeriod & DAY1_PERIOD) &&
			   (theForecastSubPeriod & DAY1_MORNING_PERIOD))))
			{
			  sentence << "yön alin lämpötila" << "on";
			}
		  else if(theSubperiodId == DAY1_MORNING_PERIOD || theSubperiodId == DAY2_MORNING_PERIOD)
			{
			  sentence << "lämpötila"  << "on" << "aamulla";
			}
		  else if(theSubperiodId == DAY1_AFTERNOON_PERIOD || theSubperiodId == DAY2_AFTERNOON_PERIOD)
			{
			  sentence << "iltapäivällä";
			}
		  else if(theSubperiodId == UNDEFINED_SUBPERIOD && useDayTemperaturePhrase)
			{
				sentence << "päivän ylin lämpötila" << "on";
			}
		}
	  else
		{
		  if(thePeriodId == NIGHT_PERIOD)
			{
			  sentence << "yölämpötila" << "on";
			}
		  else if(useDayTemperaturePhrase)
			{
			  sentence << "päivälämpötila" << "on";
			}
		}

	  if(theAreaId == INLAND_AREA && inlandAndCoastSeparated && useAreaPhrase)
		{
		  sentence << "sisämaassa";
		}
	  else if(theAreaId == COASTAL_AREA && inlandAndCoastSeparated && useAreaPhrase)
		{
		  sentence << "rannikolla";
		}


	  return sentence;
	}

	const Sentence temperature_phrase(t36hparams& params)
	{
	  Sentence sentence;
	  
	  bool inlandAndCoastSeparated = (params.theForecastArea & COASTAL_AREA && params.theForecastArea & INLAND_AREA);

	  bool useAreaPhrase = ((params.thePeriodId == DAY1_PERIOD && params.theSubPeriodId == UNDEFINED_SUBPERIOD) ||
							(params.thePeriodId == DAY2_PERIOD && !(params.theForecastPeriod & DAY1_PERIOD)) ||
							(params.thePeriodId == NIGHT_PERIOD) ||
							(params.theSubPeriodId == DAY1_MORNING_PERIOD));

	  bool useDayTemperaturePhrase = (params.thePeriodId == DAY1_PERIOD || 
									  (params.thePeriodId == DAY2_PERIOD && !(params.theForecastPeriod & DAY1_PERIOD)));

	  
	  if(params.theSeasonId == SUMMER_SEASON)
		{
		  if(params.thePeriodId == NIGHT_PERIOD && !params.theNightPeriodTautologyFlag)
			{
			  sentence << "yön alin lämpötila" << "on";
			  params.theNightPeriodTautologyFlag = true;
			  params.theDayPeriodTautologyFlag = false;
			}
		  else
			{
			  params.theNightPeriodTautologyFlag = false;

			  if(params.theSubPeriodId == DAY1_MORNING_PERIOD || params.theSubPeriodId == DAY2_MORNING_PERIOD)
				{
				  sentence << "lämpötila"  << "on" << "aamulla";
				}
			  else if(params.theSubPeriodId == DAY1_AFTERNOON_PERIOD || params.theSubPeriodId == DAY2_AFTERNOON_PERIOD)
				{
				  sentence << "iltapäivällä";
				}
			  else if(params.theSubPeriodId == UNDEFINED_SUBPERIOD && 
					  useDayTemperaturePhrase && 
					  !params.theDayPeriodTautologyFlag)
				{
				  sentence << "päivän ylin lämpötila" << "on";
				  params.theDayPeriodTautologyFlag = true;
				}
			}
		}
	  else
		{
		  if(params.thePeriodId == NIGHT_PERIOD)
			{
			  sentence << "yölämpötila" << "on";
			  params.theNightPeriodTautologyFlag = true;
			  params.theDayPeriodTautologyFlag = false;
			}
		  else 
			{
			  params.theNightPeriodTautologyFlag = false;

			  if(useDayTemperaturePhrase && !params.theDayPeriodTautologyFlag)
				{
				  sentence << "päivälämpötila" << "on";
				  params.theDayPeriodTautologyFlag = true;
				}
			}
		}

	  if(params.theAreaId == COASTAL_AREA && inlandAndCoastSeparated && 
		 useAreaPhrase && !params.theOnCoastalAreaTautologyFlag)
		{
			  sentence << "rannikolla";
			  params.theOnCoastalAreaTautologyFlag = true;
		}
	  else
		{
		  params.theOnCoastalAreaTautologyFlag = false;
		  if(params.theAreaId == INLAND_AREA && inlandAndCoastSeparated && useAreaPhrase)
			{
			  sentence << "sisämaassa";
			}		
		}

	  return sentence;

	}

	const Sentence night_sentence(const std::string theVariable,
								  const forecast_season_id& theSeasonId,
								  const forecast_area_id& theAreaId,
								  const unsigned short& theForecastArea,
								  const unsigned short& theForecastPeriod,
								  const unsigned short& theForecastSubPeriod,
								  const double& theMaxTemperatureDay1,
								  const double& theMeanTemperatureDay1,
								  const double& theMinimum,
								  const double& theMaximum,
								  const double& theMean)
	{
	  Sentence sentence;

	  double temperatureDifferenceDay1Night = 100.0;
	  bool day1PeriodIncluded = theMaxTemperatureDay1 != kFloatMissing;

	  if(day1PeriodIncluded)
		{
		  temperatureDifferenceDay1Night = theMean - theMeanTemperatureDay1;
		}

	  // If day2 is included, we have to use numbers to describe temperature, since
	  // day2 story is told before night story
	  bool nightlyMinHigherThanDailyMax = !(theForecastPeriod & DAY2_PERIOD) && 
		(theMaxTemperatureDay1 - theMinimum < 0);
	  bool smallChangeBetweenDay1AndNight = !(theForecastPeriod & DAY2_PERIOD) && 
		(day1PeriodIncluded && abs(temperatureDifferenceDay1Night) < 2.0);
	  bool moderateChangeBetweenDay1AndNight = !(theForecastPeriod & DAY2_PERIOD) && 
		(day1PeriodIncluded && abs(temperatureDifferenceDay1Night) >= 2.0 && 
		 abs(temperatureDifferenceDay1Night) <= 5.0);

	  sentence << temperature_phrase(theSeasonId, 
									 NIGHT_PERIOD, 
									 UNDEFINED_SUBPERIOD, 
									 theForecastPeriod, 
									 theForecastSubPeriod, 
									 theAreaId, 
									 theForecastArea);

	  if(smallChangeBetweenDay1AndNight) // no change or small change
		{
		  sentence << "suunnilleen sama";
		}
	  else if(moderateChangeBetweenDay1AndNight) // moderate change
		{		  
		  if(temperatureDifferenceDay1Night > 0)
			sentence << "hieman korkeampi";
		  else
			sentence << "hieman alempi";
		}
	  else if(nightlyMinHigherThanDailyMax)
		{
			sentence << "lämpötila nousee";
		}
	  else
		{
		  sentence << temperature_sentence(theVariable, theMinimum, theMaximum);
		}

	  return sentence;
	}

	const Sentence night_sentence(t36hparams& params)
	{
	  Sentence sentence;

	  double temperatureDifferenceDay1Night = 100.0;
	  bool day1PeriodIncluded = params.theMaxTemperatureDay1 != kFloatMissing;

	  if(day1PeriodIncluded)
		{
		  temperatureDifferenceDay1Night = params.theMean - params.theMeanTemperatureDay1;
		}

	  // If day2 is included, we have to use numbers to describe temperature, since
	  // day2 story is told before night story
	  bool nightlyMinHigherThanDailyMax = !(params.theForecastPeriod & DAY2_PERIOD) && 
		(params.theMaxTemperatureDay1 - params.theMinimum < 0);
	  bool smallChangeBetweenDay1AndNight = !(params.theForecastPeriod & DAY2_PERIOD) && 
		(day1PeriodIncluded && abs(temperatureDifferenceDay1Night) < 2.0);
	  bool moderateChangeBetweenDay1AndNight = !(params.theForecastPeriod & DAY2_PERIOD) && 
		(day1PeriodIncluded && abs(temperatureDifferenceDay1Night) >= 2.0 && 
		 abs(temperatureDifferenceDay1Night) <= 5.0);

	  params.thePeriodId = NIGHT_PERIOD;
	  params.theSubPeriodId = UNDEFINED_SUBPERIOD;

	  sentence << temperature_phrase(params);


	  if(smallChangeBetweenDay1AndNight) // no change or small change
		{
		  sentence << "suunnilleen sama";
		}
	  else if(moderateChangeBetweenDay1AndNight) // moderate change
		{		  
		  if(temperatureDifferenceDay1Night > 0)
			sentence << "hieman korkeampi";
		  else
			sentence << "hieman alempi";
		}
	  else if(nightlyMinHigherThanDailyMax)
		{
			sentence << "lämpötila nousee";
		}
	  else
		{
		  sentence << temperature_sentence(params.theVariable, params.theMinimum, params.theMaximum);
		}

	  params.theTomorrowTautologyFlag = false;

	  return sentence;
	}


	const Sentence day2_sentence(const std::string theVariable,
								 const forecast_season_id& theSeasonId,
								 const forecast_area_id& theAreaId,
								 const forecast_subperiod_id& theSubperiodId,
								 const unsigned short& theForecastArea,
								 const unsigned short& theForecastPeriod,
								  const unsigned short& theForecastSubPeriod,
								 const double& theMeanTemperatureDay1,
								 const double& theMinimum,
								 const double& theMaximum,
								 const double& theMean)
	{
	  Sentence sentence;

	  double temperatureDifferenceDay1Day2 = 100.0;

	  if(theForecastPeriod & DAY1_PERIOD)
		{
			temperatureDifferenceDay1Day2 =  theMean - theMeanTemperatureDay1;
		}

	  bool day1PeriodIncluded = theMeanTemperatureDay1 != kFloatMissing;
	  bool smallChangeBetweenDay1AndDay2 = day1PeriodIncluded && 
		abs(temperatureDifferenceDay1Day2) < 2.0;
	  bool moderateChangeBetweenDay1AndDay2 = day1PeriodIncluded && 
		abs(temperatureDifferenceDay1Day2) >= 2.0 && 
		abs(temperatureDifferenceDay1Day2) <= 5.0;

	  sentence << "huomenna";

	  sentence << temperature_phrase(theSeasonId, 
									 DAY2_PERIOD, 
									 theSubperiodId, 
									 theForecastPeriod, 
									 theForecastSubPeriod, 
									 theAreaId, 
									 theForecastArea);

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


	const Sentence day2_sentence(t36hparams& params)
	{
	  Sentence sentence;

	  double temperatureDifferenceDay1Day2 = 100.0;

	  if(params.theForecastPeriod & DAY1_PERIOD)
		{
			temperatureDifferenceDay1Day2 =  params.theMean - params.theMeanTemperatureDay1;
		}

	  bool day1PeriodIncluded = params.theMeanTemperatureDay1 != kFloatMissing;
	  bool smallChangeBetweenDay1AndDay2 = day1PeriodIncluded && 
		abs(temperatureDifferenceDay1Day2) < 2.0;
	  bool moderateChangeBetweenDay1AndDay2 = day1PeriodIncluded && 
		abs(temperatureDifferenceDay1Day2) >= 2.0 && 
		abs(temperatureDifferenceDay1Day2) <= 5.0;

	  if(!params.theTomorrowTautologyFlag)
		sentence << "huomenna";

	  params.theTomorrowTautologyFlag = true;

	  params.thePeriodId = DAY2_PERIOD;

	  sentence << temperature_phrase(params);


	  if(smallChangeBetweenDay1AndDay2 && params.theSubPeriodId != DAY2_MORNING_PERIOD && 
		 params.theSubPeriodId != DAY2_AFTERNOON_PERIOD)
		{

		  sentence << "suunnilleen sama";
		}
	  else if(moderateChangeBetweenDay1AndDay2 && params.theSubPeriodId != DAY2_MORNING_PERIOD &&
		 params.theSubPeriodId != DAY2_AFTERNOON_PERIOD)
		{
		  // small change
		  if(temperatureDifferenceDay1Day2 > 0)
			sentence << "hieman korkeampi";
		  else
			sentence << "hieman alempi";
		}
	  else
		{
		  sentence << temperature_sentence(params.theVariable, params.theMinimum, params.theMaximum);
		}

	  return sentence;

	}

	const Sentence day1_sentence(const std::string theVariable,
								 const forecast_season_id& theSeasonId,
								 const forecast_area_id& theAreaId,
								 const forecast_subperiod_id& theSubperiodId,
								 const unsigned short& theForecastArea,
								 const unsigned short& theForecastPeriod,
								 const unsigned short& theForecastSubPeriod,
								 const double& theMinimum,
								 const double& theMaximum,
								 const double& theMean)
	{
	  Sentence sentence;

	  sentence << temperature_phrase(theSeasonId, 
									 DAY1_PERIOD, 
									 theSubperiodId, 
									 theForecastPeriod, 
									 theForecastSubPeriod, 
									 theAreaId, 
									 theForecastArea);

	  sentence << temperature_sentence(theVariable,
									   theMinimum,
									   theMaximum);

	  return sentence;
	}

	const Sentence day1_sentence(t36hparams& params)
	{
	  Sentence sentence;

	  params.thePeriodId = DAY1_PERIOD;

	  sentence << temperature_phrase(params);

	  sentence << temperature_sentence(params.theVariable, params.theMinimum, params.theMaximum);

	  params.theTomorrowTautologyFlag = false;

	  return sentence;
	}


	const Sentence construct_sentence(const std::string theVariable,
									  const forecast_area_id& theAreaId,
									  const unsigned short& theForecastArea,
									  const forecast_period_id& thePeriodId, 
									  const unsigned short& theForecastPeriod,
									  unsigned short& theForecastSubPeriod,
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
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
											weatherResults[AREA_MIN_DAY1_MORNING]->value(),
											weatherResults[AREA_MAX_DAY1_MORNING]->value(),
											weatherResults[AREA_MEAN_DAY1_MORNING]->value());

				  sentence << Delimiter(",");

				  sentence << day1_sentence(theVariable,
											theSeasonId,
											FULL_AREA,
											DAY1_AFTERNOON_PERIOD,
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
											weatherResults[AREA_MIN_DAY1_AFTERNOON]->value(),
											weatherResults[AREA_MAX_DAY1_AFTERNOON]->value(),
											weatherResults[AREA_MEAN_DAY1_AFTERNOON]->value());

				  theForecastSubPeriod |= DAY1_MORNING_PERIOD;
				  theForecastSubPeriod |= DAY1_AFTERNOON_PERIOD;
				}
			  else
				{
				  sentence << day1_sentence(theVariable,
											theSeasonId,
											FULL_AREA,
											UNDEFINED_SUBPERIOD,
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
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
										 theForecastArea,
										 theForecastPeriod,
										 theForecastSubPeriod,
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
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
											weatherResults[AREA_MEAN_DAY1]->value(),
											weatherResults[AREA_MIN_DAY2_MORNING]->value(),
											weatherResults[AREA_MAX_DAY2_MORNING]->value(),
											weatherResults[AREA_MEAN_DAY2_MORNING]->value());

				  sentence << Delimiter(",");

				  sentence << day2_sentence(theVariable,
											theSeasonId,
											FULL_AREA,
											DAY2_AFTERNOON_PERIOD,
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
											weatherResults[AREA_MEAN_DAY1]->value(),
											weatherResults[AREA_MIN_DAY2_AFTERNOON]->value(),
											weatherResults[AREA_MAX_DAY2_AFTERNOON]->value(),
											weatherResults[AREA_MEAN_DAY2_AFTERNOON]->value());

				  theForecastSubPeriod |= DAY2_MORNING_PERIOD;
				  theForecastSubPeriod |= DAY2_AFTERNOON_PERIOD;
				}
			  else
				{
				  sentence << day2_sentence(theVariable,
											theSeasonId,
											FULL_AREA,
											UNDEFINED_SUBPERIOD,
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
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
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
											weatherResults[INLAND_MIN_DAY1_MORNING]->value(),
											weatherResults[INLAND_MAX_DAY1_MORNING]->value(),
											weatherResults[INLAND_MEAN_DAY1_MORNING]->value());

				  sentence << Delimiter(",");

				  sentence << day1_sentence(theVariable,
											theSeasonId,
											INLAND_AREA,
											DAY1_AFTERNOON_PERIOD,
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
											weatherResults[INLAND_MIN_DAY1_AFTERNOON]->value(),
											weatherResults[INLAND_MAX_DAY1_AFTERNOON]->value(),
											weatherResults[INLAND_MEAN_DAY1_AFTERNOON]->value());

				  theForecastSubPeriod |= DAY1_MORNING_PERIOD;
				  theForecastSubPeriod |= DAY1_AFTERNOON_PERIOD;
				}
			  else
				{
				  sentence << day1_sentence(theVariable,
											theSeasonId,
											INLAND_AREA,
											UNDEFINED_SUBPERIOD,
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
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
										 theForecastArea,
										 theForecastPeriod,
										 theForecastSubPeriod,
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
											INLAND_AREA,
											DAY2_MORNING_PERIOD,
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
											weatherResults[INLAND_MEAN_DAY1]->value(),
											weatherResults[INLAND_MIN_DAY2_MORNING]->value(),
											weatherResults[INLAND_MAX_DAY2_MORNING]->value(),
											weatherResults[INLAND_MEAN_DAY2_MORNING]->value());

				  sentence << Delimiter(",");

				  sentence << day2_sentence(theVariable,
											theSeasonId,
											INLAND_AREA,
											DAY2_AFTERNOON_PERIOD,
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
											weatherResults[INLAND_MEAN_DAY1]->value(),
											weatherResults[INLAND_MIN_DAY2_AFTERNOON]->value(),
											weatherResults[INLAND_MAX_DAY2_AFTERNOON]->value(),
											weatherResults[INLAND_MEAN_DAY2_AFTERNOON]->value());

				  theForecastSubPeriod |= DAY2_MORNING_PERIOD;
				  theForecastSubPeriod |= DAY2_AFTERNOON_PERIOD;
				}
			  else
				{
				  sentence << day2_sentence(theVariable,
											theSeasonId,
											INLAND_AREA,
											UNDEFINED_SUBPERIOD,
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
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
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
											weatherResults[COAST_MIN_DAY1_MORNING]->value(),
											weatherResults[COAST_MAX_DAY1_MORNING]->value(),
											weatherResults[COAST_MEAN_DAY1_MORNING]->value());

				  sentence << Delimiter(",");

				  sentence << day1_sentence(theVariable,
											theSeasonId,
											COASTAL_AREA,
											DAY1_AFTERNOON_PERIOD,
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
											weatherResults[COAST_MIN_DAY1_AFTERNOON]->value(),
											weatherResults[COAST_MAX_DAY1_AFTERNOON]->value(),
											weatherResults[COAST_MEAN_DAY1_AFTERNOON]->value());

				  theForecastSubPeriod |= DAY1_MORNING_PERIOD;
				  theForecastSubPeriod |= DAY1_AFTERNOON_PERIOD;
				}
			  else
				{
				  sentence << day1_sentence(theVariable,
											theSeasonId,
											COASTAL_AREA,
											UNDEFINED_SUBPERIOD,
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
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
										 theForecastArea,
										 theForecastPeriod,
										 theForecastSubPeriod,
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
											COASTAL_AREA,
											DAY2_MORNING_PERIOD,
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
											weatherResults[COAST_MEAN_DAY1]->value(),
											weatherResults[COAST_MIN_DAY2_MORNING]->value(),
											weatherResults[COAST_MAX_DAY2_MORNING]->value(),
											weatherResults[COAST_MEAN_DAY2_MORNING]->value());

				  sentence << Delimiter(",");

				  sentence << day2_sentence(theVariable,
											theSeasonId,
											COASTAL_AREA,
											DAY2_AFTERNOON_PERIOD,
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
											weatherResults[COAST_MEAN_DAY1]->value(),
											weatherResults[COAST_MIN_DAY2_AFTERNOON]->value(),
											weatherResults[COAST_MAX_DAY2_AFTERNOON]->value(),
											weatherResults[COAST_MEAN_DAY2_AFTERNOON]->value());

				  theForecastSubPeriod |= DAY2_MORNING_PERIOD;
				  theForecastSubPeriod |= DAY2_AFTERNOON_PERIOD;
				}
			  else
				{
				  sentence << day2_sentence(theVariable,
											theSeasonId,
											COASTAL_AREA,
											UNDEFINED_SUBPERIOD,
											theForecastArea,
											theForecastPeriod,
											theForecastSubPeriod,
											weatherResults[COAST_MEAN_DAY1]->value(),
											weatherResults[coastMin]->value(),
											weatherResults[coastMax]->value(),
											weatherResults[coastMean]->value());
				}
			}
		}

	  return sentence;
	}

	const Sentence construct_sentence(t36hparams& params)
	{
	  Sentence sentence;

	  int areaMin = (params.thePeriodId == DAY1_PERIOD ? AREA_MIN_DAY1 : 
					 (params.thePeriodId == NIGHT_PERIOD ? AREA_MIN_NIGHT : AREA_MIN_DAY2));
	  int areaMax = (params.thePeriodId == DAY1_PERIOD ? AREA_MAX_DAY1 : 
					 (params.thePeriodId == NIGHT_PERIOD ? AREA_MAX_NIGHT : AREA_MAX_DAY2));
	  int areaMean = (params.thePeriodId == DAY1_PERIOD ? AREA_MEAN_DAY1 : 
					  (params.thePeriodId == NIGHT_PERIOD ? AREA_MEAN_NIGHT : AREA_MEAN_DAY2));
	  int coastMin = (params.thePeriodId == DAY1_PERIOD ? COAST_MIN_DAY1 : 
					  (params.thePeriodId == NIGHT_PERIOD ? COAST_MIN_NIGHT : COAST_MIN_DAY2));
	  int coastMax = (params.thePeriodId == DAY1_PERIOD ? COAST_MAX_DAY1 : 
					  (params.thePeriodId == NIGHT_PERIOD ? COAST_MAX_NIGHT : COAST_MAX_DAY2));
	  int coastMean = (params.thePeriodId == DAY1_PERIOD ? COAST_MEAN_DAY1 : 
					   (params.thePeriodId == NIGHT_PERIOD ? COAST_MEAN_NIGHT : COAST_MEAN_DAY2));
	  int inlandMin = (params.thePeriodId == DAY1_PERIOD ? INLAND_MIN_DAY1 : 
					   (params.thePeriodId == NIGHT_PERIOD ? INLAND_MIN_NIGHT : INLAND_MIN_DAY2));
	  int inlandMax = (params.thePeriodId == DAY1_PERIOD ? INLAND_MAX_DAY1 : 

					   (params.thePeriodId == NIGHT_PERIOD ? INLAND_MAX_NIGHT : INLAND_MAX_DAY2));
	  int inlandMean = (params.thePeriodId == DAY1_PERIOD ? INLAND_MEAN_DAY1 : 
						(params.thePeriodId == NIGHT_PERIOD ? INLAND_MEAN_NIGHT : INLAND_MEAN_DAY2));

	  if(params.theAreaId == FULL_AREA)
		{
		  if(params.thePeriodId == DAY1_PERIOD)
			{
			  if(params.theSeasonId == SUMMER_SEASON && 
				 params.theWeatherResults[AREA_MAX_DAY1_MORNING]->value() > 
				 params.theWeatherResults[AREA_MAX_DAY1_AFTERNOON]->value())
				{
				  params.theAreaId = FULL_AREA;
				  params.theSubPeriodId = DAY1_MORNING_PERIOD;
				  params.theMinimum = params.theWeatherResults[AREA_MIN_DAY1_MORNING]->value();
				  params.theMaximum = params.theWeatherResults[AREA_MAX_DAY1_MORNING]->value();
				  params.theMean = params.theWeatherResults[AREA_MEAN_DAY1_MORNING]->value();

				  sentence << day1_sentence(params);

				  sentence << Delimiter(",");

				  params.theForecastSubPeriod |= DAY1_MORNING_PERIOD;
				  params.theForecastSubPeriod |= DAY1_AFTERNOON_PERIOD;

				  params.theAreaId = FULL_AREA;
				  params.theSubPeriodId = DAY1_AFTERNOON_PERIOD;
				  params.theMinimum = params.theWeatherResults[AREA_MIN_DAY1_AFTERNOON]->value();
				  params.theMaximum = params.theWeatherResults[AREA_MAX_DAY1_AFTERNOON]->value();
				  params.theMean = params.theWeatherResults[AREA_MEAN_DAY1_AFTERNOON]->value();

				  sentence << day1_sentence(params);

				}
			  else
				{
				  params.theAreaId = FULL_AREA;
				  params.theSubPeriodId = UNDEFINED_SUBPERIOD;
				  params.theMinimum = params.theWeatherResults[areaMin]->value();
				  params.theMaximum = params.theWeatherResults[areaMax]->value();
				  params.theMean = params.theWeatherResults[areaMean]->value();

				  sentence << day1_sentence(params);

				}

			}
		  else if(params.thePeriodId == NIGHT_PERIOD)
			{
			  params.theAreaId = FULL_AREA;
			  params.theMaxTemperatureDay1 = params.theWeatherResults[AREA_MAX_DAY1]->value();
			  params.theMeanTemperatureDay1 = params.theWeatherResults[AREA_MEAN_DAY1]->value();
			  params.theMinimum = params.theWeatherResults[areaMin]->value();
			  params.theMaximum = params.theWeatherResults[areaMax]->value();
			  params.theMean = params.theWeatherResults[areaMean]->value();

			  sentence << night_sentence(params);

			}
		  else if(params.thePeriodId == DAY2_PERIOD)
			{
			  if(params.theSeasonId == SUMMER_SEASON && 
				 params.theWeatherResults[INLAND_MAX_DAY2_MORNING]->value() > 
				 params.theWeatherResults[INLAND_MAX_DAY2_AFTERNOON]->value())
				{
				  params.theAreaId = FULL_AREA;
				  params.theSubPeriodId = DAY2_MORNING_PERIOD;
				  params.theMeanTemperatureDay1 = params.theWeatherResults[AREA_MEAN_DAY1]->value();
				  params.theMinimum = params.theWeatherResults[AREA_MIN_DAY2_MORNING]->value();
				  params.theMaximum = params.theWeatherResults[AREA_MAX_DAY2_MORNING]->value();
				  params.theMean = params.theWeatherResults[AREA_MEAN_DAY2_MORNING]->value();

				  sentence << day2_sentence(params);

				  sentence << Delimiter(",");

				  params.theForecastSubPeriod |= DAY2_MORNING_PERIOD;
				  params.theForecastSubPeriod |= DAY2_AFTERNOON_PERIOD;
				  params.theAreaId = FULL_AREA;
				  params.theSubPeriodId = DAY2_AFTERNOON_PERIOD;
				  params.theMeanTemperatureDay1 = params.theWeatherResults[AREA_MEAN_DAY1]->value();
				  params.theMinimum = params.theWeatherResults[AREA_MIN_DAY2_AFTERNOON]->value();
				  params.theMaximum = params.theWeatherResults[AREA_MAX_DAY2_AFTERNOON]->value();
				  params.theMean = params.theWeatherResults[AREA_MEAN_DAY2_AFTERNOON]->value();

				  sentence << day2_sentence(params);
				}
			  else
				{
				  params.theAreaId = FULL_AREA;
				  params.theSubPeriodId = UNDEFINED_SUBPERIOD;
				  params.theMeanTemperatureDay1 = params.theWeatherResults[AREA_MEAN_DAY1]->value();
				  params.theMinimum = params.theWeatherResults[areaMin]->value();
				  params.theMaximum = params.theWeatherResults[areaMax]->value();
				  params.theMean = params.theWeatherResults[areaMean]->value();

				  sentence << day2_sentence(params);

				}
			}
		}
	  else if(params.theAreaId == INLAND_AREA)
		{
		  if(params.thePeriodId == DAY1_PERIOD)
			{
			  if(params.theSeasonId == SUMMER_SEASON && 
				 params.theWeatherResults[INLAND_MAX_DAY1_MORNING]->value() > 
				 params.theWeatherResults[INLAND_MAX_DAY1_AFTERNOON]->value())
				{
				  params.theAreaId = INLAND_AREA;
				  params.theSubPeriodId = DAY1_MORNING_PERIOD;
				  params.theMinimum = params.theWeatherResults[INLAND_MIN_DAY1_MORNING]->value();
				  params.theMaximum = params.theWeatherResults[INLAND_MAX_DAY1_MORNING]->value();
				  params.theMean = params.theWeatherResults[INLAND_MEAN_DAY1_MORNING]->value();

				  sentence << day1_sentence(params);


				  sentence << Delimiter(",");

				  params.theForecastSubPeriod |= DAY1_MORNING_PERIOD;
				  params.theForecastSubPeriod |= DAY1_AFTERNOON_PERIOD;
				  params.theAreaId = INLAND_AREA;
				  params.theSubPeriodId = DAY1_AFTERNOON_PERIOD;
				  params.theMinimum = params.theWeatherResults[INLAND_MIN_DAY1_AFTERNOON]->value();
				  params.theMaximum = params.theWeatherResults[INLAND_MAX_DAY1_AFTERNOON]->value();
				  params.theMean = params.theWeatherResults[INLAND_MEAN_DAY1_AFTERNOON]->value();

				  sentence << day1_sentence(params);
				}
			  else
				{
				  params.theAreaId = INLAND_AREA;
				  params.theSubPeriodId = UNDEFINED_SUBPERIOD;
				  params.theMinimum = params.theWeatherResults[inlandMin]->value();
				  params.theMaximum = params.theWeatherResults[inlandMax]->value();
				  params.theMean = params.theWeatherResults[inlandMean]->value();

				  sentence << day1_sentence(params);

				}
			}
		  else if(params.thePeriodId == NIGHT_PERIOD)
			{
			  params.theAreaId = INLAND_AREA;
			  params.theMaxTemperatureDay1 = params.theWeatherResults[INLAND_MAX_DAY1]->value();
			  params.theMeanTemperatureDay1 = params.theWeatherResults[INLAND_MEAN_DAY1]->value();
			  params.theMinimum = params.theWeatherResults[inlandMin]->value();
			  params.theMaximum = params.theWeatherResults[inlandMax]->value();
			  params.theMean = params.theWeatherResults[inlandMean]->value();

			  sentence << night_sentence(params);


			}
		  else if(params.thePeriodId == DAY2_PERIOD)
			{
			  if(params.theSeasonId == SUMMER_SEASON && 
				 params.theWeatherResults[INLAND_MAX_DAY2_MORNING]->value() > 
				 params.theWeatherResults[INLAND_MAX_DAY2_AFTERNOON]->value())
				{
				  params.theAreaId = INLAND_AREA;
				  params.theSubPeriodId = DAY2_MORNING_PERIOD;
				  params.theMeanTemperatureDay1 = params.theWeatherResults[INLAND_MEAN_DAY1]->value();
				  params.theMinimum = params.theWeatherResults[INLAND_MIN_DAY2_MORNING]->value();
				  params.theMaximum = params.theWeatherResults[INLAND_MAX_DAY2_MORNING]->value();
				  params.theMean = params.theWeatherResults[INLAND_MEAN_DAY2_MORNING]->value();

				  sentence << day2_sentence(params);

				  sentence << Delimiter(",");

				  params.theForecastSubPeriod |= DAY2_MORNING_PERIOD;
				  params.theForecastSubPeriod |= DAY2_AFTERNOON_PERIOD;
				  params.theAreaId = INLAND_AREA;
				  params.theSubPeriodId = DAY2_AFTERNOON_PERIOD;
				  params.theMeanTemperatureDay1 = params.theWeatherResults[INLAND_MEAN_DAY1]->value();
				  params.theMinimum = params.theWeatherResults[INLAND_MIN_DAY2_AFTERNOON]->value();
				  params.theMaximum = params.theWeatherResults[INLAND_MAX_DAY2_AFTERNOON]->value();
				  params.theMean = params.theWeatherResults[INLAND_MEAN_DAY2_AFTERNOON]->value();

				  sentence << day2_sentence(params);

				}
			  else
				{
				  params.theAreaId = INLAND_AREA;
				  params.theSubPeriodId = UNDEFINED_SUBPERIOD;
				  params.theMeanTemperatureDay1 = params.theWeatherResults[INLAND_MEAN_DAY1]->value();
				  params.theMinimum = params.theWeatherResults[inlandMin]->value();
				  params.theMaximum = params.theWeatherResults[inlandMax]->value();
				  params.theMean = params.theWeatherResults[inlandMean]->value();

				  sentence << day2_sentence(params);

				}
			}
		}
	  else if(params.theAreaId == COASTAL_AREA)
		{
		  if(params.thePeriodId == DAY1_PERIOD)
			{
			  if(params.theSeasonId == SUMMER_SEASON && 
				 params.theWeatherResults[COAST_MAX_DAY1_MORNING]->value() > 
				 params.theWeatherResults[COAST_MAX_DAY1_AFTERNOON]->value())
				{
				  params.theAreaId = COASTAL_AREA;
				  params.theSubPeriodId = DAY1_MORNING_PERIOD;
				  params.theMinimum = params.theWeatherResults[COAST_MIN_DAY1_MORNING]->value();
				  params.theMaximum = params.theWeatherResults[COAST_MAX_DAY1_MORNING]->value();
				  params.theMean = params.theWeatherResults[COAST_MEAN_DAY1_MORNING]->value();

				  sentence << day1_sentence(params);

				  sentence << Delimiter(",");

				  params.theForecastSubPeriod |= DAY1_MORNING_PERIOD;
				  params.theForecastSubPeriod |= DAY1_AFTERNOON_PERIOD;

				  params.theAreaId = COASTAL_AREA;
				  params.theSubPeriodId = DAY1_AFTERNOON_PERIOD;
				  params.theMinimum = params.theWeatherResults[COAST_MIN_DAY1_AFTERNOON]->value();
				  params.theMaximum = params.theWeatherResults[COAST_MAX_DAY1_AFTERNOON]->value();
				  params.theMean = params.theWeatherResults[COAST_MEAN_DAY1_AFTERNOON]->value();

				  sentence << day1_sentence(params);

				}
			  else
				{
				  params.theAreaId = COASTAL_AREA;
				  params.theSubPeriodId = UNDEFINED_SUBPERIOD;
				  params.theMinimum = params.theWeatherResults[coastMin]->value();
				  params.theMaximum = params.theWeatherResults[coastMax]->value();
				  params.theMean = params.theWeatherResults[coastMean]->value();

				  sentence << day1_sentence(params);
				}
			}
		  else if(params.thePeriodId == NIGHT_PERIOD)
			{
			  params.theAreaId = COASTAL_AREA;
			  params.theMaxTemperatureDay1 = params.theWeatherResults[COAST_MAX_DAY1]->value();
			  params.theMeanTemperatureDay1 = params.theWeatherResults[COAST_MEAN_DAY1]->value();
			  params.theMinimum = params.theWeatherResults[coastMin]->value();
			  params.theMaximum = params.theWeatherResults[coastMax]->value();
			  params.theMean = params.theWeatherResults[coastMean]->value();

			  sentence << night_sentence(params);

			}
		  else if(params.thePeriodId == DAY2_PERIOD)
			{
			  if(params.theSeasonId == SUMMER_SEASON && 
				 params.theWeatherResults[COAST_MAX_DAY2_MORNING]->value() > 
				 params.theWeatherResults[COAST_MAX_DAY2_AFTERNOON]->value())
				{
				  params.theAreaId = COASTAL_AREA;
				  params.theSubPeriodId = DAY2_MORNING_PERIOD;
				  params.theMeanTemperatureDay1 = params.theWeatherResults[COAST_MEAN_DAY1]->value();
				  params.theMinimum = params.theWeatherResults[COAST_MIN_DAY2_MORNING]->value();
				  params.theMaximum = params.theWeatherResults[COAST_MAX_DAY2_MORNING]->value();
				  params.theMean = params.theWeatherResults[COAST_MEAN_DAY2_MORNING]->value();

				  sentence << day2_sentence(params);

				  sentence << Delimiter(",");

				  params.theForecastSubPeriod |= DAY2_MORNING_PERIOD;
				  params.theForecastSubPeriod |= DAY2_AFTERNOON_PERIOD;
				  params.theAreaId = COASTAL_AREA;
				  params.theSubPeriodId = DAY2_AFTERNOON_PERIOD;
				  params.theMeanTemperatureDay1 = params.theWeatherResults[COAST_MEAN_DAY1]->value();
				  params.theMinimum = params.theWeatherResults[COAST_MIN_DAY2_AFTERNOON]->value();
				  params.theMaximum = params.theWeatherResults[COAST_MAX_DAY2_AFTERNOON]->value();
				  params.theMean = params.theWeatherResults[COAST_MEAN_DAY2_AFTERNOON]->value();

				  sentence << day2_sentence(params);
				}
			  else
				{
				  params.theAreaId = COASTAL_AREA;
				  params.theSubPeriodId = UNDEFINED_SUBPERIOD;
				  params.theMeanTemperatureDay1 = params.theWeatherResults[COAST_MEAN_DAY1]->value();
				  params.theMinimum = params.theWeatherResults[coastMin]->value();
				  params.theMaximum = params.theWeatherResults[coastMax]->value();
				  params.theMean = params.theWeatherResults[coastMean]->value();

				  sentence << day2_sentence(params);

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

	  if(forecast_area == NO_AREA)
		return sentence;

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
	  // 2. Day2 inland
	  // 3. Night coastal
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

	  bool separate_inland_and_coast_day1 = false;
	  bool separate_inland_and_coast_day2 = false;
	  bool separate_inland_and_coast_night = false;

	  if(weatherResults[INLAND_MEAN_DAY1]->value() != kFloatMissing &&
		 weatherResults[COAST_MEAN_DAY1]->value() != kFloatMissing)
		{
		  float temperature_diff_day1 = abs(round(weatherResults[INLAND_MEAN_DAY1]->value() - 
												  weatherResults[COAST_MEAN_DAY1]->value()));
		  separate_inland_and_coast_day1 = (temperature_diff_day1 >= temperature_limit_coast_inland);
		}

	  if(weatherResults[INLAND_MEAN_DAY2]->value() != kFloatMissing &&
		 weatherResults[COAST_MEAN_DAY2]->value() != kFloatMissing)
		{
		  float temperature_diff_day2 = abs(weatherResults[INLAND_MEAN_DAY2]->value() - 
											weatherResults[COAST_MEAN_DAY2]->value());
		  separate_inland_and_coast_day2 = (temperature_diff_day2 >= temperature_limit_coast_inland);
		}

	  if(separate_inland_and_coast_day1)
		separate_inland_and_coast_day2 = true;

	  if(weatherResults[INLAND_MEAN_NIGHT]->value() != kFloatMissing &&
		 weatherResults[COAST_MEAN_NIGHT]->value() != kFloatMissing)
		{
		  float temperature_diff_night = abs(weatherResults[INLAND_MEAN_NIGHT]->value() - 
											 weatherResults[COAST_MEAN_NIGHT]->value());
		  separate_inland_and_coast_night = (temperature_diff_night >= temperature_limit_coast_inland);
		}


	  if(processingOrder == DAY1_DAY2_NIGHT)
		{
		  if(separate_inland_and_coast_day1)
			  periodAreas.push_back(DAY1_INLAND);
		  else
			  periodAreas.push_back(DAY1_FULL);

		  periodAreas.push_back(DELIMITER_COMMA);

		  if(separate_inland_and_coast_day2)
			  periodAreas.push_back(DAY2_INLAND);
		  else
			  periodAreas.push_back(DAY2_FULL);

		  periodAreas.push_back(DELIMITER_COMMA);

		  if(separate_inland_and_coast_night)
			  periodAreas.push_back(NIGHT_INLAND);
		  else
			  periodAreas.push_back(NIGHT_FULL);

		  if(separate_inland_and_coast_day1)
			{
			  periodAreas.push_back(DELIMITER_DOT);
			  periodAreas.push_back(DAY1_COASTAL);
			}
		  if(separate_inland_and_coast_day2)
			{
			  periodAreas.push_back(DELIMITER_COMMA);
			  periodAreas.push_back(DAY2_COASTAL);
			}
		  if(separate_inland_and_coast_night)
			{
			  periodAreas.push_back(DELIMITER_COMMA);
			  periodAreas.push_back(NIGHT_COASTAL);
			}
		}
	  else if(processingOrder == DAY1_NIGHT)
		{
		  if(separate_inland_and_coast_day1)
			{
			  periodAreas.push_back(DAY1_INLAND);
			}
		  else
			{
			  periodAreas.push_back(DAY1_FULL);
			}

		  periodAreas.push_back(DELIMITER_COMMA);

		  if(separate_inland_and_coast_night)
			{
			  periodAreas.push_back(NIGHT_INLAND);
			}
		  else
			{
			  periodAreas.push_back(NIGHT_FULL);
			}

		  if(separate_inland_and_coast_day1)
			{
			  periodAreas.push_back(DELIMITER_DOT);
			  periodAreas.push_back(DAY1_COASTAL);

			  if(separate_inland_and_coast_night)
				{
				  periodAreas.push_back(DELIMITER_COMMA);
				  periodAreas.push_back(NIGHT_COASTAL);
				}
			}
		  else if(separate_inland_and_coast_night)
			{
			  periodAreas.push_back(DELIMITER_COMMA);
			  periodAreas.push_back(NIGHT_COASTAL);
			}
		}
	  else if(processingOrder == NIGHT_DAY2)
		{
		  if(separate_inland_and_coast_night)
			{
			  periodAreas.push_back(NIGHT_INLAND);
			  periodAreas.push_back(DELIMITER_COMMA);
			  periodAreas.push_back(NIGHT_COASTAL);
			}
		  else
			{
			  periodAreas.push_back(NIGHT_FULL);
			}

		  periodAreas.push_back(DELIMITER_COMMA);

		  if(separate_inland_and_coast_day2)
			{
			  periodAreas.push_back(DAY2_INLAND);
			  periodAreas.push_back(DELIMITER_COMMA);
			  periodAreas.push_back(DAY2_COASTAL);
			}
		  else
			{
			  periodAreas.push_back(DAY2_FULL);
			}
		}

	  unsigned short story_forecast_subperiods = UNDEFINED_SUBPERIOD;

	  for(unsigned int i = 0; i < periodAreas.size(); i++)
		{
		  if(periodAreas[i] == DELIMITER_COMMA || periodAreas[i] == DELIMITER_DOT)
			{
			  continue;
			}

		  unsigned short story_forecast_areas = 0x0;
		  forecast_period_id period_id;
		  forecast_area_id area_id;

		  int periodArea = periodAreas[i];
   
		  if(periodArea == DAY1_INLAND || periodArea == DAY1_COASTAL || periodArea == DAY1_FULL)
			{
			  period_id = DAY1_PERIOD;

			  if(forecast_area & INLAND_AREA && forecast_area & COASTAL_AREA)
				{
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
			  else if(forecast_area & INLAND_AREA)
				{
				  area_id = INLAND_AREA;
				  story_forecast_areas |= INLAND_AREA;
				}
			  else if(forecast_area & COASTAL_AREA)
				{
				  area_id = COASTAL_AREA;
				  story_forecast_areas |= COASTAL_AREA;
				}
			}
		  else if(periodArea == NIGHT_INLAND || periodArea == NIGHT_COASTAL || periodArea == NIGHT_FULL)
			{	
			  period_id = NIGHT_PERIOD;
			  if(forecast_area & INLAND_AREA && forecast_area & COASTAL_AREA)
				{
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
			  else if(forecast_area & INLAND_AREA)
				{
				  area_id = INLAND_AREA;
				  story_forecast_areas |= INLAND_AREA;
				}
			  else if(forecast_area & COASTAL_AREA)
				{
				  area_id = COASTAL_AREA;
				  story_forecast_areas |= COASTAL_AREA;
				}
			}
		  else if(periodArea == DAY2_INLAND || periodArea == DAY2_COASTAL || periodArea == DAY2_FULL)
			{
			  period_id = DAY2_PERIOD;
			  if(forecast_area & INLAND_AREA && forecast_area & COASTAL_AREA)
				{
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
			  else if(forecast_area & INLAND_AREA)
				{
				  area_id = INLAND_AREA;
				  story_forecast_areas |= INLAND_AREA;
				}
			  else if(forecast_area & COASTAL_AREA)
				{
				  area_id = COASTAL_AREA;
				  story_forecast_areas |= COASTAL_AREA;
				}
			}

		  Sentence addThisSentence;
		  addThisSentence << construct_sentence(theVariable,
												area_id,
												story_forecast_areas,
												period_id,
												theForecastPeriod,
												story_forecast_subperiods,
												forecast_season,
												weatherResults);

		  if(!addThisSentence.empty())
			{
			  if(!sentence.empty() && i > 0)
				{
				  if(periodAreas[i-1] == DELIMITER_COMMA)
					sentence << Delimiter(",");
				  else if(periodAreas[i-1] == DELIMITER_DOT)
					sentence << Delimiter(".");
				}

			  sentence << addThisSentence;
			}
		}

	  return sentence;
	}






	const Sentence temperature_max36hours_sentence(t36hparams& params)
	{
	  Sentence sentence;

	  if(params.theForecastArea == NO_AREA)
		return sentence;

	  const int temperature_limit_coast_inland = optional_int(params.theVariable + "::temperature_limit_coast_inland", 2);

	  forecast_season_id forecast_season(UNDEFINED_SEASON);

	  // find out seasons of the forecast periods
	  if(params.theForecastPeriod & DAY1_PERIOD)
		{
		  forecast_season = get_forecast_season(params.theGenerator.period(1).localStartTime(), params.theVariable);
		}
	  else if(params.theForecastPeriod & NIGHT_PERIOD)
		{
		  forecast_season = get_forecast_season(params.theGenerator.period(1).localStartTime(), params.theVariable);
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
	  // 2. Day2 inland
	  // 3. Night coastal
	  // 4. Day2 coastal

	  vector<int> periodAreas;
	  processing_order processingOrder;

	  if(params.theForecastPeriod & DAY1_PERIOD && 
		 params.theForecastPeriod & NIGHT_PERIOD && 
		 params.theForecastPeriod & DAY2_PERIOD)
		{
		  processingOrder = DAY1_DAY2_NIGHT;
		}
	  else if(params.theForecastPeriod & NIGHT_PERIOD && 
			  params.theForecastPeriod & DAY2_PERIOD)
		{
		  processingOrder = NIGHT_DAY2;
		}
	  else if(params.theForecastPeriod & DAY1_PERIOD && 
			  params.theForecastPeriod & NIGHT_PERIOD)
		{
		  processingOrder = DAY1_NIGHT;
		}

	  bool separate_inland_and_coast_day1 = false;
	  bool separate_inland_and_coast_day2 = false;
	  bool separate_inland_and_coast_night = false;

	  if(params.theWeatherResults[INLAND_MEAN_DAY1]->value() != kFloatMissing &&
		 params.theWeatherResults[COAST_MEAN_DAY1]->value() != kFloatMissing)
		{
		  float temperature_diff_day1 = abs(round(params.theWeatherResults[INLAND_MEAN_DAY1]->value() - 
												  params.theWeatherResults[COAST_MEAN_DAY1]->value()));
		  separate_inland_and_coast_day1 = (temperature_diff_day1 >= temperature_limit_coast_inland);
		}

	  if(params.theWeatherResults[INLAND_MEAN_DAY2]->value() != kFloatMissing &&
		 params.theWeatherResults[COAST_MEAN_DAY2]->value() != kFloatMissing)
		{
		  float temperature_diff_day2 = abs(params.theWeatherResults[INLAND_MEAN_DAY2]->value() - 
											params.theWeatherResults[COAST_MEAN_DAY2]->value());
		  separate_inland_and_coast_day2 = (temperature_diff_day2 >= temperature_limit_coast_inland);
		}

	  if(separate_inland_and_coast_day1)
		separate_inland_and_coast_day2 = true;

	  if(params.theWeatherResults[INLAND_MEAN_NIGHT]->value() != kFloatMissing &&
		 params.theWeatherResults[COAST_MEAN_NIGHT]->value() != kFloatMissing)
		{
		  float temperature_diff_night = abs(params.theWeatherResults[INLAND_MEAN_NIGHT]->value() - 
											 params.theWeatherResults[COAST_MEAN_NIGHT]->value());
		  separate_inland_and_coast_night = (temperature_diff_night >= temperature_limit_coast_inland);
		}


	  if(processingOrder == DAY1_DAY2_NIGHT)
		{
		  if(separate_inland_and_coast_day1)
			  periodAreas.push_back(DAY1_INLAND);
		  else
			  periodAreas.push_back(DAY1_FULL);

		  periodAreas.push_back(DELIMITER_COMMA);

		  if(separate_inland_and_coast_day2)
			  periodAreas.push_back(DAY2_INLAND);
		  else
			  periodAreas.push_back(DAY2_FULL);

		  periodAreas.push_back(DELIMITER_COMMA);

		  if(separate_inland_and_coast_night)
			  periodAreas.push_back(NIGHT_INLAND);
		  else
			  periodAreas.push_back(NIGHT_FULL);

		  if(separate_inland_and_coast_day1)
			{
			  periodAreas.push_back(DELIMITER_DOT);
			  periodAreas.push_back(DAY1_COASTAL);
			}
		  if(separate_inland_and_coast_day2)
			{
			  periodAreas.push_back(DELIMITER_COMMA);
			  periodAreas.push_back(DAY2_COASTAL);
			}
		  if(separate_inland_and_coast_night)
			{
			  periodAreas.push_back(DELIMITER_COMMA);
			  periodAreas.push_back(NIGHT_COASTAL);
			}
		}
	  else if(processingOrder == DAY1_NIGHT)
		{
		  if(separate_inland_and_coast_day1)
			{
			  periodAreas.push_back(DAY1_INLAND);
			  periodAreas.push_back(DELIMITER_COMMA);
			  periodAreas.push_back(DAY1_COASTAL);
			}
		  else
			{
			  periodAreas.push_back(DAY1_FULL);
			}

		  periodAreas.push_back(DELIMITER_DOT);

		  if(separate_inland_and_coast_night)
			{
			  periodAreas.push_back(NIGHT_INLAND);
			  periodAreas.push_back(DELIMITER_COMMA);
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
			  periodAreas.push_back(DELIMITER_COMMA);
			  periodAreas.push_back(NIGHT_COASTAL);
			}
		  else
			{
			  periodAreas.push_back(NIGHT_FULL);
			}

		  periodAreas.push_back(DELIMITER_COMMA);

		  if(separate_inland_and_coast_day2)
			{
			  periodAreas.push_back(DAY2_INLAND);
			  periodAreas.push_back(DELIMITER_COMMA);
			  periodAreas.push_back(DAY2_COASTAL);
			}
		  else
			{
			  periodAreas.push_back(DAY2_FULL);
			}

		  periodAreas.push_back(DELIMITER_COMMA);
		}

	  //unsigned short story_forecast_subperiods = UNDEFINED_SUBPERIOD;

	  for(unsigned int i = 0; i < periodAreas.size(); i++)
		{
		  if(periodAreas[i] == DELIMITER_COMMA || periodAreas[i] == DELIMITER_DOT)
			{
			  continue;
			}

		  unsigned short story_forecast_areas = 0x0;
		  forecast_period_id period_id;
		  forecast_area_id area_id;

		  int periodArea = periodAreas[i];
   
		  if(periodArea == DAY1_INLAND || periodArea == DAY1_COASTAL || periodArea == DAY1_FULL)
			{
			  period_id = DAY1_PERIOD;

			  if(params.theForecastArea & INLAND_AREA && params.theForecastArea & COASTAL_AREA)
				{
				  if(separate_inland_and_coast_day1)
					{
					  area_id = periodAreas[i] == DAY1_INLAND ? INLAND_AREA : COASTAL_AREA;
					  story_forecast_areas |= COASTAL_AREA;
					  story_forecast_areas |= INLAND_AREA;
					  params.theForecastAreaDay1 |= COASTAL_AREA;
					  params.theForecastAreaDay1 |= INLAND_AREA;
					}
				  else
					{
					  area_id = FULL_AREA;
					  story_forecast_areas |= FULL_AREA;
					  params.theForecastAreaDay1 |= FULL_AREA;
					}
				}
			  else if(params.theForecastArea & INLAND_AREA)
				{
				  area_id = INLAND_AREA;
				  story_forecast_areas |= INLAND_AREA;
				  params.theForecastAreaDay1 |= INLAND_AREA;
				}
			  else if(params.theForecastArea & COASTAL_AREA)
				{
				  area_id = COASTAL_AREA;
				  story_forecast_areas |= COASTAL_AREA;
				  params.theForecastAreaDay1 |= COASTAL_AREA;
				}
			}
		  else if(periodArea == NIGHT_INLAND || periodArea == NIGHT_COASTAL || periodArea == NIGHT_FULL)
			{	
			  period_id = NIGHT_PERIOD;
			  if(params.theForecastArea & INLAND_AREA && params.theForecastArea & COASTAL_AREA)
				{
				  if(separate_inland_and_coast_night)
					{
					  area_id = periodAreas[i] == NIGHT_INLAND ? INLAND_AREA : COASTAL_AREA;
					  story_forecast_areas |= COASTAL_AREA;
					  story_forecast_areas |= INLAND_AREA;
					  params.theForecastAreaNight |= COASTAL_AREA;
					  params.theForecastAreaNight |= INLAND_AREA;
					}
				  else
					{
					  area_id = FULL_AREA;
					  story_forecast_areas |= FULL_AREA;
					  params.theForecastAreaNight |= FULL_AREA;
					}
				}
			  else if(params.theForecastArea & INLAND_AREA)
				{
				  area_id = INLAND_AREA;
				  story_forecast_areas |= INLAND_AREA;
					  params.theForecastAreaNight |= INLAND_AREA;
				}
			  else if(params.theForecastArea & COASTAL_AREA)
				{
				  area_id = COASTAL_AREA;
				  story_forecast_areas |= COASTAL_AREA;
				  params.theForecastAreaNight |= COASTAL_AREA;
				}
			}
		  else if(periodArea == DAY2_INLAND || periodArea == DAY2_COASTAL || periodArea == DAY2_FULL)
			{
			  period_id = DAY2_PERIOD;
			  if(params.theForecastArea & INLAND_AREA && params.theForecastArea & COASTAL_AREA)
				{
				  if(separate_inland_and_coast_day2)
					{
					  area_id = periodAreas[i] == DAY2_INLAND ? INLAND_AREA : COASTAL_AREA;
					  story_forecast_areas |= COASTAL_AREA;
					  story_forecast_areas |= INLAND_AREA;
					  params.theForecastAreaDay2 |= COASTAL_AREA;
					  params.theForecastAreaDay2 |= INLAND_AREA;
					}
				  else
					{
					  area_id = FULL_AREA;
					  story_forecast_areas |= FULL_AREA;
					  params.theForecastAreaDay2 |= FULL_AREA;
					}
				}
			  else if(params.theForecastArea & INLAND_AREA)
				{
				  area_id = INLAND_AREA;
				  story_forecast_areas |= INLAND_AREA;
				  params.theForecastAreaDay2 |= INLAND_AREA;
				}
			  else if(params.theForecastArea & COASTAL_AREA)
				{
				  area_id = COASTAL_AREA;
				  story_forecast_areas |= COASTAL_AREA;
				  params.theForecastAreaDay2 |= COASTAL_AREA;
				}
			}

		  Sentence addThisSentence;

		  params.theAreaId = area_id;
		  params.thePeriodId = period_id;
		  params.theSeasonId = forecast_season;

		  addThisSentence << construct_sentence(params);

		  if(!addThisSentence.empty())
			{
			  
			  if(!sentence.empty() && i > 0)
				{
				  if(periodAreas[i-1] == DELIMITER_COMMA)
					sentence << Delimiter(",");
				  else if(periodAreas[i-1] == DELIMITER_DOT)
					{
					  sentence << Delimiter(",");
					}
				}
			  
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
	/*
	if(itsArea.isNamed())
	  {
		std::string nimi(itsArea.name());
		log <<  nimi;
	  }
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

	// container to hold the results
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


	// Initialize the container for WeatherResult objects
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
		valid_value_period_check(weatherResults[INLAND_MIN_DAY1]->value(), forecast_period, DAY1_PERIOD);
		calculate_results(log, itsVar, itsSources, itsArea,
						  period, DAY1_PERIOD, forecast_season, COASTAL_AREA, weatherResults);
		valid_value_period_check(weatherResults[COAST_MIN_DAY1]->value(), forecast_period, DAY1_PERIOD);
		calculate_results(log, itsVar, itsSources, itsArea,
						  period, DAY1_PERIOD, forecast_season, FULL_AREA, weatherResults);
		valid_value_period_check(weatherResults[AREA_MIN_DAY1]->value(), forecast_period, DAY1_PERIOD);

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
				forecast_area |= (weatherResults[COAST_MIN_NIGHT]->value() != kFloatMissing ? COASTAL_AREA : 0x0); 
				forecast_area |= (weatherResults[INLAND_MIN_NIGHT]->value() != kFloatMissing ? INLAND_AREA : 0x0);

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
					valid_value_period_check(weatherResults[INLAND_MIN_DAY2]->value(), forecast_period, DAY2_PERIOD);
				  }
				if(forecast_area & COASTAL_AREA && (forecast_period & DAY2_PERIOD))
				  {
					calculate_results(log, itsVar, itsSources, itsArea,
									  period, DAY2_PERIOD, forecast_season, COASTAL_AREA, weatherResults);
					valid_value_period_check(weatherResults[COAST_MIN_DAY2]->value(), forecast_period, DAY2_PERIOD);
				  }
				if((forecast_area & INLAND_AREA && forecast_area & COASTAL_AREA) && (forecast_period & NIGHT_PERIOD))
				  {
					calculate_results(log, itsVar, itsSources, itsArea,
									  period, DAY2_PERIOD, forecast_season, FULL_AREA, weatherResults);
					valid_value_period_check(weatherResults[AREA_MIN_DAY2]->value(), forecast_period, DAY2_PERIOD);
				  }
				
				forecast_area |= (weatherResults[COAST_MIN_DAY2]->value() != kFloatMissing ? COASTAL_AREA : 0x0);
 				forecast_area |= (weatherResults[INLAND_MIN_DAY2]->value() != kFloatMissing ? INLAND_AREA : 0x0);				
			  }
		  }
	  }
	else
	  {
		// calculate results for night
		log_start_time_and_end_time(log, "Night: ", period);

		calculate_results(log, itsVar, itsSources, itsArea,
						  period, NIGHT_PERIOD, forecast_season, INLAND_AREA, weatherResults);
		//		valid_value_period_check(weatherResults[INLAND_MIN_NIGHT]->value(), forecast_period, NIGHT_PERIOD);
		calculate_results(log, itsVar, itsSources, itsArea,
						  period, NIGHT_PERIOD, forecast_season, COASTAL_AREA, weatherResults);
		//valid_value_period_check(weatherResults[COAST_MIN_NIGHT]->value(), forecast_period, NIGHT_PERIOD);
		calculate_results(log, itsVar, itsSources, itsArea,
						  period, NIGHT_PERIOD, forecast_season, FULL_AREA, weatherResults);
		//valid_value_period_check(weatherResults[AREA_MIN_NIGHT]->value(), forecast_period, NIGHT_PERIOD);

		// night period exists, so
		// if the area is included, it must have valid values
		forecast_area |= (weatherResults[COAST_MIN_NIGHT]->value() != kFloatMissing ? COASTAL_AREA : 0x0); 
		forecast_area |= (weatherResults[INLAND_MIN_NIGHT]->value() != kFloatMissing ? INLAND_AREA : 0x0);

		if(!(forecast_area & (COASTAL_AREA | INLAND_AREA)))
		  valid_value_period_check(kFloatMissing, forecast_period, NIGHT_PERIOD);


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
					valid_value_period_check(weatherResults[COAST_MIN_DAY2]->value(), forecast_period, DAY2_PERIOD);
				  }

				if(forecast_area & INLAND_AREA && forecast_area & COASTAL_AREA&& (forecast_period & DAY2_PERIOD))
				  calculate_results(log, itsVar, itsSources, itsArea,
									period, DAY2_PERIOD, forecast_season, FULL_AREA, weatherResults);
			  }
		  }
	  }

	forecast_area_id f_area_id = NO_AREA;
	forecast_period_id f_period_id;
	forecast_subperiod_id f_subperiod_id = UNDEFINED_SUBPERIOD;
	unsigned short f_area_day1 = 0x0;
	unsigned short f_area_night = 0x0;
	unsigned short f_area_day2 = 0x0;
	unsigned short f_sub_period = 0x0;
	double double1, double2, double3, double4, double5;

	t36hparams params(itsVar,
					  log,
					  generator,
					  forecast_season,
					  f_area_id,
					  f_period_id,
					  f_subperiod_id,
					  forecast_area,
					  f_area_day1,
					  f_area_night,
					  f_area_day2,
					  forecast_period,
					  f_sub_period,
					  weatherResults,
					  double1, 
					  double2, 
					  double3, 
					  double4,
					  double5);

	//	paragraph << temperature_max36hours_sentence(log, itsVar, generator, forecast_area, forecast_period, weatherResults);
	paragraph << temperature_max36hours_sentence(params);

	log_weather_results(log, weatherResults, itsArea);

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
  
