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

#ifdef LATER
  Sentence WindForecast::windSentenceWithEvents(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

    unsigned int forecastPeriodLength = get_period_length(theParameters.theForecastPeriod);
	thePreviousRangeBeg = INT_MAX;
	thePreviousRangeEnd = INT_MAX;

	sentence << direction_sentence(theParameters.theWindDirectionVector[0]->theWindDirection);
	sentence << wind_speed_sentence(theParameters.theWindDirectionVector[0]->thePeriod);

	for(unsigned int i = 0; i < storyParams.theWindEventVector.size(); i++)
	  {
		wind_event_id windEventId = storyParams.theWindEventVector[i].second;

		switch (windEventId)
		  {
		  case TUULI_HEIKKENEE:
			break;
		  case TUULI_VOIMISTUU:
			break;
		  case TUULI_TYYNTYY:
			break;
		  case TUULI_KAANTYY:
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI:
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
	  }
  enum wind_event_id 
	{
	  TUULI_HEIKKENEE = 0x1,
	  TUULI_VOIMISTUU = 0x2,
	  TUULI_TYYNTYY = 0x4,
	  TUULI_KAANTYY = 0x8,
	  TUULI_MUUTTUU_VAIHTELEVAKSI = 0x10,
	  TUULI_KAANTYY_JA_HEIKEKNEE = 0x9,
	  TUULI_KAANTYY_JA_VOIMISTUU = 0xA,
	  TUULI_KAANTYY_JA_TYYNTYY = 0xC,
	  TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE = 0x11,
	  TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU = 0x12,
	  TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY = 0x14,
	  MISSING_WIND_EVENT = 0x0
	};

	returnb sentence;

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
  }
#endif




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
	for(unsigned int i = 0; i < theParameters.theWindDirectionVector.size(); i++)
	  {
		if(sentence.size() > 0)
		  sentence << Delimiter(COMMA_PUNCTUATION_MARK);

		if(i > 0)
		  {
			bool spefifyDay = (forecastPeriodLength > 24 &&
							   abs(theParameters.theForecastTime.DifferenceInHours(theParameters.theWindDirectionVector[i]->thePeriod.localStartTime())) > 21);
			// day phase specifier
			std::string dayPhasePhrase;
			sentence << get_time_phrase_large(theParameters.theWindDirectionVector[i]->thePeriod,
											  spefifyDay,
											  theParameters.theVar, 
											  dayPhasePhrase,
											  true);
		  }
		sentence << direction_sentence(theParameters.theWindDirectionVector[i]->theWindDirection);
		sentence << wind_speed_sentence(theParameters.theWindDirectionVector[i]->thePeriod);
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
