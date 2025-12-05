// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Time
 */
// ======================================================================

#pragma once

#include "Glyph.h"
#include <calculator/TextGenPosixTime.h>
#include <string>

namespace TextGen
{
class Dictionary;

class WeatherTime : public Glyph
{
 public:
  WeatherTime() = delete;
  ~WeatherTime() override;
  WeatherTime(const TextGenPosixTime& theTime);
#ifdef NO_COMPILER_GENERATED
  WeatherTime(const WeatherTime& theTime);
  WeatherTime& operator=(const WeatherTime& theTime);
#endif
  std::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;
  bool isDelimiter() const override;

  inline const TextGenPosixTime& nfmiTime() const { return itsTime; }

 private:
  TextGenPosixTime itsTime;

};  // class Time

}  // namespace TextGen

// ======================================================================
