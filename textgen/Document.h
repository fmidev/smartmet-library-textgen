// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Document
 */
// ======================================================================

#pragma once

#include "GlyphContainer.h"
#include <memory>
#include <string>

namespace TextGen
{
class Document : public GlyphContainer
{
 public:
  ~Document() override = default;
#ifdef NO_COMPILER_GENERATED
  Document();
  Document(const Document& theDocument);
  Document& operator=(const Document& theDocument);
#endif

  std::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;
  bool isDelimiter() const override;

  Document& operator<<(const Document& theDocument);
  Document& operator<<(const Glyph& theGlyph);

};  // class Document

}  // namespace TextGen

// ======================================================================
