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
  using namespace WeatherAnalysis;
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

	WindEventId eventIdPrevious(MISSING_WIND_EVENT);
	WindDirectionId windDirectionIdPrevious(MISSING_WIND_DIRECTION_ID);

	WindEventPeriodDataItem* eventPeriodItemPrevious(0);
	WindEventPeriodDataItem* eventPeriodItemNext(0);
	for(unsigned int i = 0; i < theParameters.theMergedWindEventPeriodVector.size(); i++)
	  {
		Sentence sentence;
		WindEventPeriodDataItem* eventPeriodItem = theParameters.theMergedWindEventPeriodVector[i];
		bool lastPeriod(i == theParameters.theMergedWindEventPeriodVector.size() - 1);
		bool firstPeriod(i == 0);
		
		if(!lastPeriod)
		  eventPeriodItemNext = theParameters.theMergedWindEventPeriodVector[i+1];
		
		// if the last period is 1 hour long, ignore it
		if(lastPeriod && get_period_length(eventPeriodItem->thePeriod) == 0)
		  continue;

		WindEventId eventId = eventPeriodItem->theWindEvent;

		bool firstSentenceInTheStory = paragraph.empty();

		theParameters.theAlkaenPhraseUsed = false;

		const WeatherPeriod& eventPeriod = eventPeriodItem->thePeriod;

		WeatherPeriod eventStartPeriod(eventPeriod.localStartTime(),
									   eventPeriod.localStartTime());
		WeatherPeriod eventEndPeriod(eventPeriod.localEndTime(),
									 eventPeriod.localEndTime());
		
		WindDirectionId windDirectionIdBeg(get_wind_direction_id_at(theParameters,
																	eventPeriod.localStartTime(),
																	theParameters.theVar));
		WindDirectionId windDirectionIdAvg(get_wind_direction_id_at(theParameters,
																	eventPeriod,
																	theParameters.theVar));
				
		bool same_part_of_the_day(get_part_of_the_day_id(eventPeriod.localStartTime()) ==
								  get_part_of_the_day_id(eventPeriod.localEndTime()) &&
								  eventPeriod.localStartTime().GetDay() == eventPeriod.localEndTime().GetDay());

		if((eventId & TUULI_HEIKKENEE || eventId & TUULI_VOIMISTUU) &&
		   !(eventPeriodItem->theWindSpeedChangeStarts &&
			!eventPeriodItem->theWindSpeedChangeEnds) &&
		   !wind_speed_differ_enough(theParameters.theSources,
									 theParameters.theArea,
									 eventPeriod,
									 theParameters.theVar,
									 theParameters.theWindDataVector))
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
		// if previous event was 'tuuli muuttuu vaihtelevaksi',
		// dont report 'tuuli on vaihtelevaa' on current period
		if(eventIdPrevious & TUULI_MUUTTUU_VAIHTELEVAKSI &&
		   eventId == MISSING_WIND_EVENT &&
		   windDirectionIdAvg == VAIHTELEVA)
		  continue;

		if(eventId == TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE && get_period_length(eventPeriod) == 0)
		  eventId = mask_wind_event(eventId, TUULI_HEIKKENEE);
	  		  
		// if wind vas vaihteleva on previous period, dont report 'tuuli muuttuu vaihtelevaksi'
		// on this period
		if(eventId & TUULI_MUUTTUU_VAIHTELEVAKSI &&
		   eventPeriodItemPrevious && get_wind_direction_id_at(theParameters,
															   eventPeriodItemPrevious->thePeriod,
															   theParameters.theVar) == VAIHTELEVA)
		  continue;

		// if wind direction changes in two successive periods,
		// it can not be the same in the end of these periods
		if(eventPeriodItemPrevious && 
		   (eventId & TUULI_KAANTYY) && 
		   (eventIdPrevious & TUULI_KAANTYY))
		  {
			if(get_wind_direction_id_at(theParameters,
										eventPeriodItem->thePeriod.localEndTime(),
										theParameters.theVar) ==
			   get_wind_direction_id_at(theParameters,
										eventPeriodItemPrevious->thePeriod.localEndTime(),
										theParameters.theVar))
			  eventId = mask_wind_event(eventId, TUULI_KAANTYY);
		  }

		WindDirectionId directionIdPrevious(windDirectionIdPrevious);
		if(eventIdPrevious & TUULI_KAANTYY)
		  {
			if(get_wind_direction_event(*eventPeriodItemPrevious,
										theParameters.theVar,
										theParameters.theWindDirectionThreshold) == MISSING_WIND_DIRECTION_EVENT)
			  {
				directionIdPrevious =  get_wind_direction_id_at(theParameters,
																eventPeriodItemPrevious->thePeriod,
																theParameters.theVar);
			  }
			else
			  {
				directionIdPrevious =  get_wind_direction_id_at(theParameters,
																eventPeriodItemPrevious->thePeriod.localEndTime(),
																theParameters.theVar);
			  }
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
			  Sentence directionSentenceP;
			  Sentence directionSentence;
			  if((i > 0 && directionIdPrevious == windDirectionIdAvg) ||
				 (eventIdPrevious == TUULI_MUUTTUU_VAIHTELEVAKSI && windDirectionIdAvg == VAIHTELEVA))
				{
				  directionSentenceP << TUULTA_WORD;
				  directionSentence << TUULI_WORD;
				}
			  else
				{
				  directionSentenceP << windDirectionSentence(windDirectionIdAvg);
				  directionSentence << windDirectionSentence(windDirectionIdAvg, true);
				}

			  if(!getWindSpeedChangeAttribute(eventPeriod, changeAttributeStr,
											  smallChange, gradualChange, fastChange))
				{
				  return paragraph;
				}
			  
			  bool useAlkaaHeiketaVoimistuaPhrase(false);

			  bool appendDecreasingIncreasingInterval(true);

			  findOutActualWindSpeedChangePeriod(eventPeriodItem, eventPeriodItemNext);
			  bool windStrengthStartsToChange(eventPeriodItem->theWindSpeedChangeStarts &&
											  !eventPeriodItem->theWindSpeedChangeEnds);
			  
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
					  if((eventIdPrevious & TUULI_VOIMISTUU && eventId == TUULI_VOIMISTUU) ||
						 (eventIdPrevious & TUULI_HEIKKENEE && eventId == TUULI_HEIKKENEE))
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
					  if((eventIdPrevious & TUULI_VOIMISTUU && eventId == TUULI_VOIMISTUU) ||
						 (eventIdPrevious & TUULI_HEIKKENEE && eventId == TUULI_HEIKKENEE))
						{
						  sentence << ILTAPAIVALLA_EDELLEEN_HEIKKENEVAA_POHJOISTUULTA
								   << getTimePhrase(eventPeriod, DONT_USE_ALKAEN_PHRASE)
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

			  if(eventPeriodItem->theWindSpeedChangeEnds)
				sentence << decreasingIncreasingInterval(*eventPeriodItem,
														 firstSentenceInTheStory,
														 useAlkaaHeiketaVoimistuaPhrase,
														 eventId);
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
													  eventIdPrevious,
													  directionIdPrevious);
			}
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI:
			{
			  WeatherPeriod actualEventPeriod(eventPeriod);
			  // report together
			  if(eventPeriodItemNext && eventPeriodItemNext->theWindEvent == MISSING_WIND_EVENT &&
				 get_wind_direction_id_at(theParameters,
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
								   << windDirectionSentence(VAIHTELEVA)
								   << windSpeedIntervalSentence(actualEventPeriod, 
																USE_AT_ITS_STRONGEST_PHRASE);
						}
					  else
						{
						  sentence << ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
								   << getTimePhrase(eventStartPeriod, DONT_USE_ALKAEN_PHRASE);
						}
					}
				}
			}
			break;
		  case TUULI_KAANTYY_JA_VOIMISTUU:
		  case TUULI_KAANTYY_JA_HEIKKENEE:
			{
			  bool useAlkaaHeiketaVoimistuaPhrase(false);
			  bool appendDecreasingIncreasingInterval(true);

			  findOutActualWindSpeedChangePeriod(eventPeriodItem, eventPeriodItemNext);

			  sentence << windDirectionAndSpeedChangesSentence(theParameters,
															   eventPeriod,
															   firstSentenceInTheStory,
															   eventIdPrevious,
															   eventId,
															   directionIdPrevious,
															   eventPeriodItem->theWindSpeedChangeStarts,
															   eventPeriodItem->theWindSpeedChangeEnds,
															   appendDecreasingIncreasingInterval);

			  if(!sentence.empty())
				{
				  if(eventPeriodItem->theWindSpeedChangeEnds)
					sentence << decreasingIncreasingInterval(*eventPeriodItem,
															 firstSentenceInTheStory,
															 useAlkaaHeiketaVoimistuaPhrase,
															 eventId);
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
			  if(eventIdPrevious & TUULI_HEIKKENEE)
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

			  bool useAlkaaHeiketaVoimistuaPhrase(false);
			  sentence << decreasingIncreasingInterval(*eventPeriodItem,
													   firstSentenceInTheStory,
													   useAlkaaHeiketaVoimistuaPhrase,
													   eventId);
			}
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU:
			{
			  if(eventIdPrevious == TUULI_VOIMISTUU)
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

			  bool useAlkaaHeiketaVoimistuaPhrase(false);
			  sentence << decreasingIncreasingInterval(*eventPeriodItem,
													   firstSentenceInTheStory,
													   useAlkaaHeiketaVoimistuaPhrase,
													   eventId);
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
					  eventIdPrevious & TUULI_MUUTTUU_VAIHTELEVAKSI)
				{
				  WeatherPeriod period(eventPeriodItemPrevious->thePeriod.localEndTime(),
									   eventPeriod.localEndTime());
				  bool windSpeedDifferEnough = wind_speed_differ_enough(theParameters.theSources,
																		theParameters.theArea,
																		period,
																		theParameters.theVar,
																		theParameters.theWindDataVector);

				  bool useAlkaenPhrase(get_period_length(eventPeriod) >= 6);

				  if(directionIdPrevious == windDirectionIdAvg)
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
			  else if((eventIdPrevious & TUULI_MUUTTUU_VAIHTELEVAKSI) && 
					  i < theParameters.theMergedWindEventPeriodVector.size() - 1 &&
					  get_period_length(eventPeriod) > 2)
				{
				  sentence << windDirectionChangeSentence(theParameters,
														  eventPeriod,
														  firstSentenceInTheStory,
														  eventIdPrevious,
														  directionIdPrevious);
				}
			}
			break;
		  default:
			break;
		  };

		if(!sentence.empty())
		  {
			eventPeriodItemPrevious = theParameters.theMergedWindEventPeriodVector[i];
			eventIdPrevious = eventId;
			windDirectionIdPrevious = windDirectionIdAvg;
		  }

		paragraph << sentence;

	  } // for


	return paragraph;
  }

  Sentence WindForecast::windDirectionChangeSentence(const wo_story_params& theParameters,
													 const WeatherPeriod& eventPeriod,
													 const bool& firstSentenceInTheStory,
													 const WindEventId& eventIdPrevious,
													 const WindDirectionId& windDirectionIdPrevious) const
  {
	Sentence sentence;

	bool useAlkaenPhrase(get_period_length(eventPeriod) >= 6);
	WindDirectionId windDirectionIdBeg(MISSING_WIND_DIRECTION_ID);
	WindDirectionId windDirectionIdEnd(MISSING_WIND_DIRECTION_ID);
	/*
	WindDirectionId windDirectionIdAvg(get_wind_direction_id_at(theParameters,
																eventPeriod,
																theParameters.theVar));	
	*/

	getWindDirectionBegEnd(eventPeriod, windDirectionIdBeg, windDirectionIdEnd);

	if(firstSentenceInTheStory)
	  {
		if(windDirectionIdBeg == VAIHTELEVA ||
		   windDirectionIdBeg == MISSING_WIND_DIRECTION_ID ||
		   windDirectionIdPrevious == VAIHTELEVA)
		  {
			sentence << directedWindSentenceAfterVaryingWind(theParameters,
															 eventPeriod,
															 firstSentenceInTheStory);
		  }
		else
		  {
			sentence << POHJOISTUULTA_INTERVALLI_MS_JOKA_KAANTYY_ILTAPAIVALLA_ETELAAN_COMPOSITE_PHRASE
					 << windDirectionSentence(windDirectionIdBeg)
					 << windSpeedIntervalSentence(eventPeriod, 
												  DONT_USE_AT_ITS_STRONGEST_PHRASE)
					 << getTimePhrase(eventPeriod, useAlkaenPhrase)
					 << getWindDirectionTurntoString(windDirectionIdEnd);
		  }
	  }
	else
	  {
		if(windDirectionIdPrevious == VAIHTELEVA ||
		   eventIdPrevious & TUULI_MUUTTUU_VAIHTELEVAKSI)
		  {
			sentence << directedWindSentenceAfterVaryingWind(theParameters,
															 eventPeriod,
															 firstSentenceInTheStory);
		  }
		else if(windDirectionIdEnd != windDirectionIdPrevious)
		  {
			sentence << ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE
					 << getTimePhrase(eventPeriod, useAlkaenPhrase)
					 << getWindDirectionTurntoString(windDirectionIdEnd);
		  }
	  }

	return sentence;
  }

  Sentence WindForecast::windDirectionAndSpeedChangesSentence(const wo_story_params& theParameters,
															  const WeatherPeriod& eventPeriod,
															  const bool& firstSentenceInTheStory,
															  const WindEventId& previousWindEventId,
															  const WindEventId& currentWindEventId,
															  const WindDirectionId& previousWindDirectionId,
															  const bool& theWindSpeedChangeStarts,
															  const bool& theWindSpeedChangeEnds,
															  bool& appendDecreasingIncreasingInterval) const
  {
	Sentence sentence;
	
	WindDirectionId windDirectionIdBeg(MISSING_WIND_DIRECTION_ID);
	WindDirectionId windDirectionIdEnd(MISSING_WIND_DIRECTION_ID);
	getWindDirectionBegEnd(eventPeriod, windDirectionIdBeg, windDirectionIdEnd);
	WindDirectionId windDirectionIdAvg(get_wind_direction_id_at(theParameters,
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
				  }
				else
				  {
					//	ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
					if(strengtheningWind)
					  sentence << ILTAPAIVALLA_ETELATUULI_VOIMISTUU_NOPEASTI_COMPOSITE_PHRASE;
					else
					  sentence << ILTAPAIVALLA_ETELATUULI_HEIKKENEE_NOPEASTI_COMPOSITE_PHRASE;
					sentence  << getTimePhrase(eventPeriod, useAlkaenPhrase)
							  << windDirectionSentence(windDirectionIdEnd, true)
							  << changeAttributeStr;
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
							//ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
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
							//ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
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
							appendDecreasingIncreasingInterval = false;
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
							appendDecreasingIncreasingInterval = false;
						  }
						else
						  {
							sentence << ILTAPAIVALLA_TUULI_VOIMISTUU_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE;
						  }
					  }
				  }

				sentence << getTimePhrase(eventPeriod, useAlkaenPhrase)
						 << getWindDirectionTurntoString(windDirectionIdEnd);
			  }
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
		WindDirectionId windDirectionIdBegPlus1(get_wind_direction_id_at(theParameters,
																		 begTimePlus1,
																		 theParameters.theVar));
		WindDirectionId windDirectionIdAvgPlus1(get_wind_direction_id_at(theParameters,
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
						//ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
						if(strengtheningWind)
						  sentence << ILTAPAIVALLA_ETELATUULI_VOIMISTUU_NOPEASTI_COMPOSITE_PHRASE;
						else
						  sentence << ILTAPAIVALLA_ETELATUULI_HEIKKENEE_NOPEASTI_COMPOSITE_PHRASE;
						sentence  << getTimePhrase(eventPeriod, useAlkaenPhrase)
								  << windDirectionSentence(windDirectionIdAvgPlus1, true)
								  << changeAttributeStr;
					  }
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
								 << changeAttributeStr
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
				  }
			  }
		  }
		else // not first sentence 
		  {
			if(previousWindEventId & TUULI_MUUTTUU_VAIHTELEVAKSI ||
			   previousWindDirectionId == VAIHTELEVA)
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
						sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
								 << timePhrase
								 << changeAttributeStr
								 << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
								 << windDirectionSentence(windDirectionIdAvgPlus1);
					  }
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
				  }
				else
				  {
					sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
							 << timePhrase
							 << changeAttributeStr
							 << (strengtheningWind ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
							 << windDirectionSentence(windDirectionIdAvg);
				  }
			  }
		  }

	  }

	return sentence;
  }

  Sentence WindForecast::directedWindSentenceAfterVaryingWind(const wo_story_params& theParameters,
															  const WeatherPeriod& eventPeriod,
															  const bool& firstSentenceInTheStory) const
  {
	Sentence sentence;

#ifdef LATER
	NFmiTime periodStartTime(eventPeriod.localStartTime());
	NFmiTime periodStartTimePlus1(eventPeriod.localStartTime());
	periodStartTimePlus1.ChangeByHours(1);
	NFmiTime periodEndTime(eventPeriod.localEndTime());
	WeatherPeriod periodStart(periodStartTime,
							  periodStartTime);
	WeatherPeriod periodEnd(periodEndTime,
							periodEndTime);

	WindDirectionId windDirectionIdEnd(get_wind_direction_id_at(theParameters, 
																periodEnd,
																theParameters.theVar));

	WeatherPeriod periodStartPlus1(periodStartTimePlus1, periodStartTimePlus1);
	WindDirectionId windDirectionIdPlus1(get_wind_direction_id_at(theParameters, 
																  periodStartPlus1,
																  theParameters.theVar));
	bool useAlkaenPhrase(get_period_length(eventPeriod) >= 6);
	// if only one hour in the beginning is VAIHTELEVA, ignore it
	if(firstSentenceInTheStory && windDirectionIdPlus1 == VAIHTELEVA)
	  {
		sentence << windDirectionSentence(VAIHTELEVA)
				 << windSpeedIntervalSentence(periodStart, 
											  USE_AT_ITS_STRONGEST_PHRASE)
				 << Delimiter(COMMA_PUNCTUATION_MARK)
				 << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
				 << getTimePhrase(periodEnd, useAlkaenPhrase)
				 << windDirectionSentence(windDirectionIdEnd)
				 << windSpeedIntervalSentence(periodEnd,
											  USE_AT_ITS_STRONGEST_PHRASE);
	  }
	else
	  {
		if(firstSentenceInTheStory)
		  {
			WeatherPeriod period(periodStartTimePlus1, eventPeriod.localEndTime());
			sentence << windDirectionSentence(windDirectionIdEnd);
			sentence << windSpeedIntervalSentence(eventPeriod, USE_AT_ITS_STRONGEST_PHRASE);
		  }
		else
		  {
			sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
					 << getTimePhrase(periodEnd, useAlkaenPhrase)
					 << windDirectionSentence(windDirectionIdEnd);
		  }
	  }

#endif

	bool useAlkaenPhrase(get_period_length(eventPeriod) >= 6);
	WeatherPeriod periodStart(eventPeriod.localStartTime(),
							  eventPeriod.localStartTime());

	WindDirectionId windDirectionIdAvg(get_wind_direction_id_at(theParameters, 
																eventPeriod,
																theParameters.theVar));

	if(useAlkaenPhrase)
	  sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
			   << getTimePhrase(periodStart, USE_ALKAEN_PHRASE)
			   << windDirectionSentence(windDirectionIdAvg);
	else
	  sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
			   << getTimePhrase(eventPeriod, DONT_USE_ALKAEN_PHRASE)
			   << windDirectionSentence(windDirectionIdAvg);
	
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
													  const bool& startsToWeakenStrengthenPhraseUsed,
													  const WindEventId& eventId) const
  {
	Sentence sentence;

	WeatherPeriod speedEventPeriod(eventPeriodDataItem.theWindSpeedChangePeriod);
	int periodLength(wind_forecast_period_length(speedEventPeriod));
	WeatherPeriod periodBeg(speedEventPeriod.localStartTime(), 
							speedEventPeriod.localStartTime());
	WeatherPeriod periodEnd(speedEventPeriod.localEndTime(), 
							speedEventPeriod.localEndTime());
	
	bool useTimePhrase = ((theParameters.theAlkaenPhraseUsed 
						   || startsToWeakenStrengthenPhraseUsed) && !fit_into_short_day_part(speedEventPeriod));
	bool windIsDecreasing =  (eventId & TUULI_HEIKKENEE);

	if(!wind_speed_differ_enough(theParameters.theSources,
								 theParameters.theArea,
								 speedEventPeriod,
								 theParameters.theVar,
								 theParameters.theWindDataVector))
	  {
		if(useTimePhrase && !firstSentenceInTheStory)
		  {
		
			  sentence << getTimePhrase(periodEnd, eventPeriodDataItem.theLongTermSpeedChangeFlag)
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
					 << getTimePhrase(periodEnd, eventPeriodDataItem.theLongTermSpeedChangeFlag)
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
							 << getTimePhrase(periodEnd, eventPeriodDataItem.theLongTermSpeedChangeFlag)
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
								
				WeatherResult lowerLimit1(kFloatMissing, 0), 
				  lowerLimit2(kFloatMissing, 0), 
				  upperLimit1(kFloatMissing, 0), 
				  upperLimit2(kFloatMissing, 0);

				getSpeedIntervalLimits(currentPeriodEnd,
									   lowerLimit1,
									   upperLimit1);

				getSpeedIntervalLimits(nextPeriodEnd,
									   lowerLimit2,
									   upperLimit2);

				int lowerLimit1Int(static_cast<int>(lowerLimit1.value())), 
				  lowerLimit2Int(static_cast<int>(lowerLimit2.value())), 
				  upperLimit1Int(static_cast<int>(upperLimit1.value())), 
				  upperLimit2Int(static_cast<int>(upperLimit2.value()));

				if(abs(lowerLimit2Int-lowerLimit1Int) <= 1 && 
				   abs(upperLimit2Int-upperLimit1Int) <= 1)
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
					sentence << Delimiter(COMMA_PUNCTUATION_MARK);
					if(lastPeriod)
					  {
						Sentence timePhrase;
						if(get_adjusted_part_of_the_day_id(speedChangePeriods[i]) != MISSING_PART_OF_THE_DAY_ID)
						  timePhrase << getTimePhrase(speedChangePeriods[i], eventPeriodDataItem.theLongTermSpeedChangeFlag);
						if(timePhrase.empty())
						  timePhrase << getTimePhrase(speedChangePeriods[i], USE_ALKAEN_PHRASE);
						sentence << timePhrase;
					  }
					else
					  {
						sentence << getTimePhrase(currentPeriodEnd, DONT_USE_ALKAEN_PHRASE);
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
										 && !fit_into_short_day_part(thePeriod));

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
											 && !fit_into_short_day_part(shortenedPeriod));
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

	WeatherPeriod speedEventPeriod(eventPeriodDataItem.theWindSpeedChangePeriod);//thePeriod);
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
		float previousThresholdMaxWind(kFloatMissing);
		for(unsigned int i = begIndex; i < theParameters.theWindDataVector.size(); i++)
		  {
			const WindDataItemUnit& windDataItem = (*theParameters.theWindDataVector[i])(theParameters.theArea.type());
			if(!is_inside(windDataItem.thePeriod.localStartTime(), speedEventPeriod))
			  break;
			
			if(i == begIndex)
			  {
				// speed at the beginning of the period is reported
				reportingIndexes.push_back(i);
				previousThresholdMaxWind = windDataItem.theEqualizedMaximumWind.value();
			  }
			else if(abs(windDataItem.theEqualizedMaximumWind.value() - previousThresholdMaxWind) > 4.0 &&
					((round(previousThresholdMaxWind) > KOHTALAINEN_LOWER_LIMIT+1.5 && (eventId & TUULI_HEIKKENEE)) || 
					 (round(windDataItem.theEqualizedMaximumWind.value()) > KOHTALAINEN_LOWER_LIMIT+1.5  && (eventId & TUULI_VOIMISTUU))))
			  {
				// speed is reported when it has changed 4.0 from the previous raporting point
				reportingIndexes.push_back(i);
				previousThresholdMaxWind = windDataItem.theEqualizedMaximumWind.value();
			  }			  
			endIndex = i;
		  }

		if(reportingIndexes.size() > 1)
		  {
			const WindDataItemUnit& windDataItemLast = (*theParameters.theWindDataVector[endIndex])(theParameters.theArea.type());
			WeatherPeriod previousPeriod((*theParameters.theWindDataVector[reportingIndexes[0]])(theParameters.theArea.type()).thePeriod);
			for(unsigned int i = 0; i < reportingIndexes.size(); i++)
			  {
				unsigned int index = reportingIndexes[i];
				const WindDataItemUnit& windDataItem = (*theParameters.theWindDataVector[index])(theParameters.theArea.type());
				if(i < reportingIndexes.size() - 1 &&
				   abs(windDataItem.theEqualizedMaximumWind.value() - windDataItemLast.theEqualizedMaximumWind.value()) > 4.0)
				  {
					// dont report two times on the same period
					if(retVector.size() > 0 &&
					   get_part_of_the_day_id_large(windDataItem.thePeriod) == get_part_of_the_day_id_large(previousPeriod))
					  {
						retVector[retVector.size()-1] = windDataItem.thePeriod;
					  }
					else
					  {
						retVector.push_back(windDataItem.thePeriod);
					  }
					previousPeriod = windDataItem.thePeriod;
				  }
				else
				  {					
					WeatherPeriod period(windDataItem.thePeriod.localEndTime(), //windDataItem.thePeriod.localStartTime(), 
										 windDataItemLast.thePeriod.localEndTime());
					// dont report two times on the same period
					if(retVector.size() > 0 &&
					   get_part_of_the_day_id_large(period) == get_part_of_the_day_id_large(previousPeriod))
					  {
						retVector[retVector.size()-1] = period;
					  }
					else
					  {
						retVector.push_back(period);/*WeatherPeriod(windDataItem.thePeriod.localStartTime(), 
													  windDataItemLast.thePeriod.localEndTime()));*/
					  }
				  }
			  }
		  }
	  }

	if(retVector.size() == 0)
	  {
		retVector.push_back(speedEventPeriod);
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
	int begLowerLimit(0), 
	  begUpperLimit(0), 
	  endLowerLimit(0), 
	  endUpperLimit(0);
	float changeRatePerHour(0.0);

	get_wind_speed_interval_parameters(changePeriod,
									   theParameters.theArea,
									   theParameters.theWindDataVector,
									   begLowerLimit,
									   begUpperLimit,
									   endLowerLimit,
									   endUpperLimit,
									   changeRatePerHour);

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
											const WeatherResult & theMaxSpeed,
											const WeatherResult & theMedianSpeed,
											const string & theVariable, 
											bool theUseAtItsStrongestPhrase) const
  {
	using Settings::optional_int;

	Sentence sentence;

	const int maxvalue = static_cast<int>(round(theMaxSpeed.value()));
	const int medianvalue = static_cast<int>(round(theMedianSpeed.value()));
	
	int peakWind = -1;

	if(theParameters.theMetersPerSecondFormat == TEXTPHRASE_WORD)
	  {
		sentence << *UnitFactory::create(MetersPerSecond, maxvalue) << TUULTA_WORD;
	  }
	else
	  {
		int intervalMin(medianvalue);
		int intervalMax(maxvalue);
		get_wind_speed_interval(thePeriod,
								theParameters.theArea,									
								theParameters.theWindDataVector,
								intervalMin,
								intervalMax);

		if(abs(intervalMax - intervalMin) < theParameters.theMinIntervalSize)
		  {
			sentence << intervalMin
					 << *UnitFactory::create(MetersPerSecond);				
			thePreviousRangeBeg = intervalMin;
		  }
		else
		  {

			// TODO: make the interval configurable
			// if size of the interval is 7 or more, we use kovimmillaan-phrase
			// if size of the interval is 6, we increase lower limit by 1
			if(intervalMax - intervalMin > 6)
			  {
				peakWind = maxvalue;
				intervalMax = intervalMin + 5;
			  }
			else if(intervalMax - intervalMin == 6)
			  {
				intervalMin++;
			  }

			if(intervalMax == 2)
			  intervalMin = 0;

			sentence << IntegerRange(intervalMin , intervalMax, theParameters.theRangeSeparator)
					 << *UnitFactory::create(MetersPerSecond);
			
			thePreviousRangeBeg = intervalMin;
			thePreviousRangeEnd = intervalMax;

			if(peakWind > intervalMax)
			  {
				if(theUseAtItsStrongestPhrase)
				  {
					sentence << Delimiter(COMMA_PUNCTUATION_MARK) 
							 << KOVIMMILLAAN_PHRASE
							 << peakWind
							 << *UnitFactory::create(MetersPerSecond);
				  }
			  }
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
	WindDataItemUnit dataItemMinMax((*theParameters.theWindDataVector[0])(theParameters.theArea.type()));

	bool firstRound(true);

	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		dataItem = (*theParameters.theWindDataVector[i])(theParameters.theArea.type());
		
		if(is_inside(dataItem.thePeriod.localStartTime(), thePeriod))
		  {
			if(firstRound)
			  {
				dataItemMinMax.theEqualizedMaximumWind = dataItem.theEqualizedMaximumWind;
				dataItemMinMax.theEqualizedMedianWind = dataItem.theEqualizedMedianWind;
				firstRound = false;
				continue;
			  }

			if (dataItem.theEqualizedMaximumWind.value() > 
				dataItemMinMax.theEqualizedMaximumWind.value())
			  {
				dataItemMinMax.theEqualizedMaximumWind = dataItem.theEqualizedMaximumWind;
			  }
			if (dataItem.theEqualizedMedianWind.value() < 
				dataItemMinMax.theEqualizedMedianWind.value())
			  {
				dataItemMinMax.theEqualizedMedianWind = dataItem.theEqualizedMedianWind;
			  }
		  }
	  }

	// no data found for thePeriod
	if(firstRound)
	  return false;

	lowerLimit = dataItemMinMax.theEqualizedMedianWind;
	upperLimit = dataItemMinMax.theEqualizedMaximumWind;

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

	if(getSpeedIntervalLimits(thePeriod, lowerLimit, upperLimit))
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
	theWindDirectionIdBeg = get_wind_direction_id_at(theParameters,
													 thePeriod.localStartTime(),
													 theParameters.theVar);

	theWindDirectionIdEnd = get_wind_direction_id_at(theParameters,
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
	WeatherResult windMaxSpeedBeg(kFloatMissing, kFloatMissing);
	WeatherResult windMaxSpeedEnd(kFloatMissing, kFloatMissing);
	
	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		WindDataItemUnit& item = theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
		if(item.thePeriod.localStartTime() == thePeriod.localStartTime())
		  {
			windDirectionBeg = item.theEqualizedWindDirection;
			windMaxSpeedBeg = item.theEqualizedMaximumWind;
		  }
		else if(item.thePeriod.localStartTime() == thePeriod.localEndTime())
		  {
			windDirectionEnd = item.theEqualizedWindDirection;
			windMaxSpeedEnd = item.theEqualizedMaximumWind;
			break;
		  }
	  }

	return wind_direction_turns(windDirectionBeg,
								windDirectionEnd,
								windMaxSpeedBeg,
								windMaxSpeedEnd,
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
			return (item.theEqualizedMaximumWind.value() >= 20.0 &&
					item.theGustSpeed.value() - item.theEqualizedMaximumWind.value() >= theParameters.theGustyWindMaximumWindDifference);
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
			speedSum += item.theEqualizedMaximumWind.value();
			gustSpeedSum += item.theGustSpeed.value();
			n++;
		  }
	  }

	if(n > 0)
	  {
		float avgMaximumWind(speedSum / n);
		float avgGustSpeed(gustSpeedSum / n);		
		return (avgMaximumWind >= 20.0 &&
				avgGustSpeed - avgMaximumWind >= theParameters.theGustyWindMaximumWindDifference);
	  }

	return false;
 }

  pair<WeatherResult, WindDirectionId> get_wind_direction_pair_at(const wo_story_params& theParameters,
																  const NFmiTime& pointOfTime,
																  const string& var)
  {
	WeatherResult directionValue(kFloatMissing, 0.0);
	WindDirectionId directionId(MISSING_WIND_DIRECTION_ID);

	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		WindDataItemUnit& item = theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
		if(item.thePeriod.localStartTime() == pointOfTime)
		  {
			directionValue = item.theEqualizedWindDirection;
			directionId = wind_direction_id(item.theEqualizedWindDirection, 
											item.theEqualizedMaximumWind,
											var);
		  }
	  }

	return make_pair(directionValue, directionId);
  }

  pair<WeatherResult, WindDirectionId> get_wind_direction_pair_at(const wo_story_params& theParameters,
																  const WeatherPeriod& period,
																  const string& var)
  {
	float speedSum(0.0);
	float speedStdDevSum(0.0);
	unsigned int n(0);
	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		WindDataItemUnit& item = theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
		if(is_inside(item.thePeriod.localStartTime(), period))
		  {			
			speedSum += item.theEqualizedMaximumWind.value();
			speedStdDevSum += item.theEqualizedMaximumWind.error();
			n++;
		  }
	  }
	float avgMaximumWind(speedSum / n);
	float avgStdDev(speedStdDevSum / n);
	WeatherResult resultSpeed(avgMaximumWind, avgStdDev);

	WeatherResult resultDirection = mean_wind_direction(theParameters.theSources,
														theParameters.theArea,
														period,
														resultSpeed,
														var);

	return make_pair(resultDirection, wind_direction_id(resultDirection, resultSpeed, var));
  }

  WeatherResult get_wind_direction_at(const wo_story_params& theParameters,
									  const NFmiTime& pointOfTime,
									  const string& var)
  {
	return(get_wind_direction_pair_at(theParameters,
									  pointOfTime,
									  var).first);	
  }

  WeatherResult get_wind_direction_at(const wo_story_params& theParameters,
									  const WeatherPeriod& period,
									  const string& var)
  {
	return(get_wind_direction_pair_at(theParameters,
									  period,
									  var).first);
  }

  WindDirectionId get_wind_direction_id_at(const wo_story_params& theParameters,
										   const NFmiTime& pointOfTime,
										   const string& var)
  {
	return(get_wind_direction_pair_at(theParameters,
									  pointOfTime,
									  var).second);	
  }

  WindDirectionId get_wind_direction_id_at(const wo_story_params& theParameters,
										   const WeatherPeriod& period,
										   const string& var)
  {
	return(get_wind_direction_pair_at(theParameters,
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
	float changeRatePerHour(kFloatMissing);
   
	NFmiTime startTime(thePeriod.localStartTime());
	NFmiTime endTime(thePeriod.localEndTime());
	WeatherPeriod period(startTime, endTime);
	
	get_wind_speed_interval_parameters(thePeriod,
									   theArea,									   
									   windDataVector,
									   begLowerLimit,
									   begUpperLimit,
									   endLowerLimit,
									   endUpperLimit,
									   changeRatePerHour);

	float lowerDiff = abs(endLowerLimit - begLowerLimit);
	float upperDiff = abs(endUpperLimit - begUpperLimit);

	return (lowerDiff + upperDiff >= 3.0);
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
			upperLimit = static_cast<int>(round(windDataItem.theEqualizedMaximumWind.value()));
			found = true;
			break;
		  }
	  }
	/*
	  if(!found)
	  {
	  cout << "pointOfTime: " << pointOfTime << endl;
	  cout << "whole period: " << ((*windDataVector[0])(areaType)).thePeriod.localStartTime() << "..."
	  << ((*windDataVector[windDataVector.size()-1])(areaType)).thePeriod.localEndTime();
	  }
	*/
	  
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
	bool found(false);
	for(unsigned int i = 0; i < windDataVector.size(); i++)
	  {
		const WindDataItemUnit& windDataItem = (*windDataVector[i])(area.type());
			
		if(is_inside(windDataItem.thePeriod,period))
		  {
			if(!firstMatchProcessed)
			  {
				minValue = windDataItem.theEqualizedMedianWind.value();
				maxValue = windDataItem.theEqualizedMaximumWind.value();
				firstMatchProcessed = true;
				found = true;
			  }
			else
			  {
				if(windDataItem.theEqualizedMedianWind.value() < minValue)
				  minValue = windDataItem.theEqualizedMedianWind.value();
				if(windDataItem.theEqualizedMaximumWind.value() > maxValue)
				  maxValue = windDataItem.theEqualizedMaximumWind.value();
				found = true;
			  }
		  }
	  }
	lowerLimit = static_cast<int>(round(minValue));
	upperLimit = static_cast<int>(round(maxValue));
	/*
	  if(!found)
	  {
	  cout << "period: " << period.localStartTime() << "..." 
	  << period.localEndTime() << endl;
	  cout << "whole period: " << ((*windDataVector[0])(areaType)).thePeriod.localStartTime() << "..."
	  << ((*windDataVector[windDataVector.size()-1])(areaType)).thePeriod.localEndTime();
	  }
	*/
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

	for(unsigned int i = 0; i < windSpeedDistributionVector.size(); i++)
	  {
		WeatherResult cumulativeShare(windSpeedDistributionVector[i].second);
		windSpeedDistributionVector[i].second = WeatherResult(cumulativeShare.value() / counter, 0.0);
	  }

	const int maxvalue = upperLimit;
	const int minvalue = lowerLimit;

	// subtract 1 m/s from the lowest limit
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

} // namespace TextGen
