// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::WindStory
 */
// ======================================================================

#include "WindStory.h"
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
  
  WindStory::~WindStory()
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
  
  WindStory::WindStory(const NFmiTime & theForecastTime,
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
  
  bool WindStory::hasStory(const string & theName)
  {
	if(theName == "wind_overview")
	  return true;
	if(theName == "wind_simple_overview")
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
  
  Paragraph WindStory::makeStory(const string & theName) const
  {
	if(theName == "wind_overview")
	  return overview();
	if(theName == "wind_simple_overview")
	  return simple_overview();
	throw TextGenError("WindStory cannot make story "+theName);
  }

} // namespace TextGen
  
// ======================================================================
