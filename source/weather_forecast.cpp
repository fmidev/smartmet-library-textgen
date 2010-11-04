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
#include "NightAndDayPeriodGenerator.h"
#include "PeriodPhraseFactory.h"
#include "PrecipitationPeriodTools.h"
#include "PrecipitationStoryTools.h"
#include "RangeAcceptor.h"
#include "ValueAcceptor.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "TimeTools.h"
#include "WeatherResult.h"
#include "WeekdayTools.h"
#include "NullPeriodGenerator.h"
#include "WeatherPeriodTools.h"
#include "AreaTools.h"
#include "MathTools.h"
#include "SeasonTools.h"
#include "SubMaskExtractor.h"
#include "WeatherForecast.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "PlainTextFormatter.h"
#include "PrecipitationForecast.h"
#include "CloudinessForecast.h"
#include "FogForecast.h"
#include "ThunderForecast.h"
#include "NFmiCombinedParam.h"
#include "WeatherHistory.h"

#include <boost/lexical_cast.hpp>
#include <vector>
#include <map>

namespace TextGen
{

using namespace Settings;
using namespace WeatherAnalysis;
using namespace AreaTools;
using namespace boost;
using namespace std;


  void print_out_trend_vector(std::ostream& theOutput, 
							  const trend_id_vector& theTrendVector)
  {
	for(unsigned int i = 0; i < theTrendVector.size(); i++)
	  {
		trend_id trid(theTrendVector.at(i).second);

		theOutput << theTrendVector.at(i).first
				  << ": "
				  << trend_string(trid)
				  << endl;
	  }
  }


  std::ostream& operator<<(std::ostream & theOutput,
						   const WeatherResultDataItem& theWeatherResultDataItem)
  {
	const WeatherResult theResult(theWeatherResultDataItem.theResult);

	theOutput << theWeatherResultDataItem.thePeriod.localStartTime()
			  << " ... "
			  << theWeatherResultDataItem.thePeriod.localEndTime()
			  << ": "
			  << '('
			  << theWeatherResultDataItem.theResult.value()
			  << ','
			  << theWeatherResultDataItem.theResult.error()
			  << ')'
			  << endl;

	return theOutput;
  }


  bool same_content(const Sentence& sentence1, const Sentence& sentence2)
  {
	shared_ptr<TextGen::Dictionary> dict;
	dict.reset(TextGen::DictionaryFactory::create("multimysql"));

	dict->init("fi");
	TextGen::PlainTextFormatter formatter;
	formatter.dictionary(dict);
	std::string string1(sentence1.realize(formatter));
	std::string string2(sentence2.realize(formatter));

	return (string1.compare(string2) == 0);
  }

  void get_part_of_the_day_time_series(wf_story_params& theParameters,
									   const WeatherPeriod& theWeatherPeriod,
									   const part_of_the_day_id& thePartOfTheDayId, 
									   const weather_result_data_id& theDataId,
									   weather_result_data_item_vector& theResultVector)
  {
#ifdef LATER
	weather_result_data_item_vector* theCompleteDataVector = theParameters.theHourlyDataContainer[theDataId];

	for(unsigned int i = 0; i < theCompleteDataVector->size(); i++)
	  {
		//		WeatherResult& weatherResult = (*theCompleteDataVector)[i]->theResult;
		WeatherPeriod& weatherPeriod = (*theCompleteDataVector)[i]->thePeriod;
		if(weatherPeriod.localStartTime().EpochTime() >= theWeatherPeriod.localStartTime().EpochTime() &&
		   weatherPeriod.localEndTime().EpochTime() <= theWeatherPeriod.localEndTime().EpochTime() &&
		   weatherPeriod.localStartTime().GetJulianDay() == theWeatherPeriod.localStartTime().GetJulianDay() &&
		   weatherPeriod.localEndTime().GetJulianDay() == theWeatherPeriod.localEndTime().GetJulianDay())
		  {
			int starthour = -1;
			int endhour = -1;
			switch(thePartOfTheDayId)
			  {
			  case AAMU:
				{
				  starthour = 6;
				  endhour = 9;
				}
				break;
			  case AAMUPAIVA:
				{
				  starthour = 9;
				  endhour = 11;
				}
				break;
			  case PITKA_AAMUPAIVA:
				{
				  starthour = 9;
				  endhour = 12;
				}
				break;
			  case KESKIPAIVA:
				{
				  starthour = 11;
				  endhour = 13;
				}
				break;
			  case ILTAPAIVA:
				{
				  starthour = 13;
				  endhour = 18;
				}
				break;
			  case PITKA_ILTAPAIVA:
				{
				  starthour = 12;
				  endhour = 18;
				}
				break;
			  case ILTA:
				{
				  starthour = 18;
				  endhour = 22;
				}
				break;
			  case ILTAYO:
				{
				  starthour = 22;
				  endhour = 0;
				}
				break;
			  case KESKIYO:
				{
				  starthour = 0;
				  endhour = 3;
				}
				break;
			  case AAMUYO:
				{
				  starthour = 3;
				  endhour = 6;
				}
				break;
			  case PAIVA:
				{
				  starthour = 9;
				  endhour = 18;
				}
				break;
				break;
			  case PITKA_PAIVA:
				{
				  starthour = 6;
				  endhour = 18;
				}
				break;
			  case YO:
				{
				  starthour = 0;
				  endhour = 6;
				}
				break;
			  case PITKA_YO:
				{
				  starthour = 22;
				  endhour = 6;
				}
				break;
			  case YOPUOLI:
				{
				  starthour = 18;
				  endhour = 6;
				}
				break;
			  case PAIVAPUOLI:
				{
				  starthour = 6;
				  endhour = 18;
				}
				break;
			  }
			if(starthour != -1 && endhour != -1)
			  {
				if(weatherPeriod.localStartTime().GetHour() >= starthour &&
				   weatherPeriod.localEndTime().GetHour() <= endhour)
				  theResultVector.push_back((*theCompleteDataVector)[i]);
			  }
		  }
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
  /*
  const void log_weather_result_data_item(MessageLogger& theLog, 
										  const WeatherResultDataItem& theWeatherResultDataItem)
  {
	theLog << theWeatherResultDataItem.thePeriod.localStartTime()
		   << " ... "
		   << theWeatherResultDataItem.thePeriod.localEndTime()
		   << ": "
		   << theWeatherResultDataItem.theResult
		   << endl;
  }
  */
  const void log_weather_result_time_series(MessageLogger& theLog, 
											const std::string& theLogMessage, 
											const weather_result_data_item_vector& theTimeSeries)
  {
	theLog << NFmiStringTools::Convert(theLogMessage) << endl;

	for(unsigned int i = 0; i < theTimeSeries.size(); i++)
	  {
		//		const WeatherResultDataItem& theWeatherResultDataItem = *theTimeSeries[i]; 
		//log_weather_result_data_item(theLog, *theTimeSeries[i]);
		theLog << *theTimeSeries[i];
	  } 
  }

  void log_weather_result_data(MessageLogger& theLog, 
							   weather_forecast_result_container& theDataContainer)
  {
	std::string theLogMessage;
	weather_result_data_item_vector* timeSeries = 0;
	for(unsigned int i = CLOUDINESS_DATA; i < UNDEFINED_DATA_ID; i++)
	  {
		timeSeries = theDataContainer[i];
		if(i == PRECIPITATION_DATA)
		  theLogMessage = "*** precipitation ****";
		else if(i == PRECIPITATION_EXTENT_DATA)
		  theLogMessage = "*** precipitation extent ****";
		else if(i == PRECIPITATION_TYPE_DATA)
		  theLogMessage = "*** precipitation type ****";
		else if(i == CLOUDINESS_DATA)
		  theLogMessage = "*** cloudiness ****";
		else if(i == PRECIPITATION_FORM_WATER_DATA)
		  theLogMessage = "*** precipitation form water ****";
		else if(i == PRECIPITATION_FORM_DRIZZLE_DATA)
		  theLogMessage = "*** precipitation form drizzle ****";
		else if(i == PRECIPITATION_FORM_SLEET_DATA)
		  theLogMessage = "*** precipitation form sleet ****";
		else if(i == PRECIPITATION_FORM_SNOW_DATA)
		  theLogMessage = "*** precipitation form snow ****";
		else if(i == PRECIPITATION_FORM_FREEZING_DATA)
		  theLogMessage = "*** precipitation form freezing ****";
		else if(i == THUNDER_PROBABILITY_DATA)
		  theLogMessage = "*** thunder probability ****";
		else if(i == FOG_INTENSITY_MODERATE_DATA)
		  theLogMessage = "*** fog intensity moderate ****";
		else if(i == FOG_INTENSITY_DENSE_DATA)
		  theLogMessage = "*** fog intensity dense ****";
		else if(i == PRECIPITATION_NORTHEAST_SHARE_DATA)
		  theLogMessage = "*** precipitation share northeast ****";
		else if(i == PRECIPITATION_SOUTHEAST_SHARE_DATA)
		  theLogMessage = "*** precipitation share southeast ****";
		else if(i == PRECIPITATION_SOUTHWEST_SHARE_DATA)
		  theLogMessage = "*** precipitation share southwest ****";
		else if(i == PRECIPITATION_NORTHWEST_SHARE_DATA)
		  theLogMessage = "*** precipitation share northwest ****";
		else if(i == PRECIPITATION_POINT_DATA)
		  theLogMessage = "*** precipitation point ****";
		else if(i == CLOUDINESS_NORTHEAST_SHARE_DATA)
		  theLogMessage = "*** cloudiness share northeast ****";
		else if(i == CLOUDINESS_SOUTHEAST_SHARE_DATA)
		  theLogMessage = "*** cloudiness share southeast ****";
		else if(i == CLOUDINESS_SOUTHWEST_SHARE_DATA)
		  theLogMessage = "*** cloudiness share southwest ****";
		else if(i == CLOUDINESS_NORTHWEST_SHARE_DATA)
		  theLogMessage = "*** cloudiness share northwest ****";
		else if(i == THUNDER_NORTHEAST_SHARE_DATA)
		  theLogMessage = "*** thunder share northeast ****";
		else if(i == THUNDER_SOUTHEAST_SHARE_DATA)
		  theLogMessage = "*** thunder share southeast ****";
		else if(i == THUNDER_SOUTHWEST_SHARE_DATA)
		  theLogMessage = "*** thunder share southwest ****";
		else if(i == THUNDER_NORTHWEST_SHARE_DATA)
		  theLogMessage = "*** thunder share northwest ****";
		else if(i == FOG_NORTHEAST_SHARE_DATA)
		  theLogMessage = "*** fog share northeast ****";
		else if(i == FOG_SOUTHEAST_SHARE_DATA)
		  theLogMessage = "*** fog share southeast ****";
		else if(i == FOG_SOUTHWEST_SHARE_DATA)
		  theLogMessage = "*** fog share southwest ****";
		else if(i == FOG_NORTHWEST_SHARE_DATA)
		  theLogMessage = "*** fog share northwest ****";
		else
		  theLogMessage = "*** unknown data ****";

		log_weather_result_time_series(theLog, theLogMessage, *timeSeries);
	  }
  }
  const void log_weather_result_data(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		theParameters.theLog << "**  INLAND AREA DATA   **" << endl;
		log_weather_result_data(theParameters.theLog, *theParameters.theCompleteData[INLAND_AREA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		theParameters.theLog << "**  COASTAL AREA DATA   **" << endl;
		log_weather_result_data(theParameters.theLog, *theParameters.theCompleteData[COASTAL_AREA]);
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		theParameters.theLog << "**  FULL AREA DATA   **" << endl;
		log_weather_result_data(theParameters.theLog, *theParameters.theCompleteData[FULL_AREA]);
	  }
  }

const void log_subperiods(wf_story_params& theParameters)
  {
	NightAndDayPeriodGenerator generator(theParameters.theForecastPeriod, theParameters.theVariable);

	for(unsigned int i = 1; i <= generator.size(); i++ )
	  {
		weather_result_data_item_vector resultVector;
		if(generator.isday(i))
		  {
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											AAMU, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "AAMUPERIODI",
										   resultVector);
			resultVector.clear();

			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											AAMUPAIVA, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "AAMUPÄIVÄPERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											ILTAPAIVA, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "ILTAPÄIVÄPERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											ILTA, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "ILTAPERIODI",
										   resultVector);
			resultVector.clear();

		  }
		else
		  {
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											ILTA, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "ILTAPERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											ILTAYO, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "ILTAYÖPERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											YO, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "YÖPERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											AAMUYO, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "AAMUYÖPERIODI",
										   resultVector);
			resultVector.clear();
		  }
	  }
  }

  void populate_precipitation_time_series(const string& theVariable, 
										  const AnalysisSources& theSources,
										  const WeatherArea& theArea,	
										  weather_forecast_result_container& theHourlyDataContainer)
  {
	GridForecaster theForecaster;

	weather_result_data_item_vector* precipitationHourly = 
	  theHourlyDataContainer[PRECIPITATION_DATA];
	weather_result_data_item_vector* precipitationExtentHourly = 
	  theHourlyDataContainer[PRECIPITATION_EXTENT_DATA];
	weather_result_data_item_vector* precipitationFormWaterHourly = 
	  theHourlyDataContainer[PRECIPITATION_FORM_WATER_DATA];
	weather_result_data_item_vector* precipitationFormDrizzleHourly = 
	  theHourlyDataContainer[PRECIPITATION_FORM_DRIZZLE_DATA];
	weather_result_data_item_vector* precipitationFormSleetHourly = 
	  theHourlyDataContainer[PRECIPITATION_FORM_SLEET_DATA];
	weather_result_data_item_vector* precipitationFormSnowHourly = 
	  theHourlyDataContainer[PRECIPITATION_FORM_SNOW_DATA];
	weather_result_data_item_vector* precipitationFormFreezingHourly = 
	  theHourlyDataContainer[PRECIPITATION_FORM_FREEZING_DATA];
	weather_result_data_item_vector* precipitationTypeHourly = 
	  theHourlyDataContainer[PRECIPITATION_TYPE_DATA];
	weather_result_data_item_vector* precipitationShareNorthEastHourly = 
	  theHourlyDataContainer[PRECIPITATION_NORTHEAST_SHARE_DATA];
	weather_result_data_item_vector* precipitationShareSouthEastHourly = 
	  theHourlyDataContainer[PRECIPITATION_SOUTHEAST_SHARE_DATA];
	weather_result_data_item_vector* precipitationShareSouthWestHourly = 
	  theHourlyDataContainer[PRECIPITATION_SOUTHWEST_SHARE_DATA];
	weather_result_data_item_vector* precipitationShareNorthWestHourly = 
	  theHourlyDataContainer[PRECIPITATION_NORTHWEST_SHARE_DATA];
	weather_result_data_item_vector* precipitationPointHourly = 
	  theHourlyDataContainer[PRECIPITATION_POINT_DATA];

	RangeAcceptor precipitationlimits;
	precipitationlimits.lowerLimit(0.02);
	ValueAcceptor waterfilter;
	waterfilter.value(kTRain);	// 1 = water
	//RangeAcceptor percentagelimits;
	//percentagelimits.lowerLimit(maxprecipitationlimit);
	ValueAcceptor drizzlefilter;
	drizzlefilter.value(kTDrizzle);	// 0 = drizzle
	ValueAcceptor sleetfilter;
	sleetfilter.value(kTSleet);	// 2 = sleet
	ValueAcceptor snowfilter;
	snowfilter.value(kTSnow);	// 3 = snow
	ValueAcceptor freezingfilter;
	freezingfilter.value(kTFreezingDrizzle);	// 4 = freezing
	ValueAcceptor showerfilter;
	showerfilter.value(2);	// 1=large scale, 2=showers

	//ut << "  ALKU   " << endl;

	for(unsigned int i = 0; i < precipitationHourly->size(); i++)
	  {
        (*precipitationHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								Precipitation,
								Mean,
								Sum,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								precipitationlimits);

        (*precipitationExtentHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								Precipitation,
								Percentage,
								Sum,
								theArea,
								(*precipitationExtentHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								precipitationlimits);

		(*precipitationFormWaterHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								waterfilter);

		(*precipitationFormDrizzleHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								drizzlefilter);

		(*precipitationFormSleetHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								sleetfilter);

		(*precipitationFormSnowHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								snowfilter);

		(*precipitationFormFreezingHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								freezingfilter);

		(*precipitationTypeHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationType,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								showerfilter);

		NFmiPoint precipitationPoint = 
		  AreaTools::getArealDistribution(theSources,
										  Precipitation,
										  theArea,
										  (*precipitationShareNorthEastHourly)[i]->thePeriod,
										  precipitationlimits,
										  (*precipitationShareNorthEastHourly)[i]->theResult,
										  (*precipitationShareSouthEastHourly)[i]->theResult,
										  (*precipitationShareSouthWestHourly)[i]->theResult,
										  (*precipitationShareNorthWestHourly)[i]->theResult);

		// lets store lat/lon pair into WeatherResult
		WeatherResult precipitationPointResult(precipitationPoint.X(), precipitationPoint.Y());
		(*precipitationPointHourly)[i]->theResult = precipitationPointResult;

	  }
  }

  void populate_precipitation_time_series(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		WeatherArea inlandArea = theParameters.theArea;
		inlandArea.type(WeatherArea::Inland);
		populate_precipitation_time_series(theParameters.theVariable,
										   theParameters.theSources,
										   inlandArea,
										   *theParameters.theCompleteData[INLAND_AREA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_precipitation_time_series(theParameters.theVariable,
										   theParameters.theSources,
										   coastalArea,
										   *theParameters.theCompleteData[COASTAL_AREA]);
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_precipitation_time_series(theParameters.theVariable,
										   theParameters.theSources,
										   theParameters.theArea,
										   *theParameters.theCompleteData[FULL_AREA]);
	  }
  }

  void populate_thunderprobability_time_series(const string& theVariable, 
											   const AnalysisSources& theSources,
											   const WeatherArea& theArea,										
											   weather_forecast_result_container& theHourlyDataContainer)
  {

	weather_result_data_item_vector& thunderProbabilityHourly = 
	  *(theHourlyDataContainer[THUNDER_PROBABILITY_DATA]);
	weather_result_data_item_vector* thunderNorthEastHourly = 
	  theHourlyDataContainer[THUNDER_NORTHEAST_SHARE_DATA];
	weather_result_data_item_vector* thunderSouthEastHourly = 
	  theHourlyDataContainer[THUNDER_SOUTHEAST_SHARE_DATA];
	weather_result_data_item_vector* thunderSouthWestHourly = 
	  theHourlyDataContainer[THUNDER_SOUTHWEST_SHARE_DATA];
	weather_result_data_item_vector* thunderNorthWestHourly = 
	  theHourlyDataContainer[THUNDER_NORTHWEST_SHARE_DATA];
	  
 	GridForecaster theForecaster;

	for(unsigned int i = 0; i < thunderProbabilityHourly.size(); i++)
	  {
		thunderProbabilityHourly[i]->theResult = theForecaster.analyze(theVariable,
																	   theSources,
																	   Thunder,
																	   Maximum,
																	   Maximum,
																	   theArea,
																	   thunderProbabilityHourly[i]->thePeriod);

		RangeAcceptor thunderlimits;
		thunderlimits.lowerLimit(SMALL_PROBABILITY_FOR_THUNDER_LOWER_LIMIT);
		AreaTools::getArealDistribution(theSources,
										Thunder,
										theArea,
										(*thunderNorthEastHourly)[i]->thePeriod,
										thunderlimits,
										(*thunderNorthEastHourly)[i]->theResult,
										(*thunderSouthEastHourly)[i]->theResult,
										(*thunderSouthWestHourly)[i]->theResult,
										(*thunderNorthWestHourly)[i]->theResult);
	  }
  }

 void populate_thunderprobability_time_series(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		WeatherArea inlandArea = theParameters.theArea;
		inlandArea.type(WeatherArea::Inland);
		populate_thunderprobability_time_series(theParameters.theVariable,
												theParameters.theSources,
												inlandArea,
												*theParameters.theCompleteData[INLAND_AREA]);
	  }

	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_thunderprobability_time_series(theParameters.theVariable,
												theParameters.theSources,
												coastalArea,
												*theParameters.theCompleteData[COASTAL_AREA]);
	  }

	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_thunderprobability_time_series(theParameters.theVariable,
												theParameters.theSources,
												theParameters.theArea,
												*theParameters.theCompleteData[FULL_AREA]);
	  }
  }



  void populate_fogintensity_time_series(const string& theVariable, 
										 const AnalysisSources& theSources,
										 const WeatherArea& theArea,										
										 weather_forecast_result_container& theHourlyDataContainer)
  {

	weather_result_data_item_vector& fogIntensityModerateHourly = 
	  *(theHourlyDataContainer[FOG_INTENSITY_MODERATE_DATA]);
	weather_result_data_item_vector& fogIntensityDenseHourly = 
	  *(theHourlyDataContainer[FOG_INTENSITY_DENSE_DATA]);
	weather_result_data_item_vector* fogNorthEastHourly = 
	  theHourlyDataContainer[FOG_NORTHEAST_SHARE_DATA];
	weather_result_data_item_vector* fogSouthEastHourly = 
	  theHourlyDataContainer[FOG_SOUTHEAST_SHARE_DATA];
	weather_result_data_item_vector* fogSouthWestHourly = 
	  theHourlyDataContainer[FOG_SOUTHWEST_SHARE_DATA];
	weather_result_data_item_vector* fogNorthWestHourly = 
	  theHourlyDataContainer[FOG_NORTHWEST_SHARE_DATA];
	  
 	GridForecaster theForecaster;
	ValueAcceptor moderateFogFilter;
	moderateFogFilter.value(kTModerateFog);
	ValueAcceptor denseFogFilter;
	denseFogFilter.value(kTDenseFog);

	for(unsigned int i = 0; i < fogIntensityModerateHourly.size(); i++)
	  {
        fogIntensityModerateHourly[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								Fog,
								Mean,
								Percentage,
								theArea,
								fogIntensityModerateHourly[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								moderateFogFilter);

        fogIntensityDenseHourly[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								Fog,
								Mean,
								Percentage,
								theArea,
								fogIntensityDenseHourly[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								denseFogFilter);

		RangeAcceptor foglimits;
		foglimits.lowerLimit(kTModerateFog);
		foglimits.lowerLimit(kTDenseFog);
		AreaTools::getArealDistribution(theSources,
										Fog,
										theArea,
										(*fogNorthEastHourly)[i]->thePeriod,
										foglimits,
										(*fogNorthEastHourly)[i]->theResult,
										(*fogSouthEastHourly)[i]->theResult,
										(*fogSouthWestHourly)[i]->theResult,
										(*fogNorthWestHourly)[i]->theResult);
	  }
  }

 void populate_fogintensity_time_series(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		WeatherArea inlandArea = theParameters.theArea;
		inlandArea.type(WeatherArea::Inland);
		populate_fogintensity_time_series(theParameters.theVariable,
										  theParameters.theSources,
										  inlandArea,
										  *theParameters.theCompleteData[INLAND_AREA]);
	  }

	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_fogintensity_time_series(theParameters.theVariable,
										  theParameters.theSources,
										  coastalArea,
										  *theParameters.theCompleteData[COASTAL_AREA]);
	  }

	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_fogintensity_time_series(theParameters.theVariable,
										  theParameters.theSources,
										  theParameters.theArea,
										  *theParameters.theCompleteData[FULL_AREA]);
	  }
  }




  void populate_cloudiness_time_series(const string& theVariable, 
										const AnalysisSources& theSources,
										const WeatherArea& theArea,										
										 weather_forecast_result_container& theHourlyDataContainer)
  {
	weather_result_data_item_vector& cloudinessHourly = 
	  *(theHourlyDataContainer[CLOUDINESS_DATA]);
	weather_result_data_item_vector* cloudinessNorthEastHourly = 
	  theHourlyDataContainer[CLOUDINESS_NORTHEAST_SHARE_DATA];
	weather_result_data_item_vector* cloudinessSouthEastHourly = 
	  theHourlyDataContainer[CLOUDINESS_SOUTHEAST_SHARE_DATA];
	weather_result_data_item_vector* cloudinessSouthWestHourly = 
	  theHourlyDataContainer[CLOUDINESS_SOUTHWEST_SHARE_DATA];
	weather_result_data_item_vector* cloudinessNorthWestHourly = 
	  theHourlyDataContainer[CLOUDINESS_NORTHWEST_SHARE_DATA];
	
 	GridForecaster theForecaster;
	for(unsigned int i = 0; i < cloudinessHourly.size(); i++)
	  {
		cloudinessHourly[i]->theResult = theForecaster.analyze(theVariable,
															   theSources,
															   Cloudiness,
															   Maximum,
															   Mean,
															   theArea,
															   cloudinessHourly[i]->thePeriod);

		RangeAcceptor cloudinesslimits;
		cloudinesslimits.lowerLimit(VERRATTAIN_PILVISTA_LOWER_LIMIT);
		AreaTools::getArealDistribution(theSources,
										Cloudiness,
										theArea,
										(*cloudinessNorthEastHourly)[i]->thePeriod,
										cloudinesslimits,
										(*cloudinessNorthEastHourly)[i]->theResult,
										(*cloudinessSouthEastHourly)[i]->theResult,
										(*cloudinessSouthWestHourly)[i]->theResult,
										(*cloudinessNorthWestHourly)[i]->theResult);
	  }
  }

  void populate_cloudiness_time_series(wf_story_params& theParameters)
  {
	// theParameters.theVariable+"::fake::"+hourstr+"::cloudiness",
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		WeatherArea inlandArea = theParameters.theArea;
		inlandArea.type(WeatherArea::Inland);
		populate_cloudiness_time_series(theParameters.theVariable,
										theParameters.theSources,
										inlandArea,
										*theParameters.theCompleteData[INLAND_AREA]);
	  }

	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_cloudiness_time_series(theParameters.theVariable,
										theParameters.theSources,
										coastalArea,
										*theParameters.theCompleteData[COASTAL_AREA]);
	  }

	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_cloudiness_time_series(theParameters.theVariable,
										theParameters.theSources,
										theParameters.theArea,
										*theParameters.theCompleteData[FULL_AREA]);
	  }
  }


  void allocate_data_structures(wf_story_params& theParameters, const forecast_area_id& theAreaId)
  {
	weather_result_data_item_vector* hourlyPrecipitation = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationExtent = new weather_result_data_item_vector();

	weather_result_data_item_vector* hourlyPrecipitationType = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormWater = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormDrizzle = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormSleet = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormSnow = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormFreezing = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyCloudiness = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyThunderProbability = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyFogIntensityModerate = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyFogIntensityDense = new weather_result_data_item_vector();

	weather_result_data_item_vector* hourlyPrecipitationShareNortEast = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationShareSouthEast = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationShareSouthWest = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationShareNorthWest = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationPoint = new weather_result_data_item_vector();

	weather_result_data_item_vector* hourlyCloudinessShareNortEast = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyCloudinessShareSouthEast = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyCloudinessShareSouthWest = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyCloudinessShareNorthWest = new weather_result_data_item_vector();

	weather_result_data_item_vector* hourlyThunderProbabilityShareNortEast = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyThunderProbabilityShareSouthEast = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyThunderProbabilityShareSouthWest = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyThunderProbabilityShareNorthWest = new weather_result_data_item_vector();

	weather_result_data_item_vector* hourlyFogShareNortEast = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyFogShareSouthEast = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyFogShareSouthWest = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyFogShareNorthWest = new weather_result_data_item_vector();

	// first split the whole period to one-hour subperiods
	NFmiTime periodStartTime = theParameters.theDataPeriod.localStartTime();

	theParameters.theHourPeriodCount = 0;
	while(periodStartTime.IsLessThan(theParameters.theDataPeriod.localEndTime()))
	  {
		NFmiTime periodEndTime = periodStartTime;
		periodEndTime.ChangeByHours(1);
		WeatherPeriod theWeatherPeriod(periodEndTime, periodEndTime);
		WeatherResult theWeatherResult(kFloatMissing, kFloatMissing);

		part_of_the_day_id  partOfTheDayId = get_part_of_the_day_id(theWeatherPeriod);

		hourlyPrecipitation->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																 theWeatherResult, 
																 partOfTheDayId));
		hourlyPrecipitationExtent->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																	   theWeatherResult, 
																	   partOfTheDayId));
		hourlyPrecipitationType->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																	 theWeatherResult, 
																	 partOfTheDayId));
		hourlyPrecipitationFormWater->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																		  theWeatherResult, 
																		  partOfTheDayId));
		hourlyPrecipitationFormDrizzle->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			theWeatherResult, 
																			partOfTheDayId));
		hourlyPrecipitationFormSleet->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																		  theWeatherResult, 
																		  partOfTheDayId));
		hourlyPrecipitationFormSnow->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																		 theWeatherResult, 
																		 partOfTheDayId));
		hourlyPrecipitationFormFreezing->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			 theWeatherResult, 
																			 partOfTheDayId));
		hourlyPrecipitationPoint->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			 theWeatherResult, 
																			 partOfTheDayId));
		hourlyCloudiness->push_back(new WeatherResultDataItem(theWeatherPeriod, 
															  theWeatherResult, 
															  partOfTheDayId));
		hourlyThunderProbability->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																	  theWeatherResult, 
																	  partOfTheDayId));
		hourlyFogIntensityModerate->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																		theWeatherResult, 
																		partOfTheDayId));
		hourlyFogIntensityDense->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																	 theWeatherResult, 
																	 partOfTheDayId));
			  
		hourlyPrecipitationShareNortEast->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			  theWeatherResult, 
																			  partOfTheDayId));
		hourlyPrecipitationShareSouthEast->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			   theWeatherResult, 
																			   partOfTheDayId));
		hourlyPrecipitationShareSouthWest->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			   theWeatherResult, 
																			   partOfTheDayId));
		hourlyPrecipitationShareNorthWest->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			   theWeatherResult, 
																			   partOfTheDayId));
		
		hourlyCloudinessShareNortEast->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																		   theWeatherResult, 
																		   partOfTheDayId));
		hourlyCloudinessShareSouthEast->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			theWeatherResult, 
																			partOfTheDayId));
		hourlyCloudinessShareSouthWest->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			theWeatherResult, 
																			partOfTheDayId));
		hourlyCloudinessShareNorthWest->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			theWeatherResult, 
																			partOfTheDayId));

		hourlyThunderProbabilityShareNortEast->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																				   theWeatherResult, 
																				   partOfTheDayId));
		hourlyThunderProbabilityShareSouthEast->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																					theWeatherResult, 
																					partOfTheDayId));
		hourlyThunderProbabilityShareSouthWest->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																					theWeatherResult, 
																					partOfTheDayId));
		hourlyThunderProbabilityShareNorthWest->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																					theWeatherResult, 
																					partOfTheDayId));

		hourlyFogShareNortEast->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																	theWeatherResult, 
																	partOfTheDayId));
		hourlyFogShareSouthEast->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																	 theWeatherResult, 
																	 partOfTheDayId));
		hourlyFogShareSouthWest->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																	 theWeatherResult, 
																	 partOfTheDayId));
		hourlyFogShareNorthWest->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																	 theWeatherResult, 
																	 partOfTheDayId));

		periodStartTime.ChangeByHours(1);
		theParameters.theHourPeriodCount++;
	  }

	weather_forecast_result_container* resultContainer = new weather_forecast_result_container();

	resultContainer->insert(make_pair(PRECIPITATION_DATA, hourlyPrecipitation));
	resultContainer->insert(make_pair(PRECIPITATION_EXTENT_DATA, hourlyPrecipitationExtent));
	resultContainer->insert(make_pair(PRECIPITATION_TYPE_DATA, hourlyPrecipitationType));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_WATER_DATA, hourlyPrecipitationFormWater));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_DRIZZLE_DATA, hourlyPrecipitationFormDrizzle));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_SLEET_DATA, hourlyPrecipitationFormSleet));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_SNOW_DATA, hourlyPrecipitationFormSnow));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_FREEZING_DATA, hourlyPrecipitationFormFreezing));
	resultContainer->insert(make_pair(CLOUDINESS_DATA, hourlyCloudiness));
	resultContainer->insert(make_pair(THUNDER_PROBABILITY_DATA, hourlyThunderProbability));
	resultContainer->insert(make_pair(FOG_INTENSITY_MODERATE_DATA, hourlyFogIntensityModerate));
	resultContainer->insert(make_pair(FOG_INTENSITY_DENSE_DATA, hourlyFogIntensityDense));
	resultContainer->insert(make_pair(PRECIPITATION_NORTHEAST_SHARE_DATA, hourlyPrecipitationShareNortEast));
	resultContainer->insert(make_pair(PRECIPITATION_SOUTHEAST_SHARE_DATA, hourlyPrecipitationShareSouthEast));
	resultContainer->insert(make_pair(PRECIPITATION_SOUTHWEST_SHARE_DATA, hourlyPrecipitationShareSouthWest));
	resultContainer->insert(make_pair(PRECIPITATION_NORTHWEST_SHARE_DATA, hourlyPrecipitationShareNorthWest));
	resultContainer->insert(make_pair(PRECIPITATION_POINT_DATA, hourlyPrecipitationPoint));
	resultContainer->insert(make_pair(CLOUDINESS_NORTHEAST_SHARE_DATA, hourlyCloudinessShareNortEast));
	resultContainer->insert(make_pair(CLOUDINESS_SOUTHEAST_SHARE_DATA, hourlyCloudinessShareSouthEast));
	resultContainer->insert(make_pair(CLOUDINESS_SOUTHWEST_SHARE_DATA, hourlyCloudinessShareSouthWest));
	resultContainer->insert(make_pair(CLOUDINESS_NORTHWEST_SHARE_DATA, hourlyCloudinessShareNorthWest));
	resultContainer->insert(make_pair(THUNDER_NORTHEAST_SHARE_DATA, hourlyThunderProbabilityShareNortEast));
	resultContainer->insert(make_pair(THUNDER_SOUTHEAST_SHARE_DATA, hourlyThunderProbabilityShareSouthEast));
	resultContainer->insert(make_pair(THUNDER_SOUTHWEST_SHARE_DATA, hourlyThunderProbabilityShareSouthWest));
	resultContainer->insert(make_pair(THUNDER_NORTHWEST_SHARE_DATA, hourlyThunderProbabilityShareNorthWest));
	resultContainer->insert(make_pair(FOG_NORTHEAST_SHARE_DATA, hourlyFogShareNortEast));
	resultContainer->insert(make_pair(FOG_SOUTHEAST_SHARE_DATA, hourlyFogShareSouthEast));
	resultContainer->insert(make_pair(FOG_SOUTHWEST_SHARE_DATA, hourlyFogShareSouthWest));
	resultContainer->insert(make_pair(FOG_NORTHWEST_SHARE_DATA, hourlyFogShareNorthWest));

	theParameters.theCompleteData.insert(make_pair(theAreaId, resultContainer));
  }

  void create_data_structures(wf_story_params& theParameters)
  {
	// first check which areas exists and create the data structures accordinly
	WeatherArea fullArea = theParameters.theArea;
	WeatherArea inlandArea = theParameters.theArea;
	inlandArea.type(WeatherArea::Inland);
	WeatherArea coastalArea = theParameters.theArea;
	coastalArea.type(WeatherArea::Coast);
	bool inlandExists = false;
	bool coastExists = false;
 	GridForecaster theForecaster;
	WeatherResult result = theForecaster.analyze(theParameters.theVariable,
												 theParameters.theSources,
												 Temperature,
												 Mean,
												 Maximum,
												 inlandArea,
												 theParameters.theForecastPeriod);
	if(result.value() != kFloatMissing)
	  {
		inlandExists = true;
	  }
	
	result = theForecaster.analyze(theParameters.theVariable,
								   theParameters.theSources,
								   Temperature,
								   Mean,
								   Maximum,
								   coastalArea,
								   theParameters.theForecastPeriod);
	if(result.value() != kFloatMissing)
	  {
		coastExists = true;
	  }

	if(inlandExists && coastExists)
	  {
		// create data structures for inland, coast and full areas
		allocate_data_structures(theParameters, INLAND_AREA);
		allocate_data_structures(theParameters, COASTAL_AREA);
		allocate_data_structures(theParameters, FULL_AREA);
		theParameters.theForecastArea |= INLAND_AREA;
		theParameters.theForecastArea |= COASTAL_AREA;
		theParameters.theForecastArea |= FULL_AREA;

		theParameters.theLog << "inland area included" << endl;
		theParameters.theLog << "coastal area included" << endl;
		theParameters.theLog << "full area included" << endl;

	  }
	else if(inlandExists)
	  {
		// create data structures for inland only
		allocate_data_structures(theParameters, INLAND_AREA);
		theParameters.theForecastArea |= INLAND_AREA;
		theParameters.theLog << "inland area included" << endl;
	  }
	else if(coastExists)
	  {
		// create data structures for coast only
		allocate_data_structures(theParameters, COASTAL_AREA);
		theParameters.theForecastArea |= COASTAL_AREA;
		theParameters.theLog << "coastal area included" << endl;
	  }
	else
	  {
		// error not inland, nor coastal area included
	  }
  }

  void deallocate_data_structure(unsigned int& thePeriodCount, weather_forecast_result_container& theResultContainer)
  {
	for(unsigned int i = 0; i < thePeriodCount; i++)
	  {
		delete (*theResultContainer[CLOUDINESS_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_EXTENT_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_TYPE_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_WATER_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_DRIZZLE_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_SLEET_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_SNOW_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_FREEZING_DATA])[i];
		delete (*theResultContainer[THUNDER_PROBABILITY_DATA])[i];
		delete (*theResultContainer[FOG_INTENSITY_MODERATE_DATA])[i];
	  }

	theResultContainer[PRECIPITATION_DATA]->clear();
	theResultContainer[PRECIPITATION_EXTENT_DATA]->clear();
	theResultContainer[PRECIPITATION_TYPE_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_WATER_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_DRIZZLE_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_SLEET_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_SNOW_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_FREEZING_DATA]->clear();
	theResultContainer[THUNDER_PROBABILITY_DATA]->clear();
	theResultContainer[FOG_INTENSITY_MODERATE_DATA]->clear();
	theResultContainer[FOG_INTENSITY_DENSE_DATA]->clear();

	delete theResultContainer[CLOUDINESS_DATA];
	delete theResultContainer[CLOUDINESS_NORTHEAST_SHARE_DATA];
	delete theResultContainer[CLOUDINESS_SOUTHEAST_SHARE_DATA];
	delete theResultContainer[CLOUDINESS_SOUTHWEST_SHARE_DATA];
	delete theResultContainer[CLOUDINESS_NORTHWEST_SHARE_DATA];
	delete theResultContainer[PRECIPITATION_DATA];
	delete theResultContainer[PRECIPITATION_EXTENT_DATA];
	delete theResultContainer[PRECIPITATION_TYPE_DATA];
	delete theResultContainer[PRECIPITATION_FORM_WATER_DATA];
	delete theResultContainer[PRECIPITATION_FORM_DRIZZLE_DATA];
	delete theResultContainer[PRECIPITATION_FORM_SLEET_DATA];
	delete theResultContainer[PRECIPITATION_FORM_SNOW_DATA];
	delete theResultContainer[PRECIPITATION_FORM_FREEZING_DATA];
	delete theResultContainer[PRECIPITATION_NORTHEAST_SHARE_DATA];
	delete theResultContainer[PRECIPITATION_SOUTHEAST_SHARE_DATA];
	delete theResultContainer[PRECIPITATION_SOUTHWEST_SHARE_DATA];
	delete theResultContainer[PRECIPITATION_NORTHWEST_SHARE_DATA];
	delete theResultContainer[PRECIPITATION_POINT_DATA];
	delete theResultContainer[THUNDER_PROBABILITY_DATA];
	delete theResultContainer[THUNDER_NORTHEAST_SHARE_DATA];
	delete theResultContainer[THUNDER_SOUTHEAST_SHARE_DATA];
	delete theResultContainer[THUNDER_SOUTHWEST_SHARE_DATA];
	delete theResultContainer[THUNDER_NORTHWEST_SHARE_DATA];
	delete theResultContainer[FOG_INTENSITY_MODERATE_DATA];
	delete theResultContainer[FOG_INTENSITY_DENSE_DATA];
	delete theResultContainer[FOG_NORTHEAST_SHARE_DATA];
	delete theResultContainer[FOG_SOUTHEAST_SHARE_DATA];
	delete theResultContainer[FOG_SOUTHWEST_SHARE_DATA];
	delete theResultContainer[FOG_NORTHWEST_SHARE_DATA];

	theResultContainer.clear();
  }

  void delete_data_structures(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		deallocate_data_structure(theParameters.theHourPeriodCount, *theParameters.theCompleteData[INLAND_AREA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		deallocate_data_structure(theParameters.theHourPeriodCount, *theParameters.theCompleteData[COASTAL_AREA]);
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		deallocate_data_structure(theParameters.theHourPeriodCount, *theParameters.theCompleteData[FULL_AREA]);
	  }

	for(unsigned int i = 0; i < theParameters.theCloudinessData.size(); i++)
	  {
		cloudiness_data_item_container* cloudinessDataItemContainer = theParameters.theCloudinessData[i];
		for(unsigned int k = 0; k < cloudinessDataItemContainer->size(); k++)
		  delete (*cloudinessDataItemContainer)[k];
		cloudinessDataItemContainer->clear();
	  }
	theParameters.theCloudinessData.clear();
	for(unsigned int i = 0; i < theParameters.thePrecipitationData.size(); i++)
	  {
		precipitation_data_item_container* precipitationDataItemContainer = theParameters.thePrecipitationData[i];
		for(unsigned int k = 0; k < precipitationDataItemContainer->size(); k++)
		  delete (*precipitationDataItemContainer)[k];
		precipitationDataItemContainer->clear();
	  }
	theParameters.thePrecipitationData.clear();

	for(unsigned int i = 0; i < theParameters.theThunderData.size(); i++)
	  {
		thunder_data_item_container* thunderDataItemContainer = theParameters.theThunderData[i];
		for(unsigned int k = 0; k < thunderDataItemContainer->size(); k++)
		  delete (*thunderDataItemContainer)[k];
		thunderDataItemContainer->clear();
	  }
	theParameters.theFogData.clear();
	for(unsigned int i = 0; i < theParameters.theFogData.size(); i++)
	  {
		fog_data_item_container* fogIntensityDataItemContainer = theParameters.theFogData[i];
		for(unsigned int k = 0; k < fogIntensityDataItemContainer->size(); k++)
		  delete (*fogIntensityDataItemContainer)[k];
		fogIntensityDataItemContainer->clear();
	  }
	theParameters.theFogData.clear();
  }

  void init_parameters(wf_story_params& theParameters)
  {
	create_data_structures(theParameters);

	theParameters.theDryWeatherLimitWater =
	  Settings::optional_double(theParameters.theVariable + "::dry_weather_limit_water", DRY_WEATHER_LIMIT_WATER);
	theParameters.theDryWeatherLimitDrizzle =
	  Settings::optional_double(theParameters.theVariable + "::dry_weather_limit_drizzle", DRY_WEATHER_LIMIT_DRIZZLE);
	theParameters.theDryWeatherLimitSleet =
	  Settings::optional_double(theParameters.theVariable + "::dry_weather_limit_sleet", DRY_WEATHER_LIMIT_SLEET);
	theParameters.theDryWeatherLimitSnow =
	  Settings::optional_double(theParameters.theVariable + "::dry_weather_limit_snow", DRY_WEATHER_LIMIT_SNOW);
	theParameters.theWeakPrecipitationLimitWater =
	  Settings::optional_double(theParameters.theVariable + "::weak_precipitation_limit_water", WEAK_PRECIPITATION_LIMIT_WATER);
	theParameters.theWeakPrecipitationLimitSleet =
	  Settings::optional_double(theParameters.theVariable + "::weak_precipitation_limit_sleet", WEAK_PRECIPITATION_LIMIT_SLEET);
	theParameters.theWeakPrecipitationLimitSnow =
	  Settings::optional_double(theParameters.theVariable + "::weak_precipitation_limit_snow", WEAK_PRECIPITATION_LIMIT_SNOW);
	theParameters.theHeavyPrecipitationLimitWater =
	  Settings::optional_double(theParameters.theVariable + "::heavy_precipitation_limit_water", HEAVY_PRECIPITATION_LIMIT_WATER);
	theParameters.theHeavyPrecipitationLimitSleet =
	  Settings::optional_double(theParameters.theVariable + "::heavy_precipitation_limit_sleet", HEAVY_PRECIPITATION_LIMIT_SLEET);
	theParameters.theHeavyPrecipitationLimitSnow =
	  Settings::optional_double(theParameters.theVariable + "::heavy_precipitation_limit_snow", HEAVY_PRECIPITATION_LIMIT_SNOW);
	theParameters.thePrecipitationStormLimit =
	  Settings::optional_double(theParameters.theVariable + "::rainstorm_limit", RAINSTORM_LIMIT);
	theParameters.theMostlyDryWeatherLimit =
	  Settings::optional_double(theParameters.theVariable + "::mostly_dry_weather_limit", MOSTLY_DRY_WEATHER_LIMIT);
	theParameters.theInSomePlacesLowerLimit =
	  Settings::optional_double(theParameters.theVariable + "::in_some_places_lower_limit", IN_SOME_PLACES_LOWER_LIMIT);
	theParameters.theInSomePlacesUpperLimit =
	  Settings::optional_double(theParameters.theVariable + "::in_some_places_upper_limit", IN_SOME_PLACES_UPPER_LIMIT);
	theParameters.theInManyPlacesLowerLimit =
	  Settings::optional_double(theParameters.theVariable + "::in_many_places_lower_limit", IN_MANY_PLACES_LOWER_LIMIT);
	theParameters.theInManyPlacesUpperLimit =
	  Settings::optional_double(theParameters.theVariable + "::in_many_places_upper_limit", IN_MANY_PLACES_UPPER_LIMIT);
	theParameters.theFreezingPrecipitationLimit =
	  Settings::optional_double(theParameters.theVariable + "::freezing_rain_limit", FREEZING_RAIN_LIMIT);
	theParameters.theShowerLimit =
	  Settings::optional_double(theParameters.theVariable + "::shower_limit", SHOWER_LIMIT);
  }

  void check_short_term_precipitation(const vector<WeatherPeriod>& thePrecipitationPeriods,
									  const NFmiTime& theForecastPeriodEndTime,
									  story_part_vector& theStoryParts)
  {
	for(unsigned int i = 0; i < theStoryParts.size(); i++)
	  {
		if(theStoryParts.at(i).second == POUTAANTUU_STORY_PART || 
		   theStoryParts.at(i).second == PILVISTYY_STORY_PART )
		  {
			if(i == theStoryParts.size() - 1)
			  {
				theStoryParts.insert(theStoryParts.begin() + i + 1, 
									 make_pair(WeatherPeriod(theStoryParts.at(i).first.localEndTime(), 
															 theForecastPeriodEndTime), 
											   SHORT_PRECIPITATION_STORY_PART));
			  }
			else
			  {
				theStoryParts.insert(theStoryParts.begin() + i + 1, 
									 make_pair(WeatherPeriod(theStoryParts.at(i).first.localEndTime(), 
															 theStoryParts.at(i+1).first.localStartTime()), 
											   SHORT_PRECIPITATION_STORY_PART));
			  }
		  }
	  }
  }

  void join_trend_id_vectors(const trend_id_vector& theTrendIdVector1,
							 const trend_id_vector& theTrendIdVector2,
							 trend_id_vector& theOutputTrendIdVector)
  {
	unsigned int vector1Index = 0;
	unsigned int vector2Index = 0;
	
	while(1 == 1)
	  {
		NFmiTime trend1Time, trend2Time;
		if(vector1Index < theTrendIdVector1.size())
		  {
			trend1Time = theTrendIdVector1.at(vector1Index).first;
			if(vector2Index < theTrendIdVector2.size())
			  {
				trend2Time = theTrendIdVector2.at(vector2Index).first;
				if(trend2Time <= trend1Time)
				  {
					theOutputTrendIdVector.push_back(theTrendIdVector2.at(vector2Index));
					vector2Index++;
				  }
				else
				  {
					theOutputTrendIdVector.push_back(theTrendIdVector1.at(vector1Index));
					vector1Index++;
				  }
			  }
			else
			  {
				theOutputTrendIdVector.push_back(theTrendIdVector1.at(vector1Index));
				vector1Index++;
			  }
		  }
		else if(vector2Index < theTrendIdVector2.size())
		  {
			theOutputTrendIdVector.push_back(theTrendIdVector2.at(vector2Index));
			vector2Index++;
		  }
		else
		  {
			break;
		  }
	  }
  }

  void construct_story_id_vector(const trend_id_vector& theTrends,
								 const wf_story_params theParameters,
								 const WeatherPeriod& thePeriod,
								 const PrecipitationForecast& thePrecipitationForecast,
								 story_part_vector& theStoryParts)
  {
	unsigned int previous_story_part = MISSING_STORY_PART_ID;
	NFmiTime startTime;
	NFmiTime endTime;
	NFmiTime lastStoryPartEndTime;

	bool firstValidWeatherEvent = true;

	theParameters.theLog << "STORY PERIOD: " << thePeriod.localStartTime() << ".." <<  thePeriod.localEndTime() << endl;

	for(unsigned int i = 0; i < theTrends.size(); i++)
	  {
		if(theTrends.at(i).first < thePeriod.localStartTime() ||
		   theTrends.at(i).first > thePeriod.localEndTime())
		  {
			continue;
		  }

		if(firstValidWeatherEvent)
		  {
			firstValidWeatherEvent = false;

			startTime = (thePeriod.localStartTime());
			endTime = (theTrends.at(i).first);

			theParameters.theLog << startTime << ".. " << endTime;
	
			switch(theTrends.at(i).second)
			  {
			  case SADE_ALKAA:
				{
				  if(endTime.DifferenceInHours(startTime) >= 1)					
					theStoryParts.push_back(make_pair(WeatherPeriod(startTime, endTime), 
													  PILVISYYS_STORY_PART + SADE_STORY_PART));

				  theStoryParts.push_back(make_pair(WeatherPeriod(endTime, endTime), SADE_ALKAA_STORY_PART));
				  theParameters.theLog << " pre SADE_ALKAA: report Precipitation+Cloudiness" << endl;
				  previous_story_part = SADE_ALKAA_STORY_PART;
				  lastStoryPartEndTime = endTime;
				}
				break;
			  case POUTAANTUU:
				{
				  if(endTime.DifferenceInHours(startTime) >= 1)
					theStoryParts.push_back(make_pair(WeatherPeriod(startTime, endTime), SADE_STORY_PART));

				  theStoryParts.push_back(make_pair(WeatherPeriod(endTime, endTime), POUTAANTUU_STORY_PART));
				  theParameters.theLog << " pre POUTAANTUU: report Precipitation" << endl;
				  previous_story_part = POUTAANTUU_STORY_PART;
				  lastStoryPartEndTime = endTime;
				}
				break;
			  case SELKENEE:
				{
				  if(endTime.DifferenceInHours(startTime) >= 1)
					theStoryParts.push_back(make_pair(WeatherPeriod(startTime, endTime), 
													  PILVISYYS_STORY_PART + SADE_STORY_PART));

				  theStoryParts.push_back(make_pair(WeatherPeriod(endTime, endTime), SELKENEE_STORY_PART));
				  theParameters.theLog << " pre SELKENEE: report Precipitation+Cloudiness" << endl;
				  previous_story_part = SELKENEE_STORY_PART;
				  lastStoryPartEndTime = endTime;
				}
				break;
			  case PILVISTYY:
				{
				  if(endTime.DifferenceInHours(startTime) >= 1)
					theStoryParts.push_back(make_pair(WeatherPeriod(startTime, endTime), 
													  PILVISYYS_STORY_PART));
				  theStoryParts.push_back(make_pair(WeatherPeriod(endTime, endTime), PILVISTYY_STORY_PART));
				  theParameters.theLog << " pre PILVISTYY: report Precipitation" << endl;
				  previous_story_part = PILVISTYY_STORY_PART;
				  lastStoryPartEndTime = endTime;
				}
				break;
			  case NO_TREND:
				theParameters.theLog << " pre NO_TREND: report Precipitation" << endl;
				break;
			  }
		  }
		else
		  {
			startTime = (theTrends.at(i-1).first);
			endTime = (theTrends.at(i).first);
			
			if(theTrends.at(i-1).first < thePeriod.localStartTime())
			  startTime = thePeriod.localStartTime();
			if(theTrends.at(i-1).first > thePeriod.localEndTime())
			  endTime = thePeriod.localEndTime();

			if(theTrends.at(i).first > thePeriod.localEndTime())
			  {
				endTime = thePeriod.localEndTime();
				theParameters.theLog << startTime << ".. " << endTime;
				theParameters.theLog << " NO_TRENDS: report Cloudiness, Precipitation" << endl;				
				theStoryParts.push_back(make_pair(WeatherPeriod(startTime, endTime), 
												  PILVISYYS_STORY_PART + SADE_STORY_PART));
				lastStoryPartEndTime = endTime;
				previous_story_part = PILVISYYS_STORY_PART + SADE_STORY_PART;
				break;
			  }

			theParameters.theLog << startTime << ".. " << endTime;
			switch(theTrends.at(i).second)
			  {
			  case SADE_ALKAA:
				{
				  NFmiTime previousPeriodStartTime(startTime);
				  previousPeriodStartTime.ChangeByHours(1);
				  NFmiTime previousPeriodEndTime(endTime);
				  if(previousPeriodEndTime.DifferenceInHours(previousPeriodEndTime) > 1)
					  previousPeriodEndTime.ChangeByHours(-1);
				  if(previous_story_part != POUTAANTUU_STORY_PART)
					  theStoryParts.push_back(make_pair(WeatherPeriod(previousPeriodStartTime, previousPeriodEndTime), 
														PILVISYYS_STORY_PART));
				  theStoryParts.push_back(make_pair(WeatherPeriod(endTime, endTime), 
													SADE_ALKAA_STORY_PART));
				  previous_story_part = SADE_ALKAA_STORY_PART;
				  lastStoryPartEndTime = endTime;
				  theParameters.theLog << " pre SADE_ALKAA: report Precipitation+Cloudiness" << endl;
				}
				break;
			  case POUTAANTUU:
				{
				  NFmiTime endTimeBeforeChange(endTime);
				  if(endTimeBeforeChange > startTime)
					  endTimeBeforeChange.ChangeByHours(-1);
				  if(theStoryParts.size() == 0 || previous_story_part != SADE_ALKAA_STORY_PART)
					theStoryParts.push_back(make_pair(WeatherPeriod(startTime, endTimeBeforeChange), SADE_STORY_PART));
				  
				  theStoryParts.push_back(make_pair(WeatherPeriod(endTime, endTime), 
													POUTAANTUU_STORY_PART));
				  theParameters.theLog << " pre POUTAANTUU: report Precipitation" << endl;
				  previous_story_part = POUTAANTUU_STORY_PART;
				  lastStoryPartEndTime = endTime;
				}
				break;
			  case SELKENEE:
				{
				  NFmiTime endTimeBeforeChange(endTime);
				  if(endTimeBeforeChange > startTime)
					endTimeBeforeChange.ChangeByHours(-1);
				  theStoryParts.push_back(make_pair(WeatherPeriod(endTime, endTime), 
													SELKENEE_STORY_PART));
				  theParameters.theLog << " pre SELKENEE: report Precipitation+Cloudiness" << endl;
				  previous_story_part = SELKENEE_STORY_PART;
				  lastStoryPartEndTime = endTime;
				}
				break;
			  case PILVISTYY:
				{
				  NFmiTime endTimeBeforeChange(endTime);
				  if(endTimeBeforeChange > startTime)
					endTimeBeforeChange.ChangeByHours(-1);
				  theParameters.theLog << " pre PILVISTYY: report Precipitation" << endl;
				  theStoryParts.push_back(make_pair(WeatherPeriod(endTime, endTime), 
													PILVISTYY_STORY_PART));
				  previous_story_part = PILVISTYY_STORY_PART;

				  lastStoryPartEndTime = endTime;
				}
				break;
			  case NO_TREND:
				theParameters.theLog << " pre NO_TREND: report Precipitation" << endl;
				break;
			  }
		  }
	  }

	if(theStoryParts.size() == 0)
	  {
		if(!thePrecipitationForecast.isDryPeriod(thePeriod, theParameters.theForecastArea))
		  {
			theStoryParts.push_back(make_pair(thePeriod, 
											  SADE_STORY_PART));  
		  }
		else
		  {
			theStoryParts.push_back(make_pair(thePeriod, 
											  PILVISYYS_STORY_PART + SADE_STORY_PART));  
		  }
	  }
	else
	  {
		if(previous_story_part == PILVISTYY_STORY_PART &&
		   thePeriod.localEndTime().DifferenceInHours(lastStoryPartEndTime) > 1)
		  {
			lastStoryPartEndTime.ChangeByHours(1);
			theStoryParts.push_back(make_pair(WeatherPeriod(lastStoryPartEndTime, thePeriod.localEndTime()), 
											  SADE_STORY_PART));  
		  }
	  }

	vector<WeatherPeriod> thePrecipitationPeriods;
	thePrecipitationForecast.getPrecipitationPeriods(thePeriod, thePrecipitationPeriods);

	check_short_term_precipitation(thePrecipitationPeriods,
								   thePeriod.localEndTime(),
								   theStoryParts);
  }

  Paragraph weather_forecast_sentence(const story_part_vector& theStoryParts,
									  const wf_story_params theParameters)
  {
	Paragraph paragraph;

	theParameters.theLog << "REPORTING WEATHER FORECAST START" << endl;

	unsigned int previousStoryPartId(MISSING_STORY_PART_ID);

	PrecipitationForecast& precipitationForecast = *theParameters.thePrecipitationForecast;
	CloudinessForecast& cloudinessForecast = *theParameters.theCloudinessForecast;
	FogForecast& fogForecast = *theParameters.theFogForecast;

	for(unsigned int i = 0; i < theStoryParts.size(); i++)
	  {
		unsigned int storyPartId(theStoryParts.at(i).second);
		WeatherPeriod storyPartPeriod(theStoryParts.at(i).first);
		
		if(i > 0)
		  previousStoryPartId = theStoryParts.at(i-1).second;

		switch(storyPartId)
		  {
		  case PILVISTYY_STORY_PART:
			{
			  theParameters.theLog << theStoryParts.at(i).first.localStartTime() 
								   << ": PILVISTYY" << endl;
			  paragraph << cloudinessForecast.cloudinessChangeSentence(storyPartPeriod);
			}
			break;
		  case SELKENEE_STORY_PART:
			{
			  theParameters.theLog << theStoryParts.at(i).first.localStartTime() 
								   << ": SELEKENEE" << endl;
			  paragraph << cloudinessForecast.cloudinessChangeSentence(storyPartPeriod);
			}
			break;
		  case POUTAANTUU_STORY_PART:
			{
			  theParameters.theLog << theStoryParts.at(i).first.localStartTime() 
								   << ": POUTAANTUU" << endl;
			  Sentence sentence;
			  sentence << precipitationForecast.precipitationChangeSentence(storyPartPeriod);
			  // TODO: vaihtelee puolipilvisestä pilviseen?
			  sentence << JA_WORD << ON_WORD;
			  // TODO: length of the storyPartPeriod
			  sentence << cloudinessForecast.cloudinessSentence(storyPartPeriod, true, false);
			  paragraph << sentence;
			}
			break;
		  case SADE_ALKAA_STORY_PART:
			{
			  theParameters.theLog << theStoryParts.at(i).first.localStartTime() 
								   << ": SADE_ALKAA" << endl;
			  paragraph << precipitationForecast.precipitationChangeSentence(storyPartPeriod);
			}
			break;
		  case (PILVISYYS_STORY_PART + SADE_STORY_PART):
			{
			  theParameters.theLog << storyPartPeriod.localStartTime() << ".. " 
								   << storyPartPeriod.localEndTime() 
								   << ": PILVISYYS JA SADE" << endl;

			  Sentence cloudinessSentence;
			  Sentence precipitationSentence;
			  Sentence fogSentence;
			  cloudinessSentence << cloudinessForecast.cloudinessSentence(storyPartPeriod, false, true);
			  precipitationSentence << precipitationForecast.precipitationSentence(storyPartPeriod, false);
			  fogSentence << fogForecast.fogSentence(storyPartPeriod);

			  if(precipitationForecast.isDryPeriod(storyPartPeriod, theParameters.theForecastArea))
				{

				  if(cloudinessForecast.getCloudinessId(storyPartPeriod) == PUOLIPILVINEN_JA_PILVINEN &&
					 !precipitationForecast.shortTermPrecipitationExists(storyPartPeriod))
					{
					  // sää vaihtelee puolipilvisestä pilviseen ja on poutainen
					  Sentence sentence;
					  sentence << cloudinessSentence << JA_WORD;
					  sentence << ON_WORD;
					  sentence << precipitationSentence;
					  paragraph << sentence;
					}
				  else
					{
					  if(!precipitationForecast.shortTermPrecipitationExists(storyPartPeriod)) 
						{
						  // esim. sää on pilvinen ja poutainen
						  Sentence sentence;
						  sentence << cloudinessSentence << JA_WORD;
						  precipitationForecast.useOllaVerb(true);
						  sentence << precipitationForecast.precipitationSentence(storyPartPeriod, false);
						  precipitationForecast.useOllaVerb(false);
						  paragraph << sentence;
						}
					  else
						{
						  paragraph << cloudinessSentence;
						  paragraph << fogSentence;
						  paragraph << precipitationSentence;
						}
					}
				}
			  else
				{
				  paragraph << cloudinessSentence;
				  paragraph << fogSentence;
				  paragraph << precipitationSentence;
				}
			}
			break;
		  case PILVISYYS_STORY_PART:
			{
			  theParameters.theLog << storyPartPeriod.localStartTime() << ".. " 
								   << storyPartPeriod.localEndTime() 
								   << ": PILVISYYS" << endl;
			  
			  paragraph << cloudinessForecast.cloudinessSentence(theStoryParts.at(i).first, false, true);
			  paragraph << fogForecast.fogSentence(theStoryParts.at(i).first);
			}
			break;
		  case SADE_STORY_PART:
			{
			  theParameters.theLog << storyPartPeriod.localStartTime() << ".. " 
								   << storyPartPeriod.localEndTime() 
								   << ": SADE" << endl;
			  Sentence sentence;
			  if(precipitationForecast.isDryPeriod(storyPartPeriod, theParameters.theForecastArea) || 
				 precipitationForecast.isMostlyDryPeriod(storyPartPeriod, theParameters.theForecastArea))
				{
				  sentence << SAA_WORD << ON_WORD;
				}
			  sentence << precipitationForecast.precipitationSentence(storyPartPeriod, false);
			  paragraph << sentence;
			}
			break;
		  case SHORT_PRECIPITATION_STORY_PART:
			{
			  theParameters.theLog << storyPartPeriod.localStartTime() << ".. " 
								   << storyPartPeriod.localEndTime() 
								   << ": SHORT_PRECIPITATION_STORY_PART" << endl;
			  paragraph << precipitationForecast.shortTermPrecipitationSentence(storyPartPeriod);
			}
			break;
		  case MISSING_STORY_PART_ID:
			{
			  theParameters.theLog << storyPartPeriod.localStartTime() << ".. " 
								   << storyPartPeriod.localEndTime() 
								   << ": MISSING_STORY_PART_ID" << endl;
			}
			break;
		  }
	  }

	theParameters.theLog << "REPORTING WEATHER FORECAST END" << endl;

	return paragraph;
  }

// ----------------------------------------------------------------------
  /*!
   * \brief Generate overview on weather
   *
   * \return The story
   *
   * \see page_weather_forecast
   *
   * \todo Much missing
   */
  // ----------------------------------------------------------------------

  const Paragraph WeatherStory::forecast() const
  {
	MessageLogger log("WeatherStory::forecast");

	using namespace PrecipitationPeriodTools;

	Paragraph paragraph;

	// Period generator
	NightAndDayPeriodGenerator generator00(itsPeriod, itsVar);

	if(generator00.size() == 0)
	  {
		log << "No weather periods available!" << endl;
		log << paragraph;
		return paragraph;
	  }

	NFmiTime dataPeriodStartTime(itsPeriod.localStartTime());
	NFmiTime dataPeriodEndTime(itsPeriod.localEndTime());

	log << "period contains ";

	if(generator00.isday(1))
	  {
		if(generator00.size() > 2)
		  {
			log << "today, night and tomorrow, ..." << endl;
		  }
		else if(generator00.size() == 2)
		  {
			log << "today and night" << endl;
		  }
		else
		  {
			log << "today" << endl;
		  }
	  }
	else
	  {
		if(generator00.size() == 1)
		  {
			log << "one night" << endl;
		  }
		else
		  {
			log << "night and tomorrow" << endl;		  
		  }
	  }

	dataPeriodStartTime.ChangeByHours(-12);
	dataPeriodEndTime.ChangeByHours(12);

	/*
	if(generator00.isday(1))
	  {
		if(generator00.size() > 2)
		  {
			log << "today, night and tomorrow, ..." << endl;
		  }
		else if(generator00.size() == 2)
		  {
			log << "today and night" << endl;
			if(abs(itsForecastTime.DifferenceInHours(generator00.period(1).localStartTime())) > 12)
			  dataPeriodStartTime.ChangeByHours(-24);
			else
			  dataPeriodEndTime.ChangeByHours(12);
		  }
		else
		  {
			log << "today" << endl;
			if(abs(itsForecastTime.DifferenceInHours(generator00.period(1).localStartTime())) > 12)
			  dataPeriodStartTime.ChangeByHours(-24);
			else
			  dataPeriodEndTime.ChangeByHours(24);
		  }
	  }
	else
	  {
		if(generator00.size() == 1)
		  {
			log << "one night" << endl;
			dataPeriodStartTime.ChangeByHours(-12);
			dataPeriodEndTime.ChangeByHours(12);
		  }
		else
		  {
			log << "night and tomorrow" << endl;		  
			dataPeriodStartTime.ChangeByHours(-12);
			dataPeriodEndTime.ChangeByHours(12);
		  }
	  }
	*/

	log_start_time_and_end_time(log, 
								"the forecast period: ",
								itsPeriod);
	
	log_start_time_and_end_time(log, 
								"the data gathering period: ",
								WeatherPeriod(dataPeriodStartTime, dataPeriodEndTime));

	Rect areaRect(itsArea);		
	if(itsArea.isNamed())
	  {
		std::string name(itsArea.name());
		log << "** " << name  << " **" << endl;
	  }

	WeatherPeriod theDataGatheringPeriod(dataPeriodStartTime, dataPeriodEndTime);


	wf_story_params theParameters(itsVar,
								  itsArea,
								  theDataGatheringPeriod,
								  itsPeriod,
								  itsForecastTime,
								  itsSources,
								  log);

	init_parameters(theParameters);

	if(theParameters.theForecastArea == NO_AREA)
	  return paragraph;

	populate_precipitation_time_series(theParameters);
	populate_cloudiness_time_series(theParameters);
	populate_fogintensity_time_series(theParameters);
	populate_thunderprobability_time_series(theParameters);

	CloudinessForecast cloudinessForecast(theParameters);
	PrecipitationForecast precipitationForecast(theParameters);
	FogForecast fogForecast(theParameters);
	ThunderForecast thunderForecast(theParameters);

	theParameters.thePrecipitationForecast = &precipitationForecast;
	theParameters.theCloudinessForecast = &cloudinessForecast;
	theParameters.theFogForecast = &fogForecast;
	theParameters.theThunderForecast = &thunderForecast;

	/*
	precipitationForecast.printOutPrecipitationData(log);
	precipitationForecast.printOutPrecipitationPeriods(log);
	precipitationForecast.printOutPrecipitationTrends(log);
	cloudinessForecast.printOutCloudinessTrends(log);
	*/
	trend_id_vector precipitationTrends;
	trend_id_vector cloudinessTrends;
	trend_id_vector joinedTrends;
	vector<WeatherPeriod> precipitationPeriods;

	precipitationForecast.getTrendIdVector(precipitationTrends);
	precipitationForecast.getPrecipitationPeriods(itsPeriod, precipitationPeriods);
	cloudinessForecast.getTrendIdVector(cloudinessTrends);
	join_trend_id_vectors(precipitationTrends, cloudinessTrends, joinedTrends);
	//	join_short_term_precipitation(joinedTrends, precipitationPeriods);

	//	theParameters.theLog << "JOINED TRENDS: "<< endl;
	//theParameters.theLog << itsPeriod.localStartTime() << ".. " << itsPeriod.localEndTime() << endl;
	/*
	print_out_trend_vector(theParameters.theLog, joinedTrends);
	fogForecast.printOutFogPeriods(theParameters.theLog);
	fogForecast.printOutFogTypePeriods(theParameters.theLog);
	*/

	story_part_vector story_parts;

	construct_story_id_vector(joinedTrends, 
							  theParameters,							  
							  itsPeriod, 
							  precipitationForecast, 
							  story_parts);


	paragraph << weather_forecast_sentence(story_parts, 
										   theParameters);

	precipitationForecast.printOutPrecipitationData(log);
	/*
	precipitationForecast.printOutPrecipitationData(log);
	cloudinessForecast.printOutCloudinessData(log);
	fogForecast.printOutFogPeriods(log);
	*/

/*
	theParameters.theLog << "COMBINED SENTENCE: ";
	theParameters.theLog << paragraph;

	cloudinessForecast.printOutCloudinessData(log);
	cloudinessForecast.printOutCloudinessPeriods(log);
	cloudinessForecast.printOutCloudinessTrends(log);

	precipitationForecast.printOutPrecipitationData(log);
	precipitationForecast.printOutPrecipitationPeriods(log);
	precipitationForecast.printOutPrecipitationTrends(log);
	*/

	/*
	Paragraph para;
	para << fogSentence;
	cout << itsArea.name();
	cout << ": ";
	cout << para;
	*/
	//	fogForecast.printOutFogPeriods(log);
	//	fogForecast.printOutFogTypePeriods(log);

	/*																		 
	cloudinessForecast.printOutCloudinessData(log);
	cloudinessForecast.printOutCloudinessPeriods(log);
	cloudinessForecast.printOutCloudinessTrends(log);

	precipitationForecast.printOutPrecipitationData(log);
	precipitationForecast.printOutPrecipitationPeriods(log);
	precipitationForecast.printOutPrecipitationTrends(log);
	//precipitationForecast.printOutPrecipitationDistribution(log);
	*/

	//log_weather_result_data(theParameters);

	//	precipitationForecast.printOutPrecipitationLocation(cout);

	//	precipitationForecast.printOutPrecipitationData(cout);

	delete_data_structures(theParameters);

	log << paragraph;

	const_cast<WeatherHistory&>(itsArea.history()).updateTimePhrase("", NFmiTime(1970,1,1));

	return paragraph;
  }


} // namespace TextGen

// ======================================================================
