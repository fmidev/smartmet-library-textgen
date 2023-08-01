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
  ~RealRange() override;
  explicit RealRange(float theStartValue,
                     float theEndValue,
                     int thePrecision = 1,
                     std::string theSeparator = "-");
#ifdef NO_COMPILER_GENERATED
  RealRange(const RealRange& theRealRange);
  RealRange& operator=(const RealRange& theRealRange);
#endif

  boost::shared_ptr<Glyph> clone() const override;
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

 private:
  RealRange() = delete;

};  // class RealRange
}  // namespace TextGen

// ======================================================================
