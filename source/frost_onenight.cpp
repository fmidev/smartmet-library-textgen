// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::FrostStory::one night
 */
// ======================================================================

#include "FrostStory.h"
#include "FrostStoryTools.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "HourPeriodGenerator.h"
#include "Integer.h"
#include "MathTools.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "UnitFactory.h"
#include "WeatherResult.h"
#include "WeatherPeriodTools.h"
#include "PositiveValueAcceptor.h"
#include "ComparativeAcceptor.h"
#include "AreaTools.h"
#include "WeatherForecast.h"

#include <map>
#include <newbase/NFmiStringTools.h>

namespace TextGen
{
  namespace FrostOnenight
  {

	using namespace std;
	using namespace WeatherAnalysis;
	using namespace AreaTools;

#define HALLAN_VAARA_COMPOSITE_PHRASE "[sisämaassa] [alavilla mailla] hallan vaara"
#define MAHDOLLISESTI_HALLAA_COMPOSITE_PHRASE  "[rannikolla] mahdollisesti hallaa"
#define PAIKOIN_HALLAA_COMPOSITE_PHRASE "[rannikolla] [paikoin] hallaa"
#define PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE "[rannikolla] [paikoin] hallaa, joka voi olla ankar"

	enum frost_category{CAT_NA,
						CAT_0010, 
						CAT_1020,
						CAT_3040, 
						CAT_5060, 
						CAT_7080, 
						CAT_90100, 
						CAT_FROST};

	enum frost_phrase_id{EMPTY_STORY, 
						 ALAVILLA_MAILLA_HALLAN_VAARA, 
						 MAHDOLLISESTI_HALLAA, 
						 PAIKOIN_HALLAA, 
						 MONIN_PAIKOIN_HALLAA, 
						 HALLAA_YLEISESTI, 
						 YOPAKKASTA, 
						 RANNIKOLLA_HALLAN_VAARA, 
						 RANNIKOLLA_MAHDOLLISESTI_HALLAA, 
						 RANNIKOLLA_PAIKOIN_HALLAA, 
						 RANNIKOLLA_MONIN_PAIKOIN_HALLAA, 
						 RANNIKOLLA_HALLAA, 
						 SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA, 
						 SISAMAASSA_MAHDOLLISESTI_HALLAA, 
						 SISAMAASSA_PAIKOIN_HALLAA, 
						 SISAMAASSA_MONIN_PAIKOIN_HALLAA, 
						 SISAMAASSA_YLEISESTI_HALLAA};

	const frost_category get_frost_category(const double& frostProbability, 
											const unsigned short& forecast_areas, 
											const unsigned short& growing_season_started, 
											const unsigned short& night_frost, 
											const forecast_area_id& area_id)
	{
	  // area not included or growing season not yet started
	  if(!(forecast_areas & area_id) || !(growing_season_started & area_id))
		return CAT_NA;
	  else if(night_frost & area_id) // night frost
		return CAT_FROST;
	  
	  if(frostProbability >= 0.0 && frostProbability < 10.0)
		return CAT_0010;
	  else if(frostProbability >= 10.0 && frostProbability < 25.0)
		return CAT_1020;
	  else if(frostProbability >= 25.0 && frostProbability < 45.0)
		return CAT_3040;
	  else if(frostProbability >= 45.0 && frostProbability < 65.0)
		return CAT_5060;
	  else if(frostProbability >= 65.0 && frostProbability < 85.0)
		return CAT_7080;
	  else if(frostProbability >= 85.0 && frostProbability <= 100.0)
		return CAT_90100;
	  else
		return CAT_NA;
	}

	const int get_frost_onenight_phrase_id(const double& coastalFrostProbability, 
										   const double& inlandFrostProbability, 
										   const unsigned short& forecast_areas, 
										   const unsigned short& growing_season_started, 
										   const unsigned short& night_frost)
	{
	  frost_category categoryCoastal = get_frost_category(coastalFrostProbability, 
														  forecast_areas, 
														  growing_season_started, 
														  night_frost, 
														  COASTAL_AREA);
	  frost_category categoryInland = get_frost_category(inlandFrostProbability, 
														 forecast_areas, 
														 growing_season_started, 
														 night_frost, 
														 INLAND_AREA);
	  
	  // if coastal or inland area is missing, read story from story selection table diagonal
	  if(!(forecast_areas & COASTAL_AREA))
		categoryCoastal = categoryInland;
	  if(!(forecast_areas & INLAND_AREA))
		categoryInland = categoryCoastal;
	  
	  if(categoryCoastal == CAT_NA)
		{
		  if(categoryInland == CAT_NA || categoryInland == CAT_0010 || categoryInland == CAT_FROST)
			return EMPTY_STORY;
		  else if(categoryInland == CAT_1020)
			return ALAVILLA_MAILLA_HALLAN_VAARA;
		  else if(categoryInland == CAT_3040)
			return MAHDOLLISESTI_HALLAA;
		  else if(categoryInland == CAT_5060)
			return PAIKOIN_HALLAA;
		  else if(categoryInland == CAT_7080)
			return MONIN_PAIKOIN_HALLAA;
		  else if(categoryInland == CAT_90100)
			return HALLAA_YLEISESTI;
		}
	  if(categoryCoastal == CAT_0010)
		{
		  if(categoryInland == CAT_NA || categoryInland == CAT_0010 || categoryInland == CAT_FROST)
			return EMPTY_STORY;
		  else if(categoryInland == CAT_1020)
			return ALAVILLA_MAILLA_HALLAN_VAARA;
		  else if(categoryInland == CAT_3040)
			return MAHDOLLISESTI_HALLAA;
		  else if(categoryInland == CAT_5060)
			return SISAMAASSA_PAIKOIN_HALLAA;
		  else if(categoryInland == CAT_7080)
			return SISAMAASSA_MONIN_PAIKOIN_HALLAA;
		  else if(categoryInland == CAT_90100)
			return SISAMAASSA_YLEISESTI_HALLAA;
		}
	  else if(categoryCoastal == CAT_1020)
		{
		  if(categoryInland == CAT_NA)
			return RANNIKOLLA_MAHDOLLISESTI_HALLAA;
		  else if(categoryInland == CAT_0010 ||categoryInland == CAT_1020)
			return ALAVILLA_MAILLA_HALLAN_VAARA;
		  else if(categoryInland == CAT_3040)
			return MAHDOLLISESTI_HALLAA;
		  else if(categoryInland == CAT_5060)
			return PAIKOIN_HALLAA;
		  else if(categoryInland == CAT_7080)
			return SISAMAASSA_MONIN_PAIKOIN_HALLAA;
		  else if(categoryInland == CAT_90100)
			return SISAMAASSA_YLEISESTI_HALLAA;
		  else if(categoryInland == CAT_FROST)
			return EMPTY_STORY;
		}
	  else if(categoryCoastal == CAT_3040)
		{
		  if(categoryInland == CAT_NA || categoryInland == CAT_0010)
			return RANNIKOLLA_MAHDOLLISESTI_HALLAA;
		  else if(categoryInland == CAT_1020 || categoryInland == CAT_3040)
			return MAHDOLLISESTI_HALLAA;
		  else if(categoryInland == CAT_5060)
			return PAIKOIN_HALLAA;
		  else if(categoryInland == CAT_7080)
			return (SISAMAASSA_MONIN_PAIKOIN_HALLAA*100) + RANNIKOLLA_MAHDOLLISESTI_HALLAA;
		  else if(categoryInland == CAT_90100)
			return (SISAMAASSA_YLEISESTI_HALLAA*100) + RANNIKOLLA_MAHDOLLISESTI_HALLAA;
		  else if(categoryInland == CAT_FROST)
			
			return EMPTY_STORY;
		}
	  else if(categoryCoastal == CAT_5060)
		{
		  if(categoryInland == CAT_NA || categoryInland == CAT_0010)
			return RANNIKOLLA_PAIKOIN_HALLAA;
		  else if(categoryInland == CAT_1020 || categoryInland == CAT_3040 || categoryInland == CAT_5060)
			return PAIKOIN_HALLAA;
		  else if(categoryInland == CAT_7080)
			return MONIN_PAIKOIN_HALLAA;
		  else if(categoryInland == CAT_90100)
			return HALLAA_YLEISESTI;
		  else if(categoryInland == CAT_FROST)
			return EMPTY_STORY;
		}
	  else if(categoryCoastal == CAT_7080)
		{
		  if(categoryInland == CAT_NA || categoryInland == CAT_0010)
			return RANNIKOLLA_MONIN_PAIKOIN_HALLAA;
		  else if(categoryInland == CAT_1020)
			return (SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA*100) + RANNIKOLLA_MONIN_PAIKOIN_HALLAA;
		  else if(categoryInland == CAT_3040)
			return (RANNIKOLLA_MONIN_PAIKOIN_HALLAA*100) + SISAMAASSA_MAHDOLLISESTI_HALLAA;
		  else if(categoryInland == CAT_5060)
			return PAIKOIN_HALLAA;
		  else if(categoryInland == CAT_7080)
			return MONIN_PAIKOIN_HALLAA;
		  else if(categoryInland == CAT_90100)
			return HALLAA_YLEISESTI;
		  else if(categoryInland == CAT_FROST)
			return EMPTY_STORY;
		}
	  else if(categoryCoastal == CAT_90100)
		{
		  if(categoryInland == CAT_NA || categoryInland == CAT_0010)
			return RANNIKOLLA_HALLAA;
		  else if(categoryInland == CAT_1020)
			return (SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA*100) + RANNIKOLLA_HALLAA;
		  else if(categoryInland == CAT_3040)
			return (RANNIKOLLA_HALLAA*100) + SISAMAASSA_MAHDOLLISESTI_HALLAA;
		  else if(categoryInland == CAT_5060)
			return (RANNIKOLLA_HALLAA*100) + SISAMAASSA_PAIKOIN_HALLAA;
		  else if(categoryInland == CAT_7080 || categoryInland == CAT_90100)
			return HALLAA_YLEISESTI;
		  else if(categoryInland == CAT_FROST)
			return EMPTY_STORY;
		}
	  else if(categoryCoastal == CAT_FROST)
		{
		  return EMPTY_STORY; 
		}
	  return EMPTY_STORY;
	}
	
	Sentence get_frost_onenight_phrase(const int& phraseId,
									   const bool& tellSevereFrostStory)
	{
	  Sentence sentence;
	  
	  switch(phraseId)
		{
		case EMPTY_STORY:
		  {
			sentence << "";
		  }
		  break;
		case ALAVILLA_MAILLA_HALLAN_VAARA:
		  {
			sentence << HALLAN_VAARA_COMPOSITE_PHRASE 
					 << EMPTY_STRING 
					 << "alavilla mailla"; 
			// sentence << "alavilla mailla" << "hallan vaara"; 
		  }
		  break;
		case MAHDOLLISESTI_HALLAA:
		  {
			sentence << MAHDOLLISESTI_HALLAA_COMPOSITE_PHRASE
					 << EMPTY_STRING;
			  // sentence << "mahdollisesti" << "hallaa"; 
		  }
		  break;
		case PAIKOIN_HALLAA:
		  {
			sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE 
					 << EMPTY_STRING 
					 << "paikoin";
			// sentence << "paikoin" << "hallaa"; 
		  }
		  break;
		case MONIN_PAIKOIN_HALLAA:
		  {
			if(tellSevereFrostStory)
			  sentence << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE;
			else
			  sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE;
			
			sentence << EMPTY_STRING 
					 << "monin paikoin";

			/*
			sentence << "monin paikoin" << "hallaa"; 
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
				}*/
		  }
		  break;
		case HALLAA_YLEISESTI:
		  {
			if(tellSevereFrostStory)
			  sentence << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE;
			else
			  sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE;
			sentence << EMPTY_STRING 
					 << EMPTY_STRING;

			/*
			sentence << "hallaa"; 
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }
			*/
		  }
		  break;
		case YOPAKKASTA:
		  {
			sentence << "";
		  }
		  break;
		case RANNIKOLLA_HALLAN_VAARA:
		  {
			sentence << HALLAN_VAARA_COMPOSITE_PHRASE 
					 << "rannikolla" 
					 << EMPTY_STRING; 
			// sentence << "rannikolla" << "hallan vaara"; 
		  }
		  break;
		case RANNIKOLLA_MAHDOLLISESTI_HALLAA:
		  {
			sentence << MAHDOLLISESTI_HALLAA_COMPOSITE_PHRASE
					 << "rannikolla";
			  // sentence << "rannikolla" << "mahdollisesti" << "hallaa"; 
		  }
		  break;
		case RANNIKOLLA_PAIKOIN_HALLAA:
		  {
			sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE
					 << "rannikolla" 
					 << "paikoin"; 
			// sentence << "rannikolla" << "paikoin" << "hallaa"; 
		  }
		  break;
		case RANNIKOLLA_MONIN_PAIKOIN_HALLAA:
		  {
			if(tellSevereFrostStory)
			  sentence << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE;
			else
			  sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE;
			sentence << "rannikolla" 
					 << "monin paikoin";
			/*
			sentence << "rannikolla" << "monin paikoin" << "hallaa"; 
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }
			*/
		  }
		  break;
		case RANNIKOLLA_HALLAA:
		  {
			if(tellSevereFrostStory)
			  sentence << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE;
			else
			  sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE;
			sentence << "rannikolla" 
					 << EMPTY_STRING; 
			/*
			sentence << "rannikolla" << "hallaa"; 
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }
			*/
		  }
		  break;
		case SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA:
		  {
			sentence << HALLAN_VAARA_COMPOSITE_PHRASE 
					 << "sisämaassa"
					 << "alavilla mailla";
			//			sentence << "sisämaassa" << "alavilla mailla"  << "hallan vaara"; 
		  }
		  break;
		case SISAMAASSA_MAHDOLLISESTI_HALLAA:
		  {
			sentence << MAHDOLLISESTI_HALLAA_COMPOSITE_PHRASE
					 << "sisämaassa";
			//sentence << "sisämaassa" << "mahdollisesti"  << "hallaa"; 
		  }
		  break;
		case SISAMAASSA_PAIKOIN_HALLAA:
		  {
			sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE;
			sentence << "sisämaassa" 
					 << "paikoin"; 
			// sentence << "sisämaassa" << "paikoin" << "hallaa"; 
		  }
		  break;
		case SISAMAASSA_MONIN_PAIKOIN_HALLAA:
		  {
			if(tellSevereFrostStory)
			  sentence << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE;
			else
			  sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE;
			sentence << "sisämaassa" 
					 << "monin paikoin"; 
			/*
			sentence << "sisämaassa" << "monin paikoin" << "hallaa"; 
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }
			*/
		  }
		  break;
		case SISAMAASSA_YLEISESTI_HALLAA:
		  {
			if(tellSevereFrostStory)
			  sentence << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE;
			else
			  sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE;
			sentence << "sisämaassa" 
					 << EMPTY_STRING;
			/*
			sentence << "sisämaassa" << "hallaa"; 
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }
			*/
		  }
		  break;
		case (SISAMAASSA_MONIN_PAIKOIN_HALLAA*100) + RANNIKOLLA_MAHDOLLISESTI_HALLAA:
		  {
			if(tellSevereFrostStory)
			  sentence << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE;
			else
			  sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE;
			sentence << "sisämaassa" 
					 << "monin paikoin"
					 <<  Delimiter(COMMA_PUNCTUATION_MARK)
					 << MAHDOLLISESTI_HALLAA_COMPOSITE_PHRASE
					 << "rannikolla";
			/*
			sentence << "sisämaassa" << "monin paikoin" << "hallaa" << Delimiter(COMMA_PUNCTUATION_MARK);
			if(tellSevereFrostStory)
			  {
				sentence << "joka voi olla ankaraa";
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
			  }
			 sentence << "rannikolla" << "mahdollisesti" << "hallaa";  
			*/
		  }
		  break;
		case (SISAMAASSA_YLEISESTI_HALLAA*100) + RANNIKOLLA_MAHDOLLISESTI_HALLAA:
		  {
			if(tellSevereFrostStory)
			  sentence << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE;
			else
			  sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE;
			sentence << "sisämaassa" 
					 << EMPTY_STRING
					 <<  Delimiter(COMMA_PUNCTUATION_MARK)
					 << MAHDOLLISESTI_HALLAA_COMPOSITE_PHRASE
					 << "rannikolla";
			/*
			sentence << "sisämaassa" << "hallaa" << Delimiter(COMMA_PUNCTUATION_MARK); 
			if(tellSevereFrostStory)
			  {
				sentence << "joka voi olla ankaraa";
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
			  }
			 sentence << "rannikolla" << "mahdollisesti" << "hallaa";  
			*/
		  }
		  break;
		case (SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA*100) + RANNIKOLLA_PAIKOIN_HALLAA:
		  {
			sentence << HALLAN_VAARA_COMPOSITE_PHRASE
					 << "sisämaassa"
					 << "alavilla mailla"
					 << Delimiter(COMMA_PUNCTUATION_MARK)
					 << PAIKOIN_HALLAA_COMPOSITE_PHRASE
					 << "rannikolla" 
					 << "paikoin";

			/*
			sentence << "sisämaassa" << "alavilla mailla" << "hallan vaara" << Delimiter(COMMA_PUNCTUATION_MARK) 
					 << "rannikolla" << "paikoin" << "hallaa";  
			*/
		  }
		  break;
		case (SISAMAASSA_YLEISESTI_HALLAA*100) + RANNIKOLLA_PAIKOIN_HALLAA:
		  {
			if(tellSevereFrostStory)
			  sentence << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE;
			else
			  sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE;
			sentence << "sisämaassa" 
					 << EMPTY_STRING
					 <<  Delimiter(COMMA_PUNCTUATION_MARK)
					 << PAIKOIN_HALLAA_COMPOSITE_PHRASE
					 << "rannikolla"
					 << "monin paikoin";
			/*
			sentence << "sisämaassa" << "hallaa" << Delimiter(COMMA_PUNCTUATION_MARK);
			if(tellSevereFrostStory)
			  {
				sentence << "joka voi olla ankaraa";
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
			  }
			sentence << "rannikolla" << "monin paikoin" << "hallaa";  
			*/
		  }
		  break;
		case (SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA*100) + RANNIKOLLA_MONIN_PAIKOIN_HALLAA:
		  {
			sentence << HALLAN_VAARA_COMPOSITE_PHRASE
					 << "sisämaassa"
					 << "alavilla mailla"
					 << Delimiter(COMMA_PUNCTUATION_MARK);
			if(tellSevereFrostStory)
			  sentence << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE;
			else
			  sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE;
			sentence << "rannikolla"
					 << "monin paikoin";

			  /*
			sentence << "sisämaassa" << "alavilla mailla" << "hallan vaara" << Delimiter(COMMA_PUNCTUATION_MARK) 
					 << "rannikolla" << "monin paikoin" << "hallaa";
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }
			  */ 
		  }
		  break;
		case (RANNIKOLLA_MONIN_PAIKOIN_HALLAA*100) + SISAMAASSA_MAHDOLLISESTI_HALLAA:
		  {
			if(tellSevereFrostStory)
			  sentence << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE;
			else
			  sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE;
			sentence << "rannikolla"
					 << "monin paikoin"
					 << Delimiter(COMMA_PUNCTUATION_MARK)
					 << MAHDOLLISESTI_HALLAA_COMPOSITE_PHRASE
					 << "sisämaassa";
			/*
			sentence << "rannikolla" << "monin paikoin" << "hallaa" << Delimiter(COMMA_PUNCTUATION_MARK);
			if(tellSevereFrostStory)
			  {
				sentence << "joka voi olla ankaraa";
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
			  }
			 sentence << "sisämaassa" << "mahdollisesti" << "hallaa";
			*/
		  }
		  break;
		case (SISAMAASSA_PAIKOIN_HALLAA*100) + RANNIKOLLA_MONIN_PAIKOIN_HALLAA:
		  {
			sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE
					 << "sisämaassa"
					 << "paikoin"
					 << Delimiter(COMMA_PUNCTUATION_MARK);
			if(tellSevereFrostStory)
			  sentence << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE;
			else
			  sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE;
			sentence << "rannikolla"
					 << "monin paikoin";

			  /*
			sentence << "sisämaassa" << "paikoin" << "hallaa" << Delimiter(COMMA_PUNCTUATION_MARK) 
					 << "rannikolla" << "monin paikoin" << "hallaa";  
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }
			  */
		  }
		  break;
		case (SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA*100) + RANNIKOLLA_HALLAA:
		  {
			sentence << HALLAN_VAARA_COMPOSITE_PHRASE
					 << "sisämaassa" 
					 << "alavilla mailla"
					 << Delimiter(COMMA_PUNCTUATION_MARK);
			if(tellSevereFrostStory)
			  sentence << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE;
			else
			  sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE;
			sentence << "rannikolla"
					 << EMPTY_STRING;

			/*
			sentence << "sisämaassa" << "alavilla mailla" << "hallan vaara" << Delimiter(COMMA_PUNCTUATION_MARK) 
					 << "rannikolla" << "hallaa";  
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }
			*/
		  }
		  break;
		case (RANNIKOLLA_HALLAA*100) + SISAMAASSA_MAHDOLLISESTI_HALLAA:
		  {
			if(tellSevereFrostStory)
			  sentence << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE;
			else
			  sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE;
			sentence << "rannikolla"
					 << EMPTY_STRING;
			sentence << Delimiter(COMMA_PUNCTUATION_MARK)
					 << MAHDOLLISESTI_HALLAA_COMPOSITE_PHRASE
					 << "sisämaassa";
			/*
			sentence << "rannikolla" << "hallaa" << Delimiter(COMMA_PUNCTUATION_MARK);
			if(tellSevereFrostStory)
			  {
				sentence << "joka voi olla ankaraa";
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
			  }
			sentence << "sisämaassa" << "mahdollisesti" << "hallaa";
			*/
		  }
		  break;
		case (RANNIKOLLA_HALLAA*100) + SISAMAASSA_PAIKOIN_HALLAA:
		  {
			if(tellSevereFrostStory)
			  sentence << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE;
			else
			  sentence << PAIKOIN_HALLAA_COMPOSITE_PHRASE;
			sentence << "rannikolla"
					 << EMPTY_STRING;
			sentence << Delimiter(COMMA_PUNCTUATION_MARK)
					 << PAIKOIN_HALLAA_COMPOSITE_PHRASE
					 << "sisämaassa"
					 << "paikoin";
			/*
			sentence  << "rannikolla" << "hallaa" << Delimiter(COMMA_PUNCTUATION_MARK);
			if(tellSevereFrostStory)
			  {
				sentence << "joka voi olla ankaraa";
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
			  }
			sentence << "sisämaassa" << "paikoin" << "hallaa";
			*/
		  }
		  break;
		default:
		  {
			sentence << "";
		  }
		  break;
		} 
	  return sentence;
	}

#ifdef OLD_IMPLEMENTATION
	Sentence get_frost_onenight_phrase(const int& phraseId,
									   const bool& tellSevereFrostStory)
	{
	  Sentence sentence;
	  
	  switch(phraseId)
		{
		case EMPTY_STORY:
		  {
			sentence << "";
		  }
		  break;
		case ALAVILLA_MAILLA_HALLAN_VAARA:
		  {
			sentence << "alavilla mailla" << "hallan vaara"; 
		  }
		  break;
		case MAHDOLLISESTI_HALLAA:
		  {
			sentence << "mahdollisesti" << "hallaa"; 
		  }
		  break;
		case PAIKOIN_HALLAA:
		  {
			sentence << "paikoin" << "hallaa"; 
		  }
		  break;
		case MONIN_PAIKOIN_HALLAA:
		  {
			sentence << "monin paikoin" << "hallaa"; 
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }
		  }
		  break;
		case HALLAA_YLEISESTI:
		  {
			sentence << "hallaa"; 
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }
		  }
		  break;
		case YOPAKKASTA:
		  {
			sentence << "";
		  }
		  break;
		case RANNIKOLLA_HALLAN_VAARA:
		  {
			sentence << "rannikolla" << "hallan vaara"; 
		  }
		  break;
		case RANNIKOLLA_MAHDOLLISESTI_HALLAA:
		  {
			sentence << "rannikolla" << "mahdollisesti" << "hallaa"; 
		  }
		  break;
		case RANNIKOLLA_PAIKOIN_HALLAA:
		  {
			sentence << "rannikolla" << "paikoin" << "hallaa"; 
		  }
		  break;
		case RANNIKOLLA_MONIN_PAIKOIN_HALLAA:
		  {
			sentence << "rannikolla" << "monin paikoin" << "hallaa"; 
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }
		  }
		  break;
		case RANNIKOLLA_HALLAA:
		  {
			sentence << "rannikolla" << "hallaa"; 
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }
		  }
		  break;
		case SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA:
		  {
			sentence << "sisämaassa" << "alavilla mailla"  << "hallan vaara"; 
		  }
		  break;
		case SISAMAASSA_MAHDOLLISESTI_HALLAA:
		  {
			sentence << "sisämaassa" << "mahdollisesti"  << "hallaa"; 
		  }
		  break;
		case SISAMAASSA_PAIKOIN_HALLAA:
		  {
			sentence << "sisämaassa" << "paikoin" << "hallaa"; 
		  }
		  break;
		case SISAMAASSA_MONIN_PAIKOIN_HALLAA:
		  {
			sentence << "sisämaassa" << "monin paikoin" << "hallaa"; 
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }
		  }
		  break;
		case SISAMAASSA_YLEISESTI_HALLAA:
		  {
			sentence << "sisämaassa" << "hallaa"; 
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }
		  }
		  break;
		case (SISAMAASSA_MONIN_PAIKOIN_HALLAA*100) + RANNIKOLLA_MAHDOLLISESTI_HALLAA:
		  {
			sentence << "sisämaassa" << "monin paikoin" << "hallaa" << Delimiter(COMMA_PUNCTUATION_MARK);
			if(tellSevereFrostStory)
			  {
				sentence << "joka voi olla ankaraa";
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
			  }
			 sentence << "rannikolla" << "mahdollisesti" << "hallaa";  
		  }
		  break;
		case (SISAMAASSA_YLEISESTI_HALLAA*100) + RANNIKOLLA_MAHDOLLISESTI_HALLAA:
		  {
			sentence << "sisämaassa" << "hallaa" << Delimiter(COMMA_PUNCTUATION_MARK); 
			if(tellSevereFrostStory)
			  {
				sentence << "joka voi olla ankaraa";
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
			  }
			 sentence << "rannikolla" << "mahdollisesti" << "hallaa";  
		  }
		  break;
		case (SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA*100) + RANNIKOLLA_PAIKOIN_HALLAA:
		  {
			sentence << "sisämaassa" << "alavilla mailla" << "hallan vaara" << Delimiter(COMMA_PUNCTUATION_MARK) 
					 << "rannikolla" << "paikoin" << "hallaa";  
		  }
		  break;
		case (SISAMAASSA_YLEISESTI_HALLAA*100) + RANNIKOLLA_PAIKOIN_HALLAA:
		  {
			sentence << "sisämaassa" << "hallaa" << Delimiter(COMMA_PUNCTUATION_MARK);
			if(tellSevereFrostStory)
			  {
				sentence << "joka voi olla ankaraa";
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
			  }
			sentence << "rannikolla" << "monin paikoin" << "hallaa";  
		  }
		  break;
		case (SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA*100) + RANNIKOLLA_MONIN_PAIKOIN_HALLAA:
		  {
			sentence << "sisämaassa" << "alavilla mailla" << "hallan vaara" << Delimiter(COMMA_PUNCTUATION_MARK) 
					 << "rannikolla" << "monin paikoin" << "hallaa";
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }  
		  }
		  break;
		case (RANNIKOLLA_MONIN_PAIKOIN_HALLAA*100) + SISAMAASSA_MAHDOLLISESTI_HALLAA:
		  {
			sentence << "rannikolla" << "monin paikoin" << "hallaa" << Delimiter(COMMA_PUNCTUATION_MARK);
			if(tellSevereFrostStory)
			  {
				sentence << "joka voi olla ankaraa";
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
			  }
			 sentence << "sisämaassa" << "mahdollisesti" << "hallaa";
		  }
		  break;
		case (SISAMAASSA_PAIKOIN_HALLAA*100) + RANNIKOLLA_MONIN_PAIKOIN_HALLAA:
		  {
			sentence << "sisämaassa" << "paikoin" << "hallaa" << Delimiter(COMMA_PUNCTUATION_MARK) 
					 << "rannikolla" << "monin paikoin" << "hallaa";  
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }
		  }
		  break;
		case (SISAMAASSA_ALAVILLA_MAILLA_HALLAN_VAARA*100) + RANNIKOLLA_HALLAA:
		  {
			sentence << "sisämaassa" << "alavilla mailla" << "hallan vaara" << Delimiter(COMMA_PUNCTUATION_MARK) 
					 << "rannikolla" << "hallaa";  
			if(tellSevereFrostStory)
			  {
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
				sentence << "joka voi olla ankaraa";
			  }
		  }
		  break;
		case (RANNIKOLLA_HALLAA*100) + SISAMAASSA_MAHDOLLISESTI_HALLAA:
		  {
			sentence << "rannikolla" << "hallaa" << Delimiter(COMMA_PUNCTUATION_MARK);
			if(tellSevereFrostStory)
			  {
				sentence << "joka voi olla ankaraa";
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
			  }
			sentence << "sisämaassa" << "mahdollisesti" << "hallaa";
		  }
		  break;
		case (RANNIKOLLA_HALLAA*100) + SISAMAASSA_PAIKOIN_HALLAA:
		  {
			sentence  << "rannikolla" << "hallaa" << Delimiter(COMMA_PUNCTUATION_MARK);
			if(tellSevereFrostStory)
			  {
				sentence << "joka voi olla ankaraa";
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
			  }
			sentence << "sisämaassa" << "paikoin" << "hallaa";
		  }
		  break;
		default:
		  {
			sentence << "";
		  }
		  break;
		} 
	  return sentence;
	}
#endif

	const Sentence frost_onenight_sentence(const double& coastalFrostProbability, 
										   const double& inlandFrostProbability, 
										   const bool& severeFrostCoastal, 
										   const bool& severeFrostInland, 
										   const unsigned short& forecast_areas, 
										   const unsigned short& growing_season_started, 
										   const unsigned short& night_frost)
	{
	  Sentence sentence;
	  
	  int phraseId = get_frost_onenight_phrase_id(coastalFrostProbability, 
												  inlandFrostProbability, 
												  forecast_areas, 
												  growing_season_started, 
												  night_frost);

	  sentence << get_frost_onenight_phrase(phraseId, severeFrostCoastal || severeFrostInland);

	  return sentence;
	}
	
	const bool is_night_frost(MessageLogger& log, 
							  const std::string& theLogMessage, 
							  const std::string& theFakeVariable,
							  const double& theNightFrostLimit,
							  const GridForecaster& forecaster, 
							  const AnalysisSources& theSources, 
							  const WeatherArea& theArea, 
							  const WeatherPeriod& thePeriod)
	{
	  ComparativeAcceptor comparativeAcceptor(0.0, LESS_THAN);
	  WeatherResult nightFrostPercentage = forecaster.analyze(theFakeVariable,
															  theSources,
															  Temperature,
															  Percentage,
															  Minimum,
															  theArea,
															  thePeriod,
															  DefaultAcceptor(),
															  DefaultAcceptor(),
															  comparativeAcceptor);

	  log << NFmiStringTools::Convert(theLogMessage) << nightFrostPercentage << endl;

	  // At least 20% of the area has night frost
	  bool retval = nightFrostPercentage.value() != kFloatMissing && nightFrostPercentage.value() >= theNightFrostLimit;
	  
	  return retval;
	}
  } // namespace FrostOnenight

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on one night frost
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  const Paragraph FrostStory::onenight() const
  {
	using namespace FrostOnenight;


	MessageLogger log("FrostStory::onenight");

	  log << "forecasttime: "
			 << itsForecastTime
			 << endl;

	Paragraph paragraph;


#ifdef TESTII
	/*
	if(itsArea.isNamed())
	  {
		if(itsArea.name().compare("ahvenanmaa") != 0)
		  return paragraph;
	  }
	*/

	//#define HALLAN_VAARA_COMPOSITE_PHRASE "[sisämaassa] [alavilla mailla] hallan vaara"
	//#define PAIKOIN_HALLAA_COMPOSITE_PHRASE "[rannikolla] [paikoin] [hallaa]"
	//#define PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE "[rannikolla] [paikoin] hallaa, joka voi olla ankar"

	Sentence snt;
	//Sentence snt2;

	/*
	snt << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE
		<<  "sisämaassa"
		<< EMPTY_STRING;
	*/

	snt << PAIKOIN_HALLAA_JOKAVOIOLLA_ANKARAA_COMPOSITE_PHRASE
		<< "sisämaassa"
		<< EMPTY_STRING
		<< Delimiter(COMMA_PUNCTUATION_MARK)
		<< PAIKOIN_HALLAA_COMPOSITE_PHRASE
		<< "rannikolla"
		<< "mahdollisesti";
	

	/*
			sentence << "sisämaassa" << "hallaa" << Delimiter(COMMA_PUNCTUATION_MARK); 
			if(tellSevereFrostStory)
			  {
				sentence << "joka voi olla ankaraa";
				sentence << Delimiter(COMMA_PUNCTUATION_MARK);
			  }
			 sentence << "rannikolla" << "mahdollisesti" << "hallaa";  

	*/

	paragraph << snt;
	//	paragraph << snt2;
#endif


	GridForecaster forecaster;

	const int starthour    = Settings::require_hour(itsVar+"::night::starthour");
	const int endhour      = Settings::require_hour(itsVar+"::night::endhour");
	const int maxstarthour = Settings::optional_hour(itsVar+"::night::maxstarthour", starthour);
	const int minendhour   = Settings::optional_hour(itsVar+"::night::minendhour",endhour);

	const double required_night_frost_percentage = Settings::require_double(itsVar+"::required_night_frost_percentage");
	const double required_severe_frost_probability = Settings::require_double(itsVar+"::required_severe_frost_probability");
	std::string parameter_name(itsVar+"::required_growing_season_percentage::default");
	if(itsArea.isNamed() && (Settings::isset(itsVar+"::required_growing_season_percentage::"+itsArea.name())))
	  parameter_name = itsVar+"::required_growing_season_percentage::"+itsArea.name();

	const double required_growing_season_percentage = Settings::require_double(parameter_name);
												 
	log << "starthour " << starthour << endl;
	log << "endhour " << endhour << endl;
	log << "required_growing_season_percentage " << required_growing_season_percentage << endl;
	log << "required_night_frost_percentage " << required_night_frost_percentage << endl;
	log << "required_severe_frost_probability " << required_severe_frost_probability << endl;
	log << "maxstarthour " << maxstarthour << endl;
	log << "minendhour " << minendhour << endl;

	HourPeriodGenerator generator(itsPeriod, itsVar+"::night");

    // Too late for this night? Return empty story then
    if(generator.size() == 0)
	  {
		log << paragraph;
		return paragraph;
	  }

	WeatherPeriod night1 = WeatherPeriodTools::getPeriod(itsPeriod,
														 1,
														 starthour,
														 endhour,
														 maxstarthour,
														 minendhour);

	// Calculate values for coastal and inland area separately
	WeatherArea coastalArea = itsArea;
	coastalArea.type(WeatherArea::Coast);

	WeatherArea inlandArea = itsArea;
	inlandArea.type(WeatherArea::Inland);

	PositiveValueAcceptor positiveValueAcceptor;
	WeatherResult temperatureSumCoastal = forecaster.analyze(itsVar+"::fake::growing_season_percentange::coastal",
															 itsSources,
															 EffectiveTemperatureSum,
															 Percentage,
															 Maximum,
															 coastalArea,
															 night1,
															 DefaultAcceptor(),
															 DefaultAcceptor(),
															 positiveValueAcceptor);

	log << "actual growing season percentage, coastal: " << temperatureSumCoastal << endl;

	// Test if the growing season has started at coastal area
	bool growingSeasonCoastal = temperatureSumCoastal.value() != kFloatMissing && 
	  temperatureSumCoastal.value() >= required_growing_season_percentage;

	if(!growingSeasonCoastal)
	  log << "Growing season has not yet started on coastal area!" << endl;


	WeatherResult temperatureSumInland = forecaster.analyze(itsVar+"::fake::growing_season_percentange::inland",
															itsSources,
															EffectiveTemperatureSum,
															Percentage,
															Maximum,
															inlandArea,
															night1,
															DefaultAcceptor(),
															DefaultAcceptor(),
															positiveValueAcceptor);

	log << "actual growing season percentage, inland: " << temperatureSumInland << endl;

	// Test if the growing season has started at inland area
	bool growingSeasonInland = temperatureSumInland.value() != kFloatMissing && 
	  temperatureSumInland.value() >= required_growing_season_percentage;

	if(!growingSeasonInland)
	  log << "Growing season has not yet started on inland area!" << endl;

	if(!growingSeasonInland && !growingSeasonCoastal)
	  {
		return paragraph;
	  }

	unsigned short growing_season_started = 0x0;
	unsigned short forecast_areas = 0x0;
	unsigned short night_frost = 0x0;

	growing_season_started |= (growingSeasonCoastal ? COASTAL_AREA : 0x0); 
	growing_season_started |= (growingSeasonInland ? INLAND_AREA : 0x0); 
	forecast_areas |= (temperatureSumCoastal.value() != kFloatMissing ? COASTAL_AREA : 0x0); 
	forecast_areas |= (temperatureSumInland.value() != kFloatMissing ? INLAND_AREA : 0x0); 

	if(!(forecast_areas & (COASTAL_AREA | INLAND_AREA)))
	  {
		log << "Neither Coastal nor Inland area is included! " << endl;
		return paragraph;
	  }
	if(!(forecast_areas & COASTAL_AREA))
	  {
		log << "Coastal area is not included" << endl;
	  }
	if(!(forecast_areas & INLAND_AREA))
	  {
		log << "Inland area is not included" << endl;
	  }


	WeatherResult temperatureMinMinCoastal(kFloatMissing, 0.0);
	WeatherResult temperatureMinMinInland(kFloatMissing, 0.0);
	
	// coastal area is included and growing season has started
	if((forecast_areas & COASTAL_AREA) && (growing_season_started & COASTAL_AREA))
	  {
		bool isNightFrost =  is_night_frost(log, 
											"actual night frost percentage, coastal: ",
											itsVar+"::fake::night_frost_percentage::coastal",
											required_night_frost_percentage,
											forecaster, 
											itsSources, 
											coastalArea, 
											night1);

		night_frost |= (isNightFrost ? COASTAL_AREA : 0x0); 

	  }

	// inland area is included and growing season has started
	if((forecast_areas & INLAND_AREA) && (growing_season_started & INLAND_AREA))
	  {
		bool isNightFrost =  is_night_frost(log, 
											"actual night frost percentage, inland: ",
											itsVar+"::fake::night_frost_percentage::inland",
											required_night_frost_percentage,
											forecaster, 
											itsSources, 
											inlandArea, 
											night1);

		night_frost |= (isNightFrost ? INLAND_AREA : 0x0); 
	  }

	WeatherResult frostMaxMaxCoastal(kFloatMissing, 0.0);
	WeatherResult severeFrostMaxMaxCoastal(kFloatMissing, 0.0);
	WeatherResult frostMaxMaxInland(kFloatMissing, 0.0);
	WeatherResult severeFrostMaxMaxInland(kFloatMissing, 0.0);

	// night frost at both areas
	if(night_frost == (COASTAL_AREA | INLAND_AREA))
	  {
		log << "Night frost both on coastal and inland area!" << endl;
		return paragraph;
	  }
	else
	  {
		// do calculation only if coastal area is included,
		// growing season has started and there is no night frost
		if((forecast_areas & COASTAL_AREA) && 
		   (growing_season_started & COASTAL_AREA) && 
		   !(night_frost & COASTAL_AREA)) 
		  {
			frostMaxMaxCoastal = forecaster.analyze(itsVar+"::fake::frost_probability::coastal",
										  itsSources,
										  Frost,
										  Maximum,
										  Maximum,
										  coastalArea,
										  night1);

			log << "Frost_Maximum_Maximum Coastal: " << frostMaxMaxCoastal << endl;

			// if frost probability >= 70%, then examine severe frost probabailty
			if(frostMaxMaxCoastal.value() >= 70.0)
			  {
				severeFrostMaxMaxCoastal = forecaster.analyze(itsVar+"::fake::severe_frost_probability::coastal",
														itsSources,
														SevereFrost,
														Maximum,
														Maximum,
														coastalArea,
														night1);

				log << "Severe_Frost_Maximum_Maximum Coastal: " << severeFrostMaxMaxCoastal << endl;
			  }
		  }

		// do calculation only if inland area is included,
		// growing season has started and there is no night frost
		if((forecast_areas & INLAND_AREA) && 
		   (growing_season_started & INLAND_AREA) && 
		   !(night_frost & INLAND_AREA))
		  {
			frostMaxMaxInland = forecaster.analyze(itsVar+"::fake::frost_probability::inland",
										 itsSources,
										 Frost,
										 Maximum,
										 Maximum,
										 inlandArea,
										 night1);

			log << "Frost_Maximum_Maximum Inland: " << frostMaxMaxInland << endl;

			// if frost probability >= 70%, then examine severe frost probabailty
			if(frostMaxMaxInland.value() >= 70.0)
			  {
				severeFrostMaxMaxInland = forecaster.analyze(itsVar+"::fake::severe_frost_probability::inland",
															 itsSources,
															 SevereFrost,
															 Maximum,
															 Maximum,
															 inlandArea,
															 night1);

				log << "Severe_Frost_Maximum_Maximum Inland: " << severeFrostMaxMaxInland << endl;
			  }
		  }

		log << "forecast_areas:  " << forecast_areas << endl;
		log << "growing_season_started:  " << growing_season_started << endl;
		log << "night_frost:  " << night_frost << endl;

		bool is_severe_frost_coastal = (severeFrostMaxMaxCoastal.value() > required_severe_frost_probability &&
										severeFrostMaxMaxCoastal.value() <= 100);
		bool is_severe_frost_inland = (severeFrostMaxMaxInland.value() > required_severe_frost_probability &&
										severeFrostMaxMaxInland.value() <= 100);


		paragraph << frost_onenight_sentence(frostMaxMaxCoastal.value(),
											 frostMaxMaxInland.value(),
											 is_severe_frost_coastal,
											 is_severe_frost_inland,
											 forecast_areas,
											 growing_season_started,
											 night_frost);
	  }

	log << paragraph;
	return paragraph;
  }


} // namespace TextGen
  
// ======================================================================
  
