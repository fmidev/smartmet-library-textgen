// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::CloudinessStory
 */
// ======================================================================

#pragma once

#include "Story.h"

class TextGenPosixTime;

namespace TextGen
{
class AnalysisSources;
class WeatherArea;
class WeatherPeriod;
}  // namespace TextGen

namespace TextGen
{
class Paragraph;

class CloudinessStory : public Story
{
 public:
  ~CloudinessStory() override;
  CloudinessStory(const TextGenPosixTime& theForecastTime,
                  const TextGen::AnalysisSources& theSources,
                  const TextGen::WeatherArea& theArea,
                  const TextGen::WeatherPeriod& thePeriod,
                  std::string  theVariable);

  static bool hasStory(const std::string& theName);
  Paragraph makeStory(const std::string& theName) const override;

 private:
  Paragraph overview() const;

  CloudinessStory();
  CloudinessStory(const CloudinessStory& theStory);
  CloudinessStory& operator=(const CloudinessStory& theStory);

  const TextGenPosixTime itsForecastTime;
  const TextGen::AnalysisSources& itsSources;
  const TextGen::WeatherArea& itsArea;
  const TextGen::WeatherPeriod& itsPeriod;
  const std::string itsVar;

};  // class CloudinessStory
}  // namespace TextGen

// ======================================================================
