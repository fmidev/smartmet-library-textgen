// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::RealRange
 */
// ======================================================================

#pragma once

#include "Glyph.h"
#include "TextFormatter.h"
#include <string>

namespace TextGen
{
class Dictionary;

class RealRange : public Glyph
{
 public:
  RealRange() = delete;
  ~RealRange() override;
  explicit RealRange(float theStartValue,
                     float theEndValue,
                     std::string theSeparator = "-",
                     int thePrecision = 1);
#ifdef NO_COMPILER_GENERATED
  RealRange(const RealRange& theRealRange);
  RealRange& operator=(const RealRange& theRealRange);
#endif

  std::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;

  bool isDelimiter() const override;

  const std::string& rangeSeparator() const;
  float startValue() const;
  float endValue() const;
  int precision() const;

 protected:
  std::string itsRangeSeparator;
  float itsStartValue;
  float itsEndValue;
  int itsPrecision;

};  // class RealRange
}  // namespace TextGen

// ======================================================================
