// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::IntegerRange
 */
// ======================================================================

#pragma once

#include "Glyph.h"
#include "TextFormatter.h"
#include <string>

namespace TextGen
{
class Dictionary;

class IntegerRange : public Glyph
{
 public:
  ~IntegerRange() override;
  explicit IntegerRange(int theStartValue, int theEndValue, const std::string& theSeparator = "-");
#ifdef NO_COMPILER_GENERATED
  IntegerRange(const IntegerRange& theIntegerRange);
  IntegerRange& operator=(const IntegerRange& theIntegerRange);
#endif

  boost::shared_ptr<Glyph> clone() const override;
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

 private:
  IntegerRange() = delete;

};  // class IntegerRange
}  // namespace TextGen

// ======================================================================
