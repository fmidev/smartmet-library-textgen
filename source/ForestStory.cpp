// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::ForestStory
 */
// ======================================================================
/*!
 * \class TextGen::ForestStory
 *
 * \brief Generates stories on forest
 *
 */
// ======================================================================

#include "ForestStory.h"
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
  
  ForestStory::~ForestStory()
  {
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theForecastTime The forecast time
   * \param theSources The analysis sources
   * \param theArea The area to be analyzed
   * \param thePeriod The time interval to be analyzed
   * \param theVariable The associated configuration variable
   */
  // ----------------------------------------------------------------------
  
  ForestStory::ForestStory(const NFmiTime & theForecastTime,
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
  
  bool ForestStory::hasStory(const string & theName)
  {
	if(theName == "forestfireindex_twodays")
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
  
  const Paragraph ForestStory::makeStory(const string & theName) const
  {
	if(theName == "forestfireindex_twodays")
	  return forestfireindex_twodays();

	throw TextGenError("ForestStory cannot make story "+theName);

  }

} // namespace TextGen
  
// ======================================================================
  
