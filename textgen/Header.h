// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Header
 */
// ======================================================================

#pragma once

#include "GlyphContainer.h"
#include <boost/shared_ptr.hpp>
#include <string>

namespace TextGen
{
class Header : public GlyphContainer
{
 public:
  ~Header() override {}
#ifdef NO_COMPILER_GENERATED
  Header();
  Header(const Header& theHeader);
  Header& operator=(const Header& theHeader);
#endif

  boost::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;
  bool isDelimiter() const override;

  Header& operator<<(const Glyph& theGlyph);
  Header& operator<<(const std::string& thePhrase);
  Header& operator<<(int theNumber);

};  // class Header

}  // namespace TextGen

// ======================================================================
