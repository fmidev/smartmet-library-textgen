// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::DebugTextFormatter
 */
// ======================================================================
/*!
 * \class TextGen::DebugTextFormatter
 *
 * \brief Glyph visitor generating output for debug logs
 */
// ======================================================================

#ifndef TEXTGEN_DEBUGTEXTFORMATTER_H
#define TEXTGEN_DEBUGTEXTFORMATTER_H

#include "TextFormatter.h"
#include "DebugDictionary.h"

namespace TextGen
{
  class DebugTextFormatter : public TextFormatter
  {
  public:
	DebugTextFormatter() { }

	virtual ~DebugTextFormatter() { }
	virtual void dictionary(const boost::shared_ptr<Dictionary> & theDict) { }
	virtual void variable(const std::string & theVariable) { }

	virtual std::string format(const Glyph & theGlyph) const;

	// override for all composites
	virtual std::string visit(const Glyph & theGlyph) const;
	virtual std::string visit(const Integer & theInteger) const;
	virtual std::string visit(const IntegerRange & theRange) const;
	virtual std::string visit(const Sentence & theSentence) const;
	virtual std::string visit(const Paragraph & theParagraph) const;
	virtual std::string visit(const Header & theHeader) const;
	virtual std::string visit(const Document & theDocument) const;

  private:

	DebugDictionary itsDictionary;

  }; // class DebugTextFormatter
} // namespace TextGen

#endif // TEXTGEN_DEBUGTEXTFORMATTER_H

// ======================================================================
