// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::PrecipitationStory
 */
// ======================================================================

#include "PrecipitationStory.h"
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
  
  PrecipitationStory::~PrecipitationStory()
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
  
  PrecipitationStory::PrecipitationStory(const NFmiTime & theForecastTime,
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
  
  bool PrecipitationStory::hasStory(const string & theName)
  {
	if(theName == "precipitation_total")
	  return true;
	if(theName == "precipitation_range")
	  return true;
	if(theName == "precipitation_classification")
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
  
  Paragraph PrecipitationStory::makeStory(const string & theName) const
  {
	if(theName == "precipitation_total")
	  return total();
	if(theName == "precipitation_range")
	  return range();
	if(theName == "precipitation_classification")
	  return classification();

	throw TextGenError("PrecipitationStory cannot make story "+theName);

  }

} // namespace TextGen
  
// ======================================================================
  
