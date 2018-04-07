// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::WeatherStory
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

class WeatherStory : public Story
{
 public:
  virtual ~WeatherStory();
  WeatherStory(const TextGenPosixTime& theForecastTime,
               const TextGen::AnalysisSources& theSources,
               const TextGen::WeatherArea& theArea,
               const TextGen::WeatherPeriod& thePeriod,
               const std::string& theVariable);

  static bool hasStory(const std::string& theName);
  virtual Paragraph makeStory(const std::string& theName) const;

 private:
  Paragraph overview() const;
  Paragraph forecast() const;
  Paragraph forecast_at_sea() const;
  Paragraph shortoverview() const;
  Paragraph thunderprobability() const;
  Paragraph thunderprobability_simplified() const;

  WeatherStory();
  WeatherStory(const WeatherStory& theStory);
  WeatherStory& operator=(const WeatherStory& theStory);

  const TextGenPosixTime itsForecastTime;
  const TextGen::AnalysisSources& itsSources;
  const TextGen::WeatherArea& itsArea;
  const TextGen::WeatherPeriod& itsPeriod;
  const std::string itsVar;

};  // class WeatherStory
}  // namespace TextGen

// ======================================================================
