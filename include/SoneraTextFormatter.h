// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::SoneraTextFormatter
 */
// ======================================================================
/*!
 * \class TextGen::SoneraTextFormatter
 *
 * \brief Glyph visitor generating Sonera tailored output
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
	virtual void variable(const std::string & theVariable);

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

	typedef std::list<std::string> container_type;
	mutable container_type itsParts;
	mutable int itsDepth;

	boost::shared_ptr<Dictionary> itsDictionary;
	std::string itsVar;

  }; // class SoneraTextFormatter
} // namespace TextGen

#endif // TEXTGEN_SONERATEXTFORMATTER_H

// ======================================================================
