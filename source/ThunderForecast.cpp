// ======================================================================
/*!
 * \file
 * \brief Implementation of ThunderForecast class
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
#include "ThunderForecast.h"

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

  thunder_probability_id get_thunder_probability_id(const float& theThunderProbability)
  {
 	if(theThunderProbability == 0)
	  return NO_THUNDER;

	if(theThunderProbability < SMALL_PROBABILITY_FOR_THUNDER_UPPER_LIMIT)
	  {
		return SMALL_PROBABILITY_FOR_THUNDER;
	  }
	else if(theThunderProbability >= POSSIBLY_THUNDER_LOWER_LIMIT &&
			theThunderProbability < POSSIBLY_THUNDER_UPPER_LIMIT)
	  {
		return POSSIBLY_THUNDER;
	  }
	else
	  {
		return OCCASIONALLY_THUNDER;
	  }
 }


  ThunderForecast::ThunderForecast(wf_story_params& parameters):
	theParameters(parameters)
  {
  }

  float ThunderForecast::getMaxValue(const WeatherPeriod& theWeatherPeriod,
									 const weather_result_data_item_vector& theDataVector) const
  {
	float maxValue(0.0);
	for(unsigned int i = 0; i < theDataVector.size(); i++)
	  {
		if(theDataVector[i]->thePeriod.localStartTime() >= theWeatherPeriod.localStartTime() &&
		   theDataVector[i]->thePeriod.localStartTime() <= theWeatherPeriod.localEndTime() &&
		   theDataVector[i]->thePeriod.localEndTime() >= theWeatherPeriod.localStartTime() &&
		   theDataVector[i]->thePeriod.localEndTime() <= theWeatherPeriod.localEndTime())
		  {
			if(theDataVector[i]->theResult.value() > maxValue)
			  maxValue = theDataVector[i]->theResult.value();
		  }
	  }
	return maxValue;
  }

  Sentence ThunderForecast::thunderSentence(const WeatherPeriod& thePeriod,
											const forecast_area_id& theForecastAreaId) const
  {
	Sentence sentence;

	const weather_result_data_item_vector* thunderProbabilityData = 0;
	const weather_result_data_item_vector* thunderExtentData = 0;

	if(theForecastAreaId == FULL_AREA)
	  {
		thunderProbabilityData = ((*theParameters.theCompleteData[FULL_AREA])[THUNDER_PROBABILITY_DATA]);
		thunderExtentData = ((*theParameters.theCompleteData[FULL_AREA])[THUNDER_EXTENT_DATA]);
	  }
	if(theForecastAreaId == COASTAL_AREA)
	  {
		thunderProbabilityData = ((*theParameters.theCompleteData[COASTAL_AREA])[THUNDER_PROBABILITY_DATA]);
		thunderExtentData = ((*theParameters.theCompleteData[COASTAL_AREA])[THUNDER_EXTENT_DATA]);
	  }
	if(theForecastAreaId == INLAND_AREA)
	  {
		thunderProbabilityData = ((*theParameters.theCompleteData[INLAND_AREA])[THUNDER_PROBABILITY_DATA]);
		thunderExtentData = ((*theParameters.theCompleteData[INLAND_AREA])[THUNDER_EXTENT_DATA]);
	  }

	if(thunderProbabilityData && thunderExtentData)
	  {
		//		thunder_probability_id thunderId(NO_THUNDER);
		float maxThunderProbability(0.0);
		float maxThunderExtent(0.0);

		maxThunderProbability = getMaxValue(thePeriod, *thunderProbabilityData);
		maxThunderExtent = getMaxValue(thePeriod, *thunderExtentData);

		if(maxThunderExtent >= 5.0 && maxThunderExtent < 30.0)
		  {
			if(maxThunderProbability >= 5.0 && maxThunderProbability < 25.0)
			  {
				sentence << Delimiter(",");
				sentence << PAIKOIN_VOI_MYOS_UKKOSTAA_PHRASE;
			  }
			else if(maxThunderProbability >= 25.0 && maxThunderProbability < 55.0)
			  {
				sentence << Delimiter(",");
				sentence << PAIKOIN_MYOS_UKKOSTAA_PHRASE;
			  }
			else if(maxThunderProbability >= 55)
			  {
				sentence << JA_WORD;
				sentence << TODENNAKOISESTI_MYOS_UKKOSTAA_PHRASE;
			  }
		  }
		else if(maxThunderExtent >= 30)
		  {
			if(maxThunderProbability >= 5.0 && maxThunderProbability < 25.0)
			  {
				sentence << Delimiter(",");
				sentence << MAHDOLLISESTI_MYOS_UKKOSTAA_PHRASE;
			  }
			else if(maxThunderProbability >= 25.0 && maxThunderProbability < 55.0)
			  {
				sentence << Delimiter(",");
				sentence << MYOS_UKKOSTA_ESIINTYY_PHRASE;
			  }
			else if(maxThunderProbability >= 55)
			  {
				sentence << JA_WORD;
				sentence << TODENNAKOISESTI_MYOS_UKKOSTAA_PHRASE;
			  }
		  }

		/*
		thunderId = get_thunder_probability_id(maxThunderProbability);

		theParameters.theLog << "Thunder probability: "
							 << maxThunderProbability 
							 << " (" << thePeriod.localStartTime() 
							 << "..." 
							 << thePeriod.localEndTime() 
							 << ")"
							 << endl;

		if(thunderId == SMALL_PROBABILITY_FOR_THUNDER)
		  sentence << PIENI_UKKOSEN_TODENNAKOISYYS_PHRASE;
		else if(thunderId == POSSIBLY_THUNDER)
		  sentence << MAHDOLLISESTI_UKKOSTA_PHRASE;
		else if(thunderId == OCCASIONALLY_THUNDER)
		  sentence << AJOITTAIN_UKKOSTA_PHRASE;
		*/
	  }

	/*
	if(sentence.size() > 0 && !(theParameters.theForecastArea & FULL_AREA))
	  sentence << areaSpecificSentence(thePeriod);
	*/

	return sentence;
  }

  Sentence ThunderForecast::areaSpecificSentence(const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

	WeatherResult northEastShare(kFloatMissing, 0);
	WeatherResult southEastShare(kFloatMissing, 0);
	WeatherResult southWestShare(kFloatMissing, 0);
	WeatherResult northWestShare(kFloatMissing, 0);

	RangeAcceptor thunderlimits;
	thunderlimits.lowerLimit(SMALL_PROBABILITY_FOR_THUNDER_LOWER_LIMIT);
	AreaTools::getArealDistribution(theParameters.theSources,
									Thunder,
									theParameters.theArea,
									thePeriod,
									thunderlimits,
									northEastShare,
									southEastShare,
									southWestShare,
									northWestShare);

	float north = northEastShare.value() + northWestShare.value();
	float south = southEastShare.value() + southWestShare.value();
	float east = northEastShare.value() + southEastShare.value();
	float west = northWestShare.value() + southWestShare.value();

	sentence << area_specific_sentence(north,
									   south,
									   east,
									   west,
									   northEastShare.value(),
									   southEastShare.value(),
									   southWestShare.value(),
									   northWestShare.value(),
									   false);

	return sentence;
  }

}
