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
  virtual ~SingleLineTextFormatter() {}
  using PlainTextFormatter::visit;
  virtual std::string visit(const Document& theDocument) const override;

  std::string name() const { return "singleline"; }

};  // class SingleLineTextFormatter
}  // namespace TextGen

// ======================================================================
