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
#include "MathTools.h"
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
	 * \brief Return smaller wind speed result
	 *
	 * \param theSpeed1 The first wind speed
	 * \param theSpeed2 The second wind speed
	 * \return The smaller wind speed
	 */
	// ----------------------------------------------------------------------

	const WeatherResult minspeed(const WeatherResult & theSpeed1,
								 const WeatherResult & theSpeed2)
	{
	  WeatherResult result(min(theSpeed1.value(),theSpeed2.value()),
						   max(theSpeed1.error(),theSpeed2.error()));
	  return result;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return greater wind speed result
	 *
	 * \param theSpeed1 The first wind speed
	 * \param theSpeed2 The second wind speed
	 * \return The greater wind speed
	 */
	// ----------------------------------------------------------------------

	const WeatherResult maxspeed(const WeatherResult & theSpeed1,
								 const WeatherResult & theSpeed2)
	{
	  WeatherResult result(max(theSpeed1.value(),theSpeed2.value()),
						   max(theSpeed1.error(),theSpeed2.error()));
	  return result;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return mean wind speed result
	 *
	 * \param theSpeed1 The first wind speed
	 * \param theSpeed2 The second wind speed
	 * \return The mean wind speed
	 */
	// ----------------------------------------------------------------------

	const WeatherResult meanspeed(const WeatherResult & theSpeed1,
								  const WeatherResult & theSpeed2)
	{
	  WeatherResult result(MathTools::mean(theSpeed1.value(),theSpeed2.value()),
						   max(theSpeed1.error(),theSpeed2.error()));
	  return result;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return smallest wind speed result
	 *
	 * \param theSpeed1 The first wind speed
	 * \param theSpeed2 The second wind speed
	 * \param theSpeed3 The third wind speed
	 * \return The smaller wind speed
	 */
	// ----------------------------------------------------------------------
	
	const WeatherResult minspeed(const WeatherResult & theSpeed1,
								 const WeatherResult & theSpeed2,
								 const WeatherResult & theSpeed3)
	{
	  WeatherResult result(MathTools::min(theSpeed1.value(),
										  theSpeed2.value(),
										  theSpeed3.value()),
						   MathTools::max(theSpeed1.error(),
										  theSpeed2.error(),
										  theSpeed3.error()));
	  return result;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return greatest wind speed result
	 *
	 * \param theSpeed1 The first wind speed
	 * \param theSpeed2 The second wind speed
	 * \param theSpeed3 The third wind speed
	 * \return The greater wind speed
	 */
	// ----------------------------------------------------------------------

	const WeatherResult maxspeed(const WeatherResult & theSpeed1,
								 const WeatherResult & theSpeed2,
								 const WeatherResult & theSpeed3)
	{
	  WeatherResult result(MathTools::max(theSpeed1.value(),
										  theSpeed2.value(),
										  theSpeed3.value()),
						   MathTools::max(theSpeed1.error(),
										  theSpeed2.error(),
										  theSpeed3.error()));
	  return result;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return mean wind speed result
	 *
	 * \param theSpeed1 The first wind speed
	 * \param theSpeed2 The second wind speed
	 * \param theSpeed3 The third wind speed
	 * \return The mean wind speed
	 */
	// ----------------------------------------------------------------------

	const WeatherResult meanspeed(const WeatherResult & theSpeed1,
								  const WeatherResult & theSpeed2,
								  const WeatherResult & theSpeed3)
	{
	  WeatherResult result(MathTools::mean(theSpeed1.value(),
										   theSpeed2.value(),
										   theSpeed3.value()),
						   MathTools::max(theSpeed1.error(),
										  theSpeed2.error(),
										  theSpeed3.error()));
	  return result;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Test if two wind speed ranges are close enough to be the same
	 *
	 * \param theMinSpeed1 The first range minimum
	 * \param theMaxSpeed1 The first range maximum
	 * \param theMinSpeed2 The second range minimum
	 * \param theMaxSpeed2 The second range maximum
	 * \param theVariable The variable containing similarity limits
	 * \return True if the intervals are close enough
	 */
	// ----------------------------------------------------------------------

	bool similar_speed_range(const WeatherResult & theMinSpeed1,
							 const WeatherResult & theMaxSpeed1,
							 const WeatherResult & theMinSpeed2,
							 const WeatherResult & theMaxSpeed2,
							 const string & theVariable)
	{
	  using Settings::optional_int;

	  const int same_minimum = optional_int(theVariable+"::same::minimum",0);
	  const int same_maximum = optional_int(theVariable+"::same::maximum",0);

	  const int minspeed1 = FmiRound(theMinSpeed1.value());
	  const int maxspeed1 = FmiRound(theMaxSpeed1.value());
	  const int minspeed2 = FmiRound(theMinSpeed2.value());
	  const int maxspeed2 = FmiRound(theMaxSpeed2.value());

	  return(abs(minspeed1-minspeed2) <= same_minimum &&
			 abs(maxspeed1-maxspeed2) <= same_maximum);
	  
	}

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
	  
	  const int minvalue = FmiRound(theMinSpeed.value());
	  const int maxvalue = FmiRound(theMaxSpeed.value());

	  if(maxvalue - minvalue < mininterval)
		{
		  sentence << "noin"
				   << Integer(FmiRound(theMeanSpeed.value()));
		}
	  else
		{
		  sentence << IntegerRange(minvalue,maxvalue);
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
