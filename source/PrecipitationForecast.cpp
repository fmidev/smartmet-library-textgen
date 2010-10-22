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
	theOutput << setw(8) << setfill(' ') << setprecision(3) << thePrecipitationDataItemData.thePrecipitationIntensity;
	theOutput << setw(8) << setfill(' ') << setprecision(1) << thePrecipitationDataItemData.thePrecipitationExtent;
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
				  << thePrecipitationDataItemData.thePrecipitationPercentageNorthWest << "" << endl;
	  }
	else
	  {
		theOutput << endl;
	  }
	/*
	theOutput << thePrecipitationDataItemData.theObservationTime << ": "
			  << "intensity=" << setprecision(3) << thePrecipitationDataItemData.thePrecipitationIntensity << " "
			  << "extent=" << setprecision(3) << thePrecipitationDataItemData.thePrecipitationExtent;
	if(thePrecipitationDataItemData.thePrecipitationIntensity > 0)
	  {
		theOutput << " ";
		if(thePrecipitationDataItemData.thePrecipitationFormWater > 0.0)
		  theOutput << "water=" << fixed << setprecision(2) 
					<< thePrecipitationDataItemData.thePrecipitationFormWater << " ";
		if(thePrecipitationDataItemData.thePrecipitationFormDrizzle > 0.0)
		  theOutput << "drizzle=" << fixed << setprecision(2) 
					<< thePrecipitationDataItemData.thePrecipitationFormDrizzle << " ";
		if(thePrecipitationDataItemData.thePrecipitationFormSleet > 0.0)
		  theOutput << "sleet=" << fixed << setprecision(2) 
					<< thePrecipitationDataItemData.thePrecipitationFormSleet << " ";
		if( thePrecipitationDataItemData.thePrecipitationFormSnow > 0.0)
		  theOutput << "snow=" << fixed << setprecision(2) 
					<< thePrecipitationDataItemData.thePrecipitationFormSnow << " ";
		if(thePrecipitationDataItemData.thePrecipitationFormFreezing > 0.0)
		  theOutput << "freezing=" << fixed << setprecision(2) 
					<< thePrecipitationDataItemData.thePrecipitationFormFreezing << " ";
		if(thePrecipitationDataItemData.thePrecipitationTypeShowers > 0.0)
		  theOutput << "showers=" << fixed << setprecision(2) 
					<< thePrecipitationDataItemData.thePrecipitationTypeShowers << " ";

		theOutput << "ne=" << fixed << setprecision(2) 
				  << thePrecipitationDataItemData.thePrecipitationPercentageNorthEast << " "
				  << "se=" << fixed << setprecision(2) 
				  << thePrecipitationDataItemData.thePrecipitationPercentageSouthEast << " "
				  << "sw=" << fixed << setprecision(2) 
				  << thePrecipitationDataItemData.thePrecipitationPercentageSouthWest << " "
				  << "nw=" << fixed << setprecision(2) 
				  << thePrecipitationDataItemData.thePrecipitationPercentageNorthWest << "" << endl;
	  }
	else
	  {
		theOutput << endl;
	  }
	*/

	/*
	theOutput << "timestamp = " 
			  << thePrecipitationDataItemData.theObservationTime << endl;
	theOutput << "intensity = " 
			  << thePrecipitationDataItemData.thePrecipitationIntensity << endl;
	theOutput << "extent = " 
			  << thePrecipitationDataItemData.thePrecipitationExtent << endl;
	if(thePrecipitationDataItemData.thePrecipitationIntensity > 0)
	  {
		theOutput << "water-form = " 
				  << thePrecipitationDataItemData.thePrecipitationFormWater << endl;
		theOutput << "drizzle-form = " 
				  << thePrecipitationDataItemData.thePrecipitationFormDrizzle << endl;
		theOutput << "sleet-form = " 
				  << thePrecipitationDataItemData.thePrecipitationFormSleet << endl;
		theOutput << "snow-form = " 
				  << thePrecipitationDataItemData.thePrecipitationFormSnow << endl;
		theOutput << "freezing-form = " 
				  << thePrecipitationDataItemData.thePrecipitationFormFreezing << endl;
		theOutput << "showers-type = " 
				  << thePrecipitationDataItemData.thePrecipitationTypeShowers << endl;
		theOutput << "precipitation nort = " 
				  << thePrecipitationDataItemData.precipitationPercentageNorth() << endl;
		theOutput << "precipitation south = " 
				  << thePrecipitationDataItemData.precipitationPercentageSouth() << endl;
		theOutput << "precipitation east = " 
				  << thePrecipitationDataItemData.precipitationPercentageEast() << endl;
		theOutput << "precipitation west = " 
				  << thePrecipitationDataItemData.precipitationPercentageWest() << endl;
		theOutput << "precipitation northeast = " 
				  << thePrecipitationDataItemData.thePrecipitationPercentageNorthEast << endl;
		theOutput << "precipitation southeast = " 
				  << thePrecipitationDataItemData.thePrecipitationPercentageSouthEast << endl;
		theOutput << "precipitation southwest = " 
				  << thePrecipitationDataItemData.thePrecipitationPercentageSouthWest << endl;
		theOutput << "precipitation northwest = " 
				  << thePrecipitationDataItemData.thePrecipitationPercentageNorthWest << endl;
	  }
	*/

	return theOutput;
  }

  std::string operator<<(std::string& theDestinationString, const std::string& theSourceString)
  {
	theDestinationString.append(theSourceString);
	//	theDestinationString.append(" ");

	return theDestinationString;
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

  std::string can_be_freezing_phrase(const bool& theCanBeFreezingFlag)
  {
	std::string retval;

	if(theCanBeFreezingFlag)
	  {
		retval << ",";
		retval << JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE;
	  }
	return retval;
  }

  int precipitation_sentence_string_vector(wf_story_params& theParameters,
										   const WeatherPeriod& thePeriod,
										   const unsigned int& thePrecipitationForm,
										   const float thePrecipitationIntensity,
										   const float thePrecipitationExtent,
										   const float thePrecipitationFormWater,
										   const float thePrecipitationFormDrizzle,
										   const float thePrecipitationFormSleet,
										   const float thePrecipitationFormSnow,
										   const float thePrecipitationFormFreezing,
										   const float thePrecipitationTypeShowers,
										   const bool& theUseOllaVerb,
										   vector<std::string>& theStringVector)
  {
	bool dry_weather = is_dry_weather(theParameters, thePrecipitationForm, thePrecipitationIntensity, thePrecipitationExtent);

	if(dry_weather)
	  {
		theStringVector.push_back(POUTAINEN_WORD);
	  }
	else
	  {
		const bool has_showers = (thePrecipitationTypeShowers != kFloatMissing && 
								  thePrecipitationTypeShowers >= theParameters.theShowerLimit);
		const bool mostly_dry_weather = thePrecipitationExtent <= theParameters.theMostlyDryWeatherLimit;
		const bool in_some_places = thePrecipitationExtent > theParameters.theInSomePlacesLowerLimit && 
		  thePrecipitationExtent <= theParameters.theInSomePlacesUpperLimit;
		const bool in_many_places = thePrecipitationExtent > theParameters.theInManyPlacesLowerLimit && 
		  thePrecipitationExtent <= theParameters.theInManyPlacesUpperLimit;
		  		
		bool can_be_freezing =  thePrecipitationFormFreezing > theParameters.theFreezingPrecipitationLimit;
		bool is_summer = SeasonTools::isSummer(thePeriod.localStartTime(), theParameters.theVariable);

		if(has_showers)
		  ; // report the cloudiness?
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
					  theStringVector.push_back(ENIMMAKSEEN_POUTAINEN_PHRASE);
					  if(has_showers)
						theStringVector.push_back((is_summer ? YKSITTAISET_SADEKUUROT_MAHDOLLISIA : YKSITTAISET_VESIKUUROT_MAHDOLLISIA));
					}
				  else
					{
					  in_places_phrase(in_some_places,
									   in_many_places,
									   theUseOllaVerb,
									   theStringVector);
				  
					  if(has_showers)
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitWater)
							theStringVector.push_back((is_summer ? HEIKKOJA_SADEKUUROJA_PHRASE : HEIKKOJA_VESIKUUROJA_PHRASE));
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitWater)
							theStringVector.push_back((is_summer ? VOIMAKKAITA_SADEKUUROJA_PHRASE : VOIMAKKAITA_VESIKUUROJA_PHRASE));
						  else
							theStringVector.push_back(SADEKUUROJA_WORD);

						  theStringVector.push_back(can_be_freezing_phrase(can_be_freezing));
						}
					  else
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitWater)
							theStringVector.push_back((is_summer ? HEIKKOA_SADETTA_PHRASE : HEIKKOA_VESISADETTA_PHRASE));
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitWater)
							theStringVector.push_back((is_summer ? RUNSASTA_SADETTA_PHRASE : RUNSASTA_VESISADETTA_PHRASE));
						  else
							theStringVector.push_back(precipitation_phrase(thePeriod.localStartTime(), theParameters.theVariable));

						  theStringVector.push_back(can_be_freezing_phrase(can_be_freezing));
						}
					}
				}
			  else
				{
				  theStringVector.push_back(ENIMMAKSEEN_POUTAINEN_PHRASE);
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
					  theStringVector.push_back(ENIMMAKSEEN_POUTAINEN_PHRASE);
					  if(has_showers)
						theStringVector.push_back(YKSITTAISET_RANTAKUUROT_MAHDOLLISIA);
					}
				  else
					{
					  in_places_phrase(in_some_places,
									   in_many_places,
									   theUseOllaVerb,
									   theStringVector);

					  if(has_showers)
						{						
						  theStringVector.push_back(RANTAKUUROJA_WORD);
						}
					  else
						{
						  theStringVector.push_back(RANTASADETTA_WORD);
						}
					}
				}
			  else
				{
				  theStringVector.push_back(ENIMMAKSEEN_POUTAINEN_PHRASE);
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
					  theStringVector.push_back(ENIMMAKSEEN_POUTAINEN_PHRASE);
					  if(has_showers)
						theStringVector.push_back(YKSITTAISET_LUMIKUUROT_MAHDOLLISIA);
					}
				  else
					{
					  in_places_phrase(in_some_places,
									   in_many_places,
									   theUseOllaVerb,
									   theStringVector);
					  if(has_showers)
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow)
							theStringVector.push_back(HEIKKOJA_LUMIKUUROJA_PHRASE);
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							theStringVector.push_back(SAKEITA_LUMIKUUROJA_PHRASE);
						  else
							theStringVector.push_back(LUMIKUUROJA_WORD);
						}
					  else
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow)
							theStringVector.push_back(HEIKKOA_LUMISADETTA_PHRASE);
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							theStringVector.push_back(SAKEAA_LUMISADETTA_PHRASE);
						  else
							theStringVector.push_back(LUMISADETTA_WORD);
						}
					}
				}
			  else
				{
				  theStringVector.push_back(ENIMMAKSEEN_POUTAINEN_PHRASE);
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
					  theStringVector.push_back(ENIMMAKSEEN_POUTAINEN_PHRASE);
					  if(has_showers)
						{
						  theStringVector.push_back((is_summer ? YKSITTAISET_SADEKUUROT_MAHDOLLISIA : 
									   YKSITTAISET_VESIKUUROT_MAHDOLLISIA));
						}
					}
				  else
					{
					  in_places_phrase(in_some_places,
									   in_many_places,
									   theUseOllaVerb,
									   theStringVector);

					  if(has_showers)
						{
						  theStringVector.push_back((is_summer ? SADEKUUROJA_WORD : VESIKUUROJA_WORD));
						  theStringVector.push_back(can_be_freezing_phrase(can_be_freezing));
						}
					  else
						{
						  theStringVector.push_back(TIHKUSADETTA_WORD);
						  theStringVector.push_back(can_be_freezing_phrase(can_be_freezing));
						}
					}
				}
			  else
				{
				  theStringVector.push_back(ENIMMAKSEEN_POUTAINEN_PHRASE);
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
					  theStringVector.push_back(ENIMMAKSEEN_POUTAINEN_PHRASE);
					  if(has_showers)
						{
						  theStringVector.push_back((is_summer ? YKSITTAISET_SADEKUUROT_MAHDOLLISIA : 
									   YKSITTAISET_VESIKUUROT_MAHDOLLISIA));
						}
					}
				  else
					{
					  in_places_phrase(in_some_places,
									   in_many_places,
									   theUseOllaVerb,
									   theStringVector);

					  if(has_showers)
						{
						  theStringVector.push_back((is_summer ? SADEKUUROJA_WORD : VESIKUUROJA_WORD));
						  theStringVector.push_back(can_be_freezing_phrase(can_be_freezing));
						}
					  else
						{
						  if(thePrecipitationFormDrizzle < 70)
							{
							  theStringVector.push_back( precipitation_phrase(thePeriod.localStartTime(), theParameters.theVariable));
							}
						  else
							{
							  theStringVector.push_back(TIHKUSADETTA_WORD);
							}
						  theStringVector.push_back(can_be_freezing_phrase(can_be_freezing));
						}
					}
				}
			  else
				{
				  theStringVector.push_back(ENIMMAKSEEN_POUTAINEN_PHRASE);
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
					  theStringVector.push_back(ENIMMAKSEEN_POUTAINEN_PHRASE);
					  if(has_showers)
						{
						  if(thePrecipitationFormWater >= thePrecipitationFormSleet)
							theStringVector.push_back(YKSITTAISET_VESI_RANTA_KUUROT_MAHDOLLISIA);
						  else
							theStringVector.push_back(YKSITTAISET_RANTA_VESI_KUUROT_MAHDOLLISIA);
						}
					}
				  else
					{
					  in_places_phrase(in_some_places,
									   in_many_places,
									   theUseOllaVerb,
									   theStringVector);

					  if(has_showers)
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

						  theStringVector.push_back(can_be_freezing_phrase(can_be_freezing));
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
							}

						  theStringVector.push_back(can_be_freezing_phrase(can_be_freezing));
						}
					}
				}
			  else
				{
				  theStringVector.push_back(ENIMMAKSEEN_POUTAINEN_PHRASE);
				}
			  break;
			}

		  case WATER_SNOW_FORM:
		  case DRIZZLE_SNOW_FORM:
		  case WATER_DRIZZLE_SNOW_FORM:
		  case WATER_SLEET_SNOW_FORM:
		  case DRIZZLE_SNOW_FREEZING_FORM:
		  case WATER_SNOW_FREEZING_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitDrizzle)
				{
				  if(mostly_dry_weather)
					{
					  theStringVector.push_back(ENIMMAKSEEN_POUTAINEN_PHRASE);

					  if(has_showers)
						theStringVector.push_back(YKSITTAISET_VESI_LUMI_KUUROT_MAHDOLLISIA);
					}
				  else
					{
					  in_places_phrase(in_some_places,
									   in_many_places,
									   theUseOllaVerb,
									   theStringVector);

					  if(has_showers)
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow)
							{
							  theStringVector.push_back(VESI_TAVUVIIVA_WORD);
							  theStringVector.push_back(TAI_WORD);
							  theStringVector.push_back(LUMIKUUROJA_WORD);
							}
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							{
							  theStringVector.push_back((is_summer ? VOIMAKKAITA_SADEKUUROJA_PHRASE : VOIMAKKAITA_VESIKUUROJA_PHRASE));
							  theStringVector.push_back(TAI_WORD);
							  theStringVector.push_back(SAKEITA_LUMIKUUROJA_PHRASE);
							}
						  else
							{
							  theStringVector.push_back(VESI_TAVUVIIVA_WORD);
							  theStringVector.push_back(TAI_WORD);
							  theStringVector.push_back(LUMIKUUROJA_WORD);
							}

						  theStringVector.push_back(can_be_freezing_phrase(can_be_freezing));
						}
					  else
						{
						  if(thePrecipitationIntensity <  theParameters.theWeakPrecipitationLimitSnow)
							theStringVector.push_back(HEIKKOA_WORD);
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							theStringVector.push_back(KOVAA_WORD);
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
							}

						  theStringVector.push_back(can_be_freezing_phrase(can_be_freezing));
						}
					}
				}
			  else
				{
				  theStringVector.push_back(ENIMMAKSEEN_POUTAINEN_PHRASE);
				}
			  break;
			}

		  case DRIZZLE_SLEET_SNOW_FORM:
		  case SLEET_SNOW_FREEZING_FORM:
		  case SLEET_SNOW_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitSleet)
				{
				  if(mostly_dry_weather)
					{
					  theStringVector.push_back(ENIMMAKSEEN_POUTAINEN_PHRASE);
					  if(has_showers)
						theStringVector.push_back(YKSITTAISET_LUMI_RANTA_KUUROT_MAHDOLLISIA);
					}
				  else
					{
					  in_places_phrase(in_some_places,
									   in_many_places,
									   theUseOllaVerb,
									   theStringVector);

					  if(has_showers)
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow)
							{
							  theStringVector.push_back(RANTA_TAVUVIIVA_WORD);
							  theStringVector.push_back(TAI_WORD);
							  theStringVector.push_back(LUMIKUUROJA_WORD);
							}
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							{
							  theStringVector.push_back(SAKEITA_LUMIKUUROJA_PHRASE);
							  theStringVector.push_back(TAI_WORD);
							  theStringVector.push_back(RANTASADETTA_WORD);
							}
						  else
							{
							  theStringVector.push_back(RANTA_TAVUVIIVA_WORD);
							  theStringVector.push_back(TAI_WORD);
							  theStringVector.push_back(LUMIKUUROJA_WORD);
							}
						}
					  else
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow)
							{
							  theStringVector.push_back(HEIKKOA_LUMISADETTA_PHRASE);
							  theStringVector.push_back(TAI_WORD);
							  theStringVector.push_back(RANTASADETTA_WORD);
							}
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							{
							  theStringVector.push_back(SAKEAA_LUMISADETTA_PHRASE);
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
								}
							}

						  theStringVector.push_back(can_be_freezing_phrase(can_be_freezing));
						}
					}
				}
			  else
				{
				  theStringVector.push_back(ENIMMAKSEEN_POUTAINEN_PHRASE);
				}
			  break;
			}
		  }
	  }

	return theStringVector.size();
 }

  std::string precipitation_sentence_string(wf_story_params& theParameters,
											const WeatherPeriod& thePeriod,
											const unsigned int& thePrecipitationForm,
											const float thePrecipitationIntensity,
											const float thePrecipitationExtent,
											const float thePrecipitationFormWater,
											const float thePrecipitationFormDrizzle,
											const float thePrecipitationFormSleet,
											const float thePrecipitationFormSnow,
											const float thePrecipitationFormFreezing,
											const float thePrecipitationTypeShowers,
											const bool& theUseOllaVerb)
  {
	std::string retval;

	vector<std::string> stringVector;

	precipitation_sentence_string_vector(theParameters,
										 thePeriod,
										 thePrecipitationForm,
										 thePrecipitationIntensity,
										 thePrecipitationExtent,
										 thePrecipitationFormWater,
										 thePrecipitationFormDrizzle,
										 thePrecipitationFormSleet,
										 thePrecipitationFormSnow,
										 thePrecipitationFormFreezing,
										 thePrecipitationTypeShowers,
										 theUseOllaVerb,
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

  Sentence precipitation_sentence(wf_story_params& theParameters,
								  const WeatherPeriod& thePeriod,
								  const unsigned int& thePrecipitationForm,
								  const float thePrecipitationIntensity,
								  const float thePrecipitationExtent,
								  const float thePrecipitationFormWater,
								  const float thePrecipitationFormDrizzle,
								  const float thePrecipitationFormSleet,
								  const float thePrecipitationFormSnow,
								  const float thePrecipitationFormFreezing,
								  const float thePrecipitationTypeShowers,
								  const bool& theUseOllaVerb)
  {
	Sentence sentence;

	vector<std::string> stringVector;

	precipitation_sentence_string_vector(theParameters,
										 thePeriod,
										 thePrecipitationForm,
										 thePrecipitationIntensity,
										 thePrecipitationExtent,
										 thePrecipitationFormWater,
										 thePrecipitationFormDrizzle,
										 thePrecipitationFormSleet,
										 thePrecipitationFormSnow,
										 thePrecipitationFormFreezing,
										 thePrecipitationTypeShowers,
										 theUseOllaVerb,
										 stringVector);

	for(unsigned int i = 0; i < stringVector.size(); i++)
	  {
		sentence << stringVector.at(i);
	  }

	return sentence;
  }

#ifdef OLD_IMPL
  Sentence precipitation_sentence(wf_story_params& theParameters,
								  const WeatherPeriod& thePeriod,
								  const unsigned int& thePrecipitationForm,
								  const float thePrecipitationIntensity,
								  const float thePrecipitationExtent,
								  const float thePrecipitationFormWater,
								  const float thePrecipitationFormDrizzle,
								  const float thePrecipitationFormSleet,
								  const float thePrecipitationFormSnow,
								  const float thePrecipitationFormFreezing,
								  const float thePrecipitationTypeShowers,
								  const bool& theUseOllaVerb)
  {
	Sentence sentence;

	bool dry_weather = is_dry_weather(theParameters, thePrecipitationForm, thePrecipitationIntensity, thePrecipitationExtent);

	if(dry_weather)
	  {
		sentence << POUTAINEN_WORD;
	  }
	else
	  {
		const bool has_showers = (thePrecipitationTypeShowers != kFloatMissing && 
								  thePrecipitationTypeShowers >= theParameters.theShowerLimit);
		const bool mostly_dry_weather = thePrecipitationExtent <= theParameters.theMostlyDryWeatherLimit;
		const bool in_some_places = thePrecipitationExtent > theParameters.theInSomePlacesLowerLimit && 
		  thePrecipitationExtent <= theParameters.theInSomePlacesUpperLimit;
		const bool in_many_places = thePrecipitationExtent > theParameters.theInManyPlacesLowerLimit && 
		  thePrecipitationExtent <= theParameters.theInManyPlacesUpperLimit;
		  		
		bool can_be_freezing =  thePrecipitationFormFreezing > theParameters.theFreezingPrecipitationLimit;
		bool is_summer = SeasonTools::isSummer(thePeriod.localStartTime(), theParameters.theVariable);

		if(has_showers)
		  ; // report the cloudiness?
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
					  sentence << ENIMMAKSEEN_POUTAINEN_PHRASE;
					  if(has_showers)
						sentence << (is_summer ? YKSITTAISET_SADEKUUROT_MAHDOLLISIA : YKSITTAISET_VESIKUUROT_MAHDOLLISIA);
					}
				  else
					{
					  sentence << in_places_phrase(in_some_places,
												   in_many_places,
												   theUseOllaVerb);
					  if(has_showers)
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitWater)
							sentence << (is_summer ? HEIKKOJA_SADEKUUROJA_PHRASE : HEIKKOJA_VESIKUUROJA_PHRASE);
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitWater)
							sentence << (is_summer ? VOIMAKKAITA_SADEKUUROJA_PHRASE : VOIMAKKAITA_VESIKUUROJA_PHRASE);
						  else
							sentence << SADEKUUROJA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE;
						}
					  else
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitWater)
							sentence << (is_summer ? HEIKKOA_SADETTA_PHRASE : HEIKKOA_VESISADETTA_PHRASE);
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitWater)
							sentence << (is_summer ? RUNSASTA_SADETTA_PHRASE : RUNSASTA_VESISADETTA_PHRASE);
						  else
							sentence << precipitation_phrase(thePeriod.localStartTime(), theParameters.theVariable);

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;
						}
					}
				}
			  else
				{
				  sentence << ENIMMAKSEEN_POUTAINEN_PHRASE;
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
					  sentence << ENIMMAKSEEN_POUTAINEN_PHRASE;
					  if(has_showers)
						sentence << YKSITTAISET_RANTAKUUROT_MAHDOLLISIA;
					}
				  else
					{
					  sentence << in_places_phrase(in_some_places,
												   in_many_places,
												   theUseOllaVerb);

					  if(has_showers)
						{						
						  sentence << RANTAKUUROJA_WORD;
						}
					  else
						{
						  sentence << RANTASADETTA_WORD;
						}
					}
				}
			  else
				{
				  sentence << ENIMMAKSEEN_POUTAINEN_PHRASE;
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
					  sentence << ENIMMAKSEEN_POUTAINEN_PHRASE;
					  if(has_showers)
						sentence << YKSITTAISET_LUMIKUUROT_MAHDOLLISIA;
					}
				  else
					{
					  sentence << in_places_phrase(in_some_places,
												   in_many_places,
												   theUseOllaVerb);
					  if(has_showers)
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow)
							sentence << HEIKKOJA_LUMIKUUROJA_PHRASE;
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							sentence << SAKEITA_LUMIKUUROJA_PHRASE;
						  else
							sentence << LUMIKUUROJA_WORD;
						}
					  else
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow)
							sentence << HEIKKOA_LUMISADETTA_PHRASE;
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							sentence << SAKEAA_LUMISADETTA_PHRASE;
						  else
							sentence << LUMISADETTA_WORD;
						}
					}
				}
			  else
				{
				  sentence << ENIMMAKSEEN_POUTAINEN_PHRASE;
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
					  sentence << ENIMMAKSEEN_POUTAINEN_PHRASE;
					  if(has_showers)
						{
						  sentence << (is_summer ? YKSITTAISET_SADEKUUROT_MAHDOLLISIA : 
									   YKSITTAISET_VESIKUUROT_MAHDOLLISIA);
						}
					}
				  else
					{
					  sentence << in_places_phrase(in_some_places,
												   in_many_places,
												   theUseOllaVerb);

					  if(has_showers)
						{
						  sentence << (is_summer ? SADEKUUROJA_WORD : VESIKUUROJA_WORD);

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE;
						}
					  else
						{
						  sentence << TIHKUSADETTA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;
						}
					}
				}
			  else
				{
				  sentence << ENIMMAKSEEN_POUTAINEN_PHRASE;
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
					  sentence << ENIMMAKSEEN_POUTAINEN_PHRASE;
					  if(has_showers)
						{
						  sentence << (is_summer ? YKSITTAISET_SADEKUUROT_MAHDOLLISIA : 
									   YKSITTAISET_VESIKUUROT_MAHDOLLISIA);
						}
					}
				  else
					{
					  sentence << in_places_phrase(in_some_places,
												   in_many_places,
												   theUseOllaVerb);

					  if(has_showers)
						{
						  sentence << (is_summer ? SADEKUUROJA_WORD : VESIKUUROJA_WORD);

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE;
						}
					  else
						{
						  if(thePrecipitationFormDrizzle < 70)
							{
							  sentence <<  precipitation_phrase(thePeriod.localStartTime(), theParameters.theVariable);
							}
						  else
							{
							  sentence << TIHKUSADETTA_WORD;
							}

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;
						}
					}
				}
			  else
				{
				  sentence << ENIMMAKSEEN_POUTAINEN_PHRASE;
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
					  sentence << ENIMMAKSEEN_POUTAINEN_PHRASE;
					  if(has_showers)
						{
						  if(thePrecipitationFormWater >= thePrecipitationFormSleet)
							sentence << YKSITTAISET_VESI_RANTA_KUUROT_MAHDOLLISIA;
						  else
							sentence << YKSITTAISET_RANTA_VESI_KUUROT_MAHDOLLISIA;
						}
					}
				  else
					{
					  sentence << in_places_phrase(in_some_places,
												   in_many_places,
												   theUseOllaVerb);

					  if(has_showers)
						{
						  if(thePrecipitationFormWater >= thePrecipitationFormSleet)
							sentence << VESI_TAVUVIIVA_WORD << TAI_WORD << RANTAKUUROJA_WORD;
						  else
							sentence << RANTA_TAVUVIIVA_WORD << TAI_WORD << VESIKUUROJA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE;
						}
					  else
						{
						  if(thePrecipitationFormWater >= thePrecipitationFormSleet)
							sentence << VESI_TAVUVIIVA_WORD << TAI_WORD << RANTASADETTA_WORD;
						  else
							sentence << RANTA_TAVUVIIVA_WORD << TAI_WORD << VESISADETTA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;
						}
					}
				}
			  else
				{
				  sentence << ENIMMAKSEEN_POUTAINEN_PHRASE;
				}
			  break;
			}

		  case WATER_SNOW_FORM:
		  case DRIZZLE_SNOW_FORM:
		  case WATER_DRIZZLE_SNOW_FORM:
		  case WATER_SLEET_SNOW_FORM:
		  case DRIZZLE_SNOW_FREEZING_FORM:
		  case WATER_SNOW_FREEZING_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitDrizzle)
				{
				  if(mostly_dry_weather)
					{
					  sentence << ENIMMAKSEEN_POUTAINEN_PHRASE;

					  if(has_showers)
						sentence << YKSITTAISET_VESI_LUMI_KUUROT_MAHDOLLISIA;
					}
				  else
					{
					  sentence << in_places_phrase(in_some_places,
												   in_many_places,
												   theUseOllaVerb);

					  if(has_showers)
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow)
							{
							  sentence << VESI_TAVUVIIVA_WORD << TAI_WORD << LUMIKUUROJA_WORD;
							}
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							{
							  sentence << (is_summer ? VOIMAKKAITA_SADEKUUROJA_PHRASE : VOIMAKKAITA_VESIKUUROJA_PHRASE);
							  sentence << TAI_WORD << SAKEITA_LUMIKUUROJA_PHRASE;
							}
						  else
							{
							  sentence << VESI_TAVUVIIVA_WORD << TAI_WORD << LUMIKUUROJA_WORD;
							}

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE;
						}
					  else
						{
						  if(thePrecipitationIntensity <  theParameters.theWeakPrecipitationLimitSnow)
							sentence << HEIKKOA_WORD;
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							sentence << KOVAA_WORD;
						  if(thePrecipitationFormWater >= thePrecipitationFormSnow)
							sentence << VESI_TAVUVIIVA_WORD << TAI_WORD << LUMISADETTA_WORD;
						  else
							sentence << LUMI_TAVUVIIVA_WORD << TAI_WORD << VESISADETTA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;
						}
					}
				}
			  else
				{
				  sentence << ENIMMAKSEEN_POUTAINEN_PHRASE;
				}
			  break;
			}

		  case DRIZZLE_SLEET_SNOW_FORM:
		  case SLEET_SNOW_FREEZING_FORM:
		  case SLEET_SNOW_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitSleet)
				{
				  if(mostly_dry_weather)
					{
					  sentence << ENIMMAKSEEN_POUTAINEN_PHRASE;
					  if(has_showers)
						sentence << YKSITTAISET_LUMI_RANTA_KUUROT_MAHDOLLISIA;
					}
				  else
					{
					  sentence << in_places_phrase(in_some_places,
												   in_many_places,
												   theUseOllaVerb);

					  if(has_showers)
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow)
							sentence << RANTA_TAVUVIIVA_WORD << TAI_WORD << LUMIKUUROJA_WORD;
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							sentence << SAKEITA_LUMIKUUROJA_PHRASE << TAI_WORD << RANTASADETTA_WORD;
						  else
							sentence << RANTA_TAVUVIIVA_WORD << TAI_WORD << LUMIKUUROJA_WORD;
						}
					  else
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow)
							sentence << HEIKKOA_LUMISADETTA_PHRASE << TAI_WORD << RANTASADETTA_WORD;
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							sentence << SAKEAA_LUMISADETTA_PHRASE << TAI_WORD << RANTASADETTA_WORD;
						  else
							{
							  if(thePrecipitationFormSnow >= thePrecipitationFormSleet)
								sentence << LUMI_TAVUVIIVA_WORD << TAI_WORD << RANTASADETTA_WORD;
							  else
								sentence << RANTA_TAVUVIIVA_WORD << TAI_WORD << VESISADETTA_WORD;
							}

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;
						}
					}
				}
			  else
				{
				  sentence << ENIMMAKSEEN_POUTAINEN_PHRASE;
				}
			  break;
			}
		  }
	  }

	return sentence;
  }
#endif

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

  // depends on season: in summertime sadetta; in wintertime vesisadetta
  const char* precipitation_phrase(const NFmiTime& theTime, const string& theVariable)
  {
	return (SeasonTools::isWinterHalf(theTime, theVariable) ? VESISADETTA_WORD : SADETTA_WORD);
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
	const precipitation_data_vector* dataVector = 0;

	if(theForecastArea & FULL_AREA)
	  dataVector = &theFullData;
	else if(theForecastArea & INLAND_AREA)
	  dataVector = &theInlandData;
	else if(theForecastArea & COASTAL_AREA)
	  dataVector = &theCoastalData;
	  
	if(dataVector)
	  {
		theIntensity =   getMean(*dataVector,
								 PRECIPITATION_DATA,
								 theWeatherPeriod);

		theForm = MISSING_PRECIPITATION_FORM;

		for(unsigned int i = 0; i < dataVector->size(); i++)
		  if(dataVector->at(i)->theObservationTime >= theWeatherPeriod.localStartTime() &&
			 dataVector->at(i)->theObservationTime <= theWeatherPeriod.localEndTime() &&
			 dataVector->at(i)->thePrecipitationForm != MISSING_PRECIPITATION_FORM)
			{
			  if(theForm == MISSING_PRECIPITATION_FORM)
				theForm = dataVector->at(i)->thePrecipitationForm;
			  else if(dataVector->at(i)->thePrecipitationForm < theForm)
				theForm = dataVector->at(i)->thePrecipitationForm;				
			}

		theExtent =   getMean(*dataVector,
							  PRECIPITATION_EXTENT_DATA,
							  theWeatherPeriod);
		return true;
	  }
	return false;
  }
  
  bool PrecipitationForecast::isMostlyDryPeriod(const WeatherPeriod& theWeatherPeriod,
												const unsigned short theForecastArea) const
  {
	float precipitationIntensity(0.0);
	unsigned int precipitationForm = MISSING_PRECIPITATION_FORM;
	float precipitationExtent(0.0);

	if(getIntensityFormExtent(theWeatherPeriod,
						   theForecastArea,
						   precipitationIntensity,
						   precipitationForm,
							  precipitationExtent))
	  return(precipitationExtent <= theParameters.theMostlyDryWeatherLimit);
	else
	  return true;
  }

  bool PrecipitationForecast::isDryPeriod(const WeatherPeriod& theWeatherPeriod,
										  const unsigned short theForecastArea) const
  {
	float precipitationIntensity(0.0);
	unsigned int precipitationForm = MISSING_PRECIPITATION_FORM;
	float precipitationExtent(0.0);

	if(getIntensityFormExtent(theWeatherPeriod,
						   theForecastArea,
						   precipitationIntensity,
						   precipitationForm,
							  precipitationExtent))
	  {

	/*
	const precipitation_data_vector* dataVector = 0;

	if(theForecastArea & FULL_AREA)
	  dataVector = &theFullData;
	else if(theForecastArea & INLAND_AREA)
	  dataVector = &theInlandData;
	else if(theForecastArea & COASTAL_AREA)
	  dataVector = &theCoastalData;
	  
	if(dataVector)
	  {
		float precipitationIntensity =   getMean(*dataVector,
												 PRECIPITATION_DATA,
												 theWeatherPeriod);
		unsigned int precipitationForm = MISSING_PRECIPITATION_FORM;

		for(unsigned int i = 0; i < dataVector->size(); i++)
		  if(dataVector->at(i)->theObservationTime >= theWeatherPeriod.localStartTime() &&
			 dataVector->at(i)->theObservationTime <= theWeatherPeriod.localEndTime() &&
			 dataVector->at(i)->thePrecipitationForm != MISSING_PRECIPITATION_FORM)
			{
			  if(precipitationForm == MISSING_PRECIPITATION_FORM)
				precipitationForm = dataVector->at(i)->thePrecipitationForm;
			  else if(dataVector->at(i)->thePrecipitationForm < precipitationForm)
				precipitationForm = dataVector->at(i)->thePrecipitationForm;				
			}

		float precipitationExtent =   getMean(*dataVector,
											  PRECIPITATION_EXTENT_DATA,
											  theWeatherPeriod);
	*/
		return is_dry_weather(theParameters,
							  precipitationForm,
							  precipitationIntensity,
							  precipitationExtent);

	  }

	return true;
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
		if(theData[i]->theObservationTime < theWeatherPeriod.localStartTime() ||
		   theData[i]->theObservationTime > theWeatherPeriod.localEndTime())
		  /* ||
		   is_dry_weather(theParameters,
						  theData[i]->thePrecipitationForm,
						  theData[i]->thePrecipitationIntensity,
						  theData[i]->thePrecipitationExtent))*/
		  continue;

		switch(theDataId)
		  {
		  case PRECIPITATION_DATA:
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


  unsigned int PrecipitationForecast::getPrecipitationForm(const precipitation_data_vector& theData,
														   const WeatherPeriod& theWeatherPeriod) const
  {
	unsigned int retval = MISSING_PRECIPITATION_FORM;

	for(unsigned int i = 0; i < theData.size(); i++)
	  {
		if(theData[i]->theObservationTime < theWeatherPeriod.localStartTime() ||
		   theData[i]->theObservationTime > theWeatherPeriod.localEndTime())
		  continue;

		retval |= theData[i]->thePrecipitationForm;
	  }

	return retval;
  }

  unsigned int PrecipitationForecast::getPrecipitationGategory(const float& thePrecipitation, 
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

	float coastalPrecipitation = getMean(theCoastalData, PRECIPITATION_DATA, theWeatherPeriod);
	float inlandPrecipitation = getMean(theInlandData, PRECIPITATION_DATA, theWeatherPeriod);
	unsigned int coastalPrecipitationForm = getPrecipitationForm(theCoastalData, theWeatherPeriod);
	unsigned int inlandPrecipitationForm = getPrecipitationForm(theInlandData, theWeatherPeriod);
	/*
	float coastalExtent = getMean(theCoastalData, PRECIPITATION_EXTENT_DATA, theWeatherPeriod);
	float inlandExtent = getMean(theInlandData, PRECIPITATION_EXTENT_DATA, theWeatherPeriod);
	float coastalShowersPercentage = getMean(theCoastalData, PRECIPITATION_TYPE_DATA, theWeatherPeriod);
	float inlandShowersPercentage = getMean(theInlandData, PRECIPITATION_TYPE_DATA, theWeatherPeriod);
	*/
	unsigned int precipitationCategoryCoastal = getPrecipitationGategory(coastalPrecipitation, coastalPrecipitationForm);
	unsigned int precipitationCategoryInland = getPrecipitationGategory(inlandPrecipitation, inlandPrecipitationForm);

	if(((coastalPrecipitation < 0.04 && inlandPrecipitation > 0.4) ||
		(coastalPrecipitation > 0.4 && inlandPrecipitation < 0.04)) ||
	   ((inlandPrecipitation > 0.04 || coastalPrecipitation > 0.04) &&
		precipitationCategoryCoastal != precipitationCategoryInland))
	  return true;
	  	  /*
	   (coastalExtent < 20.0 && inlandExtent > 80.0) ||
	   (coastalExtent > 80.0 && inlandExtent < 30.0) ||
	   (coastalShowersPercentage < 20.0 && inlandShowersPercentage > 80.0) ||
	   (coastalShowersPercentage > 80.0 && inlandShowersPercentage < 20.0) ||
	   precipitationCategoryCoastal != precipitationCategoryInland)*/

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


	weather_result_data_item_vector* precipitationHourly = 
	  (*theParameters.theCompleteData[theAreaId])[PRECIPITATION_DATA];
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

	precipitation_data_vector& dataVector = (theAreaId == COASTAL_AREA ? theCoastalData : 
											 (theAreaId == INLAND_AREA ? theInlandData : theFullData));

	for(unsigned int i = 0; i < precipitationHourly->size(); i++)
	  {
		float precipitationIntesity = (*precipitationHourly)[i]->theResult.value();
		float precipitationExtent = (*precipitationExtentHourly)[i]->theResult.value();
		float precipitationFormWater = (*precipitationFormWaterHourly)[i]->theResult.value();
		float precipitationFormDrizzle = (*precipitationFormDrizzleHourly)[i]->theResult.value();
		float precipitationFormSleet = (*precipitationFormSleetHourly)[i]->theResult.value();
		float precipitationFormSnow = (*precipitationFormSnowHourly)[i]->theResult.value();
		float precipitationFormFreezing = (*precipitationFormFreezingHourly)[i]->theResult.value();
		float precipitationTypeShowers = (*precipitationTypeHourly)[i]->theResult.value();
		// float precipitationPercentageNorthEast = 
		  

		const unsigned int precipitationForm = 
		  get_complete_precipitation_form(theParameters.theVariable,
										  precipitationFormWater,
										  precipitationFormDrizzle,
										  precipitationFormSleet,
										  precipitationFormSnow,
										  precipitationFormFreezing);
		  
		PrecipitationDataItemData* item = 
		  new PrecipitationDataItemData(precipitationForm,
										precipitationIntesity,
										precipitationExtent,
										precipitationFormWater,
										precipitationFormDrizzle,
										precipitationFormSleet,
										precipitationFormSnow,
										precipitationFormFreezing,
										precipitationTypeShowers,
										NO_TREND,
										0.0,
										(*precipitationHourly)[i]->thePeriod.localStartTime());
			
		item->thePrecipitationPercentageNorthEast = (*precipitationNorthEastShareHourly)[i]->theResult.value();
		item->thePrecipitationPercentageSouthEast = (*precipitationSouthEastShareHourly)[i]->theResult.value();
		item->thePrecipitationPercentageSouthWest = (*precipitationSouthWestShareHourly)[i]->theResult.value();
		item->thePrecipitationPercentageNorthWest = (*precipitationNorthWestShareHourly)[i]->theResult.value();

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
			theParameters.theLog << "Joining precipitation periods: " 
								 << thePrecipitationPeriodVector.at(i-1).localStartTime() << "..."
								 << thePrecipitationPeriodVector.at(i-1).localEndTime()
								 << " and "
								 << thePrecipitationPeriodVector.at(i).localStartTime() << "..."
								 << thePrecipitationPeriodVector.at(i).localEndTime();

			WeatherPeriod joinedPeriod(thePrecipitationPeriodVector.at(i-1).localStartTime(),
									   thePrecipitationPeriodVector.at(i).localEndTime());
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
		dataDestinationVector->push_back(WeatherPeriod(startTime, endTime));
	  }
	joinPrecipitationPeriods(*dataDestinationVector);
  }

  void PrecipitationForecast::findOutPrecipitationTrends()
  {
	if(theParameters.theForecastArea & COASTAL_AREA)
	  findOutPrecipitationTrends(thePrecipitationPeriodsCoastal, thePrecipitationTrendsCoastal);
	if(theParameters.theForecastArea & INLAND_AREA)
	  findOutPrecipitationTrends(thePrecipitationPeriodsInland, thePrecipitationTrendsInland);
	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  findOutPrecipitationTrends(thePrecipitationPeriodsFull, thePrecipitationTrendsFull);
  }

  void PrecipitationForecast::findOutPrecipitationTrends(const vector<WeatherPeriod>& thePrecipitationPeriods,
														 trend_id_vector& thePrecipitationTrends)
  {
	for(unsigned int i = 0; i < thePrecipitationPeriods.size(); i++)
	  {
		if(thePrecipitationPeriods.at(i).localEndTime().DifferenceInHours(thePrecipitationPeriods.at(i).localStartTime()) >= 6)
		  {
			NFmiTime precipitationStartTime(thePrecipitationPeriods.at(i).localStartTime());
			NFmiTime precipitationEndTime(thePrecipitationPeriods.at(i).localEndTime());
			
			// start year 1970 indicates that rain starts on previous period
			if(precipitationStartTime.GetYear() != 1970 && 
			   precipitationStartTime != theParameters.theForecastPeriod.localEndTime())
			  {
				thePrecipitationTrends.push_back(make_pair(precipitationStartTime, SADE_ALKAA));
			  }
			
			if(precipitationEndTime.GetYear() != 2037 &&
			   precipitationEndTime != theParameters.theForecastPeriod.localEndTime())
			  {
				thePrecipitationTrends.push_back(make_pair(precipitationEndTime, POUTAANTUU));
			  }

			/*
			if(precipitationEndTime.DifferenceInHours(theParameters.theForecastPeriod.localEndTime()) == 0)
			  {			
				NFmiTime beforeStartStart(precipitationEndTime);
				NFmiTime beforeStartEnd(precipitationEndTime);
				NFmiTime afterStartStart(precipitationStartTime);
				NFmiTime afterStartEnd(precipitationStartTime);
				beforeStartStart.ChangeByHours(-3);
				beforeStartEnd.ChangeByHours(-1);
				afterStartEnd.ChangeByHours(3);

				if(theParameters.theForecastPeriod.localEndTime().DifferenceInHours(afterStartEnd) > 2 &&
				   beforeStartStart.DifferenceInHours(theParameters.theForecastPeriod.localStartTime()) > 2)
				  {
					WeatherPeriod beforePeriod(beforeStartStart, beforeStartEnd);
					WeatherPeriod afterPeriod(afterStartStart, afterStartEnd);
					
					if(!isDryPeriod(beforePeriod, theParameters.theForecastArea) &&
					   isDryPeriod(afterPeriod, theParameters.theForecastArea))
					  thePrecipitationTrends.push_back(make_pair(precipitationEndTime, POUTAANTUU));
				  }
			  }
			else
			  {
				// end year 2037 indicates that precipitation ends after forecast period
				if(precipitationEndTime.GetYear() != 2037)
				  thePrecipitationTrends.push_back(make_pair(precipitationEndTime, POUTAANTUU));
			  }
			*/
		  }
	  }
  }

	
  void PrecipitationForecast::removeRedundantTrends(trend_id_vector& thePrecipitationTrends, const vector<int>& theRemoveIndexes)
  {
	if(theRemoveIndexes.size() > 0)
	  {
		for(unsigned int i = theRemoveIndexes.size(); i > 0; i--)
		  {
			thePrecipitationTrends.erase(thePrecipitationTrends.begin()+theRemoveIndexes[i-1]);
		  }
	  }
  }

  void PrecipitationForecast::removeDuplicatePrecipitationTrends(trend_id_vector& thePrecipitationTrends)
  {
	vector<int> removeIndexes;

	for(unsigned int i = 1; i < thePrecipitationTrends.size(); i++)
	  {
		trend_id previousPeriodTrendId(thePrecipitationTrends[i-1].second);
		trend_id currentPeriodTrendId(thePrecipitationTrends[i].second);
		if( previousPeriodTrendId == currentPeriodTrendId)
		  removeIndexes.push_back(i);
	  }
	removeRedundantTrends(thePrecipitationTrends, removeIndexes);
  }

  // take into account the continuous precipitation periods
  void PrecipitationForecast::cleanUpPrecipitationTrends(trend_id_vector& thePrecipitationTrends)
  {
	vector<int> removeIndexes;

	for(unsigned int i = 1; i < thePrecipitationTrends.size(); i++)
	  {
		NFmiTime previousPeriodEndTime(thePrecipitationTrends[i-1].first);
		NFmiTime currentPeriodStartTime(thePrecipitationTrends[i].first);
		trend_id previousPeriodTrendId(thePrecipitationTrends[i-1].second);
		trend_id currentPeriodTrendId(thePrecipitationTrends[i].second);
		if(abs(previousPeriodEndTime.DifferenceInHours(currentPeriodStartTime)) <= 2 &&
		   previousPeriodTrendId == SADE_ALKAA && currentPeriodTrendId == POUTAANTUU)
		  removeIndexes.push_back(i-1);
	  }
	removeRedundantTrends(thePrecipitationTrends, removeIndexes);
	removeDuplicatePrecipitationTrends(thePrecipitationTrends);
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

  void PrecipitationForecast::printOutPrecipitationTrends(std::ostream& theOutput) const
  {
	theOutput << "** PRECIPITATION TRENDS **" << endl; 
	bool isTrends = false;
	if(thePrecipitationTrendsCoastal.size() > 0)
	  {
		theOutput << "Coastal precipitation trends: " << endl; 
		print_out_trend_vector(theOutput, thePrecipitationTrendsCoastal);
		isTrends = true;
	  }
	if(thePrecipitationTrendsInland.size() > 0)
	  {
		theOutput << "Inland precipitation trends: " << endl; 
		print_out_trend_vector(theOutput, thePrecipitationTrendsInland);
		isTrends = true;
	  }
	if(thePrecipitationTrendsFull.size() > 0)
	  {
		theOutput << "Full area precipitation trends: " << endl; 
		print_out_trend_vector(theOutput, thePrecipitationTrendsFull);
		isTrends = true;
	  }

	if(!isTrends)
	  theOutput << "No trends!" << endl; 
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
				  << "min=" << setw(8) << setfill(' ') << setprecision(4)
				  << getMin(theDataVector, PRECIPITATION_DATA, period)
				  << " mean=" << setw(8) << setfill(' ') << setprecision(4)
				  << getMean(theDataVector, PRECIPITATION_DATA, period)
				  << " max=" << setw(8) << setfill(' ') << setprecision(4)
				  << getMax(theDataVector, PRECIPITATION_DATA, period)
				  << endl;
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

  Sentence PrecipitationForecast::precipitationChangeSentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

	const trend_id_vector* thePrecipitationTrendsVector = 
	  (theParameters.theForecastArea & FULL_AREA ? &thePrecipitationTrendsFull :
	   (theParameters.theForecastArea & COASTAL_AREA ? &thePrecipitationTrendsCoastal :
		(theParameters.theForecastArea & INLAND_AREA ? &thePrecipitationTrendsInland : 0)));

	int periodLength = theParameters.theForecastPeriod.localEndTime().DifferenceInHours(theParameters.theForecastPeriod.localStartTime());

	if(thePrecipitationTrendsVector)
	  {
		for(unsigned int i = 0; i < thePrecipitationTrendsVector->size(); i++)
		  {
			NFmiTime precipitationTrendTimestamp(thePrecipitationTrendsVector->at(i).first);
			
			if(!(precipitationTrendTimestamp >= thePeriod.localStartTime() &&
				 precipitationTrendTimestamp <= thePeriod.localEndTime()))
			  {
				continue;
			  }

			trend_id trid(thePrecipitationTrendsVector->at(i).second);
			if(trid == POUTAANTUU)
			  {
				if(sentence.size() > 0)
				  sentence << Delimiter(",");
				
				if(periodLength > 24)
				  {
					
					sentence << get_today_phrase(precipitationTrendTimestamp,
												 theParameters.theVariable,
												 theParameters.theArea,
												 thePeriod,
												 theParameters.theForecastTime);
				  }

				sentence << get_time_phrase(precipitationTrendTimestamp);
				//				sentence << POUTAA_WORD;
				sentence << SAA_POUTAANTUU_PHRASE;
			  }
			else
			  {
				NFmiTime startTime;
				NFmiTime endTime;

				if(getPrecipitationPeriod(precipitationTrendTimestamp, startTime, endTime))
				  {
					if(sentence.size() > 0)
					  sentence << Delimiter(",");
					
					if(periodLength > 24)
					  {
						sentence << get_today_phrase(precipitationTrendTimestamp,
													 theParameters.theVariable,
													 theParameters.theArea,
													 thePeriod,
													 theParameters.theForecastTime);
					  }

					sentence << get_time_phrase(precipitationTrendTimestamp, true);

					sentence << precipitationSentence(WeatherPeriod(startTime, endTime), false);
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
	// if both coastal and inland exists we use not areal distribution
	if(!(theParameters.theForecastArea & FULL_AREA))
	  {
		const precipitation_data_vector* precipitationDataVector =
		  ((theParameters.theForecastArea & INLAND_AREA) ? &theInlandData : &theCoastalData);

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
  }

  Sentence  PrecipitationForecast::constructPrecipitationSentence(const WeatherPeriod& thePeriod,
																  const bool& theCheckPrecipitationChange, 
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
													   theCheckPrecipitationChange,
													   INLAND_AREA);
			sentence << COAST_PHRASE;
			sentence << constructPrecipitationSentence(thePeriod,
													   theCheckPrecipitationChange,
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

		float precipitationIntensity = getMean(*dataVector, PRECIPITATION_DATA, thePeriod);
		float precipitationExtent = getMean(*dataVector, PRECIPITATION_EXTENT_DATA, thePeriod);
		float precipitationFormWater = getMean(*dataVector, PRECIPITATION_FORM_WATER_DATA, thePeriod);
		float precipitationFormDrizzle = getMean(*dataVector, PRECIPITATION_FORM_DRIZZLE_DATA, thePeriod);
		float precipitationFormSleet = getMean(*dataVector, PRECIPITATION_FORM_SLEET_DATA, thePeriod);
		float precipitationFormSnow = getMean(*dataVector, PRECIPITATION_FORM_SNOW_DATA, thePeriod);
		float precipitationFormFreezing = getMean(*dataVector, PRECIPITATION_FORM_FREEZING_DATA, thePeriod);
		float precipitationTypeShowers = getMean(*dataVector, PRECIPITATION_TYPE_DATA, thePeriod);
		unsigned int precipitationForm = get_complete_precipitation_form(theParameters.theVariable,
																		 precipitationFormWater,
																		 precipitationFormDrizzle,
																		 precipitationFormSleet,
																		 precipitationFormSnow,
																		 precipitationFormFreezing);
		
		sentence << precipitation_sentence(theParameters,
										   thePeriod,
										   precipitationForm,
										   precipitationIntensity,
										   precipitationExtent,
										   precipitationFormWater,
										   precipitationFormDrizzle,
										   precipitationFormSleet,
										   precipitationFormSnow,
										   precipitationFormFreezing,
										   precipitationTypeShowers,
										   theUseOllaVerb);


		bool dry_weather = is_dry_weather(theParameters, 
										  precipitationForm,
										  precipitationIntensity, 
										  precipitationExtent);

		bool mostly_dry_weather = precipitationExtent <= theParameters.theMostlyDryWeatherLimit;

		if(!dry_weather && !mostly_dry_weather)
		  {
			sentence << areaSpecificSentence(thePeriod);
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
		for(unsigned int i = 0; i < precipitationPeriodVector->size(); i++)
		  {
			NFmiTime startTime(precipitationPeriodVector->at(i).localStartTime());
			NFmiTime endTime(precipitationPeriodVector->at(i).localEndTime());

			//			int differenceInHours(endTime.DifferenceInHours(startTime));
			if(endTime.DifferenceInHours(startTime) < 6 && 
			   is_inside(startTime, thePeriod) &&
			   is_inside(endTime, thePeriod))
			  {
				//
				if(sentence.size() > 0)
				  sentence << Delimiter(",");
				
				if(get_part_of_the_day_id(startTime) == get_part_of_the_day_id(endTime))
				  {
					sentence << get_time_phrase(startTime);
				  }
				else
				  sentence << get_time_phrase_large(precipitationPeriodVector->at(i));

				sentence << constructPrecipitationSentence(WeatherPeriod(startTime, endTime), 
														   false, 
														   theParameters.theForecastArea);
			  }
		  }
	  }

	return sentence;
  }

  std::string PrecipitationForecast::precipitationSentenceString(const WeatherPeriod& thePeriod, 
																 const bool& theCheckPrecipitationChange /*= true*/) const
  {
	/*
	return precipitation_sentence_string(wf_story_params& theParameters,
										 const WeatherPeriod& thePeriod,
										 const unsigned int& thePrecipitationForm,
										 const float thePrecipitationIntensity,
										 const float thePrecipitationExtent,
										 const float thePrecipitationFormWater,
										 const float thePrecipitationFormDrizzle,
										 const float thePrecipitationFormSleet,
										 const float thePrecipitationFormSnow,
										 const float thePrecipitationFormFreezing,
										 const float thePrecipitationTypeShowers,
										 const bool& theUseOllaVerb)
	*/
	return "";
 }

  Sentence PrecipitationForecast::precipitationSentence(const WeatherPeriod& thePeriod,
														const bool& theCheckPrecipitationChange /*= true*/) const
  {
	if(theCheckPrecipitationChange)
	  {
		Sentence precipitationChange;
		precipitationChange << precipitationChangeSentence(thePeriod);
		if(precipitationChange.size() > 0)
		  return precipitationChange;
	  }

	Sentence sentence;

	sentence << shortTermPrecipitationSentence(thePeriod);

	/*
	if(sentence.size() > 0 && isDryPeriod(thePeriod, theParameters.theForecastArea))
	  return sentence;
	
	if(sentence.size() > 0)
	  {
		sentence << Delimiter(",");
		sentence << JONKA_JALKEEN_PHRASE << SAA_POUTAANTUU_PHRASE;
		sentence << Delimiter(",");
	  }
	*/

	if(sentence.size() > 0)
	  return sentence;
	else
	  sentence <<  constructPrecipitationSentence(thePeriod,
												  theCheckPrecipitationChange, 
												  theParameters.theForecastArea);	
	
	return sentence;
  }

  Sentence PrecipitationForecast::areaSpecificSentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

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

	sentence << area_specific_sentence(north,
									   south,
									   east,
									   west,
									   northEast,
									   southEast,
									   southWest,
									   northWest);

	return sentence;
  }

  void PrecipitationForecast::getTrendIdVector(trend_id_vector& thePrecipitationTrends) const
  {
	const trend_id_vector* vectorToClone = 0;

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  vectorToClone = &thePrecipitationTrendsFull;
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  vectorToClone = &thePrecipitationTrendsCoastal;
	else if(theParameters.theForecastArea & INLAND_AREA)
	  vectorToClone = &thePrecipitationTrendsInland;

	if(vectorToClone)
	  thePrecipitationTrends = *vectorToClone;
  }


  PrecipitationForecast::PrecipitationForecast(wf_story_params& parameters)
	: theParameters(parameters)
	, theUseOllaVerb(false)
  {
	gatherPrecipitationData();
	findOutPrecipitationPeriods();
	findOutPrecipitationTrends();
  }

  PrecipitationForecast::~PrecipitationForecast()
  {
	theCoastalData.clear();
	theInlandData.clear();
	theFullData.clear();
  }
} // namespace TextGen

// ======================================================================
