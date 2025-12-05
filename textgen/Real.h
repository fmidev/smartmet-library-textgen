// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Real
 */
// ======================================================================

#pragma once

#include "Glyph.h"
#include <string>

namespace TextGen
{
class Dictionary;
class TextFormatter;

class Real : public Glyph
{
 public:
  Real() = delete;
  ~Real() override;
  explicit Real(float theReal, int thePrecision = 1, bool theComma = true);

#ifdef NO_COMPILER_GENERATED
  Real(const Real& theReal);
  Real& operator=(const Real& theReal);
#endif

  std::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;
  bool isDelimiter() const override;
  float value() const;
  int precision() const;
  bool comma() const;

 private:
  float itsReal = 0;
  int itsPrecision = 1;
  bool itsComma = true;

};  // class Real
}  // namespace TextGen

// ======================================================================
