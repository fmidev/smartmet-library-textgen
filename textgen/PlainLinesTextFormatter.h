// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::PlainLinesTextFormatter
 */
// ======================================================================

#pragma once

#include "PlainTextFormatter.h"
#include <string>

namespace TextGen
{
class PlainLinesTextFormatter : public PlainTextFormatter
{
 public:
  PlainLinesTextFormatter() = default;
  ~PlainLinesTextFormatter() override = default;
  using PlainTextFormatter::visit;
  std::string visit(const Paragraph& theParagraph) const override;

};  // class PlainLinesTextFormatter
}  // namespace TextGen

// ======================================================================
