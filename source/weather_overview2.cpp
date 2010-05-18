// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WeatherStory::overview
 */
// ======================================================================

#include "WeatherStory.h"
#include "CloudinessStory.h"
#include "CloudinessStoryTools.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "HourPeriodGenerator.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "PrecipitationPeriodTools.h"
#include "PrecipitationStoryTools.h"
#include "RangeAcceptor.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "TimeTools.h"
#include "WeatherResult.h"
#include "WeekdayTools.h"
#include "NullPeriodGenerator.h"
#include "WeatherPeriodTools.h"

#include <boost/lexical_cast.hpp>
#include <vector>

using namespace Settings;
using namespace WeatherAnalysis;
using namespace boost;
using namespace std;

namespace TextGen
{
	struct WeatherResultDataItem
	{
	  WeatherResultDataItem(const WeatherPeriod& weatherPeriod, const WeatherResult& weatherResult) :
		theWeatherPeriod(weatherPeriod),
		theWeatherResult(weatherResult)
	  {}

	  WeatherPeriod theWeatherPeriod;
	  WeatherResult theWeatherResult;
	};

	typedef vector<WeatherResultDataItem*> weather_result_data_item_vector;

	struct wo_params
	{								 
	  wo_params(const string& variable,
				const WeatherArea& weatherArea,
				const AnalysisSources& analysisSources,
				MessageLogger& log,
				weather_result_data_item_vector& temperatureTimeSeries,
				weather_result_data_item_vector& precipitationTimeSeries,
				weather_result_data_item_vector& cloudySkyTimeSeries,
				weather_result_data_item_vector& clearSkyTimeSeries):
	  theVariable(variable),
	  theWeatherArea(weatherArea),
	  theAnalysisSources(analysisSources),
	  theLog(log),
	  theTemperatureTimeSeries(temperatureTimeSeries),
	  thePrecipitationTimeSeries(precipitationTimeSeries),
	  theCloudySkyTimeSeries(cloudySkyTimeSeries),
	  theClearSkyTimeSeries(clearSkyTimeSeries)
	  {
	  }

	  const string& theVariable;
	  const WeatherArea& theWeatherArea;
	  const AnalysisSources& theAnalysisSources;
	  MessageLogger& theLog;
	  weather_result_data_item_vector& theTemperatureTimeSeries;
	  weather_result_data_item_vector& thePrecipitationTimeSeries;
	  weather_result_data_item_vector& theCloudySkyTimeSeries;
	  weather_result_data_item_vector& theClearSkyTimeSeries;
	};








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

  const void log_weather_result_data_item(MessageLogger& theLog, 
										  const WeatherResultDataItem& theWeatherResultDataItem)
  {
	theLog << theWeatherResultDataItem.theWeatherPeriod.localStartTime()
		   << " ... "
		   << theWeatherResultDataItem.theWeatherPeriod.localEndTime()
		   << ": "
		   << theWeatherResultDataItem.theWeatherResult
		   << endl;
  }

  const void log_weather_result_time_series(MessageLogger& theLog, 
											const std::string& theLogMessage, 
											weather_result_data_item_vector& theTimeSeries)
  {
	theLog << NFmiStringTools::Convert(theLogMessage) << endl;

	for(unsigned int i = 0; i < theTimeSeries.size(); i++)
	  {
		//		const WeatherResultDataItem& theWeatherResultDataItem = *theTimeSeries[i]; 
		log_weather_result_data_item(theLog, *theTimeSeries[i]);
	  } 
  }

  void populate_temperature_time_series(const wo_params& theParameters)
  {
	GridForecaster theForecaster;
	for(unsigned int i = 0; i < theParameters.theTemperatureTimeSeries.size(); i++)
	  {
		  WeatherResult result = theForecaster.analyze(theParameters.theVariable,
													   theParameters.theAnalysisSources,
													   Temperature,
													   Mean,
													   Maximum,
													   theParameters.theWeatherArea,
													   theParameters.theTemperatureTimeSeries[i]->theWeatherPeriod);

		  theParameters.theTemperatureTimeSeries[i]->theWeatherResult = result;
	  } 
  }

  void populate_precipitation_time_series(const wo_params& theParameters)
  {
	GridForecaster theForecaster;
	RangeAcceptor rainlimits;
	rainlimits.lowerLimit(Settings::optional_double(theParameters.theVariable+"::minrain",0));

	for(unsigned int i = 0; i < theParameters.thePrecipitationTimeSeries.size(); i++)
	  {
        WeatherResult result = theForecaster.analyze(theParameters.theVariable,
													 theParameters.theAnalysisSources,
													 Precipitation,
													 Mean,
													 Sum,
													 theParameters.theWeatherArea,
													 theParameters.thePrecipitationTimeSeries[i]->theWeatherPeriod,
													 DefaultAcceptor(),
													 rainlimits);
		
		theParameters.thePrecipitationTimeSeries[i]->theWeatherResult = result;
	  } 
  }

  void populate_cloudiness_time_series(const wo_params& theParameters)
  {
	  const int clear = optional_percentage(theParameters.theVariable+"::clear",40);
	  const int cloudy = optional_percentage(theParameters.theVariable+"::cloudy",70);
	  
	  RangeAcceptor cloudylimits;
	  cloudylimits.lowerLimit(cloudy);
	  
	  RangeAcceptor clearlimits;
	  clearlimits.upperLimit(clear);

	  GridForecaster forecaster;

	  for(unsigned int i = 0; i < theParameters.theCloudySkyTimeSeries.size(); i++)
		{
		  const string hourstr = "hour"+lexical_cast<string>(i+1);
		  WeatherPeriod weatherPeriod = theParameters.theCloudySkyTimeSeries[i]->theWeatherPeriod;
		  const WeatherResult cloudy_percentage =
			forecaster.analyze(theParameters.theVariable+"::fake::"+hourstr+"::cloudy",
							   theParameters.theAnalysisSources,
							   Cloudiness,
							   Mean,
							   Percentage,
							   theParameters.theWeatherArea,
							   weatherPeriod,//theParameters.theCloudySkyTimeSeries[i]->theWeatherPeriod,
							   DefaultAcceptor(),
							   DefaultAcceptor(),
							   cloudylimits);

		  theParameters.theCloudySkyTimeSeries[i]->theWeatherResult = cloudy_percentage;
		}

	  for(unsigned int i = 0; i < theParameters.theClearSkyTimeSeries.size(); i++)
		{
		  const string hourstr = "hour"+lexical_cast<string>(i+1);
		  WeatherPeriod weatherPeriod = theParameters.theCloudySkyTimeSeries[i]->theWeatherPeriod;
		  const WeatherResult clear_percentage =
			forecaster.analyze(theParameters.theVariable+"::fake::"+hourstr+"::clear",
							   theParameters.theAnalysisSources,
							   Cloudiness,
							   Mean,
							   Percentage,
							   theParameters.theWeatherArea,
							   weatherPeriod,//theParameters.theClearSkyTimeSeries[i]->theWeatherPeriod,
							   DefaultAcceptor(),
							   DefaultAcceptor(),
							   clearlimits);

		  theParameters.theClearSkyTimeSeries[i]->theWeatherResult = clear_percentage;
		}
  }


  // ----------------------------------------------------------------------
  /*!populate_temperature_time_series
   * \brief Generate overview on weather
   *
   * \return The story
   *
   * \see page_weather_overview
   *
   * \todo Much missing
   */
  // ----------------------------------------------------------------------

  const Paragraph WeatherStory::overview2() const
  {
	MessageLogger log("WeatherStory::overview2");

	using namespace PrecipitationPeriodTools;

	Paragraph paragraph;

	weather_result_data_item_vector temperatureTimeSeries;
	weather_result_data_item_vector precipitationTimeSeries;
	weather_result_data_item_vector cloudySkyTimeSeries;
	weather_result_data_item_vector clearSkyTimeSeries;


	// first split the whole period to one-hour subperiods
	NFmiTime periodStartTime = itsPeriod.localStartTime();
	//NFmiTime periodEndTime = itsPeriod.localEndTime();

	while(periodStartTime.IsLessThan(itsPeriod.localEndTime()))
	  {
		NFmiTime periodEndTime = periodStartTime;
		periodEndTime.ChangeByHours(1);
		WeatherPeriod theWeatherPeriod(periodStartTime, periodEndTime);
		WeatherResult theWeatherResult(kFloatMissing, 0);
		//		log_start_time_and_end_time(log, "periodi: ", theWeatherPeriod);
		temperatureTimeSeries.push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		precipitationTimeSeries.push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		cloudySkyTimeSeries.push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		clearSkyTimeSeries.push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));

		periodStartTime.ChangeByHours(1);
	  }

	wo_params parameters(itsVar,
						 itsArea,
						 itsSources,
						 log,
						 temperatureTimeSeries,
						 precipitationTimeSeries,
						 cloudySkyTimeSeries,
						 clearSkyTimeSeries);

	
	populate_temperature_time_series(parameters);
	populate_precipitation_time_series(parameters);
	populate_cloudiness_time_series(parameters);

	log_weather_result_time_series(log, "*** TEMPERATURE ****", temperatureTimeSeries);
	log_weather_result_time_series(log, "*** PRECIPITATION ****", precipitationTimeSeries);
	log_weather_result_time_series(log, "*** CLOUDY SKY PERCENTAGE ****", cloudySkyTimeSeries);
	log_weather_result_time_series(log, "*** CLEAR SKY PERCENTAGE ****", clearSkyTimeSeries);

	// delete the allocated WeatherResultDataItem-objects
	for(unsigned int i = 0; i < temperatureTimeSeries.size(); i++)
	  {
		delete temperatureTimeSeries[i];
		delete precipitationTimeSeries[i];
		delete cloudySkyTimeSeries[i];
		delete clearSkyTimeSeries[i];
	  } 
	
	log << paragraph;
	return paragraph;
  }


} // namespace TextGen

// ======================================================================
