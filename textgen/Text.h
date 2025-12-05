// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Text
 */
// ======================================================================

#pragma once

#include "Glyph.h"
#include <string>

namespace TextGen
{
class Dictionary;
class TextFormatter;

class Text : public Glyph
{
 public:
  Text() = delete;
  ~Text() override;
  explicit Text(std::string theText);

#ifdef NO_COMPILER_GENERATED
  Text(const Text& theText);
  Text& operator=(const Text& theText);
#endif

  std::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;

  bool isDelimiter() const override;
  const std::string& value() const;

 private:
  const std::string itsText;

};  // class Text
}  // namespace TextGen

// ======================================================================
