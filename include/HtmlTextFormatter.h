// ======================================================================
/*!
 * \file
 * \brief Interface of class HtmlTextFormatter
 */
// ======================================================================
/*!
 * \class TextGen::HtmlTextFormatter
 *
 * \brief Glyph visitor generating HTML output
 */
// ======================================================================

#ifndef TEXTGEN_HTMLTEXTFORMATTER_H
#define TEXTGEN_HTMLTEXTFORMATTER_H

#include "TextFormatter.h"

namespace TextGen
{
  class HtmlTextFormatter : public TextFormatter
  {
  public:
	virtual ~HtmlTextFormatter() { }
	virtual void dictionary(const boost::shared_ptr<Dictionary> & theDict);

	virtual std::string format(const Glyph & theGlyph) const;

	// override for all composites
	virtual std::string visit(const Glyph & theGlyph) const;
	virtual std::string visit(const Sentence & theSentence) const;
	virtual std::string visit(const Paragraph & theParagraph) const;
	virtual std::string visit(const Header & theHeader) const;
	virtual std::string visit(const Document & theDocument) const;

  private:

	boost::shared_ptr<Dictionary> itsDictionary;

  }; // class HtmlTextFormatter
} // namespace TextGen

#endif // TEXTGEN_HTMLTEXTFORMATTER_H

// ======================================================================
