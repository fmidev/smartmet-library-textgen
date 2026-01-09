// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::PositiveRange
 */
// ======================================================================
/*!
 * \class TextGen::PositiveRange
 * \brief Representation of a generic PositiveRange
 */
// ======================================================================

#include "PositiveRange.h"
#include "Dictionary.h"
#include <macgyver/Exception.h>
#include <macgyver/StringConversion.h>

#include <memory>
#include <sstream>
#include <utility>

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 */
// ----------------------------------------------------------------------

PositiveRange::~PositiveRange() = default;
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 */
// ----------------------------------------------------------------------

PositiveRange::PositiveRange(int theStartValue, int theEndValue, std::string theSeparator)
    : itsRangeSeparator(std::move(theSeparator)),
      itsStartValue(theStartValue),
      itsEndValue(theEndValue)
{
  if (itsStartValue < 0 || itsEndValue < 0)
    throw Fmi::Exception(BCP, "Positive value range cannot have negative values")
        .addParameter("startvalue", Fmi::to_string(theStartValue))
        .addParameter("endvalue", Fmi::to_string(theEndValue));
}

// ----------------------------------------------------------------------
/*!
 * \brief Return a clone
 */
// ----------------------------------------------------------------------

std::shared_ptr<Glyph> PositiveRange::clone() const
{
  std::shared_ptr<Glyph> ret(new PositiveRange(*this));
  return ret;
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the range
 *
 * \param theDictionary The dictionary to be used
 * \return The text
 */
// ----------------------------------------------------------------------

std::string PositiveRange::realize(const Dictionary& /*theDictionary*/) const
{
  std::ostringstream os;
  if (itsStartValue == itsEndValue)
    os << itsStartValue;
  else
  {
    if (itsStartValue < 0 && itsEndValue > 0)
      os << itsStartValue << itsRangeSeparator << "+" << itsEndValue;
    else if (itsStartValue > 0 && itsEndValue < 0)
      os << "+" << itsStartValue << itsRangeSeparator << itsEndValue;
    else
      os << itsStartValue << itsRangeSeparator << itsEndValue;
  }

  return os.str();
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the number
 *
 * \param theFormatter The formatter
 * \return The text
 */
// ----------------------------------------------------------------------

std::string PositiveRange::realize(const TextFormatter& theFormatter) const
{
  return theFormatter.visit(*this);
}

// ----------------------------------------------------------------------
/*!
 * \brief Returns false since PositiveRange is not a separator
 */
// ----------------------------------------------------------------------

bool PositiveRange::isDelimiter() const
{
  return false;
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the range separator string
 *
 * \return The range separator string
 */
// ----------------------------------------------------------------------

const std::string& PositiveRange::rangeSeparator() const
{
  return itsRangeSeparator;
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the start value of the range
 *
 * \return The start value
 */
// ----------------------------------------------------------------------

int PositiveRange::startValue() const
{
  return itsStartValue;
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the end value of the range
 *
 * \return The end value
 */
// ----------------------------------------------------------------------

int PositiveRange::endValue() const
{
  return itsEndValue;
}
}  // namespace TextGen

// ======================================================================
