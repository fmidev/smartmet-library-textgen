// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::TimePeriod
 */
// ======================================================================

#pragma once

#include "Glyph.h"
#include <calculator/WeatherPeriod.h>
#include <string>

namespace TextGen
{
class Dictionary;

class TimePeriod : public Glyph
{
 public:
  TimePeriod() = delete;
  ~TimePeriod() override;
  TimePeriod(WeatherPeriod thePeriod);
#ifdef NO_COMPILER_GENERATED
  TimePeriod(const TimePeriod& thePeriod);
  TimePeriod& operator=(const WeatherPeriod& thePeriod);
#endif
  std::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;
  bool isDelimiter() const override;

  const TextGenPosixTime& localStartTime() const { return itsPeriod.localStartTime(); }
  const TextGenPosixTime& localEndTime() const { return itsPeriod.localEndTime(); }
  const WeatherPeriod& weatherPeriod() const { return itsPeriod; }

 private:
  WeatherPeriod itsPeriod;

};  // class TimePeriod

}  // namespace TextGen

// ======================================================================
