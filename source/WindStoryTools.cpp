// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::WindStoryTools
 */
// ======================================================================
/*!
 * \namespace TextGen::WindStoryTools
 *
 * \brief Utility enumerations and functions for WindStory methods
 *
 */
// ======================================================================

#include "WindStoryTools.h"
#include "Integer.h"
#include "IntegerRange.h"
#include "Sentence.h"
#include "Settings.h"
#include "UnitFactory.h"
#include "WeatherResult.h"

#include <newbase/NFmiGlobals.h>

#include <boost/lexical_cast.hpp>
#include <cmath>

using namespace WeatherAnalysis;
using namespace boost;
using namespace std;

namespace TextGen
{
  namespace WindStoryTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Calculate the wind direction accuracy class
	 *
	 * \param theError The error estimate for the wind direction
	 * \param theVariable The control variable
	 * \return The accuracy class
	 */
	// ----------------------------------------------------------------------
	
	WindDirectionAccuracy direction_accuracy(double theError,
											 const string & theVariable)
	{
	  using Settings::optional_double;

	  const double accurate_limit = optional_double(theVariable+"::wind_direction::accurate_limit",22.5);
	  const double variable_limit = optional_double(theVariable+"::wind_direction::variable_limit",45);
	  
	  if(theError<=accurate_limit) return good_accuracy;
	  if(theError<=variable_limit) return moderate_accuracy;
	  return bad_accuracy;
	  
	}
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Calculate the generic wind 8th direction from angle
	 *
	 * \param theDirection The direction value
	 * \return The direction in units of 1-8 (north ... north-west)
	 */
	// ----------------------------------------------------------------------
	
	int direction8th(double theDirection)
	{
	  return 1+(int(theDirection/45.0+0.5) % 8);
	}
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Calculate the generic wind 16th direction from angle
	 *
	 * \param theDirection The direction value
	 * \return The direction in units of 1-16
	 */
	// ----------------------------------------------------------------------
	
	int direction16th(double theDirection)
	{
	  return 1+(int(theDirection/22.5+0.5) % 16);
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return a sentence on wind direction
	 *
	 * \param theDirection The direction
	 * \param theVariable The control variable
	 * \return The sentence
	 */
	// ----------------------------------------------------------------------
	
	Sentence direction_sentence(const WeatherResult & theDirection,
								const string & theVariable)
	{
	  Sentence sentence;

	  const int n = direction8th(theDirection.value());
	  switch(direction_accuracy(theDirection.error(),theVariable))
		{
		case good_accuracy:
		  sentence << lexical_cast<string>(n)+"-tuulta";
		  break;
		case moderate_accuracy:
		  sentence << lexical_cast<string>(n)+"-puoleista tuulta";
		  break;
		case bad_accuracy:
		  sentence << "suunnaltaan vaihtelevaa" << "tuulta";
		  break;
		}
	  return sentence;
	}

	string direction_string(const WeatherResult & theDirection,
							const string & theVariable)
	{
	  string retval;;

	  const int n = direction8th(theDirection.value());
	  switch(direction_accuracy(theDirection.error(),theVariable))
		{
		case good_accuracy:
		  {
			switch(n)
			  {
			  case 1:
				retval = "pohjoistuulta";
				break;
			  case 2:
				retval = "koillistuulta";
				break;
			  case 3:
				retval = "itatuulta";
				break;
			  case 4:
				retval = "kaakkoistuulta";
				break;
			  case 5:
				retval = "etelatuulta";
				break;
			  case 6:
				retval = "lounaistuulta";
				break;
			  case 7:
				retval = "lansituulta";
				break;
			  case 8:
				retval = "luoteistuulta";
				break;
			  default:
				break;
			  }
		  }
		  break;
		case moderate_accuracy:
		  {
			switch(n)
			  {
			  case 1:
				retval = "pohjoisen puoleista tuulta";
				break;
			  case 2:
				retval = "koillisen puoleista tuulta";
				break;
			  case 3:
				retval = "idan puoleista tuulta";
				break;
			  case 4:
				retval = "kaakon puoleista tuulta";
				break;
			  case 5:
				retval = "etelan puoleista tuulta";
				break;
			  case 6:
				retval = "lounaan puoleista tuulta";
				break;
			  case 7:
				retval = "lannen puoleista tuulta";
				break;
			  case 8:
				retval = "luoteen puoleista tuulta";
				break;
			  default:
				break;
			  }
		  }
		  break;
		case bad_accuracy:
		  retval = "suunnaltaan vaihtelevaa tuulta";
		  break;
		}
	  return retval;
	}

	string direction16_string(const WeatherResult & theDirection,
							  const string & theVariable)
	{
	  string retval;

	  const int n = direction16th(theDirection.value());

	  switch(direction_accuracy(theDirection.error(),theVariable))
		{
		case good_accuracy:
		  {
			switch(n)
			  {
			  case 1:
				retval = "pohjoistuulta";
				break;
			  case 2:
				retval = "pohjoisen ja koillisen valista tuulta";
				break;
			  case 3:
				retval = "koillistuulta";
				break;
			  case 4:
				retval = "idan ja koillisen valista tuulta";
				break;
			  case 5:
				retval = "itatuulta";
				break;
			  case 6:
				retval = "idan ja kaakon valista tuulta";
				break;
			  case 7:
				retval = "kaakkoistuulta";
				break;
			  case 8:
				retval = "etelan ja kaakon valista tuulta";
				break;
			  case 9:
				retval = "etelatuulta";
				break;
			  case 10:
				retval = "etelan ja lounaan valista tuulta";
				break;
			  case 11:
				retval = "lounaistuulta";
				break;
			  case 12:
				retval = "lannen ja lounaan valista tuulta";
				break;
			  case 13:
				retval = "lansituulta";
				break;
			  case 14:
				retval = "lannen ja luoteen valista tuulta";
				break;
			  case 15:
				retval = "luoteistuulta";
				break;
			  case 16:
				retval = "pohjoisen ja luoteen valista tuulta";
				break;
			  default:
				break;
			  }
		  }
		  break;
		case moderate_accuracy:
		  {
			switch(n)
			  {
			  case 1:
				retval = "pohjoisen puoleista";
				break;
			  case 2:
				{
				  if(theDirection.value() > 337.5 || theDirection.value() < 22.5)
					retval = "pohjoisen puoleista";
				  else
					retval = "koillisen puoleista";
				}
				break;
			  case 3:
				retval = "koillisen puoleista";
				break;
			  case 4:
				{
				  if(theDirection.value() < 67.5)
					retval = "koillisen puoleista";
				  else
					retval = "idan puoleista";
				}
				break;
			  case 5:
				retval = "idan puoleista";
				break;
			  case 6:
				{
				  if(theDirection.value() < 112.5)
					retval = "idan puoleista";
				  else
					retval = "kaakon puoleista";
				}
				break;
			  case 7:
				retval = "kaakon puoleista";
				break;
			  case 8:
				{
				  if(theDirection.value() < 157.5)
					retval = "kaakon puoleista";
				  else
					retval = "etelan puoleista";
				}
				break;
			  case 9:
				retval = "etelan puoleista";
				break;
			  case 10:
				{
				  if(theDirection.value() < 202.5)
					retval = "etelan puoleista";
				  else
					retval = "lounaan puoleista";
				}
				break;
			  case 11:
				retval = "lounaan puoleista";
				break;
			  case 12:
				{
				  if(theDirection.value() < 247.5)
					retval = "lounaan puoleista";
				  else
					retval = "lannen puoleista";
				}
				break;
			  case 13:
				retval = "lannen puoleista";
				break;
			  case 14:
				{
				  if(theDirection.value() < 292.5)
					retval = "lannen puoleista";
				  else
					retval = "luoteen puoleista";
				}
				break;
			  case 15:
				retval = "luoteen puoleista";
				break;
			  case 16:
				{
				  if(theDirection.value() < 337.5)
					retval = "luoteen puoleista";
				  else
					retval = "pohjoisen puoleista";
				}
				break;
			  default:
				break;
			  }
		  }
		  break;
		case bad_accuracy:
		  retval = "suunnaltaan vaihtelevaa tuulta";
		  break;
		}
	  return retval;
	}
	// ----------------------------------------------------------------------
	/*!
	 * \brief Return a sentence on wind speed range
	 *
	 * \param theMinSpeed The minimum speed
	 * \param theMaxSpeed The maximum speed
	 * \param theMeanSpeed The mean speed
	 * \param theVariable The control variable
	 * \return The sentence
	 */
	// ----------------------------------------------------------------------
	
	Sentence speed_range_sentence(const WeatherResult & theMinSpeed,
								  const WeatherResult & theMaxSpeed,
								  const WeatherResult & theMeanSpeed,
								  const string & theVariable)
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
		  sentence << *UnitFactory::create(MetersPerSecond, meanvalue) << "tuulta";
		}
	  else
		{
		  const int mininterval = optional_int(theVariable+"::mininterval",0);
		  const string rangeseparator = Settings::optional_string(theVariable+"::rangeseparator","-");
	  
		  if(maxvalue - minvalue < mininterval)
			{
			  sentence << "noin"
					   << Integer(static_cast<int>(round(theMeanSpeed.value())));
			}
		  else
			{
			  sentence << IntegerRange(minvalue,maxvalue,rangeseparator);
			}
		  sentence << *UnitFactory::create(MetersPerSecond);
		}
	  
	  return sentence;
	  
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return a sentence on wind speed and direction
	 *
	 * The sentence is of form "[suuntatuulta] X...Y m/s"
	 *
	 * \param theMinSpeed The minimum speed
	 * \param theMaxSpeed The maximum speed
	 * \param theMeanSpeed The mean speed
	 * \param theDirection The direction
	 * \param theVariable The control variable
	 * \return The sentence
	 */
	// ----------------------------------------------------------------------
	
	Sentence directed_speed_sentence(const WeatherResult & theMinSpeed,
									 const WeatherResult & theMaxSpeed,
									 const WeatherResult & theMeanSpeed,
									 const WeatherResult & theDirection,
									 const string & theVariable)
	{
	  Sentence sentence;

	  const string var = "textgen::units::meterspersecond::format";
	  const string opt = Settings::optional_string(var,"SI");

	  if(opt == "textphrase")
		{
		  int int_value(static_cast<int>(round(theMeanSpeed.value())));
		  if(int_value >= 1)
			{
			  sentence << *UnitFactory::create(MetersPerSecond, int_value);
			  if(int_value <= 20)
				sentence << direction_sentence(theDirection,
											   theVariable);
			}			  
		}
	  else
		{
		  sentence << direction_sentence(theDirection,
										 theVariable)
				   << speed_range_sentence(theMinSpeed,
										   theMaxSpeed,
										   theMeanSpeed,
										   theVariable);
		}
	  
	  return sentence;
	  
	}

	std::string speed_string(const WeatherResult& theMeanSpeed)
	{
	  std::string retval;

	  int int_value(static_cast<int>(round(theMeanSpeed.value())));

	  if(int_value >= 1)
		{
		  if(int_value >= 1 && int_value <= 3)
			{
			  retval = "heikkoa" ;
			}
		  else if(int_value >= 4 && int_value <= 7)
			{
			  retval = "kohtalaista";
			}
		  else if(int_value >= 8 && int_value <= 13)
			{
			  retval = "navakkaa";
			}
		  else if(int_value >= 14 && int_value <= 20)
			{
			  retval = "kovaa";
			}
		  else if(int_value >= 21 && int_value <= 32)
			{
			  retval = "myrskya";
			}
		  else
			{
			  retval = "hirmumyrskya";
			}

		}			  
	  return retval;	  
	}


	string directed_speed_string(const WeatherResult & theMeanSpeed,
								 const WeatherResult & theDirection,
								 const string & theVariable)
	{
	  string retval;

	  int int_value(static_cast<int>(round(theMeanSpeed.value())));

	  if(int_value >= 1)
		{
		  if(int_value >= 1 && int_value <= 3)
			{
			  retval = "heikkoa" ;
			}
		  else if(int_value >= 4 && int_value <= 7)
			{
			  retval = "kohtalaista";
			}
		  else if(int_value >= 8 && int_value <= 13)
			{
			  retval = "navakkaa";
			}
		  else if(int_value >= 14 && int_value <= 20)
			{
			  retval = "kovaa";
			}
		  else if(int_value >= 21 && int_value <= 32)
			{
			  retval = "myrskya";
			}
		  else if(int_value > 32)
			{
			  retval = "hirmumyrskya";
			}

		  if(int_value <= 20)
			{
			  retval += " ";
			  retval += direction_string(theDirection,
										 theVariable);
			}
		}			  
	  return retval;	  
	}

	string directed16_speed_string(const WeatherResult & theMeanSpeed,
								   const WeatherResult & theDirection,
								   const string & theVariable)
	{
	  string retval;

	  int int_value(static_cast<int>(round(theMeanSpeed.value())));

	  if(int_value >= 1)
		{
		  if(int_value >= 1 && int_value <= 3)
			{
			  retval = "heikkoa" ;
			}
		  else if(int_value >= 4 && int_value <= 7)
			{
			  retval = "kohtalaista";
			}
		  else if(int_value >= 8 && int_value <= 13)
			{
			  retval = "navakkaa";
			}
		  else if(int_value >= 14 && int_value <= 20)
			{
			  retval = "kovaa";
			}
		  else if(int_value >= 21 && int_value <= 32)
			{
			  retval = "myrskya";
			}
		  else if(int_value > 32)
			{
			  retval = "hirmumyrskya";
			}

		  retval += " ";
		  retval += direction16_string(theDirection,
									   theVariable);
		}
  
	  return retval;  
	}

	WindSpeedId wind_speed_id(const WeatherResult& theWindSpeed)
	{
	  return wind_speed_id(theWindSpeed.value());
	}

	WindSpeedId wind_speed_id(const float& theWindSpeed)
	{
	  if(theWindSpeed < HEIKKO_LOWER_LIMIT)
		return TYYNI;
	  else if(theWindSpeed >= HEIKKO_LOWER_LIMIT && theWindSpeed < HEIKKO_UPPER_LIMIT)
		return HEIKKO;
	  else if(theWindSpeed >= KOHTALAINEN_LOWER_LIMIT && theWindSpeed < KOHTALAINEN_UPPER_LIMIT)
		return KOHTALAINEN;
	  else if(theWindSpeed >= NAVAKKA_LOWER_LIMIT && theWindSpeed < NAVAKKA_UPPER_LIMIT)
		return NAVAKKA;
	  else if(theWindSpeed >= KOVA_LOWER_LIMIT && theWindSpeed < KOVA_UPPER_LIMIT)
		return KOVA;
	  else if(theWindSpeed >= MYRSKY_LOWER_LIMIT && theWindSpeed < MYRSKY_UPPER_LIMIT)
		return MYRSKY;
	  else
		return HIRMUMYRSKY;
	}

	std::string wind_speed_string(const WindSpeedId& theWindSpeedId)
	{
	  std::string retval;

	  switch(theWindSpeedId)
		{
		case TYYNI:
		  retval = TYYNI_WORD;		
		  break;
		case HEIKKO:
		  retval = HEIKKO_WORD;
		  break;
		case KOHTALAINEN:
		  retval = KOHTALAINEN_WORD;
		  break;
		case NAVAKKA:
		  retval = NAVAKKA_WORD;
		  break;
		case KOVA:
		  retval = KOVA_WORD;
		  break;
		case MYRSKY:
		  retval = MYRSKY_WORD;
		  break;
		case HIRMUMYRSKY:
		  retval = HIRMUMYRSKY_WORD;
		  break;
		}

	  return retval;
	}
		
	pair<int, int> wind_speed_interval(const wind_speed_vector& theWindSpeedVector)
	{
	  pair<int, int> retval;
	  
	  float min_value(kFloatMissing);
	  float max_value(kFloatMissing);
	  
	  for(unsigned int i = 0; i < theWindSpeedVector.size(); i++)
		{
		  if(i == 0)
			{
			  min_value = theWindSpeedVector[i].first;
			  max_value = theWindSpeedVector[i].second;
			}
		  else
			{
			  if(theWindSpeedVector[i].first < min_value)
				min_value = theWindSpeedVector[i].first;
			  if(theWindSpeedVector[i].second < max_value)
				max_value = theWindSpeedVector[i].second;
			}
		}
	  retval.first = static_cast<int>(round(min_value));
	  retval.second = static_cast<int>(round(max_value));

	  return retval;
	}


	WindDirectionId puoleinen_direction_id(const float& theWindDirection,
										   const WindDirectionId& theWindDirectionId)
	{

	  WindDirectionId windDirectionId(MISSING_WIND_DIRECTION_ID);

	  switch(theWindDirectionId)
		{
		case 1:
		  {
			windDirectionId = POHJOISEN_PUOLEINEN;
		  }
		  break;
		case 2:
		  {
			if(theWindDirection > 337.5 || theWindDirection < 22.5)
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
			if(theWindDirection < 67.5)
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
			if(theWindDirection < 112.5)
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
			if(theWindDirection < 157.5)
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
			if(theWindDirection < 202.5)
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
			if(theWindDirection < 247.5)
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
			if(theWindDirection < 292.5)
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
			if(theWindDirection < 337.5)
			  windDirectionId = LUOTEEN_PUOLEINEN;
			else
			  windDirectionId = POHJOISEN_PUOLEINEN;
		  }
		  break;
		default:
		  break;
		}
	
	  return windDirectionId;
	}

	WindDirectionId wind_direction_id(const WeatherAnalysis::WeatherResult& theWindDirection, 
									  const WeatherAnalysis::WeatherResult& theMaxWindSpeed,
									  const string& theVariable)
	{
	  WindDirectionAccuracy accuracy(direction_accuracy(theWindDirection.error(), theVariable));

	  WindDirectionId windDirectionId(MISSING_WIND_DIRECTION_ID);

	  // wind speed is > 6.5 m/s it can not be vaihteleva
	  if(accuracy == bad_accuracy && theMaxWindSpeed.value() <= 6.5)
		{
		  windDirectionId = VAIHTELEVA;
		}
	  else
		{
		  windDirectionId = static_cast<WindDirectionId>(direction16th(theWindDirection.value()));

		  if(accuracy != good_accuracy)
			windDirectionId = puoleinen_direction_id(theWindDirection.value(), windDirectionId);
		}
	  
	  return windDirectionId;
	}

	WindDirectionId plain_wind_direction_id(const WeatherAnalysis::WeatherResult& theWindDirection, 
											const WeatherAnalysis::WeatherResult& theMaxWindSpeed,
											const string& theVariable)
	{
	  WindDirectionAccuracy accuracy(direction_accuracy(theWindDirection.error(), theVariable));

	  WindDirectionId windDirectionId(MISSING_WIND_DIRECTION_ID);

	  // wind speed is > 6.5 m/s it can not be vaihteleva
	  if(accuracy == bad_accuracy && theMaxWindSpeed.value() <= 6.5)
		{
		  windDirectionId = VAIHTELEVA;
		}
	  else
		{
		  windDirectionId = static_cast<WindDirectionId>(direction16th(theWindDirection.value()));
		}
	  
	  return windDirectionId;
	}

	std::string wind_direction_string(const WindDirectionId& theWindDirectionId)
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

	float direction_difference(const float& direction1, const float& direction2)
	{
	  float difference = direction2 - direction1;
	  if(abs(difference) > 180.0)
		difference = ((direction2 > direction1 ? direction2 - (direction1+360) : direction1 - (direction2+360)));

	  return difference;
	}
	
	bool wind_direction_turns(const WeatherAnalysis::WeatherResult& theDirectionStart,
							  const WeatherAnalysis::WeatherResult& theDirectionEnd,
							  const WeatherAnalysis::WeatherResult& theMaxSpeedStart,
							  const WeatherAnalysis::WeatherResult& theMaxSpeedEnd,
							  const string& theVariable)
	{
	  WindDirectionId windDirectionIdBeg(wind_direction_id(theDirectionStart,
														   theMaxSpeedStart,
														   theVariable));
	  WindDirectionId windDirectionIdEnd(wind_direction_id(theDirectionEnd,
														   theMaxSpeedEnd,
														   theVariable));

	  float directionDifference(abs(direction_difference(theDirectionEnd.value(),
														 theDirectionStart.value())));

	  return (directionDifference > 45.0 && windDirectionIdBeg != windDirectionIdEnd);
	}

	bool same_direction(const WeatherAnalysis::WeatherResult& theDirection1,
						const WeatherAnalysis::WeatherResult& theDirection2,
						const WeatherAnalysis::WeatherResult& theMaxSpeed1,
						const WeatherAnalysis::WeatherResult& theMaxSpeed2,
						const string& theVariable,
						const bool& ignore_suuntainen)
	{
	  WindDirectionId direction1(wind_direction_id(theDirection1, theMaxSpeed1, theVariable));
	  WindDirectionId direction2(wind_direction_id(theDirection2, theMaxSpeed2, theVariable));
	  
	  if(direction1 == direction2)
		return true;

	  WindDirectionId plainDirection1(plain_wind_direction_id(theDirection1, theMaxSpeed1, theVariable));
	  WindDirectionId plainDirection2(plain_wind_direction_id(theDirection2, theMaxSpeed2, theVariable));

	  if(plainDirection1 == plainDirection2 && ignore_suuntainen)
		return true;

	  return false;
	}

	bool same_direction(const WindDirectionId& theDirection1,
						const WindDirectionId& theDirection2,
						const bool& ignore_suuntainen)
	{
	  if(theDirection1 == theDirection2)
		return true;

	  bool retval = false;

	  // e.g. "pohjoinen" and "pohjoisen puoleinen" are considered the same
	  if(ignore_suuntainen)
		{
		  switch(theDirection1)
			{
			case POHJOINEN:
			  retval = (theDirection2 == POHJOISEN_PUOLEINEN);
			  break;
			case POHJOISEN_PUOLEINEN:
			  retval = (theDirection2 == POHJOINEN);
			  break;
			case POHJOINEN_KOILLINEN:
			  //empty
			  break;
			case KOILLINEN:
			  retval = (theDirection2 == KOILLISEN_PUOLEINEN);
			  break;
			case KOILLISEN_PUOLEINEN:
			  retval = (theDirection2 == KOILLINEN);
			  break;
			case ITA_KOILLINEN:
			  //empty
			  break;
			case ITA:
			  retval = (theDirection2 == IDAN_PUOLEINEN);
			  break;
			case IDAN_PUOLEINEN:
			  retval = (theDirection2 == ITA);
			  break;
			case ITA_KAAKKO:
			  //empty
			  break;
			case KAAKKO:
			  retval = (theDirection2 == KAAKON_PUOLEINEN);
			  break;
			case KAAKON_PUOLEINEN:
			  retval = (theDirection2 == KAAKKO);
			  break;
			case ETELA_KAAKKO:
			  //empty
			  break;
			case ETELA:
			  retval = (theDirection2 == ETELAN_PUOLEINEN);
			  break;
			case ETELAN_PUOLEINEN:
			  retval = (theDirection2 == ETELA);
			  break;
			case ETELA_LOUNAS:
			  //empty
			  break;
			case LOUNAS:
			  retval = (theDirection2 == LOUNAAN_PUOLEINEN);
			  break;
			case LOUNAAN_PUOLEINEN:
			  retval = (theDirection2 == LOUNAS);
			  break;
			case LANSI_LOUNAS:
			  //empty
			  break;
			case LANSI:
			  retval = (theDirection2 == LANNEN_PUOLEINEN);
			  break;
			case LANNEN_PUOLEINEN:
			  retval = (theDirection2 == LANSI);
			  break;
			case LANSI_LUODE:
			  //empty
			  break;
			case LUODE:
			  retval = (theDirection2 == LUOTEEN_PUOLEINEN);
			  break;
			case LUOTEEN_PUOLEINEN:
			  retval = (theDirection2 == LUODE);
			  break;
			case POHJOINEN_LUODE:
			  //empty
			  break;
			case VAIHTELEVA:
			case MISSING_WIND_DIRECTION_ID:
			  //empty
			  break;
			}
		}

	  return retval;
	}

  } // namespace WindStoryTools
} // namespace TextGen

// ======================================================================
