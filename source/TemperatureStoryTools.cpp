// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::TemperatureStoryTools
 */
// ======================================================================
/*!
 * \namespace TextGen::TemperatureStoryTools
 *
 * \brief Common utilities for TemperatureStory methods
 *
 */
// ======================================================================

#include "TemperatureStoryTools.h"
#include "Integer.h"
#include "IntegerRange.h"
#include "Sentence.h"
#include "Settings.h"
#include "UnitFactory.h"

using namespace std;

namespace TextGen
{
  namespace TemperatureStoryTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return temperature comparison phrase
	 *
	 * \param theMean1 The first mean
	 * \param theMean2 The second mean
	 * \param theVariable The variable containing the limits
	 */
	// ----------------------------------------------------------------------
	
	const char * temperature_comparison_phrase(int theMean1,
											   int theMean2,
											   const string & theVariable)
	{
	  using namespace Settings;
	  
	  const int significantly_higher = require_percentage(theVariable+"::comparison::significantly_higher");
	  const int significantly_lower = require_percentage(theVariable+"::comparison::significantly_lower");
	  const int higher = require_percentage(theVariable+"::comparison::higher");
	  const int lower = require_percentage(theVariable+"::comparison::lower");
	  const int somewhat_higher = require_percentage(theVariable+"::comparison::somewhat_higher");
	  const int somewhat_lower = require_percentage(theVariable+"::comparison::somewhat_lower");
	  
	  if(theMean2 - theMean1 >= significantly_higher)
		return "huomattavasti korkeampi";
	  if(theMean2 - theMean1 >= higher)
		return "korkeampi";
	  if(theMean2 - theMean1 >= somewhat_higher)
		return "hieman korkeampi";
	  if(theMean1 - theMean2 >= significantly_lower)
		return "huomattavasti alempi";
	  if(theMean1 - theMean2 >= lower)
		return "alempi";
	  if(theMean1 - theMean2 >= somewhat_lower)
		return "hieman alempi";
	  return "suunnilleen sama";
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return temperature sentence
	 *
	 * Possible sentences are
	 *
	 *  - "noin x astetta"
	 *  - "x...y astetta";
	 *
	 * \param theMinimum The minimum temperature
	 * \param theMean The mean temperature
	 * \param theMaximum The maximum temperature
	 * \param theMinInterval The minimum interval limit
	 * \param theZeroFlag True if zero is always intervalled
	 * \return The sentence
	 */
	// ----------------------------------------------------------------------
	
	TextGen::Sentence temperature_sentence(int theMinimum,
										   int theMean,
										   int theMaximum,
										   int theMinInterval,
										   bool theZeroFlag)
	{
	  Sentence sentence;
	  
	  bool range = true;
	  
	  if(theMinimum == theMaximum)
		range = false;
	  else if(theMaximum - theMinimum >= theMinInterval)
		range = true;
	  else if(theMinimum <= 0 && theMaximum >= 0)
		range = true;
	  else
		range = false;
	  
	  if(range)
		{
		  sentence << IntegerRange(theMinimum,theMaximum)
				   << *UnitFactory::create(DegreesCelsius);
		}
	  else
		{
		  sentence << "noin"
				   << Integer(theMean)
				   << *UnitFactory::create(DegreesCelsius);
		}
	  
	  return sentence;
	}

  } // namespace TemperatureStoryTools
} // namespace TextGen

// ======================================================================
