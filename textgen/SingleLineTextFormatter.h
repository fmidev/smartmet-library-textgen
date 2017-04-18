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

  virtual std::string visit(const Document& theDocument) const;

  std::string name() const { return "singleline"; }

};  // class SingleLineTextFormatter
}  // namespace TextGen


// ======================================================================
