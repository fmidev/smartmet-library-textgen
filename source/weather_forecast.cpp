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

#include <boost/lexical_cast.hpp>
#include <vector>
#include <map>

using namespace Settings;
using namespace WeatherAnalysis;
using namespace boost;
using namespace std;

namespace TextGen
{

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


  enum weather_overview_data_id
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
	  UNDEFINED_DATA_ID
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
  typedef map<int, weather_result_data_item_vector*> weather_overview_data_container;

	struct wo_params
	{								 
	  wo_params(const string& variable,
				const WeatherArea& weatherArea,
				const WeatherPeriod weatherPeriod,
				const AnalysisSources& analysisSources,
				MessageLogger& log,
				weather_overview_data_container& hourlyDataContainer,
				weather_overview_data_container& dailyDataContainer):
	  theVariable(variable),
	  theArea(weatherArea),
	  thePeriod(weatherPeriod),
	  theSources(analysisSources),
	  theLog(log),
	  theHourlyDataContainer(hourlyDataContainer),
	  theDailyDataContainer(dailyDataContainer),
	  theHourPeriodCount(0),
	  theOriginalPeriodCount(0)
	  {
	  }

	  const string& theVariable;
	  const WeatherArea& theArea;
	  const WeatherPeriod thePeriod;
	  const AnalysisSources& theSources;
	  MessageLogger& theLog;
	  weather_overview_data_container& theHourlyDataContainer;
	  weather_overview_data_container& theDailyDataContainer;
	  unsigned int theHourPeriodCount;
	  unsigned int theOriginalPeriodCount;
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

  const void log_weather_overview_data(MessageLogger& theLog,
									   weather_overview_data_container& theDataContainer)
  {
	std::string theLogMessage("*********");

	for(unsigned int i = TEMPERATURE_DATA; i < UNDEFINED_DATA_ID; i++)
	  {
		weather_result_data_item_vector* timeSeries = theDataContainer[i];
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
		else
		  continue;

		log_weather_result_time_series(theLog, theLogMessage, *timeSeries);
	  }
  }

  void populate_temperature_time_series(const wo_params& theParameters)
  {
	weather_result_data_item_vector* temperatureHourly = theParameters.theHourlyDataContainer[TEMPERATURE_DATA];
	weather_result_data_item_vector* temperatureDaily = theParameters.theDailyDataContainer[TEMPERATURE_DATA];

	GridForecaster theForecaster;
	for(unsigned int i = 0; i < temperatureHourly->size(); i++)
	  {
		WeatherResult result = theForecaster.analyze(theParameters.theVariable,
													 theParameters.theSources,
													 Temperature,
													 Mean,
													   Maximum,
													 theParameters.theArea,
													 (*temperatureHourly)[i]->thePeriod);

		  (*temperatureHourly)[i]->theResult = result;
	  }
	for(unsigned int i = 0; i < temperatureDaily->size(); i++)
	  {
		WeatherResult result = theForecaster.analyze(theParameters.theVariable,
													 theParameters.theSources,
													 Temperature,
													 Mean,
													 Maximum,
													 theParameters.theArea,
													 (*temperatureDaily)[i]->thePeriod);

		  (*temperatureDaily)[i]->theResult = result;
	  }
  }

  void populate_precipitation_time_series(const wo_params& theParameters)
  {
	GridForecaster theForecaster;
	RangeAcceptor rainlimits;
	// dry weather: 0.3 mm/day == 0.0125/hour
	rainlimits.lowerLimit(Settings::optional_double(theParameters.theVariable+"::minrain", 0.0125));

	weather_result_data_item_vector* precipitationHourly = theParameters.theHourlyDataContainer[PRECIPITATION_DATA];
	weather_result_data_item_vector* precipitationExtentHourly = theParameters.theHourlyDataContainer[PRECIPITATION_EXTENT_DATA];
	weather_result_data_item_vector* precipitationFormWaterHourly = theParameters.theHourlyDataContainer[PRECIPITATION_FORM_WATER_DATA];
	weather_result_data_item_vector* precipitationFormDrizzleHourly = theParameters.theHourlyDataContainer[PRECIPITATION_FORM_DRIZZLE_DATA];
	weather_result_data_item_vector* precipitationFormSleetHourly = theParameters.theHourlyDataContainer[PRECIPITATION_FORM_SLEET_DATA];
	weather_result_data_item_vector* precipitationFormSnowHourly = theParameters.theHourlyDataContainer[PRECIPITATION_FORM_SNOW_DATA];
	weather_result_data_item_vector* precipitationTypeHourly = theParameters.theHourlyDataContainer[PRECIPITATION_TYPE_DATA];
	weather_result_data_item_vector* precipitationDaily = theParameters.theDailyDataContainer[PRECIPITATION_DATA];
	weather_result_data_item_vector* precipitationExtentDaily = theParameters.theDailyDataContainer[PRECIPITATION_EXTENT_DATA];
	weather_result_data_item_vector* precipitationFormWaterDaily = theParameters.theDailyDataContainer[PRECIPITATION_FORM_WATER_DATA];
	weather_result_data_item_vector* precipitationFormDrizzleDaily = theParameters.theDailyDataContainer[PRECIPITATION_FORM_DRIZZLE_DATA];
	weather_result_data_item_vector* precipitationFormSleetDaily = theParameters.theDailyDataContainer[PRECIPITATION_FORM_SLEET_DATA];
	weather_result_data_item_vector* precipitationFormSnowDaily = theParameters.theDailyDataContainer[PRECIPITATION_FORM_SNOW_DATA];
	weather_result_data_item_vector* precipitationTypeDaily = theParameters.theDailyDataContainer[PRECIPITATION_TYPE_DATA];

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
	ValueAcceptor showerfilter;
	showerfilter.value(2);	// 1=large scale, 2=showers

	for(unsigned int i = 0; i < precipitationHourly->size(); i++)
	  {
        (*precipitationHourly)[i]->theResult =
		  theForecaster.analyze(theParameters.theVariable,
								theParameters.theSources,
								Precipitation,
								Mean,
								Sum,
								theParameters.theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								rainlimits);

        (*precipitationExtentHourly)[i]->theResult =
		  theForecaster.analyze(theParameters.theVariable,
								theParameters.theSources,
								Precipitation,
								Percentage,
								Sum,
								theParameters.theArea,
								(*precipitationExtentHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								rainlimits);

		(*precipitationFormWaterHourly)[i]->theResult =
		  theForecaster.analyze(theParameters.theVariable,
								theParameters.theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theParameters.theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								waterfilter);

		(*precipitationFormDrizzleHourly)[i]->theResult =
		  theForecaster.analyze(theParameters.theVariable,
								theParameters.theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theParameters.theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								drizzlefilter);

		(*precipitationFormSleetHourly)[i]->theResult =
		  theForecaster.analyze(theParameters.theVariable,
								theParameters.theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theParameters.theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								sleetfilter);

		(*precipitationFormSnowHourly)[i]->theResult =
		  theForecaster.analyze(theParameters.theVariable,
								theParameters.theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theParameters.theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								snowfilter);

		(*precipitationTypeHourly)[i]->theResult =
		  theForecaster.analyze(theParameters.theVariable,
								theParameters.theSources,
								PrecipitationType,
								Mean,
								Percentage,
								theParameters.theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								showerfilter);
	  }

	for(unsigned int i = 0; i < precipitationDaily->size(); i++)
	  {
        (*precipitationDaily)[i]->theResult =
		  theForecaster.analyze(theParameters.theVariable,
								theParameters.theSources,
								Precipitation,
								Mean,
								Sum,
								theParameters.theArea,
								(*precipitationDaily)[i]->thePeriod,
								DefaultAcceptor(),
								rainlimits);
		
       (*precipitationExtentDaily)[i]->theResult =
		 theForecaster.analyze(theParameters.theVariable,
							   theParameters.theSources,
							   Precipitation,
							   Percentage,
							   Sum,
							   theParameters.theArea,
							   (*precipitationExtentDaily)[i]->thePeriod,
							   DefaultAcceptor(),
							   DefaultAcceptor(),
							   rainlimits);

		(*precipitationFormWaterDaily)[i]->theResult =
		  theForecaster.analyze(theParameters.theVariable,
								theParameters.theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theParameters.theArea,
								(*precipitationDaily)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								waterfilter);

		(*precipitationFormDrizzleDaily)[i]->theResult =
		  theForecaster.analyze(theParameters.theVariable,
								theParameters.theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theParameters.theArea,
								(*precipitationDaily)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								drizzlefilter);

		(*precipitationFormSleetDaily)[i]->theResult =
		  theForecaster.analyze(theParameters.theVariable,
								theParameters.theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theParameters.theArea,
								(*precipitationDaily)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								sleetfilter);

		(*precipitationFormSnowDaily)[i]->theResult =
		  theForecaster.analyze(theParameters.theVariable,
								theParameters.theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theParameters.theArea,
								(*precipitationDaily)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								snowfilter);

		(*precipitationTypeDaily)[i]->theResult =
		  theForecaster.analyze(theParameters.theVariable,
								theParameters.theSources,
								PrecipitationType,
								Mean,
								Percentage,
								theParameters.theArea,
								(*precipitationDaily)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								showerfilter);
	  }
  }

  void populate_cloudiness_time_series(const wo_params& theParameters)
  {
	/*
	  const int clear = optional_percentage(theParameters.theVariable+"::clear",40);
	  const int cloudy = optional_percentage(theParameters.theVariable+"::cloudy",70);
	  
	  RangeAcceptor cloudylimits;
	  cloudylimits.lowerLimit(cloudy);
	  
	  RangeAcceptor clearlimits;
	  clearlimits.upperLimit(clear);
	*/
	  GridForecaster forecaster;
	  weather_result_data_item_vector* cloudinessHourly = theParameters.theHourlyDataContainer[CLOUDINESS_DATA];
	  weather_result_data_item_vector* cloudinessDaily = theParameters.theDailyDataContainer[CLOUDINESS_DATA];

	  for(unsigned int i = 0; i < cloudinessHourly->size(); i++)
		{
		  const string hourstr = "hour"+lexical_cast<string>(i+1);
		  WeatherPeriod weatherPeriod = (*cloudinessHourly)[i]->thePeriod;
		  (*cloudinessHourly)[i]->theResult =
		  /*
			forecaster.analyze(theParameters.theVariable+"::fake::"+hourstr+"::cloudiness",
							   theParameters.theSources,
							   Cloudiness,
							   Mean,
							   Percentage,
							   theParameters.theArea,
							   weatherPeriod,//theParameters.theCloudySkyTimeSeries[i]->thePeriod,
							   DefaultAcceptor(),
							   DefaultAcceptor(),
							   cloudylimits);
		  */
			forecaster.analyze(theParameters.theVariable+"::fake::"+hourstr+"::cloudiness",
							   theParameters.theSources,
							   Cloudiness,
							   Maximum,
							   Mean,
							   theParameters.theArea,
							   weatherPeriod);
		}

	  for(unsigned int i = 0; i < cloudinessDaily->size(); i++)
		{
		  const string hourstr = "hour"+lexical_cast<string>(i+1);
		  WeatherPeriod weatherPeriod = (*cloudinessDaily)[i]->thePeriod;
		  (*cloudinessDaily)[i]->theResult =

			forecaster.analyze(theParameters.theVariable+"::fake::"+hourstr+"::cloudiness",
							   theParameters.theSources,
							   Cloudiness,
							   Maximum,
							   Mean,
							   theParameters.theArea,
							   weatherPeriod);
		}


	  /*
	  for(unsigned int i = 0; i < theParameters.theClearSkyTimeSeries.size(); i++)
		{
		  const string hourstr = "hour"+lexical_cast<string>(i+1);
		  WeatherPeriod weatherPeriod = theParameters.theCloudySkyTimeSeries[i]->thePeriod;
		  const WeatherResult clear_percentage =
			forecaster.analyze(theParameters.theVariable+"::fake::"+hourstr+"::clear",
							   theParameters.theSources,
							   Cloudiness,
							   Mean,
							   Percentage,
							   theParameters.theArea,
							   weatherPeriod,//theParameters.theClearSkyTimeSeries[i]->thePeriod,
							   DefaultAcceptor(),
							   DefaultAcceptor(),
							   clearlimits);

		  theParameters.theClearSkyTimeSeries[i]->theResult = clear_percentage;
		}
	  */
	  /*
		const WeatherResult trend =
		  forecaster.analyze(itsVar+"::fake::"+daystr+"::trend",
							 itsSources,
							 Cloudiness,
							 Mean,
							 Trend,
							 itsArea,
							 period);
	  */
  }

  Sentence cloudiness_sentence(const wo_params& theParameters)
  {
	Sentence sentence;

	const weather_result_data_item_vector& theCloudinessTimeSeries = *theParameters.theHourlyDataContainer[CLOUDINESS_DATA];


	float cloudinessMin;
	float cloudinessMax;
	float cloudinessMean;
	float cloudinessSpreadMin;
	float cloudinessSpreadMax;
	float cloudinessSpreadMean;
	for(unsigned int i = 0; i < theCloudinessTimeSeries.size(); i++)
	  {
		// examine the cloudiness and the trend
		if(i == 0)
		  {
			cloudinessMin = cloudinessMax = cloudinessMean = theCloudinessTimeSeries[i]->theResult.value();
			cloudinessSpreadMin = cloudinessSpreadMax = cloudinessSpreadMean = theCloudinessTimeSeries[i]->theResult.error();
		  }
		else
		  {
			if(cloudinessMin > theCloudinessTimeSeries[i]->theResult.value())
			  cloudinessMin = theCloudinessTimeSeries[i]->theResult.value();
			if(cloudinessMax < theCloudinessTimeSeries[i]->theResult.value())
			  cloudinessMax = theCloudinessTimeSeries[i]->theResult.value();
			cloudinessMean += theCloudinessTimeSeries[i]->theResult.value();

			if(cloudinessSpreadMin > theCloudinessTimeSeries[i]->theResult.error())
			  cloudinessSpreadMin = theCloudinessTimeSeries[i]->theResult.error();
			if(cloudinessSpreadMax < theCloudinessTimeSeries[i]->theResult.error())
			  cloudinessSpreadMax = theCloudinessTimeSeries[i]->theResult.error();
			cloudinessSpreadMean += theCloudinessTimeSeries[i]->theResult.error();
		  }
	  } 
	
	cloudinessMean = cloudinessMean / theCloudinessTimeSeries.size();
	cloudinessSpreadMean = cloudinessSpreadMean / theCloudinessTimeSeries.size();

	if(cloudinessMin >= 35.0 && cloudinessMin <= 65.0 &&
	   cloudinessMax >= 85.0)
	  {
		sentence << SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
	  }
	else
	  {
		if(cloudinessMax < 50.0 && cloudinessMax - cloudinessMin > 20)
		  sentence << SAA_ON_VERRATTAIN_SELKEAA_PHRASE;
		else if(cloudinessMin > 50.0 && cloudinessMax - cloudinessMin > 20)
		  sentence << SAA_ON_VERRATTAIN_PILVISTA_PHRASE;
		else
		  {
			if(cloudinessMean  <= 9.99)
			  sentence << SAA_WORD << ON_WORD << SELKEAA_WORD;
			else if(cloudinessMean  <= 35)
			  sentence << SAA_WORD << ON_WORD << MELKO_SELKEAA_PHRASE;
			else if(cloudinessMean  <= 65)
			  sentence << SAA_WORD << ON_WORD << PUOLIPILVISTA_WORD;
			else if(cloudinessMean  <= 85)
			  sentence << SAA_ON_VERRATTAIN_PILVISTA_PHRASE;
			else
			  sentence << SAA_WORD << ON_WORD << PILVISTA_WORD;
		  }
	  }
	/*
	if(cloudinessMean  <= 9.99)
	  sentence << "selke‰‰";
	else if(cloudinessMean  <= 35)
	  sentence << "melkein selke‰‰";
	else if(cloudinessMean  <= 65)
	  sentence << "puolipilvist‰";
	else if(cloudinessMean  <= 85)
	  sentence << "verrattain pilvist‰";
	else
	  sentence << "pilvist‰";
	*/



	return sentence;
  }

  Sentence precipitation_sentence(const wo_params& theParameters)
  {
	Sentence sentence;

	const double weak_rain_lower_limit = optional_double(theParameters.theVariable + "::weak_rain_limit",0.04);
	const double moderate_rain_lower_limit = optional_double(theParameters.theVariable + "::moderate_rain_limit",0.24);
	const double hard_rain_lower_limit = optional_double(theParameters.theVariable + "::hard_rain_limit",2.0);
	//	const double in_some_places_limit_lower = optional_double(theParameters.theVariable + "::in_some_places_limit",2.0);
	//	const double in_many_places_limit_lower = optional_double(theParameters.theVariable + "::in_many_places_limit",2.0);
	const weather_result_data_item_vector& thePrecipitationTimeSeries = *theParameters.theHourlyDataContainer[PRECIPITATION_DATA];

	double rain_sum = 0.0;
	for(unsigned int i = 0; i < thePrecipitationTimeSeries.size(); i++)
	  {
		rain_sum += thePrecipitationTimeSeries[i]->theResult.value();
	  }

	if(rain_sum <= weak_rain_lower_limit)
	  sentence << SAA_ON_POUTAINEN_PHRASE;
	else if(rain_sum <= moderate_rain_lower_limit)
	  sentence << HEIKKOA_SADETTA_PHRASE;
	else if(rain_sum <= hard_rain_lower_limit)
	  sentence << KOHTALAISTA_SADETTA_phrase;
	else if(rain_sum > hard_rain_lower_limit)
	  sentence << RUNSASTA_SADETTA_PHRASE;

	return sentence;
  }

  void create_data_structures(wo_params& theParameters)
  {
	weather_result_data_item_vector* hourlyTemperature = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitation = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationExtent = new weather_result_data_item_vector();

	weather_result_data_item_vector* hourlyPrecipitationType = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormWater = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormDrizzle = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormSleet = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormSnow = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyCloudiness = new weather_result_data_item_vector();

	weather_result_data_item_vector* dailyTemperature = new weather_result_data_item_vector();
	weather_result_data_item_vector* dailyPrecipitation = new weather_result_data_item_vector();
	weather_result_data_item_vector* dailyPrecipitationExtent = new weather_result_data_item_vector();
	weather_result_data_item_vector* dailyPrecipitationType = new weather_result_data_item_vector();
	weather_result_data_item_vector* dailyPrecipitationFormWater = new weather_result_data_item_vector();
	weather_result_data_item_vector* dailyPrecipitationFormDrizzle = new weather_result_data_item_vector();
	weather_result_data_item_vector* dailyPrecipitationFormSleet = new weather_result_data_item_vector();
	weather_result_data_item_vector* dailyPrecipitationFormSnow = new weather_result_data_item_vector();
	weather_result_data_item_vector* dailyCloudiness = new weather_result_data_item_vector();

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
		hourlyCloudiness->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));

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
	theParameters.theHourlyDataContainer.insert(make_pair(CLOUDINESS_DATA, hourlyCloudiness));

	// the store the daily periods
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);
	for(unsigned int i = 1; i <= generator.size(); i++)
	  {
		WeatherPeriod theWeatherPeriod = generator.period(i);
		WeatherResult theWeatherResult(kFloatMissing, 0);

		dailyTemperature->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		dailyPrecipitation->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		dailyPrecipitationExtent->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		dailyPrecipitationType->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		dailyPrecipitationFormWater->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		dailyPrecipitationFormDrizzle->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		dailyPrecipitationFormSleet->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		dailyPrecipitationFormSnow->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
		dailyCloudiness->push_back(new WeatherResultDataItem(theWeatherPeriod, theWeatherResult));
	  }
	theParameters.theOriginalPeriodCount = generator.size();

	theParameters.theDailyDataContainer.insert(make_pair(TEMPERATURE_DATA, dailyTemperature));
	theParameters.theDailyDataContainer.insert(make_pair(PRECIPITATION_DATA, dailyPrecipitation));
	theParameters.theDailyDataContainer.insert(make_pair(PRECIPITATION_EXTENT_DATA, dailyPrecipitationExtent));
	theParameters.theDailyDataContainer.insert(make_pair(PRECIPITATION_TYPE_DATA, dailyPrecipitationType));
	theParameters.theDailyDataContainer.insert(make_pair(PRECIPITATION_FORM_WATER_DATA, dailyPrecipitationFormWater));
	theParameters.theDailyDataContainer.insert(make_pair(PRECIPITATION_FORM_DRIZZLE_DATA, dailyPrecipitationFormDrizzle));
	theParameters.theDailyDataContainer.insert(make_pair(PRECIPITATION_FORM_SLEET_DATA, dailyPrecipitationFormSleet));
	theParameters.theDailyDataContainer.insert(make_pair(PRECIPITATION_FORM_SNOW_DATA, dailyPrecipitationFormSnow));
	theParameters.theDailyDataContainer.insert(make_pair(CLOUDINESS_DATA, dailyCloudiness));

  }


  void delete_data_structures(wo_params& theParameters)
  {
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


	for(unsigned int i = 0; i < theParameters.theOriginalPeriodCount; i++)
	  {
		delete (*theParameters.theDailyDataContainer[TEMPERATURE_DATA])[i];
		delete (*theParameters.theDailyDataContainer[CLOUDINESS_DATA])[i];
		delete (*theParameters.theDailyDataContainer[PRECIPITATION_DATA])[i];
		delete (*theParameters.theDailyDataContainer[PRECIPITATION_EXTENT_DATA])[i];
		delete (*theParameters.theDailyDataContainer[PRECIPITATION_TYPE_DATA])[i];
		delete (*theParameters.theDailyDataContainer[PRECIPITATION_FORM_WATER_DATA])[i];
		delete (*theParameters.theDailyDataContainer[PRECIPITATION_FORM_DRIZZLE_DATA])[i];
		delete (*theParameters.theDailyDataContainer[PRECIPITATION_FORM_SLEET_DATA])[i];
		delete (*theParameters.theDailyDataContainer[PRECIPITATION_FORM_SNOW_DATA])[i];

	  } 
	delete theParameters.theDailyDataContainer[TEMPERATURE_DATA];
	delete theParameters.theDailyDataContainer[CLOUDINESS_DATA];
	delete theParameters.theDailyDataContainer[PRECIPITATION_DATA];
	delete theParameters.theDailyDataContainer[PRECIPITATION_EXTENT_DATA];
	delete theParameters.theDailyDataContainer[PRECIPITATION_TYPE_DATA];
	delete theParameters.theDailyDataContainer[PRECIPITATION_FORM_WATER_DATA];
	delete theParameters.theDailyDataContainer[PRECIPITATION_FORM_DRIZZLE_DATA];
	delete theParameters.theDailyDataContainer[PRECIPITATION_FORM_SLEET_DATA];
	delete theParameters.theDailyDataContainer[PRECIPITATION_FORM_SNOW_DATA];

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

  const Paragraph WeatherStory::forecast() const
  {
	MessageLogger log("WeatherStory::forecast");

	using namespace PrecipitationPeriodTools;

	Paragraph paragraph;

	weather_overview_data_container theHourlyDataContainer;
	weather_overview_data_container theDailyDataContainer;

	wo_params theParameters(itsVar,
							itsArea,
							itsPeriod,
							itsSources,
							log,
							theHourlyDataContainer,
							theDailyDataContainer);

	create_data_structures(theParameters);
	
	populate_temperature_time_series(theParameters);
	populate_cloudiness_time_series(theParameters);
	populate_precipitation_time_series(theParameters);


	paragraph << cloudiness_sentence(theParameters);
	paragraph << precipitation_sentence(theParameters);

	log << "HOURLY:" << endl;
	log_weather_overview_data(log, theHourlyDataContainer);
	log << "DAILY:" << endl;
	log_weather_overview_data(log, theDailyDataContainer);


	delete_data_structures(theParameters);

	log << paragraph;
	return paragraph;
  }


} // namespace TextGen

// ======================================================================
