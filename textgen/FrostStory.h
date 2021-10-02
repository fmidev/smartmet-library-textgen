// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::FrostStory
 */
// ======================================================================

#pragma once

#include "Story.h"
#include <string>

class TextGenPosixTime;

namespace TextGen
{
class Paragraph;

class FrostStory : public Story
{
 public:
  ~FrostStory() override;
  FrostStory(const TextGenPosixTime& theForecastTime,
             const TextGen::AnalysisSources& theSources,
             const TextGen::WeatherArea& theArea,
             const TextGen::WeatherPeriod& thePeriod,
             const std::string& theVariable);

  static bool hasStory(const std::string& theName);
  Paragraph makeStory(const std::string& theName) const override;

 private:
  Paragraph mean() const;
  Paragraph maximum() const;
  Paragraph range() const;
  Paragraph twonights() const;
  Paragraph onenight() const;
  Paragraph day() const;

  FrostStory();
  FrostStory(const FrostStory& theStory);
  FrostStory& operator=(const FrostStory& theStory);

  const TextGenPosixTime itsForecastTime;
  const TextGen::AnalysisSources& itsSources;
  const TextGen::WeatherArea& itsArea;
  const TextGen::WeatherPeriod& itsPeriod;
  const std::string itsVar;

};  // class FrostStory
}  // namespace TextGen

// ======================================================================
