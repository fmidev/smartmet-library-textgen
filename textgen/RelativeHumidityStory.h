// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::RelativeHumidityStory
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

class RelativeHumidityStory : public Story
{
 public:
  ~RelativeHumidityStory() override;
  RelativeHumidityStory(const TextGenPosixTime& theForecastTime,
                        const TextGen::AnalysisSources& theSources,
                        const TextGen::WeatherArea& theArea,
                        const TextGen::WeatherPeriod& thePeriod,
                        std::string  theVariable);

  static bool hasStory(const std::string& theName);
  Paragraph makeStory(const std::string& theName) const override;

 private:
  Paragraph lowest() const;
  Paragraph day() const;
  Paragraph range() const;

  RelativeHumidityStory();
  RelativeHumidityStory(const RelativeHumidityStory& theStory);
  RelativeHumidityStory& operator=(const RelativeHumidityStory& theStory);

  const TextGenPosixTime itsForecastTime;
  const TextGen::AnalysisSources& itsSources;
  const TextGen::WeatherArea& itsArea;
  const TextGen::WeatherPeriod& itsPeriod;
  const std::string itsVar;

};  // class RelativeHumidityStory
}  // namespace TextGen

// ======================================================================
