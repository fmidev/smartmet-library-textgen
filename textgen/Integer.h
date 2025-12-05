// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Integer
 */
// ======================================================================

#pragma once

#include "Glyph.h"
#include <string>

namespace TextGen
{
class Dictionary;
class TextFormatter;

class Integer : public Glyph
{
 public:
  Integer() = delete;
  ~Integer() override;
  explicit Integer(int theInteger);

#ifdef NO_COMPILER_GENERATED
  Integer(const Integer& theInteger);
  Integer& operator=(const Integer& theInteger);
#endif

  std::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;

  bool isDelimiter() const override;
  int value() const;

 private:
  int itsInteger;

};  // class Integer
}  // namespace TextGen

// ======================================================================
