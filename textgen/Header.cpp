// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::Header
 */
// ======================================================================
/*!
 * \class TextGen::Header
 *
 * \brief A sequence of glyphs forming a header
 *
 */
// ======================================================================

#include "Header.h"

#include "Dictionary.h"
#include "Integer.h"
#include "Phrase.h"
#include "PlainTextFormatter.h"
#include <macgyver/Exception.h>

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Return a clone
 */
// ----------------------------------------------------------------------

std::shared_ptr<Glyph> Header::clone() const
{
  try
  {
    std::shared_ptr<Glyph> ret(new Header(*this));
    return ret;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the header
 *
 * \param theDictionary The dictionary to be used
 * \return The text
 */
// ----------------------------------------------------------------------

std::string Header::realize(const Dictionary& /*theDictionary*/) const
{
  try
  {
    throw Fmi::Exception(BCP, "Header::realize(Dictionary) should not be called");
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the header
 *
 * \param theFormatter The formatter
 * \return The text
 */
// ----------------------------------------------------------------------

std::string Header::realize(const TextFormatter& theFormatter) const
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
 * \brief Returns false since header is not a separator
 */
// ----------------------------------------------------------------------

bool Header::isDelimiter() const
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
 * \brief Adding a glyph to a header
 *
 * \param theGlyph The glyph to be added
 * \result The header added to
 */
// ----------------------------------------------------------------------

Header& Header::operator<<(const Glyph& theGlyph)
{
  try
  {
    itsData.push_back(theGlyph.clone());
    return *this;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Add a phrase to a header with automatic conversion
 *
 * \param thePhrase The string initializer for the phrase
 * \return The header added to
 */
// ----------------------------------------------------------------------

Header& Header::operator<<(const string& thePhrase)
{
  try
  {
    if (!thePhrase.empty())
    {
      std::shared_ptr<Phrase> phrase(new Phrase(thePhrase));
      itsData.push_back(phrase);
    }
    return *this;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("thePhrase", thePhrase);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Add a number to a header with automatic conversion
 *
 * \param theNumber The integer initializer for the number
 * \return The header added to
 */
// ----------------------------------------------------------------------

Header& Header::operator<<(int theNumber)
{
  try
  {
    std::shared_ptr<Integer> number(new Integer(theNumber));
    itsData.push_back(number);
    return *this;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("theNumber", std::to_string(theNumber));
  }
}

}  // namespace TextGen

// ======================================================================
