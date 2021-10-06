// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Phrase
 */
// ======================================================================

#pragma once

#include "Glyph.h"
#include <string>

namespace TextGen
{
class Dictionary;

class Phrase : public Glyph
{
 public:
  ~Phrase() override;
  Phrase(const std::string& theWord);
#ifdef NO_COMPILER_GENERATED
  Phrase(const Phrase& thePhrase);
  Phrase& operator=(const Phrase& thePhrase);
#endif
  boost::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;
  bool isDelimiter() const override;

 private:
  Phrase() = delete;
  std::string itsWord;

};  // class Phrase

}  // namespace TextGen

// ======================================================================
