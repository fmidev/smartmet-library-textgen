// ======================================================================
/*!
 * \file
 * \brief Implementation of class PrecipitationStory
 */
// ======================================================================

#include "PrecipitationStory.h"
#include "Paragraph.h"
#include "Sentence.h"
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
   * \param theSources The analysis sources
   * \param theArea The area to be analyzed
   * \param thePeriod The time interval to be analyzed
   */
  // ----------------------------------------------------------------------
  
  PrecipitationStory::PrecipitationStory(const AnalysisSources & theSources,
										 const WeatherArea & theArea,
										 const WeatherPeriod & thePeriod)
	: itsSources(theSources)
	, itsArea(theArea)
	, itsPeriod(thePeriod)
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

	throw TextGenError("PrecipitationStory cannot make story "+theName);

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on total precipitation
   *
   * \return The story
   */
  // ----------------------------------------------------------------------

  Paragraph PrecipitationStory::total() const
  {
	Paragraph paragraph;
	Sentence sentence;
	sentence << "sadesumma"
			 << 10
			 << "millimetriä";
	paragraph << sentence;
	return paragraph;
  }



} // namespace TextGen
  
// ======================================================================
  
