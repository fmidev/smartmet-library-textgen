// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::PlainLinesTextFormatter
 */
// ======================================================================
/*!
 * \class TextGen::PlainLinesTextFormatter
 *
 * \brief Glyph visitor generating normal ASCII output
 */
// ======================================================================

#ifndef TEXTGEN_PLAINLINESTEXTFORMATTER_H
#define TEXTGEN_PLAINLINESTEXTFORMATTER_H

#include "PlainTextFormatter.h"

namespace TextGen
{
  class PlainLinesTextFormatter : public PlainTextFormatter
  {
  public:
	PlainLinesTextFormatter() { }

	virtual ~PlainLinesTextFormatter() { }
	virtual std::string visit(const Paragraph & theParagraph) const;

  }; // class PlainLinesTextFormatter
} // namespace TextGen

#endif // TEXTGEN_PLAINLINESTEXTFORMATTER_H

// ======================================================================
