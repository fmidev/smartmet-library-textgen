// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::SoneraTextFormatter
 */
// ======================================================================

#ifndef TEXTGEN_SONERATEXTFORMATTER_H
#define TEXTGEN_SONERATEXTFORMATTER_H

#include "TextFormatter.h"

#include <list>

namespace TextGen
{
  class SoneraTextFormatter : public TextFormatter
  {
  public:
	SoneraTextFormatter();

	virtual ~SoneraTextFormatter() { }
	virtual void dictionary(const boost::shared_ptr<Dictionary> & theDict);

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

	typedef std::list<std::string> container_type;
	mutable container_type itsParts;
	mutable int itsDepth;

	boost::shared_ptr<Dictionary> itsDictionary;
	mutable std::string itsSectionVar;
	mutable std::string itsStoryVar;

  }; // class SoneraTextFormatter
} // namespace TextGen

#endif // TEXTGEN_SONERATEXTFORMATTER_H

// ======================================================================
