// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::StoryTag
 */
// ======================================================================

#pragma once

#include "Glyph.h"
#include <string>

namespace TextGen
{
class Dictionary;
class TextFormatter;

class StoryTag : public Glyph
{
 public:
  ~StoryTag() override;
  StoryTag(std::string theName, const bool& prefixTag = true);
#ifdef NO_COMPILER_GENERATED
  StoryTag(const StoryTag& theStoryTag);
  StoryTag& operator=(const StoryTag& theStoryTag);
#endif

  std::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;

  bool isDelimiter() const override;
  virtual bool isPrefixTag() const;

 private:
  StoryTag() = delete;
  std::string itsName;
  bool itsPrefixTag;

};  // class StoryTag
}  // namespace TextGen

// ======================================================================
