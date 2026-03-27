// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::Paragraph
 */
// ======================================================================
/*!
 * \class TextGen::Paragraph
 *
 * \brief A sequence of sentences.
 *
 * The responsibility of the Paragraph class is to contain
 * a list of sentences, which can then be converted into text.
 *
 */
// ======================================================================

#include "Paragraph.h"

#include "Dictionary.h"
#include "PlainTextFormatter.h"
#include "TextFormatter.h"
#include <macgyver/Exception.h>

#include <iterator>

using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Return a clone
 */
// ----------------------------------------------------------------------

std::shared_ptr<Glyph> Paragraph::clone() const
{
  try
  {
    std::shared_ptr<Glyph> ret(new Paragraph(*this));
    return ret;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the paragraph
 *
 * \param theDictionary The dictionary to be used
 * \return The text
 */
// ----------------------------------------------------------------------

std::string Paragraph::realize(const Dictionary& /*theDictionary*/) const
{
  try
  {
    throw Fmi::Exception(BCP, "Paragraph::realize(Dictionary) should not be called");
  }
  catch(...)
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

std::string Paragraph::realize(const TextFormatter& theFormatter) const
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
 * \brief Returns false since paragraph is not a separator
 */
// ----------------------------------------------------------------------

bool Paragraph::isDelimiter() const
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
 * \brief Add a paragraph to the end of this paragraph
 *
 * \param theParagraph The paragraph to be added
 * \result The paragraph added to
 */
// ----------------------------------------------------------------------

Paragraph& Paragraph::operator<<(const Paragraph& theParagraph)
{
  try
  {
    if (this != &theParagraph)
    {
      copy(theParagraph.itsData.begin(), theParagraph.itsData.end(), back_inserter(itsData));
    }
    else
    {
      storage_type tmp(itsData);
      copy(tmp.begin(), tmp.end(), back_inserter(itsData));
    }
    return *this;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Adding a glyph to a paragraph
 *
 * \param theGlyph The glyph to be added
 * \result The paragraph added to
 */
// ----------------------------------------------------------------------

Paragraph& Paragraph::operator<<(const Glyph& theGlyph)
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

}  // namespace TextGen

// ======================================================================
