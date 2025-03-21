// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::Time
 */
// ======================================================================
/*!
 * \class TextGen::Time
 * \brief Representation of timestamp
 */
// ======================================================================

#include "WeatherTime.h"
#include "Dictionary.h"
#include "TextFormatter.h"
#include <calculator/WeatherPeriod.h>

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 */
// ----------------------------------------------------------------------

WeatherTime::~WeatherTime() = default;
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 *
 * \param thePeriod The period
 */
// ----------------------------------------------------------------------

WeatherTime::WeatherTime(const TextGenPosixTime& theTime) : itsTime(theTime) {}
// ----------------------------------------------------------------------
/*!
 * \brief Return a clone
 */
// ----------------------------------------------------------------------

std::shared_ptr<Glyph> WeatherTime::clone() const
{
  std::shared_ptr<Glyph> ret(new WeatherTime(*this));
  return ret;
}

// ----------------------------------------------------------------------
/*!
 * \brief Realize using the given dictionary
 *
 * \param theDictionary The dictionary to realize with
 * \return The realized string
 */
// ----------------------------------------------------------------------

std::string WeatherTime::realize(const Dictionary& /*theDictionary*/) const
{
  return "";
}
// ----------------------------------------------------------------------
/*!
 * \brief Realize using the given text formatter
 *
 * \param theFormatter The formatter
 * \return The formatter string
 */
// ----------------------------------------------------------------------

std::string WeatherTime::realize(const TextFormatter& theFormatter) const
{
  return theFormatter.visit(*this);
}

// ----------------------------------------------------------------------
/*!
 * \brief Returns false since phrase is not a separator
 */
// ----------------------------------------------------------------------

bool WeatherTime::isDelimiter() const
{
  return false;
}
}  // namespace TextGen

// ======================================================================
