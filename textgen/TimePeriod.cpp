// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::TimePeriod
 */
// ======================================================================
/*!
 * \class TextGen::TimePeriod
 * \brief Representation of time period
 */
// ======================================================================

#include "TimePeriod.h"
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

TimePeriod::~TimePeriod() = default;
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 *
 * \param thePeriod The period
 */
// ----------------------------------------------------------------------

TimePeriod::TimePeriod(const WeatherPeriod& thePeriod) : itsPeriod(thePeriod) {}
// ----------------------------------------------------------------------
/*!
 * \brief Return a clone
 */
// ----------------------------------------------------------------------

std::shared_ptr<Glyph> TimePeriod::clone() const
{
  std::shared_ptr<Glyph> ret(new TimePeriod(*this));
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

std::string TimePeriod::realize(const Dictionary& /*theDictionary*/) const
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

std::string TimePeriod::realize(const TextFormatter& theFormatter) const
{
  return theFormatter.visit(*this);
}

// ----------------------------------------------------------------------
/*!
 * \brief Returns false since phrase is not a separator
 */
// ----------------------------------------------------------------------

bool TimePeriod::isDelimiter() const
{
  return false;
}
}  // namespace TextGen

// ======================================================================
