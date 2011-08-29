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
#include "IntegerRange.h"
#include "UnitFactory.h"
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


#define TUULI_TYYNTYY_PHRASE "tuuli tyyntyy"
#define VOIMISTUVAA_TUULTA_PHRASE "voimistuvaa tuulta"
#define HEIKKENEVAA_TUULTA_PHRASE "heikkenev‰‰ tuulta"
#define POHJOISEEN_PHRASE "pohjoiseen"
#define ETELAAN_PHRASE "etel‰‰n"
#define ITAAN_PHRASE "it‰‰n"
#define LANTEEN_PHRASE "l‰nteen"
#define KOILLISEEN_PHRASE "koilliseen"
#define KAAKKOON_PHRASE "kaakkoon"
#define LOUNAASEEN_PHRASE "lounaaseen"
#define LUOTEESEEN_PHRASE "luoteeseen"
#define POHJOISEN_PUOLELLE_PHRASE "pohjoisen puolelle"
#define ETELAN_PUOLELLE_PHRASE "etel‰n puolelle"
#define IDAN_PUOLELLE_PHRASE "id‰n puolelle"
#define LANNEN_PUOLELLE_PHRASE "l‰nnen puolelle"
#define KOILLISEN_PUOLELLE_PHRASE "koillisen puolelle"
#define KAAKON_PUOLELLE_PHRASE "kaakon puolelle"
#define LOUNAAN_PUOLELLE_PHRASE "lounaan puolelle"
#define LUOTEEN_PUOLELLE_PHRASE "luoteen puolelle"
#define POHJOISEN_JA_KOILLISEN_VALILLE_PHRASE "pohjoisen ja koillisen v‰lille"
#define KOILLISEN_JA_IDAN_VALILLE_PHRASE "koillisen ja id‰n v‰lille"
#define IDAN_JA_KAAKON_VALILLE_PHRASE "id‰n ja kaakon v‰lille"
#define KAAKON_JA_ETELAN_VALILLE_PHRASE "kaakon ja etel‰n v‰lille"
#define LOUNAAN_JA_ETELAN_VALILLE_PHRASE "lounaan ja etel‰n v‰lille"
#define LANNEN_JA_LOUNAAN_VALILLE_PHRASE "l‰nnen ja lounaan v‰lille"
#define LUOTEEN_JA_LANNEN_VALILLE_PHRASE "luoteen ja l‰nnen v‰lille"
#define LUOTEEN_JA_POHJOISEN_VALILLE_PHRASE "luoteen ja pohjoisen v‰lille"
#define TUULI_MUUTTUU_VAIHTELEVAKSI_PHRASE "tuuli muuttuu vaihtelevaksi"

#define ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltap‰iv‰ll‰] tuuli k‰‰ntyy [etel‰‰n]"
#define ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI "[iltap‰iv‰ll‰] tuuli muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_ETELAAN_KAANTYVAA_TUULTA "[iltap‰iv‰ll‰] [etel‰‰n] k‰‰ntyv‰‰ tuulta"

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

  std::string get_wind_direction16_turnto_string(const wind_direction16_id& theWindDirectionId)
  {
	std::string retval;

	/*
#define POHJOISEN_PUOLELLE_PHRASE "pohjoisen puolelle"
#define ETELAN_PUOLELLE_PHRASE "etel‰n puolelle"
#define IDAN_PUOLELLE_PHRASE "id‰n puolelle"
#define LANNEN_PUOLELLE_PHRASE "l‰nnen puolelle"
#define KOILLISEN_PUOLELLE_PHRASE "koillisen puolelle"
#define KAAKON_PUOLELLE_PHRASE "kaakon puolelle"
#define LOUNAAN_PUOLELLE_PHRASE "lounaan puolelle"
#define LUOTEEN_PUOLELLE_PHRASE "luoteen puolelle"

	 */

	switch(theWindDirectionId)
	  {
	  case POHJOINEN_:
		retval = POHJOISEEN_PHRASE;
		break;
	  case POHJOINEN_PUOLEINEN:
		retval = POHJOISEN_PUOLELLE_PHRASE;
		break;
	  case POHJOINEN_KOILLINEN:
		retval = POHJOISEN_JA_KOILLISEN_VALILLE_PHRASE;
		break;
	  case KOILLINEN_:
		retval = KOILLISEEN_PHRASE;
		break;
	  case KOILLINEN_PUOLEINEN:
		retval = KOILLISEN_PUOLELLE_PHRASE;
		break;
	  case KOILLINEN_ITA:
		retval = KOILLISEN_JA_IDAN_VALILLE_PHRASE;
		break;
	  case ITA_:
		retval = ITAAN_PHRASE;
		break;
	  case ITA_PUOLEINEN:
		retval = IDAN_PUOLELLE_PHRASE;
		break;
	  case ITA_KAAKKO:
		retval = IDAN_JA_KAAKON_VALILLE_PHRASE;
		break;
	  case KAAKKO_:
		retval = KAAKKOON_PHRASE;
		break;
	  case KAAKKO_PUOLEINEN:
		retval = KAAKON_PUOLELLE_PHRASE;
		break;
	  case KAAKKO_ETELA:
		retval = KAAKON_JA_ETELAN_VALILLE_PHRASE;
		break;
	  case ETELA_:
		retval = ETELAAN_PHRASE;
		break;
	  case ETELA_PUOLEINEN:
		retval = ETELAN_PUOLELLE_PHRASE;
		break;
	  case ETELA_LOUNAS:
		retval = LOUNAAN_JA_ETELAN_VALILLE_PHRASE;
		break;
	  case LOUNAS_:
		retval = LOUNAASEEN_PHRASE;
		break;
	  case LOUNAS_PUOLEINEN:
		retval = LOUNAAN_PUOLELLE_PHRASE;
		break;
	  case LOUNAS_LANSI:
		retval = LANNEN_JA_LOUNAAN_VALILLE_PHRASE;
		break;
	  case LANSI_:
		retval = LANTEEN_PHRASE;
		break;
	  case LANSI_PUOLEINEN:
		retval = LANNEN_PUOLELLE_PHRASE;
		break;
	  case LANSI_LUODE:
		retval = LUOTEEN_JA_LANNEN_VALILLE_PHRASE;
		break;
	  case LUODE_:
		retval = LUOTEESEEN_PHRASE;
		break;
	  case LUODE_PUOLEINEN:
		retval = LUOTEEN_PUOLELLE_PHRASE;
		break;
	  case LUODE_POHJOINEN:
		retval = LUOTEEN_JA_POHJOISEN_VALILLE_PHRASE;
		break;
	  case VAIHTELEVA_:
		retval = "vaihtelevaksi";
		break;
	  }

	return retval;
  }
  
  wind_direction16_id get_wind_direction16_id(const NFmiTime& timestamp,
											  const wind_direction16_period_data_item_vector& windDirection16Vector)
  {
	for(unsigned int i = 0; i < windDirection16Vector.size(); i++)
	  {
		if(is_inside(timestamp, windDirection16Vector[i]->thePeriod))
		  return windDirection16Vector[i]->theWindDirection;
	  }
	  
	return VAIHTELEVA_;
  }

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

  Sentence WindForecast::windSentenceWithEvents(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

	// first tell about the beginning of the forecast period
	sentence << direction16_sentence(theParameters.theWindDirection16Vector[0]->theWindDirection);
	sentence << wind_speed_sentence(theParameters.theWindDirection16Vector[0]->thePeriod);

	// then iterate through the events and raport about them
	//    unsigned int forecastPeriodLength = get_period_length(theParameters.theForecastPeriod);
	thePreviousRangeBeg = INT_MAX;
	thePreviousRangeEnd = INT_MAX;

	Paragraph paragraph;

	for(unsigned int i = 0; i < theParameters.theWindEventVector.size(); i++)
	  {
		Sentence eventSentence;

		wind_event_id windEventId = theParameters.theWindEventVector[i].second;
		Sentence timePhrase;
		timePhrase << get_time_phrase(theParameters.theWindEventVector[i].first, theParameters.theVar);
		Sentence windDirectionTurningToPhrase;
		wind_direction16_id directionId = get_wind_direction16_id(theParameters.theWindEventVector[i].first,
																  theParameters.theWindDirection16Vector);
		windDirectionTurningToPhrase << get_wind_direction16_turnto_string(directionId);

		/*
#define TUULI_TYYNTYY_PHRASE "tuuli tyyntyy"
#define VOIMISTUVAA_TUULTA_PHRASE "voimistuvaa tuulta"
#define HEIKKENEVAA_TUULTA_PHRASE "heinnenev‰‰ tuulta"
#define POHJOISEEN_PHRASE "pohjoiseen"
#define ETELAAN_PHRASE "etel‰‰n"
#define ITAAN_PHRASE "it‰‰n"
#define LANTEEN_PHRASE "l‰nteen"
#define KOILLISEEN_PHRASE "koilliseen"
#define KAAKKOON_PHRASE "kaakkoon"
#define LOUNAASEEN_PHRASE "lounaaseen"
#define LUOTEESEEN_PHRASE "luoteseen"
#define POHJOISEN_JA_KOILLISEN_VALILLE_PHRASE "pohjoisen ja koillisen v‰lille"
#define KOILLISEN_JA_IDAN_VALILLE_PHRASE "koillisen ja id‰n v‰lille"
#define IDAN_JA_KAAKON_VALILLE_PHRASE "id‰n ja kaakon v‰lille"
#define KAAKON_JA_ETELAN_VALILLE_PHRASE "kaakkon ja etel‰n v‰lille"
#define LOUNAAN_JA_ETELAN_VALILLE_PHRASE "lounaan ja etel‰n v‰lille"
#define LANNEN_JA_LOUNAAN_VALILLE_PHRASE "l‰nnen ja lounaan v‰lille"
#define LUOTEEN_JA_LANNEN_VALILLE_PHRASE "luoteen ja l‰nnen v‰lille"
#define LUOTEEN_JA_POHJOISEN_VALILLE_PHRASE "luoteen ja pohjoisen v‰lille"

#define ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[huomenna] tuuli k‰‰ntyy [etel‰‰n]"
#define ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI "[iltap‰iv‰ll‰] tuuli muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_ETELAAN_KAANTYVAA_TUULTA "[iltap‰iv‰ll‰] [etel‰‰n] k‰‰ntyv‰‰ tuulta"

 std::string get_wind_event_string(const wind_event_id& theWindEventId)
  {
	std::string retval;

	switch(theWindEventId)
	  {
	  case TUULI_HEIKKENEE:
		retval = "tuuli heikkenee";
		break;
	  case TUULI_VOIMISTUU:
		retval = "tuuli voimistuu";
		break;
	  case TUULI_TYYNTYY:
		retval = "tuuli tyyntyy";
		break;
	  case TUULI_KAANTYY:
		retval = "tuuli k‰‰ntyy";
		break;
	  case TUULI_KAANTYY_JA_HEIKEKNEE:
		retval = "tuuli k‰‰ntyy ja heikkenee";
		break;
	  case TUULI_KAANTYY_JA_VOIMISTUU:
		retval = "tuuli k‰‰ntyy ja voimistuu";
		break;
	  case TUULI_KAANTYY_JA_TYYNTYY:
		retval = "tuuli k‰‰ntyy ja tyyntyy";
		break;
	  case TUULI_MUUTTUU_VAIHTELEVAKSI:
		retval = "tuuli muuttuu vaihtelevaksi";
		break;
	  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE:
		retval = "tuuli muuttuu vaihtelevaksi ja heikkenee";
		break;
	  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU:
		retval = "tuuli muuttuu vaihtelevaksi ja voimistuu";
		break;
	  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY:
		retval = "tuuli muuttuu vaihtelevaksi ja tyyntyy";
		break;
	  case MISSING_WIND_EVENT:
		retval = "missing wind event";
		break;
	  }

	return retval;
  }


		 */
		switch (windEventId)
		  {
		  case TUULI_HEIKKENEE:
			eventSentence << timePhrase << HEIKKENEVAA_TUULTA_PHRASE;
			// TODO: wind speed period
			//eventSentence << wind_speed_sentence(theParameters.theWindDirection16Vector[i]->thePeriod);

			break;
		  case TUULI_VOIMISTUU:
			eventSentence << timePhrase << VOIMISTUVAA_TUULTA_PHRASE;
			break;
		  case TUULI_TYYNTYY:
			eventSentence << timePhrase << TUULI_TYYNTYY_PHRASE;
			break;
		  case TUULI_KAANTYY:
			eventSentence << ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE
						  << timePhrase 
						  << windDirectionTurningToPhrase;
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI:
			eventSentence << ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI
						  << timePhrase; 
			break;
		  case TUULI_KAANTYY_JA_HEIKEKNEE:
			break;
		  case TUULI_KAANTYY_JA_VOIMISTUU:
			break;
		  case TUULI_KAANTYY_JA_TYYNTYY:
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE:
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU:
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY:
			break;
		  case MISSING_WIND_EVENT:
			break;
		  };

		paragraph << eventSentence;
	  }

	sentence << paragraph;

	return sentence;

#ifdef LATER

  /*
 void log_wind_events(wo_story_params& storyParams)
  {
	storyParams.theLog << "*********** WIND EVENTS ***********" << endl;

	unsigned int windDirectionIndex = 0;
	unsigned int windSpeedIndex = 0;

	for(unsigned int i = 0; i < storyParams.theWindEventVector.size(); i++)
	  {
		wind_event_id windEventId = storyParams.theWindEventVector[i].second;

		storyParams.theLog << storyParams.theWindEventVector[i].first 
						   << " "
						   <<  get_wind_event_string(storyParams.theWindEventVector[i].second)
						   << " ";

		if(windEventId > 0x0 && windEventId <= TUULI_TYYNTYY ||
		   windEventId >= TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE && windEventId <= TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY)
		  {
			storyParams.theLog << "(";
			storyParams.theLog << get_wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex++]->theWindSpeedId);
			storyParams.theLog <<  " -> ";
			storyParams.theLog << get_wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex]->theWindSpeedId);
			storyParams.theLog << ")" << endl;
			if(windEventId >= TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE)
			  windDirectionIndex++;
		  }
		else if(windEventId == TUULI_KAANTYY)
		  {
			storyParams.theLog << "(";
			storyParams.theLog << get_wind_direction16_string(storyParams.theWindDirection16Vector[windDirectionIndex++]->theWindDirection);
			storyParams.theLog <<  " -> ";
			storyParams.theLog <<  get_wind_direction16_string(storyParams.theWindDirection16Vector[windDirectionIndex]->theWindDirection);			
			storyParams.theLog << ")" << endl;
		  }
		else if(windEventId >= TUULI_KAANTYY_JA_HEIKEKNEE && windEventId <= TUULI_KAANTYY_JA_TYYNTYY)
		  {
			storyParams.theLog << "(";
			storyParams.theLog << get_wind_direction16_string(storyParams.theWindDirection16Vector[windDirectionIndex++]->theWindDirection);
			storyParams.theLog <<  " -> ";
			storyParams.theLog << get_wind_direction16_string(storyParams.theWindDirection16Vector[windDirectionIndex]->theWindDirection);
			storyParams.theLog << "; ";
			storyParams.theLog << get_wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex++]->theWindSpeedId);
			storyParams.theLog <<  " -> ";
			storyParams.theLog <<  get_wind_speed_string(storyParams.theWindSpeedVector[windSpeedIndex]->theWindSpeedId);
			storyParams.theLog << ")" << endl;
		  }
		else
		  {
			storyParams.theLog << "" << endl;

			if(windEventId == TUULI_MUUTTUU_VAIHTELEVAKSI)
			  windDirectionIndex++;
		  }

		*/
#endif

  }




  Sentence WindForecast::windSentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

    unsigned int forecastPeriodLength = get_period_length(theParameters.theForecastPeriod);
	thePreviousRangeBeg = INT_MAX;
	thePreviousRangeEnd = INT_MAX;

	/*
	  for(unsigned int i = 0; i < theParameters.theWindDirection16Vector.size(); i++)
	  {
	  if(sentence.size() > 0)
	  sentence << Delimiter(COMMA_PUNCTUATION_MARK);

	  if(i > 0)
	  {
	  bool spefifyDay = (forecastPeriodLength > 24 &&
	  abs(theParameters.theForecastTime.DifferenceInHours(theParameters.theWindDirection16Vector[i]->thePeriod.localStartTime())) > 21);
	  // day phase specifier
	  std::string dayPhasePhrase;
	  sentence << get_time_phrase_large(theParameters.theWindDirection16Vector[i]->thePeriod,
	  spefifyDay,
	  theParameters.theVar, 
	  dayPhasePhrase,
	  true);
	  }
	  sentence <<  direction16_sentence(theParameters.theWindDirection16Vector[i]->theWindDirection);
	  //		sentence <<  direction_sentence(theParameters.theWindDirectionVector[i]->theWindDirection);
	  sentence << wind_speed_sentence(theParameters.theWindDirection16Vector[i]->thePeriod);
	  }
	*/		
	for(unsigned int i = 0; i < theParameters.theWindDirection16Vector.size(); i++)
	  {
		if(sentence.size() > 0)
		  sentence << Delimiter(COMMA_PUNCTUATION_MARK);

		if(i > 0)
		  {
			bool spefifyDay = (forecastPeriodLength > 24 &&
							   abs(theParameters.theForecastTime.DifferenceInHours(theParameters.theWindDirection16Vector[i]->thePeriod.localStartTime())) > 21);
			// day phase specifier
			std::string dayPhasePhrase;
			sentence << get_time_phrase_large(theParameters.theWindDirection16Vector[i]->thePeriod,
											  spefifyDay,
											  theParameters.theVar, 
											  dayPhasePhrase,
											  true);
		  }
		sentence << direction16_sentence(theParameters.theWindDirection16Vector[i]->theWindDirection);
		sentence << wind_speed_sentence(theParameters.theWindDirection16Vector[i]->thePeriod);
	  }

	return sentence;
  }

  const Sentence WindForecast::speed_range_sentence_(const WeatherResult & theMinSpeed,
													 const WeatherResult & theMaxSpeed,
													 const WeatherResult & theMeanSpeed,
													 const string & theVariable) const
  {
	using Settings::optional_int;

	Sentence sentence;

	const int minvalue = static_cast<int>(round(theMinSpeed.value()));
	const int maxvalue = static_cast<int>(round(theMaxSpeed.value()));
	const int meanvalue = static_cast<int>(round(theMeanSpeed.value()));

	const string var = "textgen::units::meterspersecond::format";
	const string opt = Settings::optional_string(var,"SI");

	if(opt == "textphrase")
	  {
		sentence << *UnitFactory::create(MetersPerSecond, maxvalue) << "tuulta";
	  }
	else
	  {
		const int mininterval = optional_int(theVariable+"::mininterval",0);
		const string rangeseparator = Settings::optional_string(theVariable+"::rangeseparator","-");
	  
		if(maxvalue - minvalue < mininterval)
		  {
			if(meanvalue != thePreviousRangeBeg)
			  {
				sentence << "noin"
						 << meanvalue;
				thePreviousRangeBeg = meanvalue;
			  }
		  }
		else
		  {
			if(meanvalue != thePreviousRangeBeg && maxvalue != thePreviousRangeEnd)
			  {
				sentence << IntegerRange(meanvalue,maxvalue,rangeseparator);
				thePreviousRangeBeg = meanvalue;
				thePreviousRangeEnd = maxvalue;
			  }
		  }
		if(!sentence.empty())
		  sentence << *UnitFactory::create(MetersPerSecond);
	  }
	  
	return sentence;
	  
  }

  const Sentence WindForecast::wind_speed_sentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

	unsigned int min_index = 0;
	unsigned int max_index = 0;

	double meanValueSum = 0.0;
	double meanErrorSum = 0.0;
	unsigned int counter = 0;

	for(unsigned int i = 0; i < theParameters.theRawDataVector.size(); i++)
	  {
		const WindDataItemUnit& dataItem = (*theParameters.theRawDataVector[i])();
		const WindDataItemUnit& dataItemMax = (*theParameters.theRawDataVector[max_index])();
		const WindDataItemUnit& dataItemMin = (*theParameters.theRawDataVector[min_index])();

		if(is_inside(dataItem.thePeriod.localStartTime(), thePeriod))
		  {
			if (dataItem.theWindSpeedMax.value() > 
				dataItemMax.theWindSpeedMax.value())
			  max_index = i;
			if (dataItem.theWindSpeedMin.value() < 
				dataItemMin.theWindSpeedMin.value())
			  min_index = i;

			meanValueSum += dataItem.theWindSpeedMean.value();
			meanErrorSum += dataItem.theWindSpeedMean.error();
			counter++;
		  }
	  }

	if(counter == 0)
	  return sentence;

	WeatherResult meanResult(meanValueSum / counter, meanErrorSum / counter);

	const WindDataItemUnit& dataItemMin = (*theParameters.theRawDataVector[min_index])();
	const WindDataItemUnit& dataItemMax = (*theParameters.theRawDataVector[max_index])();

	sentence <<  speed_range_sentence_(dataItemMin.theWindSpeedMin,
									   dataItemMax.theWindSpeedMax,
									   meanResult,
									   theParameters.theVar);

	return sentence;
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
		sentence << "suunnaltaan vaihtelevaa tuulta";
		break;
	  };
	return sentence;
  }

  const Sentence WindForecast::direction16_sentence(const wind_direction16_id& theWindDirectionId) const
  {
	Sentence sentence;

	switch(theWindDirectionId)
	  {
	  case POHJOINEN_:
		sentence << "1-tuulta";
		break;
	  case POHJOINEN_PUOLEINEN:
		sentence << "1-puoleista tuulta";
		break;
	  case POHJOINEN_KOILLINEN:
		sentence << "pohjoisen ja koillisen v‰list‰ tuulta";
		break;
	  case KOILLINEN_:
		sentence << "2-tuulta";
		break;
	  case KOILLINEN_PUOLEINEN:
		sentence << "2-puoleista tuulta";
		break;
	  case KOILLINEN_ITA:
		sentence << "koillisen ja id‰n v‰list‰ tuulta";
		break;
	  case ITA_:
		sentence << "3-tuulta";
		break;
	  case ITA_PUOLEINEN:
		sentence << "3-puoleista tuulta";
		break;
	  case ITA_KAAKKO:
		sentence << "id‰n ja kaakon v‰list‰ tuulta";
		break;
	  case KAAKKO_:
		sentence << "4-tuulta";
		break;
	  case KAAKKO_PUOLEINEN:
		sentence << "4-puoleista tuulta";
		break;
	  case KAAKKO_ETELA:
		sentence << "kaakon ja etel‰n v‰list‰ tuulta";
		break;
	  case ETELA_:
		sentence << "5-tuulta";
		break;
	  case ETELA_PUOLEINEN:
		sentence << "5-puoleista tuulta";
		break;
	  case ETELA_LOUNAS:
		sentence << "etel‰n ja lounaan v‰list‰ tuulta";
		break;
	  case LOUNAS_:
		sentence << "6-tuulta";
		break;
	  case LOUNAS_PUOLEINEN:
		sentence << "6-puoleista tuulta";
		break;
	  case LOUNAS_LANSI:
		sentence << "lounaan ja l‰nnen v‰list‰ tuulta";
		break;
	  case LANSI_:
		sentence << "7-tuulta";
		break;
	  case LANSI_PUOLEINEN:
		sentence << "7-puoleista tuulta";
		break;
	  case LANSI_LUODE:
		sentence << "l‰nnen ja luoteen v‰list‰ tuulta";
		break;
	  case LUODE_:
		sentence << "8-tuulta";
		break;
	  case LUODE_PUOLEINEN:
		sentence << "8-puoleista tuulta";
		break;
	  case LUODE_POHJOINEN:
		sentence << "luoteen ja pohjoisen v‰list‰ tuulta";
		break;
	  case VAIHTELEVA_:
		sentence << "suunnaltaan vaihtelevaa tuulta";
		break;
	  }

	return sentence;
  }


}
