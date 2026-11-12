// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::TemperatureRange
 */
// ======================================================================

#pragma once

#include "IntegerRange.h"
#include <string>

namespace TextGen
{

class TemperatureRange : public IntegerRange
{
 public:
  explicit TemperatureRange(int theStartValue,
                            int theEndValue,
                            const std::string& theSeparator = "-");

  std::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;

};  // class TemperatureRange

}  // namespace TextGen

// ======================================================================
