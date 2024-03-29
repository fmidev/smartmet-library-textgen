// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::TemperatureStory
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

class TemperatureStory : public Story
{
 public:
  ~TemperatureStory() override;
  TemperatureStory(const TextGenPosixTime& theForecastTime,
                   const TextGen::AnalysisSources& theSources,
                   const TextGen::WeatherArea& theArea,
                   const TextGen::WeatherPeriod& thePeriod,
                   std::string theVariable);

  static bool hasStory(const std::string& theName);
  Paragraph makeStory(const std::string& theName) const override;

 private:
  Paragraph mean() const;
  Paragraph meanmax() const;
  Paragraph meanmin() const;
  Paragraph dailymax() const;
  Paragraph nightlymin() const;
  Paragraph weekly_minmax() const;
  Paragraph weekly_averages() const;
  Paragraph day() const;
  Paragraph range() const;
  Paragraph max36hours() const;
  Paragraph anomaly() const;

  TemperatureStory();
  TemperatureStory(const TemperatureStory& theStory);
  TemperatureStory& operator=(const TemperatureStory& theStory);

  const TextGenPosixTime itsForecastTime;
  const TextGen::AnalysisSources& itsSources;
  const TextGen::WeatherArea& itsArea;
  const TextGen::WeatherPeriod& itsPeriod;
  const std::string itsVar;

};  // class TemperatureStory
}  // namespace TextGen

// ======================================================================
