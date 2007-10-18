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

	  const int mininterval = optional_int(theVariable+"::mininterval",0);
	  const string rangeseparator = Settings::optional_string(theVariable+"::rangeseparator","-");
	  
	  const int minvalue = FmiRound(theMinSpeed.value());
	  const int maxvalue = FmiRound(theMaxSpeed.value());

	  if(maxvalue - minvalue < mininterval)
		{
		  sentence << "noin"
				   << Integer(FmiRound(theMeanSpeed.value()));
		}
	  else
		{
		  sentence << IntegerRange(minvalue,maxvalue,rangeseparator);
		}
	  sentence << *UnitFactory::create(MetersPerSecond);
	  
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

	  sentence << direction_sentence(theDirection,
									 theVariable)
			   << speed_range_sentence(theMinSpeed,
									   theMaxSpeed,
									   theMeanSpeed,
									   theVariable);
	  
	  return sentence;
	  
	}

  } // namespace WindStoryTools
} // namespace TextGen

// ======================================================================
