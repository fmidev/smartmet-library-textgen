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
	if(theName == "frost_mean")
	  return true;
	if(theName == "frost_maximum")
	  return true;
	if(theName == "frost_maxtwonights")
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
  
  Paragraph FrostStory::makeStory(const string & theName) const
  {
	if(theName == "frost_mean")
	  return mean();
	if(theName == "frost_maximum")
	  return maximum();
	if(theName == "frost_maxtwonights")
	  return maxtwonights();

	throw TextGenError("FrostStory cannot make story "+theName);

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on mean frost
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  Paragraph FrostStory::mean() const
  {
	Paragraph paragraph;
	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on maximum frost
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  Paragraph FrostStory::maximum() const
  {
	Paragraph paragraph;
	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on 1/2 night frost
   *
   * \return The generated paragraph
   */
  // ----------------------------------------------------------------------
  
  Paragraph FrostStory::maxtwonights() const
  {
	Paragraph paragraph;
	return paragraph;
  }


} // namespace TextGen
  
// ======================================================================
  
