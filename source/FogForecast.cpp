// ======================================================================
/*!
 * \file
 * \brief Implementation of FogForecast class
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
#include "FogForecast.h"

#include "NFmiCombinedParam.h"
#include "NFmiMercatorArea.h"

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

#define PAIKOIN_SUMUA_COMPOSITE_PHRASE "[aamupäivällä] [rannikolla] [paikoin] sumua"
#define PAIKOIN_SUMUA_JOKAVOIOLLA_SAKEAA_COMPOSITE_PHRASE "[aamupäivällä] [rannikolla] [paikoin] sumua, joka"

  /*
  std::ostream& operator<<(std::ostream & theOutput,
						   const FogIntensityDataItem& theFogIntensityDataItem)
  {
	theOutput << "moderate fog=" << theFogIntensityDataItem.moderateFogExtent << " ";
	theOutput << "dense fog=" << theFogIntensityDataItem.denseFogExtent << " ";
	return theOutput;
  }

  std::ostream& operator<<(std::ostream & theOutput,
						   const CloudinessDataItem& theCloudinessDataItem)
  {
	if(theCloudinessDataItem.theCoastalData)
	  {
		theOutput << "  Coastal" << endl;
		theOutput << *theCloudinessDataItem.theCoastalData;
	  }
	if(theCloudinessDataItem.theInlandData)
	  {
		theOutput << "  Inland" << endl;
		theOutput << *theCloudinessDataItem.theInlandData;
	  }
	if(theCloudinessDataItem.theFullData)
	  {
		theOutput << "  Full area" << endl;
		theOutput << *theCloudinessDataItem.theFullData;
	  }
	return theOutput;
  }
  */

  
  pair<WeatherPeriod, FogIntensityDataItem> get_fog_wp(const NFmiTime& theStartTime,
													   const NFmiTime& theEndTime,
													   const float& theModerateFogExtent,
													   const float& theDenseFogExtent)
  {
	return make_pair(WeatherPeriod(theStartTime, theEndTime), 
					 FogIntensityDataItem(theModerateFogExtent, theDenseFogExtent));
  }

  pair<WeatherPeriod, fog_type_id> get_fog_type_wp(const NFmiTime& theStartTime,
												   const NFmiTime& theEndTime,
												   const fog_type_id& theFogTypeId)
  {
	return make_pair(WeatherPeriod(theStartTime, theEndTime), theFogTypeId);
  }

  const char* get_fog_type_string(const fog_type_id& theFogTypeId)
  {
	const char* retval = "";

	switch(theFogTypeId)
	  {
	  case FOG:
		retval = "sumua";
		break;
	  case FOG_POSSIBLY_DENSE:
		retval = "sumua, joka voi olla sakeaa";
		break;
	  case FOG_IN_SOME_PLACES:
		retval = "paikoin sumua";
		break;
	  case FOG_IN_SOME_PLACES_POSSIBLY_DENSE:
		retval = "paikoin sumua, joka voi olla sakeaa";
		break;
	  case FOG_IN_MANY_PLACES:
		retval = "monin paikoin sumua";
		break;
	  case FOG_IN_MANY_PLACES_POSSIBLY_DENSE:
		retval = "monin paikoin sumua, joka voi olla sakeaa";
		break;
	  default:
		retval = "ei sumua";
		break;
	  }

	return retval;
  }

  fog_type_id get_fog_type(const float& theModerateFog, const float& theDenseFog)
  {
	float totalFog = theModerateFog + theDenseFog;

	if(totalFog < IN_SOME_PLACES_LOWER_LIMIT_FOG)
	  {
		return NO_FOG;
	  }
	else if(totalFog >= IN_SOME_PLACES_LOWER_LIMIT_FOG && 
		  totalFog <= IN_SOME_PLACES_UPPER_LIMIT)
	  {
		if(theDenseFog < IN_SOME_PLACES_LOWER_LIMIT)
		  return FOG_IN_SOME_PLACES;
		else
		  return FOG_IN_SOME_PLACES_POSSIBLY_DENSE;
	  }
	else if(totalFog > IN_MANY_PLACES_LOWER_LIMIT && 
			totalFog < IN_MANY_PLACES_UPPER_LIMIT)
	  {
		if(theDenseFog < IN_SOME_PLACES_LOWER_LIMIT)
		  return FOG_IN_MANY_PLACES;
		else
		  return FOG_IN_MANY_PLACES_POSSIBLY_DENSE;
	  }
	else
	  {
		if(theDenseFog < IN_SOME_PLACES_LOWER_LIMIT)
		  return FOG;
		else
		  return FOG_POSSIBLY_DENSE;
	  }
  }

  std::string FogForecast::theDayPhasePhraseOld = "";

  FogForecast::FogForecast(wf_story_params& parameters):
	theParameters(parameters),
	theCoastalModerateFogData(0),
	theInlandModerateFogData(0),
	theFullAreaModerateFogData(0),
	theCoastalDenseFogData(0),
	theInlandDenseFogData(0),
	theFullAreaDenseFogData(0)

  {
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		theFullAreaModerateFogData = ((*theParameters.theCompleteData[FULL_AREA])[FOG_INTENSITY_MODERATE_DATA]);
		theFullAreaDenseFogData = ((*theParameters.theCompleteData[FULL_AREA])[FOG_INTENSITY_DENSE_DATA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		theCoastalModerateFogData = ((*theParameters.theCompleteData[COASTAL_AREA])[FOG_INTENSITY_MODERATE_DATA]);
		theCoastalDenseFogData = ((*theParameters.theCompleteData[COASTAL_AREA])[FOG_INTENSITY_DENSE_DATA]);
	  }
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		theInlandModerateFogData = ((*theParameters.theCompleteData[INLAND_AREA])[FOG_INTENSITY_MODERATE_DATA]);
		theInlandDenseFogData = ((*theParameters.theCompleteData[INLAND_AREA])[FOG_INTENSITY_DENSE_DATA]);
	  }
	
	findOutFogPeriods(); 
	findOutFogTypePeriods(); 
  }

  void FogForecast::findOutFogPeriods(const weather_result_data_item_vector* theModerateFogData, 
									  const weather_result_data_item_vector* theDenseFogData, 
									  fog_period_vector& theFogPeriods)
  {
	if(theModerateFogData && theDenseFogData && theModerateFogData->size() > 0)
	  {
		NFmiTime fogPeriodStartTime(theModerateFogData->at(0)->thePeriod.localStartTime());

		for(unsigned int i = 1; i < theModerateFogData->size(); i++)
		  {
			if(i == theModerateFogData->size() - 1)
			  {
				if((theModerateFogData->at(i)->theResult.value() != 
					theModerateFogData->at(i-1)->theResult.value() ||
					theDenseFogData->at(i)->theResult.value() != 
					theDenseFogData->at(i-1)->theResult.value()))
				  {
					theFogPeriods.push_back(get_fog_wp(fogPeriodStartTime,
													   theModerateFogData->at(i-1)->thePeriod.localEndTime(),
													   theModerateFogData->at(i-1)->theResult.value(),
													   theDenseFogData->at(i-1)->theResult.value()));
					theFogPeriods.push_back(get_fog_wp(theModerateFogData->at(i)->thePeriod.localStartTime(),
													   theModerateFogData->at(i)->thePeriod.localEndTime(),
													   theModerateFogData->at(i)->theResult.value(),
													   theDenseFogData->at(i)->theResult.value()));
				  }
				else
				  {
					theFogPeriods.push_back(get_fog_wp(fogPeriodStartTime,
													   theModerateFogData->at(i)->thePeriod.localEndTime(),
													   theModerateFogData->at(i)->theResult.value(),
													   theDenseFogData->at(i)->theResult.value()));
				  }
			  }
			else
			  {
				if((theModerateFogData->at(i)->theResult.value() != 
					theModerateFogData->at(i-1)->theResult.value() ||
					theDenseFogData->at(i)->theResult.value() != 
					theDenseFogData->at(i-1)->theResult.value()))
				  {
					theFogPeriods.push_back(get_fog_wp(fogPeriodStartTime,
													   theModerateFogData->at(i-1)->thePeriod.localEndTime(),
													   theModerateFogData->at(i-1)->theResult.value(),
													   theDenseFogData->at(i-1)->theResult.value()));
					fogPeriodStartTime = theModerateFogData->at(i)->thePeriod.localStartTime();
				  }
			  }
		  }
	  }
  }

  void FogForecast::findOutFogPeriods()
  {
	findOutFogPeriods(theCoastalModerateFogData,
					  theCoastalDenseFogData,
					  theCoastalFog);
	findOutFogPeriods(theInlandModerateFogData,
					  theInlandDenseFogData,
					  theInlandFog);
	findOutFogPeriods(theFullAreaModerateFogData,
					  theFullAreaDenseFogData,
					  theFullAreaFog);
  }

  void FogForecast::findOutFogTypePeriods(const fog_period_vector& theFogPeriods,
										  fog_type_period_vector& theFogTypePeriods)
  {
	if(theFogPeriods.size() == 0)
	  return;

	NFmiTime fogPeriodStartTime(theFogPeriods.at(0).first.localStartTime());
	fog_type_id previousFogType = get_fog_type(theFogPeriods.at(0).second.theModerateFogExtent,
											   theFogPeriods.at(0).second.theDenseFogExtent);
	fog_type_id currentFogType = currentFogType;

	for(unsigned int i = 1; i < theFogPeriods.size(); i++)
	  {
		currentFogType = get_fog_type(theFogPeriods.at(i).second.theModerateFogExtent,
									  theFogPeriods.at(i).second.theDenseFogExtent);

		if(i == theFogPeriods.size() - 1)
		  {
			if(previousFogType != currentFogType)
			  {
				if(previousFogType != NO_FOG)
				  theFogTypePeriods.push_back(get_fog_type_wp(fogPeriodStartTime,
															  theFogPeriods.at(i-1).first.localEndTime(),
															  previousFogType));
				if(currentFogType != NO_FOG)
				  theFogTypePeriods.push_back(get_fog_type_wp(theFogPeriods.at(i).first.localStartTime(),
															  theFogPeriods.at(i).first.localEndTime(),
															  currentFogType));
			  }
			else
			  {
				if(currentFogType != NO_FOG)
				  theFogTypePeriods.push_back(get_fog_type_wp(fogPeriodStartTime,
															  theFogPeriods.at(i).first.localEndTime(),
															  currentFogType));
			  }
		  }
		else
		  {
			if(previousFogType != currentFogType)
			  {
				if(previousFogType != NO_FOG)
				  theFogTypePeriods.push_back(get_fog_type_wp(fogPeriodStartTime,
															  theFogPeriods.at(i-1).first.localEndTime(),
															  previousFogType));
				fogPeriodStartTime = theFogPeriods.at(i).first.localStartTime();
				previousFogType = currentFogType;
			  }
		  }
	  }
  }

  void FogForecast::findOutFogTypePeriods()
  {
	findOutFogTypePeriods(theCoastalFog, theCoastalFogType);
	findOutFogTypePeriods(theInlandFog, theInlandFogType);
	findOutFogTypePeriods(theFullAreaFog, theFullAreaFogType);
  }

  void FogForecast::printOutFogPeriods(std::ostream& theOutput,
									   const fog_period_vector& theFogPeriods) const
  {
	for(unsigned int i = 0; i < theFogPeriods.size(); i++)
	  {
		WeatherPeriod period(theFogPeriods.at(i).first.localStartTime(),
							 theFogPeriods.at(i).first.localEndTime());

		float moderateFog(theFogPeriods.at(i).second.theModerateFogExtent);
		float denseFog(theFogPeriods.at(i).second.theDenseFogExtent);

		theOutput << period.localStartTime()
				  << "..."
				  << period.localEndTime()
				  << ": moderate=" << moderateFog
				  << " dense=" << denseFog
				  << endl;
	  }
  }
  
  void FogForecast::printOutFogPeriods(std::ostream& theOutput) const
  {
	theOutput << "** FOG PERIODS **" << endl;
	
	if(theCoastalFog.size() > 0)
	  {
		theOutput << "Coastal fog: " << endl; 
		printOutFogPeriods(theOutput, theCoastalFog);
	  }
	if(theInlandFog.size() > 0)
	  {
		theOutput << "Inland fog: " << endl; 
		printOutFogPeriods(theOutput, theInlandFog);
	  }
	if(theFullAreaFog.size() > 0)
	  {
		theOutput << "Full area fog: " << endl; 
		printOutFogPeriods(theOutput, theFullAreaFog);
	  }
  }
  
  void FogForecast::printOutFogTypePeriods(std::ostream& theOutput,
										   const fog_type_period_vector& theFogTypePeriods) const
  {
	for(unsigned int i = 0; i < theFogTypePeriods.size(); i++)
	  {
		WeatherPeriod period(theFogTypePeriods.at(i).first.localStartTime(),
							 theFogTypePeriods.at(i).first.localEndTime());

		theOutput << period.localStartTime()
				  << "..."
				  << period.localEndTime()
				  << ": "
				  << get_fog_type_string(theFogTypePeriods.at(i).second)
				  << endl;
	  }
  }

  void FogForecast::printOutFogTypePeriods(std::ostream& theOutput) const
  {
	theOutput << "** FOG TYPE PERIODS **" << endl;
	
	if(theCoastalFog.size() > 0)
	  {
		theOutput << "Coastal fog types: " << endl; 
		printOutFogTypePeriods(theOutput, theCoastalFogType);
	  }
	if(theInlandFog.size() > 0)
	  {
		theOutput << "Inland fog types: " << endl; 
		printOutFogTypePeriods(theOutput, theInlandFogType);
	  }
	if(theFullAreaFog.size() > 0)
	  {
		theOutput << "Full area fog types: " << endl; 
		printOutFogTypePeriods(theOutput, theFullAreaFogType);
	  }
  }

  float FogForecast::getMean(const fog_period_vector& theFogPeriods,
							 const WeatherPeriod& theWeatherPeriod) const
  {
	float sum(0.0);
	unsigned int count(0);

	for(unsigned int i = 0; i < theFogPeriods.size(); i++)
	  {
		float totalFog = theFogPeriods.at(i).second.theModerateFogExtent + theFogPeriods.at(i).second.theDenseFogExtent;
		if(theFogPeriods.at(i).first.localStartTime() >= theWeatherPeriod.localStartTime() &&
		   theFogPeriods.at(i).first.localStartTime() <= theWeatherPeriod.localEndTime() &&
		   theFogPeriods.at(i).first.localEndTime() >= theWeatherPeriod.localStartTime() &&
		   theFogPeriods.at(i).first.localEndTime() <= theWeatherPeriod.localEndTime() &&
		   totalFog > 0)
		  {
			sum += totalFog;
			count++;
		  }
	  }
	
	return (count > 0 ? sum / count : 0);
  }

  bool FogForecast::separateCoastInlandFog(const WeatherPeriod& theWeatherPeriod) const
  {
	float coastalFogAvgExtent(getMean(theCoastalFog, theWeatherPeriod));
	float inlandFogAvgExtent(getMean(theInlandFog, theWeatherPeriod));
	
	if(abs(coastalFogAvgExtent - inlandFogAvgExtent) > 50 && 
	   coastalFogAvgExtent >= 10.0 && inlandFogAvgExtent >= 10.0 &&
	   !theParameters.theCoastalAndInlandTogetherFlag)
	  {
		return true;
	  }
	
	return false;
  }

  Sentence FogForecast::getFogPhrase(const fog_type_id& theFogTypeId) const
  {
	Sentence sentence;

	switch(theFogTypeId)
	  {
	  case FOG:
		sentence << SUMUA_WORD;
		break;
	  case FOG_POSSIBLY_DENSE:
		sentence << SUMUA_WORD << Delimiter(",") << JOKA_VOI_OLLA_SAKEAA_PHRASE;
		break;
	  case FOG_IN_SOME_PLACES:
		sentence << PAIKOIN_WORD << SUMUA_WORD;
		break;
	  case FOG_IN_SOME_PLACES_POSSIBLY_DENSE:
		sentence << PAIKOIN_WORD << SUMUA_WORD << Delimiter(",") << JOKA_VOI_OLLA_SAKEAA_PHRASE;
		break;
	  case FOG_IN_MANY_PLACES:
		sentence << MONIN_PAIKOIN_WORD << SUMUA_WORD;
		break;
	  case FOG_IN_MANY_PLACES_POSSIBLY_DENSE:
		sentence << MONIN_PAIKOIN_WORD << SUMUA_WORD << Delimiter(",") << JOKA_VOI_OLLA_SAKEAA_PHRASE;
		break;
	  default:
		break;
	  }
	return sentence;
  }

  WeatherPeriod FogForecast::getActualFogPeriod(const WeatherPeriod& theForecastPeriod, const WeatherPeriod& theFogPeriod) const
  {
	if(is_inside(theFogPeriod.localStartTime(), theForecastPeriod) && 
	   !is_inside(theFogPeriod.localEndTime(), theForecastPeriod))
	  return(WeatherPeriod(theFogPeriod.localStartTime(), theForecastPeriod.localEndTime()));
	else if(is_inside(theFogPeriod.localEndTime(), theForecastPeriod) && 
			!is_inside(theFogPeriod.localStartTime(), theForecastPeriod))
	  return(WeatherPeriod(theForecastPeriod.localStartTime(), theFogPeriod.localEndTime()));
	else if(theFogPeriod.localStartTime() <=  theForecastPeriod.localStartTime() &&
			theFogPeriod.localEndTime() >=  theForecastPeriod.localEndTime())
	  return theForecastPeriod;
	  
	return theFogPeriod;
  }

#ifdef OLD_IMPLEMENTATION
  Sentence FogForecast::fogSentence(const WeatherPeriod& thePeriod,
									const fog_type_period_vector& theFogTypePeriods) const
  {
	Sentence sentence;

	for(unsigned int i = 0; i < theFogTypePeriods.size(); i++)
	  {		
		WeatherPeriod actualFogPeriod(getActualFogPeriod(thePeriod, theFogTypePeriods.at(i).first));

		if(thePeriod.localStartTime() <= actualFogPeriod.localStartTime() &&
		   thePeriod.localEndTime() >= actualFogPeriod.localStartTime() &&
		   thePeriod.localStartTime() <= actualFogPeriod.localEndTime() &&
		   thePeriod.localEndTime() >= actualFogPeriod.localEndTime())
		  {
			theParameters.theLog << "Actual fog-period: " 
								 << actualFogPeriod.localStartTime() << "..." 
								 << actualFogPeriod.localEndTime()  << endl;

			Sentence fogSentence;
			Sentence todayPhrase;
			fogSentence << getFogPhrase(theFogTypePeriods.at(i).second);
			if(!fogSentence.empty())
			  {
				if(theParameters.thePrecipitationForecast->isDryPeriod(actualFogPeriod,
																	   theParameters.theForecastArea))
				  {
					if(thePeriod.localEndTime().DifferenceInHours(thePeriod.localStartTime()) > 24)
					  {
						todayPhrase << PeriodPhraseFactory::create("today",
																	 theParameters.theVariable,
																	 theParameters.theForecastTime,
																	 thePeriod,
																	 theParameters.theArea);

					  }
					vector<std::string> theStringVector;
					Sentence timePhrase(get_time_phrase_large(actualFogPeriod, 
															  theParameters.theVariable, 
															  false, 
															  &theStringVector));
					std::string timePhraseString;
					for(unsigned int k = 0; k < theStringVector.size(); k++)
					  timePhraseString += theStringVector[k];

					bool todayPhraseExists = todayPhrase.size() > 0;
					bool timePhraseExists = false;
					sentence << todayPhrase;
					if(timePhraseString != thePreviousTimePhrase)
					  {
						timePhraseExists = timePhrase.size() > 0;
						sentence << timePhrase;
						thePreviousTimePhrase = timePhraseString;
					  }
					if(todayPhraseExists || timePhraseExists)
					  sentence << ON_WORD;
					sentence << fogSentence;
					if(!(theParameters.theForecastArea & FULL_AREA))
					  sentence <<  areaSpecificSentence(thePeriod);
				  }
			  }

			break;
		  }
	  }

	return sentence;
  }
#endif

  void FogForecast::getFogPeriodAndId(const fog_type_period_vector& theFogTypePeriods,
									  WeatherPeriod& thePeriod,
									  fog_type_id theFogTypeId) const
  {
	if(theFogTypePeriods.size() == 1)
	  {
		thePeriod = getActualFogPeriod(thePeriod, theFogTypePeriods.at(0).first);
		theFogTypeId = theFogTypePeriods.at(0).second;
	  }
	else
	  {
		// Merge close periods. If one long fog-period use that and ignore the small ones,
		// otherwise theFogTypeId is weighted average of all fog-periods and the
		// returned fog-period encompasses all small periods.
		for(unsigned int i = 0; i < theFogTypePeriods.size(); i++)
		  {		
			WeatherPeriod actualFogPeriod(getActualFogPeriod(thePeriod, theFogTypePeriods.at(i).first));
			
			if(actualFogPeriod.localEndTime().DifferenceInHours(actualFogPeriod.localStartTime()) == 1 &&
			   (actualFogPeriod.localStartTime() == thePeriod.localStartTime() ||
				actualFogPeriod.localEndTime() == thePeriod.localEndTime()))
			  continue;
		  
		  }

	  }
  }

  Sentence FogForecast::fogSentence(const WeatherPeriod& thePeriod,
									const fog_type_period_vector& theFogTypePeriods,
									const std::string& theAreaString) const
  {
	Sentence sentence;
	std::string dayPhasePhrase;
	Sentence todayPhrase;
	std::string fogPhrase;

	for(unsigned int i = 0; i < theFogTypePeriods.size(); i++)
	  {		
		WeatherPeriod actualFogPeriod(getActualFogPeriod(thePeriod, theFogTypePeriods.at(i).first));
		
		if(actualFogPeriod.localEndTime().DifferenceInHours(actualFogPeriod.localStartTime()) == 1 &&
		   (actualFogPeriod.localStartTime() == thePeriod.localStartTime() ||
			actualFogPeriod.localEndTime() == thePeriod.localEndTime()))
		   continue;
		  

		if(thePeriod.localStartTime() <= actualFogPeriod.localStartTime() &&
		   thePeriod.localEndTime() >= actualFogPeriod.localStartTime() &&
		   thePeriod.localStartTime() <= actualFogPeriod.localEndTime() &&
		   thePeriod.localEndTime() >= actualFogPeriod.localEndTime())
		  {
			theParameters.theLog << "Actual fog-period: " 
								 << actualFogPeriod.localStartTime() << "..." 
								 << actualFogPeriod.localEndTime()  << endl;

			Sentence fogSentence;

			fog_type_id fogTypeId(theFogTypePeriods.at(i).second);

			if(fogTypeId != NO_FOG)
			  {
				if(theParameters.thePrecipitationForecast->isDryPeriod(actualFogPeriod,
																	   theParameters.theForecastArea))
				  {
					short dayNumber = 0;
					if(thePeriod.localEndTime().DifferenceInHours(thePeriod.localStartTime()) > 24)
					  {
						todayPhrase << PeriodPhraseFactory::create("today",
																   theParameters.theVariable,
																   theParameters.theForecastTime,
																   actualFogPeriod,
																   theParameters.theArea);
						dayNumber = actualFogPeriod.localStartTime().GetWeekday();
					  }

					theParameters.theLog << todayPhrase;
					
					vector<std::string> theStringVector;

					bool specifyDay = get_period_length(theParameters.theForecastPeriod) > 24 &&
					  todayPhrase.size() > 0;

					Sentence partOfTheDay(get_time_phrase_large(actualFogPeriod,
																specifyDay,
																theParameters.theVariable, 
																true, 
																&theStringVector));

					Sentence timePhrase(parse_weekday_phrase(dayNumber, partOfTheDay));


					for(unsigned int k = 0; k < theStringVector.size(); k++)
					  dayPhasePhrase += theStringVector[k];

					if(dayPhasePhrase != theDayPhasePhraseOld)
					  {
						theDayPhasePhraseOld = dayPhasePhrase;
					  }

					switch(fogTypeId)
					  {
					  case FOG:
						fogSentence << PAIKOIN_SUMUA_COMPOSITE_PHRASE
									<< timePhrase
									<< theAreaString
									<< EMPTY_STRING;
						break;
					  case FOG_POSSIBLY_DENSE:
						fogSentence << PAIKOIN_SUMUA_JOKAVOIOLLA_SAKEAA_COMPOSITE_PHRASE
									<< timePhrase
									<< theAreaString
									<< EMPTY_STRING;
						break;
					  case FOG_IN_SOME_PLACES:
						fogSentence << PAIKOIN_SUMUA_COMPOSITE_PHRASE
									<< timePhrase
									<< theAreaString
									<< PAIKOIN_WORD;
						break;
					  case FOG_IN_SOME_PLACES_POSSIBLY_DENSE:
						fogSentence << PAIKOIN_SUMUA_JOKAVOIOLLA_SAKEAA_COMPOSITE_PHRASE
									<< timePhrase
									<< theAreaString
									<< PAIKOIN_WORD;
						break;
					  case FOG_IN_MANY_PLACES:
						fogSentence << PAIKOIN_SUMUA_COMPOSITE_PHRASE
									<< timePhrase
									<< theAreaString
									<< MONIN_PAIKOIN_WORD;
						break;
					  case FOG_IN_MANY_PLACES_POSSIBLY_DENSE:
						fogSentence << PAIKOIN_SUMUA_JOKAVOIOLLA_SAKEAA_COMPOSITE_PHRASE
									<< timePhrase
									<< theAreaString
									<< MONIN_PAIKOIN_WORD;
						break;
					  default:
						break;
					  };
					if(fogSentence.size() > 0)
					  {
						if(sentence.size() > 0)
						  sentence << Delimiter(",");
						sentence << fogSentence;
					  }
					//break;
				  }
			  }
		  }
	  }

	return sentence;
  }

  Sentence FogForecast::fogSentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

	if(theParameters.theForecastArea & FULL_AREA)
	  {
		if(separateCoastInlandFog(thePeriod))
		  {
			sentence << COAST_PHRASE;
			sentence << fogSentence(thePeriod, theCoastalFogType, COAST_PHRASE);
			sentence << Delimiter(",");
			sentence << INLAND_PHRASE;
			sentence << fogSentence(thePeriod, theInlandFogType, INLAND_PHRASE);
		  }
		else
		  {
			sentence << fogSentence(thePeriod, theFullAreaFogType, EMPTY_STRING);
		  }
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		sentence << fogSentence(thePeriod, theInlandFogType, EMPTY_STRING);
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		sentence << fogSentence(thePeriod, theCoastalFogType, EMPTY_STRING);
	  }

	theDayPhasePhraseOld.clear();

	return sentence;
  }

#ifdef OLD_IMPL
  Sentence FogForecast::fogSentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

	if(theParameters.theForecastArea & FULL_AREA)
	  {
		if(separateCoastInlandFog(thePeriod))
		  {
			sentence << COAST_PHRASE;
			sentence << fogSentence(thePeriod, theCoastalFogType);
			sentence << Delimiter(",");
			sentence << INLAND_PHRASE;
			sentence << fogSentence(thePeriod, theInlandFogType);
		  }
		else
		  {
			sentence << fogSentence(thePeriod, theFullAreaFogType);
		  }
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		sentence << fogSentence(thePeriod, theInlandFogType);
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		sentence << fogSentence(thePeriod, theCoastalFogType);
	  }
	thePreviousTimePhrase.clear();

	return sentence;
  }
#endif

  Sentence FogForecast::areaSpecificSentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

	// If the area contains both coast and inland, we don't ude area specific sentence

	if(!(theParameters.theForecastArea & FULL_AREA))
	  {
		WeatherResult northEastShare(kFloatMissing, 0);
		WeatherResult southEastShare(kFloatMissing, 0);
		WeatherResult southWestShare(kFloatMissing, 0);
		WeatherResult northWestShare(kFloatMissing, 0);
		RangeAcceptor acceptor;
		acceptor.lowerLimit(kTModerateFog);
		acceptor.upperLimit(kTDenseFog);

		AreaTools::getArealDistribution(theParameters.theSources,
										Fog,
										theParameters.theArea,
										thePeriod,
										acceptor,
										northEastShare,
										southEastShare,
										southWestShare,
										northWestShare);

		float north = northEastShare.value() + northWestShare.value();
		float south = southEastShare.value() + southWestShare.value();
		float east = northEastShare.value() + southEastShare.value();
		float west = northWestShare.value() + southWestShare.value();

		area_specific_sentence_id sentenceId = 
		  get_area_specific_sentence_id(north,
										south,
										east,
										west,
										northEastShare.value(),
										southEastShare.value(),
										southWestShare.value(),
										northWestShare.value(),
										false);

		Rect areaRect(theParameters.theArea);
		NFmiMercatorArea mercatorArea(areaRect.getBottomLeft(), areaRect.getTopRight());
		float areaHeightWidthRatio = mercatorArea.WorldRect().Height()/mercatorArea.WorldRect().Width();
		
		Sentence areaSpecificSentence;
		areaSpecificSentence << area_specific_sentence(north,
													   south,
													   east,
													   west,
													   northEastShare.value(),
													   southEastShare.value(),
													   southWestShare.value(),
													   northWestShare.value(),
													   false);
		
		// If the area is too cigar-shaped, we can use only north-south/east-west specifier
		if((areaHeightWidthRatio >= 0.6 && 
			areaHeightWidthRatio <= 1.5) ||
		   (areaHeightWidthRatio < 0.6 &&
			(sentenceId == ALUEEN_ITAOSISSA || 
			 sentenceId == ALUEEN_LANSIOSISSA ||
			 sentenceId == ENIMMAKSEEN_ALUEEN_ITAOSISSA ||
			 sentenceId == ENIMMAKSEEN_ALUEEN_LANSIOSISSA)) ||
		   (areaHeightWidthRatio > 1.5 &&
			(sentenceId == ALUEEN_POHJOISOSISSA || 
			 sentenceId == ALUEEN_ETELAOSISSA ||
			 sentenceId == ENIMMAKSEEN_ALUEEN_POHJOISOSISSA ||
			 sentenceId == ENIMMAKSEEN_ALUEEN_ETELAOSISSA)))
		  {
			sentence << areaSpecificSentence;
		  }
	  }

	return sentence;
  }

}
