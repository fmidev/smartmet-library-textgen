// ======================================================================
/*!
 * \file
 * \brief Implementation of class RelativeHumidityStory
 */
// ======================================================================

#include "RelativeHumidityStory.h"
#include "DefaultAcceptor.h"
#include "GridForecaster.h"
#include "Number.h"
#include "Paragraph.h"
#include "Sentence.h"
#include "TextGenError.h"
#include "WeatherFunction.h"
#include "WeatherParameter.h"
#include "WeatherResult.h"

using namespace WeatherAnalysis;
using namespace std;

namespace TextGen
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------
  
  RelativeHumidityStory::~RelativeHumidityStory()
  {
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theSources The analysis sources
   * \param theArea The area to be analyzed 
   * \param thePeriod The period to be analyzed
   * \param theVariable The associated configuration variable
  */
  // ----------------------------------------------------------------------
  
  RelativeHumidityStory::RelativeHumidityStory(const AnalysisSources & theSources,
											   const WeatherArea & theArea,
											   const WeatherPeriod & thePeriod,
											   const string & theVariable)
	: itsSources(theSources)
	, itsArea(theArea)
	, itsPeriod(thePeriod)
	, itsVariable(theVariable)
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
  
  bool RelativeHumidityStory::hasStory(const string & theName)
  {
	if(theName == "relative_humidity_lowest")
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
  
  Paragraph RelativeHumidityStory::makeStory(const string & theName) const
  {
	if(theName == "relative_humidity_lowest")
	  return lowest();

	throw TextGenError("RelativeHumidityStory cannot make story "+theName);

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on lowest relative humidity
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  Paragraph RelativeHumidityStory::lowest() const
  {
	Paragraph paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
  
