// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::HtmlTextFormatter
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

	virtual const std::string format(const Glyph & theGlyph) const;

	// override for all composites
	virtual const std::string visit(const Glyph & theGlyph) const;
	virtual const std::string visit(const Integer & theInteger) const;
	virtual const std::string visit(const Real & theReal) const;
	virtual const std::string visit(const IntegerRange & theRange) const;
	virtual const std::string visit(const Sentence & theSentence) const;
	virtual const std::string visit(const Paragraph & theParagraph) const;
	virtual const std::string visit(const Header & theHeader) const;
	virtual const std::string visit(const Document & theDocument) const;

	virtual const std::string visit(const SectionTag & theSectionTag) const;
	virtual const std::string visit(const StoryTag & theStoryTag) const;

  private:

	boost::shared_ptr<Dictionary> itsDictionary;
	mutable std::string itsSectionVar;
	mutable std::string itsStoryVar;

  }; // class HtmlTextFormatter
} // namespace TextGen

#endif // TEXTGEN_HTMLTEXTFORMATTER_H

// ======================================================================
