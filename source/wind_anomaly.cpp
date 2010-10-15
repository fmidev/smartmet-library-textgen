// ======================================================================
/*!
 * \file
 *†\brief Implementation of method TextGen::WindStory::anomaly
 */
// ======================================================================

#include "WindStory.h"

#include "GridForecaster.h"
#include "NightAndDayPeriodGenerator.h"
#include "TemperatureStoryTools.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "Settings.h"
#include "WindStoryTools.h"
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

  namespace WindAnomaly
  {
	using NFmiStringTools::Convert;
	using namespace WeatherAnalysis;
	using namespace WindStoryTools;
	using namespace TemperatureStoryTools;
	using namespace boost;
	using namespace std;
	using namespace Settings;
	using namespace SeasonTools;
	using namespace AreaTools;
	using Settings::optional_int;
	using Settings::optional_bool;
	using Settings::optional_string;

#define WINDY_WEATER_LIMIT 7.0
#define EXTREMELY_WINDY_WEATHER_LIMIT 10.0
#define WIND_COOLING_THE_WEATHER_LIMIT 6.0
#define EXTREME_WINDCHILL_LIMIT -35.0
#define MILD_WINDCHILL_LIMIT -25.0
#define ZERO_DEGREES 0.0

#define SAA_WORD "s‰‰"
#define ON_WORD "on"
#define POIKKEUKSELLISEN_WORD "poikkeuksellisen"
#define HYVIN_WORD "hyvin"
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

	struct wind_anomaly_params
	{
	  wind_anomaly_params(const string& variable,
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
		  theFakeVariable(""),
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
	  string theFakeVariable;
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

	void log_data(const wind_anomaly_params& theParameters)
	{
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
	}

	void calculate_windspeed_and_chill(wind_anomaly_params& theParameters, 
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

	const Sentence test_windspeed(const float& theWindSpeed1, const float& theWindSpeed2, const char* theReturnString)
	{
	  Sentence retval;

	  if(abs(theWindSpeed1 - theWindSpeed2) > 1.0)
		retval << theReturnString;

	  return retval;
	}

	const Sentence windiness_sentence(const wind_anomaly_params& theParameters)
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

	  Sentence theSpecifiedDay;
	  theSpecifiedDay << PeriodPhraseFactory::create("today",
													 theParameters.theVariable,
													 theParameters.theForecastTime,
													 theParameters.theDay2Period,
													 theParameters.theArea);
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
	  if(sentence.empty())
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
					 float morningWind = temperatureInlandMorning > temperatureCoastalMorning ?
					   temperatureInlandMorning : temperatureCoastalMorning;
					 float afternoonWind = temperatureInlandAfternoon > temperatureCoastalAfternoon ?
					   temperatureInlandAfternoon : temperatureCoastalAfternoon;
					 temperature = afternoonWind > morningWind ? afternoonWind : morningWind;
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

	const Sentence windchill_sentence(const wind_anomaly_params& theParameters)
	{
	  Sentence sentence;
	  
	  bool inlandIncluded = theParameters.theWindchillInlandMorningMinimum.value() != kFloatMissing;
	  bool coastIncluded = theParameters.theWindchillInlandMorningMinimum.value() != kFloatMissing;

	  WeatherResult windChillMorningMean = theParameters.theWindchillInlandMorningMinimum;
	  WeatherResult windChillAfternoonMean = theParameters.theWindchillInlandMorningMinimum;

	  forecast_area_id areaMorning = FULL_AREA;
	  forecast_area_id areaAfternoon = FULL_AREA;
	  Sentence theSpecifiedDay;
	  theSpecifiedDay << PeriodPhraseFactory::create("today",
													 theParameters.theVariable,
													 theParameters.theForecastTime,
													 theParameters.theDay2Period,
													 theParameters.theArea);
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


  } // namespace WindAnomaly

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on wind anomaly
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  const Paragraph WindStory::anomaly() const
  {
	using namespace WindAnomaly;

	Paragraph paragraph;
	MessageLogger log("WindStory::anomaly");

	if(itsArea.isNamed())
	  {
		std::string nimi(itsArea.name());
		log <<  nimi << endl;
	  }

	bool handleWindchill = false;
	//const int day_starthour = optional_hour(itsVar+"::day::starthour",6);
	//const int day_maxstarthour = optional_hour(itsVar+"::day::maxstarthour",12);

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

	if(generator00.size() == 0)
	  {
		log << "No weather periods available!" << endl;
		log << paragraph;
		return paragraph;
	  }

	log << "period contains ";

	if(generator00.isday(1))
	  {
		if(generator00.size() > 2)
		  {
			log << "today, night and tomorrow" << endl;
		  }
		else if(generator00.size() == 2)
		  {
			log << "today and night" << endl;
			// if forecast time is not today, extend period from the start
			if(abs(itsForecastTime.DifferenceInHours(generator00.period(1).localStartTime())) > 12)
			  periodStartTime.ChangeByHours(-24);
			else
			  periodEndTime.ChangeByHours(12);
		  }
		else
		  {
			log << "today" << endl;
			// if forecast time is not today, extend period from the start
			if(abs(itsForecastTime.DifferenceInHours(generator00.period(1).localStartTime())) > 12)
			  periodStartTime.ChangeByHours(-24);
			else
			  periodEndTime.ChangeByHours(24);
		  }
	  }
	else
	  {
		if(generator00.size() == 1)
		  {
			log << "one night" << endl;
			periodStartTime.ChangeByHours(-12);
			periodEndTime.ChangeByHours(12);
		  }
		else
		  {
			log << "night and tomorrow" << endl;		  
			periodStartTime.ChangeByHours(-12);
		  }
	  }


	/*
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
	*/


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

	wind_anomaly_params parameters(itsVar,
								   log,
								   itsSources,
								   itsArea,
								   theExtendedPeriod,
								   day1Period,
								   nightPeriod,
								   day2Period,
								   theSeasonId,
								   itsForecastTime);

	WeatherArea inlandArea = itsArea;
	inlandArea.type(WeatherArea::Inland);
	WeatherArea coastalArea = itsArea;
	coastalArea.type(WeatherArea::Coast);

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

	
	Paragraph paragraphDev;
	Sentence windinessSentence;
	Sentence windChillSentence;

	windinessSentence << windiness_sentence(parameters);

	if(handleWindchill)
	  windChillSentence << windchill_sentence(parameters);

	log_data(parameters);

	log << "windiness: ";
	log << windinessSentence;
	log << "wind chill: ";
	log << windChillSentence;

	paragraph << windinessSentence;
	paragraph << windChillSentence;

	log << paragraph;

	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================

  
