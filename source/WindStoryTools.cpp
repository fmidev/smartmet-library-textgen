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
	  
	  const double accurate_limit = optional_double(theVariable+"::direction::accurate",22.5);
	  const double variable_limit = optional_double(theVariable+"::direction::variable_limit",45);
	  
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
	
	const Sentence direction_sentence(const WeatherResult & theDirection,
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

	const string direction_string(const WeatherResult & theDirection,
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

	const string direction16_string(const WeatherResult & theDirection,
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
				retval = "pohjoisen ja koillisen välistä tuulta";
				break;
			  case 3:
				retval = "koillistuulta";
				break;
			  case 4:
				retval = "idän ja koillisen välistä tuulta";
				break;
			  case 5:
				retval = "itätuulta";
				break;
			  case 6:
				retval = "idän ja kaakon välistä tuulta";
				break;
			  case 7:
				retval = "kaakkoistuulta";
				break;
			  case 8:
				retval = "etelän ja kaakon välistä tuulta";
				break;
			  case 9:
				retval = "etelätuulta";
				break;
			  case 10:
				retval = "etelän ja lounaaan välistä tuulta";
				break;
			  case 11:
				retval = "lounaistuulta";
				break;
			  case 12:
				retval = "lännen ja lounaan välistä tuulta";
				break;
			  case 13:
				retval = "länsituulta";
				break;
			  case 14:
				retval = "lännen ja luoteen välistä tuulta";
				break;
			  case 15:
				retval = "luoteistuulta";
				break;
			  case 16:
				retval = "pohjoisen ja luoteen välistä tuulta";
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
					retval = "idän puoleista";
				}
				break;
			  case 5:
				retval = "idän puoleista";
				break;
			  case 6:
				{
				  if(theDirection.value() < 112.5)
					retval = "idän puoleista";
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
					retval = "etelän puoleista";
				}
				break;
			  case 9:
				retval = "etelän puoleista";
				break;
			  case 10:
				{
				  if(theDirection.value() < 202.5)
					retval = "etelän puoleista";
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
					retval = "lännen puoleista";
				}
				break;
			  case 13:
				retval = "lännen puoleista";
				break;
			  case 14:
				{
				  if(theDirection.value() < 292.5)
					retval = "lännen puoleista";
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
	
	const Sentence speed_range_sentence(const WeatherResult & theMinSpeed,
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
	
	const Sentence directed_speed_sentence(const WeatherResult & theMinSpeed,
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

	const std::string speed_string(const WeatherResult& theMeanSpeed)
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


	const string directed_speed_string(const WeatherResult & theMeanSpeed,
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

	const string directed16_speed_string(const WeatherResult & theMeanSpeed,
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


  } // namespace WindStoryTools
} // namespace TextGen

// ======================================================================
