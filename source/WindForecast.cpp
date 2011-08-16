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
#include "WindStoryTools.h"
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
  using namespace WindStoryTools;
  using namespace boost;
  using namespace std;

#define HUOMENNA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[huomenna] tuuli k‰‰ntyy [etel‰‰n]"

  /*
#define TIME_PLACE_INPLACES_FOG_COMPOSITE_PHRASE "[huomenna] [sis‰maassa] [paikoin] sumua"
#define TIME_PLACE_FOG_COMPOSITE_PHRASE "[huomenna] [sis‰maassa] sumua"
#define PLACE_INPLACES_FOG_COMPOSITE_PHRASE "[sis‰maassa] [paikoin] sumua"
#define PLACE_FOG_COMPOSITE_PHRASE "[sis‰maassa] sumua"
#define TIME_INPLACES_FOG_COMPOSITE_PHRASE "[huomenna] [paikoin] sumua"
#define TIME_FOG_COMPOSITE_PHRASE "[huomenna] sumua"
#define INPLACES_FOG_COMPOSITE_PHRASE "[paikoin] sumua"


#define TIME_PLACE_INPLACES_FOG_DENSE_COMPOSITE_PHRASE "[huomenna] [sis‰maassa] [paikoin] sumua, joka voi olla sakeaa"
#define TIME_PLACE_FOG_DENSE_COMPOSITE_PHRASE "[huomenna] [sis‰maassa] sumua, joka voi olla sakeaa"
#define PLACE_INPLACES_FOG_DENSE_COMPOSITE_PHRASE "[sis‰maassa] [paikoin] sumua, joka voi olla sakeaa"
#define PLACE_FOG_DENSE_COMPOSITE_PHRASE "[sis‰maassa] sumua, joka voi olla sakeaa"
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
	
    unsigned int forecastPeriodLength = get_period_length(theParameters.theForecastPeriod);
	for(unsigned int i = 0; i < theParameters.theWindDirectionLargeVector.size(); i++)
	  {
		if(sentence.size() > 0)
		  sentence << Delimiter(COMMA_PUNCTUATION_MARK);

		if(i > 0)
		  {
			bool spefifyDay = (forecastPeriodLength > 24 &&
							   abs(theParameters.theForecastTime.DifferenceInHours(theParameters.theWindDirectionLargeVector[i]->thePeriod.localStartTime())) > 21);
			// day phase specifier
			std::string dayPhasePhrase;
			sentence << get_time_phrase_large(theParameters.theWindDirectionLargeVector[i]->thePeriod,
											  spefifyDay,
											  theParameters.theVar, 
											  dayPhasePhrase,
											  true);

			//			sentence << dayPhasePhrase;
		  }
		sentence <<  direction_large_sentence(theParameters.theWindDirectionLargeVector[i]->theWindDirection);
		//		sentence <<  direction_sentence(theParameters.theWindDirectionVector[i]->theWindDirection);
		sentence << wind_speed_sentence(theParameters.theWindDirectionLargeVector[i]->thePeriod);
		
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

  const Sentence WindForecast::wind_speed_sentence(const WeatherPeriod& thePeriod) const
  {
	unsigned int min_index = 0;
	unsigned int max_index = 0;

	double meanValueSum = 0.0;
	double meanErrorSum = 0.0;

	for(unsigned int i = 1; i < theParameters.theRawDataVector.size(); i++)
	  {
		if(is_inside(theParameters.theRawDataVector[i]->thePeriod.localStartTime(), thePeriod))
		  {
			if (theParameters.theRawDataVector[i]->theWindSpeedMax.value() > 
				theParameters.theRawDataVector[max_index]->theWindSpeedMax.value())
			  max_index = i;
			if (theParameters.theRawDataVector[i]->theWindSpeedMin.value() < 
				theParameters.theRawDataVector[max_index]->theWindSpeedMin.value())
			  min_index = i;

			meanValueSum += theParameters.theRawDataVector[i]->theWindSpeedMean.value();
			meanErrorSum += theParameters.theRawDataVector[i]->theWindSpeedMean.error();
		  }
	  }
	WeatherResult meanResult(meanValueSum / theParameters.theRawDataVector.size(), 
							 meanErrorSum / theParameters.theRawDataVector.size());
	
	return speed_range_sentence(theParameters.theRawDataVector[min_index]->theWindSpeedMin,
								theParameters.theRawDataVector[max_index]->theWindSpeedMax,
								meanResult,
								theParameters.theVar);
  }

  const Sentence WindForecast::direction_sentence(const wind_direction_id& theDirectionId) const
  {
	Sentence sentence;

	switch(theDirectionId)
	  {
	  case POHJOINEN:
		sentence << "1-tuulta";
		break;
	  case POHJOISEN_PUOLEINEN:
		sentence << "1-puoleista tuulta";
		break;
	  case KOILLINEN:
		sentence << "2-tuulta";
		break;
	  case KOILLISEN_PUOLEINEN:
		sentence << "2-puoleista tuulta";
		break;
	  case ITA:
		sentence << "3-tuulta";
		break;
	  case IDAN_PUOLEINEN:
		sentence << "3-puoleista tuulta";
		break;
	  case KAAKKO:
		sentence << "4-tuulta";
		break;
	  case KAAKON_PUOLEINEN:
		sentence << "4-puoleista tuulta";
		break;
	  case ETELA:
		sentence << "5-tuulta";
		break;
	  case ETELAN_PUOLEINEN:
		sentence << "5-puoleista tuulta";
		break;
	  case LOUNAS:
		sentence << "6-tuulta";
		break;
	  case LOUNAAN_PUOLEINEN:
		sentence << "6-puoleista tuulta";
		break;
	  case LANSI:
		sentence << "7-tuulta";
		break;
	  case LANNEN_PUOLEINEN:
		sentence << "7-puoleista tuulta";
		break;
	  case LUODE:
		sentence << "8-tuulta";
		break;
	  case LUOTEEN_PUOLEINEN:
		sentence << "8-puoleista tuulta";
		break;
	  case VAIHTELEVA:
		sentence << "suunnaltaan vaihtelevaa" << "tuulta";
		break;
	  };
	return sentence;
  }

  const Sentence WindForecast::direction_large_sentence(const wind_direction_large_id& theWindDirectionId) const
  {
	Sentence sentence;

	switch(theWindDirectionId)
	  {
	  case POHJOINEN_:
		sentence << "1-tuulta";
		break;
	  case POHJOINEN_KOILLINEN:
		sentence << "pohjoisen ja koillisen v‰list‰ tuulta";
		break;
	  case KOILLINEN_:
		sentence << "2-tuulta";
		break;
	  case KOILLINEN_ITA:
		sentence << "koillisen ja id‰n v‰list‰ tuulta";
		break;
	  case ITA_:
		sentence << "3-tuulta";
		break;
	  case ITA_KAAKKO:
		sentence << "id‰n ja kaakon v‰list‰ tuulta";
		break;
	  case KAAKKO_:
		sentence << "4-tuulta";
		break;
	  case KAAKKO_ETELA:
		sentence << "kaakon ja etel‰n v‰list‰ tuulta";
		break;
	  case ETELA_:
		sentence << "5-tuulta";
		break;
	  case ETELA_LOUNAS:
		sentence << "etel‰n ja lounaan v‰list‰ tuulta";
		break;
	  case LOUNAS_:
		sentence << "6-tuulta";
		break;
	  case LOUNAS_LANSI:
		sentence << "lounaan ja l‰nnen v‰list‰ tuulta";
		break;
	  case LANSI_:
		sentence << "7-tuulta";
		break;
	  case LANSI_LUODE:
		sentence << "l‰nnen ja luoteen v‰list‰ tuulta";
		break;
	  case LUODE_:
		sentence << "8-tuulta";
		break;
	  case LUODE_POHJOINEN:
		sentence << "luoteen ja pohjoisen v‰list‰ tuulta";
		break;
	  case VAIHTELEVA_:
		sentence << "vaihtelevaa tuulta";
		break;
	  }

	return sentence;
  }


}
