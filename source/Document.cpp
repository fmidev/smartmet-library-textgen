// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::Document
 */
// ======================================================================

#include "Document.h"

#include "Dictionary.h"
#include "PlainTextFormatter.h"
#include "TextGenError.h"

using namespace std;
using namespace boost;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Return a clone
   */
  // ----------------------------------------------------------------------

  shared_ptr<Glyph> Document::clone() const
  {
	shared_ptr<Glyph> ret(new Document(*this));
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the text for the document
   *
   * \param theDictionary The dictionary to be used
   * \return The text
   */
  // ----------------------------------------------------------------------

  std::string Document::realize(const Dictionary & theDictionary) const
  {
	throw TextGenError("Document::realize(Dictionary) should not be called");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the prefix for documents
   *
   * The prefix for documents is always the space character
   *
   * \return The space character
   */
  // ----------------------------------------------------------------------
  
  std::string Document::prefix() const
  {
	return " ";
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the suffix for documents
   *
   * The suffix for documents is always an empty string
   *
   * \return An empty string
   */
  // ----------------------------------------------------------------------
  
  std::string Document::suffix() const
  {
	return "";
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Add a document to the end of this document
   *
   * \param theDocument The document to be added
   * \result The document added to
   */
  // ----------------------------------------------------------------------

  Document & Document::operator<<(const Document & theDocument)
  {
	if(this != &theDocument)
	  {
		copy(theDocument.itsData.begin(),
			 theDocument.itsData.end(),
			 back_inserter(itsData));
	  }
	else
	  {
		storage_type tmp(itsData);
		copy(tmp.begin(),tmp.end(), back_inserter(itsData));
	  }
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Add a glyph to a document
   *
   * \param theGlyph The glyph to be added
   * \result The document added to
   */
  // ----------------------------------------------------------------------

  Document & Document::operator<<(const Glyph & theGlyph)
  {
	itsData.push_back(theGlyph.clone());
	return *this;
  }

} // namespace TextGen

// ======================================================================
