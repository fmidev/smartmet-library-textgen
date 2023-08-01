// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::WaveStory
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

class WaveStory : public Story
{
 public:
  ~WaveStory() override;
  WaveStory(const TextGenPosixTime& theForecastTime,
            const TextGen::AnalysisSources& theSources,
            const TextGen::WeatherArea& theArea,
            const TextGen::WeatherPeriod& thePeriod,
            std::string theVariable);

  static bool hasStory(const std::string& theName);
  Paragraph makeStory(const std::string& theName) const override;

 private:
  WaveStory();
  WaveStory(const WaveStory& theStory);
  WaveStory& operator=(const WaveStory& theStory);

  Paragraph range() const;

  const TextGenPosixTime itsForecastTime;
  const TextGen::AnalysisSources& itsSources;
  const TextGen::WeatherArea& itsArea;
  const TextGen::WeatherPeriod& itsPeriod;
  const std::string itsVar;

};  // class WaveStory
}  // namespace TextGen

// ======================================================================
