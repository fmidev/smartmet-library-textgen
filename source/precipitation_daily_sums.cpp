// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::PrecipitationStory::daily_sums
 */
// ======================================================================

#include "PrecipitationStory.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "Integer.h"
#include "IntegerRange.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "RangeAcceptor.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "TimeTools.h"
#include "UnitFactory.h"
#include "WeatherResult.h"

#include "boost/lexical_cast.hpp"

#include <vector>

using namespace boost;
using namespace std;
using namespace WeatherAnalysis;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on daily total precipitation ranges
   *
   * \return The story
   */
  // ----------------------------------------------------------------------

  const Paragraph PrecipitationStory::daily_sums() const
  {
	MessageLogger log("PrecipitationStory::daily_sums");

	Paragraph paragraph;

	log << paragraph;
	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================
  
