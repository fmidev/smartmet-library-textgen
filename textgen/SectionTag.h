// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::SectionTag
 */
// ======================================================================

#pragma once

#include "Glyph.h"
#include <string>

namespace TextGen
{
class Dictionary;
class TextFormatter;

class SectionTag : public Glyph
{
 public:
  ~SectionTag() override;
  SectionTag(std::string theName, const bool& prefixTag = true);
#ifdef NO_COMPILER_GENERATED
  SectionTag(const SectionTag& theSectionTag);
  SectionTag& operator=(const SectionTag& theSectionTag);
#endif

  std::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;

  bool isDelimiter() const override;
  virtual bool isPrefixTag() const;

 private:
  SectionTag() = delete;
  std::string itsName;
  bool itsPrefixTag;

};  // class SectionTag
}  // namespace TextGen

// ======================================================================
