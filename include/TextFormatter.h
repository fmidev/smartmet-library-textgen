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
  class Document;
  class Glyph;
  class GlyphContainer;
  class Header;
  class Paragraph;
  class Sentence;

  class TextFormatter
  {
  public:
	virtual ~TextFormatter() { }
#ifdef NO_COMPILER_GENERATED
	TextFormatter() { }
#endif

	virtual void dictionary(const boost::shared_ptr<Dictionary> & theDict) = 0;
	virtual void variable(const std::string & theVariable) = 0;

	virtual std::string format(const Glyph & theGlyph) const = 0;

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
