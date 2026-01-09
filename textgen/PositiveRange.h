// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::PositiveRange
 */
// ======================================================================

#pragma once

#include "Glyph.h"
#include "TextFormatter.h"
#include <string>

namespace TextGen
{
class Dictionary;

class PositiveRange : public Glyph
{
 public:
  PositiveRange() = delete;
  ~PositiveRange() override;
  explicit PositiveRange(int theStartValue, int theEndValue, std::string theSeparator = "-");
#ifdef NO_COMPILER_GENERATED
  PositiveRange(const PositiveRange& thePositiveRange);
  PositiveRange& operator=(const PositiveRange& thePositiveRange);
#endif

  std::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;

  bool isDelimiter() const override;

  const std::string& rangeSeparator() const;
  int startValue() const;
  int endValue() const;

 protected:
  std::string itsRangeSeparator;
  int itsStartValue;
  int itsEndValue;

};  // class PositiveRange
}  // namespace TextGen

// ======================================================================
