// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::SpecialStory
 */
// ======================================================================
/*!
 * \class TextGen::SpecialStory
 *
 * \brief Generates stories with special content
 *
 * \see docs/programmers/stories.md
 */
// ======================================================================

#include "SpecialStory.h"
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

SpecialStory::~SpecialStory() = default;
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

SpecialStory::SpecialStory(const TextGenPosixTime& theForecastTime,
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

bool SpecialStory::hasStory(const string& theName)
{
  try
  {
    if (theName == "none")
      return true;
    if (theName.substr(0, 4) == "text")  // Allows more flexible definitions
      return true;
    if (theName == "date")
      return true;
#if 0
	if(theName == "table")
	  return true;
#endif
    return false;
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

Paragraph SpecialStory::makeStory(const string& theName) const
{
  try
  {
    if (theName == "none")
      return none();
    if (theName.substr(0, 4) == "text")  // Allows more flexible definitions
      return text();
    if (theName == "date")
      return date();
#if 0
	if(theName == "table")
	  return table();
#endif

    throw Fmi::Exception(BCP, "SpecialStory cannot make story " + theName);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("theName", theName);
  }
}

}  // namespace TextGen

// ======================================================================
