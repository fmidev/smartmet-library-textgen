// ======================================================================
/*!
 * \file
 * \brief ..
 */
 
// ======================================================================
/*!
 * \class TextGen::AK_FrostStory
 *
 * \brief Generates stories on frost
 *
 * \see page_tarinat
 */
// ======================================================================

#include "ak_FrostStory.h"

#include "AnalysisSources.h"
#include "Paragraph.h"
#include "TextGenError.h"

using namespace std;
//using namespace WeatherAnalysis;

namespace TextGen
{
  
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
  
  AK_FrostStory::AK_FrostStory( const NFmiTime & theForecastTime,
						 const WeatherAnalysis::AnalysisSources & theSources,
						 const WeatherAnalysis::WeatherArea & theArea,
						 const WeatherAnalysis::WeatherPeriod & thePeriod,
						 const string & theVariable )
	: itsForecastTime(theForecastTime)
	, itsSources(theSources)
	, itsArea(theArea)
	, itsPeriod(thePeriod)
	, itsVar(theVariable) {}
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Test whether the given story is known to this class
   *
   * \param theName The story name
   * \return True if this class can generate the story
   */
  // ----------------------------------------------------------------------
  
  bool AK_FrostStory::hasStory(const string & theName)
  {
    const string known[]= { 
        "frost_text_overview",
        "frost_numeric_overview"
        // ...
     };

    for( unsigned i=0; i<sizeof(known)/sizeof(known[0]); i++ ) {
        if (theName == known[i]) {
            return true;
        }
    }
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
  
  const Paragraph AK_FrostStory::makeStory(const string & theName) const
  {
	if (theName == "frost_text_overview")      return text_overview();
	if (theName == "frost_numeric_overview")   return numeric_overview();
        //...

	throw TextGenError("AK_FrostStory cannot make story "+theName);
  }


} // namespace TextGen
  
// ======================================================================
  
