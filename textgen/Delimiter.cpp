// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::Delimiter
 */
// ======================================================================
/*!
 * \class TextGen::Delimiter
 * \brief Representation of a sentence delimiter, such as ","
 *
 */
// ======================================================================

#include "Delimiter.h"

#include "TextFormatter.h"
#include <macgyver/Exception.h>
#include <utility>

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 */
// ----------------------------------------------------------------------

Delimiter::~Delimiter() = default;
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 *
 * \param theSeparator The word
 */
// ----------------------------------------------------------------------

Delimiter::Delimiter(std::string theSeparator) : itsSeparator(std::move(theSeparator)) {}
// ----------------------------------------------------------------------
/*!
 * \brief Return a clone
 */
// ----------------------------------------------------------------------

std::shared_ptr<Glyph> Delimiter::clone() const
{
  try
  {
    std::shared_ptr<Glyph> ret(new Delimiter(*this));
    return ret;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Realize using the given dictionary
 *
 * Note that the dictionary is not needed in the implementation,
 * the returned string is always the one given in the constructor.
 *
 * \param theDictionary The dictionary to realize with
 * \return The realized string
 */
// ----------------------------------------------------------------------

std::string Delimiter::realize(const Dictionary& /*theDictionary*/) const
{
  try
  {
    return itsSeparator;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Realize using the given text formatter
 *
 * \param theFormatter The text formatter
 * \return The realized string
 */
// ----------------------------------------------------------------------

std::string Delimiter::realize(const TextFormatter& theFormatter) const
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
 * \brief Returns true
 */
// ----------------------------------------------------------------------

bool Delimiter::isDelimiter() const
{
  try
  {
    return true;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the delimiter as is without realization
 *
 * \return The delimiter string
 */
// ----------------------------------------------------------------------

const string& Delimiter::value() const
{
  try
  {
    return itsSeparator;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
}  // namespace TextGen

// ======================================================================
