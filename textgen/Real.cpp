// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::Real
 */
// ======================================================================
/*!
 * \class TextGen::Real
 * \brief Representation of a generic floating point number
 */
// ======================================================================

#include "Real.h"
#include "Dictionary.h"
#include "TextFormatter.h"
#include <macgyver/Exception.h>

#include <newbase/NFmiStringTools.h>

#include <memory>

#include <iomanip>
#include <sstream>

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 */
// ----------------------------------------------------------------------

Real::~Real() = default;
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 */
// ----------------------------------------------------------------------

Real::Real(float theReal, int thePrecision, bool theComma)
    : itsReal(theReal), itsPrecision(thePrecision), itsComma(theComma)
{
}

// ----------------------------------------------------------------------
/*!
 * \brief Return a clone
 */
// ----------------------------------------------------------------------

std::shared_ptr<Glyph> Real::clone() const
{
  try
  {
    std::shared_ptr<Glyph> ret(new Real(*this));
    return ret;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the Real
 *
 * \param theDictionary The dictionary to be used
 * \return The text
 */
// ----------------------------------------------------------------------

std::string Real::realize(const Dictionary& /*theDictionary*/) const
{
  try
  {
    ostringstream os;
    os << fixed << setprecision(itsPrecision) << itsReal;
    string result = os.str();
    if (!itsComma)
      NFmiStringTools::ReplaceChars(result, '.', ',');
    return result;
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

std::string Real::realize(const TextFormatter& theFormatter) const
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
 * \brief Returns false since Real is not a separator
 */
// ----------------------------------------------------------------------

bool Real::isDelimiter() const
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
 * \brief Return the value of the integer
 *
 * \return The integer
 */
// ----------------------------------------------------------------------

float Real::value() const
{
  try
  {
    return itsReal;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the precision of the real
 *
 * \return The precision
 */
// ----------------------------------------------------------------------

int Real::precision() const
{
  try
  {
    return itsPrecision;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Return true if comma is in use
 *
 * \return True, if comma is in use
 */
// ----------------------------------------------------------------------

bool Real::comma() const
{
  try
  {
    return itsComma;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
}  // namespace TextGen

// ======================================================================
