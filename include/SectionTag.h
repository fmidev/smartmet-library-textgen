// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::SectionTag
 */
// ======================================================================

#ifndef TEXTGEN_SECTIONTAG_H
#define TEXTGEN_SECTIONTAG_H

#include "Glyph.h"
#include <string>

namespace TextGen
{
  class Dictionary;
  class TextFormatter;

  class SectionTag : public Glyph
  {
  public:

	virtual ~SectionTag();
	SectionTag(const std::string & theName);
#ifdef NO_COMPILER_GENERATED
	SectionTag(const SectionTag & theSectionTag);
	SectionTag & operator=(const SectionTag & theSectionTag);
#endif

	virtual boost::shared_ptr<Glyph> clone() const;
	virtual const std::string realize(const Dictionary & theDictionary) const;
	virtual const std::string realize(const TextFormatter & theFormatter) const;

	virtual bool isDelimiter() const;

  private:

	SectionTag();
	std::string itsName;

  }; // class SectionTag
} // namespace SectionTag

#endif // TEXTGEN_SECTIONTAG_H

// ======================================================================
