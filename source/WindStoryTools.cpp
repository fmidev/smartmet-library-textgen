// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::WindStoryTools
 */
// ======================================================================

#include "WindStoryTools.h"
#include "IntegerRange.h"
#include "Sentence.h"
#include "Settings.h"
#include "UnitFactory.h"
#include "WeatherResult.h"

#include "NFmiGlobals.h"

#include "boost/lexical_cast.hpp"

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
	 * \param theAccuracy The accuracy
	 * \param theVariable The control variable
	 * \return The accuracy class
	 */
	// ----------------------------------------------------------------------
	
	WindDirectionAccuracy direction_accuracy(double theAccuracy,
											 const string & theVariable)
	{
	  using namespace Settings;
	  
	  const double accurate_limit = optional_double(theVariable+"::direction::accurate",0.8);
	  const double variable_limit = optional_double(theVariable+"::direction::variable_limit",0.4);
	  
	  if(theAccuracy>=accurate_limit) return good_accuracy;
	  if(theAccuracy>=variable_limit) return moderate_accuracy;
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
	
	Sentence direction_sentence(const WeatherResult & theDirection,
								const string & theVariable)
	{
	  Sentence sentence;

	  const int n = direction8th(theDirection.value());
	  switch(direction_accuracy(theDirection.accuracy(),theVariable))
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
	 * \param theVariable The control variable
	 * \return The sentence
	 */
	// ----------------------------------------------------------------------
	
	Sentence speed_range_sentence(const WeatherResult & theMinSpeed,
								  const WeatherResult & theMaxSpeed,
								  const string & theVariable)
	{
	  Sentence sentence;
	  
	  IntegerRange range(FmiRound(theMinSpeed.value()),
						 FmiRound(theMaxSpeed.value()));
	  
	  sentence << range << *UnitFactory::create(MetersPerSecond);
	  
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
	 * \param theDirection The direction
	 * \param theVariable The control variable
	 * \return The sentence
	 */
	// ----------------------------------------------------------------------
	
	Sentence directed_speed_sentence(const WeatherResult & theMinSpeed,
									 const WeatherResult & theMaxSpeed,
									 const WeatherResult & theDirection,
									 const string & theVariable)
	{
	  Sentence sentence;

	  sentence << direction_sentence(theDirection,theVariable)
			   << speed_range_sentence(theMinSpeed,theMaxSpeed,theVariable);
	  
	  return sentence;
	  
	}

  } // namespace WindStoryTools
} // namespace TextGen

// ======================================================================