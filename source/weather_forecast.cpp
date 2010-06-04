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


#define SAA_WORD "s‰‰"
#define ON_WORD "on"
#define HEIKKOA_WORD "heikkoa"
#define KOHTALAISTA_WORD "heikkoa"
#define SADETTA_WORD "sadetta"
#define POUTAA_WORD "poutaa"
#define RUNSASTA_SADETTA_PHRASE "runsasta sadetta"
#define SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE "s‰‰ vaihtelee puolipilvisest‰ pilviseen"
#define SAA_ON_VERRATTAIN_SELKEAA_PHRASE "s‰‰ on verrattain selke‰‰"
#define SAA_ON_VERRATTAIN_PILVISTA_PHRASE "s‰‰ on verrattain pilvist‰"
#define SELKEAA_WORD "selke‰‰"
#define PILVISTA_WORD "pilvist‰"
#define MELKO_SELKEAA_PHRASE "melko selke‰‰"
#define PUOLIPILVISTA_WORD "puolipilvist‰"
#define SAA_ON_POUTAINEN_PHRASE "poutaa"
#define HEIKKOA_SADETTA_PHRASE "heikkoa sadetta"
#define KOHTALAISTA_SADETTA_phrase "kohtalaista sadetta"
#define RUNSASTA_SADETTA_PHRASE "runsasta sadetta"


#define SELKEAA_LIMIT 9.9
#define MELKEIN_SELKEAA_LIMIT 35.0
#define PUOLIPILVISTA_LIMIT 65.0
#define VERRATTAIN_PILVISTA_LIMIT 85.0


  enum weather_result_data_id
	{
	  TEMPERATURE_DATA = 0x1,
	  CLOUDINESS_DATA,
	  PRECIPITATION_DATA,
	  PRECIPITATION_EXTENT_DATA,
	  PRECIPITATION_TYPE_DATA,
	  PRECIPITATION_FORM_WATER_DATA,
	  PRECIPITATION_FORM_DRIZZLE_DATA,
	  PRECIPITATION_FORM_SLEET_DATA,
	  PRECIPITATION_FORM_SNOW_DATA,
	  PRECIPITATION_FORM_FREEZING_DATA,
	  THUNDERPROBABILITY_DATA,
	  UNDEFINED_DATA_ID
	};

  enum rain_form_id
	{
	  WATER_FORM = 0x1,
	  DRIZZLE_FORM,
	  SLEET_FORM,
	  SNOW_FORM,
	  FREEZING_FORM,
	  UNDEFINED_FORM
	};

  enum rain_intesity_id
	{
	  DRY_WEATHER,
	  WEAK_RAIN,
	  MODERATE_RAIN,
	  HEAVY_RAIN,
	  MOSTLY_DRY_WEATHER,
	  UNDEFINED_INTENSITY_ID
	};

  enum part_of_the_day_id
	{
	  AAMU, // 06-09
	  AAMUPAIVA, // 09-11
	  PITKA_AAMUPAIVA, // 09-12
	  KESKIPAIVA, // 11-13
	  ILTAPAIVA, // 13-18
	  PITKA_ILTAPAIVA, // 12-18
	  ILTA, // 18-21
	  ILTAYO, // 22-00
	  KESKIYO, // 00-03
	  AAMUYO, // 03-06
	  PAIVA, // 09-18
	  PITKA_PAIVA, // 06-18
	  YO, // 00-06
	  PITKA_YO, // 22-06
	  YOPUOLI, // 18-06
	  PAIVAPUOLI // 06-18
	};

  /*
   1. on niin suuri, ett‰ mit‰‰n muuta keli‰ ei voi esiinty‰ monin paikoin. Esim. Y=90%
   2. Keli on monin paikoin jotain, jos keli‰ esiintyy yli M prosenttia. Vain yht‰ keli‰ voi esiinty‰ monin paikoin, eli M on yli 50 prosenttia. Esim. M=51-60%
   3. Keli on paikoin jotain, jos keli‰ esiintyy yli P prosenttia, miss‰ P on alle 50 prosenttia.
   4. Jos 90% < P <100%, voidaan k‰ytt‰‰ sanontaa "í°ƒenimm‰kseen poutaa" tai kun sateen tyyppi on kuuro "...enimm‰kseen on poutaa .Yksitt‰iset sadekuurot ovat kuitenkin mahdollisia" 
  */
	struct WeatherResultDataItem
	{
	  WeatherResultDataItem(const WeatherPeriod& period, const WeatherResult& result) :
		thePeriod(period),
		theResult(result)
	  {}

	  WeatherPeriod thePeriod;
	  WeatherResult theResult;
	};

  typedef vector<WeatherResultDataItem*> weather_result_data_item_vector;
  typedef map<int, weather_result_data_item_vector*> weather_forecast_result_container;
  typedef map<int, weather_forecast_result_container*> weather_forecast_data_container;

	struct wf_story_params
	{								 
	  wf_story_params(const string& variable,
					  const WeatherArea& weatherArea,
					  const WeatherPeriod weatherPeriod,
					  const AnalysisSources& analysisSources,
					  MessageLogger& log) :
		theVariable(variable),
		theArea(weatherArea),
		thePeriod(weatherPeriod),
		theSources(analysisSources),
		theLog(log),
		theHourPeriodCount(0),
		theOriginalPeriodCount(0),
		theForecastArea(NO_AREA)
	  {
	  }

	  const string& theVariable;
	  const WeatherArea& theArea;
	  const WeatherPeriod thePeriod;
	  const AnalysisSources& theSources;
	  MessageLogger& theLog;
	  unsigned int theHourPeriodCount;
	  unsigned int theOriginalPeriodCount;
	  unsigned short theForecastArea;
	  weather_forecast_data_container theData;
	};

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
				  endhour = 21;
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

  const void log_weather_result_time_series(MessageLogger& theLog, 
											const std::string& theLogMessage, 
											const weather_result_data_item_vector& theTimeSeries)
  {
	theLog << NFmiStringTools::Convert(theLogMessage) << endl;

	for(unsigned int i = 0; i < theTimeSeries.size(); i++)
	  {
		//		const WeatherResultDataItem& theWeatherResultDataItem = *theTimeSeries[i]; 
		log_weather_result_data_item(theLog, *theTimeSeries[i]);
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
		else if(i == PRECIPITATION_FORM_SLEET_DATA)
		  theLogMessage = "*** precipitation form sleet ****";
		else if(i == PRECIPITATION_FORM_SNOW_DATA)
		  theLogMessage = "*** precipitation form snow ****";
		else if(i == PRECIPITATION_FORM_FREEZING_DATA)
		  theLogMessage = "*** precipitation form freezing ****";
		else if(i == THUNDERPROBABILITY_DATA)
		  theLogMessage = "*** thunder probability ****";
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
		log_weather_result_data(theParameters.theLog, *theParameters.theData[INLAND_AREA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		theParameters.theLog << "**  COASTAL AREA DATA   **" << endl;
		log_weather_result_data(theParameters.theLog, *theParameters.theData[COASTAL_AREA]);
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		theParameters.theLog << "**  FULL AREA DATA   **" << endl;
		log_weather_result_data(theParameters.theLog, *theParameters.theData[FULL_AREA]);
	  }
 

#ifdef LATER
	std::string theLogMessage("*********");

	weather_result_data_container* theDataContainer =  &theParameters.theHourlyDataContainer;
	weather_result_data_item_vector* timeSeries = 0;
	for(unsigned int i = TEMPERATURE_DATA; i < UNDEFINED_DATA_ID; i++)
	  {
		timeSeries = (*theDataContainer)[i];
		if(i == TEMPERATURE_DATA)
		  theLogMessage = "*** TEMPERATURE ****";
		else if(i == PRECIPITATION_DATA)
		  theLogMessage = "*** PRECIPITATION ****";
		else if(i == PRECIPITATION_EXTENT_DATA)
		  theLogMessage = "*** PRECIPITATION EXTENT ****";
		else if(i == PRECIPITATION_TYPE_DATA)
		  theLogMessage = "*** PRECIPITATION TYPE ****";
		else if(i == CLOUDINESS_DATA)
		  theLogMessage = "*** CLOUDINESS ****";
		else if(i == PRECIPITATION_FORM_WATER_DATA)
		  theLogMessage = "*** PRECIPITATION FORM WATER ****";
		else if(i == PRECIPITATION_FORM_SLEET_DATA)
		  theLogMessage = "*** PRECIPITATION FORM SLEET ****";
		else if(i == PRECIPITATION_FORM_SNOW_DATA)
		  theLogMessage = "*** PRECIPITATION FORM SNOW ****";
		else if(i == PRECIPITATION_FORM_FREEZING_DATA)
		  theLogMessage = "*** PRECIPITATION FORM FREEZING ****";
		else if(i == THUNDERPROBABILITY_DATA)
		  theLogMessage = "*** THUNDER PROBABILITY ****";
		else
		  continue;

		log_weather_result_time_series(theParameters.theLog, theLogMessage, *timeSeries);
	  }
#endif
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
										   "AAMUPƒIVƒPERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											ILTAPAIVA, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "ILTAPƒIVƒPERIODI",
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
										   "ILTAY÷PERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											YO, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "Y÷PERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											AAMUYO, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "AAMUY÷PERIODI",
										   resultVector);
			resultVector.clear();
		  }
	  }
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
										 *(*theParameters.theData[INLAND_AREA])[TEMPERATURE_DATA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_temperature_time_series(theParameters.theVariable,
										 theParameters.theSources,
										 coastalArea,
										 *(*theParameters.theData[COASTAL_AREA])[TEMPERATURE_DATA]);
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_temperature_time_series(theParameters.theVariable,
										 theParameters.theSources,
										 theParameters.theArea,
										 *(*theParameters.theData[FULL_AREA])[TEMPERATURE_DATA]);
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

	// dry weather: 0.3 mm/day == 0.0125/hour
	float dry_weather_limit = Settings::optional_double(theVariable+"::dry_weather_limit", 0.0125);
	RangeAcceptor rainlimits;
	rainlimits.lowerLimit(dry_weather_limit);
	RangeAcceptor waterfilter;
	waterfilter.lowerLimit(0);	// 0 = drizzle
	waterfilter.upperLimit(1);	// 1 = water
	//RangeAcceptor percentagelimits;
	//percentagelimits.lowerLimit(maxrainlimit);
	ValueAcceptor drizzlefilter;
	drizzlefilter.value(0);	// 0 = drizzle
	ValueAcceptor sleetfilter;
	sleetfilter.value(2);	// 2 = sleet
	ValueAcceptor snowfilter;
	snowfilter.value(3);	// 3 = snow
	ValueAcceptor freezingfilter;
	freezingfilter.value(4);	// 4 = freezing
	ValueAcceptor showerfilter;
	showerfilter.value(2);	// 1=large scale, 2=showers

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
								rainlimits);

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
								rainlimits);

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
										   *theParameters.theData[INLAND_AREA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_precipitation_time_series(theParameters.theVariable,
										   theParameters.theSources,
										   coastalArea,
										   *theParameters.theData[COASTAL_AREA]);
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_precipitation_time_series(theParameters.theVariable,
										   theParameters.theSources,
										   theParameters.theArea,
										   *theParameters.theData[FULL_AREA]);
	  }
  }


  void populate_thunderprobability_time_series(const string& theVariable, 
											   const AnalysisSources& theSources,
											   const WeatherArea& theArea,										
											   weather_result_data_item_vector& theResultData)
  {
 	GridForecaster theForecaster;
	for(unsigned int i = 0; i < theResultData.size(); i++)
	  {
		theResultData[i]->theResult = theForecaster.analyze(theVariable,
															theSources,
															Thunder,
															Maximum,
															Maximum,
															theArea,
															theResultData[i]->thePeriod);
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
												*(*theParameters.theData[INLAND_AREA])[THUNDERPROBABILITY_DATA]);
	  }

	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_thunderprobability_time_series(theParameters.theVariable,
												theParameters.theSources,
												coastalArea,
												*(*theParameters.theData[COASTAL_AREA])[THUNDERPROBABILITY_DATA]);
	  }

	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_thunderprobability_time_series(theParameters.theVariable,
												theParameters.theSources,
												theParameters.theArea,
												*(*theParameters.theData[FULL_AREA])[THUNDERPROBABILITY_DATA]);
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
										*(*theParameters.theData[INLAND_AREA])[CLOUDINESS_DATA]);
	  }

	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_cloudiness_time_series(theParameters.theVariable,
										theParameters.theSources,
										coastalArea,
										*(*theParameters.theData[COASTAL_AREA])[CLOUDINESS_DATA]);
	  }

	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_cloudiness_time_series(theParameters.theVariable,
										theParameters.theSources,
										theParameters.theArea,
										*(*theParameters.theData[FULL_AREA])[CLOUDINESS_DATA]);
	  }
  }

  Sentence cloudiness_sentence(wf_story_params& theParameters)
  {

	Sentence sentence;
#ifdef LATER
	weather_result_data_item_vector& theCloudinessTimeSeries = *theParameters.theHourlyDataContainer[CLOUDINESS_DATA];


	float cloudinessMin;
	float cloudinessMax;
	float cloudinessMean;
	for(unsigned int i = 0; i < theCloudinessTimeSeries.size(); i++)
	  {
		// examine the cloudiness and the trend
		if(i == 0)
		  {
			cloudinessMin = cloudinessMax = cloudinessMean = theCloudinessTimeSeries[i]->theResult.value();
		  }
		else
		  {
			if(cloudinessMin > theCloudinessTimeSeries[i]->theResult.value())
			  cloudinessMin = theCloudinessTimeSeries[i]->theResult.value();
			if(cloudinessMax < theCloudinessTimeSeries[i]->theResult.value())
			  cloudinessMax = theCloudinessTimeSeries[i]->theResult.value();
			cloudinessMean += theCloudinessTimeSeries[i]->theResult.value();
		  }
	  } 
	
	cloudinessMean = cloudinessMean / theCloudinessTimeSeries.size();

	if(cloudinessMin >= MELKEIN_SELKEAA_LIMIT && cloudinessMin <= PUOLIPILVISTA_LIMIT &&
	   cloudinessMax >= VERRATTAIN_PILVISTA_LIMIT)
	  {
		sentence << SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
	  }
	else
	  {
		if(cloudinessMean  <= SELKEAA_LIMIT)
		  sentence << SAA_WORD << ON_WORD << SELKEAA_WORD;
		else if(cloudinessMean  <= MELKEIN_SELKEAA_LIMIT)
		  sentence << SAA_WORD << ON_WORD << MELKO_SELKEAA_PHRASE;
		else if(cloudinessMean  <= PUOLIPILVISTA_LIMIT)
		  sentence << SAA_WORD << ON_WORD << PUOLIPILVISTA_WORD;
		else if(cloudinessMean  <= VERRATTAIN_PILVISTA_LIMIT)
		  sentence << SAA_ON_VERRATTAIN_PILVISTA_PHRASE;
		else
		  sentence << SAA_WORD << ON_WORD << PILVISTA_WORD;
	  }
#endif
	return sentence;
  }

  float get_average(const weather_result_data_item_vector& thePrecipitationTimeSeries)
  {
	float rain_sum = 0.0;
	for(unsigned int i = 0; i < thePrecipitationTimeSeries.size(); i++)
	  {
		rain_sum += thePrecipitationTimeSeries[i]->theResult.value();
	  }
	return rain_sum / thePrecipitationTimeSeries.size();
  }

  Sentence precipitation_sentence(wf_story_params& theParameters)
  {
	Sentence sentence;

	const double weak_rain_lower_limit = optional_double(theParameters.theVariable + "::weak_rain_limit",0.04);
	const double moderate_rain_lower_limit = optional_double(theParameters.theVariable + "::moderate_rain_limit",0.24);
	const double hard_rain_lower_limit = optional_double(theParameters.theVariable + "::hard_rain_limit",2.0);
	const double extensive_rain_limit = optional_double(theParameters.theVariable + "::extensive_rain_limit", 90.0);
	const double in_many_places_limit = optional_double(theParameters.theVariable + "::in_many_places_limit", 50.0);
	const double in_some_places_limit = optional_double(theParameters.theVariable + "::in_some_places_limit", 10.0);

	const weather_result_data_item_vector* thePrecipitationInland = 0;
	const weather_result_data_item_vector* thePrecipitationCoastal = 0;
	const weather_result_data_item_vector* thePrecipitationFull = 0;
	const weather_result_data_item_vector* thePrecipitationExtentInland = 0;
	const weather_result_data_item_vector* thePrecipitationExtentCoastal = 0;
	const weather_result_data_item_vector* thePrecipitationExtentFull = 0;
	const weather_result_data_item_vector* thePrecipitationFromWaterInland = 0;
	const weather_result_data_item_vector* thePrecipitationFromDrizzleInland = 0;
	const weather_result_data_item_vector* thePrecipitationFromSleetInland = 0;
	const weather_result_data_item_vector* thePrecipitationFromSnowInland = 0;
	const weather_result_data_item_vector* thePrecipitationFromFreezingInland = 0;
	const weather_result_data_item_vector* thePrecipitationFromWaterCoastal = 0;
	const weather_result_data_item_vector* thePrecipitationFromDrizzleCoastal = 0;
	const weather_result_data_item_vector* thePrecipitationFromSleetCoastal = 0;
	const weather_result_data_item_vector* thePrecipitationFromSnowCoastal = 0;
	const weather_result_data_item_vector* thePrecipitationFromFreezingCoastal = 0;
	const weather_result_data_item_vector* thePrecipitationFromWaterFull = 0;
	const weather_result_data_item_vector* thePrecipitationFromDrizzleFull = 0;
	const weather_result_data_item_vector* thePrecipitationFromSleetFull = 0;
	const weather_result_data_item_vector* thePrecipitationFromSnowFull = 0;
	const weather_result_data_item_vector* thePrecipitationFromFreezingFull = 0;

	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		thePrecipitationInland = (*theParameters.theData[INLAND_AREA])[PRECIPITATION_DATA];
		thePrecipitationExtentInland = (*theParameters.theData[INLAND_AREA])[PRECIPITATION_EXTENT_DATA];
		thePrecipitationFromWaterInland = (*theParameters.theData[INLAND_AREA])[PRECIPITATION_FORM_WATER_DATA];
		thePrecipitationFromDrizzleInland = (*theParameters.theData[INLAND_AREA])[PRECIPITATION_FORM_DRIZZLE_DATA];
		thePrecipitationFromSleetInland = (*theParameters.theData[INLAND_AREA])[PRECIPITATION_FORM_SLEET_DATA];
		thePrecipitationFromSnowInland = (*theParameters.theData[INLAND_AREA])[PRECIPITATION_FORM_SNOW_DATA];
		thePrecipitationFromFreezingInland = (*theParameters.theData[INLAND_AREA])[PRECIPITATION_FORM_FREEZING_DATA];
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		thePrecipitationCoastal = (*theParameters.theData[COASTAL_AREA])[PRECIPITATION_DATA];
		thePrecipitationExtentCoastal = (*theParameters.theData[COASTAL_AREA])[PRECIPITATION_EXTENT_DATA];
		thePrecipitationFromWaterCoastal = (*theParameters.theData[COASTAL_AREA])[PRECIPITATION_FORM_WATER_DATA];
		thePrecipitationFromDrizzleCoastal = (*theParameters.theData[COASTAL_AREA])[PRECIPITATION_FORM_DRIZZLE_DATA];
		thePrecipitationFromSleetCoastal = (*theParameters.theData[COASTAL_AREA])[PRECIPITATION_FORM_SLEET_DATA];
		thePrecipitationFromSnowCoastal = (*theParameters.theData[COASTAL_AREA])[PRECIPITATION_FORM_SNOW_DATA];
		thePrecipitationFromFreezingCoastal = (*theParameters.theData[COASTAL_AREA])[PRECIPITATION_FORM_FREEZING_DATA];
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		thePrecipitationFull = (*theParameters.theData[FULL_AREA])[PRECIPITATION_DATA];
		thePrecipitationExtentFull = (*theParameters.theData[FULL_AREA])[PRECIPITATION_EXTENT_DATA];
		thePrecipitationFromWaterFull = (*theParameters.theData[FULL_AREA])[PRECIPITATION_FORM_WATER_DATA];
		thePrecipitationFromDrizzleFull = (*theParameters.theData[FULL_AREA])[PRECIPITATION_FORM_DRIZZLE_DATA];
		thePrecipitationFromSleetFull = (*theParameters.theData[FULL_AREA])[PRECIPITATION_FORM_SLEET_DATA];
		thePrecipitationFromSnowFull = (*theParameters.theData[FULL_AREA])[PRECIPITATION_FORM_SNOW_DATA];
		thePrecipitationFromFreezingFull = (*theParameters.theData[FULL_AREA])[PRECIPITATION_FORM_FREEZING_DATA];
	  }

	float rain_intesity = 0.0;
	float rain_extent = 0.0;
	float rain_form_water = 0.0;
	float rain_form_drizzle = 0.0;
	float rain_form_sleet = 0.0;
	float rain_form_snow = 0.0;
	float rain_form_freezing = 0.0;

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		rain_intesity = get_average(*thePrecipitationFull);
		rain_extent = get_average(*thePrecipitationExtentFull);
		if(rain_extent > 0.0 && rain_intesity > weak_rain_lower_limit)
		  {
			rain_form_water = get_average(*thePrecipitationFromWaterFull);
			rain_form_drizzle = get_average(*thePrecipitationFromDrizzleFull);
			rain_form_sleet = get_average(*thePrecipitationFromSleetFull);
			rain_form_snow = get_average(*thePrecipitationFromSnowFull);
			rain_form_freezing = get_average(*thePrecipitationFromFreezingFull);
		  }
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		rain_intesity = get_average(*thePrecipitationInland);
		rain_extent = get_average(*thePrecipitationExtentInland);
		if(rain_extent > 0.0 && rain_intesity > weak_rain_lower_limit)
		  {
			rain_form_water = get_average(*thePrecipitationFromWaterInland);
			rain_form_drizzle = get_average(*thePrecipitationFromDrizzleInland);
			rain_form_sleet = get_average(*thePrecipitationFromSleetInland);
			rain_form_snow = get_average(*thePrecipitationFromSnowInland);
			rain_form_freezing = get_average(*thePrecipitationFromFreezingInland);
		  }
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		rain_intesity = get_average(*thePrecipitationCoastal);
		rain_extent = get_average(*thePrecipitationExtentCoastal);
		if(rain_extent > 0.0 && rain_intesity > weak_rain_lower_limit)
		  {
			rain_form_water = get_average(*thePrecipitationFromWaterCoastal);
			rain_form_drizzle = get_average(*thePrecipitationFromDrizzleCoastal);
			rain_form_sleet = get_average(*thePrecipitationFromSleetCoastal);
			rain_form_snow = get_average(*thePrecipitationFromSnowCoastal);
			rain_form_freezing = get_average(*thePrecipitationFromFreezingCoastal);
		  }
	  }

	if(rain_extent == 0.0 || rain_intesity <= weak_rain_lower_limit)
	  {
		  sentence << SAA_ON_POUTAINEN_PHRASE;
	  }
	else
	  {
		typedef std::pair<float, rain_form_id> rain_form_type;
		rain_form_type water(rain_form_water, WATER_FORM);
		rain_form_type drizzle(rain_form_drizzle, DRIZZLE_FORM);
		rain_form_type sleet(rain_form_sleet, SLEET_FORM);
		rain_form_type snow(rain_form_snow, SNOW_FORM);
		rain_form_type freezing(rain_form_freezing, FREEZING_FORM);


		vector<rain_form_type> rain_forms;
		rain_forms.push_back(water);
		rain_forms.push_back(drizzle);
		rain_forms.push_back(sleet);
		rain_forms.push_back(snow);
		rain_forms.push_back(freezing);

		sort(rain_forms.begin(),rain_forms.end());

		//	rain_form_id majorRainForm = rain_forms[4].second;
		// rain_form_id minorRainForm = rain_forms[3].second;

		if(rain_extent >= extensive_rain_limit)
		  {
		  }
		else if(rain_extent >= in_many_places_limit)
		  {
		  }
		else if(rain_extent >= in_some_places_limit)
		  {
		  }
		else if(rain_extent > 0)
		  {
			if(rain_intesity <= weak_rain_lower_limit)
			  sentence << SAA_ON_POUTAINEN_PHRASE;
			else
			  sentence << "S‰‰ on enimm‰kseen poutaista. Yksitt‰iset sadekuurot ovat kuitenkin mahdollisia";
		  }
	  }

	if(rain_intesity <= weak_rain_lower_limit)
	  sentence << SAA_ON_POUTAINEN_PHRASE;
	else if(rain_intesity <= moderate_rain_lower_limit)
	  sentence << HEIKKOA_SADETTA_PHRASE;
	else if(rain_intesity <= hard_rain_lower_limit)
	  sentence << KOHTALAISTA_SADETTA_phrase;
	else if(rain_intesity > hard_rain_lower_limit)
	  sentence << RUNSASTA_SADETTA_PHRASE;

#ifdef LATER
	const weather_result_data_item_vector& thePrecipitationTimeSeries = *theParameters.theHourlyDataContainer[PRECIPITATION_DATA];

	double rain_sum = 0.0;
	for(unsigned int i = 0; i < thePrecipitationTimeSeries.size(); i++)
	  {
		rain_sum += thePrecipitationTimeSeries[i]->theResult.value();
	  }

	rain_sum = rain_sum / thePrecipitationTimeSeries.size();

	if(rain_sum <= weak_rain_lower_limit)
	  sentence << SAA_ON_POUTAINEN_PHRASE;
	else if(rain_sum <= moderate_rain_lower_limit)
	  sentence << HEIKKOA_SADETTA_PHRASE;
	else if(rain_sum <= hard_rain_lower_limit)
	  sentence << KOHTALAISTA_SADETTA_phrase;
	else if(rain_sum > hard_rain_lower_limit)
	  sentence << RUNSASTA_SADETTA_PHRASE;
#endif
	return sentence;
  }

  Sentence thunder_probability_sentence(wf_story_params& theParameters)
  {
	Sentence sentence;

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

	// first split the whole period to one-hour subperiods
	NFmiTime periodStartTime = theParameters.thePeriod.localStartTime();

	theParameters.theHourPeriodCount = 0;
	while(periodStartTime.IsLessThan(theParameters.thePeriod.localEndTime()))
	  {
		NFmiTime periodEndTime = periodStartTime;
		periodEndTime.ChangeByHours(1);
		WeatherPeriod theWeatherPeriod(periodStartTime, periodEndTime);
		WeatherResult theWeatherResult(kFloatMissing, 0);

		hourlyTemperature->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyPrecipitation->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyPrecipitationExtent->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyPrecipitationType->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyPrecipitationFormWater->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyPrecipitationFormDrizzle->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyPrecipitationFormSleet->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyPrecipitationFormSnow->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyPrecipitationFormFreezing->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyCloudiness->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyThunderProbability->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));

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
	
	theParameters.theData.insert(make_pair(theAreaId, resultContainer));
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

#ifdef LATER
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

	// first split the whole period to one-hour subperiods
	NFmiTime periodStartTime = theParameters.thePeriod.localStartTime();

	while(periodStartTime.IsLessThan(theParameters.thePeriod.localEndTime()))
	  {
		NFmiTime periodEndTime = periodStartTime;
		periodEndTime.ChangeByHours(1);
		WeatherPeriod theWeatherPeriod(periodStartTime, periodEndTime);
		WeatherResult theWeatherResult(kFloatMissing, 0);

		hourlyTemperature->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyPrecipitation->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyPrecipitationExtent->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyPrecipitationType->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyPrecipitationFormWater->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyPrecipitationFormDrizzle->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyPrecipitationFormSleet->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyPrecipitationFormSnow->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyPrecipitationFormFreezing->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyCloudiness->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		hourlyThunderProbability->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));

		periodStartTime.ChangeByHours(1);
		theParameters.theHourPeriodCount++;
	  }
	//	const unsigned int hourPeriodCount = temperature.size();

	theParameters.theHourlyDataContainer.insert(make_pair(TEMPERATURE_DATA, hourlyTemperature));
	theParameters.theHourlyDataContainer.insert(make_pair(PRECIPITATION_DATA, hourlyPrecipitation));
	theParameters.theHourlyDataContainer.insert(make_pair(PRECIPITATION_EXTENT_DATA, hourlyPrecipitationExtent));
	theParameters.theHourlyDataContainer.insert(make_pair(PRECIPITATION_TYPE_DATA, hourlyPrecipitationType));
	theParameters.theHourlyDataContainer.insert(make_pair(PRECIPITATION_FORM_WATER_DATA, hourlyPrecipitationFormWater));
	theParameters.theHourlyDataContainer.insert(make_pair(PRECIPITATION_FORM_DRIZZLE_DATA, hourlyPrecipitationFormDrizzle));
	theParameters.theHourlyDataContainer.insert(make_pair(PRECIPITATION_FORM_SLEET_DATA, hourlyPrecipitationFormSleet));
	theParameters.theHourlyDataContainer.insert(make_pair(PRECIPITATION_FORM_SNOW_DATA, hourlyPrecipitationFormSnow));
	theParameters.theHourlyDataContainer.insert(make_pair(PRECIPITATION_FORM_FREEZING_DATA, hourlyPrecipitationFormFreezing));
	theParameters.theHourlyDataContainer.insert(make_pair(CLOUDINESS_DATA, hourlyCloudiness));
	theParameters.theHourlyDataContainer.insert(make_pair(THUNDERPROBABILITY_DATA, hourlyThunderProbability));
#endif
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
	  }
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
  }

  void delete_data_structures(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		deallocate_data_structure(theParameters.theHourPeriodCount, *theParameters.theData[INLAND_AREA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		deallocate_data_structure(theParameters.theHourPeriodCount, *theParameters.theData[COASTAL_AREA]);
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		deallocate_data_structure(theParameters.theHourPeriodCount, *theParameters.theData[FULL_AREA]);
	  }


#ifdef LATER
	// delete the allocated WeatherResultDataItem-objects
	for(unsigned int i = 0; i < theParameters.theHourPeriodCount; i++)
	  {
		delete (*theParameters.theHourlyDataContainer[TEMPERATURE_DATA])[i];
		delete (*theParameters.theHourlyDataContainer[CLOUDINESS_DATA])[i];
		delete (*theParameters.theHourlyDataContainer[PRECIPITATION_DATA])[i];
		delete (*theParameters.theHourlyDataContainer[PRECIPITATION_EXTENT_DATA])[i];
		delete (*theParameters.theHourlyDataContainer[PRECIPITATION_TYPE_DATA])[i];
		delete (*theParameters.theHourlyDataContainer[PRECIPITATION_FORM_WATER_DATA])[i];
		delete (*theParameters.theHourlyDataContainer[PRECIPITATION_FORM_DRIZZLE_DATA])[i];
		delete (*theParameters.theHourlyDataContainer[PRECIPITATION_FORM_SLEET_DATA])[i];
		delete (*theParameters.theHourlyDataContainer[PRECIPITATION_FORM_SNOW_DATA])[i];
		delete (*theParameters.theHourlyDataContainer[PRECIPITATION_FORM_FREEZING_DATA])[i];
		delete (*theParameters.theHourlyDataContainer[THUNDERPROBABILITY_DATA])[i];
	  }

	delete theParameters.theHourlyDataContainer[TEMPERATURE_DATA];
	delete theParameters.theHourlyDataContainer[CLOUDINESS_DATA];
	delete theParameters.theHourlyDataContainer[PRECIPITATION_DATA];
	delete theParameters.theHourlyDataContainer[PRECIPITATION_EXTENT_DATA];
	delete theParameters.theHourlyDataContainer[PRECIPITATION_TYPE_DATA];
	delete theParameters.theHourlyDataContainer[PRECIPITATION_FORM_WATER_DATA];
	delete theParameters.theHourlyDataContainer[PRECIPITATION_FORM_DRIZZLE_DATA];
	delete theParameters.theHourlyDataContainer[PRECIPITATION_FORM_SLEET_DATA];
	delete theParameters.theHourlyDataContainer[PRECIPITATION_FORM_SNOW_DATA];
	delete theParameters.theHourlyDataContainer[PRECIPITATION_FORM_FREEZING_DATA];
	delete theParameters.theHourlyDataContainer[THUNDERPROBABILITY_DATA];
#endif
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

	wf_story_params theParameters(itsVar,
								  itsArea,
								  itsPeriod,
								  itsSources,
								  log);


	if(itsArea.isNamed())
	  {
		std::string name(itsArea.name());
		log << "** " << name  << " **" << endl;
	  }


	create_data_structures(theParameters);

	if(theParameters.theForecastArea == NO_AREA)
	  return paragraph;

	populate_temperature_time_series(theParameters);
	populate_cloudiness_time_series(theParameters);
	populate_thunderprobability_time_series(theParameters);
	populate_precipitation_time_series(theParameters);

	paragraph << precipitation_sentence(theParameters);

	/*
	paragraph << cloudiness_sentence(theParameters);
	paragraph << thunder_probability_sentence(theParameters);

	log_subperiods(theParameters);
	*/
	log_weather_result_data(theParameters);

	delete_data_structures(theParameters);

	log << paragraph;
	return paragraph;
  }


} // namespace TextGen

// ======================================================================
