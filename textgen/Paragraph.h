// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Paragraph
 */
// ======================================================================

#pragma once

#include "GlyphContainer.h"
#include <memory>
#include <string>

namespace TextGen
{
class Paragraph : public GlyphContainer
{
 public:
  ~Paragraph() override = default;
#ifdef NO_COMPILER_GENERATED
  Paragraph();
  Paragraph(const Paragraph& theParagraph);
  Paragraph& operator=(const Paragraph& theParagraph);
#endif

  std::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;
  bool isDelimiter() const override;

  Paragraph& operator<<(const Paragraph& theParagraph);
  Paragraph& operator<<(const Glyph& theGlyph);

};  // class Paragraph

}  // namespace TextGen

// ======================================================================
