// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::RoadStory::condition_overview
 */
// ======================================================================

#include "RoadStory.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "Sentence.h"
#include "Settings.h"
#include "WeatherResult.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace boost;
using namespace std;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on road conditions
   *
   * \return The story
   *
   * \see page_roadcondition_overview
   */
  // ----------------------------------------------------------------------

  Paragraph RoadStory::condition_overview() const
  {
	MessageLogger log("RoadStory::condition_overview");

	Paragraph paragraph;
	log << paragraph;
	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================
