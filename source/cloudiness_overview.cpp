// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::CloudinessStory::overview
 */
// ======================================================================

#include "CloudinessStory.h"
#include "MessageLogger.h"
#include "Paragraph.h"

using namespace boost;
using namespace std;
using namespace WeatherAnalysis;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on cloudiness
   *
   * \return The story
   *
   */
  // ----------------------------------------------------------------------

  Paragraph CloudinessStory::overview() const
  {
	MessageLogger log("CloudinessStory::overview");

	Paragraph paragraph;

	log << paragraph;
	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================
  
