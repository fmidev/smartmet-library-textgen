// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Document
 */
// ======================================================================
/*!
 * \class TextGen::Document
 *
 * \brief A sequence of glyphs forming a document
 *
 */
// ======================================================================

#ifndef TEXTGEN_DOCUMENT_H
#define TEXTGEN_DOCUMENT_H

#include "GlyphContainer.h"
#include "boost/shared_ptr.hpp"
#include <string>

namespace TextGen
{
  class Document : public GlyphContainer
  {
  public:
	~Document();
	Document();
#ifdef NO_COMPILER_GENERATED
	Document(const Document & theDocument);
	Document & operator=(const Document & theDocument);
#endif

	virtual boost::shared_ptr<Glyph> clone() const;
	virtual std::string realize(const Dictionary & theDictionary) const;
	virtual std::string realize(const TextFormatter & theFormatter) const;
	virtual std::string prefix() const;
	virtual std::string suffix() const;

	Document & operator<<(const Document & theDocument);
	Document & operator<<(const Glyph & theGlyph);

  }; // class Document

} // namespace TextGen

#endif // TEXTGEN_DOCUMENT_H

// ======================================================================