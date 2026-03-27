// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::IntegerRange
 */
// ======================================================================
/*!
 * \class TextGen::IntegerRange
 * \brief Representation of a generic IntegerRange
 */
// ======================================================================

#include "IntegerRange.h"
#include "Dictionary.h"
#include <macgyver/Exception.h>

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

IntegerRange::~IntegerRange() = default;
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 */
// ----------------------------------------------------------------------

IntegerRange::IntegerRange(int theStartValue, int theEndValue, std::string theSeparator)
    : itsRangeSeparator(std::move(theSeparator)),
      itsStartValue(theStartValue),
      itsEndValue(theEndValue)
{
}

// ----------------------------------------------------------------------
/*!
 * \brief Return a clone
 */
// ----------------------------------------------------------------------

std::shared_ptr<Glyph> IntegerRange::clone() const
{
  try
  {
    std::shared_ptr<Glyph> ret(new IntegerRange(*this));
    return ret;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the range
 *
 * \param theDictionary The dictionary to be used
 * \return The text
 */
// ----------------------------------------------------------------------

std::string IntegerRange::realize(const Dictionary& /*theDictionary*/) const
{
  try
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
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the number
 *
 * \param theFormatter The formatter
 * \return The text
 */
// ----------------------------------------------------------------------

std::string IntegerRange::realize(const TextFormatter& theFormatter) const
{
  try
  {
    return theFormatter.visit(*this);
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Returns false since IntegerRange is not a separator
 */
// ----------------------------------------------------------------------

bool IntegerRange::isDelimiter() const
{
  try
  {
    return false;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the range separator string
 *
 * \return The range separator string
 */
// ----------------------------------------------------------------------

const std::string& IntegerRange::rangeSeparator() const
{
  try
  {
    return itsRangeSeparator;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the start value of the range
 *
 * \return The start value
 */
// ----------------------------------------------------------------------

int IntegerRange::startValue() const
{
  try
  {
    return itsStartValue;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the end value of the range
 *
 * \return The end value
 */
// ----------------------------------------------------------------------

int IntegerRange::endValue() const
{
  try
  {
    return itsEndValue;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
}  // namespace TextGen

// ======================================================================
