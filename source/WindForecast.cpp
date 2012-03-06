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
	thePreviousDayNumber(parameters.theForecastPeriod.localStartTime().GetWeekday())
  {
  }

  Paragraph WindForecast::getWindStory(const WeatherPeriod& thePeriod) const
  {
	Paragraph paragraph;

	WindEventId eventIdPrevious(MISSING_WIND_EVENT);
	WindDirectionId windDirectionIdPrevious(MISSING_WIND_DIRECTION_ID);

	for(unsigned int i = 0; i < theParameters.theMergedWindEventPeriodVector.size(); i++)
	  {
		Sentence sentence;
		const WindEventPeriodDataItem& eventPeriodItem = *(theParameters.theMergedWindEventPeriodVector[i]);
		WindEventId eventId = eventPeriodItem.theWindEvent;
		WindEventId nextWindEventId(MISSING_WIND_EVENT);
		theParameters.theAlkaenPhraseUsed = false;

		const WeatherPeriod& eventPeriod = eventPeriodItem.thePeriod;

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
				
		//		part_of_the_day_id partOfTheDayBeg(get_part_of_the_day_id(eventPeriod.localStartTime()));
		//		part_of_the_day_id partOfTheDayEnd(get_part_of_the_day_id(eventPeriod.localEndTime()));
		bool same_part_of_the_day(get_part_of_the_day_id(eventPeriod.localStartTime()) ==
								  get_part_of_the_day_id(eventPeriod.localEndTime()) &&
								  eventPeriod.localStartTime().GetDay() == eventPeriod.localEndTime().GetDay());
				  
		bool firstSentenceInTheStory = paragraph.empty();

		/*
		if(i > 0)
		  {
			WindEventPeriodDataItem* previousEventPeriodItem = theParameters.theMergedWindEventPeriodVector[i-1];
		  }
		*/
		if(i < theParameters.theMergedWindEventPeriodVector.size() - 1)
		  {
			WindEventPeriodDataItem* nextEventPeriodItem = theParameters.theMergedWindEventPeriodVector[i+1];
			nextWindEventId = nextEventPeriodItem->theWindEvent;
		  }
		
		switch(eventId)
		  {
		  case TUULI_HEIKKENEE:
		  case TUULI_VOIMISTUU:
			{
			  bool smallChange(false);
			  bool gradualChange(false);
			  bool fastChange(false);
			  std::string changeAttributeStr(EMPTY_STRING);
			  if(!getWindSpeedChangeAttribute(eventPeriod, changeAttributeStr,
											  smallChange, gradualChange, fastChange))
				{
				  return paragraph;
				}

			  bool useAlkaaHeiketaVoimistuaPhrase(false);

			  if(firstSentenceInTheStory)
				{
				  sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
						   << EMPTY_STRING
						   << changeAttributeStr
						   << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_WORD : VOIMISTUVAA_WORD)
						   << windDirectionSentence(windDirectionIdAvg);
				}
			  else
				{
				  useAlkaaHeiketaVoimistuaPhrase = (!smallChange &&
													wind_forecast_period_length(eventPeriod) > 6 && 
													!same_part_of_the_day);
				  if(useAlkaaHeiketaVoimistuaPhrase)
					{
					  if((eventIdPrevious & TUULI_VOIMISTUU && eventId == TUULI_VOIMISTUU) ||
						 (eventIdPrevious & TUULI_HEIKKENEE && eventId == TUULI_HEIKKENEE))
						{
						  sentence << ILTAPAIVALLA_EDELLEEN_HEIKKENEVAA_POHJOISTUULTA
								   << getTimePhrase(eventPeriod, USE_ALKAEN_PHRASE)
								   << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_WORD : VOIMISTUVAA_WORD)
								   << windDirectionSentence(windDirectionIdAvg);
						}
					  else
						{
						  sentence << (eventId == TUULI_HEIKKENEE ? ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI :
									   ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI)
								   << getTimePhrase(eventPeriod, DONT_USE_ALKAEN_PHRASE)
								   << TUULI_WORD
								   << changeAttributeStr;
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
								   << windDirectionSentence(windDirectionIdAvg);
						}
					  else
						{
						  sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_TUULTA_COMPOSITE_PHRASE
								   << getTimePhrase(eventPeriod, DONT_USE_ALKAEN_PHRASE)
								   << changeAttributeStr
								   << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_TUULTA_PHRASE : VOIMISTUVAA_TUULTA_PHRASE);
						}
					}
				}
			  sentence << decreasingIncreasingInterval(eventPeriodItem,
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
													  windDirectionIdPrevious);
			}
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI:
			{
			  if(firstSentenceInTheStory)
				{
				  sentence << POHJOISTUULTA_INTERVALLI_MS_JOKA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
						   << windDirectionSentence(windDirectionIdBeg)
						   << windSpeedIntervalSentence(eventPeriod, 
														DONT_USE_AT_ITS_STRONGEST_PHRASE);
				}
			  else
				{
				  if(eventPeriodItem.theTransientDirectionChangeFlag)
					sentence << ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE
							 << getTimePhrase(eventEndPeriod, DONT_USE_ALKAEN_PHRASE);
				  else
					sentence << ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
							 << getTimePhrase(eventEndPeriod, DONT_USE_ALKAEN_PHRASE);
				}
			}
			break;
		  case TUULI_KAANTYY_JA_VOIMISTUU:
		  case TUULI_KAANTYY_JA_HEIKKENEE:
			{
			  bool useAlkaaHeiketaVoimistuaPhrase(false);
			  bool appendDecreasingIncreasingInterval(true);

			  if(!wind_speed_differ_enough(theParameters.theSources,
										   theParameters.theArea,
										   eventPeriod,
										   theParameters.theVar,
										   theParameters.theWindDataVector))
				{
				  sentence << windDirectionChangeSentence(theParameters,
														  eventPeriod,
														  firstSentenceInTheStory,
														  windDirectionIdPrevious);
				  appendDecreasingIncreasingInterval = false;
				}
			  else
				{

				  if(eventIdPrevious == TUULI_VOIMISTUU)
					{
					  sentence << windDirectionAndSpeedChangesSentence(theParameters,
																	   eventPeriod,
																	   firstSentenceInTheStory,
																	   eventIdPrevious,
																	   eventId,
																	   windDirectionIdPrevious,
																	   useAlkaaHeiketaVoimistuaPhrase);
					}
				  else
					{
					  /*
						if(windDirectionIdBeg == VAIHTELEVA ||
						windDirectionIdBeg == MISSING_WIND_DIRECTION_ID)
						{
						sentence << directedWindSentenceAfterVaryingWind(theParameters,
						eventPeriod,
						firstSentenceInTheStory);
						appendDecreasingIncreasingInterval = false;
						}
						else
					  */
					  {
						sentence << windDirectionAndSpeedChangesSentence(theParameters,
																		 eventPeriod,
																		 firstSentenceInTheStory,
																		 eventIdPrevious,
																		 eventId,
																		 windDirectionIdPrevious,
																		 useAlkaaHeiketaVoimistuaPhrase);
					  }
					}
				}

			  if(!sentence.empty())
				{
				  if(appendDecreasingIncreasingInterval)
					sentence << decreasingIncreasingInterval(eventPeriodItem,
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
			  if(eventIdPrevious == TUULI_HEIKKENEE)
				{
				  if(eventPeriodItem.theTransientDirectionChangeFlag)
					{
					  sentence << ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE
							   << getTimePhrase(eventStartPeriod, !firstSentenceInTheStory);
					}
				  else
					{
					  sentence << ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE
							   << getTimePhrase(eventStartPeriod, !firstSentenceInTheStory);
					}
				}
			  else
				{
				  if(firstSentenceInTheStory)
					{
					  if(eventPeriodItem.theTransientDirectionChangeFlag)
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
					  if(eventPeriodItem.theTransientDirectionChangeFlag)
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
			  sentence << decreasingIncreasingInterval(eventPeriodItem,
													   firstSentenceInTheStory,
													   useAlkaaHeiketaVoimistuaPhrase,
													   eventId);
			}
			break;
		  case TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU:
			{
			  if(eventIdPrevious == TUULI_VOIMISTUU)
				{
				  if(eventPeriodItem.theTransientDirectionChangeFlag)
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
					  if(eventPeriodItem.theTransientDirectionChangeFlag)
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
					  if(eventPeriodItem.theTransientDirectionChangeFlag)
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
			  sentence << decreasingIncreasingInterval(eventPeriodItem,
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
				  sentence << windDirectionSentence(windDirectionIdAvg);
				  sentence << windSpeedIntervalSentence(eventPeriod, 
														USE_AT_ITS_STRONGEST_PHRASE);
				}
			  else if(eventIdPrevious == TUULI_MUUTTUU_VAIHTELEVAKSI && 
					  i < theParameters.theMergedWindEventPeriodVector.size() - 1)
				{
				  sentence << windDirectionChangeSentence(theParameters,
														  eventPeriod,
														  firstSentenceInTheStory,
														  windDirectionIdPrevious);
				}
			}
			break;
		  default:
			break;
		  };

		if(!sentence.empty())
		  {
			eventIdPrevious = eventId;
			windDirectionIdPrevious = wind_direction_id(eventPeriodItem.thePeriodEndDataItem.theEqualizedWindDirection, 
														eventPeriodItem.thePeriodEndDataItem.theEqualizedMaximumWind,
														theParameters.theVar);
		  }

		paragraph << sentence;

	  } // for


	return paragraph;
  }

  Sentence WindForecast::windDirectionChangeSentence(const wo_story_params& theParameters,
													 const WeatherPeriod& eventPeriod,
													 const bool& firstSentenceInTheStory,
													 const WindDirectionId& windDirectionIdPrevious) const
  {
	Sentence sentence;

	WindDirectionId windDirectionIdBeg(MISSING_WIND_DIRECTION_ID);
	WindDirectionId windDirectionIdEnd(MISSING_WIND_DIRECTION_ID);
	WindDirectionId windDirectionIdAvg(get_wind_direction_id_at(theParameters,
																eventPeriod,
																theParameters.theVar));
	

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
			sentence << POHJOISTUULTA_INTERVALLI_MS_JOKA_KAANTYY_ETELAAN_COMPOSITE_PHRASE
					 << windDirectionSentence(windDirectionIdBeg)
					 << windSpeedIntervalSentence(eventPeriod, 
												  DONT_USE_AT_ITS_STRONGEST_PHRASE)
					 << getWindDirectionTurntoString(windDirectionIdEnd);
		  }
	  }
	else
	  {
		bool useAlkaenPhrase(get_period_length(eventPeriod) >= 6);
		if(windDirectionIdPrevious == VAIHTELEVA)
		  {
			if(windDirectionIdEnd != VAIHTELEVA)
			  sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
					   << getTimePhrase(eventPeriod, useAlkaenPhrase)
					   << windDirectionSentence(windDirectionIdAvg);
		  }
		else
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
															  bool& useAlkaaHeiketaVoimistuaPhrase) const
  {
	Sentence sentence;
	
	WindDirectionId windDirectionIdBeg(MISSING_WIND_DIRECTION_ID);
	WindDirectionId windDirectionIdEnd(MISSING_WIND_DIRECTION_ID);
	getWindDirectionBegEnd(eventPeriod, windDirectionIdBeg, windDirectionIdEnd);
	WindDirectionId windDirectionIdAvg(get_wind_direction_id_at(theParameters,
																eventPeriod,
																theParameters.theVar));

	/*
	  cout << "eventPeriod: " << eventPeriod.localStartTime() << "..." << eventPeriod.localEndTime() << endl;
	  cout << "windDirectionIdBeg: " << wind_direction_string(windDirectionIdBeg) << endl;
	  cout << "windDirectionIdEnd: " << wind_direction_string(windDirectionIdEnd) << endl;
	*/

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
	bool currentWindIsIncreasing =  (currentWindEventId & TUULI_VOIMISTUU);

	if(previousWindEventId & TUULI_VOIMISTUU || previousWindEventId & TUULI_HEIKKENEE)
	  {
		if(previousWindDirectionId == VAIHTELEVA)
		  {
			// TODO: handle k‰‰ntyy
			sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
					 << getTimePhrase(eventPeriod, useAlkaenPhrase)
					 << changeAttributeStr
					 << (currentWindIsIncreasing ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
					 << windDirectionSentence(windDirectionIdEnd);
		  }
		else
		  {
			if(previousWindEventId & TUULI_VOIMISTUU)
			  {
				if(currentWindEventId & TUULI_VOIMISTUU)
				  sentence << ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_JA_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE;
				else
				  sentence << ILTAPAIVALLA_TUULI_HEIKKENEE_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE;
			  }
			else
			  {
				if(currentWindEventId & TUULI_HEIKKENEE)
				  sentence << ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE;
				else
				  sentence << ILTAPAIVALLA_TUULI_VOIMISTUU_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE;
			  }

			sentence << getTimePhrase(eventPeriod, useAlkaenPhrase)
					 << getWindDirectionTurntoString(windDirectionIdEnd);
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
					sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
							 << timePhrase
							 << changeAttributeStr
							 << (currentWindIsIncreasing ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
							 << windDirectionSentence(windDirectionIdAvgPlus1);
				  }
				else
				  {
					sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_JOKA_KAANTYY_POHJOISEEN_COMPOSITE_PHRASE
							 << timePhrase
							 << changeAttributeStr
							 << (currentWindIsIncreasing ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
							 << windDirectionSentence(windDirectionIdBegPlus1)
							 << getWindDirectionTurntoString(windDirectionIdEnd);
				  }
			  }
			else
			  {
				bool directionTurns = windDirectionTurns(WeatherPeriod(begTimePlus1, eventPeriod.localEndTime()));
				if(directionTurns)
				  {
					sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_JOKA_KAANTYY_POHJOISEEN_COMPOSITE_PHRASE
							 << timePhrase
							 << changeAttributeStr
							 << (currentWindIsIncreasing ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
							 << windDirectionSentence(windDirectionIdBeg)
							 << getWindDirectionTurntoString(windDirectionIdEnd);
				  }
				else
				  {
					sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
							 << timePhrase
							 << changeAttributeStr
							 << (currentWindIsIncreasing ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
							 << windDirectionSentence(windDirectionIdAvg);
				  }
			  }
		  }
		else
		  {
			if(previousWindDirectionId == VAIHTELEVA)
			  {
				if(windDirectionIdBegPlus1 == windDirectionIdEnd)
				  {
					sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
							 << timePhrase
							 << changeAttributeStr
							 << (currentWindIsIncreasing ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
							 << windDirectionSentence(windDirectionIdAvgPlus1);
				  }
				else
				  {
					sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_JOKA_KAANTYY_POHJOISEEN_COMPOSITE_PHRASE
							 << timePhrase
							 << changeAttributeStr
							 << (currentWindIsIncreasing ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
							 << windDirectionSentence(windDirectionIdBegPlus1)
							 << getWindDirectionTurntoString(windDirectionIdEnd);
				  }
				
			  }
			else
			  {
				bool directionTurns = windDirectionTurns(WeatherPeriod(begTimePlus1, eventPeriod.localEndTime()));
				if(directionTurns)
				  {
					sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_JOKA_KAANTYY_POHJOISEEN_COMPOSITE_PHRASE
							 << timePhrase
							 << changeAttributeStr
							 << (currentWindIsIncreasing ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
							 << windDirectionSentence(windDirectionIdBeg)
							 << getWindDirectionTurntoString(windDirectionIdEnd);
				  }
				else
				  {
					sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
							 << timePhrase
							 << changeAttributeStr
							 << (currentWindIsIncreasing ? VOIMISTUVAA_WORD : HEIKKENEVAA_WORD)
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
	bool useAlkaenPhrase(get_period_length(eventPeriod));
	// if only one hour in the beginning is VAIHTELEVAT, ignore it
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
					 << getTimePhrase(periodEnd, !firstSentenceInTheStory)
					 << windDirectionSentence(windDirectionIdEnd);
		  }
	  }

	return sentence;
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

	WeatherPeriod speedEventPeriod(eventPeriodDataItem.thePeriod);
	int periodLength(wind_forecast_period_length(speedEventPeriod));
	WeatherPeriod periodBeg(speedEventPeriod.localStartTime(), 
							speedEventPeriod.localStartTime());
	WeatherPeriod periodEnd(speedEventPeriod.localEndTime(), 
							speedEventPeriod.localEndTime());
	
	bool useTimePhrase = ((theParameters.theAlkaenPhraseUsed 
						   || startsToWeakenStrengthenPhraseUsed) && !fit_into_short_day_part(speedEventPeriod));
	bool windIsDecreasing =  (eventId & TUULI_HEIKKENEE);


	WindDirectionId windDirectionIdBeg(MISSING_WIND_DIRECTION_ID);
	WindDirectionId windDirectionIdEnd(MISSING_WIND_DIRECTION_ID);
	/*
	WindDirectionId windDirectionIdAvg(get_wind_direction_id_at(theParameters,
																eventPeriod,
																theParameters.theVar));
	*/

	getWindDirectionBegEnd(speedEventPeriod, windDirectionIdBeg, windDirectionIdEnd);

	if(!wind_speed_differ_enough(theParameters.theSources,
								 theParameters.theArea,
								 speedEventPeriod,
								 theParameters.theVar,
								 theParameters.theWindDataVector))
	  {

		if(useTimePhrase && !firstSentenceInTheStory)
		  {
		
			sentence << Delimiter(COMMA_PUNCTUATION_MARK)
					 << getTimePhrase(periodEnd, eventPeriodDataItem.theLongTermSpeedChangeFlag)
					 << getWindDirectionSentence(windDirectionIdEnd)
					 << windSpeedIntervalSentence(periodEnd,
												  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
		  }
		else
		  {
			sentence << getWindDirectionSentence(windDirectionIdBeg);
			sentence << windSpeedIntervalSentence(periodBeg,
												  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
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
					  << getWindDirectionSentence(windDirectionIdBeg)
					  << windSpeedIntervalSentence(periodBeg, 
												   DONT_USE_AT_ITS_STRONGEST_PHRASE);

			sentence << Delimiter(COMMA_PUNCTUATION_MARK)
					 << getTimePhrase(periodEnd, eventPeriodDataItem.theLongTermSpeedChangeFlag)
					 << getWindDirectionSentence(windDirectionIdEnd)
					 << windSpeedIntervalSentence(periodEnd, 
												  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
		  }
		else
		  {
			if(periodLength <= 8 &&
			   get_part_of_the_day_id(speedEventPeriod.localStartTime()) ==
			   get_part_of_the_day_id(speedEventPeriod.localEndTime()))
			  {
				sentence << getWindDirectionSentence(windDirectionIdEnd);
				sentence << windSpeedIntervalSentence(periodEnd, 
													  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
			  }
			else
			  {
				if(wind_forecast_period_length(speedEventPeriod) > 12)
				  {
					sentence << reportIntermediateSpeed(speedEventPeriod);
				  }

				if(!sentence.empty())
				  {
					sentence << Delimiter(COMMA_PUNCTUATION_MARK)
							 << getTimePhrase(periodEnd, eventPeriodDataItem.theLongTermSpeedChangeFlag)
							 << getWindDirectionSentence(windDirectionIdEnd)
							 << windSpeedIntervalSentence(periodEnd,
														  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
				  }
				else
				  {
					sentence << getWindDirectionSentence(windDirectionIdEnd)
							 << windSpeedIntervalSentence(periodEnd,
														  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
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
					  << getWindDirectionSentence(windDirectionIdBeg)
					  << windSpeedIntervalSentence(speedChangePeriods[0], 
												   USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
			useTimePhrase = true;
		  }
		
		// Note! Speed at start is not reported: startting from index 1
		for(unsigned int i = 1; i < speedChangePeriods.size(); i++)
		  {
			WeatherPeriod currentPeriodEnd(speedChangePeriods[i].localEndTime(), 
										   speedChangePeriods[i].localEndTime());
			//			unsigned int lastItemIndex = (speedChangePeriods.size() - 1);
			bool lastPeriod = (i == (speedChangePeriods.size() - 1));
			part_of_the_day_id currentDayPart(get_part_of_the_day_id(currentPeriodEnd));
			WindDirectionId windDirectionIdAvg(get_wind_direction_id_at(theParameters,
																		speedChangePeriods[i],
																		theParameters.theVar));
			
			if(!lastPeriod)//i < lastItemIndex)
			  {
				WeatherPeriod nextPeriodEnd(speedChangePeriods[i+1].localEndTime(), 
											speedChangePeriods[i+1].localEndTime());

				part_of_the_day_id nextDayPart(get_part_of_the_day_id(nextPeriodEnd));

				// wind speed is mentioned only once per day part, for example we don't say
				// iltap‰iv‰ll‰ 10...13 m/s, iltap‰iv‰ll‰ 15...18
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
			
			if(!intervalSentence.empty())
			  {
				if(useTimePhrase)
				  {
					sentence << Delimiter(COMMA_PUNCTUATION_MARK);
					if(lastPeriod)
					  sentence << getTimePhrase(speedChangePeriods[i], eventPeriodDataItem.theLongTermSpeedChangeFlag);
					else
					  sentence << getTimePhrase(currentPeriodEnd, DONT_USE_ALKAEN_PHRASE);
				  }

				getWindDirectionBegEnd(speedChangePeriods[i], windDirectionIdBeg, windDirectionIdEnd);

				sentence << windDirectionSentence(windDirectionIdAvg)//getWindDirectionSentence(windDirectionIdEnd)
						 << intervalSentence;
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

  WindDirectionId WindForecast::getWindDirectionId(const WeatherPeriod& thePeriod,
												   const CompassType& theComapssType) const
  {
	for(unsigned int i = 0; i < theParameters.theWindDataVector.size() ; i++)
	  {
		WindDataItemUnit& item = theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
		if(!is_inside(item.thePeriod.localStartTime(), thePeriod))
		  continue;
		
		WindDirectionAccuracy accuracy(direction_accuracy(item.theWindDirection.error(), theParameters.theVar));

		int direction(eight_directions ? 
					  direction8th(item.theEqualizedWindDirection.value()) :
					  direction16th(item.theEqualizedWindDirection.value()));

		
		if(accuracy == bad_accuracy)
		  return VAIHTELEVA;
		else
		  return static_cast<WindDirectionId>(direction);
		
	  }

	return VAIHTELEVA;
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


  NFmiTime WindForecast::getWindDirectionTurningTime(const WeatherPeriod& period,
													 const WindDirectionId& directionId) const
  {
	NFmiTime retTime(period.localStartTime());
	unsigned int begIndex = UINT_MAX;
	unsigned int endIndex = theParameters.theWindDataVector.size();

	getPeriodStartAndEndIndex(period,
							  begIndex,
							  endIndex);

	if(begIndex != UINT_MAX)
	  {
		for(unsigned int i = begIndex+1; i < endIndex; i++)
		  {
			const WindDataItemUnit& windDataItemCurrent = (*theParameters.theWindDataVector[i])(theParameters.theArea.type());
			WindDirectionId windDirectionIdCurrent = wind_direction_id(windDataItemCurrent.theEqualizedWindDirection, 
																	   windDataItemCurrent.theEqualizedMaximumWind,
																	   theParameters.theVar);

			if(windDirectionIdCurrent == directionId)
			  {
				retTime = windDataItemCurrent.thePeriod.localStartTime();
				break;
			  }
		  }
	  }
	return retTime;
  }

  // returns the timestamp when the wind turns to the other direction
  // and stays away from the original direction till the end of period
  NFmiTime WindForecast::getWindDirectionTurningTime(const WeatherPeriod& period) const
  {
	NFmiTime retTime(period.localStartTime());
	unsigned int begIndex = UINT_MAX;
	unsigned int endIndex = theParameters.theWindDataVector.size();

	getPeriodStartAndEndIndex(period,
							  begIndex,
							  endIndex);

	if(begIndex != UINT_MAX)
	  {
		const WindDataItemUnit& windDataItemBeg = (*theParameters.theWindDataVector[begIndex])(theParameters.theArea.type());
		WindDirectionId windDirectionBeg = wind_direction_id(windDataItemBeg.theEqualizedWindDirection, 
															 windDataItemBeg.theEqualizedMaximumWind,
															 theParameters.theVar);

		for(unsigned int i = begIndex+1; i < endIndex; i++)
		  {
			const WindDataItemUnit& windDataItemCurrent = (*theParameters.theWindDataVector[i])(theParameters.theArea.type());
			WindDirectionId windDirectionCurrent = wind_direction_id(windDataItemCurrent.theEqualizedWindDirection,
																	 windDataItemCurrent.theEqualizedMaximumWind,
																	 theParameters.theVar);

			if(windDirectionCurrent != windDirectionBeg)
			  {
				retTime = windDataItemCurrent.thePeriod.localStartTime();
				break;
			  }
		  }
	  }
	return retTime;
  }

  Sentence WindForecast::getTimePhrase(const WeatherPeriod thePeriod,
									   const bool& useAlkaenPhrase) const
  {
	Sentence sentence;

	int forecastPeriodLength = wind_forecast_period_length(theParameters.theForecastPeriod);
			
	bool specifyDay = (thePreviousDayNumber != thePeriod.localStartTime().GetWeekday() &&
					   (forecastPeriodLength > 12 &&
						abs(theParameters.theForecastTime.DifferenceInHours(thePeriod.localStartTime())) > 6 &&
						(theParameters.theForecastTime.GetDay() < thePeriod.localStartTime().GetDay() ||
						 theParameters.theForecastTime.GetYear() < thePeriod.localStartTime().GetYear())));


	std::string dayPhasePhrase;


	theParameters.theAlkaenPhraseUsed = (wind_forecast_period_length(thePeriod) > 5 
										 && useAlkaenPhrase 
										 && !fit_into_short_day_part(thePeriod));

	sentence << get_time_phrase_large(thePeriod,
									  specifyDay,
									  theParameters.theVar, 
									  dayPhasePhrase,
									  useAlkaenPhrase);

	thePreviousDayNumber = thePeriod.localStartTime().GetWeekday();

	return sentence;
  }

  WindDirectionId WindForecast::getWindDirectionId(const WeatherPeriod& thePeriod) const
  {
	WindDirectionId retval(VAIHTELEVA);

	// for short-term forecasts (8 hours or less) use 16-direction compass, for long-term forecasts 8-direction compass
	if(thePeriod.localStartTime().DifferenceInHours(theParameters.theForecastTime) < 8)
	  retval = getWindDirectionId(thePeriod,
								  sixteen_directions);
	else
	  retval = getWindDirectionId(thePeriod,
								  eight_directions);
	
	return retval;
  }

  vector<WeatherPeriod> WindForecast::getWindSpeedReportingPoints(const WindEventPeriodDataItem& eventPeriodDataItem,
																  const bool& firstSentenceInTheStory,
																  const WindEventId& eventId) const
  {
	vector<WeatherPeriod> retVector;

	WeatherPeriod speedEventPeriod(eventPeriodDataItem.thePeriod);
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
			else if(abs(windDataItem.theEqualizedMaximumWind.value() - previousThresholdMaxWind) > theParameters.theWindSpeedThreshold && previousThresholdMaxWind < KOHTALAINEN_UPPER_LIMIT-2.0)
			  {
				// speed is reported when it has changed theParameters.theWindSpeedThreshold from the previous raporting point
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
				   abs(windDataItem.theEqualizedMaximumWind.value() - windDataItemLast.theEqualizedMaximumWind.value()) > theParameters.theWindSpeedThreshold)
				  {
					// dont report two times on the same period
					if(retVector.size() > 0 &&
					   get_part_of_the_day_id(windDataItem.thePeriod) == get_part_of_the_day_id(previousPeriod))
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
					WeatherPeriod period(windDataItem.thePeriod.localStartTime(), 
										 windDataItemLast.thePeriod.localEndTime());
					// dont report two times on the same period
					if(retVector.size() > 0 &&
					   get_part_of_the_day_id(period) == get_part_of_the_day_id(previousPeriod))
					  {
						retVector[retVector.size()-1] = period;
					  }
					else
					  {
						retVector.push_back(WeatherPeriod(windDataItem.thePeriod.localStartTime(), 
														  windDataItemLast.thePeriod.localEndTime()));
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

  // tuuli voi voimistua, heiket‰ tai k‰‰nty‰ usealla per‰kk‰isel‰ periodilla (k‰yr‰ polveilee, mutta ei k‰‰nny)
  // t‰m‰ funktio etsii oikean k‰‰nnˆskohdan
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

	get_wind_speed_interval_parameters(theParameters.theSources,
									   theParameters.theArea,
									   changePeriod,
									   theParameters.theVar,
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
		return false;
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
		if(abs(maxvalue - medianvalue) < theParameters.theMinIntervalSize)
		  {
			sentence << medianvalue
					 << *UnitFactory::create(MetersPerSecond);				
			thePreviousRangeBeg = medianvalue;
		  }
		else
		  {
			int intervalMin;
			int intervalMax;
			get_wind_speed_interval(theParameters.theSources,
									theParameters.theArea,
									thePeriod,
									theParameters.theVar,
									theParameters.theWindDataVector,
									intervalMin,
									intervalMax);

			/*
			  cout << "period: " << thePeriod.localStartTime() << "..." << thePeriod.localEndTime() << endl;
			  cout << "get_wind_speed_interval:min: " << intervalMin << endl;
			  cout << "get_wind_speed_interval:max: " << intervalMax << endl;
			*/

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

			// don't report (almost) the same interval
			//if(abs(intervalMin - thePreviousRangeBeg) >= 1 || abs(intervalMax - thePreviousRangeEnd) >= 1)
			{
				
			  sentence << IntegerRange(intervalMin , intervalMax, theParameters.theRangeSeparator)
					   << *UnitFactory::create(MetersPerSecond);

			  thePreviousRangeBeg = intervalMin;
			  thePreviousRangeEnd = intervalMax;
			}

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
 	double medianValueSum = 0.0;
	double medianErrorSum = 0.0;
	unsigned int counter = 0;

	WindDataItemUnit dataItem = (*theParameters.theWindDataVector[0])(theParameters.theArea.type());
	WindDataItemUnit dataItemMinMax = (*theParameters.theWindDataVector[0])(theParameters.theArea.type());

	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		dataItem = (*theParameters.theWindDataVector[i])(theParameters.theArea.type());
		
		if(is_inside(dataItem.thePeriod.localStartTime(), thePeriod))
		  {
			if (counter == 0 ||
				dataItem.theWindMaximum.value() > 
				dataItemMinMax.theWindMaximum.value())
			  {
				dataItemMinMax.theWindMaximum = dataItem.theWindMaximum;
			  }
			if (counter == 0 ||
				dataItem.theWindSpeedMin.value() < 
				dataItemMinMax.theWindSpeedMin.value())
			  {
				dataItemMinMax.theWindSpeedMin = dataItem.theWindSpeedMin;
			  }
			
			medianValueSum += dataItem.theWindSpeedMedian.value();
			medianErrorSum += dataItem.theWindSpeedMedian.error();
			counter++;
		  }
	  }

	if(counter == 0)
	  return false;

	// claculate average value of median time series
	WeatherResult medianResult(medianValueSum / counter, medianErrorSum / counter);

	lowerLimit = medianResult;
	upperLimit = dataItemMinMax.theWindMaximum;

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

	//Sentence WindForecast::findWindSpeedReportingTime(const WeatherPeriod& speedEventPeriod) const

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

  const float WindDataItemUnit::getWindSpeedShare(const float& theLowerLimit, const float& theUpperLimit) const
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
	float directionSum(0.0);
	float directionStdDevSum(0.0);
	float speedSum(0.0);
	float speedStdDevSum(0.0);
	float n(0.0);
	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		WindDataItemUnit& item = theParameters.theWindDataVector[i]->getDataItem(theParameters.theArea.type());
		if(is_inside(item.thePeriod.localStartTime(), period))
		  {
			directionSum += item.theEqualizedWindDirection.value();
			// if wind speed is > 7.0 m/s it can not be vaihteleva
			// and if we merge it with possibly weaker wind periods
			// we have to make sure that we dont end to the situation where
			// neither of the periods are vaihteleva but the merged period is
			if(item.theEqualizedMaximumWind.value() > 7.0 &&
			   item.theEqualizedWindDirection.error() > 45.0)
			  directionStdDevSum += 40.0;
			else
			  directionStdDevSum += item.theEqualizedWindDirection.error();
			speedSum += item.theEqualizedMaximumWind.value();
			speedStdDevSum += item.theEqualizedMaximumWind.error();		   
			n += 1.0;
		  }
	  }

	WeatherResult directionValue(kFloatMissing, 0.0);
	WindDirectionId directionId(MISSING_WIND_DIRECTION_ID);

	if(directionSum == 0.0)
	  return make_pair(directionValue, directionId);

	WeatherResult resultDirection(directionSum / n, directionStdDevSum / n);
	WeatherResult resultSpeed(speedSum / n, speedStdDevSum / n);

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

  void populate_windspeed_distribution_time_series(const AnalysisSources& theSources,
												   const WeatherArea& theArea,
												   const WeatherPeriod& thePeriod,
												   const string& theVar,
												   vector <pair<float, WeatherResult> >& theWindSpeedDistribution)
  {
	GridForecaster forecaster;

	float ws_lower_limit = 0.0;
	float ws_upper_limit = 1.0;

	while (ws_lower_limit < HIRMUMYRSKY_LOWER_LIMIT)
	  {
		RangeAcceptor acceptor;
		acceptor.lowerLimit(ws_lower_limit);
		if(ws_lower_limit < HIRMUMYRSKY_LOWER_LIMIT + 1)
		  acceptor.upperLimit(ws_upper_limit-0.0001);

				
		WeatherResult share =
		  forecaster.analyze(theVar+"::fake::tyyni::share",
							 theSources,
							 WindSpeed,
							 Mean,
							 Percentage,
							 theArea,
							 thePeriod,
							 DefaultAcceptor(),
							 DefaultAcceptor(),
							 acceptor);

		pair<float, WeatherResult> shareItem(ws_lower_limit, share);
		theWindSpeedDistribution.push_back(shareItem);

		ws_lower_limit += 1.0;
		ws_upper_limit += 1.0;

	  }
  }
  
  /*
  bool wind_direction_differ_enough(const wo_story_params& theParameters,
									const WeatherPeriod& thePeriod1,
									const WeatherPeriod& thePeriod2,
									const string& theVar)
  {
	bool retval(true);

	WindDirectionId direction1(get_wind_direction_id_at(theParameters,
														thePeriod1,
														theVar));
	WindDirectionId direction2(get_wind_direction_id_(theParameters,
													  thePeriod2,
													  theVar));

	if(direction1 != direction2)
	  {
		if(direction1 == POHJOINEN && 
		   }
	  }
  }
  */
	/*
	  POHJOINEN = 0x1,
	  POHJOINEN_KOILLINEN,
	  KOILLINEN,
	  ITA_KOILLINEN,
	  ITA,
	  ITA_KAAKKO,
	  KAAKKO,
	  ETELA_KAAKKO,
	  ETELA,
	  ETELA_LOUNAS,
	  LOUNAS,
	  LANSI_LOUNAS,
	  LANSI,
	  LANSI_LUODE,
	  LUODE,
	  POHJOINEN_LUODE,
	  POHJOISEN_PUOLEINEN,
	  KOILLISEN_PUOLEINEN,
	  IDAN_PUOLEINEN,
	  KAAKON_PUOLEINEN,
	  ETELAN_PUOLEINEN,
	  LOUNAAN_PUOLEINEN,
	  LANNEN_PUOLEINEN,
	  LUOTEEN_PUOLEINEN,
	  VAIHTELEVA,
	  MISSING_WIND_DIRECTION_ID

	*/

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
	  // end of previous period
	  //	startTime.ChangeByHours(-1);
	  WeatherPeriod period(startTime, endTime);
	
	  get_wind_speed_interval_parameters(theSources,
										 theArea,
										 thePeriod,
										 theVar,
										 windDataVector,
										 begLowerLimit,
										 begUpperLimit,
										 endLowerLimit,
										 endUpperLimit,
										 changeRatePerHour);

	  float lowerDiff = abs(endLowerLimit - begLowerLimit);
	  float upperDiff = abs(endUpperLimit - begUpperLimit);

	  /*
		cout << "period: " << thePeriod.localStartTime() << "..." << thePeriod.localEndTime() << endl;
		cout << "period start interval: " << begLowerLimit << "..." << begUpperLimit << endl;
		cout << "period end interval: " << endLowerLimit << "..." << endUpperLimit << endl;
		cout << "lowerDiff: " << lowerDiff << "; upperDiff: " << upperDiff << endl;
	  */

	  return (lowerDiff >= 2.0 || upperDiff >= 2.0);
	}

	void get_wind_speed_interval_parameters(const AnalysisSources& theSources,
											const WeatherArea& theArea,
											const WeatherPeriod& thePeriod,
											const string& theVar,
											const wind_data_item_vector& windDataVector,
											int& begLowerLimit,
											int& begUpperLimit,
											int& endLowerLimit,
											int& endUpperLimit,
											float& changeRatePerHour)
	{
	  get_wind_speed_interval(theSources,
							  theArea,
							  thePeriod.localStartTime(),
							  theVar,
							  windDataVector,
							  begLowerLimit,
							  begUpperLimit);
	  get_wind_speed_interval(theSources,
							  theArea,
							  thePeriod.localEndTime(),
							  theVar,
							  windDataVector,
							  endLowerLimit,
							  endUpperLimit);

	  changeRatePerHour = abs(endUpperLimit - begUpperLimit) / wind_forecast_period_length(thePeriod);
	}

	void get_plain_wind_speed_interval(const wind_data_item_vector& windDataVector,
									   const NFmiTime& pointOfTime,
									   const WeatherArea::Type& areaType,
									   int& lowerLimit,
									   int& upperLimit)
	{
	  bool found(false);
	  for(unsigned int i = 0; i < windDataVector.size(); i++)
		{
		  const WindDataItemUnit& windDataItem = (*windDataVector[i])(areaType);
			
		  if(windDataItem.thePeriod.localStartTime() == pointOfTime)
			{
			  lowerLimit = static_cast<int>(round(windDataItem.theEqualizedMedianWindSpeed.value()));
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

	void get_plain_wind_speed_interval(const wind_data_item_vector& windDataVector,
									   const WeatherPeriod& period,
									   const WeatherArea::Type& areaType,
									   int& lowerLimit,
									   int& upperLimit)
	{
	  bool firstMatchProcessed(false);

	  float minValue(lowerLimit);
	  float maxValue(upperLimit);
	  bool found(false);
	  for(unsigned int i = 0; i < windDataVector.size(); i++)
		{
		  const WindDataItemUnit& windDataItem = (*windDataVector[i])(areaType);
			
		  if(is_inside(windDataItem.thePeriod,period))
			{
			  if(!firstMatchProcessed)
				{
				  minValue = windDataItem.theEqualizedMedianWindSpeed.value();
				  maxValue = windDataItem.theEqualizedMaximumWind.value();
				  firstMatchProcessed = true;
				  found = true;
				}
			  else
				{
				  if(windDataItem.theEqualizedMedianWindSpeed.value() < minValue)
					minValue = windDataItem.theEqualizedMedianWindSpeed.value();
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

	void windspeed_distribution_interval(const AnalysisSources& theSources,
										 const WeatherArea& theArea,
										 const WeatherPeriod& thePeriod,
										 const string& theVar,
										 int& lowerLimit,
										 int& upperLimit)
	{
	  if(lowerLimit == static_cast<int>(kFloatMissing) ||
		 upperLimit == static_cast<int>(kFloatMissing))
		return;
	
	  const int maxvalue = upperLimit;
	  const int minvalue = lowerLimit;

	  // subtract 1 m/s from the lowest limit
	  lowerLimit  = (maxvalue - (minvalue - 1) > 5 ? maxvalue - 5 : minvalue - 1);

	  // Make sure that 2/3 of values are inside the interval
	  vector <pair<float, WeatherResult> > windSpeedDistribution;
	  
	  populate_windspeed_distribution_time_series(theSources,
												  theArea,
												  thePeriod,
												  theVar,
												  windSpeedDistribution);
	  int lower_index = maxvalue - 1;
	  
	  float sumDistribution = windSpeedDistribution[maxvalue].second.value();
	  while(sumDistribution < 67.0 && lower_index > 0)
		{
		  lower_index--;
		  sumDistribution += windSpeedDistribution[lower_index].second.value();
		}

	  //lowerLimit = lower_index;
	  lowerLimit = lower_index + 1;
	}

	void get_wind_speed_interval(const AnalysisSources& theSources,
								 const WeatherArea& theArea,
								 const WeatherPeriod& thePeriod,
								 const string& theVar,
								 const wind_data_item_vector& windDataVector,
								 int& lowerLimit,
								 int& upperLimit)
	{
	  get_plain_wind_speed_interval(windDataVector,
									thePeriod,
									theArea.type(),
									lowerLimit,
									upperLimit);

	  windspeed_distribution_interval(theSources,
									  theArea,
									  thePeriod,
									  theVar,
									  lowerLimit,
									  upperLimit);
	}

	void get_wind_speed_interval(const AnalysisSources& theSources,
								 const WeatherArea& theArea,
								 const NFmiTime& pointOfTime,
								 const string& theVar,
								 const wind_data_item_vector& windDataVector,
								 int& lowerLimit,
								 int& upperLimit)
	{
	  get_plain_wind_speed_interval(windDataVector,
									pointOfTime,
									theArea.type(),
									lowerLimit,
									upperLimit);

	  WeatherPeriod period(pointOfTime, pointOfTime);

	  windspeed_distribution_interval(theSources,
									  theArea,
									  period,
									  theVar,
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

  } // namespace TextGen
