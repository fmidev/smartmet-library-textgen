// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Sentence
 */
// ======================================================================

#pragma once

#include "GlyphContainer.h"
#include <boost/shared_ptr.hpp>
#include <string>

namespace TextGen
{
class Sentence : public GlyphContainer
{
 public:
  ~Sentence() override = default;
#ifdef NO_COMPILER_GENERATED
  Sentence();
  Sentence(const Sentence& theSentence);
  Sentence& operator=(const Sentence& theSentence);
#endif

  boost::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;
  bool isDelimiter() const override;

  Sentence& operator<<(const Sentence& theSentence);
  Sentence& operator<<(const Glyph& theGlyph);
  Sentence& operator<<(const std::string& thePhrase);
  Sentence& operator<<(int theNumber);

};  // class Sentence

}  // namespace TextGen

// ======================================================================
