// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WindStory::overview
 */
// ======================================================================

#include "WindStory.h"
#include "MessageLogger.h"
#include "Paragraph.h"

using namespace WeatherAnalysis;
using namespace std;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Return wind overview story
   *
   * \return Paragraph containing the story
   */
  // ----------------------------------------------------------------------

  Paragraph WindStory::overview() const
  {
	MessageLogger log("WeatherStory::overview");

	Paragraph paragraph;
	log << paragraph;
	return paragraph;
  }

} // namespace TextGen
  
// ======================================================================
