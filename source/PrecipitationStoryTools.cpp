// ======================================================================
/*!
 * \file
 * \brief Implementation for namespace TextGen::PrecipitationStoryTools
 */
// ======================================================================
/*!
 * \namespace TextGen::PrecipitationStoryTools
 *
 * \brief Tools for TextGen::PrecipitationStory and similar classes
 *
 * This namespace contains utility functions which are not
 * exclusive for handling precipitation alone. For example,
 * TextGen::WeatherStory most likely has use for these
 * functions.
 */
// ======================================================================

#include "PrecipitationStoryTools.h"
#include "AnalysisSources.h"
#include "GridForecaster.h"
#include "MessageLogger.h"
#include "RangeAcceptor.h"
#include "Settings.h"
#include "Sentence.h"
#include "TextGenError.h"
#include "WeatherResult.h"

#include "NFmiGlobals.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace boost;
using namespace std;

namespace TextGen
{
  namespace PrecipitationStoryTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Generate the "in some place places" story
	 *
	 * The used variables are
	 * \code
	 * ::minrain      = [0-X]    (=0.1)
	 * ::places::many = [0-100]  (=90)
	 * ::places::some = [0-100]  (=70)
	 * \endcode
	 *
	 * That is,
	 *  - If percentage >= 90, then use ""
	 *  - If percentage >= 70, then use "in many places"
	 *  - Else use "in some places"
	 *
	 * \param theSources The analysis sources
	 * \param theArea The area to be analyzed
	 * \param thePeriod The rainy period to be analyzed
	 * \param theVar The control variable
	 * \param theDay The day in question
	 */
	// ----------------------------------------------------------------------
	
	Sentence places_phrase(const AnalysisSources & theSources,
						   const WeatherArea & theArea,
						   const WeatherPeriod & thePeriod,
						   const string & theVar,
						   int theDay)
	{
	  MessageLogger log("PrecipitationStoryTools::places_phrase");
	  
	  using namespace Settings;
	  
	  const int many_places = optional_percentage(theVar+"places::many",90);
	  const int some_places = optional_percentage(theVar+"places::some",50);
	  const double minrain  = optional_double(theVar+"::minrain",0.1);
	  
	  GridForecaster forecaster;
	  
	  RangeAcceptor rainlimits;
	  rainlimits.lowerLimit(minrain);
	  
	  const string day = lexical_cast<string>(theDay);
	  WeatherResult result = forecaster.analyze(theVar+"::fake::day"+day+"::places",
												theSources,
												Precipitation,
												Percentage,
												Maximum,
												theArea,
												thePeriod,
												DefaultAcceptor(),
												DefaultAcceptor(),
												rainlimits);
	  
	  log << "Precipitation percentage: " << result.value() << endl;
	  
	  if(result.value() == kFloatMissing)
		throw TextGenError("Precipitation percentage not available");
	  
	  Sentence s;
	  if(result.value() >= many_places)
		;
	  else if(result.value() >= some_places)
		s << "monin paikoin";
	  else
		s << "paikoin";
	  
	  return s;
	
	}

  } // namespace PrecipitationStoryTools
} // namespace TextGen

// ======================================================================
