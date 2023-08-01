// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::RoadStory
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

class RoadStory : public Story
{
 public:
  ~RoadStory() override;
  RoadStory(const TextGenPosixTime& theForecastTime,
            const TextGen::AnalysisSources& theSources,
            const TextGen::WeatherArea& theArea,
            const TextGen::WeatherPeriod& thePeriod,
            std::string theVariable);

  static bool hasStory(const std::string& theName);
  Paragraph makeStory(const std::string& theName) const override;

 private:
  Paragraph daynightranges() const;
  Paragraph condition_overview() const;
  Paragraph warning_overview() const;
  Paragraph condition_shortview() const;
  Paragraph shortrange() const;
  Paragraph warning_shortview() const;

  RoadStory();
  RoadStory(const RoadStory& theStory);
  RoadStory& operator=(const RoadStory& theStory);

  const TextGenPosixTime itsForecastTime;
  const TextGen::AnalysisSources& itsSources;
  const TextGen::WeatherArea& itsArea;
  const TextGen::WeatherPeriod& itsPeriod;
  const std::string itsVar;

};  // class RoadStory
}  // namespace TextGen

// ======================================================================
