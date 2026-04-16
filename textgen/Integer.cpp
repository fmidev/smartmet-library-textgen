// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::Integer
 */
// ======================================================================
/*!
 * \class TextGen::Integer
 * \brief Representation of a generic integer
 */
// ======================================================================

#include "Integer.h"
#include "Dictionary.h"
#include "TextFormatter.h"
#include <macgyver/Exception.h>

#include <memory>

#include <sstream>

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 */
// ----------------------------------------------------------------------

Integer::~Integer() = default;
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 */
// ----------------------------------------------------------------------

Integer::Integer(int theInteger) : itsInteger(theInteger) {}
// ----------------------------------------------------------------------
/*!
 * \brief Return a clone
 */
// ----------------------------------------------------------------------

std::shared_ptr<Glyph> Integer::clone() const
{
  try
  {
    std::shared_ptr<Glyph> ret(new Integer(*this));
    return ret;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the Integer
 *
 * \param theDictionary The dictionary to be used
 * \return The text
 */
// ----------------------------------------------------------------------

std::string Integer::realize(const Dictionary& /*theDictionary*/) const
{
  try
  {
    ostringstream os;
    os << itsInteger;
    return os.str();
  }
  catch (...)
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

std::string Integer::realize(const TextFormatter& theFormatter) const
{
  try
  {
    return theFormatter.visit(*this);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Returns false since Integer is not a separator
 */
// ----------------------------------------------------------------------

bool Integer::isDelimiter() const
{
  try
  {
    return false;
  }
  catch (...)
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

int Integer::value() const
{
  try
  {
    return itsInteger;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
}  // namespace TextGen

// ======================================================================
