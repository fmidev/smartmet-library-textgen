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
  virtual ~StoryTag();
  StoryTag(const std::string& theName, const bool& prefixTag = true);
#ifdef NO_COMPILER_GENERATED
  StoryTag(const StoryTag& theStoryTag);
  StoryTag& operator=(const StoryTag& theStoryTag);
#endif

  virtual boost::shared_ptr<Glyph> clone() const;
  virtual std::string realize(const Dictionary& theDictionary) const;
  virtual std::string realize(const TextFormatter& theFormatter) const;

  virtual bool isDelimiter() const;
  virtual bool isPrefixTag() const;

 private:
  StoryTag();
  std::string itsName;
  bool itsPrefixTag;

};  // class StoryTag
}  // namespace StoryTag

// ======================================================================
