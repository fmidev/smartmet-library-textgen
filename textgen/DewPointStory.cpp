// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::DewPointStory
 */
// ======================================================================
/*!
 * \class TextGen::DewPointStory
 *
 * \brief Generates stories on temperature
 *
 * \see page_tarinat
 */
// ======================================================================

#include "DewPointStory.h"
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

DewPointStory::~DewPointStory() = default;
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

DewPointStory::DewPointStory(const TextGenPosixTime& theForecastTime,
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

bool DewPointStory::hasStory(const string& theName)
{
  return theName == "dewpoint_range";
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

Paragraph DewPointStory::makeStory(const string& theName) const
{
  if (theName == "dewpoint_range")
    return range();

  throw Fmi::Exception(BCP, "DewPointStory cannot make story " + theName);
}

}  // namespace TextGen

// ======================================================================
