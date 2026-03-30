// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::Document
 */
// ======================================================================
/*!
 * \class TextGen::Document
 *
 * \brief A sequence of glyphs forming a document
 *
 */
// ======================================================================

#include "Document.h"

#include "Dictionary.h"
#include "PlainTextFormatter.h"
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

std::shared_ptr<Glyph> Document::clone() const
{
  try
  {
    std::shared_ptr<Glyph> ret(new Document(*this));
    return ret;
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the document
 *
 * \param theDictionary The dictionary to be used
 * \return The text
 */
// ----------------------------------------------------------------------

std::string Document::realize(const Dictionary& /*theDictionary*/) const
{
  try
  {
    throw Fmi::Exception(BCP, "Document::realize(Dictionary) should not be called");
  }
  catch(...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the text for the document
 *
 * \param theFormatter The formatter
 * \return The text
 */
// ----------------------------------------------------------------------

std::string Document::realize(const TextFormatter& theFormatter) const
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
 * \brief Returns false since document is not a separator
 */
// ----------------------------------------------------------------------

bool Document::isDelimiter() const
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
 * \brief Add a document to the end of this document
 *
 * \param theDocument The document to be added
 * \result The document added to
 */
// ----------------------------------------------------------------------

Document& Document::operator<<(const Document& theDocument)
{
  try
  {
    if (this != &theDocument)
    {
      copy(theDocument.itsData.begin(), theDocument.itsData.end(), back_inserter(itsData));
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
 * \brief Add a glyph to a document
 *
 * \param theGlyph The glyph to be added
 * \result The document added to
 */
// ----------------------------------------------------------------------

Document& Document::operator<<(const Glyph& theGlyph)
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
