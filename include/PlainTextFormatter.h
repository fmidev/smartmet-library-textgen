// ======================================================================
/*!
 * \file
 * \brief Interface of class PlainTextFormatter
 */
// ======================================================================
/*!
 * \class TextGen::PlainTextFormatter
 *
 * \brief Glyph visitor generating normal ASCII output
 */
// ======================================================================

#ifndef TEXTGEN_PLAINTEXTFORMATTER_H
#define TEXTGEN_PLAINTEXTFORMATTER_H

#include "TextFormatter.h"

namespace TextGen
{
  class PlainTextFormatter : public TextFormatter
  {
  public:
	virtual ~PlainTextFormatter() { }
	virtual void dictionary(const boost::shared_ptr<Dictionary> & theDict);
	virtual void variable(const std::string & theVariable);

	virtual std::string format(const Glyph & theGlyph) const;

	// override for all composites
	virtual std::string visit(const Glyph & theGlyph) const;
	virtual std::string visit(const Sentence & theSentence) const;
	virtual std::string visit(const Paragraph & theParagraph) const;
	virtual std::string visit(const Header & theHeader) const;
	virtual std::string visit(const Document & theDocument) const;

  private:

	boost::shared_ptr<Dictionary> itsDictionary;
	std::string itsVar;

  }; // class PlainTextFormatter
} // namespace TextGen

#endif // TEXTGEN_PLAINTEXTFORMATTER_H

// ======================================================================
