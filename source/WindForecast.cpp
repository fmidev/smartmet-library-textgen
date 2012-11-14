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
#include "WindForecast.h"

#include <newbase/NFmiCombinedParam.h>
#include <newbase/NFmiMercatorArea.h>

#include <boost/lexical_cast.hpp>
#include <vector>
#include <map>
#include <iomanip>
#include <cmath>

namespace TextGen
{

  using namespace Settings;
  using namespace TextGen;
  using namespace AreaTools;
  using namespace WindStoryTools;
  using namespace boost;
  using namespace std;


  void print_period_and_text(const string& label, const WeatherPeriod& period, const string& text)
  {
	cout << label << period.localStartTime() << "..." << period.localEndTime() << " " << text << endl;
  }

  int wind_forecast_period_length(const WeatherPeriod& thePeriod)
  {
	return get_period_length(thePeriod)+1;
  }
  

  WindForecast::WindForecast(wo_story_params& parameters):
	theParameters(parameters),
	thePreviousDayNumber(parameters.theForecastPeriod.localStartTime().GetWeekday()),
	thePreviousPartOfTheDay(MISSING_PART_OF_THE_DAY_ID)
  {
  }

  Paragraph WindForecast::getWindStory(const WeatherPeriod& thePeriod) const
  {
	Paragraph paragraph;

	WindEventId eventIdPreviousReported(MISSING_WIND_EVENT);
	WeatherPeriod weatherPeriodPreviousReported(WeatherPeriod(NFmiTime(1971,1,1),NFmiTime(1971,1,1)));

	WindEventPeriodDataItem* eventPeriodItemPrevious(0);
	WindEventPeriodDataItem* eventPeriodItemNext(0);
	WindDirectionId previousWindDirectionId(MISSING_WIND_DIRECTION_ID);
	for(unsigned int i = 0; i < theParameters.theMergedWindEventPeriodVector.size(); i++)
	  {
		Sentence sentence;
		WindEventPeriodDataItem* eventPeriodItem = theParameters.theMergedWindEventPeriodVector[i];
		bool lastPeriod(i == theParameters.theMergedWindEventPeriodVector.size() - 1);
		bool firstPeriod(i == 0);
		bool eventPeriodNextIsMissingEvent(false);
		
		if(!lastPeriod)
		  {
			eventPeriodItemNext = theParameters.theMergedWindEventPeriodVector[i+1];
			if(get_period_length(eventPeriodItemNext->thePeriod) == 0)
			  {
				eventPeriodItem->theWindSpeedChangeEnds = true;
			  }
			else
			  {
				eventPeriodNextIsMissingEvent = (eventPeriodItemNext->theWindEvent == MISSING_WIND_EVENT);
			  }
		  }

		// if the first or last period is 1 hour long, ignore it
		if((firstPeriod || lastPeriod) && get_period_length(eventPeriodItem->thePeriod) == 0)
		  continue;

		WindEventId eventId = eventPeriodItem->theWindEvent;

		bool firstSentenceInTheStory = paragraph.empty();

		theParameters.theAlkaenPhraseUsed = false;

		const WeatherPeriod& eventPeriod = eventPeriodItem->thePeriod;

		WeatherPeriod eventStartPeriod(eventPeriod.localStartTime(),
									   eventPeriod.localStartTime());
		WeatherPeriod eventEndPeriod(eventPeriod.localEndTime(),
									 eventPeriod.localEndTime());
		
		WindDirectionId windDirectionIdBeg(get_wind_direction_id_at(theParameters.theWindDataVector,
																	theParameters.theArea,
																	eventPeriod.localStartTime(),
																	theParameters.theVar));

		WindDirectionId windDirectionIdAvg(get_wind_direction_id_at(theParameters.theWindDataVector,
																	theParameters.theArea,
																	theParameters.theSources,
																	eventPeriod,
																	theParameters.theVar));
		if(get_period_length(eventPeriod) == 0)
		  windDirectionIdAvg = windDirectionIdBeg;
				
		bool same_part_of_the_day(get_part_of_the_day_id(eventPeriod.localStartTime()) ==
								  get_part_of_the_day_id(eventPeriod.localEndTime()) &&
								  eventPeriod.localStartTime().GetDay() == eventPeriod.localEndTime().GetDay());

		bool windSpeedDifferEnough(wind_speed_differ_enough(theParameters.theSources,
															theParameters.theArea,
															eventPeriod,
															theParameters.theVar,
															theParameters.theWindDataVector));

		bool windSpeedDifferEnoughOnNextPeriod(eventPeriodItemNext &&
											   wind_speed_differ_enough(theParameters.theSources,
																		theParameters.theArea,
																		eventPeriodItemNext->thePeriod,
																		theParameters.theVar,
																		theParameters.theWindDataVector));

		if((eventId & TUULI_HEIKKENEE || eventId & TUULI_VOIMISTUU) &&
		   (eventPeriodItem->theWindSpeedChangeStarts &&
			eventPeriodItem->theWindSpeedChangeEnds) &&
		   !windSpeedDifferEnough)
		  {
			eventId = mask_wind_event(eventId,  (eventId & TUULI_HEIKKENEE ? TUULI_HEIKKENEE : TUULI_VOIMISTUU));
		  }

		// when wind is weak dont report changes in speed and direction, just
		// tell speed and direction for the whole weak period if it is different from the perevious
		if(eventPeriodItem->theWeakWindPeriodFlag)
		  {
			// if we have short weak period in the middle, ignore it
			float period_len(get_period_length(eventPeriod));
			float forecast_period_len( get_period_length(theParameters.theForecastPeriod));

			if(period_len <= 2 && period_len / forecast_period_len < 0.2 &&
			   i > 0 && !firstPeriod && !lastPeriod)
			  continue;

			  eventId = MISSING_WIND_EVENT;
		  }
		else if((eventIdPreviousReported & TUULI_MUUTTUU_VAIHTELEVAKSI) &&
				eventId == MISSING_WIND_EVENT &&
				windDirectionIdAvg == VAIHTELEVA)
		  {
			// if previous event was 'tuuli muuttuu vaihtelevaksi',
			// dont report 'tuuli on vaihtelevaa' on current period
			continue;
		  }

		// if wind vas vaihteleva on previous period, dont report 'tuuli muuttuu vaihtelevaksi'
		// on this period
		if((eventId & TUULI_MUUTTUU_VAIHTELEVAKSI) &&
		   eventPeriodItemPrevious &&
		   get_wind_direction_id_at(theParameters.theWindDataVector,
									theParameters.theArea,
									theParameters.theSources,
									eventPeriodItemPrevious->thePeriod,
									theParameters.theVar) == VAIHTELEVA)
		  continue;

		// if wind direction changes in two successive periods,
		// it can not be the same in the end of these periods
		if(eventId & TUULI_KAANTYY)
		  {
			if(eventPeriodItemPrevious && (eventIdPreviousReported & TUULI_KAANTYY))
			  {
				if(get_wind_direction_id_at(theParameters.theWindDataVector,
											theParameters.theArea,
											eventPeriodItem->thePeriod.localEndTime(),
											theParameters.theVar) ==
				   get_wind_direction_id_at(theParameters.theWindDataVector,
											theParameters.theArea,
											weatherPeriodPreviousReported.localEndTime(),
											theParameters.theVar))
				  eventId = mask_wind_event(eventId, TUULI_KAANTYY);
			  }

			if(get_period_length(eventPeriod) == 0 && windDirectionIdAvg != VAIHTELEVA)
			  {
				eventId = mask_wind_event(eventId, TUULI_KAANTYY);
			  }
		  }

		if((eventId & TUULI_HEIKKENEE) && 
		   (eventPeriodItem->theWindSpeedChangeStarts &&
			eventPeriodItem->theWindSpeedChangeEnds))
		  {
			if(get_maximum_wind(eventStartPeriod,
								theParameters.theArea,
								theParameters.theWindDataVector) < WEAK_WIND_SPEED_UPPER_LIMIT + 1.0)
			  eventId = mask_wind_event(eventId, TUULI_HEIKKENEE);
		  }

		switch(eventId)
		  {
		  case TUULI_HEIKKENEE:
		  case TUULI_VOIMISTUU:
			{
			  // can not weaken/strengthen at once
			  if(get_period_length(eventPeriod) == 0)
				continue;

			  bool smallChange(false);
			  bool gradualChange(false);
			  bool fastChange(false);
			  std::string changeAttributeStr(EMPTY_STRING);

			  if(!getWindSpeedChangeAttribute(eventPeriod, changeAttributeStr,
											  smallChange, gradualChange, fastChange))
				{
				  return paragraph;
				}
			  
			  Sentence directionSentenceP;
			  Sentence directionSentence;

			  if((i > 0 && same_direction(previousWindDirectionId, windDirectionIdAvg, true)) ||
				 (eventIdPreviousReported == TUULI_MUUTTUU_VAIHTELEVAKSI && windDirectionIdAvg == VAIHTELEVA))
				{
				  directionSentenceP << TUULTA_WORD;
				  directionSentence << TUULI_WORD;
				}
			  else
				{
				  directionSentenceP << windDirectionSentence(windDirectionIdAvg);
				  directionSentence << windDirectionSentence(windDirectionIdAvg, true);
				}

			  bool useAlkaaHeiketaVoimistuaPhrase(false);

			  bool appendDecreasingIncreasingInterval(true);

			  bool windStrengthStartsToChange(eventPeriodItem->theWindSpeedChangeStarts &&
											  !eventPeriodItem->theWindSpeedChangeEnds);

			  bool reportSpeedEvenIfSpeedChangeContinues(!eventPeriodItem->theWindSpeedChangeEnds && 
														 windSpeedDifferEnough && 
														 windSpeedDifferEnoughOnNextPeriod);			  
			  if(firstSentenceInTheStory)
				{
				  if(windStrengthStartsToChange)
					{
					  if(eventId == TUULI_HEIKKENEE)
						sentence << POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_HEIKETA_COMPOSITE_PHRASE;
					  else
						sentence << POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_COMPOSITE_PHRASE;
					  sentence << directionSentenceP
							   << windSpeedIntervalSentence(eventStartPeriod,
															DONT_USE_AT_ITS_STRONGEST_PHRASE);
					  appendDecreasingIncreasingInterval = false;
					}
				  else
					{
					  sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
							   << EMPTY_STRING
							   << changeAttributeStr
							   << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_WORD : VOIMISTUVAA_WORD)
							   << directionSentenceP;
					}
				}
			  else
				{
				  useAlkaaHeiketaVoimistuaPhrase = (!smallChange &&
													wind_forecast_period_length(eventPeriod) > 6 && 
													!same_part_of_the_day);
				  bool useAlkaenPhrase(get_period_length(eventPeriod) >= 6);
				  if(useAlkaaHeiketaVoimistuaPhrase)
					{
					  if((eventPeriodItemPrevious && eventPeriodItemPrevious->theWindEvent != MISSING_WIND_EVENT) &&
						 ((eventIdPreviousReported & TUULI_VOIMISTUU && eventId == TUULI_VOIMISTUU) ||
						  (eventIdPreviousReported & TUULI_HEIKKENEE && eventId == TUULI_HEIKKENEE)))
						{
						  sentence << ILTAPAIVALLA_EDELLEEN_HEIKKENEVAA_POHJOISTUULTA
								   << getTimePhrase(eventPeriod, USE_ALKAEN_PHRASE)
								   << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_WORD : VOIMISTUVAA_WORD)
								   << directionSentenceP;
						}
					  else
						{
						  if(windStrengthStartsToChange)
							{
							  sentence << (eventId == TUULI_HEIKKENEE ? ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI_COMPOSITE_PHRASE :
										   ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI_COMPOSITE_PHRASE)
									   << getTimePhrase(eventPeriod, useAlkaenPhrase)
									   << directionSentence
									   << EMPTY_STRING;
							  appendDecreasingIncreasingInterval = false;
							}
						  else
							{
							  sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
									   << getTimePhrase(eventPeriod, useAlkaenPhrase)
									   << changeAttributeStr
									   << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_WORD : VOIMISTUVAA_WORD)
									   << directionSentenceP;
							}
						}
					}
				  else
					{
					  if((eventPeriodItemPrevious && eventPeriodItemPrevious->theWindEvent != MISSING_WIND_EVENT) &&
						 ((eventIdPreviousReported & TUULI_VOIMISTUU && eventId == TUULI_VOIMISTUU) ||
						  (eventIdPreviousReported & TUULI_HEIKKENEE && eventId == TUULI_HEIKKENEE)))
						{
						  sentence << ILTAPAIVALLA_EDELLEEN_HEIKKENEVAA_POHJOISTUULTA
								   << getTimePhrase(eventPeriod, useAlkaenPhrase)
								   << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_WORD : VOIMISTUVAA_WORD)
								   << directionSentenceP;
						}
					  else
						{
						  if(windStrengthStartsToChange)
							{
							  sentence << (eventId == TUULI_HEIKKENEE ? ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI_COMPOSITE_PHRASE :
										   ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI_COMPOSITE_PHRASE)
									   << getTimePhrase(eventPeriod, useAlkaenPhrase)
									   << directionSentence
									   << EMPTY_STRING;
							  appendDecreasingIncreasingInterval = false;
							}
						  else
							{
							  sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
									   << getTimePhrase(eventPeriod, useAlkaenPhrase)
									   << changeAttributeStr
									   << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_WORD : VOIMISTUVAA_WORD)
									   << directionSentenceP;
							}
						}
					}
				}
			  
			  if(appendDecreasingIncreasingInterval && (eventPeriodItem->theWindSpeedChangeEnds || reportSpeedEvenIfSpeedChangeContinues))
				sentence << decreasingIncreasingInterval(*eventPeriodItem,
														 firstSentenceInTheStory,
														 eventId);

			  previousWindDirectionId = windDirectionIdAvg;
			}
			break;
		  case TUULI_TYYNTYY:
			{
			  sentence << TUULI_TYYNTYY_PHRASE;
			}
			break;
		  case TUULI_KAANTYY:
			{			  
			  sentence << windDirectionChangeSentence(theParameters,
													  eventPeriod,
													  firstSentenceInTheStory,
													  eventIdPreviousReported,
													  previousWindDirectionId);
			}
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI:
			{
			  WeatherPeriod actualEventPeriod(eventPeriod);
			  // report together
			  if(eventPeriodItemNext && eventPeriodNextIsMissingEvent &&
				 get_wind_direction_id_at(theParameters.theWindDataVector,
										  theParameters.theArea,
										  theParameters.theSources,
										  eventPeriodItemNext->thePeriod,
										  theParameters.theVar) == VAIHTELEVA)
				actualEventPeriod = WeatherPeriod(eventPeriod.localStartTime(), 
												  eventPeriodItemNext->thePeriod.localEndTime());
			  
			  if(firstSentenceInTheStory)
				{
				  sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
						   << EMPTY_STRING
						   << windDirectionSentence(VAIHTELEVA)
						   << windSpeedIntervalSentence(actualEventPeriod,
														USE_AT_ITS_STRONGEST_PHRASE);

				}
			  else
				{
				  if(eventPeriodItem->theTransientDirectionChangeFlag)
					{
					  sentence << ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE
							   << getTimePhrase(eventStartPeriod, DONT_USE_ALKAEN_PHRASE);
					}
				  else
					{

					  if(get_period_length(actualEventPeriod) >= 6)
						{
						  sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
								   << getTimePhrase(eventStartPeriod, USE_ALKAEN_PHRASE)
								   << windDirectionSentence(VAIHTELEVA);
						}
					  else
						{
						  sentence << ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
								   << getTimePhrase(eventStartPeriod, DONT_USE_ALKAEN_PHRASE);
						}
					}
				}
			  previousWindDirectionId = VAIHTELEVA;
			}
			break;
		  case TUULI_KAANTYY_JA_VOIMISTUU:
		  case TUULI_KAANTYY_JA_HEIKKENEE:
			{
			  bool appendDecreasingIncreasingInterval(true);

			  sentence << windDirectionAndSpeedChangesSentence(theParameters,
															   eventPeriod,
															   firstSentenceInTheStory,
															   eventIdPreviousReported,
															   eventId,
															   previousWindDirectionId,
															   eventPeriodItem->theWindSpeedChangeStarts,
															   eventPeriodItem->theWindSpeedChangeEnds,
															   appendDecreasingIncreasingInterval);

			  if(!sentence.empty())
				{
				  bool reportSpeedEvenIfSpeedChangeContinues(!eventPeriodItem->theWindSpeedChangeEnds && 
															 windSpeedDifferEnough && 
															 windSpeedDifferEnoughOnNextPeriod);

				  if(appendDecreasingIncreasingInterval && (eventPeriodItem->theWindSpeedChangeEnds || reportSpeedEvenIfSpeedChangeContinues))
					{
					  sentence << decreasingIncreasingInterval(*eventPeriodItem,
															   firstSentenceInTheStory,
															   eventId);
					}
				}
			}
			break;
		  case TUULI_KAANTYY_JA_TYYNTYY:
			{
			  //cout << "TUULI_KAANTYY_JA_TYYNTYY NOT ALLOWED!!" << endl;
			}
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE:
			{
			  if(eventIdPreviousReported & TUULI_HEIKKENEE)
				{
				  if(eventPeriodItem->theTransientDirectionChangeFlag)
					{
					  sentence << ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE
							   << getTimePhrase(eventStartPeriod, DONT_USE_ALKAEN_PHRASE);
					}
				  else
					{
					  sentence << ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE
							   << getTimePhrase(eventStartPeriod, DONT_USE_ALKAEN_PHRASE);
					}
				}
			  else
				{
				  if(firstSentenceInTheStory)
					{
					  if(eventPeriodItem->theTransientDirectionChangeFlag)
						{
						  sentence << ILTAPAIVALLA_POHJOISTUULI_HEIKKENEE_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE
								   << EMPTY_STRING
								   << windDirectionSentence(windDirectionIdBeg, true);
						}
					  else
						{
						  sentence << ILTAPAIVALLA_POHJOISTUULI_HEIKKENEE_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
								   << EMPTY_STRING
								   << windDirectionSentence(windDirectionIdBeg, true);
					    }
					}
				  else
					{
					  if(eventPeriodItem->theTransientDirectionChangeFlag)
						{
						  sentence << ILTAPAIVALLA_TUULI_HEIKKENEE_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE;
						}
					  else
						{
						  sentence << ILTAPAIVALLA_TUULI_HEIKKENEE_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE;
						}

					  sentence << getTimePhrase(eventStartPeriod, !firstSentenceInTheStory);
					}
				}

			  sentence << decreasingIncreasingInterval(*eventPeriodItem,
													   firstSentenceInTheStory,
													   eventId);
			  previousWindDirectionId = VAIHTELEVA;
			}
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU:
			{
			  if(eventIdPreviousReported == TUULI_VOIMISTUU)
				{
				  if(eventPeriodItem->theTransientDirectionChangeFlag)
					{
					  sentence << ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_JA_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE
							   << getTimePhrase(eventStartPeriod, !firstSentenceInTheStory);
					}
				  else
					{
					  sentence << ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE
							   << getTimePhrase(eventStartPeriod, !firstSentenceInTheStory);
					}
				}
			  else
				{
				  if(firstSentenceInTheStory)
					{
					  if(eventPeriodItem->theTransientDirectionChangeFlag)
						{						  
						  sentence << ILTAPAIVALLA_POHJOISTUULI_VOIMISTUU_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE
								   << EMPTY_STRING
								   << windDirectionSentence(windDirectionIdBeg, true);
						}
					  else
						{
						  sentence << ILTAPAIVALLA_POHJOISTUULI_VOIMISTUU_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
								   << EMPTY_STRING
								   << windDirectionSentence(windDirectionIdBeg, true);
						}
					}
				  else
					{
					  if(eventPeriodItem->theTransientDirectionChangeFlag)
						{
						  sentence << ILTAPAIVALLA_TUULI_VOIMISTUU_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE;
						}
					  else
						{
						  sentence << ILTAPAIVALLA_TUULI_VOIMISTUU_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE;
						}
					  sentence << getTimePhrase(eventStartPeriod, !firstSentenceInTheStory);
					}
				}

			  sentence << decreasingIncreasingInterval(*eventPeriodItem,
													   firstSentenceInTheStory,
													   eventId);
			  previousWindDirectionId = VAIHTELEVA;
			}
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY:
			{
			  //cout << "TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY NOT ALLOWED!!" << endl;
			}
			break;
		  case MISSING_WIND_EVENT:
			{
			  if(firstSentenceInTheStory)
				{
				  if(is_gusty_wind(theParameters,
								   eventPeriod,
								   theParameters.theVar))
					sentence << PUUSKITTAISTA_ETELATUULTA_COMPOSITE_PHRASE;
				  
				  sentence << windDirectionSentence(windDirectionIdAvg);
				  sentence << windSpeedIntervalSentence(eventPeriod, 
														USE_AT_ITS_STRONGEST_PHRASE);			  
				}
			  else if(eventPeriodItem->theWeakWindPeriodFlag ||
					  eventIdPreviousReported & TUULI_MUUTTUU_VAIHTELEVAKSI)
				{
				  WeatherPeriod period(weatherPeriodPreviousReported.localEndTime(),
									   eventPeriod.localEndTime());
				  bool windSpeedDifferEnough = wind_speed_differ_enough(theParameters.theSources,
																		theParameters.theArea,
																		period,
																		theParameters.theVar,
																		theParameters.theWindDataVector);

				  bool useAlkaenPhrase(get_period_length(eventPeriod) >= 6);

				  if(previousWindDirectionId == windDirectionIdAvg)
					{
					  if(windSpeedDifferEnough)
						sentence << getTimePhrase(eventPeriod, useAlkaenPhrase);
					}
				  else
					{
					  sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
							   << getTimePhrase(eventPeriod, useAlkaenPhrase)
							   << windDirectionSentence(windDirectionIdAvg);
					}

				  // report the speed if it differs from what has been reported
				  // in the end of the previous period
				  if(windSpeedDifferEnough)
					sentence << windSpeedIntervalSentence(eventPeriod, 
														  USE_AT_ITS_STRONGEST_PHRASE);
				}
			  else if((eventIdPreviousReported & TUULI_MUUTTUU_VAIHTELEVAKSI) && 
					  i < theParameters.theMergedWindEventPeriodVector.size() - 1 &&
					  get_period_length(eventPeriod) > 2)
				{
				  sentence << windDirectionChangeSentence(theParameters,
														  eventPeriod,
														  firstSentenceInTheStory,
														  eventIdPreviousReported,
														  previousWindDirectionId);
				}
			  if(!sentence.empty())
				previousWindDirectionId = windDirectionIdAvg;
			}
			break;
		  default:
			break;
		  };

		if(!sentence.empty())
		  {
			eventIdPreviousReported = eventId;
			weatherPeriodPreviousReported = eventPeriod;
			eventPeriodItemPrevious = eventPeriodItem;
		  }

		paragraph << sentence;

	  } // for


	return paragraph;
  }

  Sentence WindForecast::windDirectionChangeSentence(const wo_story_params& theParameters,
													 const WeatherPeriod& eventPeriod,
													 const bool& firstSentenceInTheStory,
													 const WindEventId& eventIdPreviousReported,
													 WindDirectionId& previousWindDirectionId) const
  {
	Sentence sentence;

	bool useAlkaenPhrase(get_period_length(eventPeriod) >= 6);
	WindDirectionId windDirectionIdBeg(MISSING_WIND_DIRECTION_ID);
	WindDirectionId windDirectionIdEnd(MISSING_WIND_DIRECTION_ID);

	getWindDirectionBegEnd(eventPeriod, windDirectionIdBeg, windDirectionIdEnd);

	if(firstSentenceInTheStory)
	  {
		if(windDirectionIdBeg == VAIHTELEVA ||
		   windDirectionIdBeg == MISSING_WIND_DIRECTION_ID ||
		   previousWindDirectionId == VAIHTELEVA)
		  {
			sentence << directedWindSentenceAfterVaryingWind(theParameters,
															 eventPeriod,
															 firstSentenceInTheStory,
															 previousWindDirectionId);
		  }
		else
		  {
			sentence << POHJOISTUULTA_INTERVALLI_MS_JOKA_KAANTYY_ILTAPAIVALLA_ETELAAN_COMPOSITE_PHRASE
					 << windDirectionSentence(windDirectionIdBeg)
					 << windSpeedIntervalSentence(eventPeriod, 
												  DONT_USE_AT_ITS_STRONGEST_PHRASE)
					 << getTimePhrase(eventPeriod, useAlkaenPhrase)
					 << getWindDirectionTurntoString(windDirectionIdEnd);
			previousWindDirectionId = windDirectionIdEnd;
		  }
	  }
	else
	  {
		if(previousWindDirectionId == VAIHTELEVA ||
		   eventIdPreviousReported & TUULI_MUUTTUU_VAIHTELEVAKSI)
		  {
			sentence << directedWindSentenceAfterVaryingWind(theParameters,
															 eventPeriod,
															 firstSentenceInTheStory,
															 previousWindDirectionId);
		  }
		else if(windDirectionIdEnd != previousWindDirectionId)
		  {
			sentence << ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE
					 << getTimePhrase(eventPeriod, useAlkaenPhrase)
					 << getWindDirectionTurntoString(windDirectionIdEnd);
			previousWindDirectionId = windDirectionIdEnd;
		  }
	  }

	return sentence;
  }

  Sentence WindForecast::windDirectionAndSpeedChangesSentence(const wo_story_params& theParameters,
															  const WeatherPeriod& eventPeriod,
															  const bool& firstSentenceInTheStory,
															  const WindEventId& previousWindEventId,
															  const WindEventId& currentWindEventId,
															  WindDirectionId& previousWindDirectionId,
															  const bool& theWindSpeedChangeStarts,
															  const bool& theWindSpeedChangeEnds,
															  bool& appendDecreasingIncreasingInterval) const
  {
	Sentence sentence;
	
	WindDirectionId windDirectionIdBeg(MISSING_WIND_DIRECTION_ID);
	WindDirectionId windDirectionIdEnd(MISSING_WIND_DIRECTION_ID);
	getWindDirectionBegEnd(eventPeriod, windDirectionIdBeg, windDirectionIdEnd);
	WindDirectionId windDirectionIdAvg(get_wind_direction_id_at(theParameters.theWindDataVector,
																theParameters.theArea,
																theParameters.theSources,
																eventPeriod,
																theParameters.theVar));

	bool smallChange(false);
	bool gradualChange(false);
	bool fastChange(false);
	std::string changeAttributeStr(EMPTY_STRING);
	bool windSpeedChangeAttribute(getWindSpeedChangeAttribute(eventPeriod, changeAttributeStr,
															  smallChange, gradualChange, fastChange));

	if(!windSpeedChangeAttribute)
	  {
		return sentence;
	  }

	bool useAlkaenPhrase(get_period_length(eventPeriod) >= 6);
	bool strengtheningWind =  (currentWindEventId & TUULI_VOIMISTUU);

	if(previousWindEventId & TUULI_VOIMISTUU || previousWindEventId & TUULI_HEIKKENEE)
	  {
		if(previousWindDirectionId == VAIHTELEVA)
		  {
			if(theWindSpeedChangeStarts &&
			   !theWindSpeedChangeEnds)
			  {
				sentence << (currentWindEventId == TUULI_HEIKKENEE ? ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI_COMPOSITE_PHRASE :
							 ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI_COMPOSITE_PHRASE)
						 << getTimePhrase(eventPeriod, useAlkaenPhrase)
						 << windDirectionSentence(windDirectionIdAvg, true)
						 << EMPTY_STRING;
				appendDecreasingIncreasingInterval = false;
				previousWindDirectionId = windDirectionIdAvg;
			  }
			else
			  {
				if((previousWindEventId & TUULI_HEIKKENEE && currentWindEventId & TUULI_HEIKKENEE) ||
				   (previousWindEventId & TUULI_VOIMISTUU && currentWindEventId & TUULI_VOIMISTUU))
				  {
					sentence << ILTAPAIVALLA_EDELLEEN_HEIKKENEVAA_POHJOISTUULTA
							 << getTimePhrase(eventPeriod, useAlkaenPhrase)
							 << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
							 << windDirectionSentence(windDirectionIdAvg);
					previousWindDirectionId = windDirectionIdAvg;
				  }
				else
				  {
					if(strengtheningWind)
					  sentence << ILTAPAIVALLA_ETELATUULI_VOIMISTUU_NOPEASTI_COMPOSITE_PHRASE;
					else
					  sentence << ILTAPAIVALLA_ETELATUULI_HEIKKENEE_NOPEASTI_COMPOSITE_PHRASE;
					sentence  << getTimePhrase(eventPeriod, useAlkaenPhrase)
							  << windDirectionSentence(windDirectionIdEnd, true)
							  << changeAttributeStr;
					previousWindDirectionId = windDirectionIdEnd;
				  }
			  }
		  }
		else
		  {
			if(previousWindDirectionId == windDirectionIdEnd)
			  {
				if(previousWindEventId & TUULI_VOIMISTUU)
				  {
					if(currentWindEventId & TUULI_VOIMISTUU)
					  {
						sentence << ILTAPAIVALLA_POHJOISTUULI_VOIMISTUU_EDELLEEN
								 << getTimePhrase(eventPeriod, useAlkaenPhrase)
								 << TUULI_WORD;
					  }
					else
					  {
						if(theWindSpeedChangeStarts &&
						   !theWindSpeedChangeEnds)
						  {
							sentence << ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI_COMPOSITE_PHRASE
									 << getTimePhrase(eventPeriod, useAlkaenPhrase)
									 << TUULI_WORD
									 << changeAttributeStr;
							appendDecreasingIncreasingInterval = false;
						  }
						else
						  {
							sentence << ILTAPAIVALLA_ETELATUULI_HEIKKENEE_NOPEASTI_COMPOSITE_PHRASE
									 << getTimePhrase(eventPeriod, useAlkaenPhrase)
									 << TUULI_WORD
									 << changeAttributeStr;
						  }
					  }
				  }
				else
				  {
					if(currentWindEventId & TUULI_HEIKKENEE)
					  {
						sentence << ILTAPAIVALLA_POHJOISTUULI_HEIKKENEE_EDELLEEN
								 << getTimePhrase(eventPeriod, useAlkaenPhrase)
								 << TUULI_WORD;
					  }
					else 
					  {
						if(theWindSpeedChangeStarts &&
						   !theWindSpeedChangeEnds)
						  {
							sentence << ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI_COMPOSITE_PHRASE
									 << getTimePhrase(eventPeriod, useAlkaenPhrase)
									 << TUULI_WORD
									 << changeAttributeStr;
							appendDecreasingIncreasingInterval = false;
						  }
						else
						  {
							sentence << ILTAPAIVALLA_ETELATUULI_VOIMISTUU_NOPEASTI_COMPOSITE_PHRASE
									 << getTimePhrase(eventPeriod, useAlkaenPhrase)
									 << TUULI_WORD
									 << changeAttributeStr;
						  }
					  }
				  }
			  }
			else
			  {
				bool changeStarts(false);

				if(previousWindEventId & TUULI_VOIMISTUU)
				  {
					if(currentWindEventId & TUULI_VOIMISTUU)
					  {
						sentence << ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_JA_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE;
					  }
					else
					  {
						if(theWindSpeedChangeStarts &&
						   !theWindSpeedChangeEnds)
						  {
							sentence << ILTAPAIVALLA_TUULI_ALKAA_HEIKETA_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE;
							changeStarts = true;
						  }
						else
						  {
							sentence << ILTAPAIVALLA_TUULI_HEIKKENEE_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE;
						  }
					  }
				  }
				else
				  {
					if(currentWindEventId & TUULI_HEIKKENEE)
					  {
						sentence << ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE;
					  }
					else 
					  {
						if(theWindSpeedChangeStarts &&
						   !theWindSpeedChangeEnds)
						  {
							sentence << ILTAPAIVALLA_TUULI_ALKAA_VOIMISTUA_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE;
							changeStarts = true;
						  }
						else
						  {
							sentence << ILTAPAIVALLA_TUULI_VOIMISTUU_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE;
						  }
					  }
				  }

				if(changeStarts)
				  {
					sentence << getTimePhrase(eventPeriod, useAlkaenPhrase)
							 << windDirectionSentence(windDirectionIdBeg, true)
							 << getWindDirectionTurntoString(windDirectionIdEnd);
					appendDecreasingIncreasingInterval = false;
				  }
				else
				  {
					sentence << getTimePhrase(eventPeriod, useAlkaenPhrase)
							 << getWindDirectionTurntoString(windDirectionIdEnd);
				  }
			  }
			previousWindDirectionId = windDirectionIdEnd;
		  }
	  }
	else
	  {
		Sentence timePhrase;
		if(firstSentenceInTheStory)
		  timePhrase << EMPTY_STRING;
		else
		  timePhrase << getTimePhrase(eventPeriod, useAlkaenPhrase);

		NFmiTime begTimePlus1(eventPeriod.localStartTime());
		if(get_period_length(eventPeriod) > 0)
		  begTimePlus1.ChangeByHours(1);
		WindDirectionId windDirectionIdBegPlus1(get_wind_direction_id_at(theParameters.theWindDataVector,
																		 theParameters.theArea,
																		 begTimePlus1,
																		 theParameters.theVar));
		WindDirectionId windDirectionIdAvgPlus1(get_wind_direction_id_at(theParameters.theWindDataVector,
																		 theParameters.theArea,
																		 theParameters.theSources,
																		 WeatherPeriod(begTimePlus1, eventPeriod.localEndTime()),
																		 theParameters.theVar));

		if(firstSentenceInTheStory)
		  {
			if(windDirectionIdBeg == VAIHTELEVA ||
			   windDirectionIdBeg == MISSING_WIND_DIRECTION_ID)
			  {
				bool directionTurns = windDirectionTurns(WeatherPeriod(begTimePlus1, eventPeriod.localEndTime()));
				if(windDirectionIdBegPlus1 == windDirectionIdEnd || !directionTurns)
				  {
					if(theWindSpeedChangeStarts &&
					   !theWindSpeedChangeEnds)
					  {
						if(strengtheningWind)
						  sentence << ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI_COMPOSITE_PHRASE;
						else
						  sentence << ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI_COMPOSITE_PHRASE;

						sentence << timePhrase
								 << windDirectionSentence(windDirectionIdAvgPlus1)
								 << changeAttributeStr;
					  }
					else
					  {
						if(strengtheningWind)
						  sentence << ILTAPAIVALLA_ETELATUULI_VOIMISTUU_NOPEASTI_COMPOSITE_PHRASE;
						else
						  sentence << ILTAPAIVALLA_ETELATUULI_HEIKKENEE_NOPEASTI_COMPOSITE_PHRASE;
						sentence  << getTimePhrase(eventPeriod, useAlkaenPhrase)
								  << windDirectionSentence(windDirectionIdAvgPlus1, true)
								  << changeAttributeStr;
					  }
					previousWindDirectionId = windDirectionIdAvgPlus1;
				  }
				else
				  {
					if(theWindSpeedChangeStarts &&
					   !theWindSpeedChangeEnds)
					  {
						if(strengtheningWind)
						  sentence << ILTAPAIVALLA_POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_NOPEASTI_JA_KAANTYY_ILLALLA_ETELAAN_COMPOSITE_PHRASE;
						else
						  sentence << ILTAPAIVALLA_POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_HEIKETA_NOPEASTI_JA_KAANTYY_ILLALLA_ETELAAN_COMPOSITE_PHRASE;

						sentence << timePhrase
								 << windDirectionSentence(windDirectionIdBegPlus1)
								 << windSpeedIntervalSentence(WeatherPeriod(eventPeriod.localStartTime(),eventPeriod.localStartTime()),
															  DONT_USE_AT_ITS_STRONGEST_PHRASE)
								 << (smallChange ? EMPTY_STRING : changeAttributeStr)
								 << EMPTY_STRING
								 << getWindDirectionTurntoString(windDirectionIdEnd);
					  }
					else
					  {
						sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_JOKA_KAANTYY_POHJOISEEN_COMPOSITE_PHRASE
								 << timePhrase
								 << changeAttributeStr
								 << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
								 << windDirectionSentence(windDirectionIdBegPlus1)
								 << getWindDirectionTurntoString(windDirectionIdEnd);
					  }
					previousWindDirectionId = windDirectionIdEnd;
				  }
			  }
			else
			  {
				bool directionTurns = windDirectionTurns(eventPeriod);
				if(directionTurns)
				  {

					WeatherPeriod windTurnignPeriod(get_wind_turning_period(eventPeriod, theParameters.theWindDirectionEventPeriodVector));
					bool useAlkaenPhrase2 = (get_period_length(windTurnignPeriod) >= 6);
					Sentence timePhrase2;
					
					if(useAlkaenPhrase && useAlkaenPhrase2 &&
					   get_part_of_the_day_id(windTurnignPeriod.localStartTime()) == 
					   get_part_of_the_day_id(eventPeriod.localStartTime()))
					  {
						  timePhrase2 << EMPTY_STRING;
					  }
					else
					  {
						timePhrase2 << getTimePhrase(windTurnignPeriod, useAlkaenPhrase2);
					  }

					if(theWindSpeedChangeStarts &&
					   !theWindSpeedChangeEnds)
					  {
						if(strengtheningWind)
						  sentence << ILTAPAIVALLA_POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_NOPEASTI_JA_KAANTYY_ILLALLA_ETELAAN_COMPOSITE_PHRASE;
						else
						  sentence << ILTAPAIVALLA_POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_HEIKETA_NOPEASTI_JA_KAANTYY_ILLALLA_ETELAAN_COMPOSITE_PHRASE;

						sentence << timePhrase
								 << windDirectionSentence(windDirectionIdBeg)
								 << windSpeedIntervalSentence(WeatherPeriod(eventPeriod.localStartTime(),eventPeriod.localStartTime()),
															  DONT_USE_AT_ITS_STRONGEST_PHRASE)					  
								 << changeAttributeStr
								 << timePhrase2
								 << getWindDirectionTurntoString(windDirectionIdEnd);
					  }
					else
					  {
						sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_JOKA_KAANTYY_ILLALLA_POHJOISEEN_COMPOSITE_PHRASE
								 << timePhrase
								 << changeAttributeStr
								 << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
								 << windDirectionSentence(windDirectionIdBeg)
								 << timePhrase2
								 << getWindDirectionTurntoString(windDirectionIdEnd);
					  }
					previousWindDirectionId = windDirectionIdEnd;
				  }
				else
				  {
					if(theWindSpeedChangeStarts &&
					   !theWindSpeedChangeEnds)
					  {
						if(strengtheningWind)
						  sentence << ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI_COMPOSITE_PHRASE;
						else
						  sentence << ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI_COMPOSITE_PHRASE;

						sentence << timePhrase
								 << windDirectionSentence(windDirectionIdAvg)
								 << changeAttributeStr;
					  }
					else
					  {
						sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
								 << timePhrase
								 << changeAttributeStr
								 << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
								 << windDirectionSentence(windDirectionIdAvg);
					  }
					previousWindDirectionId = windDirectionIdAvg;
				  }
			  }
		  }
		else // not first sentence 
		  {
			if(previousWindEventId & TUULI_MUUTTUU_VAIHTELEVAKSI ||
			   previousWindDirectionId == VAIHTELEVA)
			  {
				// direction doesn't turn if turn is puolienen -> suuntainen
				bool directionTurns = windDirectionTurns(WeatherPeriod(begTimePlus1, eventPeriod.localEndTime()));				
				if(windDirectionIdBegPlus1 == windDirectionIdEnd || !directionTurns)
				  {
					if(theWindSpeedChangeStarts &&
					   !theWindSpeedChangeEnds)
					  {
						if(strengtheningWind)
						  sentence << ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI_COMPOSITE_PHRASE;
						else
						  sentence << ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI_COMPOSITE_PHRASE;

						sentence << timePhrase
								 << windDirectionSentence(windDirectionIdAvgPlus1)
								 << changeAttributeStr;
					  }
					else
					  {
						sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
								 << timePhrase
								 << changeAttributeStr
								 << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
								 << windDirectionSentence(windDirectionIdAvgPlus1);
					  }
					previousWindDirectionId = windDirectionIdAvgPlus1;
				  }
				else
				  {
					WeatherPeriod windTurnignPeriod(get_wind_turning_period(eventPeriod, theParameters.theWindDirectionEventPeriodVector));
					bool useAlkaenPhrase2 = (get_period_length(windTurnignPeriod) >= 6);
					Sentence timePhrase2;
					
					if(useAlkaenPhrase && useAlkaenPhrase2 &&
					   get_part_of_the_day_id(windTurnignPeriod.localStartTime()) == 
					   get_part_of_the_day_id(eventPeriod.localStartTime()))
					  {
						  timePhrase2 << EMPTY_STRING;
					  }
					else
					  {
						timePhrase2 << getTimePhrase(windTurnignPeriod, useAlkaenPhrase2);
					  }

					if(theWindSpeedChangeStarts &&
					   !theWindSpeedChangeEnds)
					  {
						if(strengtheningWind)
						  sentence << ILTAPAIVALLA_POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_VOIMISTUA_NOPEASTI_JA_KAANTYY_ILLALLA_ETELAAN_COMPOSITE_PHRASE;
						else
						  sentence << ILTAPAIVALLA_POHJOISTUULTA_INTERVALLI_MS_JOKA_ALKAA_HEIKETA_NOPEASTI_JA_KAANTYY_ILLALLA_ETELAAN_COMPOSITE_PHRASE;

						sentence << timePhrase
								 << windDirectionSentence(windDirectionIdBegPlus1)
								 << windSpeedIntervalSentence(WeatherPeriod(eventPeriod.localStartTime(),eventPeriod.localStartTime()),
															  DONT_USE_AT_ITS_STRONGEST_PHRASE)					  
								 << changeAttributeStr
								 << timePhrase2
								 << getWindDirectionTurntoString(windDirectionIdEnd);
					  }
					else
					  {
						sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_JOKA_KAANTYY_ILLALLA_POHJOISEEN_COMPOSITE_PHRASE
								 << timePhrase
								 << changeAttributeStr
								 << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
								 << windDirectionSentence(windDirectionIdBegPlus1)
								 << timePhrase2
								 << getWindDirectionTurntoString(windDirectionIdEnd);
					  }
					previousWindDirectionId = windDirectionIdEnd;
				  }
			  }
			else
			  {
				bool directionTurns = windDirectionTurns(eventPeriod);
				if(directionTurns)
				  {
					WeatherPeriod windTurnignPeriod(get_wind_turning_period(eventPeriod, theParameters.theWindDirectionEventPeriodVector));
					bool useAlkaenPhrase2 = (get_period_length(windTurnignPeriod) >= 6);
					Sentence timePhrase2;
					
					if(useAlkaenPhrase && useAlkaenPhrase2 &&
					   get_part_of_the_day_id(windTurnignPeriod.localStartTime()) == 
					   get_part_of_the_day_id(eventPeriod.localStartTime()))
					  {
						  timePhrase2 << EMPTY_STRING;
					  }
					else
					  {
						timePhrase2 << getTimePhrase(windTurnignPeriod, useAlkaenPhrase2);
					  }

					sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_JOKA_KAANTYY_ILLALLA_POHJOISEEN_COMPOSITE_PHRASE
							 << timePhrase
							 << changeAttributeStr
							 << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
							 << windDirectionSentence(windDirectionIdBeg)
							 << timePhrase2
							 << getWindDirectionTurntoString(windDirectionIdEnd);
					previousWindDirectionId = windDirectionIdEnd;
				  }
				else
				  {
					sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
							 << timePhrase
							 << changeAttributeStr
							 << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
							 << windDirectionSentence(windDirectionIdAvg);
					previousWindDirectionId = windDirectionIdAvg;
				  }
			  }
		  }

	  }

	return sentence;
  }

  Sentence WindForecast::directedWindSentenceAfterVaryingWind(const wo_story_params& theParameters,
															  const WeatherPeriod& eventPeriod,
															  const bool& firstSentenceInTheStory,
															  WindDirectionId& previousWindDirectionId) const
  {
	Sentence sentence;

	bool useAlkaenPhrase(get_period_length(eventPeriod) >= 6);
	WeatherPeriod periodStart(eventPeriod.localStartTime(),
							  eventPeriod.localStartTime());

	WindDirectionId windDirectionIdAvg(get_wind_direction_id_at(theParameters.theWindDataVector,
																theParameters.theArea,
																theParameters.theSources,
																eventPeriod,
																theParameters.theVar));

	if(firstSentenceInTheStory)
	  {
		sentence << windDirectionSentence(windDirectionIdAvg);
		sentence << windSpeedIntervalSentence(eventPeriod, 
											  USE_AT_ITS_STRONGEST_PHRASE);
	  }
	else
	  {
		if(useAlkaenPhrase)
		  sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
				   << getTimePhrase(periodStart, USE_ALKAEN_PHRASE)
				   << windDirectionSentence(windDirectionIdAvg);
		else
		  sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
				   << getTimePhrase(eventPeriod, DONT_USE_ALKAEN_PHRASE)
				   << windDirectionSentence(windDirectionIdAvg);
	  }

	previousWindDirectionId = windDirectionIdAvg;
	
	return sentence;
  }

  void WindForecast::logWindSpeedPeriod(const WeatherPeriod& period, 
										const Sentence& intervalSentence) const
  {
	theParameters.theLog << "windspeed reporting period: ";
	theParameters.theLog << period;
	theParameters.theLog << ": ";
	theParameters.theLog << intervalSentence;
  }

  Sentence WindForecast::getWindDirectionSentence(const WindDirectionId& theWindDirectionId) const
  {
	Sentence sentence;
	return sentence;
	//	return windDirectionSentence(theWindDirectionId, false);
  }

  Sentence WindForecast::decreasingIncreasingInterval(const WindEventPeriodDataItem& eventPeriodDataItem,
													  const bool& firstSentenceInTheStory,
													  const WindEventId& eventId) const
  {
	Sentence sentence;

	WeatherPeriod speedEventPeriod(eventPeriodDataItem.theWindSpeedChangePeriod);
	int periodLength(wind_forecast_period_length(speedEventPeriod));
	WeatherPeriod periodBeg(speedEventPeriod.localStartTime(), 
							speedEventPeriod.localStartTime());
	WeatherPeriod periodEnd(speedEventPeriod.localEndTime(), 
							speedEventPeriod.localEndTime());

	NFmiTime phrasePeriodEndTimestamp(get_phrase_period_end_timestamp(speedEventPeriod.localEndTime()));
	WeatherPeriod periodEndPhrasePeriod(phrasePeriodEndTimestamp, phrasePeriodEndTimestamp);

	bool useTimePhrase = !fit_into_large_day_part(speedEventPeriod);

	bool windIsDecreasing =  (eventId & TUULI_HEIKKENEE);

	if(!wind_speed_differ_enough(theParameters.theSources,
								 theParameters.theArea,
								 speedEventPeriod,
								 theParameters.theVar,
								 theParameters.theWindDataVector))
	  {
		if(useTimePhrase && !firstSentenceInTheStory)
		  {
		
			sentence  << Delimiter(COMMA_PUNCTUATION_MARK)
					  << getTimePhrase(periodEndPhrasePeriod, eventPeriodDataItem.theLongTermSpeedChangeFlag)
					  << windSpeedIntervalSentence(periodEnd,
												   USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
			logWindSpeedPeriod(periodEnd, windSpeedIntervalSentence(periodEnd, 
																	USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing));
		  }
		else
		  {
			if(firstSentenceInTheStory)
			  {
				sentence << windSpeedIntervalSentence(speedEventPeriod,
													  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
				logWindSpeedPeriod(speedEventPeriod, windSpeedIntervalSentence(speedEventPeriod, 
																			   USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing));
			  }
			else
			  {
				sentence << windSpeedIntervalSentence(periodEnd,
													  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
				logWindSpeedPeriod(periodEnd, windSpeedIntervalSentence(periodEnd, 
																		USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing));
			  }
		  }

		return sentence;
	  }	

	if(get_period_length(speedEventPeriod) <= 2)
	  {
		sentence << windSpeedIntervalSentence(periodEnd,
											  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
		return sentence;
	  }

	vector<WeatherPeriod> speedChangePeriods = getWindSpeedReportingPoints(eventPeriodDataItem,
																		   firstSentenceInTheStory,
																		   eventId);
	
	if(speedChangePeriods.size() <= 1)
	  {

		if(firstSentenceInTheStory)
		  {
			sentence  << Delimiter(COMMA_PUNCTUATION_MARK)
					  << ALUKSI_WORD
					  << windSpeedIntervalSentence(periodBeg, 
												   DONT_USE_AT_ITS_STRONGEST_PHRASE);

			sentence << Delimiter(COMMA_PUNCTUATION_MARK)
					 << getTimePhrase(periodEndPhrasePeriod, eventPeriodDataItem.theLongTermSpeedChangeFlag)
					 << windSpeedIntervalSentence(periodEnd, 
												  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);

			logWindSpeedPeriod(periodBeg,  windSpeedIntervalSentence(periodBeg, 
																	 DONT_USE_AT_ITS_STRONGEST_PHRASE));
			logWindSpeedPeriod(periodBeg,  windSpeedIntervalSentence(periodEnd, 
																	 USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing));
		  }
		else
		  {
			if(periodLength <= 8 &&
			   get_adjusted_part_of_the_day_id(speedEventPeriod) ==
			   get_part_of_the_day_id(speedEventPeriod.localEndTime()))
			  {
				sentence << windSpeedIntervalSentence(periodEnd, 
													  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
				logWindSpeedPeriod(periodBeg, sentence);
			  }
			else
			  {
				std::string dayPhasePhraseWholePeriod;
				std::string dayPhasePhrasePeriodEnd;
				get_time_phrase_large(speedEventPeriod,
									  false,
									  theParameters.theVar, 
									  dayPhasePhraseWholePeriod,
									  false);

				get_time_phrase_large(periodEnd,
									  false,
									  theParameters.theVar, 
									  dayPhasePhrasePeriodEnd,
									  false);

				if(dayPhasePhraseWholePeriod == dayPhasePhrasePeriodEnd && 
				   get_adjusted_part_of_the_day_id(speedEventPeriod) == get_part_of_the_day_id_narrow(periodEnd))
				  {
					sentence << windSpeedIntervalSentence(periodEnd,
														  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);

					logWindSpeedPeriod(periodEnd, sentence);
				  }
				else
				  {
					sentence << Delimiter(COMMA_PUNCTUATION_MARK)
							 << getTimePhrase(periodEndPhrasePeriod, eventPeriodDataItem.theLongTermSpeedChangeFlag)
							 << windSpeedIntervalSentence(periodEnd,
														  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);

					logWindSpeedPeriod(periodEnd, windSpeedIntervalSentence(periodEnd,
																	USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing));
				  }
			  }
		  }
	  }
	else
	  {
		if(firstSentenceInTheStory)
		  {
			sentence  << Delimiter(COMMA_PUNCTUATION_MARK)
					  << ALUKSI_WORD
					  << windSpeedIntervalSentence(periodBeg,
												   USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
			useTimePhrase = true;
			logWindSpeedPeriod(periodBeg, windSpeedIntervalSentence(periodBeg,
																	USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing));
		  }
		
		// Note! Speed at start is not reported: starting from index 1
		if(!useTimePhrase)
		  useTimePhrase = speedChangePeriods.size() > 2;
		for(unsigned int i = 1; i < speedChangePeriods.size(); i++)
		  {
			WeatherPeriod currentPeriodEnd(speedChangePeriods[i].localEndTime(), 
										   speedChangePeriods[i].localEndTime());
			bool lastPeriod = (i == (speedChangePeriods.size() - 1));
			part_of_the_day_id currentDayPart(get_part_of_the_day_id_narrow(currentPeriodEnd));

			if(!lastPeriod)
			  {
				WeatherPeriod nextPeriodEnd(speedChangePeriods[i+1].localEndTime(), 
											speedChangePeriods[i+1].localEndTime());

				part_of_the_day_id nextDayPart(get_part_of_the_day_id_narrow(nextPeriodEnd));

				// wind speed is mentioned only once per day part, for example we don't say
				// iltapäivällä 10...13 m/s, iltapäivällä 15...18
				if(currentDayPart == nextDayPart)
				  continue;

				int lowerLimit1(kFloatMissing),
				  lowerLimit2(kFloatMissing),
				  upperLimit1(kFloatMissing),
				  upperLimit2(kFloatMissing);

				get_wind_speed_interval(currentPeriodEnd,
										theParameters.theArea,
										theParameters.theWindDataVector,
										lowerLimit1,
										upperLimit1);

				get_wind_speed_interval(nextPeriodEnd,
										theParameters.theArea,
										theParameters.theWindDataVector,
										lowerLimit2,
										upperLimit2);


				if(abs(lowerLimit2 - lowerLimit1) <= 1 && 
				   abs(upperLimit2 - upperLimit1) <= 1)
				  continue;
			  }
		
			Sentence intervalSentence;
			intervalSentence << windSpeedIntervalSentence(lastPeriod ? speedChangePeriods[i] : currentPeriodEnd,
														  lastPeriod &&
														  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);

			logWindSpeedPeriod((lastPeriod ? speedChangePeriods[i] : currentPeriodEnd), intervalSentence);
			
			if(!intervalSentence.empty())
			  {
				if(useTimePhrase)
				  {
					if(i < speedChangePeriods.size() - 1)
					  {
						NFmiTime currentTimestamp(speedChangePeriods[i].localStartTime());
						NFmiTime nextTimestamp(get_phrase_period_end_timestamp(speedChangePeriods[i+1].localStartTime()));
						if(get_part_of_the_day_id(currentTimestamp) == get_part_of_the_day_id(nextTimestamp))
						  continue;
					  }
					
					sentence << Delimiter(COMMA_PUNCTUATION_MARK);
					
					NFmiTime phrasePeriodEndTimestamp(get_phrase_period_end_timestamp(lastPeriod ? speedChangePeriods[i].localStartTime() : currentPeriodEnd.localStartTime()));

					WeatherPeriod phrasePeriod(phrasePeriodEndTimestamp, phrasePeriodEndTimestamp);
					if(lastPeriod)
					  {
						Sentence timePhrase;
						if(get_adjusted_part_of_the_day_id(phrasePeriod) != MISSING_PART_OF_THE_DAY_ID)
						  timePhrase << getTimePhrase(phrasePeriod, eventPeriodDataItem.theLongTermSpeedChangeFlag);
						if(timePhrase.empty())
						  timePhrase << getTimePhrase(phrasePeriod, USE_ALKAEN_PHRASE);
						sentence << timePhrase;
					  }
					else
					  {
						sentence << getTimePhrase(phrasePeriod, DONT_USE_ALKAEN_PHRASE);
					  }
				  }

				sentence << intervalSentence;
			  }
		  } // for(unsigned int i = 1; i < speedChangePeriods.size(); i++)
	  }

	return sentence;
  }

  Sentence WindForecast::reportIntermediateSpeed(const WeatherPeriod& speedEventPeriod) const
  {
	Sentence sentence;

	int periodLength(wind_forecast_period_length(speedEventPeriod));

	NFmiTime intermediateReportTime(speedEventPeriod.localStartTime());
	while((intermediateReportTime.DifferenceInHours(speedEventPeriod.localStartTime()) < periodLength / 2 ||
		   get_part_of_the_day_id(speedEventPeriod.localStartTime()) ==
		   get_part_of_the_day_id(intermediateReportTime)) &&
		  speedEventPeriod.localEndTime().DifferenceInHours(intermediateReportTime) > 6)
	  {
		intermediateReportTime.ChangeByHours(1);
	  }
	if(speedEventPeriod.localEndTime().DifferenceInHours(intermediateReportTime) >= 6)
	  {
		WeatherPeriod intermediatePeriod(intermediateReportTime, intermediateReportTime);
		WeatherPeriod period(intermediateReportTime,
							 speedEventPeriod.localEndTime());
			
		// make sure that the in windspeed difference is big enough
		if(wind_speed_differ_enough(theParameters.theSources,
									theParameters.theArea,
									period,
									theParameters.theVar,
									theParameters.theWindDataVector))
		  {
			WindDirectionId windDirectionIdBeg(MISSING_WIND_DIRECTION_ID);
			WindDirectionId windDirectionIdEnd(MISSING_WIND_DIRECTION_ID);
			getWindDirectionBegEnd(intermediatePeriod, windDirectionIdBeg, windDirectionIdEnd);

			sentence << Delimiter(COMMA_PUNCTUATION_MARK)
					 << getTimePhrase(intermediatePeriod, DONT_USE_ALKAEN_PHRASE)
					 << getWindDirectionSentence(windDirectionIdEnd)
					 << windSpeedIntervalSentence(intermediatePeriod, DONT_USE_AT_ITS_STRONGEST_PHRASE);
		  }
	  }

	return sentence;
  }

  bool WindForecast::samePartOfTheDay(const NFmiTime& time1, const NFmiTime& time2) const
  {
	NFmiTime iterTime(time1);

	while(iterTime < time2 &&
		  get_part_of_the_day_id(iterTime) == get_part_of_the_day_id(time2))
	  {
		iterTime.ChangeByHours(1);
	  }

	return (iterTime == time2);
  }

  void WindForecast::getPeriodStartAndEndIndex(const WeatherPeriod& period,
											   unsigned int& begIndex,
											   unsigned int& endIndex) const
  {
	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		const WindDataItemUnit& windDataItem = (*theParameters.theWindDataVector[i])(theParameters.theArea.type());
		
		if(period.localStartTime() == windDataItem.thePeriod.localStartTime())
		  {
			begIndex = i;
			endIndex = i;
		  }
		else if(period.localEndTime() == windDataItem.thePeriod.localEndTime())
		  {
			endIndex = i;
			break;
		  }
	  }
  }

  Sentence WindForecast::getTimePhrase(const WeatherPeriod thePeriod,
									   const bool& useAlkaenPhrase) const
  {
	Sentence sentence;

	int forecastPeriodLength = get_period_length(theParameters.theForecastPeriod);
			
	bool specifyDay = (thePreviousDayNumber != thePeriod.localStartTime().GetWeekday() &&
					   forecastPeriodLength > 24 &&
					   abs(theParameters.theForecastTime.DifferenceInHours(thePeriod.localStartTime())) > 6 &&
					   theParameters.theForecastTime.GetWeekday() != thePeriod.localStartTime().GetWeekday() &&
					   get_part_of_the_day_id_large(thePeriod) != KESKIYO);

	std::string dayPhasePhrase;

	theParameters.theAlkaenPhraseUsed = (wind_forecast_period_length(thePeriod) >= 6 
										 && useAlkaenPhrase 
										 && !fit_into_narrow_day_part(thePeriod));

	// try to prevent tautology, like "iltapäivällä"... "iltapäivästä alkaen"
	if(thePreviousPartOfTheDay == 
	   get_adjusted_part_of_the_day_id(thePeriod, theParameters.theAlkaenPhraseUsed) &&
	   thePreviousPartOfTheDay != MISSING_PART_OF_THE_DAY_ID &&
	   get_period_length(thePeriod) > 4)
	  {
		NFmiTime startTime(thePeriod.localStartTime());
		startTime.ChangeByHours(2);
		WeatherPeriod shortenedPeriod(startTime, thePeriod.localEndTime());
		theParameters.theAlkaenPhraseUsed = (wind_forecast_period_length(shortenedPeriod) >= 6 
											 && useAlkaenPhrase 
											 && !fit_into_narrow_day_part(shortenedPeriod));
		sentence << get_time_phrase_large(shortenedPeriod,
										  specifyDay,
										  theParameters.theVar, 
										  dayPhasePhrase,
										  useAlkaenPhrase);
		
		if(get_part_of_the_day_id_large(shortenedPeriod) != KESKIYO)
		  thePreviousDayNumber = shortenedPeriod.localStartTime().GetWeekday();
		
		thePreviousPartOfTheDay = get_adjusted_part_of_the_day_id(shortenedPeriod, theParameters.theAlkaenPhraseUsed);
	  }
	else
	  {
		sentence << get_time_phrase_large(thePeriod,
										  specifyDay,
										  theParameters.theVar, 
										  dayPhasePhrase,
										  useAlkaenPhrase);

		if(get_part_of_the_day_id_large(thePeriod) != KESKIYO)
		  thePreviousDayNumber = thePeriod.localStartTime().GetWeekday();
		
		thePreviousPartOfTheDay = get_adjusted_part_of_the_day_id(thePeriod, theParameters.theAlkaenPhraseUsed);
	  }

	return sentence;
  }

  vector<WeatherPeriod> WindForecast::getWindSpeedReportingPoints(const WindEventPeriodDataItem& eventPeriodDataItem,
																  const bool& firstSentenceInTheStory,
																  const WindEventId& eventId) const
  {
	vector<WeatherPeriod> retVector;

	WeatherPeriod speedEventPeriod(eventPeriodDataItem.theWindSpeedChangePeriod);
	// find the point of time when max wind has changed theParameters.theWindSpeedThreshold from the previous
	// take the period from there to the end of the speedEventPeriod
	unsigned int begIndex = 0;
	unsigned int endIndex = 0;

	do
	  {
		const WindDataItemUnit& windDataItem = (*theParameters.theWindDataVector[begIndex])(theParameters.theArea.type());
		if(is_inside(windDataItem.thePeriod.localStartTime(), speedEventPeriod) ||
		   begIndex == theParameters.theWindDataVector.size() - 1)
		  break;
		begIndex++;
	  }
	while(true);

	if(begIndex < theParameters.theWindDataVector.size())
	  {
		vector<unsigned int> reportingIndexes;
		float previousThresholdTopWind(kFloatMissing);
		for(unsigned int i = begIndex; i < theParameters.theWindDataVector.size(); i++)
		  {
			const WindDataItemUnit& windDataItem = (*theParameters.theWindDataVector[i])(theParameters.theArea.type());
			if(!is_inside(windDataItem.thePeriod.localStartTime(), speedEventPeriod))
			  break;
			
			if(i == begIndex)
			  {
				// speed at the beginning of the period is reported
				reportingIndexes.push_back(i);
				previousThresholdTopWind = windDataItem.theEqualizedTopWind.value();
			  }
			else if(abs(windDataItem.theEqualizedTopWind.value() - previousThresholdTopWind) > 4.0 &&
					((round(previousThresholdTopWind) > WEAK_WIND_SPEED_UPPER_LIMIT && (eventId & TUULI_HEIKKENEE)) || 
					 (round(windDataItem.theEqualizedTopWind.value()) > WEAK_WIND_SPEED_UPPER_LIMIT  && (eventId & TUULI_VOIMISTUU))))
			  {
				// speed is reported when it has changed 4.0 from the previous raporting point
				reportingIndexes.push_back(i);
				previousThresholdTopWind = windDataItem.theEqualizedTopWind.value();
			  }			  
			endIndex = i;
		  }

		for(unsigned int i = 0; i < reportingIndexes.size(); i++)
		  {
			unsigned int index = reportingIndexes[i];
			const WindDataItemUnit& windDataItem = (*theParameters.theWindDataVector[index])(theParameters.theArea.type());
			
			if(i < reportingIndexes.size() - 1)
			  {
				retVector.push_back(windDataItem.thePeriod);
			  }
			else
			  {
				const WindDataItemUnit& windDataItemLast = (*theParameters.theWindDataVector[endIndex])(theParameters.theArea.type());
				retVector.push_back(windDataItemLast.thePeriod);
			  }
		  }
	  }

	if(retVector.size() == 0)
	  {
		WeatherPeriod periodEnd(speedEventPeriod.localEndTime(),
								speedEventPeriod.localEndTime());
		retVector.push_back(periodEnd);
		//		retVector.push_back(speedEventPeriod);
	  }

	return retVector;
  }

  // tuuli voi voimistua, heiketä tai kääntyä usealla peräkkäiselä periodilla (käyrä polveilee, mutta ei käänny)
  // tämä funktio etsii oikean käännöskohdan
  WeatherPeriod getActualWindEventPeriod(const wind_event_period_data_item_vector& theWindEventPeriodVector,
										 unsigned int theBegIndex)
  {
	WindEventPeriodDataItem* eventPeriodItem = theWindEventPeriodVector[theBegIndex];
	NFmiTime periodStartTime(eventPeriodItem->thePeriod.localStartTime());
	NFmiTime periodEndTime(eventPeriodItem->thePeriod.localStartTime());
	
	bool firstWindSpeedEvent = (eventPeriodItem->theWindEvent == TUULI_HEIKKENEE ||
								eventPeriodItem->theWindEvent == TUULI_VOIMISTUU ||
								eventPeriodItem->theWindEvent == TUULI_TYYNTYY);


	for(unsigned int i = theBegIndex + 1; i < theWindEventPeriodVector.size(); i++)
	  {
		eventPeriodItem = theWindEventPeriodVector[i];

		bool currentWindSpeedEvent = (eventPeriodItem->theWindEvent == TUULI_HEIKKENEE ||
									  eventPeriodItem->theWindEvent == TUULI_VOIMISTUU ||
									  eventPeriodItem->theWindEvent == TUULI_TYYNTYY);
		
		if(firstWindSpeedEvent == currentWindSpeedEvent)
		  {
			periodEndTime = eventPeriodItem->thePeriod.localStartTime();
			break;
		  }
		periodEndTime = eventPeriodItem->thePeriod.localEndTime();
	  }

	return WeatherPeriod(periodStartTime, periodEndTime);
  }

  bool WindForecast::getWindSpeedChangeAttribute(const WeatherPeriod& changePeriod,
												 std::string& phraseStr,
												 bool& smallChange,
												 bool& gradualChange,
												 bool& fastChange) const
  {
	int begLowerLimit(kFloatMissing), 
	  begUpperLimit(kFloatMissing), 
	  endLowerLimit(kFloatMissing), 
	  endUpperLimit(kFloatMissing);

	get_wind_speed_interval(changePeriod.localStartTime(),
							theParameters.theArea,							
							theParameters.theWindDataVector,
							begLowerLimit,
							begUpperLimit);
	get_wind_speed_interval(changePeriod.localEndTime(),
							theParameters.theArea,							
							theParameters.theWindDataVector,
							endLowerLimit,
							endUpperLimit);


	if(begLowerLimit == kFloatMissing ||
	   begUpperLimit == kFloatMissing ||
	   endLowerLimit == kFloatMissing ||
	   endUpperLimit == kFloatMissing)
	  {
		theParameters.theLog << "Error: failed to get wind speed change parameters!" << endl;

		//	return false;
	  }

	float changeThreshold(5.0);
	int periodLength(wind_forecast_period_length(changePeriod));

	phraseStr = EMPTY_STRING;

	if(abs(endUpperLimit - begUpperLimit) <= changeThreshold * 0.5)
	  {
		phraseStr = VAHAN_WORD;
		smallChange = true;
	  }
	else if(abs(endUpperLimit - begUpperLimit) >= changeThreshold)
	  {
		if(periodLength <= 6)
		  {
			phraseStr = NOPEASTI_WORD;
			fastChange = true;
		  }
		else if(periodLength >= 12)
		  {
			phraseStr = VAHITELLEN_WORD;
			gradualChange = true;
		  }
	  }

	return true;
  }

  Sentence WindForecast::speedRangeSentence(const WeatherPeriod& thePeriod,
											const WeatherResult & theTopSpeed,
											const WeatherResult & theMedianSpeed,
											const string & theVariable, 
											bool theUseAtItsStrongestPhrase) const
  {
	using Settings::optional_int;

	Sentence sentence;

	const int maxvalue = static_cast<int>(round(theTopSpeed.value()));
	const int medianvalue = static_cast<int>(round(theMedianSpeed.value()));
	
	int intervalLowerLimit(medianvalue);
	int intervalUpperLimit(maxvalue);
	get_wind_speed_interval(thePeriod,
							theParameters.theArea,									
							theParameters.theWindDataVector,
							intervalLowerLimit,
							intervalUpperLimit);

	int actualIntervalSize(abs(intervalUpperLimit - intervalLowerLimit));
	int peakWind(intervalUpperLimit);

	if(actualIntervalSize < theParameters.theMinIntervalSize)
	  {
		sentence << intervalLowerLimit
				 << *UnitFactory::create(MetersPerSecond);
	  }
	else
	  {
		if(actualIntervalSize > theParameters.theMaxIntervalSize)
		  {
			// if size of the interval is one more than allowed, we increase lower limit by 1
			// else we use 'kovimmillaan'-phrase
			if(actualIntervalSize == theParameters.theMaxIntervalSize + 1)
			  {
				intervalLowerLimit++;
			  }
			else
			  {
				intervalUpperLimit = intervalLowerLimit + theParameters.theMaxIntervalSize;
			  }
		  }

		// if interval upper limit is 2, we set lower limit to zero
		if(intervalUpperLimit == 2)
		  intervalLowerLimit = 0;


		if(peakWind > intervalUpperLimit)
		  {
			if(theUseAtItsStrongestPhrase)
			  {
				sentence << IntegerRange(intervalLowerLimit, intervalUpperLimit, theParameters.theRangeSeparator)
						 << *UnitFactory::create(MetersPerSecond);
				sentence << Delimiter(COMMA_PUNCTUATION_MARK) 
						 << KOVIMMILLAAN_PHRASE
						 << peakWind
						 << *UnitFactory::create(MetersPerSecond);
			  }
			else
			  {
				sentence << IntegerRange(peakWind - theParameters.theMaxIntervalSize, peakWind, theParameters.theRangeSeparator)
						 << *UnitFactory::create(MetersPerSecond);
			  }
		  }
		else
		  {
			sentence << IntegerRange(intervalLowerLimit, intervalUpperLimit, theParameters.theRangeSeparator)
					 << *UnitFactory::create(MetersPerSecond);
		  }
	  }
	  
	return sentence;
	  
  }

  // return upper and lower limit for the interval
  bool WindForecast::getSpeedIntervalLimits(const WeatherPeriod& thePeriod, 
											WeatherResult& lowerLimit,
											WeatherResult& upperLimit) const
  {
	WindDataItemUnit dataItem((*theParameters.theWindDataVector[0])(theParameters.theArea.type()));
	WindDataItemUnit dataItemMinTop((*theParameters.theWindDataVector[0])(theParameters.theArea.type()));

	bool firstRound(true);

	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		dataItem = (*theParameters.theWindDataVector[i])(theParameters.theArea.type());
		
		if(is_inside(dataItem.thePeriod.localStartTime(), thePeriod))
		  {
			if(firstRound)
			  {
				dataItemMinTop.theEqualizedTopWind = dataItem.theEqualizedTopWind;
				dataItemMinTop.theEqualizedMedianWind = dataItem.theEqualizedMedianWind;
				firstRound = false;
				continue;
			  }

			if (dataItem.theEqualizedTopWind.value() > 
				dataItemMinTop.theEqualizedTopWind.value())
			  {
				dataItemMinTop.theEqualizedTopWind = dataItem.theEqualizedTopWind;
			  }
			if (dataItem.theEqualizedMedianWind.value() < 
				dataItemMinTop.theEqualizedMedianWind.value())
			  {
				dataItemMinTop.theEqualizedMedianWind = dataItem.theEqualizedMedianWind;
			  }
		  }
	  }

	// no data found for thePeriod
	if(firstRound)
	  return false;

	lowerLimit = dataItemMinTop.theEqualizedMedianWind;
	upperLimit = dataItemMinTop.theEqualizedTopWind;

	return true;
  }

  // theLastSentenceFlag tells wheather this is the last wind speed sentence on this event period
  // so that we can use "at its strongest"-expression if needed
  Sentence WindForecast::windSpeedIntervalSentence(const WeatherPeriod& thePeriod, 
												   bool theUseAtItsStrongestPhrase /* = true*/) const
  {
	Sentence sentence;

	WeatherResult lowerLimit(kFloatMissing, 0.0);
	WeatherResult upperLimit(kFloatMissing, 0.0);

	//	if(getSpeedIntervalLimits(thePeriod, lowerLimit, upperLimit))
	  {
		sentence << speedRangeSentence(thePeriod,
									   upperLimit,
									   lowerLimit,
									   theParameters.theVar,
									   theUseAtItsStrongestPhrase);
	  }
	return sentence;
  }

  Sentence WindForecast::windDirectionSentence(const WindDirectionId& theWindDirectionId,
											   const bool& theBasicForm /*=  false */) const
  {
	Sentence sentence;

	switch(theWindDirectionId)
	  {
	  case POHJOINEN:
		sentence << (theBasicForm ? POHJOINEN_TUULI : POHJOINEN_TUULI_P);
		break;
	  case POHJOISEN_PUOLEINEN:
		sentence << (theBasicForm ? POHJOISEN_PUOLEINEN_TUULI : POHJOISEN_PUOLEINEN_TUULI_P);
		break;
	  case POHJOINEN_KOILLINEN:
		sentence << (theBasicForm ? POHJOINEN_KOILLINEN_TUULI : POHJOINEN_KOILLINEN_TUULI_P);
		break;
	  case KOILLINEN:
		sentence << (theBasicForm ? KOILLINEN_TUULI : KOILLINEN_TUULI_P);
		break;
	  case KOILLISEN_PUOLEINEN:
		sentence << (theBasicForm ? KOILLISEN_PUOLEINEN_TUULI : KOILLISEN_PUOLEINEN_TUULI_P);
		break;
	  case ITA_KOILLINEN:
		sentence << (theBasicForm ? ITA_KOILLINEN_TUULI : ITA_KOILLINEN_TUULI_P);
		break;
	  case ITA:
		sentence << (theBasicForm ? ITA_TUULI : ITA_TUULI_P);
		break;
	  case IDAN_PUOLEINEN:
		sentence << (theBasicForm ? IDAN_PUOLEINEN_TUULI : IDAN_PUOLEINEN_TUULI_P);
		break;
	  case ITA_KAAKKO:
		sentence << (theBasicForm ? ITA_KAAKKO_TUULI : ITA_KAAKKO_TUULI_P);
		break;
	  case KAAKKO:
		sentence << (theBasicForm ? KAAKKO_TUULI : KAAKKO_TUULI_P);
		break;
	  case KAAKON_PUOLEINEN:
		sentence << (theBasicForm ? KAAKON_PUOLEINEN_TUULI : KAAKON_PUOLEINEN_TUULI_P);
		break;
	  case ETELA_KAAKKO:
		sentence << (theBasicForm ? ETELA_KAAKKO_TUULI : ETELA_KAAKKO_TUULI_P);
		break;
	  case ETELA:
		sentence << (theBasicForm ? ETELA_TUULI : ETELA_TUULI_P);
		break;
	  case ETELAN_PUOLEINEN:
		sentence << (theBasicForm ? ETELAN_PUOLEINEN_TUULI : ETELAN_PUOLEINEN_TUULI_P);
		break;
	  case ETELA_LOUNAS:
		sentence << (theBasicForm ? ETELA_LOUNAS_TUULI : ETELA_LOUNAS_TUULI_P);
		break;
	  case LOUNAS:
		sentence << (theBasicForm ? LOUNAS_TUULI : LOUNAS_TUULI_P);
		break;
	  case LOUNAAN_PUOLEINEN:
		sentence << (theBasicForm ? LOUNAAN_PUOLEINEN_TUULI : LOUNAAN_PUOLEINEN_TUULI_P);
		break;
	  case LANSI_LOUNAS:
		sentence << (theBasicForm ? LANSI_LOUNAS_TUULI : LANSI_LOUNAS_TUULI_P);
		break;
	  case LANSI:
		sentence << (theBasicForm ? LANSI_TUULI : LANSI_TUULI_P);
		break;
	  case LANNEN_PUOLEINEN:
		sentence << (theBasicForm ? LANNEN_PUOLEINEN_TUULI :LANNEN_PUOLEINEN_TUULI_P);
		break;
	  case LANSI_LUODE:
		sentence << (theBasicForm ? LANSI_LUODE_TUULI : LANSI_LUODE_TUULI_P);
		break;
	  case LUODE:
		sentence << (theBasicForm ? LUODE_TUULI : LUODE_TUULI_P);
		break;
	  case LUOTEEN_PUOLEINEN:
		sentence << (theBasicForm ? LUOTEEN_PUOLEINEN_TUULI : LUOTEEN_PUOLEINEN_TUULI_P);
		break;
	  case POHJOINEN_LUODE:
		sentence << (theBasicForm ? POHJOINEN_LUODE_TUULI :POHJOINEN_LUODE_TUULI_P);
		break;
	  case VAIHTELEVA:
	  case MISSING_WIND_DIRECTION_ID:
		sentence << (theBasicForm ? VAIHTELEVA_TUULI : VAIHTELEVA_TUULI_P);
		break;
	  }

	return sentence;
  }

  void WindForecast::getWindDirectionBegEnd(const WeatherPeriod& thePeriod,
											WindDirectionId& theWindDirectionIdBeg,
											WindDirectionId& theWindDirectionIdEnd) const
  {
	theWindDirectionIdBeg = get_wind_direction_id_at(theParameters.theWindDataVector,
													 theParameters.theArea,
													 thePeriod.localStartTime(),
													 theParameters.theVar);

	theWindDirectionIdEnd = get_wind_direction_id_at(theParameters.theWindDataVector,
													 theParameters.theArea,
													 thePeriod.localEndTime(),
													 theParameters.theVar);
  }


  std::string WindForecast::getWindDirectionTurntoString(const WindDirectionId& theWindDirectionId) const
  {
	std::string retval;

	switch(theWindDirectionId)
	  {
	  case POHJOINEN:
		retval = POHJOISEEN_PHRASE;
		break;
	  case POHJOISEN_PUOLEINEN:
		retval = POHJOISEN_PUOLELLE_PHRASE;
		break;
	  case POHJOINEN_KOILLINEN:
		retval = POHJOISEN_JA_KOILLISEN_VALILLE_PHRASE;
		break;
	  case KOILLINEN:
		retval = KOILLISEEN_PHRASE;
		break;
	  case KOILLISEN_PUOLEINEN:
		retval = KOILLISEN_PUOLELLE_PHRASE;
		break;
	  case ITA_KOILLINEN:
		retval = IDAN_JA_KOILLISEN_VALILLE_PHRASE;
		break;
	  case ITA:
		retval = ITAAN_PHRASE;
		break;
	  case IDAN_PUOLEINEN:
		retval = IDAN_PUOLELLE_PHRASE;
		break;
	  case ITA_KAAKKO:
		retval = IDAN_JA_KAAKON_VALILLE_PHRASE;
		break;
	  case KAAKKO:
		retval = KAAKKOON_PHRASE;
		break;
	  case KAAKON_PUOLEINEN:
		retval = KAAKON_PUOLELLE_PHRASE;
		break;
	  case ETELA_KAAKKO:
		retval = ETELAN_JA_KAAKON_VALILLE_PHRASE;
		break;
	  case ETELA:
		retval = ETELAAN_PHRASE;
		break;
	  case ETELAN_PUOLEINEN:
		retval = ETELAN_PUOLELLE_PHRASE;
		break;
	  case ETELA_LOUNAS:
		retval = ETELAN_JA_LOUNAAN_VALILLE_PHRASE;
		break;
	  case LOUNAS:
		retval = LOUNAASEEN_PHRASE;
		break;
	  case LOUNAAN_PUOLEINEN:
		retval = LOUNAAN_PUOLELLE_PHRASE;
		break;
	  case LANSI_LOUNAS:
		retval = LANNEN_JA_LOUNAAN_VALILLE_PHRASE;
		break;
	  case LANSI:
		retval = LANTEEN_PHRASE;
		break;
	  case LANNEN_PUOLEINEN:
		retval = LANNEN_PUOLELLE_PHRASE;
		break;
	  case LANSI_LUODE:
		retval = LANNEN_JA_LUOTEEN_VALILLE_PHRASE;
		break;
	  case LUODE:
		retval = LUOTEESEEN_PHRASE;
		break;
	  case LUOTEEN_PUOLEINEN:
		retval = LUOTEEN_PUOLELLE_PHRASE;
		break;
	  case POHJOINEN_LUODE:
		retval = POHJOISEN_JA_LUOTEEN_VALILLE_PHRASE;
		break;
	  case VAIHTELEVA:
		retval = TUULI_MUUTTUU_VAIHTELEVAKSI_PHRASE;
		break;
	  case MISSING_WIND_DIRECTION_ID:
		retval = EMPTY_STRING;
		break;
	  }

	return retval;
  }

  bool WindForecast::windDirectionTurns(const WeatherPeriod& thePeriod) const
  {
	WeatherResult windDirectionBeg(kFloatMissing, kFloatMissing);
	WeatherResult windDirectionEnd(kFloatMissing, kFloatMissing);
	WeatherResult windTopSpeedBeg(kFloatMissing, kFloatMissing);
	WeatherResult windTopSpeedEnd(kFloatMissing, kFloatMissing);
	
	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		WindDataItemUnit& item = theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
		if(item.thePeriod.localStartTime() == thePeriod.localStartTime())
		  {
			windDirectionBeg = item.theEqualizedWindDirection;
			windTopSpeedBeg = item.theEqualizedTopWind;
		  }
		else if(item.thePeriod.localStartTime() == thePeriod.localEndTime())
		  {
			windDirectionEnd = item.theEqualizedWindDirection;
			windTopSpeedEnd = item.theEqualizedTopWind;
			break;
		  }
	  }

	return wind_direction_turns(windDirectionBeg,
								windDirectionEnd,
								windTopSpeedBeg,
								windTopSpeedEnd,
								theParameters.theVar);
  }

  void WindForecast::findOutActualWindSpeedChangePeriod(WindEventPeriodDataItem* currentEventPeriodItem,
														WindEventPeriodDataItem* nextEventPeriodItem) const
  {
	bool windStrengthStartsToChange(currentEventPeriodItem->theWindSpeedChangeStarts &&
									!currentEventPeriodItem->theWindSpeedChangeEnds);
	
	if(windStrengthStartsToChange && nextEventPeriodItem)
	  {
		if(!wind_speed_differ_enough(theParameters.theSources,
									 theParameters.theArea,
									 nextEventPeriodItem->thePeriod,
									 theParameters.theVar,
									 theParameters.theWindDataVector))
		  {
			currentEventPeriodItem->theWindSpeedChangePeriod = WeatherPeriod(currentEventPeriodItem->thePeriod.localStartTime(),
																			 nextEventPeriodItem->thePeriod.localEndTime());
			currentEventPeriodItem->theWindSpeedChangeEnds = true;
			nextEventPeriodItem->theWindSpeedChangeEnds = false;
			
			WindEventId nextEventId(nextEventPeriodItem->theWindEvent);
			nextEventPeriodItem->theWindEvent = mask_wind_event(nextEventId, 
																(nextEventId & TUULI_HEIKKENEE ? TUULI_HEIKKENEE : TUULI_VOIMISTUU));
		  }
	  }
  }

  float WindDataItemUnit::getWindSpeedShare(const float& theLowerLimit, const float& theUpperLimit) const
  {
	float retval = 0.0;
	
	for(unsigned int i = 0; i < theWindSpeedDistribution.size(); i++)
	  {
		if(theWindSpeedDistribution[i].first >= theLowerLimit &&
		   theWindSpeedDistribution[i].first < theUpperLimit)
		  retval += theWindSpeedDistribution[i].second.value();
	  }
	
	return retval;
  }

  float WindDataItemUnit::getWindDirectionShare(const WindDirectionId& windDirectionId) const
  {
	return get_wind_direction_share(theWindDirectionDistribution, windDirectionId);
  }

  float get_wind_direction_share(const vector<pair<float, WeatherResult> >& theWindDirectionDistribution,
								 const WindDirectionId& windDirectionId)
  {
	float retval(0.0);

	for(unsigned int i = 0; i < theWindDirectionDistribution.size(); i++)
	  {
		WeatherResult directionVar(theWindDirectionDistribution[i].first + 1.0, 0.0);
		WeatherResult speedVar(WeatherResult(5.0, 0.0));
		WindDirectionId directionId = wind_direction_id(directionVar,
														speedVar,
														"");
		if(directionId == windDirectionId)
		  retval += theWindDirectionDistribution[i].second.value();
	  }

	return retval;
  }

  float get_wind_direction_share(const wo_story_params& theParameters,
								 const WeatherPeriod& period,
								 const WindDirectionId& windDirectionId)
  {
	float sumShare(0.0);
	unsigned int counter(0);

	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		WindDataItemUnit& item = theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
		if(is_inside(item.thePeriod.localStartTime(), period))
		  {
			sumShare += get_wind_direction_share(item.theWindDirectionDistribution,
												 windDirectionId);
			counter++;
		  }
	  }

	return (counter == 0 ? 0.0 : sumShare / counter);
  }

  WeatherResult get_wind_direction_result_at(const wo_story_params& theParameters,
											 const NFmiTime& pointOfTime,
											 const string& var)
  {
	WeatherResult retval(kFloatMissing, 0.0);

	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		WindDataItemUnit& item = theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
		if(item.thePeriod.localStartTime() == pointOfTime)
		  {
			retval = item.theEqualizedWindDirection;
			break;
		  }
	  }
	
	return retval;
  }  

  bool is_gusty_wind(const wo_story_params& theParameters,
					 const NFmiTime& pointOfTime,
					 const string& var)
  {
	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		WindDataItemUnit& item = theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
		if(item.thePeriod.localStartTime() == pointOfTime)
		  {
			return (item.theEqualizedTopWind.value() >= 20.0 &&
					item.theGustSpeed.value() - item.theEqualizedTopWind.value() >= theParameters.theGustyWindTopWindDifference);
		  }
	  }
	return false;	
  }

  bool is_gusty_wind(const wo_story_params& theParameters,
					 const WeatherPeriod& period,
					 const string& var)
  {
 	float speedSum(0.0);
	float gustSpeedSum(0.0);
	unsigned int n(0);
	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		WindDataItemUnit& item = theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
		if(is_inside(item.thePeriod.localStartTime(), period))
		  {			
			speedSum += item.theEqualizedTopWind.value();
			gustSpeedSum += item.theGustSpeed.value();
			n++;
		  }
	  }

	if(n > 0)
	  {
		float avgTopWind(speedSum / n);
		float avgGustSpeed(gustSpeedSum / n);		
		return (avgTopWind >= 20.0 &&
				avgGustSpeed - avgTopWind >= theParameters.theGustyWindTopWindDifference);
	  }

	return false;
 }

  pair<WeatherResult, WindDirectionId> get_wind_direction_pair_at(const wind_data_item_vector& theWindDataVector,
																  const WeatherArea& theArea,
																  const NFmiTime& pointOfTime,
																  const string& var)
  {
	WeatherResult directionValue(kFloatMissing, 0.0);
	WindDirectionId directionId(MISSING_WIND_DIRECTION_ID);

	for(unsigned int i = 0; i < theWindDataVector.size(); i++)
	  {
		WindDataItemUnit& item = theWindDataVector[i]->getDataItem(theArea.type());
		if(item.thePeriod.localStartTime() == pointOfTime)
		  {
			directionValue = item.theEqualizedWindDirection;
			directionId = wind_direction_id(item.theEqualizedWindDirection, 
											item.theEqualizedTopWind,
											var);
		  }
	  }

	return make_pair(directionValue, directionId);
  }

  pair<WeatherResult, WindDirectionId> get_wind_direction_pair_at(const wind_data_item_vector& theWindDataVector,
																  const WeatherArea& theArea,
																  const AnalysisSources& theSources,
																  const WeatherPeriod& period,
																  const string& var)
  {
	float topSpeedSum(0.0);
	float topSpeedStdDevSum(0.0);
	float medianSpeedSum(0.0);
	float medianSpeedStdDevSum(0.0);
	unsigned int n(0);
	for(unsigned int i = 0; i < theWindDataVector.size(); i++)
	  {
		WindDataItemUnit& item = theWindDataVector[i]->getDataItem(theArea.type());
		if(is_inside(item.thePeriod.localStartTime(), period))
		  {
			topSpeedSum += item.theEqualizedTopWind.value();
			topSpeedStdDevSum += item.theEqualizedTopWind.error();
			medianSpeedSum += item.theEqualizedMedianWind.value();
			medianSpeedStdDevSum += item.theEqualizedMedianWind.error();
			n++;
		  }
	  }
	float avgTopWind(topSpeedSum / n);
	float avgTopWindStdDev(topSpeedStdDevSum / n);
	float avgMedianWind(medianSpeedSum / n);
	float avgMedianWindStdDev(medianSpeedStdDevSum / n);
	WeatherResult topWindSpeed(avgTopWind, avgTopWindStdDev);
	WeatherResult medianWindSpeed(avgMedianWind, avgMedianWindStdDev);

	WeatherResult resultDirection = mean_wind_direction(theSources,
														theArea,
														period,
														medianWindSpeed,
														topWindSpeed,
														var);

	float directionError = mean_wind_direction_error(theWindDataVector, theArea, period);

	if(directionError < resultDirection.error())
	  resultDirection = WeatherResult(resultDirection.value(), directionError);

	return make_pair(resultDirection, wind_direction_id(resultDirection, topWindSpeed, var));
  }

  WeatherResult get_wind_direction_at(const wind_data_item_vector& theWindDataVector,
									  const WeatherArea& theArea,
									  const NFmiTime& pointOfTime,
									  const string& var)
  {
	return(get_wind_direction_pair_at(theWindDataVector,
									  theArea,
									  pointOfTime,
									  var).first);	
  }

  WeatherResult get_wind_direction_at(const wind_data_item_vector& theWindDataVector,
									  const WeatherArea& theArea,
									  const AnalysisSources& theSources,
									  const WeatherPeriod& period,
									  const string& var)
  {
	return(get_wind_direction_pair_at(theWindDataVector,
									  theArea,
									  theSources,
									  period,
									  var).first);
  }

  WindDirectionId get_wind_direction_id_at(const wind_data_item_vector& theWindDataVector,
										   const WeatherArea& theArea,
										   const NFmiTime& pointOfTime,
										   const string& var)
  {
	return(get_wind_direction_pair_at(theWindDataVector,
									  theArea,
									  pointOfTime,
									  var).second);	
  }

  WindDirectionId get_wind_direction_id_at(const wind_data_item_vector& theWindDataVector,
										   const WeatherArea& theArea,
										   const AnalysisSources& theSources,
										   const WeatherPeriod& period,
										   const string& var)
  {
	return(get_wind_direction_pair_at(theWindDataVector,
									  theArea,
									  theSources,
									  period,
									  var).second);
  }


  std::string get_wind_event_string(const WindEventId& theWindEventId)
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
		retval = "tuuli kaantyy";
		break;
	  case TUULI_KAANTYY_JA_HEIKKENEE:
		retval = "tuuli kaantyy ja heikkenee";
		break;
	  case TUULI_KAANTYY_JA_VOIMISTUU:
		retval = "tuuli kaantyy ja voimistuu";
		break;
	  case TUULI_KAANTYY_JA_TYYNTYY:
		retval = "tuuli kaantyy ja tyyntyy";
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
	  case MISSING_WIND_SPEED_EVENT:
		retval = "missing wind speed event";
		break;
	  case MISSING_WIND_DIRECTION_EVENT:
		retval = "missing wind direction event";
		break;
	  }

	return retval;
  }

  bool is_valid_wind_event_id(const int& eventId)
  {
	bool retval(false);

	switch(eventId)
	  {
	  case TUULI_HEIKKENEE:
	  case TUULI_VOIMISTUU:
	  case TUULI_TYYNTYY:
	  case TUULI_KAANTYY:
	  case TUULI_KAANTYY_JA_HEIKKENEE:
	  case TUULI_KAANTYY_JA_VOIMISTUU:
	  case TUULI_KAANTYY_JA_TYYNTYY:
	  case TUULI_MUUTTUU_VAIHTELEVAKSI:
	  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE:
	  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU:
	  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_TYYNTYY:
	  case MISSING_WIND_EVENT:
	  case MISSING_WIND_SPEED_EVENT:
	  case MISSING_WIND_DIRECTION_EVENT:
		retval = true;
		break;
	  }

	return retval;
  }

  bool wind_speed_differ_enough(const AnalysisSources& theSources,
								const WeatherArea& theArea,
								const WeatherPeriod& thePeriod,
								const string& theVar,
								const wind_data_item_vector& windDataVector)
  {
	int begLowerLimit(0), 
	  begUpperLimit(0), 
	  endLowerLimit(0), 
	  endUpperLimit(0);

	get_wind_speed_interval(thePeriod.localStartTime(),
							theArea,							
							windDataVector,
							begLowerLimit,
							begUpperLimit);
	get_wind_speed_interval(thePeriod.localEndTime(),
							theArea,							
							windDataVector,
							endLowerLimit,
							endUpperLimit);



	//	float lowerDiff = abs(endLowerLimit - begLowerLimit);
	float upperDiff = abs(endUpperLimit - begUpperLimit);

	return (upperDiff >= 2.0);
	//	return (lowerDiff + upperDiff >= 3.0);
  }

  void get_wind_speed_interval_parameters(const WeatherPeriod& thePeriod,
										  const WeatherArea& theArea,										  
										  const wind_data_item_vector& windDataVector,
										  int& begLowerLimit,
										  int& begUpperLimit,
										  int& endLowerLimit,
										  int& endUpperLimit,
										  float& changeRatePerHour)
  {
	get_wind_speed_interval(thePeriod.localStartTime(),
							theArea,							
							windDataVector,
							begLowerLimit,
							begUpperLimit);
	get_wind_speed_interval(thePeriod.localEndTime(),
							theArea,							
							windDataVector,
							endLowerLimit,
							endUpperLimit);

	changeRatePerHour = abs(endUpperLimit - begUpperLimit) / wind_forecast_period_length(thePeriod);
  }

  void get_plain_wind_speed_interval(const NFmiTime& pointOfTime,
									 const WeatherArea& area,
									 const wind_data_item_vector& windDataVector,
									 int& lowerLimit,
									 int& upperLimit)
  {
	bool found(false);
	for(unsigned int i = 0; i < windDataVector.size(); i++)
	  {
		const WindDataItemUnit& windDataItem = (*windDataVector[i])(area.type());
			
		if(windDataItem.thePeriod.localStartTime() == pointOfTime)
		  {
			lowerLimit = static_cast<int>(round(windDataItem.theEqualizedMedianWind.value()));
			upperLimit = static_cast<int>(round(windDataItem.theEqualizedTopWind.value()));
			found = true;
			break;
		  }
	  }
  }

  void get_plain_wind_speed_interval(const WeatherPeriod& period,
									 const WeatherArea& area,
									 const wind_data_item_vector& windDataVector,
									 int& lowerLimit,
									 int& upperLimit)
  {
	bool firstMatchProcessed(false);

	float minValue(lowerLimit);
	float maxValue(upperLimit);
	for(unsigned int i = 0; i < windDataVector.size(); i++)
	  {
		const WindDataItemUnit& windDataItem = (*windDataVector[i])(area.type());
			
		if(is_inside(windDataItem.thePeriod,period))
		  {
			if(!firstMatchProcessed)
			  {
				minValue = windDataItem.theEqualizedMedianWind.value();
				maxValue = windDataItem.theEqualizedTopWind.value();
				firstMatchProcessed = true;
			  }
			else
			  {
				if(windDataItem.theEqualizedMedianWind.value() < minValue)
				  minValue = windDataItem.theEqualizedMedianWind.value();
				if(windDataItem.theEqualizedTopWind.value() > maxValue)
				  maxValue = windDataItem.theEqualizedTopWind.value();
			  }
		  }
	  }
	lowerLimit = static_cast<int>(round(minValue));
	upperLimit = static_cast<int>(round(maxValue));
 }

  float get_median_wind(const WeatherPeriod& period,
						const WeatherArea& area,
						const wind_data_item_vector& windDataVector)
  {
 	float retval(0.0);
	unsigned int counter(0);

	for(unsigned int i = 0; i < windDataVector.size(); i++)
	  {
		const WindDataItemUnit& windDataItem = (*windDataVector[i])(area.type());
			
		if(is_inside(windDataItem.thePeriod, period))
		  {
			retval += windDataItem.theEqualizedMedianWind.value();
			counter++;
		  }
	  }

	return (counter == 0 ? retval : retval / counter);
 }

  float get_maximum_wind(const WeatherPeriod& period,
						 const WeatherArea& area,
						 const wind_data_item_vector& windDataVector)
  {
 	float retval(0.0);
	unsigned int counter(0);

	for(unsigned int i = 0; i < windDataVector.size(); i++)
	  {
		const WindDataItemUnit& windDataItem = (*windDataVector[i])(area.type());
			
		if(is_inside(windDataItem.thePeriod, period))
		  {
			retval += windDataItem.theEqualizedTopWind.value();
			counter++;
		  }
	  }

	return (counter == 0 ? retval : retval / counter);
  }

  void windspeed_distribution_interval(const WeatherPeriod& thePeriod,
									   const WeatherArea& area,
									   const wind_data_item_vector& windDataVector,
									   int& lowerLimit,
									   int& upperLimit)
  {
	if(lowerLimit == static_cast<int>(kFloatMissing) ||
	   upperLimit == static_cast<int>(kFloatMissing))
	  return;

	value_distribution_data_vector windSpeedDistributionVector;

	// first calculte the sum of the distribution values for the period
	unsigned int counter(0);
	for(unsigned int i = 0; i < windDataVector.size(); i++)
	  {
		const WindDataItemUnit& windDataItem = (*windDataVector[i])(area.type());
			
		if(is_inside(windDataItem.thePeriod, thePeriod))
		  {
			if(windSpeedDistributionVector.size() == 0)
			  {
				windSpeedDistributionVector = windDataItem.theWindSpeedDistribution;
			  }
			else
			  {
				for(unsigned int k = 0; k < windDataItem.theWindSpeedDistribution.size(); k++)
				  {
					WeatherResult originalShare(windSpeedDistributionVector[k].second);
					WeatherResult shareToAdd(windDataItem.theWindSpeedDistribution[k].second);
					windSpeedDistributionVector[k].second = WeatherResult(originalShare.value() +
																		  shareToAdd.value(),
																		  0.0);
				  }
			  }
			counter++;
		  }
	  }

	// then calculate mean value of the distribution values of the period
	for(unsigned int i = 0; i < windSpeedDistributionVector.size(); i++)
	  {
		WeatherResult cumulativeShare(windSpeedDistributionVector[i].second);
		windSpeedDistributionVector[i].second = WeatherResult(cumulativeShare.value() / counter, 0.0);
	  }
	
	const int maxvalue = upperLimit;
	const int minvalue = lowerLimit;

	// subtract 1 m/s from the lower limit
	lowerLimit  = (maxvalue - (minvalue - 1) > 5 ? maxvalue - 5 : minvalue - 1);

	// Make sure that 2/3 of values are inside the interval
	int lower_index = maxvalue;

	float sumDistribution = windSpeedDistributionVector[maxvalue].second.value();
	while(sumDistribution < 67.0 && lower_index > 0)
	  {
		lower_index--;
		sumDistribution += windSpeedDistributionVector[lower_index].second.value();
	  }

	//lowerLimit = lower_index;
	lowerLimit = lower_index + 1;
  }

  void get_wind_speed_interval(const WeatherPeriod& thePeriod,
							   const WeatherArea& theArea,							   
							   const wind_data_item_vector& windDataVector,
							   int& lowerLimit,
							   int& upperLimit)
  {
	get_plain_wind_speed_interval(thePeriod,
								  theArea,
								  windDataVector,
								  lowerLimit,
								  upperLimit);

	windspeed_distribution_interval(thePeriod,
									theArea,
									windDataVector,
									lowerLimit,
									upperLimit);
  }

  void get_wind_speed_interval(const NFmiTime& pointOfTime,
							   const WeatherArea& theArea,							   
							   const wind_data_item_vector& windDataVector,
							   int& lowerLimit,
							   int& upperLimit)
  {
	get_plain_wind_speed_interval(pointOfTime,
								  theArea,
								  windDataVector,
								  lowerLimit,
								  upperLimit);

	WeatherPeriod period(pointOfTime, pointOfTime);

	windspeed_distribution_interval(period,
									theArea,
									windDataVector,
									lowerLimit,
									upperLimit);
  }

  bool ascending_order(const float& direction1, 
					   const float& direction2)
  {
	bool retval(false);

	if(direction1 >= 180.0)
	  {
		if(direction2 > direction1 || direction2 < direction1 - 180.0)
		  retval = true;
	  }
	else
	  {
		retval = (direction2 > direction1 && direction2 - direction1 < 180);
	  }
	return retval;
  }

  bool wind_turns_to_the_same_direction(const float& direction1, 
										const float& direction2,  
										const float& direction3)
  {
	bool ascending12 = ascending_order(direction1, direction2);
	bool ascending23 = ascending_order(direction2, direction3);
	bool ascending13 = ascending_order(direction1, direction3);
	
	return ((ascending12 && ascending23 && ascending13) ||
			(!ascending12 && !ascending23 && !ascending13));	
  }

  WindEventId mask_wind_event(const WindEventId& originalId, const WindEventId& maskToRemove)
  {
	unsigned short mask(maskToRemove);
	unsigned short value = originalId;
	value &= (~mask);
	return (static_cast<WindEventId>(value));
  }

  WeatherPeriod get_wind_turning_period(const WeatherPeriod& thePeriod,
										const wind_event_period_data_item_vector& theDirectionEventVector)
  {
	for(unsigned int i = 0; i < theDirectionEventVector.size(); i++)
	  if(is_inside(theDirectionEventVector[i]->thePeriod, thePeriod))
		return theDirectionEventVector[i]->thePeriod;

	return thePeriod;
  }

  void get_median_wind_speed_by_area(const wind_data_item_vector& theWindDataVector,
									 const WeatherArea& theFullArea,
									 const WeatherPeriod& thePeriod,									 
									 float& medianWindFull,
									 float& medianWindCoastal,
									 float& medianWindInland)
  {
	WeatherArea coastalArea(theFullArea);
	coastalArea.type(WeatherArea::Coast);
	WeatherArea inlandArea(theFullArea);
	inlandArea.type(WeatherArea::Inland);

	medianWindFull = get_median_wind(thePeriod,
									 theFullArea,
									 theWindDataVector);
	medianWindCoastal = get_median_wind(thePeriod,
										coastalArea,
										theWindDataVector);
	medianWindInland = get_median_wind(thePeriod,
									   inlandArea,
									   theWindDataVector);
  }

  void get_wind_direction_by_area(const wind_data_item_vector& theWindDataVector,
								  const WeatherArea& theFullArea,
								  const AnalysisSources& theSources,
								  const WeatherPeriod& thePeriod,
								  const string& theVar,
								  WeatherResult& windDirectionFull,
								  WeatherResult& windDirectionCoastal,
								  WeatherResult& windDirectionInland)
  {
	WeatherArea coastalArea(theFullArea);
	coastalArea.type(WeatherArea::Coast);
	WeatherArea inlandArea(theFullArea);
	inlandArea.type(WeatherArea::Inland);
	
	windDirectionFull = get_wind_direction_at(theWindDataVector,
											  theFullArea,
											  theSources,
											  thePeriod,
											  theVar);
	
	windDirectionCoastal = get_wind_direction_at(theWindDataVector,
												 coastalArea,
												 theSources,
												 thePeriod,
												 theVar);

	windDirectionInland = get_wind_direction_at(theWindDataVector,
												inlandArea,
												theSources,
												thePeriod,
												theVar);
  }

  NFmiTime get_phrase_period_end_timestamp(const NFmiTime& originalTimestamp)
  {
	NFmiTime phrasePeriodTimestamp(originalTimestamp);

	if(phrasePeriodTimestamp.GetHour() == 18)
	  phrasePeriodTimestamp.ChangeByHours(-2);
	else if(phrasePeriodTimestamp.GetHour() == 17)
	  phrasePeriodTimestamp.ChangeByHours(-1);

	return phrasePeriodTimestamp;
  }


} // namespace TextGen

