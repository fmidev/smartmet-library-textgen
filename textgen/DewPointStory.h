// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::DewPointStory
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

class DewPointStory : public Story
{
 public:
  ~DewPointStory() override;
  DewPointStory(const TextGenPosixTime& theForecastTime,
                const TextGen::AnalysisSources& theSources,
                const TextGen::WeatherArea& theArea,
                const TextGen::WeatherPeriod& thePeriod,
                std::string  theVariable);

  static bool hasStory(const std::string& theName);
  Paragraph makeStory(const std::string& theName) const override;

 private:
  Paragraph range() const;

  DewPointStory();
  DewPointStory(const DewPointStory& theStory);
  DewPointStory& operator=(const DewPointStory& theStory);

  const TextGenPosixTime itsForecastTime;
  const TextGen::AnalysisSources& itsSources;
  const TextGen::WeatherArea& itsArea;
  const TextGen::WeatherPeriod& itsPeriod;
  const std::string itsVar;

};  // class DewPointStory
}  // namespace TextGen

// ======================================================================
