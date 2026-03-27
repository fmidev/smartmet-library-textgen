// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::TimePhrase
 */
// ======================================================================
/*!
 * \class TextGen::TimePhrase
 *
 * \brief A sequence describing a time as in the time a forecast was made
 *
 */
// ======================================================================

#include "TimePhrase.h"

#include "Dictionary.h"
#include "Integer.h"
#include "Phrase.h"
#include "PlainTextFormatter.h"
#include "TextFormatter.h"
#include <macgyver/Exception.h>
#include <algorithm>
#include <iterator>

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Return a clone
 */
// ----------------------------------------------------------------------

std::shared_ptr<Glyph> TimePhrase::clone() const
{
  try
  {
    std::shared_ptr<Glyph> ret(new TimePhrase(*this));
    return ret;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the sentence
 *
 * \param theDictionary The dictionary to be used
 * \return The text
 */
// ----------------------------------------------------------------------

std::string TimePhrase::realize(const Dictionary& /*theDictionary*/) const
{
  try
  {
    throw Fmi::Exception(BCP, "TimePhrase::realize(Dictionary) should not be called");
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the sentence
 *
 * \param theFormatter The formatter
 * \return The text
 */
// ----------------------------------------------------------------------

std::string TimePhrase::realize(const TextFormatter& theFormatter) const
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
 * \brief Returns false since sentence is not a separator
 */
// ----------------------------------------------------------------------

bool TimePhrase::isDelimiter() const
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
 * \brief Add a glyph to a sentence
 *
 * \param theGlyph The glyph to be added
 * \result The sentence added to
 */
// ----------------------------------------------------------------------

TimePhrase& TimePhrase::operator<<(const Glyph& theGlyph)
{
  try
  {
    itsData.push_back(theGlyph.clone());
    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Add a phrase to a sentence with automatic conversion
 *
 * \param thePhrase The string initializer for the phrase
 * \return The sentence added to
 */
// ----------------------------------------------------------------------

TimePhrase& TimePhrase::operator<<(const string& thePhrase)
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
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Add a number to a sentence with automatic conversion
 *
 * \param theNumber The integer initializer for the number
 * \return The sentence added to
 */
// ----------------------------------------------------------------------

TimePhrase& TimePhrase::operator<<(int theNumber)
{
  try
  {
    *this << Integer(theNumber);
    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

}  // namespace TextGen

// ======================================================================
