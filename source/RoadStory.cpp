// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::RoadStory
 */
// ======================================================================
/*!
 * \class TextGen::RoadStory
 *
 * \brief Generates stories on road forecasts
 *
 * \see page_tarinat
 */
// ======================================================================

#include "RoadStory.h"
#include "Paragraph.h"
#include "TextGenError.h"

using namespace WeatherAnalysis;
using namespace std;

namespace TextGen
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------
  
  RoadStory::~RoadStory()
  {
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theForecastTime The forecast time
   * \param theSources The analysis sources
   * \param theArea The area to be analyzed 
   * \param thePeriod The period to be analyzed
   * \param theVariable The associated configuration variable
  */
  // ----------------------------------------------------------------------
  
  RoadStory::RoadStory(const NFmiTime & theForecastTime,
					   const AnalysisSources & theSources,
					   const WeatherArea & theArea,
					   const WeatherPeriod & thePeriod,
					   const string & theVariable)
	: itsForecastTime(theForecastTime)
	, itsSources(theSources)
	, itsArea(theArea)
	, itsPeriod(thePeriod)
	, itsVar(theVariable)
  {
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Test whether the given story is known to this class
   *
   * \param theName The story name
   * \return True if this class can generate the story
   */
  // ----------------------------------------------------------------------
  
  bool RoadStory::hasStory(const string & theName)
  {
	if(theName == "roadtemperature_daynightranges")
	  return true;
	if(theName == "roadcondition_overview")
	  return true;
	return false;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Generate the desired story
   *
   * Throws if the story name is not recognized.
   *
   * \param theName The story name
   * \return Paragraph containing the story
   */
  // ----------------------------------------------------------------------.
  
  const Paragraph RoadStory::makeStory(const string & theName) const
  {
	if(theName == "roadtemperature_daynightranges")
	  return daynightranges();
	if(theName == "roadcondition_overview")
	  return condition_overview();
	throw TextGenError("RoadStory cannot make story "+theName);

  }


} // namespace TextGen
  
// ======================================================================
