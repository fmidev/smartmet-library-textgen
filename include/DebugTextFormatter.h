// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::DebugTextFormatter
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

	virtual const std::string format(const Glyph & theGlyph) const;

	// override for all composites
	virtual const std::string visit(const Glyph & theGlyph) const;
	virtual const std::string visit(const Integer & theInteger) const;
	virtual const std::string visit(const IntegerRange & theRange) const;
	virtual const std::string visit(const Sentence & theSentence) const;
	virtual const std::string visit(const Paragraph & theParagraph) const;
	virtual const std::string visit(const Header & theHeader) const;
	virtual const std::string visit(const Document & theDocument) const;

	virtual const std::string visit(const SectionTag & theSectionTag) const;
	virtual const std::string visit(const StoryTag & theStoryTag) const;

  private:

	DebugDictionary itsDictionary;
	mutable std::string itsSectionVar;
	mutable std::string itsStoryVar;

  }; // class DebugTextFormatter
} // namespace TextGen

#endif // TEXTGEN_DEBUGTEXTFORMATTER_H

// ======================================================================
