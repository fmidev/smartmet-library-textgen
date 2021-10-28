// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::WindStory
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

class WindStory : public Story
{
 public:
  ~WindStory() override;
  WindStory(const TextGenPosixTime& theForecastTime,
            const TextGen::AnalysisSources& theSources,
            const TextGen::WeatherArea& theArea,
            const TextGen::WeatherPeriod& thePeriod,
            std::string  theVariable);

  static bool hasStory(const std::string& theName);
  Paragraph makeStory(const std::string& theName) const override;

 private:
  WindStory();
  WindStory(const WindStory& theStory);
  WindStory& operator=(const WindStory& theStory);

  Paragraph simple_overview() const;
  Paragraph overview() const;
  Paragraph daily_ranges() const;
  Paragraph range() const;
  Paragraph anomaly() const;

  const TextGenPosixTime itsForecastTime;
  const TextGen::AnalysisSources& itsSources;
  const TextGen::WeatherArea& itsArea;
  const TextGen::WeatherPeriod& itsPeriod;
  const std::string itsVar;

};  // class WindStory
}  // namespace TextGen

// ======================================================================
