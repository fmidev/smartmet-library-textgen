// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WeatherStory::overview
 */
// ======================================================================

#include "WeatherStory.h"
#include "MessageLogger.h"
#include "Paragraph.h"

using namespace WeatherAnalysis;
using namespace std;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate overview on weather
   *
   * \return The story
   *
   * \see page_weather_overview
   *
   * \todo Much missing
   */
  // ----------------------------------------------------------------------

  Paragraph WeatherStory::overview() const
  {
	MessageLogger log("WeatherStory::overview");

	Paragraph paragraph;

	log << paragraph;
	return paragraph;
  }


} // namespace TextGen

// ======================================================================
