// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::PressureStory
 */
// ======================================================================
/*!
 * \class TextGen::PressureStory
 *
 * \brief Generates stories on pressure
 *
 * \see page_tarinat
 */
// ======================================================================

#include "PressureStory.h"
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

PressureStory::~PressureStory() = default;
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

PressureStory::PressureStory(const TextGenPosixTime& theForecastTime,
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

bool PressureStory::hasStory(const string& theName)
{
  return theName == "pressure_mean";
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

Paragraph PressureStory::makeStory(const string& theName) const
{
  if (theName == "pressure_mean")
    return mean();

  throw Fmi::Exception(BCP, "PressureStory cannot make story " + theName);
}

}  // namespace TextGen

// ======================================================================
