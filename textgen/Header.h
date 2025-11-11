// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Header
 */
// ======================================================================

#pragma once

#include "GlyphContainer.h"
#include <calculator/TextGenPosixTime.h>
#include <memory>
#include <optional>
#include <string>

namespace TextGen
{
class Header : public GlyphContainer
{
 public:
  ~Header() override = default;
#ifdef NO_COMPILER_GENERATED
  Header();
  Header(const Header& theHeader);
  Header& operator=(const Header& theHeader);
#endif

  std::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;
  bool isDelimiter() const override;
  void setForecastTime(const TextGenPosixTime& theTime) { itsTime = theTime; }
  const std::optional<TextGenPosixTime>& getForecastTime() const { return itsTime; }

  Header& operator<<(const Glyph& theGlyph);
  Header& operator<<(const std::string& thePhrase);
  Header& operator<<(int theNumber);

 private:
  std::optional<TextGenPosixTime> itsTime;

};  // class Header

}  // namespace TextGen

// ======================================================================
