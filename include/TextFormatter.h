// ======================================================================
/*!
 * \file
 * \brief Interface of class TextFormatter
 */
// ======================================================================
/*!
 * \class TextGen::TextFormatter
 *
 * \brief Abstract interface of a Glyph visitor.
 */
// ======================================================================

#ifndef TEXTGEN_TEXTFORMATTER_H
#define TEXTGEN_TEXTFORMATTER_H

#include "boost/shared_ptr.hpp"
#include <string>

namespace TextGen
{
  class Dictionary;
  class Glyph;
  class GlyphContainer;
  class Sentence;
  class Paragraph;
  class Header;
  class Document;

  class TextFormatter
  {
  public:
	virtual ~TextFormatter() { }
	TextFormatter() { }
	virtual void dictionary(const boost::shared_ptr<Dictionary> & theDict) = 0;

	virtual std::string format(const Glyph & theGlyph) const;

	// override for all composites
	virtual std::string visit(const Glyph & theGlyph) const = 0;
	virtual std::string visit(const Sentence & theSentence) const = 0;
	virtual std::string visit(const Paragraph & theParagraph) const = 0;
	virtual std::string visit(const Header & theHeader) const = 0;
	virtual std::string visit(const Document & theDocument) const = 0;

  }; // class TextFormatter
} // namespace TextGen

#endif // TEXTGEN_TEXTFORMATTER_H

// ======================================================================
