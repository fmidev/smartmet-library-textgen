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
#include "NFmiCombinedParam.h"

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
		WeatherPeriod period(theTrendVector.at(i).first.localStartTime(),
							 theTrendVector.at(i).first.localEndTime());
		trend_id trid(theTrendVector.at(i).second);
		theOutput << period.localStartTime()
				  << "..."
				  << period.localEndTime()
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
	for(unsigned int i = TEMPERATURE_DATA; i < UNDEFINED_DATA_ID; i++)
	  {
		timeSeries = theDataContainer[i];
		if(i == TEMPERATURE_DATA)
		  theLogMessage = "*** temperature ****";
		else if(i == PRECIPITATION_DATA)
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
		else if(i == THUNDERPROBABILITY_DATA)
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
		else
		  continue;

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
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

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

  double get_areal_distribution(const AnalysisSources& theSources,
							  const WeatherParameter& theParameter,
							  const WeatherArea& theArea,
							  const WeatherPeriod& thePeriod,
							  const Acceptor& theAcceptor,
							  WeatherResult& theNortEastShare,
							  WeatherResult& theSouthEastShare,
							  WeatherResult& theSouthWestShare,
							  WeatherResult& theNortWestShare)
  {
	NFmiIndexMask indexMask;
	
  	double cumulativeValue = ExtractMask(theSources,
										 theParameter,
										 theArea,
										 thePeriod,
										 theAcceptor,
										 indexMask);

	NFmiPoint averagePoint = AreaTools::getArealDistribution(theSources, 
															 theParameter, 
															 indexMask,
															 theNortEastShare,
															 theSouthEastShare,
															 theSouthWestShare,
															 theNortWestShare);


	return cumulativeValue;
  }



  void populate_temperature_time_series(const string& theVariable, 
										const AnalysisSources& theSources,
										const WeatherArea& theArea,										
										weather_result_data_item_vector& theResultData)
  {
	GridForecaster theForecaster;
	for(unsigned int i = 0; i < theResultData.size(); i++)
	  {
		theResultData[i]->theResult = theForecaster.analyze(theVariable,
															theSources,
															Temperature,
															Mean,
															Maximum,
															theArea,
															theResultData[i]->thePeriod);
	  }
  }

  void populate_temperature_time_series(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		WeatherArea inlandArea = theParameters.theArea;
		inlandArea.type(WeatherArea::Inland);
		populate_temperature_time_series(theParameters.theVariable,
										 theParameters.theSources,
										 inlandArea,
										 *(*theParameters.theCompleteData[INLAND_AREA])[TEMPERATURE_DATA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_temperature_time_series(theParameters.theVariable,
										 theParameters.theSources,
										 coastalArea,
										 *(*theParameters.theCompleteData[COASTAL_AREA])[TEMPERATURE_DATA]);
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_temperature_time_series(theParameters.theVariable,
										 theParameters.theSources,
										 theParameters.theArea,
										 *(*theParameters.theCompleteData[FULL_AREA])[TEMPERATURE_DATA]);
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

	RangeAcceptor precipitationlimits;
	precipitationlimits.lowerLimit(0.001);
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

		get_areal_distribution(theSources,
							   Precipitation,
							   theArea,
							   (*precipitationShareNorthEastHourly)[i]->thePeriod,
							   precipitationlimits,
							   (*precipitationShareNorthEastHourly)[i]->theResult,
							   (*precipitationShareSouthEastHourly)[i]->theResult,
							   (*precipitationShareSouthWestHourly)[i]->theResult,
							   (*precipitationShareNorthWestHourly)[i]->theResult);
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
	  *(theHourlyDataContainer[THUNDERPROBABILITY_DATA]);
	  
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
										weather_result_data_item_vector& theResultData)
  {
 	GridForecaster theForecaster;
	for(unsigned int i = 0; i < theResultData.size(); i++)
	  {
		theResultData[i]->theResult = theForecaster.analyze(theVariable,
															theSources,
															Cloudiness,
															Maximum,
															Mean,
															theArea,
															theResultData[i]->thePeriod);
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
										*(*theParameters.theCompleteData[INLAND_AREA])[CLOUDINESS_DATA]);
	  }

	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_cloudiness_time_series(theParameters.theVariable,
										theParameters.theSources,
										coastalArea,
										*(*theParameters.theCompleteData[COASTAL_AREA])[CLOUDINESS_DATA]);
	  }

	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_cloudiness_time_series(theParameters.theVariable,
										theParameters.theSources,
										theParameters.theArea,
										*(*theParameters.theCompleteData[FULL_AREA])[CLOUDINESS_DATA]);
	  }
  }

  CloudinessDataItemData* get_cloudiness_data_item_data(wf_story_params& theParameters, 
														const WeatherPeriod& thePeriod,
														const forecast_area_id& theForeacastAreaId)
  {
	if(!(theParameters.theForecastArea & theForeacastAreaId))
	  return 0;

	CloudinessDataItemData* item = 0;

	cloudiness_id theCloudinessId(MISSING_CLOUDINESS_ID);

	float min = -1.0;
	float mean = -1.0;
	float max = -1.0;   
	float standard_deviation = -1.0;
	float pearson_coefficient = 0.0;
	
	weather_result_data_item_vector theInterestingData;

	trend_id theTrendId(NO_TREND);

	get_sub_time_series(thePeriod,
						*(*theParameters.theCompleteData[theForeacastAreaId])[CLOUDINESS_DATA],
						theInterestingData);

	if(theInterestingData.size() > 0)
	  {
		mean = get_mean(theInterestingData);
		get_min_max(theInterestingData, min, max);
		standard_deviation = get_standard_deviation(theInterestingData);	
		theCloudinessId = get_cloudiness_id(min, mean, max, standard_deviation);
		pearson_coefficient = get_pearson_coefficient(theInterestingData);
  
		if(theInterestingData.size() > 2)
		  {
			cloudiness_id cloudinessAtStart = 
			  get_cloudiness_id(theInterestingData[0]->theResult.value());
			cloudiness_id cloudinessAtEnd = 
			  get_cloudiness_id(theInterestingData[theInterestingData.size()-1]->theResult.value());

			bool dataOk = (cloudinessAtStart != kFloatMissing && cloudinessAtEnd != kFloatMissing);

			if(dataOk && cloudinessAtStart >= VERRATTAIN_PILVISTA && 
			   cloudinessAtEnd == SELKEAA && 
			   pearson_coefficient < -TREND_CHANGE_COEFFICIENT_TRESHOLD)
			  {
				theTrendId = SELKENEE;
			  }
			else if(dataOk && cloudinessAtStart <= MELKO_SELKEAA && 
					cloudinessAtEnd >= VERRATTAIN_PILVISTA &&
					pearson_coefficient >= TREND_CHANGE_COEFFICIENT_TRESHOLD)
			  {
				theTrendId = PILVISTYY;
			  }
		  }
		item = new CloudinessDataItemData(theCloudinessId, 
										  min, 
										  mean, 
										  max, 
										  standard_deviation, 
										  theTrendId, 
										  pearson_coefficient);
	  }

	return item;


  }

 CloudinessDataItem* get_cloudiness_data_item(wf_story_params& theParameters, const WeatherPeriod& thePeriod)
  {
	CloudinessDataItemData* dataItemCoastal = 
	  get_cloudiness_data_item_data(theParameters, thePeriod, COASTAL_AREA);
	CloudinessDataItemData* dataItemInland = 
	  get_cloudiness_data_item_data(theParameters, thePeriod, INLAND_AREA);
	CloudinessDataItemData* dataItemFull = 
	  get_cloudiness_data_item_data(theParameters, thePeriod, FULL_AREA);

	CloudinessDataItem* item = new CloudinessDataItem(dataItemCoastal, dataItemInland, dataItemFull);

	return item;
  }

  Sentence cloudiness_change_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	Sentence cloudinessSentence;

	cloudiness_data_item_container* theResultData = theParameters.theCloudinessData[thePeriodNumber - 1];

	if(theResultData)
	  {
		trend_id theTrendMorning(NO_TREND);
		trend_id theTrendAfternoon(NO_TREND);
		trend_id theTrendNight(NO_TREND);

		const CloudinessDataItemData* morningDataItem = 
		  theResultData->find(AAMUPAIVA) != theResultData->end() ? ((*theResultData)[AAMUPAIVA])->theFullData : 0;
		const CloudinessDataItemData* afternoonDataItem = 
		  theResultData->find(ILTAPAIVA) != theResultData->end() ? ((*theResultData)[ILTAPAIVA])->theFullData : 0;
		const CloudinessDataItemData* nightDataItem = 
		  theResultData->find(YO) != theResultData->end() ? ((*theResultData)[YO])->theFullData : 0;

		// TODO: Think how coastal and inland cloudiness change should be reported
		if(morningDataItem)
		  theTrendMorning = morningDataItem->theTrendId;
		if(afternoonDataItem)
		  theTrendAfternoon = afternoonDataItem->theTrendId;
		if(nightDataItem)
		  theTrendNight = nightDataItem->theTrendId;

		if(theTrendNight == NO_TREND && theTrendMorning == PILVISTYY && theTrendAfternoon == NO_TREND)
		  sentence << AAMUPAIVALLA_WORD << PILVISTYVAA_WORD;
		else if(theTrendNight == NO_TREND && theTrendMorning == NO_TREND && theTrendAfternoon == PILVISTYY)
		  sentence << ILTAPAIVALLA_WORD << PILVISTYVAA_WORD;
		if(theTrendNight == NO_TREND && theTrendMorning == SELKENEE && theTrendAfternoon == NO_TREND)
		  sentence << AAMUPAIVALLA_WORD << SELKENEVAA_WORD;
		else if(theTrendNight == NO_TREND && theTrendMorning == NO_TREND && theTrendAfternoon == SELKENEE)
		  sentence << ILTAPAIVALLA_WORD << SELKENEVAA_WORD;
	  }

	return sentence;
  }

  Sentence cloudiness_night_sentence(const wf_story_params& theParameters, const CloudinessDataItem& theDataItem)
  {
	Sentence sentence;

	const CloudinessDataItemData* inlandData = theDataItem.theInlandData;
	const CloudinessDataItemData* coastalData = theDataItem.theCoastalData;
	const CloudinessDataItemData* fullData = theDataItem.theFullData;

	cloudiness_id theCloudinessIdInland(inlandData != 0 ? inlandData->theId : MISSING_CLOUDINESS_ID);
	cloudiness_id theCloudinessIdCoastal(coastalData != 0 ? coastalData->theId : MISSING_CLOUDINESS_ID);
	cloudiness_id theCloudinessIdFull(fullData != 0 ? fullData->theId : MISSING_CLOUDINESS_ID);

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		if(theCloudinessIdCoastal != MISSING_CLOUDINESS_ID &&
		   theCloudinessIdInland != MISSING_CLOUDINESS_ID &&
		   (coastalData->theMean <= TAYSIN_PILVETON_LIMIT && inlandData->theMean >= HYVIN_PILVINEN_LIMIT ||
			coastalData->theMean >= HYVIN_PILVINEN_LIMIT && inlandData->theMean <= TAYSIN_PILVETON_LIMIT))
		  {
			sentence << COAST_PHRASE << cloudiness_sentence(theCloudinessIdCoastal);
			sentence << Delimiter(",");
			sentence << INLAND_PHRASE << cloudiness_sentence(theCloudinessIdInland);
		  }
		else
		  {
			sentence << cloudiness_sentence(theCloudinessIdFull);
		  }
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		sentence << cloudiness_sentence(theCloudinessIdInland);
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		sentence << cloudiness_sentence(theCloudinessIdCoastal);
	  }

	return sentence;
  }

  bool has_different_cloudiness(const cloudiness_id& theCloudinessId1, 
								const cloudiness_id& theCloudinessId2, 
								cloudiness_id& theCloudinessIdResult)
  {
	bool retval = false;

	if(abs(theCloudinessId2 - theCloudinessId1) >= 2)
	  {
		theCloudinessIdResult = theCloudinessId1 > theCloudinessId2 ? theCloudinessId1 : theCloudinessId2;
		retval = true;
	  }

	return retval;
  }

  Sentence cloudiness_day_sentence(const wf_story_params& theParameters, 
								   const CloudinessDataItem& theMorningDataItem,
								   const CloudinessDataItem& theAfternoonDataItem)
  {
	Sentence sentence;

	theParameters.theLog << "*Morning Cloudiness" << endl;
	theParameters.theLog << theMorningDataItem;
	theParameters.theLog << "*Afternoon Cloudiness" << endl;
	theParameters.theLog << theAfternoonDataItem;
	
	const CloudinessDataItemData* inlandDataMorning = theMorningDataItem.theInlandData;
	const CloudinessDataItemData* coastalDataMorning = theMorningDataItem.theCoastalData;
	const CloudinessDataItemData* fullDataMorning = theMorningDataItem.theFullData;
	const CloudinessDataItemData* inlandDataAfternoon = theAfternoonDataItem.theInlandData;
	const CloudinessDataItemData* coastalDataAfternoon = theAfternoonDataItem.theCoastalData;
	const CloudinessDataItemData* fullDataAfternoon = theAfternoonDataItem.theFullData;

	cloudiness_id theCloudinessIdFullMorning(fullDataMorning != 0 
											 ? fullDataMorning->theId : MISSING_CLOUDINESS_ID);
	cloudiness_id theCloudinessIdCoastalMorning(coastalDataMorning != 0 
												? coastalDataMorning->theId : MISSING_CLOUDINESS_ID);
	cloudiness_id theCloudinessIdInlandMorning(inlandDataMorning != 0 
												? inlandDataMorning->theId : MISSING_CLOUDINESS_ID);
	cloudiness_id theCloudinessIdFullAfternoon(fullDataAfternoon != 0 
											 ? fullDataAfternoon->theId : MISSING_CLOUDINESS_ID);
	cloudiness_id theCloudinessIdCoastalAfternoon(coastalDataAfternoon != 0
												? coastalDataAfternoon->theId : MISSING_CLOUDINESS_ID);
	cloudiness_id theCloudinessIdInlandAfternoon(inlandDataAfternoon != 0 
												? inlandDataAfternoon->theId : MISSING_CLOUDINESS_ID);


	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		bool separateCoastInlandMorning = (inlandDataMorning->theMean <= 5.0 && 
										   coastalDataMorning->theMean >= 70.0 ||
										   inlandDataMorning->theMean >= 70.0 && 
										   coastalDataMorning->theMean <= 5.0);
		  //(abs(theCloudinessIdCoastalMorning - theCloudinessIdInlandMorning) >= 2);
		bool separateCoastInlandAfternoon = (inlandDataAfternoon->theMean <= 5.0 && 
											 coastalDataAfternoon->theMean >= 7.0 ||
											 inlandDataAfternoon->theMean >= 70.0 && 
											 coastalDataAfternoon->theMean <= 5.0);
		  //(abs(theCloudinessIdCoastalAfternoon - theCloudinessIdInlandAfternoon) >= 2);
		bool separateMorningAfternoon = (abs(theCloudinessIdFullMorning - theCloudinessIdFullAfternoon) >= 2);

		theParameters.theLog << "separateCoastInlandMorning: " << static_cast<int>(separateCoastInlandMorning) << endl;
		theParameters.theLog << "separateCoastInlandAfternoon: " << static_cast<int>(separateCoastInlandAfternoon) << endl;
		theParameters.theLog << "separateMornigAfternoon: " << static_cast<int>(separateMorningAfternoon) << endl;

		if(!separateCoastInlandMorning && !separateCoastInlandAfternoon && !separateMorningAfternoon)
		  {
			sentence << cloudiness_sentence(theCloudinessIdFullMorning);
		  }
		else
		  {
			if(separateCoastInlandMorning)
			  {
				sentence << AAMUPAIVALLA_WORD << COAST_PHRASE
						 << cloudiness_sentence(theCloudinessIdCoastalMorning);
				sentence << Delimiter(",");
				sentence << INLAND_PHRASE << cloudiness_sentence(theCloudinessIdInlandMorning, true);
			  }
			else
			  {
				sentence << AAMUPAIVALLA_WORD << cloudiness_sentence(theCloudinessIdFullMorning);
			  }
			sentence << Delimiter(",");
			if(separateCoastInlandAfternoon)
			  {
				sentence << ILTAPAIVALLA_WORD << COAST_PHRASE
						 << cloudiness_sentence(theCloudinessIdCoastalAfternoon);
				sentence << Delimiter(",");
				sentence << INLAND_PHRASE << cloudiness_sentence(theCloudinessIdInlandAfternoon, true);
			  }
			else
			  {
				sentence << ILTAPAIVALLA_WORD << cloudiness_sentence(theCloudinessIdFullAfternoon);
			  }
		  }
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		if(puolipilvisesta_pilviseen(theCloudinessIdInlandMorning,
									 theCloudinessIdInlandAfternoon))
		  {
			sentence << SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
		  }
		else if(theCloudinessIdInlandMorning != MISSING_CLOUDINESS_ID &&
				theCloudinessIdInlandMorning != PUOLIPILVISTA_JA_PILVISTA && 
				theCloudinessIdInlandAfternoon != PUOLIPILVISTA_JA_PILVISTA &&
				abs(theCloudinessIdInlandMorning - theCloudinessIdInlandAfternoon) < 2)
		  {
			sentence << cloudiness_sentence(theCloudinessIdInlandMorning);
		  }
		else
		  {
			if(theCloudinessIdInlandMorning != MISSING_CLOUDINESS_ID)
			  {
				sentence << AAMUPAIVALLA_WORD << cloudiness_sentence(theCloudinessIdInlandMorning);
				sentence << Delimiter(",");
				sentence << ILTAPAIVALLA_WORD << cloudiness_sentence(theCloudinessIdInlandAfternoon, true);
			  }
			else
			  {
				sentence << cloudiness_sentence(theCloudinessIdInlandAfternoon);
			  }
		  }
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		if(puolipilvisesta_pilviseen(theCloudinessIdCoastalMorning,
									 theCloudinessIdCoastalAfternoon))
		  {
			sentence << SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
		  }
		else if(theCloudinessIdCoastalMorning != MISSING_CLOUDINESS_ID &&
				theCloudinessIdCoastalMorning != PUOLIPILVISTA_JA_PILVISTA && 
				theCloudinessIdCoastalAfternoon != PUOLIPILVISTA_JA_PILVISTA &&
				abs(theCloudinessIdCoastalMorning - theCloudinessIdCoastalAfternoon) < 2)
		  {
			sentence << cloudiness_sentence(theCloudinessIdCoastalMorning);
		  }
		else
		  {
			if(theCloudinessIdCoastalMorning != MISSING_CLOUDINESS_ID)
			  {
				sentence << AAMUPAIVALLA_WORD << cloudiness_sentence(theCloudinessIdCoastalMorning);
				sentence << Delimiter(",");
				sentence << ILTAPAIVALLA_WORD << cloudiness_sentence(theCloudinessIdCoastalAfternoon, true);
			  }
			else
			  {
				sentence << cloudiness_sentence(theCloudinessIdCoastalAfternoon);
			  }
		  }
	  }

	return sentence;
  }
  
  Sentence cloudiness_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	if(generator.size() < thePeriodNumber)
	  return sentence;

	Sentence cloudinessSentence;

	cloudiness_data_item_container* theResultData = theParameters.theCloudinessData[thePeriodNumber - 1];

	if(theResultData)
	  {
		if(generator.isnight(thePeriodNumber))
		  {
			if(theResultData->find(YO) != theResultData->end())
			  sentence << cloudiness_night_sentence(theParameters, *((*theResultData)[YO]));
		  }
		else
		  {
			CloudinessDataItem* morningItem = 0;
			CloudinessDataItem* afternoonItem = 0;
			if(theResultData->find(AAMUPAIVA) != theResultData->end())
			  morningItem = (*theResultData)[AAMUPAIVA];
			if(theResultData->find(ILTAPAIVA) != theResultData->end())
			  afternoonItem = (*theResultData)[ILTAPAIVA];
			sentence << cloudiness_day_sentence(theParameters, *morningItem, *afternoonItem);
		  }
	  }

	return sentence;
  }

  void analyze_cloudiness_data(wf_story_params& theParameters)
  {
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	for(unsigned int i = 1; i <= generator.size(); i++)
	  {
		cloudiness_data_item_container* resultContainer = new cloudiness_data_item_container();

		WeatherPeriod theWeatherPeriod =  generator.period(i);
		WeatherPeriod theSubPeriod(theWeatherPeriod);

		if(generator.isday(i))
		  {
			if(get_part_of_the_day(theWeatherPeriod, AAMUPAIVA, theSubPeriod))
			  {				
				CloudinessDataItem* dataItem = get_cloudiness_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(AAMUPAIVA, dataItem));
			  }
			if(get_part_of_the_day(theWeatherPeriod, ILTAPAIVA, theSubPeriod))
			  {
				CloudinessDataItem* dataItem = get_cloudiness_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(ILTAPAIVA, dataItem));
			  }
		  }
		else
		  {
			if(get_part_of_the_day(theWeatherPeriod, YO, theSubPeriod))
			  {
				CloudinessDataItem* dataItem = get_cloudiness_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(YO, dataItem));
			  }
		  }
		theParameters.theCloudinessData.push_back(resultContainer);
	  }
  }

 void preprocess_cloudiness_data(wf_story_params& theParameters)
  {
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	for(unsigned int i = 1; i <= generator.size(); i++)
	  {
		cloudiness_data_item_container* resultContainer = new cloudiness_data_item_container();

		WeatherPeriod theWeatherPeriod =  generator.period(i);
		WeatherPeriod theSubPeriod(theWeatherPeriod);

		if(generator.isday(i))
		  {
			if(get_part_of_the_day(theWeatherPeriod, AAMUPAIVA, theSubPeriod))
			  {				
				CloudinessDataItem* dataItem = get_cloudiness_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(AAMUPAIVA, dataItem));
			  }
			if(get_part_of_the_day(theWeatherPeriod, ILTAPAIVA, theSubPeriod))
			  {
				CloudinessDataItem* dataItem = get_cloudiness_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(ILTAPAIVA, dataItem));
			  }
		  }
		else
		  {
			if(get_part_of_the_day(theWeatherPeriod, YO, theSubPeriod))
			  {
				CloudinessDataItem* dataItem = get_cloudiness_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(YO, dataItem));
			  }
		  }
		theParameters.theCloudinessData.push_back(resultContainer);
	  }
  }

  const weather_result_data_item_vector* get_precipitation_data(wf_story_params& theParameters,
														  const forecast_area_id& theAreaId,
														  const weather_result_data_id& theWeatherResultId)
  {
	return 	(*theParameters.theCompleteData[theAreaId])[theWeatherResultId];
  }

  void extract_the_interesting_precipitation_data(wf_story_params& theParameters,
												  const WeatherPeriod& thePeriod,
												  const forecast_area_id& theAreaId,
												  weather_result_data_item_vector& thePrecipitation,
												  weather_result_data_item_vector& thePrecipitationExtent,
												  weather_result_data_item_vector& thePrecipitationFromWater,
												  weather_result_data_item_vector& thePrecipitationFromDrizzle,
												  weather_result_data_item_vector& thePrecipitationFromSleet,
												  weather_result_data_item_vector& thePrecipitationFromSnow,
												  weather_result_data_item_vector& thePrecipitationFromFreezing,
												  weather_result_data_item_vector& thePrecipitationType,
												  weather_result_data_item_vector& thePrecipitationShareNorthEast,
												  weather_result_data_item_vector& thePrecipitationShareSouthEast,
												  weather_result_data_item_vector& thePrecipitationShareSouthWest,
												  weather_result_data_item_vector& thePrecipitationShareNorthWest)
  {
		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_DATA],
							thePrecipitation);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_EXTENT_DATA],
							thePrecipitationExtent);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_WATER_DATA],
							thePrecipitationFromWater);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_DRIZZLE_DATA],
							thePrecipitationFromDrizzle);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_SLEET_DATA],
							thePrecipitationFromSleet);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_SNOW_DATA],
							thePrecipitationFromSnow);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_FREEZING_DATA],
							thePrecipitationFromFreezing);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_TYPE_DATA],
							thePrecipitationType);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_NORTHEAST_SHARE_DATA],
							thePrecipitationShareNorthEast);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_SOUTHEAST_SHARE_DATA],
							thePrecipitationShareSouthEast);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_SOUTHWEST_SHARE_DATA],
							thePrecipitationShareSouthWest);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_NORTHWEST_SHARE_DATA],
							thePrecipitationShareNorthWest);
  }

  trend_id get_precipitation_trend(const wf_story_params& theParameters,
								   const weather_result_data_item_vector& thePrecipitationData, 
								   const unsigned int& precipitation_form)
  {
	trend_id precipitation_trend(NO_TREND);
  
	if(thePrecipitationData.size() <= 2)
	  return precipitation_trend;

	int iFromBegCount = thePrecipitationData.size() / 2;
	int iFromEndCount = thePrecipitationData.size() - iFromBegCount;
	
	// examine maximum 2 hours from the beginning and end
	iFromBegCount = iFromBegCount > 2 ? 2 : iFromBegCount;
	iFromEndCount = iFromEndCount > 2 ? 2 : iFromEndCount;
	
	float precipitationAtStart = get_mean(thePrecipitationData, 0, iFromBegCount);
	float precipitationAtEnd = get_mean(thePrecipitationData, thePrecipitationData.size()-iFromEndCount, 
										thePrecipitationData.size());

	float dryWeatherLimit;
	float weakPrecipitationLimit;

	get_dry_and_weak_precipitation_limit(theParameters, precipitation_form, dryWeatherLimit, weakPrecipitationLimit);

	if(precipitationAtStart >= weakPrecipitationLimit && precipitationAtEnd <= dryWeatherLimit)
	  precipitation_trend = POUTAANTUU;
	else if(precipitationAtStart <= dryWeatherLimit && precipitationAtEnd >= weakPrecipitationLimit)
	  precipitation_trend = SADE_ALKAA;
	
	return precipitation_trend;
  }



  PrecipitationDataItemData* get_precipitation_data_item_data(wf_story_params& theParameters, 
															  const WeatherPeriod& thePeriod,
															  const forecast_area_id& theForeacastAreaId)
  {
	if(!(theParameters.theForecastArea & theForeacastAreaId))
	  return 0;

	PrecipitationDataItemData* item = new PrecipitationDataItemData(0,
																	0.0,
																	0.0,
																	0.0,
																	0.0,
																	0.0,
																	0.0,
																	0.0,
																	0.0,
																	NO_TREND,
																	0.0,
																	thePeriod.localStartTime());

	weather_result_data_item_vector thePrecipitation;
	weather_result_data_item_vector thePrecipitationExtent;
	weather_result_data_item_vector thePrecipitationFromWater;
	weather_result_data_item_vector thePrecipitationFromDrizzle;
	weather_result_data_item_vector thePrecipitationFromSleet;
	weather_result_data_item_vector thePrecipitationFromSnow;
	weather_result_data_item_vector thePrecipitationFromFreezing;
	weather_result_data_item_vector thePrecipitationType;


	//	const double ignore_drizzle_limit = 0.02;

	extract_the_interesting_precipitation_data(theParameters,
											   thePeriod,
											   theForeacastAreaId,
											   thePrecipitation,
											   thePrecipitationExtent,
											   thePrecipitationFromWater,
											   thePrecipitationFromDrizzle,
											   thePrecipitationFromSleet,
											   thePrecipitationFromSnow,
											   thePrecipitationFromFreezing,
											   thePrecipitationType,
											   item->thePrecipitationShareNorthEast,
											   item->thePrecipitationShareSouthEast,
											   item->thePrecipitationShareSouthWest,
											   item->thePrecipitationShareNorthWest);
  


	float precipitation_intesity = get_mean(thePrecipitation);
	float precipitation_extent = get_mean(thePrecipitationExtent);
	float precipitation_form_water = get_mean(thePrecipitationFromWater);
	float precipitation_form_drizzle = get_mean(thePrecipitationFromDrizzle);
	float precipitation_form_sleet = get_mean(thePrecipitationFromSleet);
	float precipitation_form_snow = get_mean(thePrecipitationFromSnow);
	float precipitation_form_freezing = 0.0;
	float precipitation_type_showers = get_mean(thePrecipitationType);
	float pearson_coefficient = get_pearson_coefficient(thePrecipitation);
	float min_freezing = 0.0;
	float max_freezing = 0.0;
	get_min_max(thePrecipitationFromFreezing, min_freezing, max_freezing);
	precipitation_form_freezing = max_freezing;

	trend_id precipitation_trend(NO_TREND);

	const unsigned int precipitation_form = get_complete_precipitation_form(theParameters.theVariable,
														  precipitation_form_water,
														  precipitation_form_drizzle,
														  precipitation_form_sleet,
														  precipitation_form_snow,
														  precipitation_form_freezing);

	// TODO: if its raining save the location (north, east,...)

	// examine the trend
	float dryWeatherLimit;
	float weakPrecipitationLimit;
	float precipitationAtStart = thePrecipitation.at(0)->theResult.value();
	float precipitationAtEnd = thePrecipitation.at(thePrecipitation.size()-1)->theResult.value();

	get_dry_and_weak_precipitation_limit(theParameters, precipitation_form, dryWeatherLimit, weakPrecipitationLimit);
	
	if((precipitationAtStart >= weakPrecipitationLimit && 
		precipitationAtEnd <= dryWeatherLimit) && 
	   pearson_coefficient < -TREND_CHANGE_COEFFICIENT_TRESHOLD)
	  {
		precipitation_trend = POUTAANTUU;
	  }
	else if((precipitationAtStart <= dryWeatherLimit && 
			 precipitationAtEnd >= weakPrecipitationLimit) && 
			pearson_coefficient >= TREND_CHANGE_COEFFICIENT_TRESHOLD)
	  {
		precipitation_trend = SADE_ALKAA;
	  }

	item->thePrecipitationForm = precipitation_form;
	item->thePrecipitationIntensity = precipitation_intesity;
	item->thePrecipitationExtent = precipitation_extent;
	item->thePrecipitationFormWater = precipitation_form_water;
	item->thePrecipitationFormDrizzle = precipitation_form_drizzle;
	item->thePrecipitationFormSleet = precipitation_form_sleet;
	item->thePrecipitationFormSnow = precipitation_form_snow;
	item->thePrecipitationFormFreezing = precipitation_form_freezing;
	item->thePrecipitationTypeShowers = precipitation_type_showers;
	item->theTrendId = precipitation_trend;
	item->thePearsonCoefficient = pearson_coefficient;

	return item;


  }

PrecipitationDataItem* get_precipitation_data_item(wf_story_params& theParameters, 
												   const WeatherPeriod& thePeriod)
  {
	PrecipitationDataItemData* dataItemCoastal = 
	  get_precipitation_data_item_data(theParameters, thePeriod, COASTAL_AREA);
	PrecipitationDataItemData* dataItemInland = 
	  get_precipitation_data_item_data(theParameters, thePeriod, INLAND_AREA);
	PrecipitationDataItemData* dataItemFull = 
	  get_precipitation_data_item_data(theParameters, thePeriod, FULL_AREA);
	
	PrecipitationDataItem* item = new PrecipitationDataItem(dataItemCoastal, dataItemInland, dataItemFull);

	return item;
  }

  void analyze_precipitation_data(wf_story_params& theParameters)
  {
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	for(unsigned int i = 1; i <= generator.size(); i++)
	  {
		precipitation_data_item_container* resultContainer = new precipitation_data_item_container();

		WeatherPeriod theWeatherPeriod =  generator.period(i);
		WeatherPeriod theSubPeriod(theWeatherPeriod);

		if(generator.isday(i))
		  {
			if(get_part_of_the_day(theWeatherPeriod, AAMUPAIVA, theSubPeriod))
			  {
				PrecipitationDataItem* item = get_precipitation_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(AAMUPAIVA, item));
			  }
			if(get_part_of_the_day(theWeatherPeriod, ILTAPAIVA, theSubPeriod))
			  {
				PrecipitationDataItem* item = get_precipitation_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(ILTAPAIVA, item));
			  }
		  }
		else
		  {
			if(get_part_of_the_day(theWeatherPeriod, YO, theSubPeriod))
			  {
				PrecipitationDataItem* item = get_precipitation_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(YO, item));
			  }
		  }
		theParameters.thePrecipitationData.push_back(resultContainer);
	  }
  }


  Sentence precipitation_change_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;


	return sentence;
  }

  Sentence handle_morning_afternoon_sentence(Sentence& theMorningSentence, Sentence& theAfternoonSentence)
  {
	Sentence sentence;

	// TODO: is this OK?
	if(same_content(theMorningSentence, theAfternoonSentence))
	  {
		sentence << theAfternoonSentence;
	  }
	else
	  {
		if(!theMorningSentence.empty())
		  sentence << AAMUPAIVALLA_WORD << theMorningSentence;

		if(!theMorningSentence.empty() && !theAfternoonSentence.empty())
		  sentence << Delimiter(",");

		if(!theAfternoonSentence.empty())
		  if(!theMorningSentence.empty())
			sentence << ILTAPAIVALLA_WORD << theAfternoonSentence;
		else
			sentence << theAfternoonSentence;
	  }

	return sentence;
  }

  /*
  Sentence cloudiness_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	if(generator.size() < thePeriodNumber)
	  return sentence;

	Sentence cloudinessSentence;

	cloudiness_data_item_container* theResultData = theParameters.theCloudinessData[thePeriodNumber - 1];

	if(theResultData)
	  {
		if(generator.isnight(thePeriodNumber))
		  {
			if(theResultData->find(YO) != theResultData->end())
			  sentence << cloudiness_night_sentence(theParameters, *((*theResultData)[YO]));
		  }
		else
		  {
			CloudinessDataItem* morningItem = 0;
			CloudinessDataItem* afternoonItem = 0;
			if(theResultData->find(AAMUPAIVA) != theResultData->end())
			  morningItem = (*theResultData)[AAMUPAIVA];
			if(theResultData->find(ILTAPAIVA) != theResultData->end())
			  afternoonItem = (*theResultData)[ILTAPAIVA];
			sentence << cloudiness_day_sentence(theParameters, *morningItem, *afternoonItem);
		  }
	  }

	return sentence;
  }
  */



  Sentence precipitation_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

#ifdef LATER
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	if(generator.size() < thePeriodNumber)
	  return sentence;

	PrecipitationDataItem* morningDataItem = 0;
	PrecipitationDataItem* afternoonDataItem = 0;
	PrecipitationDataItem* nightDataItem = 0;

	precipitation_data_item_container* theResultData = theParameters.thePrecipitationData[thePeriodNumber - 1];

	if(theResultData)
	  {
		if(theResultData->find(AAMUPAIVA) != theResultData->end())
		  morningDataItem = (*theResultData)[AAMUPAIVA];
		if(theResultData->find(ILTAPAIVA) != theResultData->end())
		  afternoonDataItem = (*theResultData)[ILTAPAIVA];
		if(theResultData->find(YO) != theResultData->end())
		  nightDataItem = (*theResultData)[YO];
	  }

	Sentence morningSentence;
	Sentence afternoonSentence;

	/*
	if(morningDataItem && afternoonDataItem && (*morningDataItem == *afternoonDataItem))
	  {
		sentence << precipitation_sentence(theParameters,
										   afternoonDataItem->thePrecipitationForm,
										   afternoonDataItem->thePrecipitationIntensity,
										   afternoonDataItem->thePrecipitationExtent,
										   afternoonDataItem->thePrecipitationFormWater,
										   afternoonDataItem->thePrecipitationFormDrizzle,
										   afternoonDataItem->thePrecipitationFormSleet,
										   afternoonDataItem->thePrecipitationFormSnow,
										   afternoonDataItem->thePrecipitationFormFreezing,
										   afternoonDataItem->thePrecipitationTypeShowers);
		return sentence;
	  }
	*/

	if(morningDataItem)
		morningSentence << precipitation_sentence(theParameters,
												morningDataItem->thePrecipitationForm,
												morningDataItem->thePrecipitationIntensity,
												morningDataItem->thePrecipitationExtent,
												morningDataItem->thePrecipitationFormWater,
												morningDataItem->thePrecipitationFormDrizzle,
												morningDataItem->thePrecipitationFormSleet,
												morningDataItem->thePrecipitationFormSnow,
												morningDataItem->thePrecipitationFormFreezing,
												morningDataItem->thePrecipitationTypeShowers);


	if(afternoonDataItem)
	  afternoonSentence << precipitation_sentence(theParameters,
												  afternoonDataItem->thePrecipitationForm,
												  afternoonDataItem->thePrecipitationIntensity,
												  afternoonDataItem->thePrecipitationExtent,
												  afternoonDataItem->thePrecipitationFormWater,
												  afternoonDataItem->thePrecipitationFormDrizzle,
												  afternoonDataItem->thePrecipitationFormSleet,
												  afternoonDataItem->thePrecipitationFormSnow,
												  afternoonDataItem->thePrecipitationFormFreezing,
												  afternoonDataItem->thePrecipitationTypeShowers);


	
	theParameters.thePrecipitationMorning = !is_dry_weather(theParameters,
															morningDataItem->thePrecipitationForm,
															morningDataItem->thePrecipitationIntensity,
															morningDataItem->thePrecipitationExtent);
	theParameters.thePrecipitationAfternoon = !is_dry_weather(theParameters,
															  afternoonDataItem->thePrecipitationForm,
															  afternoonDataItem->thePrecipitationIntensity,
															  afternoonDataItem->thePrecipitationExtent);
	

	sentence << handle_morning_afternoon_sentence(morningSentence, afternoonSentence);

	if(!sentence.empty())
	  {
		//weather_result_data_item_vector thePrecipitationShareNorthEast;
		float meanNorthEast = get_mean(afternoonDataItem->thePrecipitationShareNorthEast);
		float meanSouthEast = get_mean(afternoonDataItem->thePrecipitationShareSouthEast);
		float meanSouthWest = get_mean(afternoonDataItem->thePrecipitationShareSouthWest);
		float meanNorthWest = get_mean(afternoonDataItem->thePrecipitationShareNorthWest);

		/*
		cout << "meanNorthEast" << meanNorthEast << endl;
		cout << "meanSouthEast" << meanSouthEast << endl;
		cout << "meanSouthWest" << meanSouthWest << endl;
		cout << "meanNorthWest" << meanNorthWest << endl;
		*/
		if(meanNorthEast > 60.0)
		  sentence << ALUEEN_KOILLISOSISSA_PHRASE;
		else if(meanSouthEast > 60.0)
		  sentence << ALUEEN_KAAKKOISOSISSA_PHRASE;
		else if(meanSouthWest > 60.0)
		  sentence << ALUEEN_LOUNAISOSISSA_PHRASE;
		else if(meanNorthWest > 60.0)
		  sentence << ALUEEN_LUOTEISOSISSA_PHRASE;
		else if(meanNorthEast + meanNorthWest > 60.0)
		  sentence << ALUEEN_POHJOISOSISSA_PHRASE;
		else if(meanSouthEast + meanSouthWest > 60.0)
		  sentence << ALUEEN_ETELAOSISSA_PHRASE;
	  }

	if(nightDataItem)
	  sentence << precipitation_sentence(theParameters,
										 nightDataItem->thePrecipitationForm,
										 nightDataItem->thePrecipitationIntensity,
										 nightDataItem->thePrecipitationExtent,
										 nightDataItem->thePrecipitationFormWater,
										 nightDataItem->thePrecipitationFormDrizzle,
										 nightDataItem->thePrecipitationFormSleet,
										 nightDataItem->thePrecipitationFormSnow,
										 nightDataItem->thePrecipitationFormFreezing,
										 nightDataItem->thePrecipitationTypeShowers);
#endif

	return sentence;
  }


  /*
  Sentence cloudiness_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	if(generator.size() < thePeriodNumber)
	  return sentence;

	Sentence cloudinessSentence;

	cloudiness_data_item_container* theResultData = theParameters.theCloudinessData[thePeriodNumber - 1];

	if(theResultData)
	  {
		if(generator.isnight(thePeriodNumber))
		  {
			if(theResultData->find(YO) != theResultData->end())
			  sentence << cloudiness_night_sentence(theParameters, *((*theResultData)[YO]));
		  }
		else
		  {
			CloudinessDataItem* morningItem = 0;
			CloudinessDataItem* afternoonItem = 0;
			if(theResultData->find(AAMUPAIVA) != theResultData->end())
			  morningItem = (*theResultData)[AAMUPAIVA];
			if(theResultData->find(ILTAPAIVA) != theResultData->end())
			  afternoonItem = (*theResultData)[ILTAPAIVA];
			sentence << cloudiness_day_sentence(theParameters, *morningItem, *afternoonItem);
		  }
	  }

	return sentence;
  }



  */


  ThunderDataItem* get_thunder_data_item(wf_story_params& theParameters, const WeatherPeriod& thePeriod)
  {
	ThunderDataItem* item = 0;

	weather_result_data_item_vector theThunderProbabilityData;

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[FULL_AREA])[THUNDERPROBABILITY_DATA],
							theThunderProbabilityData);
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[INLAND_AREA])[THUNDERPROBABILITY_DATA],
							theThunderProbabilityData);
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[COASTAL_AREA])[THUNDERPROBABILITY_DATA],
							theThunderProbabilityData);
	  }

	float min_probability = kFloatMissing;
	float mean_probability = kFloatMissing;
	float max_probability = kFloatMissing;
	float standard_deviation_probability = kFloatMissing;

	mean_probability = get_mean(theThunderProbabilityData);
	get_min_max(theThunderProbabilityData, min_probability, max_probability);
	standard_deviation_probability = get_standard_deviation(theThunderProbabilityData);
	

	item = new ThunderDataItem(min_probability,
							   mean_probability,
							   max_probability,
							   standard_deviation_probability);

	return item;
  }

  void analyze_thunder_data(wf_story_params& theParameters)
  {
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	for(unsigned int i = 1; i <= generator.size(); i++)
	  {
		thunder_data_item_container* resultContainer = new thunder_data_item_container();

		WeatherPeriod theWeatherPeriod =  generator.period(i);
		WeatherPeriod theSubPeriod(theWeatherPeriod);

		if(generator.isday(i))
		  {
			if(get_part_of_the_day(theWeatherPeriod, AAMUPAIVA, theSubPeriod))
			  {
				ThunderDataItem* item = get_thunder_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(AAMUPAIVA, item));
			  }
			if(get_part_of_the_day(theWeatherPeriod, ILTAPAIVA, theSubPeriod))
			  {
				ThunderDataItem* item = get_thunder_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(ILTAPAIVA, item));
			  }
		  }
		else
		  {
			if(get_part_of_the_day(theWeatherPeriod, YO, theSubPeriod))
			  {
				ThunderDataItem* item = get_thunder_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(YO, item));
			  }
		  }
		theParameters.theThunderData.push_back(resultContainer);
	  }
  }

  Sentence thunder_sentence(wf_story_params& theParameters,
							const float& theMinProbability,
							const float& theMeanProbability,
							const float& theMaxProbability,
							const float& theStandardDeviationProbability)
  {
	Sentence sentence;

	if(theMaxProbability == 0)
	  return sentence;

	if(theMaxProbability <= 15.0)
	  {
		sentence << PIENI_UKKOSEN_TODENNAKOISYYS_PHRASE;
	  }
	else if(theMaxProbability <= 35.0)
	  {
		sentence << MAHDOLLISESTI_UKKOSTA_PHRASE;
	  }
	else
	  {
		sentence << AJOITTAIN_UKKOSTA_PHRASE;
	  }

	return sentence;
  }

  Sentence thunder_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	ThunderDataItem* morningDataItem = 0;
	ThunderDataItem* afternoonDataItem = 0;
	ThunderDataItem* nightDataItem = 0;

	thunder_data_item_container* theResultData = theParameters.theThunderData[thePeriodNumber - 1];

	if(theResultData->find(AAMUPAIVA) != theResultData->end())
		morningDataItem = (*theResultData)[AAMUPAIVA];
	if(theResultData->find(ILTAPAIVA) != theResultData->end())
		afternoonDataItem = (*theResultData)[ILTAPAIVA];
	if(theResultData->find(YO) != theResultData->end())
		nightDataItem = (*theResultData)[YO];

	Sentence morningSentence;
	Sentence afternoonSentence;
	Sentence nightSentence;

	if(morningDataItem)
	  morningSentence << thunder_sentence(theParameters,
										  morningDataItem->theMinProbability,
										  morningDataItem->theMeanProbability,
										  morningDataItem->theMaxProbability,
										  morningDataItem->theStandardDeviationProbability);
	
	if(afternoonDataItem)
	  afternoonSentence << thunder_sentence(theParameters,
											afternoonDataItem->theMinProbability,
											afternoonDataItem->theMeanProbability,
											afternoonDataItem->theMaxProbability,
											afternoonDataItem->theStandardDeviationProbability);

	if(nightDataItem)
	  nightSentence << thunder_sentence(theParameters,
										nightDataItem->theMinProbability,
										nightDataItem->theMeanProbability,
										nightDataItem->theMaxProbability,
										nightDataItem->theStandardDeviationProbability);

	sentence << handle_morning_afternoon_sentence(morningSentence, afternoonSentence);

	sentence << nightSentence;

	return sentence;
  }

  void allocate_data_structures(wf_story_params& theParameters, const forecast_area_id& theAreaId)
  {
	weather_result_data_item_vector* hourlyTemperature = new weather_result_data_item_vector();
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

	// first split the whole period to one-hour subperiods
	NFmiTime periodStartTime = theParameters.thePeriod.localStartTime();

	theParameters.theHourPeriodCount = 0;
	while(periodStartTime.IsLessThan(theParameters.thePeriod.localEndTime()))
	  {
		NFmiTime periodEndTime = periodStartTime;
		periodEndTime.ChangeByHours(1);
		WeatherPeriod theWeatherPeriod(periodEndTime, periodEndTime);
		//		WeatherPeriod theWeatherPeriod(periodStartTime, periodEndTime);
		WeatherResult theWeatherResult(kFloatMissing, 0);

		part_of_the_day_id  partOfTheDayId = get_part_of_the_day_id(theWeatherPeriod);

		hourlyTemperature->push_back(new WeatherResultDataItem(theWeatherPeriod, 
															   theWeatherResult, 
															   partOfTheDayId));
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
		
		periodStartTime.ChangeByHours(1);
		theParameters.theHourPeriodCount++;
	  }

	weather_forecast_result_container* resultContainer = new weather_forecast_result_container();

	resultContainer->insert(make_pair(TEMPERATURE_DATA, hourlyTemperature));
	resultContainer->insert(make_pair(PRECIPITATION_DATA, hourlyPrecipitation));
	resultContainer->insert(make_pair(PRECIPITATION_EXTENT_DATA, hourlyPrecipitationExtent));
	resultContainer->insert(make_pair(PRECIPITATION_TYPE_DATA, hourlyPrecipitationType));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_WATER_DATA, hourlyPrecipitationFormWater));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_DRIZZLE_DATA, hourlyPrecipitationFormDrizzle));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_SLEET_DATA, hourlyPrecipitationFormSleet));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_SNOW_DATA, hourlyPrecipitationFormSnow));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_FREEZING_DATA, hourlyPrecipitationFormFreezing));
	resultContainer->insert(make_pair(CLOUDINESS_DATA, hourlyCloudiness));
	resultContainer->insert(make_pair(THUNDERPROBABILITY_DATA, hourlyThunderProbability));
	resultContainer->insert(make_pair(FOG_INTENSITY_MODERATE_DATA, hourlyFogIntensityModerate));
	resultContainer->insert(make_pair(FOG_INTENSITY_DENSE_DATA, hourlyFogIntensityDense));
	resultContainer->insert(make_pair(PRECIPITATION_NORTHEAST_SHARE_DATA, hourlyPrecipitationShareNortEast));
	resultContainer->insert(make_pair(PRECIPITATION_SOUTHEAST_SHARE_DATA, hourlyPrecipitationShareSouthEast));
	resultContainer->insert(make_pair(PRECIPITATION_SOUTHWEST_SHARE_DATA, hourlyPrecipitationShareSouthWest));
	resultContainer->insert(make_pair(PRECIPITATION_NORTHWEST_SHARE_DATA, hourlyPrecipitationShareNorthWest));

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
												 theParameters.thePeriod);
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
								   theParameters.thePeriod);
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
		delete (*theResultContainer[TEMPERATURE_DATA])[i];
		delete (*theResultContainer[CLOUDINESS_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_EXTENT_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_TYPE_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_WATER_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_DRIZZLE_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_SLEET_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_SNOW_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_FREEZING_DATA])[i];
		delete (*theResultContainer[THUNDERPROBABILITY_DATA])[i];
		delete (*theResultContainer[FOG_INTENSITY_MODERATE_DATA])[i];
	  }

	theResultContainer[TEMPERATURE_DATA]->clear();
	theResultContainer[PRECIPITATION_DATA]->clear();
	theResultContainer[PRECIPITATION_EXTENT_DATA]->clear();
	theResultContainer[PRECIPITATION_TYPE_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_WATER_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_DRIZZLE_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_SLEET_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_SNOW_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_FREEZING_DATA]->clear();
	theResultContainer[THUNDERPROBABILITY_DATA]->clear();
	theResultContainer[FOG_INTENSITY_MODERATE_DATA]->clear();
	theResultContainer[FOG_INTENSITY_DENSE_DATA]->clear();

	delete theResultContainer[TEMPERATURE_DATA];
	delete theResultContainer[CLOUDINESS_DATA];
	delete theResultContainer[PRECIPITATION_DATA];
	delete theResultContainer[PRECIPITATION_EXTENT_DATA];
	delete theResultContainer[PRECIPITATION_TYPE_DATA];
	delete theResultContainer[PRECIPITATION_FORM_WATER_DATA];
	delete theResultContainer[PRECIPITATION_FORM_DRIZZLE_DATA];
	delete theResultContainer[PRECIPITATION_FORM_SLEET_DATA];
	delete theResultContainer[PRECIPITATION_FORM_SNOW_DATA];
	delete theResultContainer[PRECIPITATION_FORM_FREEZING_DATA];
	delete theResultContainer[THUNDERPROBABILITY_DATA];
	delete theResultContainer[FOG_INTENSITY_MODERATE_DATA];
	delete theResultContainer[FOG_INTENSITY_DENSE_DATA];
	delete theResultContainer[PRECIPITATION_NORTHEAST_SHARE_DATA];
	delete theResultContainer[PRECIPITATION_SOUTHEAST_SHARE_DATA];
	delete theResultContainer[PRECIPITATION_SOUTHWEST_SHARE_DATA];
	delete theResultContainer[PRECIPITATION_NORTHWEST_SHARE_DATA];

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
			trend1Time = theTrendIdVector1.at(vector1Index).first.localStartTime();
			if(vector2Index < theTrendIdVector2.size())
			  {
				trend2Time = theTrendIdVector2.at(vector2Index).first.localStartTime();
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

  Sentence weather_forecast_sentence(const PrecipitationForecast& thePrecipitationForecast, 
									 const CloudinessForecast& theCloudinessForecast,
									 const FogForecast& theFogForecast,
									 const WeatherPeriod& thePeriod,
									 wf_story_params& theParameters)
  {
	Sentence sentence;
	  

#ifdef LATER
	// examine if one long rain period inside thePeriod

	vector<WeatherPeriod> theDryPeriods;

	if(thePrecipitationForecast.getDryPeriods(thePeriod, theDryPeriods))
	  {
		for(unsigned i = 0; i < theDryPeriods.size(); i++)
		  {
			sentence << theCloudinessForecast.cloudinessSentence(theDryPeriods.at(i));
			sentence << Delimiter(",");
		  }
		  /*
		if(theDryPeriods.at(0).localStartTime() == thePeriod.localStartTime())
		  {
			sentence << theCloudinessForecast.cloudinessSentence(theDryPeriods.at(0));
			sentence << Delimiter(",");
			sentence << thePrecipitationForecast.precipitationSentence(thePeriod);
		  }
		  */
		// report cloudiness
	  }
	sentence << Delimiter(".");
	sentence << Delimiter(".");
	sentence << Delimiter(".");
	
	vector<WeatherPeriod> thePrecipitationPeriods;
	if(thePrecipitationForecast.getPrecipitationPeriods(thePeriod, thePrecipitationPeriods))
	  {
		for(unsigned i = 0; i < thePrecipitationPeriods.size(); i++)
		  {
			sentence << thePrecipitationForecast.precipitationSentence(thePrecipitationPeriods.at(i));
			sentence << Delimiter(",");

			WeatherPeriod period(thePrecipitationPeriods.at(i).localStartTime(),
								 thePrecipitationPeriods.at(i).localEndTime());
			theParameters.theLog << "PR PERIOD: ";
			theParameters.theLog << period.localStartTime()
								 << "..."
								 << period.localEndTime()
								 << endl;


		  }
		/*
		if(thePrecipitationPeriods.at(0).localStartTime() == thePeriod.localStartTime())
		  {
			sentence << thePrecipitationForecast.precipitationSentence(thePrecipitationPeriods.at(0));
		  }
		*/
		// report cloudiness
	  }
#endif


	return sentence;
	//	thePrecipitationForecast.getPrecipitationPeriods()
  }



// ----------------------------------------------------------------------
  /*!populate_temperature_time_series
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

	NFmiTime periodStartTime(itsPeriod.localStartTime());
	NFmiTime periodEndTime(itsPeriod.localEndTime());

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



	Rect areaRect(itsArea);		
	if(itsArea.isNamed())
	  {
		std::string name(itsArea.name());
		log << "** " << name  << " **" << endl;
	  }

	WeatherPeriod theExtendedPeriod(periodStartTime, periodEndTime);


	wf_story_params theParameters(itsVar,
								  itsArea,
								  theExtendedPeriod,
								  itsForecastTime,
								  itsSources,
								  log);

	init_parameters(theParameters);

	if(theParameters.theForecastArea == NO_AREA)
	  return paragraph;

	populate_temperature_time_series(theParameters);
	populate_cloudiness_time_series(theParameters);
	populate_thunderprobability_time_series(theParameters);
	populate_fogintensity_time_series(theParameters);
	populate_precipitation_time_series(theParameters);

	/*
	analyze_cloudiness_data(theParameters);
	analyze_precipitation_data(theParameters);
	analyze_thunder_data(theParameters);
	*/
	/*
	NFmiTime forecastStartTime(itsForecastTime);
	NFmiTime forecastEndTime(forecastStartTime);
	forecastEndTime.ChangeByHours(24);
	*/
	/*
	NFmiTime forecastStartTime(theExtendedPeriod.localStartTime());
	NFmiTime forecastEndTime(theExtendedPeriodod.localEndTime());
	if(abs(forecastEndTime.DifferenceInHours(forecastStartTime)) < 24)
	  forecastEndTime.ChangeByHours(24);
	*/
	//	WeatherPeriod theForecastPeriod(forecastStartTime, forecastEndTime);

	CloudinessForecast cloudinessForecast(theParameters);
	PrecipitationForecast precipitationForecast(theParameters);
	FogForecast fogForecast(theParameters);
	Sentence cloudinessSentence;
	Sentence precipitationSentence;
	Sentence fogSentence;
	
	//	if(precipitationForecast.isDryPeriod(itsPeriod, theParameters.theForecastArea))
	  cloudinessSentence << cloudinessForecast.cloudinessSentence(itsPeriod);

	precipitationSentence << precipitationForecast.precipitationSentence(itsPeriod);
	fogSentence << fogForecast.fogSentence(itsPeriod);

	paragraph << precipitationSentence;
	paragraph << cloudinessSentence;
	paragraph << fogSentence;



	/*
	Sentence combinedSentence;
	combinedSentence << weather_forecast_sentence(precipitationForecast,
												  cloudinessForecast,
												  fogForecast,
												  itsPeriod,
												  theParameters);


	Paragraph para;
	para << combinedSentence;
	log << "COMBINED " << itsArea.name() << ": ";
	log << para;
	*/


	precipitationForecast.printOutPrecipitationData(log);
	precipitationForecast.printOutPrecipitationPeriods(log);
	precipitationForecast.printOutPrecipitationTrends(log);
	cloudinessForecast.printOutCloudinessTrends(log);

	trend_id_vector precipitationTrends;
	trend_id_vector cloudinessTrends;
	trend_id_vector joinedTrends;
	
	precipitationForecast.getTrendIdVector(precipitationTrends);
	cloudinessForecast.getTrendIdVector(cloudinessTrends);
	join_trend_id_vectors(precipitationTrends, cloudinessTrends, joinedTrends);

	theParameters.theLog << "JOINED TRENDS: "<< endl;
	theParameters.theLog << itsPeriod.localStartTime() << ".. " << itsPeriod.localEndTime() << endl;

	print_out_trend_vector(theParameters.theLog, joinedTrends);

	Sentence mySentence;
	if(joinedTrends.size() == 0)
	  {
		Sentence mySentence2;
		mySentence2 << precipitationForecast.shortTermPrecipitationSentence(itsPeriod);
		if(mySentence2.size() == 0)
		  mySentence2 << precipitationForecast.precipitationSentence(itsPeriod, false);
		if(mySentence2.size() == 0)
		  mySentence2 << cloudinessForecast.cloudinessSentence(itsPeriod);

		mySentence << mySentence2;
	  }
	else
	  {
		if(itsPeriod.localStartTime() < joinedTrends.at(0).first.localStartTime())
		  {
			NFmiTime startTime(itsPeriod.localStartTime());
			NFmiTime endTime(joinedTrends.at(0).first.localStartTime());
			//	endTime.ChangeByHours(-1);
			WeatherPeriod period(startTime, endTime);

			mySentence << cloudinessForecast.cloudinessSentence(period);
			if(mySentence.size() > 0)
			  mySentence << Delimiter(",");
			mySentence << precipitationForecast.shortTermPrecipitationSentence(period);
			if(precipitationForecast.shortTermPrecipitationSentence(period).size() ==  0)			  
			  mySentence << precipitationForecast.precipitationSentence(period, false);
		  }
		else 
		  {
			if(joinedTrends.at(0).second == SADE_ALKAA || joinedTrends.at(0).second == POUTAANTUU)
			  mySentence << precipitationForecast.precipitationChangeSentence(joinedTrends.at(0).first);
			else
			  mySentence << cloudinessForecast.cloudinessChangeSentence(joinedTrends.at(0).first);
		  }
		if(joinedTrends.size() > 1)
		for(unsigned int i = 1; i < joinedTrends.size(); i++)
		  {
			NFmiTime startTime(joinedTrends.at(i-1).first.localEndTime());
			NFmiTime endTime(joinedTrends.at(i).first.localStartTime());
			Sentence mySentence2;
			//	endTime.ChangeByHours(1);
			//endTime.ChangeByHours(-1);
			WeatherPeriod period(startTime, endTime);
			if(i > 0 && joinedTrends.at(i).second == SADE_ALKAA)
			  {
				mySentence << Delimiter(",");
				mySentence << cloudinessForecast.cloudinessSentence(period);
				mySentence << Delimiter(",");
				//				mySentence2 << cloudinessForecast.cloudinessSentence(period);

				if(precipitationForecast.shortTermPrecipitationSentence(period).size() > 0)
				  mySentence2 << precipitationForecast.shortTermPrecipitationSentence(period);
				else
				  mySentence2 << precipitationForecast.precipitationSentence(period, false);
			  }
			if(mySentence2.size() > 0)
			  {
				mySentence << mySentence2;
				mySentence << Delimiter(",");
			  }
			else
			  mySentence << Delimiter(",");
			mySentence << precipitationForecast.precipitationChangeSentence(joinedTrends.at(i).first);

			//  Sentence PrecipitationForecast::precipitationChangeSentence(const WeatherPeriod& thePeriod) const

		  }

		if(joinedTrends.at(joinedTrends.size()-1).first.localEndTime() < itsPeriod.localEndTime())
		  {
			NFmiTime startTime(joinedTrends.at(joinedTrends.size()-1).first.localEndTime());
			NFmiTime endTime(itsPeriod.localEndTime());
			WeatherPeriod period(startTime, endTime);
			Sentence mySentence2;
			if(precipitationForecast.shortTermPrecipitationSentence(period).size() > 0)
			  mySentence2 << precipitationForecast.shortTermPrecipitationSentence(period);
			else
			  mySentence2 << precipitationForecast.precipitationSentence(period, false);
			if(mySentence2.size() == 0)
			  mySentence2 << cloudinessForecast.cloudinessSentence(period);
			mySentence << mySentence2;
		  }
	  }
	theParameters.theLog << "COMBINED PRECIPITATION: ";
	theParameters.theLog << mySentence;// << endl;


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

	//		log_weather_result_data(theParameters);

	//	precipitationForecast.printOutPrecipitationLocation(cout);

	//	precipitationForecast.printOutPrecipitationData(cout);

	delete_data_structures(theParameters);

	log << paragraph;

	return paragraph;
  }


} // namespace TextGen

// ======================================================================
