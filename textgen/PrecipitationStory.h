// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::PrecipitationStory
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

class PrecipitationStory : public Story
{
 public:
  ~PrecipitationStory() override;
  PrecipitationStory(const TextGenPosixTime& theForecastTime,
                     const TextGen::AnalysisSources& theSources,
                     const TextGen::WeatherArea& theArea,
                     const TextGen::WeatherPeriod& thePeriod,
                     std::string theVariable);

  static bool hasStory(const std::string& theName);
  Paragraph makeStory(const std::string& theName) const override;

 private:
  Paragraph total() const;
  Paragraph total_day() const;
  Paragraph range() const;
  Paragraph classification() const;
  Paragraph pop_twodays() const;
  Paragraph pop_days() const;
  Paragraph pop_max() const;
  Paragraph sums() const;
  Paragraph daily_sums() const;

  PrecipitationStory();
  PrecipitationStory(const PrecipitationStory& theStory);
  PrecipitationStory& operator=(const PrecipitationStory& theStory);

  const TextGenPosixTime itsForecastTime;
  const TextGen::AnalysisSources& itsSources;
  const TextGen::WeatherArea& itsArea;
  const TextGen::WeatherPeriod& itsPeriod;
  const std::string itsVar;

};  // class PrecipitationStory
}  // namespace TextGen

// ======================================================================
