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

#define TUULI_WORD "tuuli"
#define VAHAN_WORD "vahan"
#define NOPEASTI_WORD "nopeasti"
#define VAHITELLEN_WORD "vahitellen"
#define KOVIMMILLAAN_PHRASE "kovimmillaan"
#define HEIKKENEVAA_PHRASE "heikkenevaa"
#define VOIMISTUVAA_PHRASE "voimistuvaa"
#define TUULTA_WORD "tuulta"
#define TUULI_TYYNTYY_PHRASE "tuuli tyyntyy"
#define VOIMISTUVAA_TUULTA_PHRASE "voimistuvaa tuulta"
#define HEIKKENEVAA_TUULTA_PHRASE "heikkenevaa tuulta"
#define POHJOISEEN_PHRASE "pohjoiseen"
#define ETELAAN_PHRASE "etelaan"
#define ITAAN_PHRASE "itaan"
#define LANTEEN_PHRASE "lanteen"
#define KOILLISEEN_PHRASE "koilliseen"
#define KAAKKOON_PHRASE "kaakkoon"
#define LOUNAASEEN_PHRASE "lounaaseen"
#define LUOTEESEEN_PHRASE "luoteeseen"
#define POHJOISEN_PUOLELLE_PHRASE "pohjoisen puolelle"
#define ETELAN_PUOLELLE_PHRASE "etelan puolelle"
#define IDAN_PUOLELLE_PHRASE "idan puolelle"
#define LANNEN_PUOLELLE_PHRASE "lannen puolelle"
#define KOILLISEN_PUOLELLE_PHRASE "koillisen puolelle"
#define KAAKON_PUOLELLE_PHRASE "kaakon puolelle"
#define LOUNAAN_PUOLELLE_PHRASE "lounaan puolelle"
#define LUOTEEN_PUOLELLE_PHRASE "luoteen puolelle"
#define POHJOISEN_JA_KOILLISEN_VALILLE_PHRASE "pohjoisen ja koillisen valille"
#define IDAN_JA_KOILLISEN_VALILLE_PHRASE "idan ja koillisen valille"
#define IDAN_JA_KAAKON_VALILLE_PHRASE "idan ja kaakon valille"
#define ETELAN_JA_KAAKON_VALILLE_PHRASE "etelan ja kaakon valille"
#define ETELAN_JA_LOUNAAN_VALILLE_PHRASE "etelan ja lounaan valille"
#define LANNEN_JA_LOUNAAN_VALILLE_PHRASE "lannen ja lounaan valille"
#define LANNEN_JA_LUOTEEN_VALILLE_PHRASE "lannen ja luoteen valille"
#define POHJOISEN_JA_LUOTEEN_VALILLE_PHRASE "pohjoisen ja luoteen valille"
#define TUULI_MUUTTUU_VAIHTELEVAKSI_PHRASE "tuuli muuttuu vaihtelevaksi"
#define TUULI_KAANTYY_PHRASE "tuuli kaantyy"

#define POHJOINEN_TUULI_G "1-tuulta"
#define POHJOINEN_TUULI "1-tuuli"
#define POHJOISEN_PUOLEINEN_TUULI_G "1-puoleista tuulta"
#define POHJOISEN_PUOLEINEN_TUULI "1-puoleinen tuuli"
#define POHJOINEN_KOILLINEN_TUULI_G "1- ja 2-valista tuulta"
#define POHJOINEN_KOILLINEN_TUULI "1- ja 2-valinen tuuli"
#define KOILLINEN_TUULI_G "2-tuulta"
#define KOILLINEN_TUULI "2-tuuli"
#define KOILLISEN_PUOLEINEN_TUULI_G "2-puoleista tuulta"
#define KOILLISEN_PUOLEINEN_TUULI "2-puoleinen tuuli"
#define ITA_KOILLINEN_TUULI_G "3- ja 2-valista tuulta"
#define ITA_KOILLINEN_TUULI "3- ja 2-valinen tuuli"
#define ITA_TUULI_G "3-tuulta"
#define ITA_TUULI "3-tuuli"
#define IDAN_PUOLEINEN_TUULI_G "3-puoleista tuulta"
#define IDAN_PUOLEINEN_TUULI "3-puoleinen tuuli"
#define ITA_KAAKKO_TUULI_G "3- ja 4-valista tuulta"
#define ITA_KAAKKO_TUULI "3- ja 4-valinen tuuli"
#define KAAKKO_TUULI_G "4-tuulta"
#define KAAKKO_TUULI "4-tuuli"
#define KAAKON_PUOLEINEN_TUULI_G "4-puoleista tuulta"
#define KAAKON_PUOLEINEN_TUULI "4-puoleinen tuuli"
#define ETELA_KAAKKO_TUULI_G "5- ja 4-valista tuulta"
#define ETELA_KAAKKO_TUULI "5- ja 4-valinen tuuli"
#define ETELA_TUULI_G "5-tuulta"
#define ETELA_TUULI "5-tuuli"
#define ETELAN_PUOLEINEN_TUULI_G "5-puoleista tuulta"
#define ETELAN_PUOLEINEN_TUULI "5-puoleinen tuuli"
#define ETELA_LOUNAS_TUULI_G "5- ja 6-valista tuulta"
#define ETELA_LOUNAS_TUULI "5- ja 6-valinen tuuli"
#define LOUNAS_TUULI_G "6-tuulta"
#define LOUNAS_TUULI "6-tuuli"
#define LOUNAAN_PUOLEINEN_TUULI_G "6-puoleista tuulta"
#define LOUNAAN_PUOLEINEN_TUULI "6-puoleinen tuuli"
#define LANSI_LOUNAS_TUULI_G "7- ja 6-valista tuulta"
#define LANSI_LOUNAS_TUULI "7- ja 6-valinen tuuli"
#define LANSI_TUULI_G "7-tuulta"
#define LANSI_TUULI "7-tuuli"
#define LANNEN_PUOLEINEN_TUULI_G "7-puoleista tuulta"
#define LANNEN_PUOLEINEN_TUULI "7-puoleinen tuuli"
#define LANSI_LUODE_TUULI_G "7- ja 8-valista tuulta"
#define LANSI_LUODE_TUULI "7- ja 8-valinen tuuli"
#define LUODE_TUULI_G "8-tuulta"
#define LUODE_TUULI "8-tuuli"
#define LUOTEEN_PUOLEINEN_TUULI_G "8-puoleista tuulta"
#define LUOTEEN_PUOLEINEN_TUULI "8-puoleinen tuuli"
#define POHJOINEN_LUODE_TUULI_G "1- ja 8-valista tuulta"
#define POHJOINEN_LUODE_TUULI "1- ja 8-valinen tuuli"
#define VAIHTELEVA_TUULI_G "suunnaltaan vaihtelevaa tuulta"
#define VAIHTELEVA_TUULI "suunnaltaan vaihteleva tuuli"



#define ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_JA_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE "[iltapaivalla] tuuli kaantyy [etelaan] ja voimistuu edelleen"
#define ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE "[iltapaivalla] tuuli kaantyy [etelaan] ja heikkenee edelleen"
#define ILTAPAIVALLA_TUULI_VOIMISTUU_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] tuuli voimistuu ja kaantyy [etelaan]"
#define ILTAPAIVALLA_POHJOISTUULI_VOIMISTUU_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] [pohjoistuuli] voimistuu ja kaantyy [etelaan]"
#define ILTAPAIVALLA_TUULI_HEIKKENEE_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] tuuli heikkenee ja kaantyy [etelaan]"
#define ILTAPAIVALLA_POHJOISTUULI_HEIKKENEE_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] [pohjoistuuli] heikkenee ja kaantyy [etelaan]"
#define ILTAPAIVALLA_TUULI_TYYNTYY_COMPOSITE_PHRASE "[iltapaivalla] tuuli tyyntyy"
  //#define ILTAPAIVALLA_TUULI_TYYNTYY_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] tuuli tyyntyy ja kaantyy [etelaan]"
#define ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_JA_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE "[iltapaivalla] tuuli muuttuu vaihtelevaksi ja voimistuu edelleen"
#define ILTAPAIVALLA_TUULI_VOIMISTUU_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli voimistuu ja muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_POHJOISTUULI_VOIMISTUU_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] [pohjoistuuli] voimistuu ja muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE "[iltapaivalla] tuuli muuttuu vaihtelevaksi ja heikkenee edelleen"
#define ILTAPAIVALLA_TUULI_HEIKKENEE_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli heikkenee ja muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_POHJOISTUULI_HEIKKENEE_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] [pohjoistuuli] heikkenee ja muuttuu vaihtelevaksi"
  //#define ILTAPAIVALLA_TUULI_TYYNTYY_JA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli tyyntyy ja muuttuu vaihtelevaksi"
#define POHJOISTUULTA_INTERVALLI_MS_JOKA_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[pohjoistuulta] [m...n] [metria sekunnissa], joka kaantyy [etelaan]"
#define ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] tuuli kaantyy [etelaan]"
#define ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] tuuli kaantyy [etelaan]"
#define POHJOISTUULTA_INTERVALLI_MS_JOKA_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[pohjoistuulta] [m...n] [metria sekunnissa], joka muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli muuttuu vaihtelevaksi"
#define ILTAPAIVALLA_ETELAAN_KAANTYVAA_TUULTA_COMPOSITE_PHRASE "[iltapaivalla] [etelaan] kaantyvaa tuulta"
#define ILTAPAIVALLA_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE "[iltapaivalla] [heikkenevaa] [etelatuulta]"
#define ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE "[iltapaivalla] [nopeasti] [heikkenevaa] [etelatuulta]"
#define ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE "[iltapaivalla] [etelatuulta]"
#define ILTAPAIVALLA_HEIKKENEVAA_TUULTA_COMPOSITE_PHRASE "[iltapaivalla] [heikkenevaa tuulta]"
#define ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_TUULTA_COMPOSITE_PHRASE "[iltapaivalla] [nopeasti] [heikkenevaa tuulta]"

#define ILTAPAIVALLA_TUULI_VOIMISTUU_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli voimistuu ja muuttuu tilapaisesti vaihtelevaksi"
#define ILTAPAIVALLA_POHJOISTUULI_VOIMISTUU_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] [pohjoistuuli] voimistuu ja muuttuu tilapaisesti vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_JA_VOIMISTUU_EDELLEEN_COMPOSITE_PHRASE "[iltapaivalla] tuuli muuttuu tilapaisesti vaihtelevaksi ja voimistuu edelleen"
#define ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_JA_HEIKKENEE_EDELLEEN_COMPOSITE_PHRASE "[iltapaivalla] tuuli muuttuu tilapaisesti vaihtelevaksi ja heikkenee edelleen"
#define ILTAPAIVALLA_POHJOISTUULI_HEIKKENEE_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] [pohjoistuuli] heikkenee ja muuttuu tilapaisesti vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_HEIKKENEE_JA_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli heikkenee ja muuttuu tilapaisesti vaihtelevaksi"
#define ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE "[iltapaivalla] tuuli muuttuu tilapaisesti vaihtelevaksi"

#define ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI "[iltapaivalla] [etelatuuli] alkaa heiketa [nopeasti]"
#define ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI "[iltapaivalla] [etelatuuli] alkaa voimistua [nopeasti]"
#define ILTAPAIVALLA_POHJOISTUULI_ALKAA_VOIMISTUA_JA_KAANTYA_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] [pohjoistuuli] alkaa voimistua ja kaantya [etelaan]"
#define ILTAPAIVALLA_POHJOISTUULI_ALKAA_HEIKETA_JA_KAANTYA_ETELAAN_COMPOSITE_PHRASE "[iltapaivalla] [pohjoistuuli] alkaa heiketa ja kaantya [etelaan]"

  void print_period_and_text(const string& label, const WeatherPeriod& period, const string& text)
  {
	cout << label << period.localStartTime() << "..." << period.localEndTime() << " " << text << endl;
  }


  WindForecast::WindForecast(wo_story_params& parameters):
	theParameters(parameters),
	thePreviousDayNumber(parameters.theForecastPeriod.localStartTime().GetWeekday())
  {
  }

  Paragraph WindForecast::getWindStory(const WeatherPeriod& thePeriod) const
  {
	Paragraph paragraph;

	wind_event_id eventIdPrevious(MISSING_WIND_EVENT);

	for(unsigned int i = 0; i < theParameters.theMergedWindEventPeriodVector.size(); i++)
	  {
		Sentence sentence;
		const WindEventPeriodDataItem& eventPeriodItem = *(theParameters.theMergedWindEventPeriodVector[i]);
		wind_event_id eventId = eventPeriodItem.theWindEvent;
		wind_event_id nextWindEventId(MISSING_WIND_EVENT);
		theParameters.theAlkaenPhraseUsed = false;

		const WeatherPeriod& eventPeriod = eventPeriodItem.thePeriod;

		WeatherPeriod eventStartPeriod(eventPeriod.localStartTime(),
									   eventPeriod.localStartTime());
		WeatherPeriod eventEndPeriod(eventPeriod.localEndTime(),
									 eventPeriod.localEndTime());
		
		wind_direction_id windDirectionIdBeg(MISSING_WIND_DIRECTION_ID);
		wind_direction_id windDirectionIdEnd(MISSING_WIND_DIRECTION_ID);

		getWindDirectionBegEnd(eventPeriod, windDirectionIdBeg, windDirectionIdEnd);

		wind_direction_id windDirectionIdPrevious(MISSING_WIND_DIRECTION_ID);
		
		part_of_the_day_id partOfTheDayBeg(get_part_of_the_day_id(eventPeriod.localStartTime()));
		part_of_the_day_id partOfTheDayEnd(get_part_of_the_day_id(eventPeriod.localEndTime()));
				  
		bool firstSentenceInTheStory = paragraph.empty();

		if(i > 0)
		  {
			WindEventPeriodDataItem* previousEventPeriodItem = theParameters.theMergedWindEventPeriodVector[i-1];
			windDirectionIdPrevious = get_wind_direction_id(previousEventPeriodItem->thePeriodEndDataItem.theWindDirection, theParameters.theVar);
		  }
		if(i < theParameters.theMergedWindEventPeriodVector.size() - 1)
		  {
			WindEventPeriodDataItem* nextEventPeriodItem = theParameters.theMergedWindEventPeriodVector[i+1];
			nextWindEventId = nextEventPeriodItem->theWindEvent;
		  }
		
		//cout << "eventId: " << get_wind_event_string(eventId) << endl;

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
						   << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_PHRASE : VOIMISTUVAA_PHRASE)
						   << windDirectionSentence(windDirectionIdBeg);
				}
			  else
				{
				  useAlkaaHeiketaVoimistuaPhrase = (!smallChange &&
													get_period_length(eventPeriod) > 6 && 
													partOfTheDayBeg != partOfTheDayEnd);
				  if(useAlkaaHeiketaVoimistuaPhrase)
					{
					  sentence << (eventId == TUULI_HEIKKENEE ? ILTAPAIVALLA_ETELATUULI_ALKAA_HEIKETA_NOPEASTI :
								   ILTAPAIVALLA_ETELATUULI_ALKAA_VOIMISTUA_NOPEASTI)
							   << getTimePhrase(eventPeriod, DONT_USE_ALKAEN_PHRASE)
							   << TUULI_WORD
							   << changeAttributeStr;
					}
				  else
					{
					  sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_TUULTA_COMPOSITE_PHRASE
							   << getTimePhrase(eventPeriod, USE_ALKAEN_PHRASE)
							   << changeAttributeStr
							   << (eventId == TUULI_HEIKKENEE ? HEIKKENEVAA_TUULTA_PHRASE : VOIMISTUVAA_TUULTA_PHRASE);
					}
				}

			  // if weakening/strengthening continues on the next period don't report speed yet
			  if(nextWindEventId == MISSING_WIND_EVENT 
				 || nextWindEventId == MISSING_WIND_SPEED_EVENT 
				 || !(nextWindEventId & eventId) ||
				 firstSentenceInTheStory)
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
			  
			  sentence << windDirectionChangesSentence(theParameters,
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
				  if(eventPeriodItem.theTransientFlag)
					sentence << ILTAPAIVALLA_TUULI_MUUTTUU_TILAPAISESTI_VAIHTELEVAKSI_COMPOSITE_PHRASE
							 << getTimePhrase(eventStartPeriod, !firstSentenceInTheStory);
				  else
					sentence << ILTAPAIVALLA_TUULI_MUUTTUU_VAIHTELEVAKSI_COMPOSITE_PHRASE
							 << getTimePhrase(eventStartPeriod, !firstSentenceInTheStory);
				}
			}
			break;
		  case TUULI_KAANTYY_JA_VOIMISTUU:
		  case TUULI_KAANTYY_JA_HEIKKENEE:
			{
			  bool useAlkaaHeiketaVoimistuaPhrase(false);
			  bool appendDecreasingIncreasingInterval(true);

			  if(!wind_speed_differ_enough(eventPeriod,
										   theParameters.theWindDataVector,
										   theParameters.theAreaPart))
				{
				  sentence << windDirectionChangesSentence(theParameters,
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
					  if(windDirectionIdBeg == VAIHTELEVA ||
						 windDirectionIdBeg == MISSING_WIND_DIRECTION_ID)
						{
						  sentence << directedWindSentenceAfterVaryingWind(theParameters,
																		   eventPeriod,
																		   firstSentenceInTheStory);
						  appendDecreasingIncreasingInterval = false;
						}
					  else
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
				  if(eventPeriodItem.theTransientFlag)
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
					  if(eventPeriodItem.theTransientFlag)
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
					  if(eventPeriodItem.theTransientFlag)
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
				  if(eventPeriodItem.theTransientFlag)
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
					  if(eventPeriodItem.theTransientFlag)
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
					  if(eventPeriodItem.theTransientFlag)
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
				  sentence << windDirectionSentence(windDirectionIdBeg);
				  sentence << windSpeedIntervalSentence(eventPeriod, 
														USE_AT_ITS_STRONGEST_PHRASE);
				}
			}
			break;
		  default:
			break;
		  };

		if(!sentence.empty())
		  eventIdPrevious = eventId;

		paragraph << sentence;

	  } // for


	return paragraph;
  }

  Sentence WindForecast::windDirectionChangesSentence(const wo_story_params& theParameters,
													  const WeatherPeriod& eventPeriod,
													  const bool& firstSentenceInTheStory,
													  const wind_direction_id& windDirectionIdPrevious) const
  {
	Sentence sentence;

	wind_direction_id windDirectionIdBeg(MISSING_WIND_DIRECTION_ID);
	wind_direction_id windDirectionIdEnd(MISSING_WIND_DIRECTION_ID);
	
	getWindDirectionBegEnd(eventPeriod, windDirectionIdBeg, windDirectionIdEnd);

	if(firstSentenceInTheStory)
	  {
		if(windDirectionIdBeg == VAIHTELEVA ||
		   windDirectionIdBeg == MISSING_WIND_DIRECTION_ID)
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
		if(windDirectionIdPrevious == VAIHTELEVA)
		  {
			/*
			// check if the direction stays the same longer time, so that
			// we can perhaps use "alkaen"-phrase
			int windDirectionChangeIndex = i+1;
			for(unsigned int k = i+1; k < theParameters.theMergedWindEventPeriodVector.size(); k++)
			{
			windDirectionChangeIndex++;
			const WindEventPeriodDataItem* wItem = theParameters.theMergedWindEventPeriodVector[k];
			wind_event_id wEvent = wItem->theWindEvent;
			if(wEvent & TUULI_KAANTYY ||
			wEvent & TUULI_MUUTTUU_VAIHTELEVAKSI)
			{
			break;
			}
			}

			const WindEventPeriodDataItem* wItem = theParameters.theMergedWindEventPeriodVector[windDirectionChangeIndex-1];
			WeatherPeriod actualEventPeriod(eventPeriod.localStartTime(), 
			wItem->thePeriod.localStartTime());

			*/
			sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
					 << getTimePhrase(eventPeriod, !firstSentenceInTheStory)
					 << windDirectionSentence(windDirectionIdEnd);
		  }
		else
		  {
			sentence << ILTAPAIVALLA_TUULI_KAANTYY_ETELAAN_COMPOSITE_PHRASE
					 << getTimePhrase(eventPeriod, USE_ALKAEN_PHRASE)
					 << getWindDirectionTurntoString(windDirectionIdEnd);
		  }
	  }

	return sentence;
  }

  Sentence WindForecast::windDirectionAndSpeedChangesSentence(const wo_story_params& theParameters,
															  const WeatherPeriod& eventPeriod,
															  const bool& firstSentenceInTheStory,
															  const wind_event_id& previousWindEventId,
															  const wind_event_id& currentWindEventId,
															  const wind_direction_id& previousWindDirectionId,
															  bool& useAlkaaHeiketaVoimistuaPhrase) const
  {
	Sentence sentence;
	
	wind_direction_id windDirectionIdBeg(MISSING_WIND_DIRECTION_ID);
	wind_direction_id windDirectionIdEnd(MISSING_WIND_DIRECTION_ID);

	getWindDirectionBegEnd(eventPeriod, windDirectionIdBeg, windDirectionIdEnd);

	bool smallChange(false);
	bool gradualChange(false);
	bool fastChange(false);
	std::string changeAttributeStr(EMPTY_STRING);
	bool windSpeedChangeAttribute(getWindSpeedChangeAttribute(eventPeriod, changeAttributeStr,
															  smallChange, gradualChange, fastChange));
	bool currentWindIsIncreasing =  (currentWindEventId & TUULI_VOIMISTUU);

	if(previousWindEventId & TUULI_VOIMISTUU || previousWindEventId & TUULI_HEIKKENEE)
	  {
		if(previousWindDirectionId == VAIHTELEVA)
		  {
			if(!windSpeedChangeAttribute)
			  {
				return sentence;
			  }
			
			sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
					 << getTimePhrase(eventPeriod, USE_ALKAEN_PHRASE)
					 << changeAttributeStr
					 << (currentWindIsIncreasing ? VOIMISTUVAA_PHRASE : HEIKKENEVAA_PHRASE)
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

			sentence << getTimePhrase(eventPeriod, USE_ALKAEN_PHRASE)
					 << getWindDirectionTurntoString(windDirectionIdEnd);
		  }
	  }
	else
	  {
		if(previousWindDirectionId == VAIHTELEVA)
		  {
			if(!windSpeedChangeAttribute)
			  {
				return sentence;
			  }

			Sentence timePhrase;
			if(firstSentenceInTheStory)
			  timePhrase << EMPTY_STRING;
			else
			  timePhrase << getTimePhrase(eventPeriod, USE_ALKAEN_PHRASE);

			sentence << ILTAPAIVALLA_NOPEASTI_HEIKKENEVAA_ETELATUULTA_COMPOSITE_PHRASE
					 << timePhrase
					 << changeAttributeStr
					 << (currentWindIsIncreasing ? VOIMISTUVAA_PHRASE : HEIKKENEVAA_PHRASE)
					 << windDirectionSentence(windDirectionIdEnd);
		  }
		else
		  {
			if(firstSentenceInTheStory)
			  {
				if(currentWindEventId & TUULI_VOIMISTUU)
				  sentence << ILTAPAIVALLA_POHJOISTUULI_VOIMISTUU_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE;
				else
				  sentence << ILTAPAIVALLA_POHJOISTUULI_HEIKKENEE_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE;

				sentence << EMPTY_STRING
						 << windDirectionSentence(windDirectionIdBeg, USE_AT_ITS_STRONGEST_PHRASE)
						 << getWindDirectionTurntoString(windDirectionIdEnd);
			  }
			else
			  {
				if(!windSpeedChangeAttribute)
				  {
					return sentence;
				  }

				part_of_the_day_id partOfTheDayBeg(get_part_of_the_day_id(eventPeriod.localStartTime()));
				part_of_the_day_id partOfTheDayEnd(get_part_of_the_day_id(eventPeriod.localEndTime()));

				useAlkaaHeiketaVoimistuaPhrase = (!smallChange &&
												  get_period_length(eventPeriod) > 6 && 
												  partOfTheDayBeg != partOfTheDayEnd);
						  
				if(useAlkaaHeiketaVoimistuaPhrase)
				  {
					WeatherPeriod periodBeg(eventPeriod.localStartTime(),
											eventPeriod.localStartTime());

					if(currentWindEventId & TUULI_VOIMISTUU)
					  sentence << ILTAPAIVALLA_POHJOISTUULI_ALKAA_VOIMISTUA_JA_KAANTYA_ETELAAN_COMPOSITE_PHRASE;
					else
					  sentence << ILTAPAIVALLA_POHJOISTUULI_ALKAA_HEIKETA_JA_KAANTYA_ETELAAN_COMPOSITE_PHRASE;

					sentence << getTimePhrase(periodBeg, DONT_USE_ALKAEN_PHRASE)
							 << TUULI_WORD
							 << getWindDirectionTurntoString(windDirectionIdEnd);
				  }
				else
				  {
					if(currentWindEventId & TUULI_VOIMISTUU)
					  sentence << ILTAPAIVALLA_TUULI_VOIMISTUU_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE;
					else
					  sentence << ILTAPAIVALLA_TUULI_HEIKKENEE_JA_KAANTYY_ETELAAN_COMPOSITE_PHRASE;

					sentence << getTimePhrase(eventPeriod, USE_ALKAEN_PHRASE)
							 << getWindDirectionTurntoString(windDirectionIdEnd);
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

	NFmiTime windTurningTime(getWindDirectionTurningTime(eventPeriod));
	NFmiTime periodEndTime(eventPeriod.localEndTime());

	WeatherPeriod periodStart(eventPeriod.localStartTime(),
							  eventPeriod.localStartTime());
	WeatherPeriod periodEnd(eventPeriod.localEndTime(),
							eventPeriod.localEndTime());
	WeatherPeriod period(windTurningTime,
						 periodEndTime);

	wind_direction_id windDirectionId(get_wind_direction_at(theParameters, 
															period,
															theParameters.theVar));

	if(firstSentenceInTheStory)
	  {
		sentence << windDirectionSentence(VAIHTELEVA)
				 << windSpeedIntervalSentence(periodStart, 
											  USE_AT_ITS_STRONGEST_PHRASE)
				 << Delimiter(COMMA_PUNCTUATION_MARK)
				 << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
				 << getTimePhrase(period, USE_ALKAEN_PHRASE)
				 << windDirectionSentence(windDirectionId)
				 << windSpeedIntervalSentence(periodEnd,
											  USE_AT_ITS_STRONGEST_PHRASE);
	  }
	else
	  {
		sentence << ILTAPAIVALLA_ETELATUULTA_COMPOSITE_PHRASE
				 << getTimePhrase(period, !firstSentenceInTheStory)
				 << windDirectionSentence(windDirectionId);
	  }

	return sentence;
  }

 Sentence WindForecast::decreasingIncreasingInterval(const WindEventPeriodDataItem& eventPeriodDataItem,
													 const bool& firstSentenceInTheStory,
													 const bool& startsToWeakenStrengthenPhraseUsed,
													 const wind_event_id& eventId) const
  {
	Sentence sentence;

	WeatherPeriod speedEventPeriod(eventPeriodDataItem.thePeriod);
	int periodLength(get_period_length(speedEventPeriod));
	WeatherPeriod periodBeg(speedEventPeriod.localStartTime(), 
							speedEventPeriod.localStartTime());
	WeatherPeriod periodEnd(speedEventPeriod.localEndTime(), 
							speedEventPeriod.localEndTime());
	
	bool useTimePhrase = (theParameters.theAlkaenPhraseUsed 
						  || startsToWeakenStrengthenPhraseUsed 
						  || firstSentenceInTheStory);
	bool windIsDecreasing =  (eventId & TUULI_HEIKKENEE);

	if(!wind_speed_differ_enough(speedEventPeriod,
								 theParameters.theWindDataVector,
								 theParameters.theAreaPart))
	  {
		if(useTimePhrase)
		  {
			sentence << Delimiter(COMMA_PUNCTUATION_MARK)
					 << getTimePhrase(periodEnd, DONT_USE_ALKAEN_PHRASE)
					 << windSpeedIntervalSentence(periodEnd,
												  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
		  }
		else
		  {
			sentence << windSpeedIntervalSentence(speedEventPeriod,
												  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
		  }

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
												   DONT_USE_AT_ITS_STRONGEST_PHRASE)
					  << Delimiter(COMMA_PUNCTUATION_MARK)
					  << getTimePhrase(periodEnd, DONT_USE_ALKAEN_PHRASE)
					  << windSpeedIntervalSentence(periodEnd, 
												   USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
		  }
		else
		  {
			if(periodLength <= 8 &&
			   get_part_of_the_day_id(speedEventPeriod.localStartTime()) ==
			   get_part_of_the_day_id(speedEventPeriod.localEndTime()))
			  {
				sentence << windSpeedIntervalSentence(periodEnd, 
													  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
			  }
			else
			  {
				if(get_period_length(speedEventPeriod) > 12)
				  {
					sentence << reportIntermediateSpeed(speedEventPeriod);
				  }
				/*
				  cout << "sentence.size() == 0: " << static_cast<bool>((sentence.size() == 0)) << endl;
				  cout << "get_part_of_the_day_id(speedEventPeriod.localStartTime()): " 
				  << static_cast<int>(get_part_of_the_day_id(speedEventPeriod.localStartTime())) << endl;
				  cout << "get_part_of_the_day_id(speedEventPeriod.localEndTime()): " 
				  << static_cast<int>(get_part_of_the_day_id(speedEventPeriod.localEndTime())) << endl;
				  cout << "speedEventPeriod: " << speedEventPeriod.localStartTime() 
				  << "..." << speedEventPeriod.localEndTime() << endl;
				  cout << "get_part_of_the_day_id(const WeatherPeriod& thePeriod): "
				  << static_cast<int>(get_part_of_the_day_id(speedEventPeriod, true)) << endl;
				*/
				if(useTimePhrase)
				  {
					sentence << Delimiter(COMMA_PUNCTUATION_MARK)
							 << getTimePhrase(periodEnd, DONT_USE_ALKAEN_PHRASE)
							 << windSpeedIntervalSentence(periodEnd,
														  USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
				  }
				else
				  {
					sentence << windSpeedIntervalSentence(periodEnd,
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
					  << windSpeedIntervalSentence(speedChangePeriods[0], 
												   USE_AT_ITS_STRONGEST_PHRASE && !windIsDecreasing);
		  }
		
		for(unsigned int i = 1; i < speedChangePeriods.size(); i++)
		  {
			WeatherPeriod currentPeriodEnd(speedChangePeriods[i].localEndTime(), 
										   speedChangePeriods[i].localEndTime());
			unsigned int lastItemIndex = (speedChangePeriods.size() - 1);
			part_of_the_day_id currentDayPart(get_part_of_the_day_id(currentPeriodEnd, true));
			
			if(i < lastItemIndex)
			  {
				WeatherPeriod nextPeriodEnd(speedChangePeriods[i+1].localEndTime(), 
											speedChangePeriods[i+1].localEndTime());

				part_of_the_day_id nextDayPart(get_part_of_the_day_id(nextPeriodEnd, true));

				/*
				cout << "currentDayPart: " 
					 << speedChangePeriods[i].localStartTime()
					 << "..."
					 << speedChangePeriods[i].localEndTime()
					 << " : "
					 << part_of_the_day_string(currentDayPart) << endl;
				cout << "nextDayPart: " 
					 << speedChangePeriods[i+1].localStartTime()
					 << "..."
					 << speedChangePeriods[i+1].localEndTime()
					 << " : "
					 << part_of_the_day_string(nextDayPart) << endl;
				*/

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
				//				cout << "interval1: " << lowerLimit1Int << "..." << upperLimit1Int << endl;
				//cout << "interval2: " << lowerLimit2Int << "..." << upperLimit2Int << endl;
 				
				// if the interval is almost the same on the next period, skip this period
				if(abs(lowerLimit2Int-lowerLimit1Int) <= 1 && 
				   abs(upperLimit2Int-upperLimit1Int) <= 1)
				  continue;
			  }
			
			Sentence intervalSentence;
			intervalSentence << windSpeedIntervalSentence(currentPeriodEnd,
														  (i == lastItemIndex));
				
			if(!intervalSentence.empty())
			  {
				

				if(useTimePhrase)
				  {
					sentence << Delimiter(COMMA_PUNCTUATION_MARK);
					sentence << getTimePhrase(speedChangePeriods[i], DONT_USE_ALKAEN_PHRASE);
				  }
				else
				  {
					/*
					cout << "useTimePhrase: " << useTimePhrase << endl;
					cout << "period: " 
						 << speedChangePeriods[i].localStartTime()
						 << "..."
						 << speedChangePeriods[i].localEndTime()
						 << endl;
					*/
				  }
				sentence << intervalSentence;
			  }
		  } // for(unsigned int i = 1; i < speedChangePeriods.size(); i++)
	  }

	return sentence;
  }

  int WindForecast::getLastSentenceIndex() const
  {
	int retval = -1;
	for(unsigned int i = 0; i < theParameters.theMergedWindEventPeriodVector.size(); i++)
	  {
		const WindEventPeriodDataItem& eventPeriodItem = *(theParameters.theMergedWindEventPeriodVector[i]);
		if(eventPeriodItem.theReportThisEventPeriodFlag)
		  retval = i;
	  }
	return retval;
  }

  Sentence WindForecast::reportIntermediateSpeed(const WeatherPeriod& speedEventPeriod) const
  {
	Sentence sentence;

	int periodLength(get_period_length(speedEventPeriod));

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
		if(wind_speed_differ_enough(period,
									theParameters.theWindDataVector,
									theParameters.theAreaPart))
		  {
			sentence << Delimiter(COMMA_PUNCTUATION_MARK)
					 << getTimePhrase(intermediatePeriod, DONT_USE_ALKAEN_PHRASE)
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

  // tries to find different part of the day than what is at period end in order to prevent tautology
  // in time expressions
  /*
  Sentence WindForecast::windSpeedIntervalSentence2(const WeatherPeriod& speedEventPeriod,
													bool theUseAtItsStrongestPhrase,
													bool theFirstSentenceInTheStory) const
  {
	Sentence sentence;

	// check if the whole period is located on the same part of the day
	if(theFirstSentenceInTheStory || samePartOfTheDay(speedEventPeriod.localStartTime(), speedEventPeriod.localEndTime()))
	  {
		sentence << windSpeedIntervalSentence(speedEventPeriod, theUseAtItsStrongestPhrase);
		
		return sentence;
	  }

	NFmiTime intermediateTime(speedEventPeriod.localEndTime());

	while((get_part_of_the_day_id(speedEventPeriod.localEndTime()) ==
		   get_part_of_the_day_id(intermediateTime)) &&
		  intermediateTime > speedEventPeriod.localStartTime())
	  {
		intermediateTime.ChangeByHours(-1);
	  }


	WeatherPeriod reportingPeriod(speedEventPeriod.localStartTime(), intermediateTime);
	
	sentence << windSpeedIntervalSentence(reportingPeriod, theUseAtItsStrongestPhrase);

	return sentence;
  }
  */

  wind_direction_id WindForecast::getWindDirectionId(const WeatherPeriod& thePeriod,
													 const CompassType& theComapssType) const
  {
	for(unsigned int i = 0; i < theParameters.theWindDataVector.size() ; i++)
	  {
		WindDataItemUnit& item = theParameters.theWindDataVector[i]->getDataItem(theParameters.theAreaPart);
		if(!is_inside(item.thePeriod.localStartTime(), thePeriod))
		  continue;
		
		WindDirectionAccuracy accuracy(direction_accuracy(item.theWindDirection.error(), theParameters.theVar));

		int direction(eight_directions ? 
					  direction8th(item.theEqualizedWindDirection.value()) :
					  direction16th(item.theEqualizedWindDirection.value()));

		
		if(accuracy == bad_accuracy)
		  return VAIHTELEVA;
		else
		  return static_cast<wind_direction_id>(direction);
		
	  }

	return VAIHTELEVA;
  }

  // returns the timestamp when the wind turns to the other direction
  // and stays away from the original direction till the end of period
  NFmiTime WindForecast::getWindDirectionTurningTime(const WeatherPeriod& period) const
  {
	NFmiTime retTime(period.localStartTime());
	unsigned int begIndex = UINT_MAX;
	unsigned int endIndex = theParameters.theWindDataVector.size();

	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		const WindDataItemUnit& windDataItem = (*theParameters.theWindDataVector[i])();
		
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

	if(begIndex != UINT_MAX)
	  {
		const WindDataItemUnit& windDataItemBeg = (*theParameters.theWindDataVector[begIndex])();
		wind_direction_id windDirectionBeg = get_wind_direction_id(windDataItemBeg.theWindDirection, theParameters.theVar);

		for(unsigned int i = begIndex+1; i < endIndex; i++)
		  {
			const WindDataItemUnit& windDataItemCurrent = (*theParameters.theWindDataVector[i])();
			wind_direction_id windDirectionCurrent = get_wind_direction_id(windDataItemCurrent.theWindDirection, theParameters.theVar);

			if(windDirectionCurrent != windDirectionBeg)
			  {
				retTime = windDataItemCurrent.thePeriod.localStartTime();
				break;
				/*
				bool permanentChange = true;
				for(unsigned int k = i; k < endIndex; k++)
				  {
					const WindDataItemUnit& windDataItemIter = (*theParameters.theWindDataVector[k])();
					wind_direction_id windDirectionIter = get_wind_direction_id(windDataItemIter.theWindDirection, theParameters.theVar);
					if(windDirectionIter == windDirectionBeg)
					  {
						permanentChange = false;
						break;
					  }
				  }
				if(permanentChange)
				  {
					retTime = windDataItemCurrent.thePeriod.localStartTime();
					break;
				  }
				*/
			  }
		  }
	  }
	return retTime;
  }

  Sentence WindForecast::getTimePhrase(const WeatherPeriod thePeriod,
									   const bool& useAlkaenPhrase) const
  {
	Sentence sentence;

	int forecastPeriodLength = get_period_length(theParameters.theForecastPeriod);
			
	bool specifyDay = (thePreviousDayNumber != thePeriod.localStartTime().GetWeekday() &&
					   (forecastPeriodLength > 12 &&
						abs(theParameters.theForecastTime.DifferenceInHours(thePeriod.localStartTime())) > 6 &&
						(theParameters.theForecastTime.GetDay() < thePeriod.localStartTime().GetDay() ||
						 theParameters.theForecastTime.GetYear() < thePeriod.localStartTime().GetYear())));


	std::string dayPhasePhrase;

	theParameters.theAlkaenPhraseUsed = ((get_period_length(thePeriod) > 5 && useAlkaenPhrase) 
										  && !fit_into_short_day_part(thePeriod));

	/*
	if(thePeriod.localEndTime() == theParameters.theForecastPeriod.localEndTime()
	   && get_period_length(thePeriod) > 5)
	  {
		WeatherPeriod begPeriod(thePeriod.localStartTime(), thePeriod.localStartTime());

		//	theParameters.theAlkaenPhraseUsed = (useAlkaenPhrase && !fit_into_short_day_part(thePeriod));

		// make sure that "alkaen"-phrase is used when wanted
		sentence << get_time_phrase_large(begPeriod,
										  specifyDay,
										  theParameters.theVar, 
										  dayPhasePhrase,
										  theParameters.theAlkaenPhraseUsed);
	  }
	else
	  {
		sentence << get_time_phrase_large(thePeriod,
										  specifyDay,
										  theParameters.theVar, 
										  dayPhasePhrase,
										  get_period_length(thePeriod) > 5 && useAlkaenPhrase);
	  }
	*/

	sentence << get_time_phrase_large(thePeriod,
									  specifyDay,
									  theParameters.theVar, 
									  dayPhasePhrase,
									  theParameters.theAlkaenPhraseUsed);

	thePreviousDayNumber = thePeriod.localStartTime().GetWeekday();

	return sentence;
  }

  wind_direction_id WindForecast::getWindDirectionId(const WeatherPeriod& thePeriod) const
  {
	wind_direction_id retval(VAIHTELEVA);

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
																  const wind_event_id& eventId) const
  {
	vector<WeatherPeriod> retVector;

	WeatherPeriod speedEventPeriod(eventPeriodDataItem.thePeriod);
	// find the point of time when max wind has changed theParameters.theWindSpeedThreshold from the previous
	// take the period from there to the end of the speedEventPeriod
	unsigned int begIndex = 0;
	unsigned int endIndex = 0;

	do
	  {
		const WindDataItemUnit& windDataItem = (*theParameters.theWindDataVector[begIndex])();
		if(is_inside(windDataItem.thePeriod.localStartTime(), speedEventPeriod) ||
		   begIndex == theParameters.theWindDataVector.size() - 1)
		  break;
		  begIndex++;
	  }
	while(true);

	float reportingPointThreshold(5.0);

	if(begIndex < theParameters.theWindDataVector.size())
	  {
		vector<unsigned int> reportingIndexes;
		float previousThresholdMaxWind(kFloatMissing);
		for(unsigned int i = begIndex; i < theParameters.theWindDataVector.size(); i++)
		  {
			const WindDataItemUnit& windDataItem = (*theParameters.theWindDataVector[i])();
			if(!is_inside(windDataItem.thePeriod.localStartTime(), speedEventPeriod))
			  break;
			
			if(i == begIndex)
			  {
				// speed at the beginning of the period is reported
				reportingIndexes.push_back(i);
				previousThresholdMaxWind = windDataItem.theEqualizedMaximumWind ;
			  }
			else if(abs(windDataItem.theEqualizedMaximumWind - previousThresholdMaxWind) > reportingPointThreshold)
			  {
				// speed is reported when it has changed reportingPointThreshold from the previous raporting point
				reportingIndexes.push_back(i);
				previousThresholdMaxWind = windDataItem.theEqualizedMaximumWind ;
			  }
			endIndex = i;
		  }

		if(reportingIndexes.size() > 1)
		  {
			for(unsigned int i = 0; i < reportingIndexes.size(); i++)
			  {
				unsigned int index = reportingIndexes[i];
				const WindDataItemUnit& windDataItem = (*theParameters.theWindDataVector[index])();
				if(i < reportingIndexes.size() - 1)
				  {
					retVector.push_back(windDataItem.thePeriod);
				  }
				else
				  {
					const WindDataItemUnit& windDataItemLast = (*theParameters.theWindDataVector[endIndex])();
					retVector.push_back(WeatherPeriod(windDataItem.thePeriod.localStartTime(), 
													  windDataItemLast.thePeriod.localEndTime()));
				  }
			  }
		  }
	  }

	if(retVector.size() == 0)
	  {
		retVector.push_back(speedEventPeriod);
	  }
	/*
	cout << "whole period: " << speedEventPeriod.localStartTime() << "..." << speedEventPeriod.localEndTime() << endl;

	for(unsigned int i = 0; i < retVector.size(); i++)
	cout << "r-period: " << retVector[i].localStartTime() << "..." << retVector[i].localEndTime() << endl;
	*/

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
			//periodEndTime.ChangeByHours(-1);
			break;
		  }
		periodEndTime = eventPeriodItem->thePeriod.localEndTime();
	  }

	return WeatherPeriod(periodStartTime, periodEndTime);
	  /*
	  TUULI_HEIKKENEE = 0x1,
	  TUULI_VOIMISTUU = 0x2,
	  TUULI_TYYNTYY = 0x4,
	  TUULI_KAANTYY = 0x8,
	  TUULI_MUUTTUU_VAIHTELEVAKSI = 0x10,
	  */
	
  }

  bool WindForecast::getWindSpeedChangeAttribute(const WeatherPeriod& changePeriod,
												 std::string& phraseStr,
												 bool& smallChange,
												 bool& gradualChange,
												 bool& fastChange) const
  {
	float begLowerLimit(0.0), 
	  begUpperLimit(0.0), 
	  endLowerLimit(0.0), 
	  endUpperLimit(0.0), 
	  changeRatePerHour(0.0);

	get_wind_speed_interval_parameters(changePeriod,
									   theParameters.theWindDataVector,
									   theParameters.theAreaPart,
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
	int periodLength(get_period_length(changePeriod));

	phraseStr = EMPTY_STRING;
	//	bool summerHalf(SeasonTools::isSummerHalf(changePeriod.localStartTime(), theParameters.theVar));
	/*
	cout << "periodLength: " << periodLength << endl;
	cout << "begLowerLimit: " << begLowerLimit << endl;
	cout << "begUpperLimit: " << begUpperLimit << endl;
	cout << "endLowerLimit: " << endLowerLimit << endl;
	cout << "endUpperLimit: " << endUpperLimit << endl;
	cout << "changeThreshold: " << changeThreshold << endl;
	cout << "changeRatePerHour: " << changeRatePerHour << endl;
	cout << "requiredChangeRatePerHour: " << requiredChangeRatePerHour << endl;
	*/
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

  const Sentence WindForecast::speedRangeSentence(const WeatherPeriod& thePeriod,
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
		// subtract 1 m/s from the lowest limit
		int intervalMin  = (maxvalue - (medianvalue - 1) > 5 ? maxvalue - 5 : medianvalue - 1);
		int intervalMax = maxvalue;

		if(abs(maxvalue - medianvalue) <= theParameters.theMinIntervalSize)
		  {
			sentence << medianvalue
					 << *UnitFactory::create(MetersPerSecond);				
			thePreviousRangeBeg = medianvalue;
		  }
		else
		  {

			// Make sure that 2/3 of values are inside the interval
			vector <pair<float, WeatherResult> > windSpeedDistribution;
				
			populate_windspeed_distribution_time_series(theParameters.theSources,
														theParameters.theArea,
														thePeriod,
														theParameters.theVar,
														windSpeedDistribution);

			std::ostringstream oss;

			int lower_index = maxvalue - 1;

			float sumDistribution = windSpeedDistribution[maxvalue].second.value();
			oss << lower_index+1 << ":"  << sumDistribution;
			while(sumDistribution < 67.0 && lower_index > 0)
			  {
				lower_index--;

				oss << "," << lower_index+1 << ":"  << windSpeedDistribution[lower_index].second.value();
				  
				sumDistribution += windSpeedDistribution[lower_index].second.value();
			  }
			//cout << oss.str() << endl;

			intervalMin = lower_index+1;
			
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
		  }

		if(!sentence.empty() && peakWind > intervalMax)
		  {
			//	sentence << *UnitFactory::create(MetersPerSecond);
			//if(peakWind != -1 && theLastSentenceFlag)
			if(theUseAtItsStrongestPhrase)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK) 
						 << KOVIMMILLAAN_PHRASE
						 << peakWind
						 << *UnitFactory::create(MetersPerSecond);
			  }
		  }
	  }
	  
	return sentence;
	  
  }

  // return upper and lower limit for the interval
  const bool WindForecast::getSpeedIntervalLimits(const WeatherPeriod& thePeriod, 
												  WeatherResult& lowerLimit,
												  WeatherResult& upperLimit) const
  {
 	double medianValueSum = 0.0;
	double medianErrorSum = 0.0;
	unsigned int counter = 0;

	WindDataItemUnit dataItem = (*theParameters.theWindDataVector[0])();
	WindDataItemUnit dataItemMinMax = (*theParameters.theWindDataVector[0])();

	//	cout << "period: " << thePeriod.localStartTime() << "..." << thePeriod.localEndTime() << endl;
	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		dataItem = (*theParameters.theWindDataVector[i])();
		
		/*
		if(dataItem.theWindMaximum.value() == kFloatMissing ||
		   dataItemMinMax.theWindMaximum.value() == kFloatMissing )
		  {
			return false;
		  }
		*/
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

	/*
	lowerLimit = static_cast<int>(medianValueSum / counter);
	upperLimit = static_cast<int>(dataItemMinMax.theWindMaximum.value());
	*/

	return true;
 }

  // theLastSentenceFlag tells wheather this is the last wind speed sentence on this event period
  // so that we can use "at its strongest"-expression if needed
  const Sentence WindForecast::windSpeedIntervalSentence(const WeatherPeriod& thePeriod, 
														 bool theUseAtItsStrongestPhrase /* = true*/) const
  {
	Sentence sentence;

	WeatherResult lowerLimit(kFloatMissing, 0.0);
	WeatherResult upperLimit(kFloatMissing, 0.0);

	//Sentence WindForecast::findWindSpeedReportingTime(const WeatherPeriod& speedEventPeriod) const

	if(getSpeedIntervalLimits(thePeriod, lowerLimit, upperLimit))
	  sentence << speedRangeSentence(thePeriod,
									 upperLimit,
									 lowerLimit,
									 theParameters.theVar,
									 theUseAtItsStrongestPhrase);
	return sentence;
  }

  const Sentence WindForecast::windDirectionSentence(const wind_direction_id& theWindDirectionId,
													 const bool& theBasicForm /*=  false */) const
  {
	Sentence sentence;

	switch(theWindDirectionId)
	  {
	  case POHJOINEN:
		sentence << (theBasicForm ? POHJOINEN_TUULI : POHJOINEN_TUULI_G);
		break;
	  case POHJOISEN_PUOLEINEN:
		sentence << (theBasicForm ? POHJOISEN_PUOLEINEN_TUULI : POHJOISEN_PUOLEINEN_TUULI_G);
		break;
	  case POHJOINEN_KOILLINEN:
		sentence << (theBasicForm ? POHJOINEN_KOILLINEN_TUULI : POHJOINEN_KOILLINEN_TUULI_G);
		break;
	  case KOILLINEN:
		sentence << (theBasicForm ? KOILLINEN_TUULI : KOILLINEN_TUULI_G);
		break;
	  case KOILLISEN_PUOLEINEN:
		sentence << (theBasicForm ? KOILLISEN_PUOLEINEN_TUULI : KOILLISEN_PUOLEINEN_TUULI_G);
		break;
	  case ITA_KOILLINEN:
		sentence << (theBasicForm ? ITA_KOILLINEN_TUULI : ITA_KOILLINEN_TUULI_G);
		break;
	  case ITA:
		sentence << (theBasicForm ? ITA_TUULI : ITA_TUULI_G);
		break;
	  case IDAN_PUOLEINEN:
		sentence << (theBasicForm ? IDAN_PUOLEINEN_TUULI : IDAN_PUOLEINEN_TUULI_G);
		break;
	  case ITA_KAAKKO:
		sentence << (theBasicForm ? ITA_KAAKKO_TUULI : ITA_KAAKKO_TUULI_G);
		break;
	  case KAAKKO:
		sentence << (theBasicForm ? KAAKKO_TUULI : KAAKKO_TUULI_G);
		break;
	  case KAAKON_PUOLEINEN:
		sentence << (theBasicForm ? KAAKON_PUOLEINEN_TUULI : KAAKON_PUOLEINEN_TUULI_G);
		break;
	  case ETELA_KAAKKO:
		sentence << (theBasicForm ? ETELA_KAAKKO_TUULI : ETELA_KAAKKO_TUULI_G);
		break;
	  case ETELA:
		sentence << (theBasicForm ? ETELA_TUULI : ETELA_TUULI_G);
		break;
	  case ETELAN_PUOLEINEN:
		sentence << (theBasicForm ? ETELAN_PUOLEINEN_TUULI : ETELAN_PUOLEINEN_TUULI_G);
		break;
	  case ETELA_LOUNAS:
		sentence << (theBasicForm ? ETELA_LOUNAS_TUULI : ETELA_LOUNAS_TUULI_G);
		break;
	  case LOUNAS:
		sentence << (theBasicForm ? LOUNAS_TUULI : LOUNAS_TUULI_G);
		break;
	  case LOUNAAN_PUOLEINEN:
		sentence << (theBasicForm ? LOUNAAN_PUOLEINEN_TUULI : LOUNAAN_PUOLEINEN_TUULI_G);
		break;
	  case LANSI_LOUNAS:
		sentence << (theBasicForm ? LANSI_LOUNAS_TUULI : LANSI_LOUNAS_TUULI_G);
		break;
	  case LANSI:
		sentence << (theBasicForm ? LANSI_TUULI : LANSI_TUULI_G);
		break;
	  case LANNEN_PUOLEINEN:
		sentence << (theBasicForm ? LANNEN_PUOLEINEN_TUULI :LANNEN_PUOLEINEN_TUULI_G);
		break;
	  case LANSI_LUODE:
		sentence << (theBasicForm ? LANSI_LUODE_TUULI : LANSI_LUODE_TUULI_G);
		break;
	  case LUODE:
		sentence << (theBasicForm ? LUODE_TUULI : LUODE_TUULI_G);
		break;
	  case LUOTEEN_PUOLEINEN:
		sentence << (theBasicForm ? LUOTEEN_PUOLEINEN_TUULI : LUOTEEN_PUOLEINEN_TUULI_G);
		break;
	  case POHJOINEN_LUODE:
		sentence << (theBasicForm ? POHJOINEN_LUODE_TUULI :POHJOINEN_LUODE_TUULI_G);
		break;
	  case VAIHTELEVA:
	  case MISSING_WIND_DIRECTION_ID:
		sentence << (theBasicForm ? VAIHTELEVA_TUULI : VAIHTELEVA_TUULI_G);
		break;
	  }

	return sentence;
  }

  void WindForecast::getWindDirectionBegEnd(const WeatherPeriod& thePeriod,
											wind_direction_id& theWindDirectionIdBeg,
											wind_direction_id& theWindDirectionIdEnd) const
  {
	theWindDirectionIdBeg = get_wind_direction_at(theParameters,
												  thePeriod.localStartTime(),
												  theParameters.theVar);

	theWindDirectionIdEnd = get_wind_direction_at(theParameters,
												  thePeriod.localEndTime(),
												  theParameters.theVar);


	/*
	unsigned int periodLength(get_period_length(thePeriod));
	if(periodLength < 6)
	  {
		theWindDirectionIdEnd = get_wind_direction_at(theParameters,
													  thePeriod.localEndTime(),
													  theParameters.theVar);
	  }
	else
	  {
		NFmiTime endPeriodStartTime(thePeriod.localEndTime());

		if(periodLength < 15)
		  endPeriodStartTime.ChangeByHours(-2);
		else
		  endPeriodStartTime.ChangeByHours(-3);
		WeatherPeriod endPeriod(endPeriodStartTime, thePeriod.localEndTime());

		cout << "endPeriod: " << endPeriod.localStartTime() << "..." << endPeriod.localEndTime() << endl;


		theWindDirectionIdEnd = get_wind_direction_at(theParameters,
													  endPeriod,
													  theParameters.theVar);
	  }


	cout << "thePeriod: " << thePeriod.localStartTime() << "..." << thePeriod.localEndTime() << endl;

	cout << "windDirectionIdBeg: " << get_wind_direction_string(theWindDirectionIdBeg) << endl;
	cout << "windDirectionIdEnd: " << get_wind_direction_string(theWindDirectionIdEnd) << endl;
	*/

  }


  std::string WindForecast::getWindDirectionTurntoString(const wind_direction_id& theWindDirectionId) const
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


  wind_speed_id get_wind_speed_id(const float& windSpeed)
  {
	if(windSpeed < 0.5)
	  return TYYNI;
	else if(windSpeed >= 0.5 && windSpeed < 3.5)
	  return HEIKKO;
	else if(windSpeed >= 3.5 && windSpeed < 7.5)
	  return KOHTALAINEN;
	else if(windSpeed >= 7.5 && windSpeed < 13.5)
	  return NAVAKKA;
	else if(windSpeed >= 13.5 && windSpeed < 20.5)
	  return KOVA;
	else if(windSpeed >= 20.5 && windSpeed < 32.5)
	  return MYRSKY;
	else
	  return HIRMUMYRSKY;
  }


  wind_speed_id get_wind_speed_id(const WeatherResult& windSpeed)
  {
	return get_wind_speed_id(windSpeed.value());
  }

  wind_direction_id get_puoleinen_direction_id(const WeatherResult& windDirection, const string& var)
  {
	int windDirectionId = direction16th(windDirection.value());

	switch(windDirectionId)
	  {
	  case 1:
		{
		  windDirectionId = POHJOISEN_PUOLEINEN;
		}
		break;
	  case 2:
		{
		  if(windDirection.value() > 337.5 || windDirection.value() < 22.5)
			windDirectionId = POHJOISEN_PUOLEINEN;
		  else
			windDirectionId = KOILLISEN_PUOLEINEN;
		}
		break;
	  case 3:
		{
		  windDirectionId = KOILLISEN_PUOLEINEN;
		}
		break;
	  case 4:
		{
		  if(windDirection.value() < 67.5)
			windDirectionId = KOILLISEN_PUOLEINEN;
		  else
			windDirectionId = IDAN_PUOLEINEN;
		}
		break;
	  case 5:
		{
		  windDirectionId = IDAN_PUOLEINEN;
		}
		break;
	  case 6:
		{
		  if(windDirection.value() < 112.5)
			windDirectionId = IDAN_PUOLEINEN;
		  else
			windDirectionId = KAAKON_PUOLEINEN;
		}
		break;
	  case 7:
		{
		  windDirectionId = KAAKON_PUOLEINEN;
		}
		break;
	  case 8:
		{
		  if(windDirection.value() < 157.5)
			windDirectionId = KAAKON_PUOLEINEN;
		  else
			windDirectionId = ETELAN_PUOLEINEN;
		}
		break;
	  case 9:
		{
		  windDirectionId = ETELAN_PUOLEINEN;
		}
		break;
	  case 10:
		{
		  if(windDirection.value() < 202.5)
			windDirectionId = ETELAN_PUOLEINEN;
		  else
			windDirectionId = LOUNAAN_PUOLEINEN;
		}
		break;
	  case 11:
		{
		  windDirectionId = LOUNAAN_PUOLEINEN;
		}
		break;
	  case 12:
		{
		  if(windDirection.value() < 247.5)
			windDirectionId = LOUNAAN_PUOLEINEN;
		  else
			windDirectionId = LANNEN_PUOLEINEN;
		}
		break;
	  case 13:
		{
		  windDirectionId = LANNEN_PUOLEINEN;
		}
		break;
	  case 14:
		{
		  if(windDirection.value() < 292.5)
			windDirectionId = LANNEN_PUOLEINEN;
		  else
			windDirectionId = LUOTEEN_PUOLEINEN;
		}
		break;
	  case 15:
		{
		  windDirectionId = LUOTEEN_PUOLEINEN;
		}
		break;
	  case 16:
		{
		  if(windDirection.value() < 337.5)
			windDirectionId = LUOTEEN_PUOLEINEN;
		  else
			windDirectionId = POHJOISEN_PUOLEINEN;
		}
		break;
	  default:
		break;
	  }
	
	return static_cast<wind_direction_id>(windDirectionId);
  }

  wind_direction_id get_wind_direction_id(const WeatherResult& windDirection, const string& var)
  {
	WindDirectionAccuracy accuracy(direction_accuracy(windDirection.error(), var));

	int windDirectionId(MISSING_WIND_DIRECTION_ID);

	if(accuracy == bad_accuracy)
	  windDirectionId = VAIHTELEVA;
	else if(accuracy == moderate_accuracy)
	  windDirectionId = get_puoleinen_direction_id(windDirection, var);
	else
	  windDirectionId = direction16th(windDirection.value());	
	
	return static_cast<wind_direction_id>(windDirectionId);
  }
					  
  wind_direction_id get_wind_direction_at(const wo_story_params& theParameters,
										  const NFmiTime& pointOfTime,
										  const string& var)
  {
	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		WindDataItemUnit& item = theParameters.theWindDataVector[i]->getDataItem(theParameters.theAreaPart);
		if(item.thePeriod.localStartTime() == pointOfTime)
		  {
			return get_wind_direction_id(item.theWindDirection, var);
		  }
	  }
	return MISSING_WIND_DIRECTION_ID;
  }

  wind_direction_id get_wind_direction_at(const wo_story_params& theParameters,
										  const WeatherPeriod& period,
										  const string& var)
  {
	float directionSum(0.0);
	float stdDevSum(0.0);
	for(unsigned int i = 0; i < theParameters.theWindDataVector.size(); i++)
	  {
		WindDataItemUnit& item = theParameters.theWindDataVector[i]->getDataItem(theParameters.theAreaPart);
		if(is_inside(item.thePeriod.localStartTime(), period))
		  {
			directionSum += item.theWindDirection.value();
			stdDevSum += item.theWindDirection.error();
		  }
	  }

	if(directionSum == 0.0)
	  return MISSING_WIND_DIRECTION_ID;

	int n(get_period_length(period) + 1);
	WeatherResult resultDirection(directionSum / n, stdDevSum / n);

	return get_wind_direction_id(resultDirection, var);
  }

  std::string get_wind_speed_string(const wind_speed_id& theWindSpeedId)
  {
	std::string retval;

	switch(theWindSpeedId)
	  {
	  case TYYNI:
		retval = "tyyni";
		break;
	  case HEIKKO:
		retval = "heikko";
		break;
	  case KOHTALAINEN:
		retval = "kohtalainen";
		break;
	  case NAVAKKA:
		retval = "navakka";
		break;
	  case KOVA:
		retval = "kova";
		break;
	  case MYRSKY:
		retval = "myrsky";
		break;
	  case HIRMUMYRSKY:
		retval = "hirmumyrsky";
		break;
	  }

	return retval;
  }

  std::string get_wind_direction_string(const wind_direction_id& theWindDirectionId)
  {
	std::string retval;

	switch(theWindDirectionId)
	  {
	  case POHJOINEN:
		retval = "pohjoinen";
		break;
	  case POHJOISEN_PUOLEINEN:
		retval = "pohjoisen puoleinen";
		break;
	  case POHJOINEN_KOILLINEN:
		retval = "pohjoisen ja koillisen valinen";
		break;
	  case KOILLINEN:
		retval = "koillinen";
		break;
	  case KOILLISEN_PUOLEINEN:
		retval = "koillisen puoleinen";
		break;
	  case ITA_KOILLINEN:
		retval = "idan ja koillisen valinen";
		break;
	  case ITA:
		retval = "ita";
		break;
	  case IDAN_PUOLEINEN:
		retval = "idan puoleinen";
		break;
	  case ITA_KAAKKO:
		retval = "idan ja kaakon valinen";
		break;
	  case KAAKKO:
		retval = "kaakko";
		break;
	  case KAAKON_PUOLEINEN:
		retval = "kaakon puoleinen";
		break;
	  case ETELA_KAAKKO:
		retval = "etelan ja kaakon valinen";
		break;
	  case ETELA:
		retval = "etela";
		break;
	  case ETELAN_PUOLEINEN:
		retval = "etelan puoleinen";
		break;
	  case ETELA_LOUNAS:
		retval = "etelan ja lounaan valinen";
		break;
	  case LOUNAS:
		retval = "lounas";
		break;
	  case LOUNAAN_PUOLEINEN:
		retval = "lounaan puoleinen";
		break;
	  case LANSI_LOUNAS:
		retval = "lannen ja lounaan valinen";
		break;
	  case LANSI:
		retval = "lansi";
		break;
	  case LANNEN_PUOLEINEN:
		retval = "lannen puoleinen";
		break;
	  case LANSI_LUODE:
		retval = "lannen ja luoteen valinen";
		break;
	  case LUODE:
		retval = "luode";
		break;
	  case LUOTEEN_PUOLEINEN:
		retval = "luoteen puoleinen";
		break;
	  case POHJOINEN_LUODE:
		retval = "pohjoisen ja luoteen valinen";
		break;
	  case VAIHTELEVA:
	  case MISSING_WIND_DIRECTION_ID:
		retval = "vaihteleva";
		break;
	  }

	return retval;
  }


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

  wind_direction_id get_moderate_accuracy_direction(const wind_direction_id& directionId)
  {
	switch(directionId)
	  {
	  case POHJOINEN:
		return POHJOISEN_PUOLEINEN;
		break;
	  case ETELA:
		return ETELAN_PUOLEINEN;
		break;
	  case ITA:
		return IDAN_PUOLEINEN;
		break;
	  case LANSI:
		return LANNEN_PUOLEINEN;
		break;
	  case KOILLINEN:
		return KOILLISEN_PUOLEINEN;
		break;
	  case KAAKKO:
		return KAAKON_PUOLEINEN;
		break;
	  case LOUNAS:
		return LOUNAAN_PUOLEINEN;
		break;
	  case LUODE:
		return LUOTEEN_PUOLEINEN;
		break;
	  default:
		return VAIHTELEVA;
		break;
	  }
  }

  bool is_principal_compass_point(const wind_direction_id& directionId)
  {
	return (directionId == POHJOINEN ||
			directionId == ETELA ||
			directionId == ITA ||
			directionId == LANSI ||
			directionId == KOILLINEN ||
			directionId == KAAKKO ||
			directionId == LOUNAS ||
			directionId == LUODE);
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
}
