// ======================================================================
/*!
 * \file
 * \brief Implementation of WindForecast class
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
#include "WindForecast.h"

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

  /*
#define TIME_PLACE_INPLACES_FOG_COMPOSITE_PHRASE "[huomenna] [sisämaassa] [paikoin] sumua"
#define TIME_PLACE_FOG_COMPOSITE_PHRASE "[huomenna] [sisämaassa] sumua"
#define PLACE_INPLACES_FOG_COMPOSITE_PHRASE "[sisämaassa] [paikoin] sumua"
#define PLACE_FOG_COMPOSITE_PHRASE "[sisämaassa] sumua"
#define TIME_INPLACES_FOG_COMPOSITE_PHRASE "[huomenna] [paikoin] sumua"
#define TIME_FOG_COMPOSITE_PHRASE "[huomenna] sumua"
#define INPLACES_FOG_COMPOSITE_PHRASE "[paikoin] sumua"


#define TIME_PLACE_INPLACES_FOG_DENSE_COMPOSITE_PHRASE "[huomenna] [sisämaassa] [paikoin] sumua, joka voi olla sakeaa"
#define TIME_PLACE_FOG_DENSE_COMPOSITE_PHRASE "[huomenna] [sisämaassa] sumua, joka voi olla sakeaa"
#define PLACE_INPLACES_FOG_DENSE_COMPOSITE_PHRASE "[sisämaassa] [paikoin] sumua, joka voi olla sakeaa"
#define PLACE_FOG_DENSE_COMPOSITE_PHRASE "[sisämaassa] sumua, joka voi olla sakeaa"
#define TIME_INPLACES_FOG_DENSE_COMPOSITE_PHRASE "[huomenna] [paikoin] sumua, joka voi olla sakeaa"
#define TIME_FOG_DENSE_COMPOSITE_PHRASE "[huomenna] sumua, joka voi olla sakeaa"
#define INPLACES_FOG_DENSE_COMPOSITE_PHRASE "[paikoin] sumua, joka voi olla sakeaa"
  */

  WindForecast::WindForecast(wo_story_params& parameters):
	theParameters(parameters)
  {
  }

  void WindForecast::printOutWindData(std::ostream& theOutput) const
  {
  }
  
  void WindForecast::printOutWindPeriods(std::ostream& theOutput) const
  {
  }

													   /*
  Sentence WindForecast::getWindPhrase(const fog_type_id& theFogTypeId) const
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
													   */


													   /*
  Sentence FogForecast::constructFogSentence(const std::string theDayPhasePhrase,
											 const std::string& theAreaString,
											 const std::string& theInPlacesString,
											 const bool& thePossiblyDenseFlag) const
  {
	Sentence sentence;

	bool dayPhaseExists(!theDayPhasePhrase.empty());
	bool placeExists(!theAreaString.empty());
	bool inPlacesPhraseExists(!theInPlacesString.empty());
	
	if(dayPhaseExists)
	  {
		if(placeExists)
		  {
			if(inPlacesPhraseExists)
			  {
				if(thePossiblyDenseFlag)
				  sentence << TIME_PLACE_INPLACES_FOG_DENSE_COMPOSITE_PHRASE;
				else
				  sentence << TIME_PLACE_INPLACES_FOG_COMPOSITE_PHRASE;
				
				sentence << theDayPhasePhrase
						 << theAreaString
						 << theInPlacesString;
			  }
			else
			  {
				if(thePossiblyDenseFlag)
				  sentence << TIME_PLACE_FOG_DENSE_COMPOSITE_PHRASE;
				else
				  sentence << TIME_PLACE_FOG_COMPOSITE_PHRASE;
				
				sentence << theDayPhasePhrase
						 << theAreaString;
			  }
		  }
		else
		  {
			if(inPlacesPhraseExists)
			  {
				if(thePossiblyDenseFlag)
				  sentence << TIME_INPLACES_FOG_DENSE_COMPOSITE_PHRASE;
				else
				  sentence << TIME_INPLACES_FOG_COMPOSITE_PHRASE;
				sentence << theDayPhasePhrase
						 << theInPlacesString;
			  }
			else
			  {
				if(thePossiblyDenseFlag)
				  sentence << TIME_FOG_DENSE_COMPOSITE_PHRASE;
				else
				  sentence << TIME_FOG_COMPOSITE_PHRASE;
				sentence << theDayPhasePhrase;
			  }
		  }
	  }
	else
	  {
		if(placeExists)
		  {
			if(inPlacesPhraseExists)
			  {
				if(thePossiblyDenseFlag)
				  sentence << PLACE_INPLACES_FOG_DENSE_COMPOSITE_PHRASE;
				else
				  sentence << PLACE_INPLACES_FOG_COMPOSITE_PHRASE;
				sentence << theAreaString
						 << theInPlacesString;
			  }
			else
			  {
				if(thePossiblyDenseFlag)
				  sentence << INPLACES_FOG_DENSE_COMPOSITE_PHRASE;
				else
				  sentence << INPLACES_FOG_COMPOSITE_PHRASE;
				sentence << theAreaString;
			  }
		  }
		else
		  {
			if(inPlacesPhraseExists)
			  {
				if(thePossiblyDenseFlag)
				  sentence << INPLACES_FOG_DENSE_COMPOSITE_PHRASE;
				else
				  sentence << INPLACES_FOG_COMPOSITE_PHRASE;
				sentence << theInPlacesString;
			  }
			else
			  {				
				sentence << SUMUA_WORD;
				if(thePossiblyDenseFlag)
				  sentence << Delimiter(",") << JOKA_VOI_OLLA_SAKEAA_PHRASE;
			  }
		  }
	  }

	return sentence;
  }
													   */

  Sentence WindForecast::windSentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

	/*
	wind_speed_period_data_item_vector theWindSpeedVector;
	wind_direction_period_data_item_vector theWindDirectionVector;
	list<unsigned int> theOriginalWindSpeedIndexes;
	list<unsigned int> theEqualizedWindSpeedIndexes;
	list<unsigned int> theOriginalWindDirectionIndexes;
	list<unsigned int> theEqualizedWindDirectionIndexes;
	*/
	
	for(unsigned int i = 0; i < theParameters.theWindDirectionVector.size(); i++)
	  {
		if(sentence.size() > 0)
		  sentence << Delimiter(COMMA_PUNCTUATION_MARK);

		if(i > 0)
		  {
			// day phase specifier
			std::string dayPhasePhrase;
			sentence << get_time_phrase_large(theParameters.theWindDirectionVector[i]->thePeriod,
											  true,
											  theParameters.theVar, 
											  dayPhasePhrase,
											  true);

			//			sentence << dayPhasePhrase;
		  }
		sentence <<  direction_sentence(theParameters.theWindDirectionVector[i]->theWindDirection);
		
		/*
  std::ostream& operator<<(std::ostream & theOutput,
						   const WindDirectionPeriodDataItem& theWindDirectionPeriodDataItem)
  {
	theOutput << theWindDirectionPeriodDataItem.thePeriod.localStartTime()
			  << " ... "
			  << theWindDirectionPeriodDataItem.thePeriod.localEndTime()
			  << ": "
			  << get_wind_direction_string(theWindDirectionPeriodDataItem.theWindDirection)
			  << endl;

	return theOutput;
  }

		*/
	  }


	/*
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
	*/

	return sentence;
  }

  const Sentence WindForecast::direction_sentence(const wind_direction_id& theDirectionId) const
  {
	Sentence sentence;

	switch(theDirectionId)
	  {
	  case pohjoinen:
		sentence << "1-tuulta";
		break;
	  case pohjoisen_puoleinen:
		sentence << "1-puoleista tuulta";
		break;
	  case koillinen:
		sentence << "2-tuulta";
		break;
	  case koillisen_puoleinen:
		sentence << "2-puoleista tuulta";
		break;
	  case ita:
		sentence << "3-tuulta";
		break;
	  case idan_puoleinen:
		sentence << "3-puoleista tuulta";
		break;
	  case kaakko:
		sentence << "4-tuulta";
		break;
	  case kaakon_puoleinen:
		sentence << "4-puoleista tuulta";
		break;
	  case etela:
		sentence << "5-tuulta";
		break;
	  case etelan_puoleinen:
		sentence << "5-puoleista tuulta";
		break;
	  case lounas:
		sentence << "6-tuulta";
		break;
	  case lounaan_puoleinen:
		sentence << "6-puoleista tuulta";
		break;
	  case lansi:
		sentence << "7-tuulta";
		break;
	  case lannen_puoleinen:
		sentence << "7-puoleista tuulta";
		break;
	  case luode:
		sentence << "8-tuulta";
		break;
	  case luoteen_puoleinen:
		sentence << "8-puoleista tuulta";
		break;
	  case vaihteleva:
		sentence << "suunnaltaan vaihtelevaa" << "tuulta";
		break;
	  };
	return sentence;
  }

}
