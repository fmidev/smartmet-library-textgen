// ======================================================================
/*!
 * \file
 * \brief Implementation of PrecipitationForecast class
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
#include "PrecipitationForecast.h"
#include "CloudinessForecast.h"
#include "ThunderForecast.h"

#include "NFmiMercatorArea.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <map>
#include <iomanip>

namespace TextGen
{

  using namespace Settings;
  using namespace WeatherAnalysis;
  using namespace AreaTools;
  using namespace boost;
  using namespace std;

  std::ostream& operator<<(std::ostream & theOutput,
						   const PrecipitationDataItemData& thePrecipitationDataItemData)
  {
	theOutput << thePrecipitationDataItemData.theObservationTime << ": ";
	theOutput << setw(8) << setfill(' ') << fixed << setprecision(3) << thePrecipitationDataItemData.thePrecipitationIntensity;
	theOutput << setw(8) << setfill(' ') << fixed << setprecision(1) 
			  << thePrecipitationDataItemData.thePrecipitationExtent;
	if(thePrecipitationDataItemData.thePrecipitationIntensity > 0)
	  {
		theOutput << " ";
		if(thePrecipitationDataItemData.thePrecipitationFormWater > 0.0)
		  theOutput << "   water=" << setw(3) << setfill(' ') << fixed << setprecision(0) 
					<< thePrecipitationDataItemData.thePrecipitationFormWater;
		if(thePrecipitationDataItemData.thePrecipitationFormDrizzle > 0.0)
		  theOutput << " drizzle=" << setw(3) << setfill(' ') << fixed << setprecision(0) 
					<< thePrecipitationDataItemData.thePrecipitationFormDrizzle;
		if(thePrecipitationDataItemData.thePrecipitationFormSleet > 0.0)
		  theOutput << "   sleet=" << setw(3) << setfill(' ') << fixed << setprecision(0) 
					<< thePrecipitationDataItemData.thePrecipitationFormSleet;
		if( thePrecipitationDataItemData.thePrecipitationFormSnow > 0.0)
		  theOutput << "    snow=" << setw(3) << setfill(' ') << fixed << setprecision(0) 
					<< thePrecipitationDataItemData.thePrecipitationFormSnow;
		if(thePrecipitationDataItemData.thePrecipitationFormFreezing > 0.0)
		  theOutput << "freezing=" << setw(3) << setfill(' ') << fixed << setprecision(0) 
					<< thePrecipitationDataItemData.thePrecipitationFormFreezing;
		if(thePrecipitationDataItemData.thePrecipitationTypeShowers > 0.0)
		  theOutput << " showers=" << setw(3) << setfill(' ') << fixed << setprecision(0) 
					<< thePrecipitationDataItemData.thePrecipitationTypeShowers;

		theOutput << " ne=" << fixed << setprecision(0) 
				  << thePrecipitationDataItemData.thePrecipitationPercentageNorthEast << " "
				  << "se=" << fixed << setprecision(0) 
				  << thePrecipitationDataItemData.thePrecipitationPercentageSouthEast << " "
				  << "sw=" << fixed << setprecision(0) 
				  << thePrecipitationDataItemData.thePrecipitationPercentageSouthWest << " "
				  << "nw=" << fixed << setprecision(0) 
				  << thePrecipitationDataItemData.thePrecipitationPercentageNorthWest << " ";

		theOutput << " coord=" << setw(3) << setfill(' ') << fixed << setprecision(2) 
				  << thePrecipitationDataItemData.thePrecipitationPoint.X() 
				  << ", " 
				  << setw(3) << setfill(' ') << fixed << setprecision(2) 
				  << thePrecipitationDataItemData.thePrecipitationPoint.Y() << endl;
	  }
	else
	  {
		theOutput << endl;
	  }

	return theOutput;
  }

  std::string operator<<(std::string& theDestinationString, const std::string& theSourceString)
  {
	theDestinationString.append(theSourceString);
	//	theDestinationString.append(" ");

	return theDestinationString;
  }


  bool get_period_start_end_index(const WeatherPeriod& thePeriod, 
								  const precipitation_data_vector& theDataVector,
								  unsigned int& theStartIndex,
								  unsigned int& theEndIndex)
  {
	theStartIndex = 0;
	theEndIndex = 0;

	bool startFound = false;
	for(unsigned int i = 0; i < theDataVector.size(); i++)
	  {
		if(theDataVector.at(i)->theObservationTime >= thePeriod.localStartTime() && 
		   theDataVector.at(i)->theObservationTime <= thePeriod.localEndTime())
		  {
			if(!startFound)
			  {
				startFound = true;
				theStartIndex = i;
			  }
			if(startFound)
			  theEndIndex = i;
		  }
	  }
	return startFound;
  }

  weather_result_data_item_vector* get_data_vector(wf_story_params& theParameters,
												   const weather_result_data_id& theDataId)
  {
	weather_result_data_item_vector* retval = 0;

	forecast_area_id areaId(NO_AREA);
	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  areaId = FULL_AREA;
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  areaId = COASTAL_AREA;
	else if(theParameters.theForecastArea & INLAND_AREA)
	  areaId = INLAND_AREA;

	if(areaId != NO_AREA)
	  retval = (*theParameters.theCompleteData[areaId])[theDataId];

	return retval;
  }

  bool get_period_start_end_index(const WeatherPeriod& thePeriod,
								  const weather_result_data_item_vector& theDataVector,
								  unsigned int& theStartIndex,
								  unsigned int& theEndIndex)
  {
	theStartIndex = 0;
	theEndIndex = 0;

	bool startFound = false;
	for(unsigned int i = 0; i < theDataVector.size(); i++)
	  {
		// Note: in weather result vector start and end times are same
		if(theDataVector.at(i)->thePeriod.localStartTime() >= thePeriod.localStartTime() && 
		   theDataVector.at(i)->thePeriod.localStartTime() <= thePeriod.localEndTime())
		  {
			if(!startFound)
			  {
				startFound = true;
				theStartIndex = i;
			  }
			if(startFound)
			  theEndIndex = i;
		  }
	  }
	return startFound;
  }

  std::string in_places_phrase(const bool& inSomePlaces, 
							   const bool& inManyPlaces, 
							   const bool& useOllaVerbFlag)
  {
	std::string phrase;

	if(inSomePlaces || inManyPlaces)
	  {
		if(useOllaVerbFlag)
		  {
			phrase << " ";
			phrase << SAADAAN_WORD;
		  }
		phrase << (inSomePlaces ? PAIKOIN_WORD : (inManyPlaces ? MONIN_PAIKOIN_WORD : ""));
	  }
	return phrase;
  }

  void in_places_phrase(const bool& inSomePlaces, 
						const bool& inManyPlaces, 
						const bool& useOllaVerbFlag,
						vector<string>& stringVector)
  {
	if(inSomePlaces || inManyPlaces)
	  {
		if(useOllaVerbFlag)
		  {
			stringVector.push_back(SAADAAN_WORD);
		  }
		stringVector.push_back((inSomePlaces ? PAIKOIN_WORD : MONIN_PAIKOIN_WORD));
	  }
  }

  void can_be_freezing_phrase(const bool& theCanBeFreezingFlag, 
							  vector<std::string>& theStringVector,
							  bool thePluralFlag)
  {
	if(theCanBeFreezingFlag)
	  {
		theStringVector.push_back(",");
		if(thePluralFlag)
		  theStringVector.push_back(JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE);
		else
		  theStringVector.push_back(JOKA_VOI_OLLA_JAATAVAA_PHRASE);
	  }
  }

  bool is_dry_weather(const wf_story_params& theParameters,
					  const unsigned int& thePrecipitationForm,
					  const float& thePrecipitationIntensity,
					  const float& thePrecipitationExtent)
  {
	if(thePrecipitationExtent == 0.0)
	  return true;

	bool dry_weather = false;

	switch(thePrecipitationForm)
	  {
	  case WATER_FREEZING_FORM:
	  case FREEZING_FORM:
	  case WATER_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitWater)
		  dry_weather = true;
		break;
	  case SLEET_FREEZING_FORM:
	  case SLEET_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitSleet)
		  dry_weather = true;
		break;
	  case SNOW_FORM:
	  case SNOW_FREEZING_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitSnow)
		  dry_weather = true;
		break;
	  case DRIZZLE_FORM:
	  case DRIZZLE_FREEZING_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitDrizzle)
		  dry_weather = true;
		break;
	  case WATER_DRIZZLE_FREEZING_FORM:
	  case WATER_DRIZZLE_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitWater)
		  dry_weather = true;
		break;
	  case DRIZZLE_SLEET_FORM:
	  case DRIZZLE_SLEET_FREEZING_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitDrizzle &&
		   thePrecipitationIntensity < theParameters.theDryWeatherLimitSleet)
		  dry_weather = true;
		break;
	  case WATER_DRIZZLE_SLEET_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitWater)
		  dry_weather = true;
		break;
	  case WATER_SLEET_FREEZING_FORM:
	  case WATER_SLEET_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitWater)
		  dry_weather = true;
		break;
	  case WATER_SNOW_FREEZING_FORM:
	  case WATER_SNOW_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitWater)
		  dry_weather = true;
		break;
	  case DRIZZLE_SNOW_FREEZING_FORM:
	  case DRIZZLE_SNOW_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitDrizzle)
		  dry_weather = true;
		break;
	  case WATER_DRIZZLE_SNOW_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitWater)
		  dry_weather = true;
		break;
	  case WATER_SLEET_SNOW_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitWater)
		  dry_weather = true;
		break;
	  case DRIZZLE_SLEET_SNOW_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitDrizzle)
		  dry_weather = true;
		break;
	  case SLEET_SNOW_FREEZING_FORM:
	  case SLEET_SNOW_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitSleet)
		  dry_weather = true;
		break;
	  case MISSING_PRECIPITATION_FORM:
		dry_weather = true;
		break;
	  }

	return dry_weather;
  }

  void PrecipitationForecast::waterAndSnowShowersPhrase(const float& thePrecipitationIntensity,
														const float thePrecipitationIntensityAbsoluteMax,
														const float& theWaterDrizzleSleetShare,
														const bool& theCanBeFreezingFlag,
														vector<std::string>& theStringVector) const
  {
	// when showers, dont use sleet, use water and snow instead (Sari Hartonen)
	// and if there is no heavy rain
	if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow && 
	   thePrecipitationIntensityAbsoluteMax < theParameters.theHeavyPrecipitationLimitSnow)
	  {
		theStringVector.push_back(HEIKKOJA_WORD);
	  }
	else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
	  {
		theStringVector.push_back(RUNSAITA_WORD);
	  }

	if(theCanBeFreezingFlag)
	  {
		theStringVector.push_back(LUMI_TAVUVIIVA_WORD);
		theStringVector.push_back(TAI_WORD);
		theStringVector.push_back(VESIKUUROJA_WORD);
		can_be_freezing_phrase(theCanBeFreezingFlag, theStringVector, true);
	  }
	else
	  {
		if(theWaterDrizzleSleetShare > MAJORITY_LIMIT)
		  {
			theStringVector.push_back(VESI_TAVUVIIVA_WORD);
			theStringVector.push_back(TAI_WORD);
			theStringVector.push_back(LUMIKUUROJA_WORD);
		  }
		else
		  {
			theStringVector.push_back(LUMI_TAVUVIIVA_WORD);
			theStringVector.push_back(TAI_WORD);
			theStringVector.push_back(VESIKUUROJA_WORD);
		  }
	  }
  }

  void PrecipitationForecast::mostlyDryWeatherPhrase(const bool& theIsShowersFlag,
													 const WeatherPeriod& thePeriod,
													 const char* thePhrase,
													 vector<std::string>& theStringVector) const
  {
	if(!theDryPeriodTautologyFlag)
	  {
		theStringVector.push_back(SAA_WORD);
		theStringVector.push_back(ON_WORD);
		theStringVector.push_back(ENIMMAKSEEN_WORD);
		theStringVector.push_back(POUTAINEN_WORD);
		if(!theDryPeriodTautologyFlag)
		  theStringVector.push_back(",");
	  }
	/*	
	bool alkaenPhrase = (get_period_length(thePeriod) >= 6 &&
						 thePeriod.localStartTime() != theParameters.theForecastPeriod.localStartTime());
	get_time_phrase_large(thePeriod, alkaenPhrase, &theStringVector);
	*/
	theStringVector.push_back(thePhrase);
  }

  int PrecipitationForecast::precipitationSentenceStringVectorTransformation(const WeatherPeriod& thePeriod,
																			 const float& thePrecipitationExtent,
																			 const precipitation_form_transformation_id& theTransformId,
																			 vector<std::string>& theStringVector) const
  {
	bool is_summer = SeasonTools::isSummer(thePeriod.localStartTime(), theParameters.theVariable);
	const bool in_some_places = thePrecipitationExtent > theParameters.theInSomePlacesLowerLimit && 
	  thePrecipitationExtent <= theParameters.theInSomePlacesUpperLimit;
	const bool in_many_places = thePrecipitationExtent > theParameters.theInManyPlacesLowerLimit && 
	  thePrecipitationExtent <= theParameters.theInManyPlacesUpperLimit;
	
	string rain_phrase(is_summer ? SADETTA_WORD : VESISADETTA_WORD);

	in_places_phrase(in_some_places,
					 in_many_places,
					 theUseOllaVerb,
					 theStringVector);

	switch(theTransformId)
	  {
	  case WATER_TO_SNOW:
	  case WATER_TO_DRIZZLE:
	  case WATER_TO_SLEET:
		{
		  theStringVector.push_back(rain_phrase);
		  theStringVector.push_back(",");
		  
		  if(theTransformId == WATER_TO_SNOW)
			theStringVector.push_back(JOKA_MUUTTUU_LUMISATEEKSI_PHRASE);
		  else if(theTransformId == WATER_TO_DRIZZLE)
			theStringVector.push_back(JOKA_MUUTTUU_TIHKUSATEEKSI_PHRASE);
		  else
			theStringVector.push_back(JOKA_MUUTTUU_RANTASATEEKSI_PHRASE);
		}
		break;
	  case SNOW_TO_WATER:
	  case SNOW_TO_DRIZZLE:
	  case SNOW_TO_SLEET:
		{
		  theStringVector.push_back(LUMISADETTA_WORD);
		  theStringVector.push_back(",");
		  
		  if(theTransformId == SNOW_TO_WATER)
			theStringVector.push_back(JOKA_MUUTTUU_VESISATEEKSI_PHRASE);
		  else if(theTransformId == SNOW_TO_DRIZZLE)
			theStringVector.push_back(JOKA_MUUTTUU_TIHKUSATEEKSI_PHRASE);
		  else
			theStringVector.push_back(JOKA_MUUTTUU_RANTASATEEKSI_PHRASE);
		}
		break;
	  case DRIZZLE_TO_WATER:
	  case DRIZZLE_TO_SNOW:
	  case DRIZZLE_TO_SLEET:
		{
		  theStringVector.push_back(TIHKUSADETTA_WORD);
		  theStringVector.push_back(",");

		  if(theTransformId == DRIZZLE_TO_WATER)
			theStringVector.push_back(JOKA_MUUTTUU_VESISATEEKSI_PHRASE);
		  else if(theTransformId == DRIZZLE_TO_SNOW)
			theStringVector.push_back(JOKA_MUUTTUU_LUMISATEEKSI_PHRASE);
		  else
			theStringVector.push_back(JOKA_MUUTTUU_RANTASATEEKSI_PHRASE);
		}
		break;
	  case SLEET_TO_WATER:
	  case SLEET_TO_DRIZZLE:
	  case SLEET_TO_SNOW:
		{
		  theStringVector.push_back(RANTASADETTA_WORD);
		  theStringVector.push_back(",");

		  if(theTransformId == SLEET_TO_WATER)
			theStringVector.push_back(JOKA_MUUTTUU_VESISATEEKSI_PHRASE);
		  else if(theTransformId ==SLEET_TO_DRIZZLE)
			theStringVector.push_back(JOKA_MUUTTUU_TIHKUSATEEKSI_PHRASE);
		  else
			theStringVector.push_back(JOKA_MUUTTUU_LUMISATEEKSI_PHRASE);
		}
		break;
	  default:
		break;
	  }
 
	return theStringVector.size();
  }

  int PrecipitationForecast::precipitationSentenceStringVector(const WeatherPeriod& thePeriod,
															   const unsigned int& thePrecipitationForm,
															   const float& thePrecipitationIntensity,
															   const float thePrecipitationIntensityAbsoluteMax,
															   const float& thePrecipitationExtent,
															   const float& thePrecipitationFormWater,
															   const float& thePrecipitationFormDrizzle,
															   const float& thePrecipitationFormSleet,
															   const float& thePrecipitationFormSnow,
															   const float& thePrecipitationFormFreezing,
															   const precipitation_type& thePrecipitationType,
															   const NFmiTime& theTypeChangeTime,
															   vector<std::string>& theStringVector) const
  {
	bool dry_weather = is_dry_weather(theParameters, thePrecipitationForm, thePrecipitationIntensity, thePrecipitationExtent);

	if(dry_weather)
	  {
		theStringVector.push_back(POUTAINEN_WORD);
		theDryPeriodTautologyFlag = true;
	  }
	else
	  {
		bool is_summer = SeasonTools::isSummer(thePeriod.localStartTime(), theParameters.theVariable);
		string rain_phrase(is_summer ? SADETTA_WORD : VESISADETTA_WORD);

		const bool is_showers = thePrecipitationType == SHOWERS;
		const bool mostly_dry_weather = thePrecipitationExtent <= theParameters.theMostlyDryWeatherLimit;
		const bool in_some_places = thePrecipitationExtent > theParameters.theInSomePlacesLowerLimit && 
		  thePrecipitationExtent <= theParameters.theInSomePlacesUpperLimit;
		const bool in_many_places = thePrecipitationExtent > theParameters.theInManyPlacesLowerLimit && 
		  thePrecipitationExtent <= theParameters.theInManyPlacesUpperLimit;
		  		
		bool can_be_freezing =  thePrecipitationFormFreezing > theParameters.theFreezingPrecipitationLimit;

		theParameters.theLog << "Precipitation form is " 
							 << precipitation_form_string(static_cast<precipitation_form_id>(thePrecipitationForm)) 
							 << endl;

		switch(thePrecipitationForm)
		  {
		  case WATER_FREEZING_FORM:
		  case FREEZING_FORM:
		  case WATER_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitWater)
				{
				  if(mostly_dry_weather)
					{
					  mostlyDryWeatherPhrase(is_showers, 
											 thePeriod,
											 (is_summer ? YKSITTAISET_SADEKUUROT_MAHDOLLISIA_PHRASE : YKSITTAISET_VESIKUUROT_MAHDOLLISIA_PHRASE),
											 theStringVector);

					}
				  else
					{
					  in_places_phrase(in_some_places,
									   in_many_places,
									   theUseOllaVerb,
									   theStringVector);
				  
					  if(is_showers)
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitWater &&
							 thePrecipitationIntensityAbsoluteMax < theParameters.theHeavyPrecipitationLimitWater)
							{
							  theStringVector.push_back((is_summer ? HEIKKOJA_SADEKUUROJA_PHRASE : HEIKKOJA_VESIKUUROJA_PHRASE));
							}
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitWater)
							{
							  theStringVector.push_back((is_summer ? VOIMAKKAITA_SADEKUUROJA_PHRASE : VOIMAKKAITA_VESIKUUROJA_PHRASE));
							}
						  else
							{
							  theStringVector.push_back((is_summer ? SADEKUUROJA_WORD : VESIKUUROJA_WORD));
							}

						 can_be_freezing_phrase(can_be_freezing, theStringVector, true);
						}
					  else
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitWater &&
							 thePrecipitationIntensityAbsoluteMax < theParameters.theHeavyPrecipitationLimitWater)
							{
							  theStringVector.push_back((is_summer ? HEIKKOA_SADETTA_PHRASE : HEIKKOA_VESISADETTA_PHRASE));
							}
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitWater)
							{
							  theStringVector.push_back((is_summer ? RUNSASTA_SADETTA_PHRASE : RUNSASTA_VESISADETTA_PHRASE));
							}
						  else
							{
							  theStringVector.push_back(rain_phrase);
							}

						  can_be_freezing_phrase(can_be_freezing, theStringVector, false);
						}
					}
				}
			  else
				{
				  theStringVector.push_back(ENIMMAKSEEN_WORD);
				  theStringVector.push_back(POUTAINEN_WORD);
				}
			  break;
			}

		  case SLEET_FREEZING_FORM:
		  case SLEET_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitSleet)
				{
				  if(mostly_dry_weather)
					{
					  mostlyDryWeatherPhrase(is_showers, 
											 thePeriod,
											 YKSITTAISET_RANTAKUUROT_MAHDOLLISIA_PHRASE,
											 theStringVector);
					}
				  else
					{
					  in_places_phrase(in_some_places,
									   in_many_places,
									   theUseOllaVerb,
									   theStringVector);

					  if(is_showers)
						{						
						  theStringVector.push_back(RANTAKUUROJA_WORD);
						}
					  else
						{
						  theStringVector.push_back(RANTASADETTA_WORD);
						  if(can_be_freezing)
							{
							  theStringVector.push_back(JA_WORD);
							  theStringVector.push_back(JAATAVAA_VESISADETTA_PHRASE);
							}
						}
					}
				}
			  else
				{
				  theStringVector.push_back(ENIMMAKSEEN_WORD);
				  theStringVector.push_back(POUTAINEN_WORD);
				}
			  break;
			}
		  case SNOW_FORM:
		  case SNOW_FREEZING_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitSnow)
				{
				  if(mostly_dry_weather)
					{
					  mostlyDryWeatherPhrase(is_showers, 
											 thePeriod,
											 YKSITTAISET_LUMIKUUROT_MAHDOLLISIA_PHRASE,
											 theStringVector);
					}
				  else
					{
					  in_places_phrase(in_some_places,
									   in_many_places,
									   theUseOllaVerb,
									   theStringVector);
					  if(is_showers)
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow &&
							 thePrecipitationIntensityAbsoluteMax < theParameters.theHeavyPrecipitationLimitSnow)
							{
							  theStringVector.push_back(HEIKKOJA_WORD);
							}
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							{
							  theStringVector.push_back(SAKEITA_WORD);
							}

						  theStringVector.push_back(LUMIKUUROJA_WORD);
						}
					  else
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow &&
							 thePrecipitationIntensityAbsoluteMax < theParameters.theHeavyPrecipitationLimitSnow)
							{
							  theStringVector.push_back(HEIKKOA_WORD);
							}
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							{
							  theStringVector.push_back(SAKEAA_WORD);
							}

						  theStringVector.push_back(LUMISADETTA_WORD);
						  if(can_be_freezing)
							{
							  theStringVector.push_back(JA_WORD);
							  theStringVector.push_back(JAATAVAA_VESISADETTA_PHRASE);
							}
						}
					}
				}
			  else
				{
				  theStringVector.push_back(ENIMMAKSEEN_WORD);
				  theStringVector.push_back(POUTAINEN_WORD);
				}
			  break;
			}

		  case DRIZZLE_FORM:
		  case DRIZZLE_FREEZING_FORM:

			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitDrizzle)
				{
				  if(mostly_dry_weather)
					{
					  mostlyDryWeatherPhrase(is_showers, 
											 thePeriod,
											 (is_summer ? YKSITTAISET_SADEKUUROT_MAHDOLLISIA_PHRASE : 
											  YKSITTAISET_VESIKUUROT_MAHDOLLISIA_PHRASE),
											 theStringVector);
					}
				  else
					{
					  in_places_phrase(in_some_places,
									   in_many_places,
									   theUseOllaVerb,
									   theStringVector);

					  if(is_showers)
						{
						  theStringVector.push_back((is_summer ? SADEKUUROJA_WORD : VESIKUUROJA_WORD));
						  can_be_freezing_phrase(can_be_freezing, theStringVector, true);
						}
					  else
						{
						  theStringVector.push_back(TIHKUSADETTA_WORD);
						  can_be_freezing_phrase(can_be_freezing, theStringVector, false);
						}
					}
				}
			  else
				{
				  theStringVector.push_back(ENIMMAKSEEN_WORD);
				  theStringVector.push_back(POUTAINEN_WORD);
				}
			  break;
			}

		  case WATER_DRIZZLE_FREEZING_FORM:
		  case WATER_DRIZZLE_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitDrizzle)
				{
				  if(mostly_dry_weather)
					{
					  mostlyDryWeatherPhrase(is_showers, 
											 thePeriod,
											 (is_summer ? YKSITTAISET_SADEKUUROT_MAHDOLLISIA_PHRASE : 
											  YKSITTAISET_VESIKUUROT_MAHDOLLISIA_PHRASE),
											 theStringVector);
					}
				  else
					{
					  in_places_phrase(in_some_places,
									   in_many_places,
									   theUseOllaVerb,
									   theStringVector);

					  if(is_showers)
						{
						  theStringVector.push_back((is_summer ? SADEKUUROJA_WORD : VESIKUUROJA_WORD));
						  can_be_freezing_phrase(can_be_freezing, theStringVector, true);
						}
					  else
						{
						  if(thePrecipitationFormDrizzle < 70)
							{
							  theStringVector.push_back(rain_phrase);
							}
						  else
							{
							  theStringVector.push_back(TIHKUSADETTA_WORD);
							}
						  can_be_freezing_phrase(can_be_freezing, theStringVector, false);
						}
					}
				}
			  else
				{
				  theStringVector.push_back(ENIMMAKSEEN_WORD);
				  theStringVector.push_back(POUTAINEN_WORD);
				}
			  break;
			}

		  case DRIZZLE_SLEET_FORM:
		  case DRIZZLE_SLEET_FREEZING_FORM:
		  case WATER_DRIZZLE_SLEET_FORM:
		  case WATER_SLEET_FREEZING_FORM:
		  case WATER_SLEET_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitDrizzle)
				{
				  if(mostly_dry_weather)
					{
					  mostlyDryWeatherPhrase(is_showers, 
											 thePeriod,
											 (thePrecipitationFormWater >= thePrecipitationFormSleet ? 
											  YKSITTAISET_VESI_RANTA_KUUROT_MAHDOLLISIA_PHRASE: 
											  YKSITTAISET_RANTA_VESI_KUUROT_MAHDOLLISIA_PHRASE),
											 theStringVector);
					}
				  else
					{
					  in_places_phrase(in_some_places,
									   in_many_places,
									   theUseOllaVerb,
									   theStringVector);

					  if(is_showers)
						{
						  if(thePrecipitationFormWater >= thePrecipitationFormSleet)
							{
							  theStringVector.push_back(VESI_TAVUVIIVA_WORD);
							  theStringVector.push_back(TAI_WORD);
							  theStringVector.push_back(RANTAKUUROJA_WORD);
							}
						  else
							{
							  theStringVector.push_back(RANTA_TAVUVIIVA_WORD);
							  theStringVector.push_back(TAI_WORD);
							  theStringVector.push_back(VESIKUUROJA_WORD);
							}
						}
					  else
						{
						  if(thePrecipitationFormWater >= thePrecipitationFormSleet)
							{
							  theStringVector.push_back(VESI_TAVUVIIVA_WORD);
							  theStringVector.push_back(TAI_WORD);
							  theStringVector.push_back(RANTASADETTA_WORD);
							}
						  else
							{
							  theStringVector.push_back(RANTA_TAVUVIIVA_WORD);
							  theStringVector.push_back(TAI_WORD);
							  theStringVector.push_back(VESISADETTA_WORD);

							  can_be_freezing_phrase(can_be_freezing, theStringVector, false);
							}
						}
					}
				}
			  else
				{
				  theStringVector.push_back(ENIMMAKSEEN_WORD);
				  theStringVector.push_back(POUTAINEN_WORD);
				}
			  break;
			}

		  case WATER_SNOW_FORM:
		  case DRIZZLE_SNOW_FORM:
		  case WATER_DRIZZLE_SNOW_FORM:
		  case DRIZZLE_SLEET_SNOW_FORM:
		  case WATER_SLEET_SNOW_FORM:
		  case DRIZZLE_SNOW_FREEZING_FORM:
		  case WATER_SNOW_FREEZING_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitDrizzle)
				{
				  if(mostly_dry_weather)
					{
					  mostlyDryWeatherPhrase(is_showers, 
											 thePeriod,
											 YKSITTAISET_VESI_LUMI_KUUROT_MAHDOLLISIA_PHRASE,
											 theStringVector);

					}
				  else
					{
					  in_places_phrase(in_some_places,
									   in_many_places,
									   theUseOllaVerb,
									   theStringVector);

					  if(is_showers)
						{
						  waterAndSnowShowersPhrase(thePrecipitationIntensity,
													thePrecipitationIntensityAbsoluteMax,
													thePrecipitationFormWater +
													thePrecipitationFormDrizzle +
													thePrecipitationFormSleet,
													can_be_freezing,
													theStringVector);
						}
					  else
						{

  /*
IF vesi on suunnilleen yhtä paljon kuin lumi AND räntä < vesi+lumi, THEN
vesi- tai lumisadetta.
  */



						  if(thePrecipitationIntensity <  theParameters.theWeakPrecipitationLimitSnow &&
							 thePrecipitationIntensityAbsoluteMax < theParameters.theHeavyPrecipitationLimitSnow)
							{
							  theStringVector.push_back(HEIKKOA_WORD);
							}
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							{
							  theStringVector.push_back(KOVAA_WORD);
							}

						  if(thePrecipitationFormWater >= thePrecipitationFormSnow)
							{
							  theStringVector.push_back(VESI_TAVUVIIVA_WORD);
							  theStringVector.push_back(TAI_WORD);
							  theStringVector.push_back(LUMISADETTA_WORD);
							}
						  else
							{
							  theStringVector.push_back(LUMI_TAVUVIIVA_WORD);
							  theStringVector.push_back(TAI_WORD);
							  theStringVector.push_back(VESISADETTA_WORD);
							  can_be_freezing_phrase(can_be_freezing, theStringVector, false);
							}
						}
					}
				}
			  else
				{
				  theStringVector.push_back(ENIMMAKSEEN_WORD);
				  theStringVector.push_back(POUTAINEN_WORD);
				}
			  break;
			}

		  case SLEET_SNOW_FREEZING_FORM:
		  case SLEET_SNOW_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitSleet)
				{
				  if(mostly_dry_weather)
					{
					  mostlyDryWeatherPhrase(is_showers, 
											 thePeriod,
											 YKSITTAISET_LUMI_RANTA_KUUROT_MAHDOLLISIA_PHRASE,
											 theStringVector);
					}
				  else
					{
					  in_places_phrase(in_some_places,
									   in_many_places,
									   theUseOllaVerb,
									   theStringVector);

					  if(is_showers)
						{
						  waterAndSnowShowersPhrase(thePrecipitationIntensity,
													thePrecipitationIntensityAbsoluteMax,
													thePrecipitationFormWater +
													thePrecipitationFormDrizzle +
													thePrecipitationFormSleet,
													can_be_freezing,
													theStringVector);
						}
					  else
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow &&
							 thePrecipitationIntensityAbsoluteMax < theParameters.theHeavyPrecipitationLimitSnow)
							{
							  theStringVector.push_back(HEIKKOA_WORD);
							  theStringVector.push_back(LUMI_TAVUVIIVA_WORD);
							  theStringVector.push_back(TAI_WORD);
							  theStringVector.push_back(RANTASADETTA_WORD);
							}
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							{
							  theStringVector.push_back(SAKEAA_WORD);
							  theStringVector.push_back(LUMI_TAVUVIIVA_WORD);
							  theStringVector.push_back(TAI_WORD);
							  theStringVector.push_back(RANTASADETTA_WORD);
							}
						  else
							{
							  if(thePrecipitationFormSnow >= thePrecipitationFormSleet)
								{
								  theStringVector.push_back(LUMI_TAVUVIIVA_WORD);
								  theStringVector.push_back(TAI_WORD);
								  theStringVector.push_back(RANTASADETTA_WORD);
								}
							  else
								{
								  theStringVector.push_back(RANTA_TAVUVIIVA_WORD);
								  theStringVector.push_back(TAI_WORD);
								  theStringVector.push_back(VESISADETTA_WORD);
								  can_be_freezing_phrase(can_be_freezing, theStringVector, false);
								}
							}
						}
					}
				}
			  else
				{
				  theStringVector.push_back(ENIMMAKSEEN_WORD);
				  theStringVector.push_back(POUTAINEN_WORD);
				}
			  break;
			}
		  }
		theDryPeriodTautologyFlag = false;
	  }

	return theStringVector.size();
 }

  /*
IF vesi on suunnilleen yhtä paljon kuin lumi AND räntä < vesi+lumi, THEN
vesi- tai lumisadetta.
  */

  std::string PrecipitationForecast::precipitationSentenceString(const WeatherPeriod& thePeriod,
																 const unsigned int& thePrecipitationForm,
																 const float thePrecipitationIntensity,
																 const float thePrecipitationIntensityAbsoluteMax,
																 const float thePrecipitationExtent,
																 const float thePrecipitationFormWater,
																 const float thePrecipitationFormDrizzle,
																 const float thePrecipitationFormSleet,
																 const float thePrecipitationFormSnow,
																 const float thePrecipitationFormFreezing,
																 const precipitation_type& thePrecipitationType,
																 const NFmiTime& theTypeChangeTime) const
  {
	std::string retval;

	vector<std::string> stringVector;

	precipitationSentenceStringVector(thePeriod,
									  thePrecipitationForm,
									  thePrecipitationIntensity,
									  thePrecipitationIntensityAbsoluteMax,
									  thePrecipitationExtent,
									  thePrecipitationFormWater,
									  thePrecipitationFormDrizzle,
									  thePrecipitationFormSleet,
									  thePrecipitationFormSnow,
									  thePrecipitationFormFreezing,
									  thePrecipitationType,
									  theTypeChangeTime,
									  stringVector);

	for(unsigned int i = 0; i < stringVector.size(); i++)
	  {
		retval << stringVector.at(i);
		if(i != stringVector.size() - 1)
		  retval << " ";
	  }
	boost::trim(retval);
	
	return retval;
  }



  std::string PrecipitationForecast::precipitationSentenceString(const WeatherPeriod& thePeriod)
  {

	std::string retval("");

	const precipitation_data_vector* dataVector = 0;

	if(theParameters.theForecastArea & FULL_AREA)
	  {
		dataVector = &theFullData;
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		dataVector = &theInlandData;
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		dataVector = &theCoastalData;
	  }

	if(dataVector)
	  {
		NFmiTime previousPeriodStartTime(thePeriod.localStartTime());
		NFmiTime previousPeriodEndTime(thePeriod.localStartTime());
		previousPeriodStartTime.ChangeByHours(-6);
		WeatherPeriod previousPeriod(previousPeriodStartTime, previousPeriodEndTime);

		float precipitationAbsoluteMaxIntensity = getMax(*dataVector, PRECIPITATION_MAX_DATA, thePeriod);
		float precipitationIntensity = getMean(*dataVector, PRECIPITATION_MEAN_DATA, thePeriod);
		float precipitationExtent = getMean(*dataVector, PRECIPITATION_EXTENT_DATA, thePeriod);
		float precipitationFormWater = getMean(*dataVector, PRECIPITATION_FORM_WATER_DATA, thePeriod);
		float precipitationFormDrizzle = getMean(*dataVector, PRECIPITATION_FORM_DRIZZLE_DATA, thePeriod);
		float precipitationFormSleet = getMean(*dataVector, PRECIPITATION_FORM_SLEET_DATA, thePeriod);
		float precipitationFormSnow = getMean(*dataVector, PRECIPITATION_FORM_SNOW_DATA, thePeriod);
		float precipitationFormFreezing = getMean(*dataVector, PRECIPITATION_FORM_FREEZING_DATA, thePeriod);

		precipitation_type precipitationType(getPrecipitationType(thePeriod, theParameters.theForecastArea));

		unsigned int typeChangeIndex = getPrecipitationTypeChange(*dataVector, thePeriod);

		unsigned int precipitationForm = get_complete_precipitation_form(theParameters.theVariable,
																		 precipitationFormWater,
																		 precipitationFormDrizzle,
																		 precipitationFormSleet,
																		 precipitationFormSnow,
																		 precipitationFormFreezing);
		

		retval = precipitationSentenceString(thePeriod,
											 precipitationForm,
											 precipitationIntensity,
											 precipitationAbsoluteMaxIntensity,
											 precipitationExtent,
											 precipitationFormWater,
											 precipitationFormDrizzle,
											 precipitationFormSleet,
											 precipitationFormSnow,
											 precipitationFormFreezing,
											 precipitationType,
											 dataVector->at(typeChangeIndex)->theObservationTime);
		
	  }
	
	return retval;
  }
  
  Sentence PrecipitationForecast::selectPrecipitationSentence(const WeatherPeriod& thePeriod,
															  const unsigned int& thePrecipitationForm,
															  const float thePrecipitationIntensity,
															  const float thePrecipitationAbsoluteMax,
															  const float thePrecipitationExtent,
															  const float thePrecipitationFormWater,
															  const float thePrecipitationFormDrizzle,
															  const float thePrecipitationFormSleet,
															  const float thePrecipitationFormSnow,
															  const float thePrecipitationFormFreezing,
															  const precipitation_type& thePrecipitationType,
															  const NFmiTime& theTypeChangeTime,
															  const precipitation_form_transformation_id& theTransformationId) const
  {
	Sentence sentence;

	vector<std::string> stringVector;

	if(theTransformationId == NO_FORM_TRANSFORMATION)
	  precipitationSentenceStringVector(thePeriod,
										thePrecipitationForm,
										thePrecipitationIntensity,
										thePrecipitationAbsoluteMax,
										thePrecipitationExtent,
										thePrecipitationFormWater,
										thePrecipitationFormDrizzle,
										thePrecipitationFormSleet,
										thePrecipitationFormSnow,
										thePrecipitationFormFreezing,
										thePrecipitationType,
										theTypeChangeTime,
										stringVector);
	else
	  precipitationSentenceStringVectorTransformation(thePeriod,
													  thePrecipitationExtent,
													  theTransformationId,
													  stringVector);

	for(unsigned int i = 0; i < stringVector.size(); i++)
	  {
		if(stringVector.at(i).compare(",") == 0)
		  sentence << Delimiter(",");
		else
		  sentence << stringVector.at(i);
	  }

	return sentence;
  }

  bool PrecipitationForecast::getDryPeriods(const WeatherPeriod& theSourcePeriod, 
											vector<WeatherPeriod>& theDryPeriods) const
  {
	const vector<WeatherPeriod>* precipitationPeriods = 0;

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  precipitationPeriods = &thePrecipitationPeriodsFull;
	else if(theParameters.theForecastArea & INLAND_AREA)
	  precipitationPeriods = &thePrecipitationPeriodsInland;
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  precipitationPeriods = &thePrecipitationPeriodsCoastal;

	if(precipitationPeriods->size() == 0)
	  {
		theDryPeriods.push_back(WeatherPeriod(theSourcePeriod.localStartTime(), 
														theSourcePeriod.localEndTime()));
		return true;
	  }

	theParameters.theLog << "START TIME: " << theSourcePeriod.localStartTime() << endl;
	theParameters.theLog << "END TIME: " << theSourcePeriod.localEndTime() << endl;

	bool overlappingPeriods = false;

	for(unsigned i = 0; i < precipitationPeriods->size(); i++)
	  {
		theParameters.theLog << "precipitation period i start: " << precipitationPeriods->at(i).localStartTime() << endl;
		theParameters.theLog << "precipitation period i end: " << precipitationPeriods->at(i).localEndTime() << endl;

		if((precipitationPeriods->at(i).localStartTime() >= theSourcePeriod.localStartTime() &&
			precipitationPeriods->at(i).localStartTime() <= theSourcePeriod.localEndTime()) ||
		   precipitationPeriods->at(i).localEndTime() >= theSourcePeriod.localStartTime() &&
		   precipitationPeriods->at(i).localEndTime() <= theSourcePeriod.localStartTime())
		  overlappingPeriods = true;

		if(i == 0)
		  {
			if(precipitationPeriods->at(i).localStartTime() >= theSourcePeriod.localStartTime() &&
			   (precipitationPeriods->at(i).localStartTime() <= theSourcePeriod.localEndTime()))
			  {
				theDryPeriods.push_back(WeatherPeriod(theSourcePeriod.localStartTime(),
													  precipitationPeriods->at(i).localStartTime()));
			  }
		  }
		else
		  {
			WeatherPeriod dryPeriod(precipitationPeriods->at(i-1).localEndTime(),
									precipitationPeriods->at(i).localStartTime());
			if(dryPeriod.localStartTime() >= theSourcePeriod.localStartTime() &&
			   dryPeriod.localStartTime() < theSourcePeriod.localEndTime())
			  {
				if(dryPeriod.localEndTime() > theSourcePeriod.localStartTime() &&
				   dryPeriod.localEndTime() <= theSourcePeriod.localEndTime())
				  {
					theDryPeriods.push_back(dryPeriod);
				  }
				else
				  {
					theDryPeriods.push_back(WeatherPeriod(dryPeriod.localStartTime(),
														  theSourcePeriod.localEndTime()));
				  }
			  }
		  }
	  }

	if(!overlappingPeriods)
	  theDryPeriods.push_back(theSourcePeriod);


	return theDryPeriods.size() > 0;
  }

  bool PrecipitationForecast::getPrecipitationPeriods(const WeatherPeriod& theSourcePeriod, 
													  vector<WeatherPeriod>& thePrecipitationPeriods) const
  {
	const vector<WeatherPeriod>* precipitationPeriods = 0;

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  precipitationPeriods = &thePrecipitationPeriodsFull;
	else if(theParameters.theForecastArea & INLAND_AREA)
	  precipitationPeriods = &thePrecipitationPeriodsInland;
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  precipitationPeriods = &thePrecipitationPeriodsCoastal;

	if(precipitationPeriods->size() == 0)
	  {
		return false;
	  }

	for(unsigned i = 0; i < precipitationPeriods->size(); i++)
	  {
		WeatherPeriod notDryPeriod(precipitationPeriods->at(i).localStartTime(),
								  precipitationPeriods->at(i).localEndTime());

		if(notDryPeriod.localStartTime() >= theSourcePeriod.localStartTime() &&
		   notDryPeriod.localStartTime() <= theSourcePeriod.localEndTime())
		  {
			if(notDryPeriod.localEndTime() >= theSourcePeriod.localStartTime() &&
			   notDryPeriod.localEndTime() <= theSourcePeriod.localEndTime())
			  {
				thePrecipitationPeriods.push_back(notDryPeriod);
			  }
			else
			  {
				thePrecipitationPeriods.push_back(WeatherPeriod(notDryPeriod.localStartTime(), 
																theSourcePeriod.localEndTime()));
			  }
		  }
		else if(notDryPeriod.localStartTime() < theSourcePeriod.localStartTime() &&
				notDryPeriod.localEndTime() > theSourcePeriod.localStartTime())
		  {
			if(notDryPeriod.localEndTime() <= theSourcePeriod.localEndTime())
			  {
				thePrecipitationPeriods.push_back(WeatherPeriod(theSourcePeriod.localStartTime(),
																notDryPeriod.localEndTime()));
			  }
			else
			  {
				thePrecipitationPeriods.push_back(theSourcePeriod);
			  }
		  }
	  }

	return thePrecipitationPeriods.size() > 0;
  }

  bool PrecipitationForecast::getIntensityFormExtent(const WeatherPeriod& theWeatherPeriod,
													 const unsigned short theForecastArea,
													 float& theIntensity,
													 unsigned int& theForm,
													 float& theExtent) const
  {
	const precipitation_data_vector& dataVector = getPrecipitationDataVector(theForecastArea);
	
	theIntensity = getMean(dataVector,
						   PRECIPITATION_MEAN_DATA,
						   theWeatherPeriod);

	theForm = MISSING_PRECIPITATION_FORM;

	for(unsigned int i = 0; i < dataVector.size(); i++)
	  if(dataVector.at(i)->theObservationTime >= theWeatherPeriod.localStartTime() &&
		 dataVector.at(i)->theObservationTime <= theWeatherPeriod.localEndTime() &&
		 dataVector.at(i)->thePrecipitationForm != MISSING_PRECIPITATION_FORM)
		{
		  if(theForm == MISSING_PRECIPITATION_FORM)
			theForm = dataVector.at(i)->thePrecipitationForm;
		  else if(dataVector.at(i)->thePrecipitationForm < theForm)
			theForm = dataVector.at(i)->thePrecipitationForm;				
		}

	theExtent =   getMean(dataVector,
						  PRECIPITATION_EXTENT_DATA,
						  theWeatherPeriod);

	return theForm != MISSING_PRECIPITATION_FORM;
  }

  bool PrecipitationForecast::isMostlyDryPeriod(const WeatherPeriod& theWeatherPeriod,
												const unsigned short theForecastArea) const
  {
	const precipitation_data_vector& dataVector = getPrecipitationDataVector(theForecastArea);

	return (getMean(dataVector, PRECIPITATION_EXTENT_DATA, theWeatherPeriod) <= 
			theParameters.theMostlyDryWeatherLimit);
  }

  bool PrecipitationForecast::isDryPeriod(const WeatherPeriod& theWeatherPeriod,
										  const unsigned short theForecastArea) const
  {
	float precipitationIntensity(0.0);
	unsigned int precipitationForm = MISSING_PRECIPITATION_FORM;
	float precipitationExtent(0.0);
	bool dry_weather = true;

	if(getIntensityFormExtent(theWeatherPeriod,
							  theForecastArea,
							  precipitationIntensity,
							  precipitationForm,
							  precipitationExtent))
	  {
		dry_weather = is_dry_weather(theParameters,
							  precipitationForm,
							  precipitationIntensity,
							  precipitationExtent);
	  }

	return dry_weather;// && !shortTermPrecipitationExists(theWeatherPeriod);
  }

  float PrecipitationForecast::getStat(const precipitation_data_vector& theData, 
									   const weather_result_data_id& theDataId,
									   const WeatherPeriod& theWeatherPeriod,
									   const stat_func_id& theStatFunc) const
  {
	float cumulativeSum = 0.0;
	float minValue = 0.0;
	float maxValue = 0.0;
	int count = 0;

	for(unsigned int i = 0; i < theData.size(); i++)
	  {
		if(theData[i]->theObservationTime < theWeatherPeriod.localStartTime())
		  continue;
		if(theData[i]->theObservationTime > theWeatherPeriod.localEndTime())
		  break;

		switch(theDataId)
		  {
		  case PRECIPITATION_MAX_DATA:
			{
			  if(count == 0 || minValue > theData[i]->thePrecipitationMaxIntensity)
				minValue = theData[i]->thePrecipitationIntensity;
			  if(count == 0 || maxValue < theData[i]->thePrecipitationMaxIntensity)
				maxValue = theData[i]->thePrecipitationMaxIntensity;
			  cumulativeSum += theData[i]->thePrecipitationMaxIntensity;
			  count++;
			}
			break;
		  case PRECIPITATION_MEAN_DATA:
			{
			  if(count == 0 || minValue > theData[i]->thePrecipitationIntensity)
				minValue = theData[i]->thePrecipitationIntensity;
			  if(count == 0 || maxValue < theData[i]->thePrecipitationIntensity)
				maxValue = theData[i]->thePrecipitationIntensity;
			  cumulativeSum += theData[i]->thePrecipitationIntensity;
			  count++;
			}
			break;
		  case PRECIPITATION_EXTENT_DATA:
			{
			  if(count == 0 || minValue > theData[i]->thePrecipitationExtent)
				minValue = theData[i]->thePrecipitationExtent;
			  if(count == 0 || maxValue < theData[i]->thePrecipitationExtent)
				maxValue = theData[i]->thePrecipitationExtent;
			  cumulativeSum += theData[i]->thePrecipitationExtent;
			  count++;
			}
			break;
		  case PRECIPITATION_FORM_WATER_DATA:
			{
			  if(theData[i]->thePrecipitationFormWater != kFloatMissing)
				{
				  if(count == 0 || minValue > theData[i]->thePrecipitationFormWater)
					minValue = theData[i]->thePrecipitationFormWater;
				  if(count == 0 || maxValue < theData[i]->thePrecipitationFormWater)
					maxValue = theData[i]->thePrecipitationFormWater;
				  cumulativeSum += theData[i]->thePrecipitationFormWater;
				  count++;
				}
			}
			break;
		  case PRECIPITATION_FORM_DRIZZLE_DATA:
			{
			  if(theData[i]->thePrecipitationFormDrizzle != kFloatMissing)
				{
				  if(count == 0 || minValue > theData[i]->thePrecipitationFormDrizzle)
					minValue = theData[i]->thePrecipitationFormDrizzle;
				  if(count == 0 || maxValue < theData[i]->thePrecipitationFormDrizzle)
					maxValue = theData[i]->thePrecipitationFormDrizzle;
				  cumulativeSum += theData[i]->thePrecipitationFormDrizzle;
				  count++;
				}
			}
			break;
		  case PRECIPITATION_FORM_SLEET_DATA:
			{
			  if(theData[i]->thePrecipitationFormSleet != kFloatMissing)
				{
				  if(count == 0 || minValue > theData[i]->thePrecipitationFormSleet)
					minValue = theData[i]->thePrecipitationFormSleet;
				  if(count == 0 || maxValue < theData[i]->thePrecipitationFormSleet)
					maxValue = theData[i]->thePrecipitationFormSleet;
				  cumulativeSum += theData[i]->thePrecipitationFormSleet;
				  count++;
				}
			}
			break;
		  case PRECIPITATION_FORM_SNOW_DATA:
			{
			  if(theData[i]->thePrecipitationFormSnow != kFloatMissing)
				{
				  if(count == 0 || minValue > theData[i]->thePrecipitationFormSnow)
					minValue = theData[i]->thePrecipitationFormSnow;
				  if(count == 0 || maxValue < theData[i]->thePrecipitationFormSnow)
					maxValue = theData[i]->thePrecipitationFormSnow;
				  cumulativeSum += theData[i]->thePrecipitationFormSnow;
				  count++;
				}
			}
			break;
		  case PRECIPITATION_FORM_FREEZING_DATA:
			{
			  if(theData[i]->thePrecipitationFormFreezing != kFloatMissing)
				{
				  if(count == 0 || minValue > theData[i]->thePrecipitationFormFreezing)
					minValue = theData[i]->thePrecipitationFormFreezing;
				  if(count == 0 || maxValue < theData[i]->thePrecipitationFormFreezing)
					maxValue = theData[i]->thePrecipitationFormFreezing;
				  cumulativeSum += theData[i]->thePrecipitationFormFreezing;
				  count++;
				}
			}
			break;
		  case PRECIPITATION_TYPE_DATA:
			{
			  if(theData[i]->thePrecipitationTypeShowers != kFloatMissing)
				{
				  if(count == 0 || minValue > theData[i]->thePrecipitationTypeShowers)
					minValue = theData[i]->thePrecipitationTypeShowers;
				  if(count == 0 || maxValue < theData[i]->thePrecipitationTypeShowers)
					maxValue = theData[i]->thePrecipitationTypeShowers;
				  cumulativeSum += theData[i]->thePrecipitationTypeShowers;
				  count++;
				}
			}
			break;
		  default:
			break;
		  }
	  }

	float retval = 0.0;
	switch(theStatFunc)
	  {
	  case MIN:
		retval = minValue;
		break;
	  case MAX:
		retval = maxValue;
		break;
	  case MEAN:
		{
		  if(count > 0)
			retval = (cumulativeSum / count);
		}
		break;
	  }

	return retval;
  }

  float PrecipitationForecast::getMin(const precipitation_data_vector& theData, 
									   const weather_result_data_id& theDataId,
									   const WeatherPeriod& theWeatherPeriod) const
  {
	return getStat(theData, theDataId, theWeatherPeriod, MIN);
  }

  float PrecipitationForecast::getMax(const precipitation_data_vector& theData, 
									   const weather_result_data_id& theDataId,
									   const WeatherPeriod& theWeatherPeriod) const
  {
	return getStat(theData, theDataId, theWeatherPeriod, MAX);
  }

  float PrecipitationForecast::getMean(const precipitation_data_vector& theData, 
									   const weather_result_data_id& theDataId,
									   const WeatherPeriod& theWeatherPeriod) const
  {
	return getStat(theData, theDataId, theWeatherPeriod, MEAN);
  }

  precipitation_type PrecipitationForecast::getPrecipitationType(const WeatherPeriod& thePeriod,
																 const unsigned short theForecastArea) const
  {
	const precipitation_data_vector& theData = getPrecipitationDataVector(theForecastArea);;

	unsigned int showers_counter(0);
	unsigned int continuous_counter(0);

	for(unsigned int i = 0; i < theData.size(); i++)
	  {
		if(theData[i]->theObservationTime < thePeriod.localStartTime())
		  continue;
		if(theData[i]->theObservationTime > thePeriod.localEndTime())
		  break;

		if(theData[i]->thePrecipitationType != MISSING_PRECIPITATION_TYPE)
		  {
			if(theData[i]->thePrecipitationType == SHOWERS)
			  showers_counter++;
			else
			  continuous_counter++;
		  }
	  }

	return (continuous_counter >= showers_counter ? CONTINUOUS : SHOWERS);
  }

  // Logic:
  // 1) save the type of the first hour
  // 2) Iterate the precipitation period through hour by hour and check
  // if the type changes
  // 3) If the type changes (i.e. is different from the original type), check if the new type
  // is the prevailing type for the rest of the period
  // returns 0, if there is no change, index in the data vector otherwise
  unsigned int PrecipitationForecast::getPrecipitationTypeChange(const precipitation_data_vector& theData,
																 const WeatherPeriod& thePeriod) const
  {
	WeatherPeriod periodToCheck(thePeriod.localStartTime(), 
								thePeriod.localEndTime().GetYear() == 2037 ? 
								theParameters.theForecastPeriod.localEndTime() : thePeriod.localEndTime());

	bool firstValue = true;
	precipitation_type originalPrecipitationType(MISSING_PRECIPITATION_TYPE);
	for(unsigned int i = 0; i < theData.size(); i++)
	  {
		if(theData[i]->theObservationTime < periodToCheck.localStartTime())
		  continue;
		if(theData[i]->theObservationTime > periodToCheck.localEndTime())
		  break;

		if(firstValue)
		  {
			// store the original type
			originalPrecipitationType = theData[i]->thePrecipitationType;
			firstValue = false;
		  }
		else
		  {
			// different from the original type
			if(originalPrecipitationType != theData[i]->thePrecipitationType)
			  {
				// check the type for the rest of the precipitation period
				for(unsigned int k = i; k < theData.size(); k++)
				  {
					// the type changes again
					if(theData[k]->thePrecipitationType != theData[i]->thePrecipitationType)
					  return 0;
				  }
				// if type changes in the beginning or in the end ignore it
				if(abs(theData[i]->theObservationTime.DifferenceInHours(periodToCheck.localStartTime())) < 3 ||
				   abs(theData[i]->theObservationTime.DifferenceInHours(periodToCheck.localEndTime())) < 3)
				  return 0;
				
				return i;
			  }
		  }
	  }

	return 0;
  }

  unsigned int PrecipitationForecast::getPrecipitationForm(const WeatherPeriod& thePeriod,
														   const unsigned short theForecastArea) const
  {
	const precipitation_data_vector& theDataVector = getPrecipitationDataVector(theForecastArea);

	float precipitationFormWater = getMean(theDataVector, PRECIPITATION_FORM_WATER_DATA, thePeriod);
	float precipitationFormDrizzle = getMean(theDataVector, PRECIPITATION_FORM_DRIZZLE_DATA, thePeriod);
	float precipitationFormSleet = getMean(theDataVector, PRECIPITATION_FORM_SLEET_DATA, thePeriod);
	float precipitationFormSnow = getMean(theDataVector, PRECIPITATION_FORM_SNOW_DATA, thePeriod);
	float precipitationFormFreezing = getMean(theDataVector, PRECIPITATION_FORM_FREEZING_DATA, thePeriod);

	return get_complete_precipitation_form(theParameters.theVariable,
										   precipitationFormWater,
										   precipitationFormDrizzle,
										   precipitationFormSleet,
										   precipitationFormSnow,
										   precipitationFormFreezing);
  }

  unsigned int PrecipitationForecast::getPrecipitationCategory(const float& thePrecipitation, 
															   const unsigned int& theType) const
  {
	unsigned int retval(DRY_WEATHER_CATEGORY);

	switch(theType)
	  {
	  case WATER_FORM:
	  case WATER_DRIZZLE_FORM:
	  case WATER_SLEET_FORM:
	  case WATER_SNOW_FORM:
	  case WATER_FREEZING_FORM:
	  case WATER_DRIZZLE_SLEET_FORM:
	  case WATER_DRIZZLE_SNOW_FORM:
	  case WATER_DRIZZLE_FREEZING_FORM:
	  case WATER_SLEET_SNOW_FORM:
	  case WATER_SLEET_FREEZING_FORM:
	  case WATER_SNOW_FREEZING_FORM:
	  case DRIZZLE_FORM:
	  case DRIZZLE_SLEET_FORM:
	  case DRIZZLE_SLEET_SNOW_FORM:
	  case DRIZZLE_SLEET_FREEZING_FORM:
	  case DRIZZLE_FREEZING_FORM:
	  case DRIZZLE_SNOW_FORM:
	  case DRIZZLE_SNOW_FREEZING_FORM:
	  case FREEZING_FORM:
		{
		  if(thePrecipitation <= theParameters.theDryWeatherLimitWater)
			retval = DRY_WEATHER_CATEGORY;
		  else if(thePrecipitation >= theParameters.theWeakPrecipitationLimitWater &&
				  thePrecipitation < theParameters.theHeavyPrecipitationLimitWater)
			retval = MODERATE_WATER_PRECIPITATION;
		  else if(thePrecipitation > theParameters.theHeavyPrecipitationLimitWater)
			retval = HEAVY_WATER_PRECIPITATION;		  
		}
		break;
	  case SLEET_FORM:
	  case SLEET_FREEZING_FORM:
	  case SLEET_SNOW_FREEZING_FORM:
		{
		  if(thePrecipitation <= theParameters.theDryWeatherLimitSleet)
			retval = DRY_WEATHER_CATEGORY;
		  else if(thePrecipitation >= theParameters.theWeakPrecipitationLimitSleet &&
				  thePrecipitation < theParameters.theHeavyPrecipitationLimitSleet)
			retval = MODERATE_WATER_PRECIPITATION;
		  else if(thePrecipitation > theParameters.theHeavyPrecipitationLimitSleet)
			retval = HEAVY_WATER_PRECIPITATION;		  
		}
		break;
	  case SNOW_FORM:
	  case SNOW_FREEZING_FORM:
		{
		  if(thePrecipitation <= theParameters.theDryWeatherLimitSnow)
			retval = DRY_WEATHER_CATEGORY;
		  else if(thePrecipitation >= theParameters.theWeakPrecipitationLimitSnow &&
				  thePrecipitation < theParameters.theHeavyPrecipitationLimitSnow)
			retval = MODERATE_WATER_PRECIPITATION;
		  else if(thePrecipitation > theParameters.theHeavyPrecipitationLimitSnow)
			retval = HEAVY_WATER_PRECIPITATION;		  
		}
		break;
	  default:
		break;
	  }

	return retval;
  }

  bool PrecipitationForecast::separateCoastInlandPrecipitation(const WeatherPeriod& theWeatherPeriod) const
  {
	if(!(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA))
	  return false;

	float coastalPrecipitation = getMean(theCoastalData, PRECIPITATION_MEAN_DATA, theWeatherPeriod);
	float inlandPrecipitation = getMean(theInlandData, PRECIPITATION_MEAN_DATA, theWeatherPeriod);

	unsigned int coastalPrecipitationForm = getPrecipitationForm(theWeatherPeriod, COASTAL_AREA);
	unsigned int inlandPrecipitationForm = getPrecipitationForm(theWeatherPeriod, INLAND_AREA);

	if(((coastalPrecipitation < 0.04 && inlandPrecipitation > 0.4) ||
		(coastalPrecipitation > 0.4 && inlandPrecipitation < 0.04)) ||
	   ((inlandPrecipitation > 0.04 && coastalPrecipitation > 0.04) &&
		coastalPrecipitationForm != inlandPrecipitationForm))
	  return true;

	return false;
  }


  void PrecipitationForecast::printOutPrecipitationVector(std::ostream & theOutput,
														  const precipitation_data_vector& thePrecipitationDataVector) const
  {
	for(unsigned int i = 0; i < thePrecipitationDataVector.size(); i++)
	  {
		theOutput << *(thePrecipitationDataVector[i]);
	  }
  }
	
  void PrecipitationForecast::printOutPrecipitationData(std::ostream & theOutput) const
  {
	theOutput << "** PRECIPITATION DATA **" << endl;
	theOutput << "time:**intensity**extent**water**drizzle**sleet**snow**freezing**showers**northeast**southeast**southwest**northwest)" << endl;
	
	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		theOutput << "Full area precipitation: " << endl;
		printOutPrecipitationVector(theOutput, theFullData);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		theOutput << "Coastal precipitation: " << endl;
		printOutPrecipitationVector(theOutput, theCoastalData);
	  }
	 if(theParameters.theForecastArea & INLAND_AREA)
	  {
		theOutput << "Inland precipitation: " << endl;
		printOutPrecipitationVector(theOutput, theInlandData);
	  }
  }


  void PrecipitationForecast::gatherPrecipitationData()
  {
	if(theParameters.theForecastArea & COASTAL_AREA)
	  fillInPrecipitationDataVector(COASTAL_AREA);
	if(theParameters.theForecastArea & INLAND_AREA)
	  fillInPrecipitationDataVector(INLAND_AREA);
	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  fillInPrecipitationDataVector(FULL_AREA);
  }

  void PrecipitationForecast::fillInPrecipitationDataVector(const forecast_area_id& theAreaId)
  {
	weather_result_data_item_vector* precipitationMaxHourly = 
	  (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_MAX_DATA];
	weather_result_data_item_vector* precipitationMeanHourly = 
	  (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_MEAN_DATA];
	weather_result_data_item_vector* precipitationExtentHourly = 
	  (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_EXTENT_DATA];
	weather_result_data_item_vector* precipitationFormWaterHourly = 
	  (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_WATER_DATA];
	weather_result_data_item_vector* precipitationFormDrizzleHourly = 
	  (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_DRIZZLE_DATA];
	weather_result_data_item_vector* precipitationFormSleetHourly = 
	  (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_SLEET_DATA];
	weather_result_data_item_vector* precipitationFormSnowHourly = 
	  (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_SNOW_DATA];
	weather_result_data_item_vector* precipitationFormFreezingHourly = 
	  (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_FREEZING_DATA];
	weather_result_data_item_vector* precipitationTypeHourly = 
	  (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_TYPE_DATA];
	weather_result_data_item_vector* precipitationNorthEastShareHourly = 
	  (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_NORTHEAST_SHARE_DATA];
	weather_result_data_item_vector* precipitationSouthEastShareHourly = 
	  (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_SOUTHEAST_SHARE_DATA];
	weather_result_data_item_vector* precipitationSouthWestShareHourly = 
	  (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_SOUTHWEST_SHARE_DATA];
	weather_result_data_item_vector* precipitationNorthWestShareHourly = 
	  (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_NORTHWEST_SHARE_DATA];
	weather_result_data_item_vector* precipitationPointHourly = 
	  (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_POINT_DATA];

	precipitation_data_vector& dataVector = (theAreaId == COASTAL_AREA ? theCoastalData : 
											 (theAreaId == INLAND_AREA ? theInlandData : theFullData));

	for(unsigned int i = 0; i < precipitationMaxHourly->size(); i++)
	  {
		float precipitationMaxIntesity = (*precipitationMaxHourly)[i]->theResult.value();
		float precipitationMeanIntesity = (*precipitationMeanHourly)[i]->theResult.value();
		float precipitationExtent = (*precipitationExtentHourly)[i]->theResult.value();
		float precipitationFormWater = (*precipitationFormWaterHourly)[i]->theResult.value();
		float precipitationFormDrizzle = (*precipitationFormDrizzleHourly)[i]->theResult.value();
		float precipitationFormSleet = (*precipitationFormSleetHourly)[i]->theResult.value();
		float precipitationFormSnow = (*precipitationFormSnowHourly)[i]->theResult.value();
		float precipitationFormFreezing = (*precipitationFormFreezingHourly)[i]->theResult.value();
		float precipitationTypeShowers = (*precipitationTypeHourly)[i]->theResult.value();

		const unsigned int precipitationForm = 
		  get_complete_precipitation_form(theParameters.theVariable,
										  precipitationFormWater,
										  precipitationFormDrizzle,
										  precipitationFormSleet,
										  precipitationFormSnow,
										  precipitationFormFreezing);
		  
		PrecipitationDataItemData* item = 
		  new PrecipitationDataItemData(theParameters,
										precipitationForm,
										(precipitationMaxIntesity+precipitationMeanIntesity)/2.0,
										precipitationMaxIntesity,
										precipitationExtent,
										precipitationFormWater,
										precipitationFormDrizzle,
										precipitationFormSleet,
										precipitationFormSnow,
										precipitationFormFreezing,
										precipitationTypeShowers,
										MISSING_WEATHER_EVENT,
										0.0,
										(*precipitationMaxHourly)[i]->thePeriod.localStartTime());
			
		item->thePrecipitationPercentageNorthEast = (*precipitationNorthEastShareHourly)[i]->theResult.value();
		item->thePrecipitationPercentageSouthEast = (*precipitationSouthEastShareHourly)[i]->theResult.value();
		item->thePrecipitationPercentageSouthWest = (*precipitationSouthWestShareHourly)[i]->theResult.value();
		item->thePrecipitationPercentageNorthWest = (*precipitationNorthWestShareHourly)[i]->theResult.value();
		item->thePrecipitationPoint.X((*precipitationPointHourly)[i]->theResult.value());
		item->thePrecipitationPoint.Y((*precipitationPointHourly)[i]->theResult.error());

		dataVector.push_back(item);
	  }
  }

  void PrecipitationForecast::findOutPrecipitationPeriods()
  {
	if(theParameters.theForecastArea & COASTAL_AREA)
	  findOutPrecipitationPeriods(COASTAL_AREA);
	if(theParameters.theForecastArea & INLAND_AREA)
	  findOutPrecipitationPeriods(INLAND_AREA);
	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  findOutPrecipitationPeriods(FULL_AREA);
  }

  // join periods if there is only one hour "dry" period between
  // and if type stays the same
  void PrecipitationForecast::joinPrecipitationPeriods(vector<WeatherPeriod>& thePrecipitationPeriodVector)
  {
	if(thePrecipitationPeriodVector.size() < 2)
	  return;

	for(unsigned int i = thePrecipitationPeriodVector.size()-1; i > 0; i--)
	  {
		NFmiTime currentPeriodStartTime(thePrecipitationPeriodVector.at(i).localStartTime());
		NFmiTime previousPeriodEndTime(thePrecipitationPeriodVector.at(i-1).localEndTime());

	    if(currentPeriodStartTime.DifferenceInHours(previousPeriodEndTime) <= 2)
		  {
			WeatherPeriod joinedPeriod(thePrecipitationPeriodVector.at(i-1).localStartTime(),
									   thePrecipitationPeriodVector.at(i).localEndTime());


			if(isDryPeriod(joinedPeriod, theParameters.theForecastArea))
			  continue;

			theParameters.theLog << "Joining precipitation periods: " 
								 << thePrecipitationPeriodVector.at(i-1).localStartTime() << "..."
								 << thePrecipitationPeriodVector.at(i-1).localEndTime()
								 << " and "
								 << thePrecipitationPeriodVector.at(i).localStartTime() << "..."
								 << thePrecipitationPeriodVector.at(i).localEndTime();

			thePrecipitationPeriodVector.erase(thePrecipitationPeriodVector.begin()+i);
			thePrecipitationPeriodVector.erase(thePrecipitationPeriodVector.begin()+i-1);
			thePrecipitationPeriodVector.insert(thePrecipitationPeriodVector.begin()+i-1,joinedPeriod);
		  }
	  }
  }

  void PrecipitationForecast::findOutPrecipitationPeriods(const forecast_area_id& theAreaId)
  {
	precipitation_data_vector* dataSourceVector = 0;
	vector<WeatherPeriod>* dataDestinationVector = 0;

	if(theAreaId & FULL_AREA)
	  {
		dataSourceVector = &theFullData;
		dataDestinationVector = &thePrecipitationPeriodsFull;
	  }
	else if(theAreaId & INLAND_AREA)
	  {
		dataSourceVector = &theInlandData;
		dataDestinationVector = &thePrecipitationPeriodsInland;
	  }
	else if(theAreaId & COASTAL_AREA)
	  {
		dataSourceVector = &theCoastalData;
		dataDestinationVector = &thePrecipitationPeriodsCoastal;
	  }

	if(!dataSourceVector)
	  return;

	bool isDryPrevious = true;
	bool isDryCurrent = true;

	unsigned int periodStartIndex = 0;
	for(unsigned int i = 1; i < dataSourceVector->size(); i++)
	  {
		isDryPrevious = is_dry_weather(theParameters, (*dataSourceVector)[i-1]->thePrecipitationForm, 
									   (*dataSourceVector)[i-1]->thePrecipitationIntensity,
									   (*dataSourceVector)[i-1]->thePrecipitationExtent);
		isDryCurrent = is_dry_weather(theParameters, (*dataSourceVector)[i]->thePrecipitationForm, 
									  (*dataSourceVector)[i]->thePrecipitationIntensity,
									  (*dataSourceVector)[i]->thePrecipitationExtent);
		if(isDryPrevious != isDryCurrent)
		  {
			if(!isDryPrevious)
			  {
				NFmiTime startTime((*dataSourceVector)[periodStartIndex]->theObservationTime);
				if(periodStartIndex == 0)
				  startTime.SetDate(1970,1,1); // precipitation starts before forecast period
				NFmiTime endTime((*dataSourceVector)[i-1]->theObservationTime);
				dataDestinationVector->push_back(WeatherPeriod(startTime, endTime));
			  }
			periodStartIndex = i;
		  }
	  }
	if(!isDryPrevious && periodStartIndex != dataSourceVector->size() - 1)
	  {
		NFmiTime startTime((*dataSourceVector)[periodStartIndex]->theObservationTime);
		if(periodStartIndex == 0)
		  startTime.SetDate(1970,1,1); // precipitation starts before forecast period
		NFmiTime endTime((*dataSourceVector)[dataSourceVector->size()-1]->theObservationTime);
		if(endTime == theParameters.theForecastPeriod.localEndTime())
		  endTime.SetDate(2037,1,1); // precipitation continues when forecast period ends

		// If type changes, split one period into two
		WeatherPeriod precipitationPeriod(startTime, endTime);
		unsigned int typeChangeIndex = getPrecipitationTypeChange(*dataSourceVector, precipitationPeriod);
		if(typeChangeIndex > 0)
		  {
			theParameters.theLog << "Split one precipitation period into two, because type changes at ";
			theParameters.theLog << (*dataSourceVector)[typeChangeIndex]->theObservationTime;
			if((*dataSourceVector)[typeChangeIndex]->thePrecipitationType == SHOWERS)
			  theParameters.theLog << " from continuous to showers";
			else
			  theParameters.theLog << " from showers to continuous";

			WeatherPeriod period1(startTime, (*dataSourceVector)[typeChangeIndex-1]->theObservationTime);
			WeatherPeriod period2((*dataSourceVector)[typeChangeIndex]->theObservationTime, endTime);
			dataDestinationVector->push_back(period1);
			dataDestinationVector->push_back(period2);
		  }
		else
		  {
			dataDestinationVector->push_back(precipitationPeriod);
		  }
	  }
	//	joinPrecipitationPeriods(*dataDestinationVector);
  }

  void PrecipitationForecast::findOutPrecipitationWeatherEvents()
  {
	if(theParameters.theForecastArea & COASTAL_AREA)
	  findOutPrecipitationWeatherEvents(thePrecipitationPeriodsCoastal, COASTAL_AREA, thePrecipitationWeatherEventsCoastal);
	if(theParameters.theForecastArea & INLAND_AREA)
	  findOutPrecipitationWeatherEvents(thePrecipitationPeriodsInland, INLAND_AREA, thePrecipitationWeatherEventsInland);
	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  findOutPrecipitationWeatherEvents(thePrecipitationPeriodsFull, FULL_AREA, thePrecipitationWeatherEventsFull);
  }

  void PrecipitationForecast::findOutPrecipitationWeatherEvents(const vector<WeatherPeriod>& thePrecipitationPeriods,
														 const unsigned short theForecastArea,
														 weather_event_id_vector& thePrecipitationWeatherEvents)
  {
	
	for(unsigned int i = 0; i < thePrecipitationPeriods.size(); i++)
	  {
		NFmiTime precipitationStartTime(thePrecipitationPeriods.at(i).localStartTime());
		NFmiTime precipitationEndTime(thePrecipitationPeriods.at(i).localEndTime());
		if(precipitationEndTime.DifferenceInHours(precipitationStartTime) >= 6)
		  {
			
			// start year 1970 indicates that rain starts on previous period
			if(precipitationStartTime.GetYear() != 1970 && 
			   precipitationStartTime >= theParameters.theForecastPeriod.localStartTime() &&
			   precipitationStartTime <= theParameters.theForecastPeriod.localEndTime())
			  {
				thePrecipitationWeatherEvents.push_back(make_pair(precipitationStartTime, SADE_ALKAA));
				// check here if type cahanes during the period (from continuous to showers or vice versa)
				
			  }
			
			if(precipitationEndTime.GetYear() != 2037 &&
			   precipitationEndTime >= theParameters.theForecastPeriod.localStartTime() &&
			   precipitationEndTime <= theParameters.theForecastPeriod.localEndTime())
			  {

				if(getPrecipitationExtent(thePrecipitationPeriods.at(i), theForecastArea) > MOSTLY_DRY_WEATHER_LIMIT)
				  thePrecipitationWeatherEvents.push_back(make_pair(precipitationEndTime, POUTAANTUU));
				else
				  thePrecipitationWeatherEvents.push_back(make_pair(precipitationEndTime, POUTAANTUU_WHEN_EXTENT_SMALL));
			  }
		  }
	  }
  }

  float PrecipitationForecast::getPrecipitationExtent(const WeatherPeriod& thePeriod,
													  const unsigned short theForecastArea) const
  {
	const precipitation_data_vector& dataVector = getPrecipitationDataVector(theForecastArea);
	
	float extent = getMean(dataVector,
						   PRECIPITATION_EXTENT_DATA,
						   thePeriod);

	return extent; 
  }
	
  float PrecipitationForecast::getMaxIntensity(const WeatherPeriod& thePeriod,
											   const unsigned short theForecastArea) const
  {
	const precipitation_data_vector& dataVector = getPrecipitationDataVector(theForecastArea);
	
	float intensity = getMean(dataVector,
							  PRECIPITATION_MAX_DATA,
							  thePeriod);

	return intensity; 
  }

  float PrecipitationForecast::getMeanIntensity(const WeatherPeriod& thePeriod,
															 const unsigned short theForecastArea) const
  {
	const precipitation_data_vector& dataVector = getPrecipitationDataVector(theForecastArea);
	
	float intensity = getMean(dataVector,
							  PRECIPITATION_MEAN_DATA,
							  thePeriod);

	return intensity;
  }


  void PrecipitationForecast::removeRedundantWeatherEvents(weather_event_id_vector& thePrecipitationWeatherEvents, const vector<int>& theRemoveIndexes)
  {
	if(theRemoveIndexes.size() > 0)
	  {
		for(unsigned int i = theRemoveIndexes.size(); i > 0; i--)
		  {
			thePrecipitationWeatherEvents.erase(thePrecipitationWeatherEvents.begin()+theRemoveIndexes[i-1]);
		  }
	  }
  }

  void PrecipitationForecast::removeDuplicatePrecipitationWeatherEvents(weather_event_id_vector& thePrecipitationWeatherEvents)
  {
	vector<int> removeIndexes;

	for(unsigned int i = 1; i < thePrecipitationWeatherEvents.size(); i++)
	  {
		weather_event_id previousPeriodWeatherEventId(thePrecipitationWeatherEvents[i-1].second);
		weather_event_id currentPeriodWeatherEventId(thePrecipitationWeatherEvents[i].second);
		if( previousPeriodWeatherEventId == currentPeriodWeatherEventId)
		  removeIndexes.push_back(i);
	  }
	removeRedundantWeatherEvents(thePrecipitationWeatherEvents, removeIndexes);
  }

  // take into account the continuous precipitation periods
  void PrecipitationForecast::cleanUpPrecipitationWeatherEvents(weather_event_id_vector& thePrecipitationWeatherEvents)
  {
	vector<int> removeIndexes;

	for(unsigned int i = 1; i < thePrecipitationWeatherEvents.size(); i++)
	  {
		NFmiTime previousPeriodEndTime(thePrecipitationWeatherEvents[i-1].first);
		NFmiTime currentPeriodStartTime(thePrecipitationWeatherEvents[i].first);
		weather_event_id previousPeriodWeatherEventId(thePrecipitationWeatherEvents[i-1].second);
		weather_event_id currentPeriodWeatherEventId(thePrecipitationWeatherEvents[i].second);
		if(abs(previousPeriodEndTime.DifferenceInHours(currentPeriodStartTime)) < 2 &&
		   previousPeriodWeatherEventId == SADE_ALKAA && (currentPeriodWeatherEventId == POUTAANTUU ||
														  currentPeriodWeatherEventId == POUTAANTUU_WHEN_EXTENT_SMALL))
		  removeIndexes.push_back(i-1);
	  }
	removeRedundantWeatherEvents(thePrecipitationWeatherEvents, removeIndexes);
	removeDuplicatePrecipitationWeatherEvents(thePrecipitationWeatherEvents);
  }

  void PrecipitationForecast::printOutPrecipitationDistribution(std::ostream& theOutput) const
  {
	theOutput << "** PRECIPITATION DISTRIBUTION **" << endl; 
	if(theCoastalData.size() > 0)
	  {
		theOutput << "Coastal distribution: " << endl; 
		printOutPrecipitationDistribution(theOutput, theCoastalData);
	  }
	if(theInlandData.size() > 0)
	  {
		theOutput << "Inland distribution: " << endl; 
		printOutPrecipitationDistribution(theOutput, theInlandData);
	  }
	if(theFullData.size() > 0)
	  {
		theOutput << "Full area distribution: " << endl; 
		printOutPrecipitationDistribution(theOutput, theFullData);
	  }
  }

  void PrecipitationForecast::printOutPrecipitationDistribution(std::ostream& theOutput, const precipitation_data_vector& theDataVector) const
  {
	for(unsigned int i = 0; i < theDataVector.size(); i++)
	  {
		theOutput << "distribution(ne,se,sw,nw,n,s,e,w): " <<
		  theDataVector[i]->theObservationTime << ", " <<
		  theDataVector[i]->thePrecipitationPercentageNorthEast << ", " <<
		  theDataVector[i]->thePrecipitationPercentageSouthEast << ", " <<
		  theDataVector[i]->thePrecipitationPercentageSouthWest << ", " <<
		  theDataVector[i]->thePrecipitationPercentageNorthWest << ", " <<
		  theDataVector[i]->precipitationPercentageNorth() << ", " <<
		  theDataVector[i]->precipitationPercentageSouth() << ", " <<
		  theDataVector[i]->precipitationPercentageEast() << ", " <<
		  theDataVector[i]->precipitationPercentageWest() << endl;
	  }
  }	

  void PrecipitationForecast::printOutPrecipitationWeatherEvents(std::ostream& theOutput) const
  {
	theOutput << "** PRECIPITATION WEATHER EVENTS **" << endl; 
	bool isWeatherEvents = false;
	if(thePrecipitationWeatherEventsCoastal.size() > 0)
	  {
		theOutput << "Coastal precipitation weather events: " << endl; 
		print_out_weather_event_vector(theOutput, thePrecipitationWeatherEventsCoastal);
		isWeatherEvents = true;
	  }
	if(thePrecipitationWeatherEventsInland.size() > 0)
	  {
		theOutput << "Inland precipitation weather events: " << endl; 
		print_out_weather_event_vector(theOutput, thePrecipitationWeatherEventsInland);
		isWeatherEvents = true;
	  }
	if(thePrecipitationWeatherEventsFull.size() > 0)
	  {
		theOutput << "Full area precipitation weather events: " << endl; 
		print_out_weather_event_vector(theOutput, thePrecipitationWeatherEventsFull);
		isWeatherEvents = true;
	  }

	if(!isWeatherEvents)
	  theOutput << "No weather events!" << endl; 
  }

  void PrecipitationForecast::printOutPrecipitationPeriods(std::ostream& theOutput) const
  {
	theOutput << "** PRECIPITATION PERIODS **" << endl; 
	bool found = false;
	if(thePrecipitationPeriodsCoastal.size() > 0)
	  {
		theOutput << "Coastal precipitation periods: " << endl; 
		printOutPrecipitationPeriods(theOutput, thePrecipitationPeriodsCoastal, theCoastalData);
		found = true;
	  }
	if(thePrecipitationPeriodsInland.size() > 0)
	  {
		theOutput << "Inland precipitation periods: " << endl; 
		printOutPrecipitationPeriods(theOutput, thePrecipitationPeriodsInland, theInlandData);
		found = true;
	  }
	if(thePrecipitationPeriodsFull.size() > 0)
	  {
		theOutput << "Full precipitation periods: " << endl; 
		printOutPrecipitationPeriods(theOutput, thePrecipitationPeriodsFull, theFullData);
		found = true;
	  }

	if(!found)
	  theOutput << "No precipitation periods!" << endl; 
  }

  void PrecipitationForecast::printOutPrecipitationPeriods(std::ostream& theOutput,
														   const vector<WeatherPeriod>& thePrecipitationPeriods,
														   const precipitation_data_vector& theDataVector) const
  {
	for(unsigned int i = 0; i < thePrecipitationPeriods.size(); i++)
	  {
		WeatherPeriod period(thePrecipitationPeriods.at(i).localStartTime(),
							 thePrecipitationPeriods.at(i).localEndTime());

		theOutput << period.localStartTime()
				  << "..."
				  << period.localEndTime()
				  << ": "
				  << "min_of_max=" << setw(7) << setfill(' ') << setprecision(3)
				  << getMin(theDataVector, PRECIPITATION_MAX_DATA, period)
				  << "min_of_mean=" << setw(7) << setfill(' ') << setprecision(3)
				  << getMin(theDataVector, PRECIPITATION_MEAN_DATA, period)
				  << " mean_of_max=" << setw(7) << setfill(' ') << setprecision(3)
				  << getMean(theDataVector, PRECIPITATION_MAX_DATA, period)
				  << " mean_of_mean=" << setw(7) << setfill(' ') << setprecision(3)
				  << getMean(theDataVector, PRECIPITATION_MEAN_DATA, period)
				  << " max_of_max=" << setw(7) << setfill(' ') << setprecision(3)
				  << getMax(theDataVector, PRECIPITATION_MAX_DATA, period)
				  << " max_of_mean=" << setw(7) << setfill(' ') << setprecision(3)
				  << getMax(theDataVector, PRECIPITATION_MEAN_DATA, period);

		precipitation_traverse_id traverseId = getPrecipitationTraverseId(period);
		if(traverseId != MISSING_TRAVERSE_ID)
		  {
			theOutput << " movement=" << precipitation_traverse_string(traverseId);
		  }
		theOutput  << endl;

	  }
  }

  void PrecipitationForecast::printOutPrecipitationDistribution(std::ostream& theOutput,
																const WeatherPeriod& thePeriod)
  {
	//	if(!isDryPeriod(thePeriod, theParameters.theForecastArea))
	  {
		float northPercentage(0.0);
		float southPercentage(0.0);
		float eastPercentage(0.0);
		float westPercentage(0.0);
		float northEastPercentage(0.0);
		float southEastPercentage(0.0);
		float southWestPercentage(0.0);
		float northWestPercentage(0.0);

		getPrecipitationDistribution(thePeriod,
									 northPercentage,
									 southPercentage,
									 eastPercentage,
									 westPercentage,
									 northEastPercentage,
									 southEastPercentage,
									 southWestPercentage,
									 northWestPercentage);

		theOutput << "precipitation north: " << northPercentage << endl;
		theOutput << "precipitation south: " << southPercentage << endl;
		theOutput << "precipitation east: " << eastPercentage << endl;
		theOutput << "precipitation west: " << westPercentage << endl;
		theOutput << "precipitation northeast: " << northEastPercentage << endl;
		theOutput << "precipitation southeast: " << southEastPercentage << endl;
		theOutput << "precipitation southwest: " << southWestPercentage << endl;
		theOutput << "precipitation northwest: " << northWestPercentage << endl;
	  }
  }

  bool PrecipitationForecast::getPrecipitationPeriod(const NFmiTime& theTimestamp, 
													 NFmiTime& theStartTime,
													 NFmiTime& theEndTime) const
  {
	const vector<WeatherPeriod>* precipitationPeriodVector = 0;


	if(theParameters.theForecastArea & FULL_AREA)
	  precipitationPeriodVector = &thePrecipitationPeriodsFull;
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  precipitationPeriodVector = &thePrecipitationPeriodsCoastal;
	else if(theParameters.theForecastArea & INLAND_AREA)
	  precipitationPeriodVector = &thePrecipitationPeriodsInland;

	if(!precipitationPeriodVector)
	  return false;

	for(unsigned int i = 0; i < precipitationPeriodVector->size(); i++)
	  {
		if(is_inside(theTimestamp, precipitationPeriodVector->at(i)))
		  {
			theStartTime = precipitationPeriodVector->at(i).localStartTime();
			theEndTime = precipitationPeriodVector->at(i).localEndTime();
			//			theWeatherPeriod = precipitationPeriodVector->at(i);
			return true;
		  }
	  }
	return false;
  }

  Sentence PrecipitationForecast::precipitationChangeSentence(const WeatherPeriod& thePeriod,
															  const weather_event_id& theWeatherEvent) const
  {
	Sentence sentence;

	if(theWeatherEvent == POUTAANTUU || theWeatherEvent == POUTAANTUU_WHEN_EXTENT_SMALL)
	  {
		sentence << SAA_POUTAANTUU_PHRASE;
		theDryPeriodTautologyFlag = true;
	  }
	else // sade alkaa
	  {
		sentence << precipitationSentence(thePeriod);
	  }

	return sentence;
  }


  Sentence PrecipitationForecast::precipitationChangeSentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

	const weather_event_id_vector* thePrecipitationWeatherEventsVector = 
	  (theParameters.theForecastArea & FULL_AREA ? &thePrecipitationWeatherEventsFull :
	   (theParameters.theForecastArea & COASTAL_AREA ? &thePrecipitationWeatherEventsCoastal :
		(theParameters.theForecastArea & INLAND_AREA ? &thePrecipitationWeatherEventsInland : 0)));

	int periodLength = theParameters.theForecastPeriod.localEndTime().DifferenceInHours(theParameters.theForecastPeriod.localStartTime());

	if(thePrecipitationWeatherEventsVector)
	  {
		for(unsigned int i = 0; i < thePrecipitationWeatherEventsVector->size(); i++)
		  {
			NFmiTime precipitationWeatherEventTimestamp(thePrecipitationWeatherEventsVector->at(i).first);
			
			if(!(precipitationWeatherEventTimestamp >= thePeriod.localStartTime() &&
				 precipitationWeatherEventTimestamp <= thePeriod.localEndTime()))
			  {
				continue;
			  }

			weather_event_id trid(thePrecipitationWeatherEventsVector->at(i).second);
			if(trid == POUTAANTUU || trid == POUTAANTUU_WHEN_EXTENT_SMALL)
			  {
				if(sentence.size() > 0)
				  sentence << Delimiter(",");
				
				if(periodLength > 24)
				  {
					sentence << get_today_phrase(precipitationWeatherEventTimestamp,
												 theParameters.theVariable,
												 theParameters.theArea,
												 thePeriod,
												 theParameters.theForecastTime);
				  }

				sentence << get_time_phrase(precipitationWeatherEventTimestamp);
				sentence << SAA_POUTAANTUU_PHRASE;


				theDryPeriodTautologyFlag = true;
			  }
			else
			  {
				NFmiTime startTime;
				NFmiTime endTime;

				if(getPrecipitationPeriod(precipitationWeatherEventTimestamp, startTime, endTime))
				  {
					if(sentence.size() > 0)
					  sentence << Delimiter(",");
					
					if(periodLength > 24)
					  {
						sentence << get_today_phrase(precipitationWeatherEventTimestamp,
													 theParameters.theVariable,
													 theParameters.theArea,
													 thePeriod,
													 theParameters.theForecastTime);
					  }

					WeatherPeriod precipitationPeriod(startTime, endTime);

					if(isMostlyDryPeriod(precipitationPeriod, theParameters.theForecastArea))
					  {
						/*
						if(!theDryPeriodTautologyFlag)
						  sentence << SAA_WORD << ON_WORD;
						*/
					  }
					else
					  {
						sentence << get_time_phrase(precipitationWeatherEventTimestamp, true);
					  }
					sentence << precipitationSentence(WeatherPeriod(startTime, endTime));
				  }
			  }
		  }
	  }	

	return sentence;
  }


  void PrecipitationForecast::getPrecipitationDistribution(const WeatherPeriod& thePeriod,
														   float& theNorthPercentage,
														   float& theSouthPercentage,
														   float& theEastPercentage,
														   float& theWestPercentage,
														   float& theNorthEastPercentage,
														   float& theSouthEastPercentage,
														   float& theSouthWestPercentage,
														   float& theNorthWestPercentage) const
  {
	const precipitation_data_vector* precipitationDataVector =
	  ((theParameters.theForecastArea & FULL_AREA) ? &theFullData : 
	   ((theParameters.theForecastArea & INLAND_AREA) ? &theInlandData : &theCoastalData));

	theNorthPercentage = 0.0;
	theSouthPercentage = 0.0;
	theEastPercentage = 0.0;
	theWestPercentage = 0.0;
	theNorthEastPercentage = 0.0;
	theSouthEastPercentage = 0.0;
	theSouthWestPercentage = 0.0;
	theNorthWestPercentage = 0.0;

	unsigned int count = 0;
	for(unsigned int i = 0; i < precipitationDataVector->size(); i++)
	  {
		if(precipitationDataVector->at(i)->thePrecipitationIntensity > 0)
		  {
			theNorthEastPercentage += precipitationDataVector->at(i)->thePrecipitationPercentageNorthEast;
			theSouthEastPercentage += precipitationDataVector->at(i)->thePrecipitationPercentageSouthEast;
			theSouthWestPercentage += precipitationDataVector->at(i)->thePrecipitationPercentageSouthWest;
			theNorthWestPercentage += precipitationDataVector->at(i)->thePrecipitationPercentageNorthWest;
			count++;
		  }
	  }
	if(count > 0)
	  {
		theNorthEastPercentage /= count;
		theSouthEastPercentage /= count;
		theSouthWestPercentage /= count;
		theNorthWestPercentage /= count;
		theNorthPercentage = (theNorthEastPercentage + theNorthWestPercentage);
		theSouthPercentage = (theSouthEastPercentage + theSouthWestPercentage);
		theEastPercentage = (theNorthEastPercentage + theSouthEastPercentage);
		theWestPercentage = (theSouthEastPercentage + theSouthWestPercentage);
	  }
  }

  precipitation_form_transformation_id 
  PrecipitationForecast::getPrecipitationFormTransformationId(const WeatherPeriod& thePeriod, 
															  const unsigned short theForecastArea) const
  {
	if(thePeriod.localEndTime().DifferenceInHours(thePeriod.localStartTime()) < 5)
	  return NO_FORM_TRANSFORMATION;
	
	NFmiTime atStartBeg(thePeriod.localStartTime());
	if(atStartBeg.GetYear() == 1970)
	  atStartBeg = theParameters.theForecastPeriod.localStartTime();
	NFmiTime atStartEnd(atStartBeg);
	atStartEnd.ChangeByHours(2);
	WeatherPeriod atStartPeriod(atStartBeg, atStartEnd);
	NFmiTime atEndBeg(thePeriod.localEndTime());
	if(atEndBeg.GetYear() == 2037)
	  atEndBeg = theParameters.theForecastPeriod.localEndTime();	
	NFmiTime atEndEnd(atEndBeg);
	atEndBeg.ChangeByHours(-2);
	WeatherPeriod atEndPeriod(atEndBeg, atEndEnd);

	const precipitation_data_vector& theDataVector = getPrecipitationDataVector(theForecastArea);

	float precipitationFormWaterBeg = getMean(theDataVector, 
											  PRECIPITATION_FORM_WATER_DATA, 
											  WeatherPeriod(atStartBeg, atStartBeg));
	float precipitationFormDrizzleBeg = getMean(theDataVector, 
												PRECIPITATION_FORM_DRIZZLE_DATA, 
												WeatherPeriod(atStartBeg, atStartBeg));
	float precipitationFormSleetBeg = getMean(theDataVector, 
											  PRECIPITATION_FORM_SLEET_DATA, 
											  WeatherPeriod(atStartBeg, atStartBeg));
	float precipitationFormSnowBeg = getMean(theDataVector, 
											 PRECIPITATION_FORM_SNOW_DATA, 
											 WeatherPeriod(atStartBeg, atStartBeg));
	float precipitationFormWaterEnd = getMean(theDataVector, 
											  PRECIPITATION_FORM_WATER_DATA, 
											  WeatherPeriod(atEndEnd, atEndEnd));
	float precipitationFormDrizzleEnd = getMean(theDataVector, 
												PRECIPITATION_FORM_DRIZZLE_DATA, 
												WeatherPeriod(atEndEnd, atEndEnd));
	float precipitationFormSleetEnd = getMean(theDataVector, 
											  PRECIPITATION_FORM_SLEET_DATA, 
											  WeatherPeriod(atEndEnd, atEndEnd));
	float precipitationFormSnowEnd = getMean(theDataVector, 
											 PRECIPITATION_FORM_SNOW_DATA, 
											 WeatherPeriod(atEndEnd, atEndEnd));


	precipitation_form_id  precipitationFormBeg(MISSING_PRECIPITATION_FORM);
	precipitation_form_id  precipitationFormEnd(MISSING_PRECIPITATION_FORM);

	if(precipitationFormWaterBeg >= 90)
	  precipitationFormBeg = WATER_FORM;
	else if(precipitationFormDrizzleBeg >= 90)
	  precipitationFormBeg = DRIZZLE_FORM;
	else if(precipitationFormSleetBeg >= 90)
	  precipitationFormBeg = SLEET_FORM;
	else if(precipitationFormSnowBeg >= 90)
	  precipitationFormBeg = SNOW_FORM;

	if(precipitationFormWaterEnd >= 90)
	  precipitationFormEnd = WATER_FORM;
	else if(precipitationFormDrizzleEnd >= 90)
	  precipitationFormEnd = DRIZZLE_FORM;
	else if(precipitationFormSleetEnd >= 90)
	  precipitationFormEnd = SLEET_FORM;
	else if(precipitationFormSnowEnd >= 90)
	  precipitationFormEnd = SNOW_FORM;

	if(precipitationFormBeg == precipitationFormEnd)
	  return NO_FORM_TRANSFORMATION;

	const weather_result_data_item_vector* precipitationFormWaterHourly = 
	  get_data_vector(theParameters, PRECIPITATION_FORM_WATER_DATA);
	const weather_result_data_item_vector* precipitationFormSnowHourly = 
	  get_data_vector(theParameters, PRECIPITATION_FORM_SNOW_DATA);
	const weather_result_data_item_vector* precipitationFormDrizzleHourly = 
	  get_data_vector(theParameters, PRECIPITATION_FORM_DRIZZLE_DATA);
	const weather_result_data_item_vector* precipitationFormSleetHourly = 
	  get_data_vector(theParameters, PRECIPITATION_FORM_SLEET_DATA);

	unsigned int startIndex, endIndex;
	get_period_start_end_index(thePeriod, 
							   *precipitationFormWaterHourly,
							   startIndex,
							   endIndex);

	double pearson_co_water = get_pearson_coefficient(*precipitationFormWaterHourly, startIndex, endIndex);
	double pearson_co_snow = get_pearson_coefficient(*precipitationFormSnowHourly, startIndex, endIndex);
	double pearson_co_drizzle = get_pearson_coefficient(*precipitationFormDrizzleHourly, startIndex, endIndex);
	double pearson_co_sleet = get_pearson_coefficient(*precipitationFormSleetHourly, startIndex, endIndex);

	

	if(precipitationFormBeg == WATER_FORM)
	  {
		if(precipitationFormEnd == SNOW_FORM)
		  {
			if(pearson_co_water <= -PEARSON_CO_FORM_TRANSFORM && pearson_co_snow >= PEARSON_CO_FORM_TRANSFORM)
			  return WATER_TO_SNOW;
		  }
		else if(precipitationFormEnd == DRIZZLE_FORM)
		  {
			if(pearson_co_water <= -PEARSON_CO_FORM_TRANSFORM && pearson_co_drizzle >= PEARSON_CO_FORM_TRANSFORM)
			  return WATER_TO_DRIZZLE;
		  }
		else if(precipitationFormEnd == SLEET_FORM)
		  {
			if(pearson_co_water <= -PEARSON_CO_FORM_TRANSFORM && pearson_co_sleet >= PEARSON_CO_FORM_TRANSFORM)
			  return WATER_TO_SLEET;
		  }
	  }
	else if(precipitationFormBeg == DRIZZLE_FORM)
	  {
		if(precipitationFormEnd == SNOW_FORM)
		  {
			if(pearson_co_drizzle <= -PEARSON_CO_FORM_TRANSFORM && pearson_co_snow >= PEARSON_CO_FORM_TRANSFORM)
			  return DRIZZLE_TO_SNOW;
		  }
		else if(precipitationFormEnd == WATER_FORM)
		  {
			if(pearson_co_drizzle <= -PEARSON_CO_FORM_TRANSFORM && pearson_co_water >= PEARSON_CO_FORM_TRANSFORM)
			  return DRIZZLE_TO_WATER;
		  }
		else if(precipitationFormEnd == SLEET_FORM)
		  {
			if(pearson_co_drizzle <= -PEARSON_CO_FORM_TRANSFORM && pearson_co_sleet >= PEARSON_CO_FORM_TRANSFORM)
			  return DRIZZLE_TO_SLEET;
		  }
	  }
	else if(precipitationFormBeg == SLEET_FORM)
	  {
		if(precipitationFormEnd == SNOW_FORM)
		  {
			if(pearson_co_sleet <= -PEARSON_CO_FORM_TRANSFORM && pearson_co_snow >= PEARSON_CO_FORM_TRANSFORM)
			  return SLEET_TO_SNOW;
		  }
		else if(precipitationFormEnd == DRIZZLE_FORM)
		  {
			if(pearson_co_sleet <= -PEARSON_CO_FORM_TRANSFORM && pearson_co_drizzle >= PEARSON_CO_FORM_TRANSFORM)
			  return SLEET_TO_DRIZZLE;
		  }
		else if(precipitationFormEnd == WATER_FORM)
		  {
			if(pearson_co_sleet <= -PEARSON_CO_FORM_TRANSFORM && pearson_co_water >= PEARSON_CO_FORM_TRANSFORM)
			  return SLEET_TO_WATER;
		  }
	  }
	else if(precipitationFormBeg == SNOW_FORM)
	  {
		if(precipitationFormEnd == WATER_FORM)
		  {
			if(pearson_co_snow <= -PEARSON_CO_FORM_TRANSFORM && pearson_co_water >= PEARSON_CO_FORM_TRANSFORM)
			  return SNOW_TO_WATER;
		  }
		else if(precipitationFormEnd == DRIZZLE_FORM)
		  {
			if(pearson_co_snow <= -PEARSON_CO_FORM_TRANSFORM && pearson_co_drizzle >= PEARSON_CO_FORM_TRANSFORM)
			  return SNOW_TO_DRIZZLE;
		  }
		else if(precipitationFormEnd == SLEET_FORM)
		  {
			if(pearson_co_snow <= -PEARSON_CO_FORM_TRANSFORM && pearson_co_sleet >= PEARSON_CO_FORM_TRANSFORM)
			  return SNOW_TO_SLEET;
		  }
	  }

	return NO_FORM_TRANSFORMATION;

  }

  Sentence  PrecipitationForecast::constructPrecipitationSentence(const WeatherPeriod& thePeriod,
																  const unsigned short& theForecastAreaId) const
  {
	Sentence sentence;

	const precipitation_data_vector* dataVector = 0;

	if(theForecastAreaId & INLAND_AREA && theForecastAreaId & COASTAL_AREA)
	  {
		if(separateCoastInlandPrecipitation(thePeriod))
		  {
			sentence << INLAND_PHRASE;
			sentence << constructPrecipitationSentence(thePeriod,
													   INLAND_AREA);
			sentence << Delimiter(",");
			sentence << COAST_PHRASE;
			sentence << constructPrecipitationSentence(thePeriod,
													   COASTAL_AREA);
		  }
		else
		  {
			dataVector = &theFullData;
		  }
	  }
	else if(theForecastAreaId & INLAND_AREA)
	  {
		dataVector = &theInlandData;
	  }
	else if(theForecastAreaId & COASTAL_AREA)
	  {
		dataVector = &theCoastalData;
	  }

	if(dataVector)
	  {
		NFmiTime previousPeriodStartTime(thePeriod.localStartTime());
		NFmiTime previousPeriodEndTime(thePeriod.localStartTime());
		previousPeriodStartTime.ChangeByHours(-6);
		WeatherPeriod previousPeriod(previousPeriodStartTime, previousPeriodEndTime);

		float precipitationIntensity = getMean(*dataVector, PRECIPITATION_MEAN_DATA, thePeriod);
		float precipitationAbsoluteMaxIntensity = getMax(*dataVector, PRECIPITATION_MAX_DATA, thePeriod);
		theParameters.theLog << "Mean intensity: " << precipitationIntensity << endl;
		theParameters.theLog << "Maximum intensity: " << precipitationAbsoluteMaxIntensity << endl;

		float precipitationExtent = getMean(*dataVector, PRECIPITATION_EXTENT_DATA, thePeriod);
		float precipitationFormWater = getMean(*dataVector, PRECIPITATION_FORM_WATER_DATA, thePeriod);
		float precipitationFormDrizzle = getMean(*dataVector, PRECIPITATION_FORM_DRIZZLE_DATA, thePeriod);
		float precipitationFormSleet = getMean(*dataVector, PRECIPITATION_FORM_SLEET_DATA, thePeriod);
		float precipitationFormSnow = getMean(*dataVector, PRECIPITATION_FORM_SNOW_DATA, thePeriod);
		float precipitationFormFreezing = getMean(*dataVector, PRECIPITATION_FORM_FREEZING_DATA, thePeriod);

		precipitation_type precipitationType(getPrecipitationType(thePeriod, theParameters.theForecastArea));

		unsigned int typeChangeIndex = getPrecipitationTypeChange(*dataVector, thePeriod);

		unsigned int precipitationForm = get_complete_precipitation_form(theParameters.theVariable,
																		 precipitationFormWater,
																		 precipitationFormDrizzle,
																		 precipitationFormSleet,
																		 precipitationFormSnow,
																		 precipitationFormFreezing);
		


		sentence << selectPrecipitationSentence(thePeriod,
												precipitationForm,
												precipitationIntensity,
												precipitationAbsoluteMaxIntensity,
												precipitationExtent,
												precipitationFormWater,
												precipitationFormDrizzle,
												precipitationFormSleet,
												precipitationFormSnow,
												precipitationFormFreezing,
												precipitationType,
												dataVector->at(typeChangeIndex)->theObservationTime,
												getPrecipitationFormTransformationId(thePeriod, theForecastAreaId));

		bool dry_weather = is_dry_weather(theParameters, 
										  precipitationForm,
										  precipitationIntensity, 
										  precipitationExtent);

		bool mostly_dry_weather = precipitationExtent <= theParameters.theMostlyDryWeatherLimit;

		if(!dry_weather && !mostly_dry_weather)
		  {
			sentence << areaSpecificSentence(thePeriod);

			Sentence thunderSentence;
			thunderSentence << theParameters.theThunderForecast->thunderSentence(thePeriod);
			sentence << thunderSentence;
		  }
	  }

	return sentence;
  }

  bool PrecipitationForecast::shortTermPrecipitationExists(const WeatherPeriod& thePeriod) const
  {
	const vector<WeatherPeriod>* precipitationPeriodVector = 0;

	if(theParameters.theForecastArea & FULL_AREA)
	  precipitationPeriodVector = &thePrecipitationPeriodsFull;
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  precipitationPeriodVector = &thePrecipitationPeriodsCoastal;
	else if(theParameters.theForecastArea & INLAND_AREA)
	  precipitationPeriodVector = &thePrecipitationPeriodsInland;

	if(precipitationPeriodVector)
	  {
		for(unsigned int i = 0; i < precipitationPeriodVector->size(); i++)
		  {
			NFmiTime startTime(precipitationPeriodVector->at(i).localStartTime());
			NFmiTime endTime(precipitationPeriodVector->at(i).localEndTime());

			if(endTime.DifferenceInHours(startTime) < 6 && 
			   is_inside(startTime, thePeriod) &&
			   is_inside(endTime, thePeriod))
			  {
				return true;
			  }
		  }
	  }

	return false;
  }


  Sentence PrecipitationForecast::shortTermPrecipitationSentenceEnh(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

	sentence << constructPrecipitationSentence(thePeriod, theParameters.theForecastArea);

	theParameters.theLog << "Short term precipitation sentence: ";
	theParameters.theLog << sentence;

	return sentence;
  }


  // check precipitation that lasts < 6 hours
  Sentence PrecipitationForecast::shortTermPrecipitationSentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

	const vector<WeatherPeriod>* precipitationPeriodVector = 0;

	if(theParameters.theForecastArea & FULL_AREA)
	  precipitationPeriodVector = &thePrecipitationPeriodsFull;
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  precipitationPeriodVector = &thePrecipitationPeriodsCoastal;
	else if(theParameters.theForecastArea & INLAND_AREA)
	  precipitationPeriodVector = &thePrecipitationPeriodsInland;

	if(precipitationPeriodVector)
	  {
		NFmiTime previousEndTime(thePeriod.localStartTime());
		for(unsigned int i = 0; i < precipitationPeriodVector->size(); i++)
		  {
			NFmiTime startTime(precipitationPeriodVector->at(i).localStartTime());
			NFmiTime endTime(precipitationPeriodVector->at(i).localEndTime());
			int periodLength = theParameters.theForecastPeriod.localEndTime().DifferenceInHours(theParameters.theForecastPeriod.localStartTime());
			
			//			int differenceInHours(endTime.DifferenceInHours(startTime));
			if(endTime.DifferenceInHours(startTime) < 6 && 
			   is_inside(startTime, thePeriod) &&
			   is_inside(endTime, thePeriod))
			  {
				// If there are successive short precipitation periods,
				// we report the cloudiness in between

				// Anssi Vähämäki: Jos tässä esimerkissä aamun ja illan sateiden välissä on 
				// esim. ALLE 3 tunnin mittainen verrattain selkeä jakso voisi sitä kommentoida 
				// (päivällä mahdollisesti selkeää TAI päivällä paikoin selkeää...  
				// aamulla tulee vesi - tai räntäkuuroja, päivällä on paikoin selkeää. 
				// Illalla saadaan heikkoja vesikuuroja.. ) Eli ehkä illan kuuroissa ei tarvitse pohtia 
				// jälleen-sanan käyttöä, kun vaihdellaan tulee ja saadaan sanoja.

				// Jos tässä esimerkissä aamun ja illan sateiden välissä on esim. YLI 3 tunnin mittainen 
				// verrattain selkeä jakso voisi sitä kommentoida (päivällä selkeää TAI 
				// päivällä monin paikoin selkeää...  aamulla tulee vesi - tai räntäkuuroja, 
				// päivällä on (laajalti, monin paikoin...) selkeää. Illalla saadaan heikkoja vesikuuroja..)

				Sentence todaySentence;
				if(periodLength > 24)
				  {
					todaySentence << get_today_phrase(startTime,
													  theParameters.theVariable,
													  theParameters.theArea,
													  WeatherPeriod(startTime, endTime),
													  theParameters.theForecastTime);
				  }

				if(previousEndTime != thePeriod.localStartTime())
				  {
					NFmiTime clPeriodStart(previousEndTime);
					NFmiTime clPeriodEnd(startTime);
					clPeriodStart.ChangeByHours(1);
					clPeriodEnd.ChangeByHours(-1);

					cloudiness_id clid = 
					  theParameters.theCloudinessForecast->getCloudinessId(WeatherPeriod(previousEndTime, startTime));
					if(clid == MELKO_SELKEAA || clid == SELKEAA)
					  {
						if(sentence.size() > 0)
						  sentence << Delimiter(",");

						sentence << todaySentence;
						if(startTime.DifferenceInHours(previousEndTime) < 3)
						  {
							sentence << get_time_phrase_large(WeatherPeriod(clPeriodStart, clPeriodEnd)) 
									 << MAHDOLLISESTI_WORD 
									 << SELKEAA_WORD;
						  }
						else
						  {
							sentence << get_time_phrase_large(WeatherPeriod(clPeriodStart, clPeriodEnd))
									 << MONIN_PAIKOIN_WORD 
									 << SELKEAA_WORD;
						  }
					  }
				  }

				if(sentence.size() > 0)
				  sentence << Delimiter(",");
				
				sentence << todaySentence;

				WeatherPeriod shortTermPrecipitationPeriod(startTime, endTime);

				if(!isMostlyDryPeriod(shortTermPrecipitationPeriod,theParameters.theForecastArea))
				  {
					if(get_part_of_the_day_id(startTime) == get_part_of_the_day_id(endTime))
					  {
						sentence << get_time_phrase(startTime);
					  }
					else
					  {
						sentence << get_time_phrase_large(precipitationPeriodVector->at(i));
					  }
				  }
				else
				  {
					/*
					if(!theDryPeriodTautologyFlag)
					  sentence << SAA_WORD << ON_WORD;
					*/
				  }

				sentence << constructPrecipitationSentence(WeatherPeriod(startTime, endTime), 
														   theParameters.theForecastArea);

				previousEndTime = endTime;
				break;
			  }
		  }
	  }
	theParameters.theLog << "Short term precipitation sentence: ";
	theParameters.theLog << sentence;

	return sentence;
  }

  Sentence PrecipitationForecast::precipitationSentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;
	
	sentence <<  constructPrecipitationSentence(thePeriod,
												theParameters.theForecastArea);

	return sentence;
  }

  Sentence PrecipitationForecast::areaSpecificSentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

	// If the area contains both coast and inland, we don't use area specific sentence
	if(theParameters.theForecastArea & FULL_AREA)
	  return sentence;


	float north(0.0);
	float south(0.0);
	float east(0.0);
	float west(0.0);
	float northEast(0.0);
	float southEast(0.0);
	float southWest(0.0);
	float northWest(0.0);

	getPrecipitationDistribution(thePeriod,
								 north,
								 south,
								 east,
								 west,
								 northEast,
								 southEast,
								 southWest,
								 northWest);
		
	area_specific_sentence_id sentenceId = 
	  get_area_specific_sentence_id(north,
									south,
									east,
									west,
									northEast,
									southEast,
									southWest,
									northWest);

	Rect areaRect(theParameters.theArea);
	NFmiMercatorArea mercatorArea(areaRect.getBottomLeft(), areaRect.getTopRight());
	float areaHeightWidthRatio = mercatorArea.WorldRect().Height()/mercatorArea.WorldRect().Width();

	Sentence areaSpecificSentence;
	areaSpecificSentence << area_specific_sentence(north,
												   south,
												   east,
												   west,
												   northEast,
												   southEast,
												   southWest,
												   northWest);

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

	return sentence;
  }

  void PrecipitationForecast::getWeatherEventIdVector(weather_event_id_vector& thePrecipitationWeatherEvents) const
  {
	const weather_event_id_vector* vectorToClone = 0;

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  vectorToClone = &thePrecipitationWeatherEventsFull;
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  vectorToClone = &thePrecipitationWeatherEventsCoastal;
	else if(theParameters.theForecastArea & INLAND_AREA)
	  vectorToClone = &thePrecipitationWeatherEventsInland;

	if(vectorToClone)
	  thePrecipitationWeatherEvents = *vectorToClone;
  }

  Rect PrecipitationForecast::getPrecipitationRect(const NFmiTime& theTimestamp, 
												   const float& theLowerLimit,
												   const float& theUpperLimit) const
  {
	NFmiIndexMask indexMask;
	RangeAcceptor precipitationlimits;
	precipitationlimits.lowerLimit(theLowerLimit);
	precipitationlimits.upperLimit(theUpperLimit);

	// precipitation in the beginning
	ExtractMask(theParameters.theSources,
				Precipitation,
				theParameters.theArea,
				WeatherPeriod(theTimestamp, theTimestamp),
				precipitationlimits,
				indexMask);

	return Rect(theParameters.theSources, Precipitation, indexMask);
 }

  direction_id PrecipitationForecast::getPrecipitationLeavingDirection(const WeatherPeriod& thePeriod) const
  {
	direction_id retval(NO_DIRECTION);

	NFmiTime startTime(thePeriod.localEndTime());
	NFmiTime endTime(thePeriod.localEndTime());
	endTime.ChangeByHours(1);
	startTime.ChangeByHours(1);
	WeatherPeriod checkPeriod(startTime, endTime);

	unsigned int startIndex, endIndex;
 	weather_result_data_item_vector* northeast_data = get_data_vector(theParameters, PRECIPITATION_NORTHEAST_SHARE_DATA);
 	weather_result_data_item_vector* southeast_data = get_data_vector(theParameters, PRECIPITATION_SOUTHEAST_SHARE_DATA);
 	weather_result_data_item_vector* southwest_data = get_data_vector(theParameters, PRECIPITATION_SOUTHWEST_SHARE_DATA);
 	weather_result_data_item_vector* northwest_data = get_data_vector(theParameters, PRECIPITATION_NORTHWEST_SHARE_DATA);

	if(!get_period_start_end_index(checkPeriod, *northeast_data, startIndex, endIndex))
	  return retval;

	float northeast = northeast_data->at(startIndex)->theResult.value();
	float southeast = southeast_data->at(startIndex)->theResult.value();
	float southwest = southwest_data->at(startIndex)->theResult.value();
	float northwest = northwest_data->at(startIndex)->theResult.value();
	float north = northeast + northwest;
	float south = southeast + southwest;
	float east = northeast + southeast;
	float west = southwest + northwest;

	if(north >= 80)
	  retval = SOUTH;
	else if(south >= 80)
	  retval = NORTH;
	else if(east >= 80)
	  retval = WEST;
	else if(west >= 80)
	  retval = EAST;

	/*
	Rect areaRect(theParameters.theArea);
	Rect precipitationRect(getPrecipitationRect(thePeriod.localEndTime(), 0.0, 0.02));

	retval = getDirection(areaRect, precipitationRect);

	*/

	return retval;
  }

  direction_id PrecipitationForecast::getPrecipitationArrivalDirection(const WeatherPeriod& thePeriod) const
  {
	direction_id retval(NO_DIRECTION);

	NFmiTime startTime(thePeriod.localStartTime());
	startTime.ChangeByHours(-1);
	NFmiTime endTime(thePeriod.localStartTime());
	WeatherPeriod checkPeriod(startTime, endTime);

	unsigned int startIndex, endIndex;
 	weather_result_data_item_vector* northeast_data = get_data_vector(theParameters, PRECIPITATION_NORTHEAST_SHARE_DATA);
 	weather_result_data_item_vector* southeast_data = get_data_vector(theParameters, PRECIPITATION_SOUTHEAST_SHARE_DATA);
 	weather_result_data_item_vector* southwest_data = get_data_vector(theParameters, PRECIPITATION_SOUTHWEST_SHARE_DATA);
 	weather_result_data_item_vector* northwest_data = get_data_vector(theParameters, PRECIPITATION_NORTHWEST_SHARE_DATA);

	if(!get_period_start_end_index(checkPeriod, *northeast_data, startIndex, endIndex))
	  return retval;

	float northeast = northeast_data->at(startIndex)->theResult.value();
	float southeast = southeast_data->at(startIndex)->theResult.value();
	float southwest = southwest_data->at(startIndex)->theResult.value();
	float northwest = northwest_data->at(startIndex)->theResult.value();
	float north = northeast + northwest;
	float south = southeast + southwest;
	float east = northeast + southeast;
	float west = southwest + northwest;

	if(northeast >= 80)
	  retval = NORTHEAST;
	else if(southeast >= 80)
	  retval = SOUTHEAST;
	else if(southwest >= 80)
	  retval = SOUTHWEST;
	else if(northwest >= 80)
	  retval = NORTHWEST;
	else if(north >= 80)
	  retval = NORTH;
	else if(south >= 80)
	  retval = SOUTH;
	else if(east >= 80)
	  retval = EAST;
	else if(west >= 80)
	  retval = WEST;


	/*
	Rect areaRect(theParameters.theArea);
	Rect precipitationRect(getPrecipitationRect(thePeriod.localStartTime(), 0.02, 100.0));

	retval = getDirection(areaRect, precipitationRect);

	if(retval != NO_DIRECTION)
	  {
		theParameters.theLog << "Whole area: " << (string)areaRect << endl;
		theParameters.theLog << "Precipitation area: " << (string)precipitationRect << endl;
		theParameters.theLog << "West: " <<	(string)areaRect.subRect(WEST) << endl;
		theParameters.theLog << "East: " <<	(string)areaRect.subRect(EAST) << endl;

		theParameters.theLog << "Forecast period: "
							 << theParameters.theForecastPeriod.localStartTime()
							 << " ... "
							 << theParameters.theForecastPeriod.localEndTime()
							 << endl;
		theParameters.theLog << "Rain direction check period: "
							 << thePeriod.localStartTime()
							 << " ... "
							 << thePeriod.localStartTime()
							 << endl;
		theParameters.theLog << getDirectionString(retval) << endl; 
	  }
	*/
	return retval;
  }

  NFmiPoint PrecipitationForecast::getPrecipitationRepresentativePoint(const WeatherPeriod& thePeriod) const
  {
	NFmiPoint retval(kFloatMissing, kFloatMissing);

	unsigned int startIndex, endIndex;
 	weather_result_data_item_vector* coordinates = get_data_vector(theParameters, PRECIPITATION_POINT_DATA);
	if(!get_period_start_end_index(thePeriod, *coordinates, startIndex, endIndex))
	  return retval;

	float lon = 0.0;
	float lat = 0.0;
	unsigned int count = 0;
	for(unsigned int i = startIndex; i <= endIndex; i++)
	  {
		if(coordinates->at(i)->theResult.value() != kFloatMissing)
		  {
			lon += coordinates->at(i)->theResult.value();
			lat += coordinates->at(i)->theResult.error();
			count++;
		  }
	  }

	if(count > 0)
	  {
		retval.X(lon/count);
		retval.Y(lat/count);
	  }

	return retval;
 }

  precipitation_traverse_id PrecipitationForecast::getPrecipitationTraverseId(const WeatherPeriod& thePeriod) const
  {
	precipitation_traverse_id retval(MISSING_TRAVERSE_ID);

	const weather_result_data_item_vector* dataVector = get_data_vector(theParameters, PRECIPITATION_MEAN_DATA);

	unsigned int startIndex, endIndex;

	if(!get_period_start_end_index(thePeriod, *dataVector, startIndex, endIndex))
	  return retval;

	NFmiIndexMask begIndexMask;
	NFmiIndexMask endIndexMask;
	RangeAcceptor precipitationlimits;
	precipitationlimits.lowerLimit(0.02);
	
	ExtractMask(theParameters.theSources,
				Precipitation,
				theParameters.theArea,
				WeatherPeriod(thePeriod.localStartTime(),thePeriod.localStartTime()),
				precipitationlimits,
				begIndexMask);
	ExtractMask(theParameters.theSources,
				Precipitation,
				theParameters.theArea,
				WeatherPeriod(thePeriod.localEndTime(),thePeriod.localEndTime()),
				precipitationlimits,
				endIndexMask);

	Rect areaRect(theParameters.theArea);
	Rect begRect(theParameters.theSources, Precipitation, begIndexMask);
	Rect endRect(theParameters.theSources, Precipitation, endIndexMask);

	direction_id begDirection = getDirection(areaRect, begRect);
	direction_id endDirection = getDirection(areaRect, endRect);

	weather_result_data_item_vector* coordinates = get_data_vector(theParameters, PRECIPITATION_POINT_DATA);
	get_period_start_end_index(thePeriod,
							   *coordinates,
							   startIndex,
							   endIndex);

	double pearson_co_lon =  get_pearson_coefficient(*coordinates, startIndex, endIndex, false);
	double pearson_co_lat =  get_pearson_coefficient(*coordinates, startIndex, endIndex, true);

	if(begDirection == EAST && endDirection == WEST && pearson_co_lon <= -0.85)
	  {
		retval = FROM_EAST_TO_WEST;
	  }
	else if(begDirection == WEST && endDirection == EAST && pearson_co_lon >= 0.85)
	  {
		retval = FROM_WEST_TO_EAST;		
	  }
	else if(begDirection == SOUTH && endDirection == NORTH && pearson_co_lat >= 0.85)
	  {
		retval = FROM_SOUTH_TO_NORTH;
	  }
	else if(begDirection == NORTH && endDirection == SOUTH && pearson_co_lat <= -0.85)
	  {
		retval = FROM_NORTH_TO_SOUTH;
	  }
	else if(begDirection == NORTHEAST && endDirection == SOUTHWEST && 
			pearson_co_lon <= -0.85 && pearson_co_lat <= -0.85)
	  {
		retval = FROM_NORTHEAST_TO_SOUTHWEST;
	  }
	else if(begDirection == SOUTHWEST && endDirection == NORTHEAST &&
			pearson_co_lon >= 0.85 && pearson_co_lat >= 0.85)
	  {
		retval = FROM_SOUTHWEST_TO_NORTHEAST;
	  }
	else if(begDirection == NORTHWEST && endDirection == SOUTHEAST &&
			pearson_co_lon >= 0.85 && pearson_co_lat <= -0.85)
	  {
		retval = FROM_NORTHWEST_TO_SOUTHEAST;
	  }
	else if(begDirection == SOUTHEAST && endDirection == NORTHWEST &&
			pearson_co_lon <= -0.85 && pearson_co_lat >= 0.85)
	  {
		retval = FROM_SOUTHEAST_TO_NORTHWEST;
	  }

	if(retval != MISSING_TRAVERSE_ID)
	  {
		theParameters.theLog << "Sadealue kulkee " 
							 << precipitation_traverse_string(retval) 
							 << endl;
	  }

	return retval;
  }

  const precipitation_data_vector&  PrecipitationForecast::getPrecipitationDataVector(const unsigned short theForecastArea) const
  {
	if(theForecastArea & INLAND_AREA && theForecastArea & COASTAL_AREA)
	  {
		return theFullData;
	  }
	else if(theForecastArea & COASTAL_AREA)
	  {
		return theCoastalData;
	  }
	else
	  {
		return theInlandData;
	  }
  }

  PrecipitationForecast::PrecipitationForecast(wf_story_params& parameters)
	: theParameters(parameters)
	, theUseOllaVerb(false)
	, theDryPeriodTautologyFlag(false)
  {
	gatherPrecipitationData();
	findOutPrecipitationPeriods();
	findOutPrecipitationWeatherEvents();
  }

  PrecipitationForecast::~PrecipitationForecast()
  {
	theCoastalData.clear();
	theInlandData.clear();
	theFullData.clear();
  }
} // namespace TextGen

// ======================================================================
