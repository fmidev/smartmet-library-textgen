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
  ~Real() override;
  explicit Real(float theReal, int thePrecision = 1, bool theComma = true);

#ifdef NO_COMPILER_GENERATED
  Real(const Real& theReal);
  Real& operator=(const Real& theReal);
#endif

  boost::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;
  bool isDelimiter() const override;
  float value() const;
  int precision() const;
  bool comma() const;

 private:
  Real() = delete;
  float itsReal;
  int itsPrecision;
  bool itsComma;

};  // class Real
}  // namespace TextGen

// ======================================================================
