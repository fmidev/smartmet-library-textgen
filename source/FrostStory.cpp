// ======================================================================
/*!
 * \file
 * \brief Implementation of class FrostStory
 */
// ======================================================================

#include "FrostStory.h"
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
  
  FrostStory::~FrostStory()
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
  
  FrostStory::FrostStory(const AnalysisSources & theSources,
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
  
  bool FrostStory::hasStory(const string & theName)
  {
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
  
  Paragraph FrostStory::makeStory(const string & theName) const
  {

	throw TextGenError("FrostStory cannot make story "+theName);

  }

} // namespace TextGen
  
// ======================================================================
  
