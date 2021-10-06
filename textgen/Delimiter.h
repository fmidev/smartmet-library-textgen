// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Delimiter
 */
// ======================================================================

#pragma once

#include "Glyph.h"
#include <string>

namespace TextGen
{
class Delimiter : public Glyph
{
 public:
  ~Delimiter() override;
  Delimiter(std::string  theSeparator);
#ifdef NO_COMPILER_GENERATED
  Delimiter(const Delimiter& thePhrase);
  Delimiter& operator=(const Delimiter& thePhrase);
#endif

  boost::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;
  bool isDelimiter() const override;

  const std::string& value() const;

 private:
  Delimiter() = delete;

  std::string itsSeparator;

};  // class Delimiter

}  // namespace TextGen

// ======================================================================
