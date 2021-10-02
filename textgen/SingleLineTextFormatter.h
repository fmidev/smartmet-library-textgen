// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::SingleLineTextFormatter
 */
// ======================================================================

#pragma once

#include "PlainTextFormatter.h"

namespace TextGen
{
class SingleLineTextFormatter : public PlainTextFormatter
{
 public:
  SingleLineTextFormatter() {}
  ~SingleLineTextFormatter() override {}
  using PlainTextFormatter::visit;
  std::string visit(const Document& theDocument) const override;

  std::string name() const { return "singleline"; }

};  // class SingleLineTextFormatter
}  // namespace TextGen

// ======================================================================
