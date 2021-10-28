// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::TemperatureRange
 */
// ======================================================================

#pragma once

#include "IntegerRange.h"
#include "TextFormatter.h"
#include <string>

namespace TextGen
{
class Dictionary;

class TemperatureRange : public IntegerRange
{
 public:
  explicit TemperatureRange(int theStartValue,
                            int theEndValue,
                            const std::string& theSeparator = "-");

  boost::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;

};  // class TemperatureRange

}  // namespace TextGen

// ======================================================================
