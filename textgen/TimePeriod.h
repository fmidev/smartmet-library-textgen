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
  ~TimePeriod() override;
  TimePeriod(const WeatherPeriod& thePeriod);
#ifdef NO_COMPILER_GENERATED
  TimePeriod(const TimePeriod& thePeriod);
  TimePeriod& operator=(const WeatherPeriod& thePeriod);
#endif
  boost::shared_ptr<Glyph> clone() const override;
  std::string realize(const Dictionary& theDictionary) const override;
  std::string realize(const TextFormatter& theFormatter) const override;
  bool isDelimiter() const override;

  inline const TextGenPosixTime& localStartTime() const { return itsPeriod.localStartTime(); }
  inline const TextGenPosixTime& localEndTime() const { return itsPeriod.localEndTime(); }
  inline const WeatherPeriod& weatherPeriod() const { return itsPeriod; }

 private:
  TimePeriod();
  WeatherPeriod itsPeriod;

};  // class TimePeriod

}  // namespace TextGen

// ======================================================================
