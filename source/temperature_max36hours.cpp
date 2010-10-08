// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::TemperatureStory::max36hours
 */
// ======================================================================

#include "TemperatureStory.h"
#include "FrostStory.h"

#include "SeasonTools.h"

#include "ClimatologyTools.h"
#include "GridClimatology.h"

#include "DefaultAcceptor.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "NightAndDayPeriodGenerator.h"
#include "HourPeriodGenerator.h"
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
#include "AreaTools.h"

#include <newbase/NFmiStringTools.h>
#include <newbase/NFmiGrid.h>
#include <newbase/NFmiQueryData.h>
#include <newbase/NFmiIndexMask.h>
#include <newbase/NFmiIndexMaskSource.h>
#include <newbase/NFmiIndexMaskTools.h>
#include <newbase/NFmiSettings.h>
#include <newbase/NFmiGlobals.h>

#include "TemperatureStoryTools.h"
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
	using namespace TextGen::TemperatureStoryTools;
	using namespace boost;
	using namespace std;
	using namespace Settings;
	using namespace AreaTools;
	using namespace SeasonTools;
	using MathTools::to_precision;
	using Settings::optional_int;
	using Settings::optional_bool;
	using Settings::optional_string;

#define COASTAL_AREA_IGNORE_LIMIT -15.0
#define DAY_NIGHT_SEPARATION_LIMIT 3.0
#define MORNING_AFTERNOON_SEPARATION_LIMIT 3.0
#define ABOUT_THE_SAME_UPPER_LIMIT 2.0
#define AROUND_ZERO_UPPER_LIMIT 2.5
#define SMALL_CHANGE_UPPER_LIMIT 4.5

#define NOIN_ASTETTA_LOW_TEMP_LIMIT 2.5
#define NOIN_ASTETTA_HIGH_TEMP_LIMIT 1.5
#define TIENOILLA_ASTETTA_LOW_TEMP_LIMIT 3.0
#define TIENOILLA_ASTETTA_HIGH_TEMP_LIMIT 2.0
#define LAHELLA_ASTETTA_ALI 2.0
#define LAHELLA_ASTETTA_YLI 1.0
#define TUNTUMASSA_ASTETTA_ALI 1.5
#define TUNTUMASSA_ASTETTA_YLI 0.5
#define VAJAAT_ASTETTA_LIMIT 2.0
#define VAHAN_YLI_ASTETTA_LOW_TEMP_LIMIT 3.0
#define VAHAN_YLI_ASTETTA_HIGH_TEMP_LIMIT 2.0


#define ABOUT_THE_SAME_PHRASE "suunnilleen sama"
#define SOMEWHAT_HIGHER_PHRASE "hieman korkeampi"
#define SOMEWHAT_LOWER_PHRASE "hieman alempi"
#define WARMING_UP_PHRASE "lämpötila nousee"
#define AROUND_ZERO_PHRASE "nollan tienoilla"
#define MINOR_PLUS_PHRASE "vähän plussan puolella"
#define MINOR_FROST_PHRASE "pikkupakkasta"
#define INLAND_PHRASE "sisämaassa"
#define COAST_PHRASE "rannikolla"
#define MORNING_PHRASE "aamulla"
#define AFTERNOON_PHRASE "iltapäivällä"
#define DAYTIME_PHRASE "päivällä"
#define NIGHTTIME_PHRASE "yöllä"
#define PLAIN_TEMPERATURE_PHRASE "lämpötila"
#define DAYTIME_HIGHEST_TEMPERATURE_PHRASE "päivän ylin lämpötila"
#define NIGHTTIME_LOWEST_TEMPERATURE_PHRASE "yön alin lämpötila"
#define NIGHTTIME_TEMPERATURE_PHRASE "yölämpötila"
#define DAYTIME_TEMPERATURE_PHRASE "päivälämpötila"
#define FROST_WORD "pakkasta"
#define IS_WORD "on"
#define NOIN_PHRASE "noin"
#define ASTEEN_PHRASE "asteen"
#define PAIKKEILLA_PHRASE "paikkeilla"
#define TIENOILLA_PHRASE "tienoilla"
#define LAHELLA_PHRASE "lähellä"
#define TUNTUMASSA_PHRASE "tuntumassa"
#define VAJAAT_PHRASE "vajaat"
#define VAHAN_PHRASE "vähän"
#define YLI_PHRASE "yli"
#define TAI_PHRASE "tai"

	enum proximity_id{NOIN_ASTETTA,
					  PAIKKEILLA_ASTETTA,
					  TIENOILLA_ASTETTA,
					  LAHELLA_ASTETTA,
					  TUNTUMASSA_ASTETTA,
					  VAJAAT_ASTETTA,
					  VAHAN_YLI_ASTETTA,
					  NO_PROXIMITY};

	enum temperature_phrase_id{MINOR_PLUS,
							   AROUND_ZERO_OR_MINOR_PLUS,
							   ABOUT_THE_SAME,
							   SOMEWHAT_HIGHER,
							   SOMEWHAT_LOWER,
							   WARMING_UP,
							   MINOR_FROST,
							   AROUND_ZERO,
							   TEMPERATURE_RANGE,
							   NO_PHRASE};

	enum forecast_period_id{DAY1_PERIOD = 0x1, 
							NIGHT_PERIOD = 0x2,
							DAY2_PERIOD = 0x4,
							NO_PERIOD = 0x0};

	enum forecast_subperiod_id{DAY1_MORNING_PERIOD = 0x1,
							   DAY1_AFTERNOON_PERIOD = 0x2,
							   DAY2_MORNING_PERIOD = 0x4,
							   DAY2_AFTERNOON_PERIOD = 0x8,
							   UNDEFINED_SUBPERIOD = 0x0};

	enum processing_order{DAY1,
						  DAY1_DAY2_NIGHT,
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
				 const unsigned short& forecastArea,
				 const unsigned short& forecastPeriod,
				 const NFmiTime& forecastTime,
				 WeatherPeriod& weatherPeriod,
				 const WeatherArea& weatherArea,
				 const AnalysisSources& analysisSources,
				 weather_result_container_type& weatherResults):
		theVariable(variable),
		theLog(log),
		theGenerator(generator),
		theSeasonId(seasonId),
		theForecastArea(forecastArea),
		theForecastPeriod(forecastPeriod),
		theForecastTime(forecastTime),
		theWeatherPeriod(weatherPeriod),
		theWeatherArea(weatherArea),
		theAnalysisSources(analysisSources),
		theWeatherResults(weatherResults),
		theForecastAreaId(NO_AREA),
		theForecastPeriodId(NO_PERIOD),
		theSubPeriodId(UNDEFINED_SUBPERIOD),
		theForecastAreaDay1(0x0),
		theForecastAreaNight(0x0),
		theForecastAreaDay2(0x0),
		theForecastSubPeriod(0x0),
		theMaxTemperatureDay1(kFloatMissing),
		theMeanTemperatureDay1(kFloatMissing),
		theMinimum(kFloatMissing),
		theMaximum(kFloatMissing),
		theMean(kFloatMissing),
		theNightPeriodTautologyFlag(false),
		theDayPeriodTautologyFlag(false),
		theTomorrowTautologyFlag(false),
		theOnCoastalAreaTautologyFlag(false),
		theOnInlandAreaTautologyFlag(false),
		theRangeSeparator("..."),
		theMinInterval(2),
		theZeroIntervalFlag(false),
		theTemperaturePhraseId(NO_PHRASE),
		theDayAndNightSeparationFlag(true),
		theUseFrostExistsPhrase(false)
	  {}

	  const string& theVariable;
	  MessageLogger& theLog;
	  const NightAndDayPeriodGenerator& theGenerator;
	  forecast_season_id& theSeasonId;
	  const unsigned short& theForecastArea;
	  const unsigned short& theForecastPeriod;
	  const NFmiTime& theForecastTime;
	  WeatherPeriod& theWeatherPeriod;
	  const WeatherArea& theWeatherArea;
	  const AnalysisSources& theAnalysisSources;
	  weather_result_container_type& theWeatherResults;
	  forecast_area_id theForecastAreaId;
	  forecast_period_id theForecastPeriodId;
	  forecast_subperiod_id theSubPeriodId;
	  unsigned short theForecastAreaDay1;
	  unsigned short theForecastAreaNight;
	  unsigned short theForecastAreaDay2;
	  unsigned short theForecastSubPeriod;
	  double theMaxTemperatureDay1;
	  double theMeanTemperatureDay1;
	  double theMinimum;
	  double theMaximum;
	  double theMean;
	  bool theNightPeriodTautologyFlag;
	  bool theDayPeriodTautologyFlag;
	  bool theTomorrowTautologyFlag;
	  bool theOnCoastalAreaTautologyFlag;
	  bool theOnInlandAreaTautologyFlag;
	  string theRangeSeparator;
	  int theMinInterval;
	  bool theZeroIntervalFlag;
	  temperature_phrase_id theTemperaturePhraseId;
	  bool theDayAndNightSeparationFlag;
	  Paragraph theOptionalFrostParagraph;
	  bool theUseFrostExistsPhrase;

	  bool morningAndAfternoonSeparated(const forecast_period_id& forecastPeriodId = NO_PERIOD)
	  {
		if(forecastPeriodId == NO_PERIOD)
		  {
			if(theForecastPeriodId == DAY1_PERIOD)
			  return theForecastSubPeriod & DAY1_MORNING_PERIOD;
			else if(theForecastPeriodId == NIGHT_PERIOD)
			  return false;
			else if(theForecastPeriodId == DAY2_PERIOD)
			  return theForecastSubPeriod & DAY2_MORNING_PERIOD;
		  }
		else if(forecastPeriodId == DAY1_PERIOD)
		  {
			return theForecastSubPeriod & DAY1_MORNING_PERIOD;
		  }
		else if(forecastPeriodId == NIGHT_PERIOD)
		  {
			return false;
		  }
		else if(forecastPeriodId == DAY2_PERIOD)
		  {
			return theForecastSubPeriod & DAY2_MORNING_PERIOD;
		  }

		return false;
	  }

	  bool inlandAndCoastSeparated(const forecast_period_id& forecastPeriodId = NO_PERIOD)
	  {
		if(forecastPeriodId == NO_PERIOD)
		  {
			if(theForecastPeriodId == DAY1_PERIOD)
			  return theForecastAreaDay1 & COASTAL_AREA && theForecastAreaDay1 & INLAND_AREA; 
			else if(theForecastPeriodId == NIGHT_PERIOD)
			  return theForecastAreaNight & COASTAL_AREA && theForecastAreaNight & INLAND_AREA; 
			else if(theForecastPeriodId == DAY2_PERIOD)
			  return theForecastAreaDay2 & COASTAL_AREA && theForecastAreaDay2 & INLAND_AREA; 
		  }
		else if(forecastPeriodId == DAY1_PERIOD)
		  {
			return (theForecastAreaDay1 & COASTAL_AREA) && (theForecastAreaDay1 & INLAND_AREA); 
		  }
		else if(forecastPeriodId == NIGHT_PERIOD)
		  {
			return (theForecastAreaNight & COASTAL_AREA) && (theForecastAreaNight & INLAND_AREA); 
		  }
		else if(forecastPeriodId == DAY2_PERIOD)
		  {
			return theForecastAreaDay2 & COASTAL_AREA && theForecastAreaDay2 & INLAND_AREA; 
		  }

		return false;
	  }
	};

	std::string weather_result_string(const std::string& areaName, const weather_result_id& id, const bool& isWinter)
	{
	  std::string retval;
	  
	  std::string timeFunSummerDay("maximum");
	  std::string timeFunSummerNight("minimum");
	  std::string timeFunWinterDay("mean");
	  std::string timeFunWinterNight("mean");

	  switch(id)
		{
		case AREA_MIN_DAY1:
		  retval = areaName + " - area day1 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): "; 
		  break;
		case AREA_MAX_DAY1:
		  retval = areaName + " - area day1 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case AREA_MEAN_DAY1:
		  retval = areaName + " - area day1 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case AREA_MIN_NIGHT:
		  retval = areaName + " - area night " + (isWinter ? timeFunWinterNight : timeFunSummerNight) + "(minimum): "; 
		  break;
		case AREA_MAX_NIGHT:
		  retval = areaName + " - area night " + (isWinter ? timeFunWinterNight : timeFunSummerNight) + "(maximum): "; 
		  break;
		case AREA_MEAN_NIGHT:
		  retval = areaName + " - area night " + (isWinter ? timeFunWinterNight : timeFunSummerNight) + "(mean): "; 
		  break;
		case AREA_MIN_DAY2:
		  retval = areaName + " - area day2 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): ";
		  break;
		case AREA_MAX_DAY2:
		  retval = areaName + " - area day2 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case AREA_MEAN_DAY2:
		  retval = areaName + " - area day2 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case AREA_MIN_DAY1_MORNING:
		  retval = areaName + " - area day1 morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): ";
		  break;
		case AREA_MAX_DAY1_MORNING:
		  retval = areaName + " - area day1 morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case AREA_MEAN_DAY1_MORNING:
		  retval = areaName + " - area day1  morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case AREA_MIN_DAY2_MORNING:
		  retval = areaName + " - area day2  morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): ";
		  break;
		case AREA_MAX_DAY2_MORNING:
		  retval = areaName + " - area day2 morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case AREA_MEAN_DAY2_MORNING:
		  retval = areaName + " - area day2 morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case AREA_MIN_DAY1_AFTERNOON:
		  retval = areaName + " - area day1 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): ";
		  break;
		case AREA_MAX_DAY1_AFTERNOON:
		  retval = areaName + " - area day1 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case AREA_MEAN_DAY1_AFTERNOON:
		  retval = areaName + " - area day1 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case AREA_MIN_DAY2_AFTERNOON:
		  retval = areaName + " - area day2 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): ";
		  break;
		case AREA_MAX_DAY2_AFTERNOON:
		  retval = areaName + " - area day2 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case AREA_MEAN_DAY2_AFTERNOON:
		  retval = areaName + " - area day2 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case INLAND_MIN_DAY1:
		  retval = areaName + " - inland day1 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): ";
		  break;
		case INLAND_MAX_DAY1:
		  retval = areaName + " - inland day1 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case INLAND_MEAN_DAY1:
		  retval = areaName + " - inland day1 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case INLAND_MIN_NIGHT:
		  retval = areaName + " - inland night " + (isWinter ? timeFunWinterNight : timeFunSummerNight) + "(minimum): "; 
		  break;
		case INLAND_MAX_NIGHT:
		  retval = areaName + " - inland night " + (isWinter ? timeFunWinterNight : timeFunSummerNight) + "(maximum): "; 
		  break;
		case INLAND_MEAN_NIGHT:
		  retval = areaName + " - inland night " + (isWinter ? timeFunWinterNight : timeFunSummerNight) + "(mean): "; 
		  break;
		case INLAND_MIN_DAY2:
		  retval = areaName + " - inland day2 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): ";
		  break;
		case INLAND_MAX_DAY2:
		  retval = areaName + " - inland day2 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case INLAND_MEAN_DAY2:
		  retval = areaName + " - inland day2 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case INLAND_MIN_DAY1_MORNING:
		  retval = areaName + " - inland day1 morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): ";
		  break;
		case INLAND_MAX_DAY1_MORNING:
		  retval = areaName + " - inland day1 morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case INLAND_MEAN_DAY1_MORNING:
		  retval = areaName + " - inland day1  morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case INLAND_MIN_DAY2_MORNING:
		  retval = areaName + " - inland day2  morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): ";
		  break;
		case INLAND_MAX_DAY2_MORNING:
		  retval = areaName + " - inland day2 morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case INLAND_MEAN_DAY2_MORNING:
		  retval = areaName + " - inland day2 morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case INLAND_MIN_DAY1_AFTERNOON:
		  retval = areaName + " - inland day1 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): ";
		  break;
		case INLAND_MAX_DAY1_AFTERNOON:
		  retval = areaName + " - inland day1 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case INLAND_MEAN_DAY1_AFTERNOON:
		  retval = areaName + " - inland day1 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case INLAND_MIN_DAY2_AFTERNOON:
		  retval = areaName + " - inland day2 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): ";
		  break;
		case INLAND_MAX_DAY2_AFTERNOON:
		  retval = areaName + " - inland day2 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case INLAND_MEAN_DAY2_AFTERNOON:
		  retval = areaName + " - inland day2 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case COAST_MIN_DAY1:
		  retval = areaName + " - coast day1 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): "; 
		  break;
		case COAST_MAX_DAY1:
		  retval = areaName + " - coast day1 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case COAST_MEAN_DAY1:
		  retval = areaName + " - coast day1 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case COAST_MIN_NIGHT:
		  retval = areaName + " - coast night " + (isWinter ? timeFunWinterNight : timeFunSummerNight) + "(minimum): ";
		  break;
		case COAST_MAX_NIGHT:
		  retval = areaName + " - coast night " + (isWinter ? timeFunWinterNight : timeFunSummerNight) + "(maximum): ";
		  break;
		case COAST_MEAN_NIGHT:
		  retval = areaName + " - coast night " + (isWinter ? timeFunWinterNight : timeFunSummerNight) + "(mean): ";
		  break;
		case COAST_MIN_DAY2:
		  retval = areaName + " - coast day2 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): ";
		  break;
		case COAST_MAX_DAY2:
		  retval = areaName + " - coast day2 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case COAST_MEAN_DAY2:
		  retval = areaName + " - coast day2 " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case COAST_MIN_DAY1_MORNING:
		  retval = areaName + " - coast day1 morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): ";
		  break;
		case COAST_MAX_DAY1_MORNING:
		  retval = areaName + " - coast day1 morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case COAST_MEAN_DAY1_MORNING:
		  retval = areaName + " - coast day1  morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case COAST_MIN_DAY2_MORNING:
		  retval = areaName + " - coast day2  morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): ";
		  break;
		case COAST_MAX_DAY2_MORNING:
		  retval = areaName + " - coast day2 morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case COAST_MEAN_DAY2_MORNING:
		  retval = areaName + " - coast day2 morning " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case COAST_MIN_DAY1_AFTERNOON:
		  retval = areaName + " - coast day1 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): ";
		  break;
		case COAST_MAX_DAY1_AFTERNOON:
		  retval = areaName + " - coast day1 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case COAST_MEAN_DAY1_AFTERNOON:
		  retval = areaName + " - coast day1 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case COAST_MIN_DAY2_AFTERNOON:
		  retval = areaName + " - coast day2 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(minimum): ";
		  break;
		case COAST_MAX_DAY2_AFTERNOON:
		  retval = areaName + " - coast day2 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(maximum): ";
		  break;
		case COAST_MEAN_DAY2_AFTERNOON:
		  retval = areaName + " - coast day2 afternoon " + (isWinter ? timeFunWinterDay : timeFunSummerDay) + "(mean): ";
		  break;
		case UNDEFINED_WEATHER_RESULT_ID:
		  break;
		}

	  return retval;
	}

	void construct_optional_frost_story(t36hparams& theParameters)
	{
	  if(Settings::isset(theParameters.theVariable + "::frost_story"))
		{
		  const string frost_function = require_string(theParameters.theVariable + "::frost_story");

		  std::string theVariable = theParameters.theVariable.substr(0, theParameters.theVariable.find("temperature_max36hours"));
		  theVariable.append(frost_function);

		  FrostStory story(theParameters.theForecastTime,
						   theParameters.theAnalysisSources,
						   theParameters.theWeatherArea, 
						   theParameters.theWeatherPeriod,
						   theVariable);

		  theParameters.theOptionalFrostParagraph = story.makeStory("frost_onenight");
		}
	}


	proximity_id get_proximity_id(const float& theMinimum, const float& theMean, 
								  const float& theMaximum, int& theProximityNumber)
	{	  
	  float range = abs(theMaximum - theMinimum);

	  // if minimum and maximum differ more than 2.0 degrees we are not in the proximity of
	  // a number divisible by five
	  if(range >= 6.0 || (theMinimum == kFloatMissing || theMaximum == kFloatMissing))
		return NO_PROXIMITY;

	  float theMinimumCalc = theMinimum;
	  float theMeanCalc = theMean;
	  float theMaximumCalc = theMaximum;

	  proximity_id retval = NO_PROXIMITY;
	  bool bBelowZeroDegrees = (theMaximum < 0);

	  if(bBelowZeroDegrees)
		{
		  theMaximumCalc = abs(theMinimum);
		  theMeanCalc = abs(theMean);
		  theMinimumCalc = abs(theMaximum);
		}

	  int iMin = static_cast<int>(floor(theMinimumCalc));
	  int iMax = static_cast<int>(floor(theMaximumCalc));
	  int iDivFiveMin = iMin / 5;
	  int iDivFiveMax = iMax / 5;
	  int iMinModFive = iMin % 5;
	  int iMaxModFive = iMax % 5;
	  enum proximity_type{BELOW, BETWEEN, ABOVE, NONE};
	  enum proximity_type theCase = NONE;
	  int theNumberDivisibleByFive = iMin;
	  while(theNumberDivisibleByFive % 5 != 0)
		theNumberDivisibleByFive += 1;

	  if((theNumberDivisibleByFive < theMaximumCalc && theMinimumCalc < theNumberDivisibleByFive &&
		 theNumberDivisibleByFive - theMinimumCalc < TIENOILLA_ASTETTA_LOW_TEMP_LIMIT &&
		  theMaximumCalc - theNumberDivisibleByFive < TIENOILLA_ASTETTA_LOW_TEMP_LIMIT) ||
		 theMaximum == theMinimum == theNumberDivisibleByFive) 
		{
		  theCase = BETWEEN;
		}
	  else
		{
		  if((iMinModFive == 0 || iMinModFive == 1 || iMinModFive == 2) &&
			 (iMaxModFive == 0 || iMaxModFive == 1 || iMaxModFive == 2) && iDivFiveMin == iDivFiveMax)
			{
			  theCase = ABOVE;
			  theNumberDivisibleByFive -= 5;
			}
		  else if((iMinModFive == 3 || iMinModFive == 4) &&
				  (iMaxModFive == 3 || iMaxModFive == 4) && iDivFiveMin == iDivFiveMax)
			{
			  theCase = BELOW;
			}
		}


	  if(theCase == BETWEEN)
		{
		  float theMinDiff = theNumberDivisibleByFive - theMinimumCalc;
		  float theMaxDiff = theMaximumCalc - theNumberDivisibleByFive;

		  if(theMinDiff < TUNTUMASSA_ASTETTA_ALI && theMaxDiff < TUNTUMASSA_ASTETTA_YLI 
			 && theMean < theNumberDivisibleByFive && theMinDiff > theMaxDiff)//tuntumassa
			{
			  retval = TUNTUMASSA_ASTETTA;
			}
		  else if(theMinDiff < LAHELLA_ASTETTA_ALI && theMaxDiff < LAHELLA_ASTETTA_YLI 
			 && theMean < theNumberDivisibleByFive && theMinDiff > theMaxDiff)//tuntumassa
			{
			  retval = LAHELLA_ASTETTA;
			}
		  else
			{
			  float noinAstettaLimit = (theMaximumCalc * (bBelowZeroDegrees ? -1 : +1)) 
										< -10.0 ? NOIN_ASTETTA_LOW_TEMP_LIMIT : NOIN_ASTETTA_HIGH_TEMP_LIMIT;
			  float tienoillaAstettaLimit = (theMaximumCalc * (bBelowZeroDegrees ? -1 : +1)) 
				< -10.0 ? TIENOILLA_ASTETTA_LOW_TEMP_LIMIT : TIENOILLA_ASTETTA_HIGH_TEMP_LIMIT;

			  if(theMinDiff < noinAstettaLimit && theMaxDiff < noinAstettaLimit)
				{
				  retval = NOIN_ASTETTA;
				}
			  else if(theMinDiff < tienoillaAstettaLimit && theMaxDiff < tienoillaAstettaLimit)
				{
				  retval = TIENOILLA_ASTETTA;
				}
			}
		  if(retval != NO_PROXIMITY)
			theProximityNumber = theNumberDivisibleByFive;

		}
	  else if(theCase == ABOVE)
		  {
			// minimum and maximum are above the number
			// that is divisible by 5
			float theMinDiff = theMinimumCalc - theNumberDivisibleByFive;
			float theMaxDiff = theMaximumCalc - theNumberDivisibleByFive;

			float vahanYliAstettaLimit = (theMaximumCalc * (bBelowZeroDegrees ? -1 : +1)) 
										  < -10.0 ? VAHAN_YLI_ASTETTA_LOW_TEMP_LIMIT : VAHAN_YLI_ASTETTA_HIGH_TEMP_LIMIT;

			if(theMinDiff < vahanYliAstettaLimit && theMaxDiff < vahanYliAstettaLimit)
			  retval = VAHAN_YLI_ASTETTA;

			if(retval != NO_PROXIMITY)
			  theProximityNumber = theNumberDivisibleByFive;
		  }
	  else if(theCase == BELOW)
		  {
			// minimum and maximum are below the number
			// that is divisible by 5
			float theMinDiff = theNumberDivisibleByFive - theMinimumCalc;
			float theMaxDiff = theNumberDivisibleByFive - theMaximumCalc;

			if(theMinDiff < TUNTUMASSA_ASTETTA_ALI && theMaxDiff < TUNTUMASSA_ASTETTA_ALI)
			  retval = TUNTUMASSA_ASTETTA;
			/*
			  // lähellä and vajaat is the same if they are both below the number that is divisible by 5
			else if(theMinDiff < LAHELLA_ASTETTA_ALI && theMaxDiff < LAHELLA_ASTETTA_ALI)
			  retval = LAHELLA_ASTETTA;
			*/
			else if(theMinDiff < VAJAAT_ASTETTA_LIMIT && theMaxDiff < VAJAAT_ASTETTA_LIMIT)
			  retval = VAJAAT_ASTETTA;

			if(retval != NO_PROXIMITY)
			  theProximityNumber = theNumberDivisibleByFive;
		  }

	  if(bBelowZeroDegrees && retval != NO_PROXIMITY)
		theProximityNumber = theProximityNumber * -1;

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

		  fakeVarFull += "::day2";
		  fakeVarMorning += "::day2::morning";
		  fakeVarAfternoon += "::day2::afternoon";
		}
	  else
		{
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
		  WeatherFunction theTimeFunction = (thePeriodId == NIGHT_PERIOD ? Minimum : Maximum);

		  if(thePeriodId == NIGHT_PERIOD)
			{
			  minResultFull = do_calculation(theVar + fakeVarFull + "::min",
											 theSources,
											 Minimum,
											 theTimeFunction,
											 theActualArea,
											 thePeriod);

			  maxResultFull = do_calculation(theVar + fakeVarFull + "::max",
											 theSources,
											 Maximum,
											 theTimeFunction,
											 theActualArea,
											 thePeriod);

			  meanResultFull = do_calculation(theVar + fakeVarFull + "::mean",
											  theSources,
											  Mean,
											  theTimeFunction,
											  theActualArea,
											  thePeriod);
			}
		  else if(thePeriodId == DAY1_PERIOD || thePeriodId == DAY2_PERIOD)
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

			  afternoon_temperature(theVar + fakeVarFull,
									theSources,
									theActualArea,
									thePeriod,
									minResultFull,
									maxResultFull,
									meanResultFull);
			}
		}
	  else
		{
		  // In wintertime we calculate Mean temperature of areas Maximum, Minimum and Mean temperatures

		  if(thePeriodId == NIGHT_PERIOD)
			{
			  minResultFull = do_calculation(theVar + fakeVarFull + "::min",
											 theSources,
											 Minimum,
											 Mean,
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
			}
		  else if(thePeriodId == DAY1_PERIOD || thePeriodId == DAY2_PERIOD)
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

			  afternoon_temperature(theVar + fakeVarFull,
									theSources,
									theActualArea,
									thePeriod,
									minResultFull,
									maxResultFull,
									meanResultFull);
			}
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


	const void log_weather_results(const t36hparams& theParameters)
	{
	  theParameters.theLog << "Weather results: " << endl;

	  // Iterate and print out the WeatherResult variables
	  for(int i = AREA_MIN_DAY1; i < UNDEFINED_WEATHER_RESULT_ID; i++)
		{
		  const WeatherResult& theWeatherResult = *(theParameters.theWeatherResults[i]);

		  if(theWeatherResult.value() != kFloatMissing)
			{
			  theParameters.theLog << weather_result_string(theParameters.theWeatherArea.isNamed() ? theParameters.theWeatherArea.name() : 
															"", static_cast<weather_result_id>(i), 
															theParameters.theSeasonId == WINTER_SEASON);
			  theParameters.theLog << theWeatherResult << endl;
			}
		} 
	}

	const forecast_season_id get_forecast_season(const NFmiTime& theTime, const std::string theVariable)
	{
	  return (SeasonTools::isSummerHalf(theTime, theVariable) ? SUMMER_SEASON : WINTER_SEASON);
	}

	const temperature_phrase_id around_zero_phrase(const t36hparams& theParameters)
	{
	  temperature_phrase_id retval = NO_PHRASE;

	  if(theParameters.theMinimum > 0.0 && theParameters.theMaximum < 2.50 && 
		 !theParameters.theZeroIntervalFlag && 
		 theParameters.theTemperaturePhraseId != MINOR_PLUS) // [+0,1...+2,49]
		{
		  retval = MINOR_PLUS;
		}
	  else if(theParameters.theMinimum < 0 && theParameters.theMinimum > -0.50 && theParameters.theMaximum >= 0 && 
			  theParameters.theMaximum < 2.50 && !theParameters.theZeroIntervalFlag &&
			  theParameters.theTemperaturePhraseId != AROUND_ZERO_OR_MINOR_PLUS) // [-0,49...+2,49]
		{
		  retval = AROUND_ZERO_OR_MINOR_PLUS;
		}
	  else if(theParameters.theMinimum < 0 && theParameters.theMinimum >= -2.0 && theParameters.theMaximum >= 0 && 
			  theParameters.theMaximum <= 2.0 && !theParameters.theZeroIntervalFlag &&
			  theParameters.theTemperaturePhraseId != AROUND_ZERO) // [-2,0...+2,0]
		{
		  retval = AROUND_ZERO;
		}
	  else if(theParameters.theMinimum > -4.50 && theParameters.theMaximum < 0.0 && 
			  !theParameters.theZeroIntervalFlag && 
			  theParameters.theTemperaturePhraseId != MINOR_FROST) // [-4,49...-0,1]
		{
		  retval = MINOR_FROST;
		}

	  return retval;
	}

	const bool separate_day_and_night(const t36hparams& theParameters,
									  const forecast_area_id& theForecastAreaId)
	{
	  bool separateDayAndNight = true;

	  if(theParameters.theSeasonId == WINTER_SEASON && around_zero_phrase(theParameters) == NO_PHRASE)
		{
		  float dayTemperature, nightTemperature;

		  if(theForecastAreaId == FULL_AREA)
			{
			  if(theParameters.theForecastPeriod & DAY1_PERIOD && theParameters.theForecastPeriod & NIGHT_PERIOD)
				{
				  dayTemperature = theParameters.theWeatherResults[AREA_MEAN_DAY1]->value();
				  nightTemperature = theParameters.theWeatherResults[AREA_MEAN_NIGHT]->value();
				  
				  separateDayAndNight = abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
				}
			  else if(theParameters.theForecastPeriod & DAY2_PERIOD && theParameters.theForecastPeriod & NIGHT_PERIOD)
				{
				  dayTemperature = theParameters.theWeatherResults[AREA_MEAN_DAY2]->value();
				  nightTemperature = theParameters.theWeatherResults[AREA_MEAN_NIGHT]->value();

				  separateDayAndNight =  abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
				}
			}
		  else if(theForecastAreaId == INLAND_AREA )
			{
			  if(theParameters.theForecastPeriod & DAY1_PERIOD && theParameters.theForecastPeriod & NIGHT_PERIOD)
				{
				  dayTemperature = theParameters.theWeatherResults[INLAND_MEAN_DAY1]->value();
				  nightTemperature = theParameters.theWeatherResults[INLAND_MEAN_NIGHT]->value();

				  separateDayAndNight = abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
				}
			  else if(theParameters.theForecastPeriod & DAY2_PERIOD && theParameters.theForecastPeriod & NIGHT_PERIOD)
				{
				  dayTemperature = theParameters.theWeatherResults[INLAND_MEAN_DAY2]->value();
				  nightTemperature = theParameters.theWeatherResults[INLAND_MEAN_NIGHT]->value();

				  separateDayAndNight = abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
				}

			  if(separateDayAndNight == false)
				{
				  if(theParameters.theForecastPeriod & DAY1_PERIOD && theParameters.theForecastPeriod & NIGHT_PERIOD)
					{
					  dayTemperature = theParameters.theWeatherResults[COAST_MEAN_DAY1]->value();
					  nightTemperature = theParameters.theWeatherResults[COAST_MEAN_NIGHT]->value();

					  separateDayAndNight = abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
					}
				  else if(theParameters.theForecastPeriod & DAY2_PERIOD && theParameters.theForecastPeriod & NIGHT_PERIOD)
					{
					  dayTemperature = theParameters.theWeatherResults[COAST_MEAN_DAY2]->value();
					  nightTemperature = theParameters.theWeatherResults[COAST_MEAN_NIGHT]->value();

					  separateDayAndNight = abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
					}
				}
			}
		  else if(theForecastAreaId == COASTAL_AREA )
			{
			  if(theParameters.theForecastPeriod & DAY1_PERIOD && theParameters.theForecastPeriod & NIGHT_PERIOD)
				{
				  dayTemperature = theParameters.theWeatherResults[COAST_MEAN_DAY1]->value();
				  nightTemperature = theParameters.theWeatherResults[COAST_MEAN_NIGHT]->value();

				  separateDayAndNight = abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
				}
			  else if(theParameters.theForecastPeriod & DAY2_PERIOD && theParameters.theForecastPeriod & NIGHT_PERIOD)
				{
				  dayTemperature = theParameters.theWeatherResults[COAST_MEAN_DAY2]->value();
				  nightTemperature = theParameters.theWeatherResults[COAST_MEAN_NIGHT]->value();

				  separateDayAndNight = abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
				}

			  if(separateDayAndNight == false)
				{
				  if(theParameters.theForecastPeriod & DAY1_PERIOD && theParameters.theForecastPeriod & NIGHT_PERIOD)
					{
					  dayTemperature = theParameters.theWeatherResults[INLAND_MEAN_DAY1]->value();
					  nightTemperature = theParameters.theWeatherResults[INLAND_MEAN_NIGHT]->value();

					  separateDayAndNight = abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
					}
				  else if(theParameters.theForecastPeriod & DAY2_PERIOD && theParameters.theForecastPeriod & NIGHT_PERIOD)
					{
					  dayTemperature = theParameters.theWeatherResults[INLAND_MEAN_DAY2]->value();
					  nightTemperature = theParameters.theWeatherResults[INLAND_MEAN_NIGHT]->value();

					  separateDayAndNight = abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
					}
				}
			}
		}
	  return separateDayAndNight;

	}

	const Sentence temperature_sentence(t36hparams& theParameters)
	{

	  Sentence sentence;

	  temperature_phrase_id phrase_id = around_zero_phrase(theParameters);

	  if(phrase_id != NO_PHRASE)
		{
		  switch(phrase_id)
			{
			case MINOR_PLUS:
			  {
				sentence << MINOR_PLUS_PHRASE;
				theParameters.theTemperaturePhraseId = MINOR_PLUS;
			  }
			  break;
			case AROUND_ZERO_OR_MINOR_PLUS:
			  {
				sentence << AROUND_ZERO_PHRASE << TAI_PHRASE << MINOR_PLUS_PHRASE;
				theParameters.theTemperaturePhraseId = AROUND_ZERO_OR_MINOR_PLUS;
			  }
			  break;
			case AROUND_ZERO:
			  {
				sentence << AROUND_ZERO_PHRASE;
				theParameters.theTemperaturePhraseId = AROUND_ZERO;
			  }
			  break;
			case MINOR_FROST:
			  {
				NFmiTime startTime(theParameters.theGenerator.period(1).localStartTime());
				if(SeasonTools::isSpring(startTime, theParameters.theVariable) ||
				   abs(theParameters.theMaximum - theParameters.theMinimum) < AROUND_ZERO_UPPER_LIMIT)
				  {
					if(theParameters.theMinimum >= -1.0) // maximum and minimum between [-1,0]
					  {
						sentence << VAJAAT_PHRASE
								 << Integer(0)
								 << *UnitFactory::create(DegreesCelsius);

					  }
					else
					  {
						int theMinimumInt = static_cast<int>(round(theParameters.theMinimum));
						int theMeanInt = static_cast<int>(round(theParameters.theMean));
						int theMaximumInt = static_cast<int>(round(theParameters.theMaximum));

						sentence = TemperatureStoryTools::temperature_sentence2(theMinimumInt, 
																				theMeanInt, 
																				theMaximumInt,
																				theParameters.theMinInterval,
																				theParameters.theZeroIntervalFlag,
																				theParameters.theRangeSeparator,
																				false);
					  }
				  }
				else
				  {
					sentence << MINOR_FROST_PHRASE;
					theParameters.theTemperaturePhraseId = MINOR_FROST;
				  }
			  }
			  break;
			default:
			  break;
			}
		}
	  else
		{	
		  int theProximityNumber;

		  proximity_id proxim_id = get_proximity_id(theParameters.theMinimum, 
													theParameters.theMean,
													theParameters.theMaximum, 
													theProximityNumber);

		  if(proxim_id != NO_PROXIMITY)
			{
			  char proximityNumberBuff[32];
			  char tempBuff[128];

			  sprintf(tempBuff, "Minimum: %.02f;Mean: %.02f;Maximum: %.02f", 
					  theParameters.theMinimum, 
					  theParameters.theMean, 
					  theParameters.theMaximum);
	
			  sprintf(proximityNumberBuff, "%i", theProximityNumber);

			  switch(proxim_id)
				{
				case NOIN_ASTETTA:
				  {
					sentence << NOIN_PHRASE
							 << Integer(theProximityNumber)
							 << *UnitFactory::create(DegreesCelsius);
					theParameters.theLog << "PROXIMITY: Noin " << proximityNumberBuff 
										 << " astetta :: " << tempBuff << endl;
				  }
				  break;
				case PAIKKEILLA_ASTETTA:
				  {
					sentence << Integer(theProximityNumber)
							 << ASTEEN_PHRASE << PAIKKEILLA_PHRASE;
					theParameters.theLog << "PROXIMITY: " << proximityNumberBuff 
										 << " asteen paikkeilla :: " << tempBuff << endl;
				  }
				  break;
				case TIENOILLA_ASTETTA:
				  {
					sentence << Integer(theProximityNumber)
							 << ASTEEN_PHRASE << TIENOILLA_PHRASE;
					theParameters.theLog << "PROXIMITY: " << proximityNumberBuff 
										 << " asteen tienoilla :: " << tempBuff << endl;
				  }
				  break;
				case LAHELLA_ASTETTA:
				  {
					sentence << LAHELLA_PHRASE
							 << Integer(theProximityNumber)
							 << *UnitFactory::create(DegreesCelsius);
					theParameters.theLog << "PROXIMITY: Lähellä " << proximityNumberBuff 
										 << " astetta :: " << tempBuff << endl;
				  }
				  break;
				case TUNTUMASSA_ASTETTA:
				  {
					sentence << Integer(theProximityNumber)
							 << ASTEEN_PHRASE << TUNTUMASSA_PHRASE;
					theParameters.theLog << "PROXIMITY: " << proximityNumberBuff 
										 << " asteen tuntumassa :: " << tempBuff << endl;
				  }
				  break;
				case VAJAAT_ASTETTA:
				  {
					sentence << VAJAAT_PHRASE
							 << Integer(theProximityNumber)
							 << *UnitFactory::create(DegreesCelsius);
					theParameters.theLog << "PROXIMITY: Vajaat " << proximityNumberBuff 
										 << " astetta :: " << tempBuff << endl;
					if(theParameters.theMaximum < 0)
					  theParameters.theUseFrostExistsPhrase = true;
				  }
				  break;
				case VAHAN_YLI_ASTETTA:
				  {
					sentence << VAHAN_PHRASE << YLI_PHRASE  
							 << Integer(theProximityNumber)
							 << *UnitFactory::create(DegreesCelsius);
					theParameters.theLog << "PROXIMITY: Vähän yli " << proximityNumberBuff 
										 << " astetta :: " << tempBuff << endl;
					if(theParameters.theMaximum < 0)
					  theParameters.theUseFrostExistsPhrase = true;
				  }
				  break;
				case NO_PROXIMITY:
				  theParameters.theLog << "NO PROXIMITY: " << tempBuff << endl;
				  break;
				}
			}
		  else
			{
			  int theMinimumInt = static_cast<int>(round(theParameters.theMinimum));
			  int theMeanInt = static_cast<int>(round(theParameters.theMean));
			  int theMaximumInt = static_cast<int>(round(theParameters.theMaximum));

			  clamp_temperature(theParameters.theVariable,
								theParameters.theSeasonId == WINTER_SEASON,
								theParameters.theForecastPeriodId != NIGHT_PERIOD,
								theMinimumInt < theMaximumInt ? theMinimumInt : theMaximumInt,
								theMaximumInt > theMinimumInt ? theMaximumInt : theMinimumInt);

			  sentence = TemperatureStoryTools::temperature_sentence2(theMinimumInt, 
																	  theMeanInt, 
																	  theMaximumInt,
																	  theParameters.theMinInterval,
																	  theParameters.theZeroIntervalFlag,
																	  theParameters.theRangeSeparator,
																	  true);

			}
		  theParameters.theTemperaturePhraseId = TEMPERATURE_RANGE;
		}

	  return sentence;
	}

	const Sentence temperature_phrase(t36hparams& theParameters)
	{
	  Sentence sentence;

	  bool useDayTemperaturePhrase = (theParameters.theForecastPeriodId == DAY1_PERIOD || 
									  (theParameters.theForecastPeriodId == DAY2_PERIOD && 
									   !(theParameters.theForecastPeriod & DAY1_PERIOD) ||
									   theParameters.inlandAndCoastSeparated(DAY1_PERIOD)));

	  Sentence theDayPhasePhrase;
	  Sentence theAreaPhrase;


	  bool plainIsVerbUsed = false;
	  if(theParameters.theSeasonId == SUMMER_SEASON)
		{
		  if(theParameters.theForecastPeriodId == NIGHT_PERIOD && !theParameters.theNightPeriodTautologyFlag)
			{
			  if(theParameters.theTemperaturePhraseId == MINOR_FROST)
				{
				  theDayPhasePhrase << NIGHTTIME_PHRASE << IS_WORD;
				}
			  else
				{
				  theDayPhasePhrase << NIGHTTIME_LOWEST_TEMPERATURE_PHRASE << IS_WORD;
				  theParameters.theNightPeriodTautologyFlag = true;
				}
			  theParameters.theDayPeriodTautologyFlag = false;
			}
		  else if(theParameters.theForecastPeriodId != NIGHT_PERIOD)
			{
			  theParameters.theNightPeriodTautologyFlag = false;

			  if((theParameters.theSubPeriodId == UNDEFINED_SUBPERIOD && 
				  useDayTemperaturePhrase && 
				  !theParameters.theDayPeriodTautologyFlag) || 
				 (theParameters.theForecastPeriodId == DAY2_PERIOD &&
				  (theParameters.theForecastAreaId == INLAND_AREA ||
				   theParameters.theForecastAreaId == FULL_AREA) &&
				  ((theParameters.inlandAndCoastSeparated(DAY2_PERIOD) && 
					theParameters.theForecastAreaId == INLAND_AREA) || 
				   theParameters.morningAndAfternoonSeparated(DAY1_PERIOD))))
				{
				  if(theParameters.theTemperaturePhraseId == MINOR_FROST)
					{
					  theDayPhasePhrase << IS_WORD;
					  plainIsVerbUsed = true;
					}
				  else
					{
					  theDayPhasePhrase << DAYTIME_HIGHEST_TEMPERATURE_PHRASE << IS_WORD;
					  theParameters.theDayPeriodTautologyFlag = true;
					}
				}
				else
				  {
					theParameters.theDayPeriodTautologyFlag = false;

					if(theParameters.theSubPeriodId == DAY1_MORNING_PERIOD || 
					   theParameters.theSubPeriodId == DAY2_MORNING_PERIOD)
					  {
						if(theParameters.theTemperaturePhraseId == MINOR_FROST)
						  {
							theDayPhasePhrase << MORNING_PHRASE << IS_WORD;
						  }
						else
						  {
							theDayPhasePhrase << PLAIN_TEMPERATURE_PHRASE  << IS_WORD << MORNING_PHRASE;
						  }
					  }
					else if(theParameters.theSubPeriodId == DAY1_AFTERNOON_PERIOD || 
							theParameters.theSubPeriodId == DAY2_AFTERNOON_PERIOD)
					  {
						if(theParameters.theTemperaturePhraseId == MINOR_FROST)
						  {
							theDayPhasePhrase << AFTERNOON_PHRASE << IS_WORD;
						  }
						else
						  {
							theDayPhasePhrase << AFTERNOON_PHRASE;
						  }
					  }
				  }
			}
		}
	  else
		{
		  if(theParameters.theForecastPeriodId == NIGHT_PERIOD && !theParameters.theNightPeriodTautologyFlag)
			{
			  if(theParameters.theTemperaturePhraseId == MINOR_FROST)
					{
					  theDayPhasePhrase << NIGHTTIME_PHRASE << IS_WORD;
					}
				  else
					{
					  Sentence tonightSentence;
					  tonightSentence << PeriodPhraseFactory::create("tonight",
											  theParameters.theVariable,
											  theParameters.theForecastTime,
											  theParameters.theWeatherPeriod);

					  if(tonightSentence.size() == 0)
						theDayPhasePhrase << NIGHTTIME_TEMPERATURE_PHRASE << IS_WORD;
					  else
						theDayPhasePhrase << PLAIN_TEMPERATURE_PHRASE << IS_WORD;
						
					  theParameters.theNightPeriodTautologyFlag = true;
					}
			  theParameters.theDayPeriodTautologyFlag = false;
			}
		  else if(theParameters.theForecastPeriodId != NIGHT_PERIOD)
			{
			  theParameters.theNightPeriodTautologyFlag = false;

			  if(theParameters.theSubPeriodId == UNDEFINED_SUBPERIOD &&
				 (!theParameters.theDayPeriodTautologyFlag ||
				  (theParameters.inlandAndCoastSeparated(DAY2_PERIOD) && 
				   theParameters.theForecastPeriodId == DAY2_PERIOD &&
				   theParameters.theForecastAreaId == INLAND_AREA 
				   )))
				{
				  if(theParameters.theTemperaturePhraseId == MINOR_FROST)
					{
					  theDayPhasePhrase << IS_WORD;
					  plainIsVerbUsed = true;
					}
				  else
					{
					  if(theParameters.theDayAndNightSeparationFlag)
						theDayPhasePhrase << DAYTIME_TEMPERATURE_PHRASE << IS_WORD;
					  else
						theDayPhasePhrase << PLAIN_TEMPERATURE_PHRASE << IS_WORD;

					  theParameters.theDayPeriodTautologyFlag = true;
					}
				}
			}
		}

	  if(theParameters.theForecastAreaId == COASTAL_AREA &&
		 theParameters.inlandAndCoastSeparated() && 
		 !theParameters.theOnCoastalAreaTautologyFlag)
		{
		  theAreaPhrase << COAST_PHRASE;
		  theParameters.theOnCoastalAreaTautologyFlag = true;
		  theParameters.theOnInlandAreaTautologyFlag = false;
		}
	  else if(theParameters.theForecastAreaId == INLAND_AREA && 
			  theParameters.inlandAndCoastSeparated() &&
			 !theParameters.theOnInlandAreaTautologyFlag)
		{
		  theAreaPhrase << INLAND_PHRASE;
		  theParameters.theOnInlandAreaTautologyFlag = true;
		  theParameters.theOnCoastalAreaTautologyFlag = false;
		}		

	  if(theParameters.theForecastAreaId == COASTAL_AREA || plainIsVerbUsed)
		{
		  sentence << theAreaPhrase;
		  if(theParameters.theUseFrostExistsPhrase)
			{
			  if(theParameters.theForecastPeriodId == NIGHT_PERIOD)
				sentence << NIGHTTIME_PHRASE;
			  sentence << FROST_WORD << IS_WORD;
			  theParameters.theUseFrostExistsPhrase = false;
			}
		  else
			{
			  sentence << theDayPhasePhrase;
			}
		}
	  else
		{
		  if(theParameters.theUseFrostExistsPhrase)
			{
			  if(theParameters.theForecastPeriodId == NIGHT_PERIOD)
				sentence << NIGHTTIME_PHRASE;
			  sentence << FROST_WORD << IS_WORD;
			  theParameters.theUseFrostExistsPhrase = false;
			}
		  else
			{
			  sentence << theDayPhasePhrase;
			}
		  sentence << theAreaPhrase;
		}

	  return sentence;
	}

	const Sentence night_sentence(t36hparams& theParameters)
	{
	  Sentence sentence;

	  double temperatureDifference = 100.0;
	  bool day1PeriodIncluded = theParameters.theMaxTemperatureDay1 != kFloatMissing;

	  if(day1PeriodIncluded)
		{
		  temperatureDifference = theParameters.theMean - theParameters.theMeanTemperatureDay1;
		}

	  // If day2 is included, we have to use numbers to describe temperature, since
	  // day2 story is told before night story
	  bool nightlyMinHigherThanDailyMax = !(theParameters.theForecastPeriod & DAY2_PERIOD) && 
		(theParameters.theMaxTemperatureDay1 - theParameters.theMinimum < 0);
	  bool smallChangeBetweenDay1AndNight = !(theParameters.theForecastPeriod & DAY2_PERIOD) && 
		(day1PeriodIncluded && abs(temperatureDifference) <= ABOUT_THE_SAME_UPPER_LIMIT);
	  bool moderateChangeBetweenDay1AndNight = !(theParameters.theForecastPeriod & DAY2_PERIOD) && 
		(day1PeriodIncluded && abs(temperatureDifference) > ABOUT_THE_SAME_UPPER_LIMIT && 
		 abs(temperatureDifference) <= SMALL_CHANGE_UPPER_LIMIT);

	  theParameters.theForecastPeriodId = NIGHT_PERIOD;
	  theParameters.theSubPeriodId = UNDEFINED_SUBPERIOD;

	  Sentence temperatureSentence;

	  bool inlandAndCoastSeparately = false;
	  if(theParameters.theForecastPeriod & DAY1_PERIOD)
		inlandAndCoastSeparately = theParameters.inlandAndCoastSeparated(DAY1_PERIOD) || 
		  theParameters.morningAndAfternoonSeparated(DAY1_PERIOD);

	  if(!inlandAndCoastSeparately)
		inlandAndCoastSeparately = theParameters.inlandAndCoastSeparated(NIGHT_PERIOD);

	  if(around_zero_phrase(theParameters) == NO_PHRASE && !inlandAndCoastSeparately)
		{
		  if(smallChangeBetweenDay1AndNight) // no change or small change
			{
			  temperatureSentence << ABOUT_THE_SAME_PHRASE;
			  theParameters.theTemperaturePhraseId = ABOUT_THE_SAME;
			}
		  else if(moderateChangeBetweenDay1AndNight && 
				  theParameters.theTemperaturePhraseId != AROUND_ZERO_OR_MINOR_PLUS) // moderate change
			{		  
			  if(temperatureDifference > 0)
				{
				  temperatureSentence << SOMEWHAT_HIGHER_PHRASE;
				  theParameters.theTemperaturePhraseId = SOMEWHAT_HIGHER;
				}
			  else
				{
				  temperatureSentence << SOMEWHAT_LOWER_PHRASE;
				  theParameters.theTemperaturePhraseId = SOMEWHAT_LOWER;
				}
			}
		  else if(nightlyMinHigherThanDailyMax)
			{
			  temperatureSentence << WARMING_UP_PHRASE;
			  theParameters.theTemperaturePhraseId = WARMING_UP;
			}
		  else
			{
			  temperatureSentence << temperature_sentence(theParameters);
			}
		}
	  else
		{
		  temperatureSentence << temperature_sentence(theParameters);
		}

	  sentence << PeriodPhraseFactory::create("tonight",
											  theParameters.theVariable,
											  theParameters.theForecastTime,
											  theParameters.theWeatherPeriod);
	  
	  sentence << temperature_phrase(theParameters);

	  sentence << temperatureSentence;

	  theParameters.theTomorrowTautologyFlag = false;

	  return sentence;
	}

	const Sentence day2_sentence(t36hparams& theParameters)
	{
	  Sentence sentence;

	  double temperatureDifference = 100.0;
	  bool day1PeriodIncluded = theParameters.theForecastPeriod & DAY1_PERIOD;

	  if(day1PeriodIncluded)
		{
			temperatureDifference =  theParameters.theMean - theParameters.theMeanTemperatureDay1;
		}

	  bool smallChangeBetweenDay1AndDay2 = day1PeriodIncluded && 
		abs(temperatureDifference) <= ABOUT_THE_SAME_UPPER_LIMIT;
	  bool moderateChangeBetweenDay1AndDay2 = day1PeriodIncluded && 
		abs(temperatureDifference) > ABOUT_THE_SAME_UPPER_LIMIT && 
		abs(temperatureDifference) <= SMALL_CHANGE_UPPER_LIMIT;

	  theParameters.theForecastPeriodId = DAY2_PERIOD;

	  Sentence temperatureSentence;

	  if(around_zero_phrase(theParameters) == NO_PHRASE && 
		 !theParameters.inlandAndCoastSeparated(DAY1_PERIOD) && 
		 !theParameters.inlandAndCoastSeparated(DAY2_PERIOD) && 
		 !theParameters.morningAndAfternoonSeparated(DAY1_PERIOD))
		{
		  if(smallChangeBetweenDay1AndDay2 && theParameters.theSubPeriodId != DAY2_MORNING_PERIOD && 
			 theParameters.theSubPeriodId != DAY2_AFTERNOON_PERIOD)
			{
			  temperatureSentence << ABOUT_THE_SAME_PHRASE;
			  theParameters.theTemperaturePhraseId = ABOUT_THE_SAME;

			}
		  else if(moderateChangeBetweenDay1AndDay2 && theParameters.theSubPeriodId != DAY2_MORNING_PERIOD &&
				  theParameters.theSubPeriodId != DAY2_AFTERNOON_PERIOD && 
				  theParameters.theTemperaturePhraseId != AROUND_ZERO_OR_MINOR_PLUS)
			{
			  // small change
			  if(temperatureDifference > 0)
				{
				  temperatureSentence << SOMEWHAT_HIGHER_PHRASE;
				  theParameters.theTemperaturePhraseId = SOMEWHAT_HIGHER;
				}
			  else
				{
				  temperatureSentence << SOMEWHAT_LOWER_PHRASE;
				  theParameters.theTemperaturePhraseId = SOMEWHAT_LOWER;
				}
			}
		  else
			{
			  temperatureSentence << temperature_sentence(theParameters);
			}
		}
	  else
		{
		  temperatureSentence << temperature_sentence(theParameters);
		}
	  
  	  Sentence nextDaySentence;

	  // exception: inland and coastal separated and morning and afternoon separated and now we are processing
	  // morning on the coastal area
	  if(!theParameters.theTomorrowTautologyFlag &&
		 !(theParameters.inlandAndCoastSeparated(DAY2_PERIOD) &&
		 theParameters.morningAndAfternoonSeparated(DAY2_PERIOD) &&
		  theParameters.theSubPeriodId == DAY2_MORNING_PERIOD && 
		   theParameters.theForecastAreaId == COASTAL_AREA))
		nextDaySentence << PeriodPhraseFactory::create("next_day",
													   theParameters.theVariable,
													   theParameters.theForecastTime,
													   theParameters.theWeatherPeriod);
	  
	  if(!nextDaySentence.empty())
		theParameters.theTomorrowTautologyFlag = true;

	  sentence << nextDaySentence;
	  sentence << temperature_phrase(theParameters);
	  sentence << temperatureSentence;

	  return sentence;

	}

	const Sentence day1_sentence(t36hparams& theParameters)
	{
	  Sentence sentence;

	  theParameters.theForecastPeriodId = DAY1_PERIOD;

	  Sentence temperatureSentence;

	  temperatureSentence << temperature_sentence(theParameters);

	  sentence << PeriodPhraseFactory::create("today",
											  theParameters.theVariable,
											  theParameters.theForecastTime,
											  theParameters.theWeatherPeriod);

	  sentence << temperature_phrase(theParameters);
	  sentence << temperatureSentence;

	  theParameters.theTomorrowTautologyFlag = false;

	  return sentence;
	}

	const Sentence construct_sentence(t36hparams& theParameters)
	{
	  Sentence sentence;

	  int areaMin = (theParameters.theForecastPeriodId == DAY1_PERIOD ? AREA_MIN_DAY1 : 
					 (theParameters.theForecastPeriodId == NIGHT_PERIOD ? AREA_MIN_NIGHT : AREA_MIN_DAY2));
	  int areaMax = (theParameters.theForecastPeriodId == DAY1_PERIOD ? AREA_MAX_DAY1 : 
					 (theParameters.theForecastPeriodId == NIGHT_PERIOD ? AREA_MAX_NIGHT : AREA_MAX_DAY2));
	  int areaMean = (theParameters.theForecastPeriodId == DAY1_PERIOD ? AREA_MEAN_DAY1 : 
					  (theParameters.theForecastPeriodId == NIGHT_PERIOD ? AREA_MEAN_NIGHT : AREA_MEAN_DAY2));
	  int coastMin = (theParameters.theForecastPeriodId == DAY1_PERIOD ? COAST_MIN_DAY1 : 
					  (theParameters.theForecastPeriodId == NIGHT_PERIOD ? COAST_MIN_NIGHT : COAST_MIN_DAY2));
	  int coastMax = (theParameters.theForecastPeriodId == DAY1_PERIOD ? COAST_MAX_DAY1 : 
					  (theParameters.theForecastPeriodId == NIGHT_PERIOD ? COAST_MAX_NIGHT : COAST_MAX_DAY2));
	  int coastMean = (theParameters.theForecastPeriodId == DAY1_PERIOD ? COAST_MEAN_DAY1 : 
					   (theParameters.theForecastPeriodId == NIGHT_PERIOD ? COAST_MEAN_NIGHT : COAST_MEAN_DAY2));
	  int inlandMin = (theParameters.theForecastPeriodId == DAY1_PERIOD ? INLAND_MIN_DAY1 : 
					   (theParameters.theForecastPeriodId == NIGHT_PERIOD ? INLAND_MIN_NIGHT : INLAND_MIN_DAY2));
	  int inlandMax = (theParameters.theForecastPeriodId == DAY1_PERIOD ? INLAND_MAX_DAY1 : 
					   (theParameters.theForecastPeriodId == NIGHT_PERIOD ? INLAND_MAX_NIGHT : INLAND_MAX_DAY2));
	  int inlandMean = (theParameters.theForecastPeriodId == DAY1_PERIOD ? INLAND_MEAN_DAY1 : 
						(theParameters.theForecastPeriodId == NIGHT_PERIOD ? INLAND_MEAN_NIGHT : INLAND_MEAN_DAY2));

	  if(theParameters.theForecastAreaId == FULL_AREA)
		{
		  theParameters.theDayAndNightSeparationFlag = separate_day_and_night(theParameters, FULL_AREA);

		  if(theParameters.theForecastPeriodId == DAY1_PERIOD)
			{
			  if(theParameters.theSeasonId == SUMMER_SEASON && 
				 theParameters.theWeatherResults[AREA_MAX_DAY1_MORNING]->value() != kFloatMissing &&
				 theParameters.theWeatherResults[AREA_MAX_DAY1_AFTERNOON]->value() != kFloatMissing &&
				 theParameters.theWeatherResults[AREA_MAX_DAY1_MORNING]->value() > 
				 theParameters.theWeatherResults[AREA_MAX_DAY1_AFTERNOON]->value() + MORNING_AFTERNOON_SEPARATION_LIMIT)
				{
				  theParameters.theForecastAreaId = FULL_AREA;
				  theParameters.theSubPeriodId = DAY1_MORNING_PERIOD;
				  theParameters.theForecastSubPeriod |= DAY1_MORNING_PERIOD;
				  theParameters.theForecastSubPeriod |= DAY1_AFTERNOON_PERIOD;
				  theParameters.theMinimum = theParameters.theWeatherResults[AREA_MIN_DAY1_MORNING]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[AREA_MAX_DAY1_MORNING]->value();
				  theParameters.theMean = theParameters.theWeatherResults[AREA_MEAN_DAY1_MORNING]->value();

				  sentence << day1_sentence(theParameters);

				  sentence << Delimiter(",");


				  theParameters.theForecastAreaId = FULL_AREA;
				  theParameters.theSubPeriodId = DAY1_AFTERNOON_PERIOD;
				  theParameters.theMinimum = theParameters.theWeatherResults[AREA_MIN_DAY1_AFTERNOON]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[AREA_MAX_DAY1_AFTERNOON]->value();
				  theParameters.theMean = theParameters.theWeatherResults[AREA_MEAN_DAY1_AFTERNOON]->value();

				  sentence << day1_sentence(theParameters);

				}
			  else
				{
				  theParameters.theForecastAreaId = FULL_AREA;
				  theParameters.theSubPeriodId = UNDEFINED_SUBPERIOD;
				  /*
				  theParameters.theMinimum = theParameters.theWeatherResults[areaMin]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[areaMax]->value();
				  theParameters.theMean = theParameters.theWeatherResults[areaMean]->value();
				  */
				  theParameters.theMinimum = theParameters.theWeatherResults[AREA_MIN_DAY1_AFTERNOON]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[AREA_MAX_DAY1_AFTERNOON]->value();
				  theParameters.theMean = theParameters.theWeatherResults[AREA_MEAN_DAY1_AFTERNOON]->value();

				  sentence << day1_sentence(theParameters);

				}

			}
		  else if(theParameters.theForecastPeriodId == NIGHT_PERIOD)
			{
			  if(theParameters.theDayAndNightSeparationFlag)
				{
				  theParameters.theForecastAreaId = FULL_AREA;
				  theParameters.theMaxTemperatureDay1 = theParameters.theWeatherResults[AREA_MAX_DAY1]->value();
				  theParameters.theMeanTemperatureDay1 = theParameters.theWeatherResults[AREA_MEAN_DAY1]->value();
				  theParameters.theMinimum = theParameters.theWeatherResults[areaMin]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[areaMax]->value();
				  theParameters.theMean = theParameters.theWeatherResults[areaMean]->value();

				  sentence << night_sentence(theParameters);
				  construct_optional_frost_story(theParameters);
				}

			}
		  else if(theParameters.theForecastPeriodId == DAY2_PERIOD)
			{
			  if(theParameters.theSeasonId == SUMMER_SEASON && 
				 theParameters.theWeatherResults[AREA_MAX_DAY2_MORNING]->value() != kFloatMissing &&
				 theParameters.theWeatherResults[AREA_MAX_DAY2_AFTERNOON]->value() != kFloatMissing &&
				 theParameters.theWeatherResults[AREA_MAX_DAY2_MORNING]->value() > 
				 theParameters.theWeatherResults[AREA_MAX_DAY2_AFTERNOON]->value() + MORNING_AFTERNOON_SEPARATION_LIMIT)
				{
				  theParameters.theForecastAreaId = FULL_AREA;
				  theParameters.theSubPeriodId = DAY2_MORNING_PERIOD;
				  theParameters.theForecastSubPeriod |= DAY2_MORNING_PERIOD;
				  theParameters.theForecastSubPeriod |= DAY2_AFTERNOON_PERIOD;
				  theParameters.theMeanTemperatureDay1 = theParameters.theWeatherResults[AREA_MEAN_DAY1]->value();
				  theParameters.theMinimum = theParameters.theWeatherResults[AREA_MIN_DAY2_MORNING]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[AREA_MAX_DAY2_MORNING]->value();
				  theParameters.theMean = theParameters.theWeatherResults[AREA_MEAN_DAY2_MORNING]->value();

				  sentence << day2_sentence(theParameters);

				  sentence << Delimiter(",");

				  theParameters.theForecastAreaId = FULL_AREA;
				  theParameters.theSubPeriodId = DAY2_AFTERNOON_PERIOD;
				  theParameters.theMeanTemperatureDay1 = theParameters.theWeatherResults[AREA_MEAN_DAY1]->value();
				  theParameters.theMinimum = theParameters.theWeatherResults[AREA_MIN_DAY2_AFTERNOON]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[AREA_MAX_DAY2_AFTERNOON]->value();
				  theParameters.theMean = theParameters.theWeatherResults[AREA_MEAN_DAY2_AFTERNOON]->value();

				  sentence << day2_sentence(theParameters);
				}
			  else
				{
				  theParameters.theForecastAreaId = FULL_AREA;
				  theParameters.theSubPeriodId = UNDEFINED_SUBPERIOD;
				  theParameters.theMeanTemperatureDay1 = theParameters.theWeatherResults[AREA_MEAN_DAY1]->value();
				  /*
				  theParameters.theMinimum = theParameters.theWeatherResults[areaMin]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[areaMax]->value();
				  theParameters.theMean = theParameters.theWeatherResults[areaMean]->value();
				  */
				  theParameters.theMinimum = theParameters.theWeatherResults[AREA_MIN_DAY2_AFTERNOON]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[AREA_MAX_DAY2_AFTERNOON]->value();
				  theParameters.theMean = theParameters.theWeatherResults[AREA_MEAN_DAY2_AFTERNOON]->value();

				  sentence << day2_sentence(theParameters);

				}
			}
		}
	  else if(theParameters.theForecastAreaId == INLAND_AREA)
		{
		  theParameters.theDayAndNightSeparationFlag = separate_day_and_night(theParameters, INLAND_AREA);

		  if(theParameters.theForecastPeriodId == DAY1_PERIOD)
			{
			  if(theParameters.theSeasonId == SUMMER_SEASON && 
				 theParameters.theWeatherResults[INLAND_MAX_DAY1_MORNING]->value() != kFloatMissing &&
				 theParameters.theWeatherResults[INLAND_MAX_DAY1_AFTERNOON]->value() != kFloatMissing &&
				 theParameters.theWeatherResults[INLAND_MAX_DAY1_MORNING]->value() > 
				 theParameters.theWeatherResults[INLAND_MAX_DAY1_AFTERNOON]->value() + MORNING_AFTERNOON_SEPARATION_LIMIT)
				{
				  theParameters.theForecastAreaId = INLAND_AREA;
				  theParameters.theSubPeriodId = DAY1_MORNING_PERIOD;
				  theParameters.theForecastSubPeriod |= DAY1_MORNING_PERIOD;
				  theParameters.theForecastSubPeriod |= DAY1_AFTERNOON_PERIOD;
				  theParameters.theMinimum = theParameters.theWeatherResults[INLAND_MIN_DAY1_MORNING]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[INLAND_MAX_DAY1_MORNING]->value();
				  theParameters.theMean = theParameters.theWeatherResults[INLAND_MEAN_DAY1_MORNING]->value();

				  sentence << day1_sentence(theParameters);


				  sentence << Delimiter(",");

				  theParameters.theForecastAreaId = INLAND_AREA;
				  theParameters.theSubPeriodId = DAY1_AFTERNOON_PERIOD;
				  theParameters.theMinimum = theParameters.theWeatherResults[INLAND_MIN_DAY1_AFTERNOON]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[INLAND_MAX_DAY1_AFTERNOON]->value();
				  theParameters.theMean = theParameters.theWeatherResults[INLAND_MEAN_DAY1_AFTERNOON]->value();

				  sentence << day1_sentence(theParameters);
				}
			  else
				{
				  theParameters.theForecastAreaId = INLAND_AREA;
				  theParameters.theSubPeriodId = UNDEFINED_SUBPERIOD;
				  /*
				  theParameters.theMinimum = theParameters.theWeatherResults[inlandMin]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[inlandMax]->value();
				  theParameters.theMean = theParameters.theWeatherResults[inlandMean]->value();
*/
				  theParameters.theMinimum = theParameters.theWeatherResults[INLAND_MIN_DAY1_AFTERNOON]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[INLAND_MAX_DAY1_AFTERNOON]->value();
				  theParameters.theMean = theParameters.theWeatherResults[INLAND_MEAN_DAY1_AFTERNOON]->value();

				  sentence << day1_sentence(theParameters);

				}
			}
		  else if(theParameters.theForecastPeriodId == NIGHT_PERIOD)
			{
			  if(theParameters.theDayAndNightSeparationFlag)
				{
				  theParameters.theForecastAreaId = INLAND_AREA;
				  theParameters.theMaxTemperatureDay1 = theParameters.theWeatherResults[INLAND_MAX_DAY1]->value();
				  theParameters.theMeanTemperatureDay1 = theParameters.theWeatherResults[INLAND_MEAN_DAY1]->value();
				  theParameters.theMinimum = theParameters.theWeatherResults[inlandMin]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[inlandMax]->value();
				  theParameters.theMean = theParameters.theWeatherResults[inlandMean]->value();

				  sentence << night_sentence(theParameters);
				  construct_optional_frost_story(theParameters);
				}
			}
		  else if(theParameters.theForecastPeriodId == DAY2_PERIOD)
			{
			  if(theParameters.theSeasonId == SUMMER_SEASON && 
				 theParameters.theWeatherResults[INLAND_MAX_DAY2_MORNING]->value() != kFloatMissing &&
				 theParameters.theWeatherResults[INLAND_MAX_DAY2_AFTERNOON]->value() != kFloatMissing &&
				 theParameters.theWeatherResults[INLAND_MAX_DAY2_MORNING]->value() > 
				 theParameters.theWeatherResults[INLAND_MAX_DAY2_AFTERNOON]->value() + MORNING_AFTERNOON_SEPARATION_LIMIT)
				{
				  theParameters.theForecastAreaId = INLAND_AREA;
				  theParameters.theSubPeriodId = DAY2_MORNING_PERIOD;
				  theParameters.theForecastSubPeriod |= DAY2_MORNING_PERIOD;
				  theParameters.theForecastSubPeriod |= DAY2_AFTERNOON_PERIOD;
				  theParameters.theMeanTemperatureDay1 = theParameters.theWeatherResults[INLAND_MEAN_DAY1]->value();
				  theParameters.theMinimum = theParameters.theWeatherResults[INLAND_MIN_DAY2_MORNING]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[INLAND_MAX_DAY2_MORNING]->value();
				  theParameters.theMean = theParameters.theWeatherResults[INLAND_MEAN_DAY2_MORNING]->value();

				  sentence << day2_sentence(theParameters);

				  sentence << Delimiter(",");

				  theParameters.theForecastAreaId = INLAND_AREA;
				  theParameters.theSubPeriodId = DAY2_AFTERNOON_PERIOD;
				  theParameters.theMeanTemperatureDay1 = theParameters.theWeatherResults[INLAND_MEAN_DAY1]->value();
				  theParameters.theMinimum = theParameters.theWeatherResults[INLAND_MIN_DAY2_AFTERNOON]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[INLAND_MAX_DAY2_AFTERNOON]->value();
				  theParameters.theMean = theParameters.theWeatherResults[INLAND_MEAN_DAY2_AFTERNOON]->value();

				  sentence << day2_sentence(theParameters);

				}
			  else
				{
				  theParameters.theForecastAreaId = INLAND_AREA;
				  theParameters.theSubPeriodId = UNDEFINED_SUBPERIOD;
				  theParameters.theMeanTemperatureDay1 = theParameters.theWeatherResults[INLAND_MEAN_DAY1]->value();
				  /*
				  theParameters.theMinimum = theParameters.theWeatherResults[inlandMin]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[inlandMax]->value();
				  theParameters.theMean = theParameters.theWeatherResults[inlandMean]->value();
				  */
				  theParameters.theMinimum = theParameters.theWeatherResults[INLAND_MIN_DAY2_AFTERNOON]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[INLAND_MAX_DAY2_AFTERNOON]->value();
				  theParameters.theMean = theParameters.theWeatherResults[INLAND_MEAN_DAY2_AFTERNOON]->value();


				  sentence << day2_sentence(theParameters);

				}
			}
		}
	  else if(theParameters.theForecastAreaId == COASTAL_AREA)
		{
		  theParameters.theDayAndNightSeparationFlag = separate_day_and_night(theParameters, COASTAL_AREA);

		  if(theParameters.theForecastPeriodId == DAY1_PERIOD)
			{
			  if(theParameters.theSeasonId == SUMMER_SEASON && 
				 theParameters.theWeatherResults[COAST_MAX_DAY1_MORNING]->value() != kFloatMissing &&
				 theParameters.theWeatherResults[COAST_MAX_DAY1_AFTERNOON]->value() != kFloatMissing &&
				 theParameters.theWeatherResults[COAST_MAX_DAY1_MORNING]->value() > 
				 theParameters.theWeatherResults[COAST_MAX_DAY1_AFTERNOON]->value() + MORNING_AFTERNOON_SEPARATION_LIMIT)
				{
				  theParameters.theForecastAreaId = COASTAL_AREA;
				  theParameters.theSubPeriodId = DAY1_MORNING_PERIOD;
				  theParameters.theForecastSubPeriod |= DAY1_MORNING_PERIOD;
				  theParameters.theForecastSubPeriod |= DAY1_AFTERNOON_PERIOD;
				  theParameters.theMinimum = theParameters.theWeatherResults[COAST_MIN_DAY1_MORNING]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[COAST_MAX_DAY1_MORNING]->value();
				  theParameters.theMean = theParameters.theWeatherResults[COAST_MEAN_DAY1_MORNING]->value();

				  sentence << day1_sentence(theParameters);

				  sentence << Delimiter(",");

				  theParameters.theForecastAreaId = COASTAL_AREA;
				  theParameters.theSubPeriodId = DAY1_AFTERNOON_PERIOD;
				  theParameters.theMinimum = theParameters.theWeatherResults[COAST_MIN_DAY1_AFTERNOON]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[COAST_MAX_DAY1_AFTERNOON]->value();
				  theParameters.theMean = theParameters.theWeatherResults[COAST_MEAN_DAY1_AFTERNOON]->value();

				  sentence << day1_sentence(theParameters);

				}
			  else
				{
				  theParameters.theForecastAreaId = COASTAL_AREA;
				  theParameters.theSubPeriodId = UNDEFINED_SUBPERIOD;
				  /*
				  theParameters.theMinimum = theParameters.theWeatherResults[coastMin]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[coastMax]->value();
				  theParameters.theMean = theParameters.theWeatherResults[coastMean]->value();
				  */
				  theParameters.theMinimum = theParameters.theWeatherResults[COAST_MIN_DAY1_AFTERNOON]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[COAST_MAX_DAY1_AFTERNOON]->value();
				  theParameters.theMean = theParameters.theWeatherResults[COAST_MEAN_DAY1_AFTERNOON]->value();

				  sentence << day1_sentence(theParameters);
				}
			}
		  else if(theParameters.theForecastPeriodId == NIGHT_PERIOD)
			{
			  if(theParameters.theDayAndNightSeparationFlag)
				{
				  theParameters.theForecastAreaId = COASTAL_AREA;
				  theParameters.theMaxTemperatureDay1 = theParameters.theWeatherResults[COAST_MAX_DAY1]->value();
				  theParameters.theMeanTemperatureDay1 = theParameters.theWeatherResults[COAST_MEAN_DAY1]->value();
				  theParameters.theMinimum = theParameters.theWeatherResults[coastMin]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[coastMax]->value();
				  theParameters.theMean = theParameters.theWeatherResults[coastMean]->value();

				  sentence << night_sentence(theParameters);
				  construct_optional_frost_story(theParameters);
				}
			}
		  else if(theParameters.theForecastPeriodId == DAY2_PERIOD)
			{
			  if(theParameters.theSeasonId == SUMMER_SEASON && 
				 theParameters.theWeatherResults[COAST_MAX_DAY2_MORNING]->value() != kFloatMissing &&
				 theParameters.theWeatherResults[COAST_MAX_DAY2_AFTERNOON]->value() != kFloatMissing &&
				 theParameters.theWeatherResults[COAST_MAX_DAY2_MORNING]->value() > 
				 theParameters.theWeatherResults[COAST_MAX_DAY2_AFTERNOON]->value() + MORNING_AFTERNOON_SEPARATION_LIMIT)
				{
				  theParameters.theForecastAreaId = COASTAL_AREA;
				  theParameters.theSubPeriodId = DAY2_MORNING_PERIOD;
				  theParameters.theForecastSubPeriod |= DAY2_MORNING_PERIOD;
				  theParameters.theForecastSubPeriod |= DAY2_AFTERNOON_PERIOD;
				  theParameters.theMeanTemperatureDay1 = theParameters.theWeatherResults[COAST_MEAN_DAY1]->value();
				  theParameters.theMinimum = theParameters.theWeatherResults[COAST_MIN_DAY2_MORNING]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[COAST_MAX_DAY2_MORNING]->value();
				  theParameters.theMean = theParameters.theWeatherResults[COAST_MEAN_DAY2_MORNING]->value();

				  sentence << day2_sentence(theParameters);

				  sentence << Delimiter(",");

				  theParameters.theForecastAreaId = COASTAL_AREA;
				  theParameters.theSubPeriodId = DAY2_AFTERNOON_PERIOD;
				  theParameters.theMeanTemperatureDay1 = theParameters.theWeatherResults[COAST_MEAN_DAY1]->value();
				  theParameters.theMinimum = theParameters.theWeatherResults[COAST_MIN_DAY2_AFTERNOON]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[COAST_MAX_DAY2_AFTERNOON]->value();
				  theParameters.theMean = theParameters.theWeatherResults[COAST_MEAN_DAY2_AFTERNOON]->value();

				  sentence << day2_sentence(theParameters);
				}
			  else
				{
				  theParameters.theForecastAreaId = COASTAL_AREA;
				  theParameters.theSubPeriodId = UNDEFINED_SUBPERIOD;
				  theParameters.theMeanTemperatureDay1 = theParameters.theWeatherResults[COAST_MEAN_DAY1]->value();
				  /*
				  theParameters.theMinimum = theParameters.theWeatherResults[coastMin]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[coastMax]->value();
				  theParameters.theMean = theParameters.theWeatherResults[coastMean]->value();
				  */
				  theParameters.theMinimum = theParameters.theWeatherResults[COAST_MIN_DAY2_AFTERNOON]->value();
				  theParameters.theMaximum = theParameters.theWeatherResults[COAST_MAX_DAY2_AFTERNOON]->value();
				  theParameters.theMean = theParameters.theWeatherResults[COAST_MEAN_DAY2_AFTERNOON]->value();


				  sentence << day2_sentence(theParameters);

				}
			}
		}

	  return sentence;
	}

	const Paragraph temperature_max36hours_sentence(t36hparams& theParameters)
	{
	  Paragraph paragraph;

	  if(theParameters.theForecastArea == NO_AREA)
		return paragraph;

	  const int temperature_limit_coast_inland = optional_int(theParameters.theVariable + 
															  "::temperature_limit_coast_inland", 3);

	  forecast_season_id forecast_season(UNDEFINED_SEASON);

	  // find out seasons of the forecast periods
	  if(theParameters.theForecastPeriod & DAY1_PERIOD)
		{
		  forecast_season = get_forecast_season(theParameters.theGenerator.period(1).localStartTime(), 
												theParameters.theVariable);
		}
	  else if(theParameters.theForecastPeriod & NIGHT_PERIOD)
		{
		  forecast_season = get_forecast_season(theParameters.theGenerator.period(1).localStartTime(), 
												theParameters.theVariable);
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

	  // Day1
	  // 1. Day1 inland 
	  // 2. Day1 coastal

	  vector<int> periodAreas;
	  processing_order processingOrder;

	  if(theParameters.theForecastPeriod & DAY1_PERIOD && 
		 theParameters.theForecastPeriod & NIGHT_PERIOD && 
		 theParameters.theForecastPeriod & DAY2_PERIOD)
		{
		  processingOrder = DAY1_DAY2_NIGHT;
		}
	  else if(theParameters.theForecastPeriod & NIGHT_PERIOD && 
			  theParameters.theForecastPeriod & DAY2_PERIOD)
		{
		  processingOrder = NIGHT_DAY2;
		}
	  else if(theParameters.theForecastPeriod & DAY1_PERIOD && 
			  theParameters.theForecastPeriod & NIGHT_PERIOD)
		{
		  processingOrder = DAY1_NIGHT;
		}
	  else if(theParameters.theForecastPeriod & DAY1_PERIOD)
		{
		  processingOrder = DAY1;
		}

	  bool separate_inland_and_coast_day1 = false;
	  bool separate_inland_and_coast_day2 = false;
	  bool separate_inland_and_coast_night = false;

	  if(theParameters.theWeatherResults[INLAND_MEAN_DAY1]->value() != kFloatMissing &&
		 theParameters.theWeatherResults[COAST_MEAN_DAY1]->value() != kFloatMissing &&
		 !(theParameters.theSeasonId == WINTER_SEASON && 
		   theParameters.theWeatherResults[COAST_MEAN_DAY1]->value() <= COASTAL_AREA_IGNORE_LIMIT))
		{
		  float temperature_diff_day1 = abs(round(theParameters.theWeatherResults[INLAND_MEAN_DAY1]->value() - 
												  theParameters.theWeatherResults[COAST_MEAN_DAY1]->value()));
		  separate_inland_and_coast_day1 = (temperature_diff_day1 >= temperature_limit_coast_inland);
		}

	  if(theParameters.theWeatherResults[INLAND_MEAN_DAY2]->value() != kFloatMissing &&
		 theParameters.theWeatherResults[COAST_MEAN_DAY2]->value() != kFloatMissing &&
		 !(theParameters.theSeasonId == WINTER_SEASON && 
		   theParameters.theWeatherResults[COAST_MEAN_DAY2]->value() <= COASTAL_AREA_IGNORE_LIMIT))
		{
		  float temperature_diff_day2 = abs(theParameters.theWeatherResults[INLAND_MEAN_DAY2]->value() - 
											theParameters.theWeatherResults[COAST_MEAN_DAY2]->value());
		  separate_inland_and_coast_day2 = (temperature_diff_day2 >= temperature_limit_coast_inland);
		}

	  if(theParameters.theWeatherResults[INLAND_MEAN_NIGHT]->value() != kFloatMissing &&
		 theParameters.theWeatherResults[COAST_MEAN_NIGHT]->value() != kFloatMissing &&
		 !(theParameters.theSeasonId == WINTER_SEASON && 
		   theParameters.theWeatherResults[COAST_MEAN_NIGHT]->value() <= COASTAL_AREA_IGNORE_LIMIT))
		{
		  float temperature_diff_night = abs(theParameters.theWeatherResults[INLAND_MEAN_NIGHT]->value() - 
											 theParameters.theWeatherResults[COAST_MEAN_NIGHT]->value());
		  separate_inland_and_coast_night = (temperature_diff_night >= temperature_limit_coast_inland);
		}

	  if(processingOrder == DAY1_DAY2_NIGHT)
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

		  periodAreas.push_back(DELIMITER_DOT);

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
	  else if(processingOrder == DAY1)
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
		}
	  
	  Sentence sentenceUnderConstruction;

	  for(unsigned int i = 0; i < periodAreas.size(); i++)
		{
		  int periodArea = periodAreas[i];

		  if(periodArea == DELIMITER_DOT)
			{
			  if(!sentenceUnderConstruction.empty())
				{
				  // if comparison between today and tomorrow, use comma
				  if(i > 0 && processingOrder == DAY1_DAY2_NIGHT && 
					 periodAreas[i-1] == DAY1_FULL && periodAreas[i+1] == DAY2_FULL &&
					 !theParameters.morningAndAfternoonSeparated(DAY1_PERIOD) &&
					 !theParameters.morningAndAfternoonSeparated(DAY2_PERIOD))
					{
					  sentenceUnderConstruction << Delimiter(",");
					}
				  else
					{
					  paragraph << sentenceUnderConstruction;
					  if(!theParameters.theOptionalFrostParagraph.empty())
						{
						  paragraph << theParameters.theOptionalFrostParagraph;
						  theParameters.theOptionalFrostParagraph.clear();
						}
					  sentenceUnderConstruction.clear();
					}
				}
			  continue;
			}
		  else if(periodArea == DELIMITER_COMMA)
			{
			  if(!sentenceUnderConstruction.empty())
				{
				  sentenceUnderConstruction << Delimiter(",");
				}
			  continue;
			}

		  unsigned short story_forecast_areas = 0x0;
		  forecast_period_id period_id;
		  forecast_area_id area_id;

   
		  if(periodArea == DAY1_INLAND || periodArea == DAY1_COASTAL || periodArea == DAY1_FULL)
			{
			  period_id = DAY1_PERIOD;
			  theParameters.theWeatherPeriod = theParameters.theGenerator.period(1);

			  if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
				{
				  if(separate_inland_and_coast_day1)
					{
					  area_id = periodAreas[i] == DAY1_INLAND ? INLAND_AREA : COASTAL_AREA;
					  story_forecast_areas |= COASTAL_AREA;
					  story_forecast_areas |= INLAND_AREA;
					  theParameters.theForecastAreaDay1 |= COASTAL_AREA;
					  theParameters.theForecastAreaDay1 |= INLAND_AREA;
					}
				  else
					{
					  area_id = FULL_AREA;
					  story_forecast_areas |= FULL_AREA;
					  theParameters.theForecastAreaDay1 |= FULL_AREA;
					}
				}
			  else if(theParameters.theForecastArea & INLAND_AREA)
				{
				  area_id = INLAND_AREA;
				  story_forecast_areas |= INLAND_AREA;
				  theParameters.theForecastAreaDay1 |= INLAND_AREA;
				}
			  else if(theParameters.theForecastArea & COASTAL_AREA)
				{
				  area_id = COASTAL_AREA;
				  story_forecast_areas |= COASTAL_AREA;
				  theParameters.theForecastAreaDay1 |= COASTAL_AREA;
				}
			  else if(theParameters.theForecastArea & FULL_AREA)
				{
				  area_id = FULL_AREA;
				  story_forecast_areas |= FULL_AREA;
				  theParameters.theForecastAreaDay1 |= FULL_AREA;
				}
			}
		  else if(periodArea == NIGHT_INLAND || periodArea == NIGHT_COASTAL || periodArea == NIGHT_FULL)
			{	
			  period_id = NIGHT_PERIOD;

			  if(theParameters.theForecastPeriod & DAY1_PERIOD)
				theParameters.theWeatherPeriod = theParameters.theGenerator.period(2);
			  else
				theParameters.theWeatherPeriod = theParameters.theGenerator.period(1);

			  if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
				{

				  if(separate_inland_and_coast_night)
					{
					  area_id = periodAreas[i] == NIGHT_INLAND ? INLAND_AREA : COASTAL_AREA;
					  story_forecast_areas |= COASTAL_AREA;
					  story_forecast_areas |= INLAND_AREA;
					  theParameters.theForecastAreaNight |= COASTAL_AREA;
					  theParameters.theForecastAreaNight |= INLAND_AREA;
					}
				  else
					{
					  area_id = FULL_AREA;
					  story_forecast_areas |= FULL_AREA;
					  theParameters.theForecastAreaNight |= FULL_AREA;
					}
				}
			  else if(theParameters.theForecastArea & INLAND_AREA)
				{
				  area_id = INLAND_AREA;
				  story_forecast_areas |= INLAND_AREA;
				  theParameters.theForecastAreaNight |= INLAND_AREA;
				}
			  else if(theParameters.theForecastArea & COASTAL_AREA)
				{
				  area_id = COASTAL_AREA;
				  story_forecast_areas |= COASTAL_AREA;
				  theParameters.theForecastAreaNight |= COASTAL_AREA;
				}
			  else if(theParameters.theForecastArea & FULL_AREA)
				{
				  area_id = FULL_AREA;
				  story_forecast_areas |= FULL_AREA;
				  theParameters.theForecastAreaNight |= FULL_AREA;
				}
			}
		  else if(periodArea == DAY2_INLAND || periodArea == DAY2_COASTAL || periodArea == DAY2_FULL)
			{
			  period_id = DAY2_PERIOD;
			  if(theParameters.theForecastPeriod & DAY1_PERIOD)
				theParameters.theWeatherPeriod = theParameters.theGenerator.period(3);
			  else if(theParameters.theForecastPeriod & NIGHT_PERIOD)
				theParameters.theWeatherPeriod = theParameters.theGenerator.period(2);
			  else
				theParameters.theWeatherPeriod = theParameters.theGenerator.period(1);

			  if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
				{
				  if(separate_inland_and_coast_day2)
					{
					  area_id = periodAreas[i] == DAY2_INLAND ? INLAND_AREA : COASTAL_AREA;
					  story_forecast_areas |= COASTAL_AREA;
					  story_forecast_areas |= INLAND_AREA;
					  theParameters.theForecastAreaDay2 |= COASTAL_AREA;
					  theParameters.theForecastAreaDay2 |= INLAND_AREA;
					}
				  else
					{
					  area_id = FULL_AREA;
					  story_forecast_areas |= FULL_AREA;
					  theParameters.theForecastAreaDay2 |= FULL_AREA;
					}
				}
			  else if(theParameters.theForecastArea & INLAND_AREA)
				{
				  area_id = INLAND_AREA;
				  story_forecast_areas |= INLAND_AREA;
				  theParameters.theForecastAreaDay2 |= INLAND_AREA;
				}
			  else if(theParameters.theForecastArea & COASTAL_AREA)
				{
				  area_id = COASTAL_AREA;
				  story_forecast_areas |= COASTAL_AREA;
				  theParameters.theForecastAreaDay2 |= COASTAL_AREA;
				}
			  else if(theParameters.theForecastArea & FULL_AREA)
				{
				  area_id = FULL_AREA;
				  story_forecast_areas |= FULL_AREA;
				  theParameters.theForecastAreaDay2 |= FULL_AREA;
				}
			}

		  Sentence addThisSentence;
		  theParameters.theForecastAreaId = area_id;
		  theParameters.theForecastPeriodId = period_id;
		  theParameters.theSeasonId = forecast_season;
		  
 
		  addThisSentence << construct_sentence(theParameters);

		  if(!addThisSentence.empty())
			{
			  sentenceUnderConstruction << addThisSentence;
			}
		}

	  if(!sentenceUnderConstruction.empty())
		{
		  paragraph << sentenceUnderConstruction;
		}

	  if(!theParameters.theOptionalFrostParagraph.empty())
		{
		  paragraph << theParameters.theOptionalFrostParagraph;
		  theParameters.theOptionalFrostParagraph.clear();
		}

	  return paragraph;
	}

	bool valid_value_period_check(const float& value, unsigned short& forecast_period, const unsigned short& mask)
	{
	  bool retval = (value != kFloatMissing);

	  if(!retval)
		forecast_period &= ~mask;

	  return retval;
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

	Paragraph paragraph;
	MessageLogger log("TemperatureStory::max36h");
	
	if(itsArea.isNamed())
	  {
		std::string nimi(itsArea.name());
		log <<  nimi << endl;
	  }

	log_start_time_and_end_time(log, "Whole period: ", itsPeriod);


	// Period generator
	NightAndDayPeriodGenerator generator(itsPeriod, itsVar);

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

	unsigned short forecast_area = 0x0;
	unsigned short forecast_period = 0x0;
	forecast_season_id forecast_season = get_forecast_season(generator.period(1).localStartTime(), itsVar);
		      
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
		calculate_results(log, itsVar, itsSources, itsArea,
						  period, DAY1_PERIOD, forecast_season, COASTAL_AREA, weatherResults);
		calculate_results(log, itsVar, itsSources, itsArea,
						  period, DAY1_PERIOD, forecast_season, FULL_AREA, weatherResults);
		valid_value_period_check(weatherResults[AREA_MIN_DAY1]->value(), forecast_period, DAY1_PERIOD);

		// day1 period exists, so
		// if the area is included, it must have valid values
		forecast_area |= (weatherResults[COAST_MIN_DAY1]->value() != kFloatMissing ? COASTAL_AREA : 0x0); 
		forecast_area |= (weatherResults[INLAND_MIN_DAY1]->value() != kFloatMissing ? INLAND_AREA : 0x0);
		forecast_area |= (weatherResults[AREA_MIN_DAY1]->value() != kFloatMissing ? FULL_AREA : 0x0);

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
				if(forecast_area & FULL_AREA && (forecast_period & NIGHT_PERIOD))
				  {
					calculate_results(log, itsVar, itsSources, itsArea,
									  period, NIGHT_PERIOD, forecast_season, FULL_AREA, weatherResults);
					valid_value_period_check(weatherResults[AREA_MIN_NIGHT]->value(), forecast_period, NIGHT_PERIOD);
				  }
				forecast_area |= (weatherResults[COAST_MIN_NIGHT]->value() != kFloatMissing ? COASTAL_AREA : 0x0); 
				forecast_area |= (weatherResults[INLAND_MIN_NIGHT]->value() != kFloatMissing ? INLAND_AREA : 0x0);
				forecast_area |= (weatherResults[AREA_MIN_NIGHT]->value() != kFloatMissing ? FULL_AREA : 0x0);
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
				if(forecast_area & FULL_AREA && (forecast_period & NIGHT_PERIOD))
				  {
					calculate_results(log, itsVar, itsSources, itsArea,
									  period, DAY2_PERIOD, forecast_season, FULL_AREA, weatherResults);
					valid_value_period_check(weatherResults[AREA_MIN_DAY2]->value(), forecast_period, DAY2_PERIOD);
				  }
				
				forecast_area |= (weatherResults[COAST_MIN_DAY2]->value() != kFloatMissing ? COASTAL_AREA : 0x0);
 				forecast_area |= (weatherResults[INLAND_MIN_DAY2]->value() != kFloatMissing ? INLAND_AREA : 0x0);
				forecast_area |= (weatherResults[AREA_MIN_DAY2]->value() != kFloatMissing ? FULL_AREA : 0x0);
			  }
		  }
	  }
	else
	  {
		period = generator.period(1);
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
		forecast_area |= (weatherResults[AREA_MIN_NIGHT]->value() != kFloatMissing ? FULL_AREA : 0x0);

		//		if(!(forecast_area & (COASTAL_AREA | INLAND_AREA)))

		if(forecast_area == NO_AREA)
		  {
			valid_value_period_check(kFloatMissing, forecast_period, NIGHT_PERIOD);
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

				if(forecast_area & INLAND_AREA && forecast_area & COASTAL_AREA && (forecast_period & DAY2_PERIOD))
				  calculate_results(log, itsVar, itsSources, itsArea,
									period, DAY2_PERIOD, forecast_season, FULL_AREA, weatherResults);
			  }
		  }
	  }
	
	const string range_separator = optional_string(itsVar + "::rangeseparator", "...");
	const int mininterval = optional_int(itsVar + "::mininterval", 2);
	const bool interval_zero = optional_bool(itsVar+"::always_interval_zero",false);

	t36hparams parameters(itsVar,
						  log,
						  generator,
						  forecast_season,
						  forecast_area,
						  forecast_period,
						  itsForecastTime,
						  period,
						  itsArea,
						  itsSources,
						  weatherResults);

	parameters.theRangeSeparator = range_separator;
	parameters.theMinInterval = mininterval;
	parameters.theZeroIntervalFlag = interval_zero;

	if(forecast_area != NO_AREA)
	  {
		paragraph << temperature_max36hours_sentence(parameters);
	  }

	log_weather_results(parameters);

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
  
