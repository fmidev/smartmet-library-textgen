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
	 * \brief Generate the generic rain description story
	 *
	 * \param theSources The analysis sources
	 * \param theArea The area to be analyzed
	 * \param thePeriod The rainy period to be analyzed
	 * \param theVar The control variable
	 * \param theDay The day in question
	 * \return The rain description story
	 */
	// ----------------------------------------------------------------------

	Sentence rain_phrase(const WeatherAnalysis::AnalysisSources & theSources,
						 const WeatherAnalysis::WeatherArea & theArea,
						 const WeatherAnalysis::WeatherPeriod & thePeriod,
						 const std::string & theVar,
						 int theDay)
	{
	  Sentence s;
	  s << places_phrase(theSources,theArea,thePeriod,theVar,theDay)
		<< strength_phrase(theSources,theArea,thePeriod,theVar,theDay)
		<< "sadetta";
	  return s;
	}

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
	 * \return The "in places" description
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

	// ----------------------------------------------------------------------
	/*!
	 * \brief Generate the strength phrase for a rain
	 *
	 * The used variables are
	 * \code
	 * ::weak = [0-X]  (=0.3)
	 * ::hard = [0-X]  (=3.0)
	 * \endcode
	 *
	 * That is,
	 *  - If max intensity < 0.3, then use "heikkoa"
	 *  - If max intensity >= 3.0, then use "ajoittain kovaa"
	 *  - Else use ""
	 *
	 * \param theSources The analysis sources
	 * \param theArea The area to be analyzed
	 * \param thePeriod The rainy period to be analyzed
	 * \param theVar The control variable
	 * \param theDay The day in question
	 * \return The "in places" description
	 */
	// ----------------------------------------------------------------------
	
	Sentence strength_phrase(const AnalysisSources & theSources,
							 const WeatherArea & theArea,
							 const WeatherPeriod & thePeriod,
							 const string & theVar,
							 int theDay)
	{
	  MessageLogger log("PrecipitationStoryTools::strength_phrase");
	  
	  using namespace Settings;
	  
	  const double weak_limit = optional_double(theVar+"::weak",0.3);
	  const double hard_limit = optional_double(theVar+"::hard",3.0);
	  
	  GridForecaster forecaster;
	  
	  const string day = lexical_cast<string>(theDay);
	  WeatherResult result = forecaster.analyze(theVar+"::fake::day"+day+"::strength",
												theSources,
												Precipitation,
												Maximum,
												Maximum,
												theArea,
												thePeriod);
	  
	  log << "Precipitation maximum: " << result.value() << endl;
	  
	  if(result.value() == kFloatMissing)
		throw TextGenError("Precipitation maximum not available");
	  
	  Sentence s;
	  if(result.value() < weak_limit)
		s << "heikkoa";
	  else if(result.value() >= hard_limit)
		s << "ajoittain" << "kovaa";
	  return s;
	
	}

  } // namespace PrecipitationStoryTools
} // namespace TextGen

// ======================================================================
