// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::TimePhrase
 */
// ======================================================================

#pragma once

#include "GlyphContainer.h"
#include <calculator/TextGenPosixTime.h>
#include <memory>
#include <string>

namespace TextGen
{
class TimePhrase : public GlyphContainer
{
 public:
  ~TimePhrase() override = default;
  TimePhrase(const TextGenPosixTime& theTime) : itsForecastTime(theTime) {}
#ifdef NO_COMPILER_GENERATED
  TimePhrase() = delete;
  TimePhrase(const TimePhrase& theTimePhrase);
  TimePhrase& operator=(const TimePhrase& theTimePhrase);
#endif

  std::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;
  bool isDelimiter() const override;

  TimePhrase& operator<<(const Glyph& theGlyph);
  TimePhrase& operator<<(const std::string& thePhrase);
  TimePhrase& operator<<(int theNumber);

  const TextGenPosixTime& getForecastTime() const { return itsForecastTime; }

 private:
  TextGenPosixTime itsForecastTime;

};  // class TimePhrase

}  // namespace TextGen

// ======================================================================
