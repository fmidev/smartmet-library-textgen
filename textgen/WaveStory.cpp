// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::WaveStory
 */
// ======================================================================
/*!
 * \class TextGen::WaveStory
 *
 * \brief Generates stories on wave speed and direction
 *
 * \see docs/programmers/stories.md
 */
// ======================================================================

#include "WaveStory.h"
#include "Paragraph.h"
#include <macgyver/Exception.h>

#include <utility>

using namespace TextGen;
using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 */
// ----------------------------------------------------------------------

WaveStory::~WaveStory() = default;
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

WaveStory::WaveStory(const TextGenPosixTime& theForecastTime,
                     const AnalysisSources& theSources,
                     const WeatherArea& theArea,
                     const WeatherPeriod& thePeriod,
                     string theVariable)
    : itsForecastTime(theForecastTime),
      itsSources(theSources),
      itsArea(theArea),
      itsPeriod(thePeriod),
      itsVar(std::move(theVariable))
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

bool WaveStory::hasStory(const string& theName)
{
  try
  {
    return theName == "wave_range";
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("theName", theName);
  }
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

Paragraph WaveStory::makeStory(const string& theName) const
{
  try
  {
    if (theName == "wave_range")
      return range();

    throw Fmi::Exception(BCP, "WaveStory cannot make story " + theName);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("theName", theName);
  }
}

}  // namespace TextGen

// ======================================================================
