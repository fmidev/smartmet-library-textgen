// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::PressureStory
 */
// ======================================================================

#pragma once

#include "Story.h"
#include <string>

namespace TextGen
{
class AnalysisSources;
class WeatherArea;
class WeatherPeriod;
}  // namespace TextGen

class TextGenPosixTime;

namespace TextGen
{
class Paragraph;

class PressureStory : public Story
{
 public:
  virtual ~PressureStory();
  PressureStory(const TextGenPosixTime& theForecastTime,
                const TextGen::AnalysisSources& theSources,
                const TextGen::WeatherArea& theArea,
                const TextGen::WeatherPeriod& thePeriod,
                const std::string& theVariable);

  static bool hasStory(const std::string& theName);
  virtual Paragraph makeStory(const std::string& theName) const;

 private:
  const Paragraph mean() const;

  PressureStory();
  PressureStory(const PressureStory& theStory);
  PressureStory& operator=(const PressureStory& theStory);

  const TextGenPosixTime itsForecastTime;
  const TextGen::AnalysisSources& itsSources;
  const TextGen::WeatherArea& itsArea;
  const TextGen::WeatherPeriod& itsPeriod;
  const std::string itsVar;

};  // class PressureStory
}  // namespace TextGen

// ======================================================================
