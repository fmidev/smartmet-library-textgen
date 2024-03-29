// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::SpecialStory
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

class SpecialStory : public Story
{
 public:
  ~SpecialStory() override;
  SpecialStory(const TextGenPosixTime& theForecastTime,
               const TextGen::AnalysisSources& theSources,
               const TextGen::WeatherArea& theArea,
               const TextGen::WeatherPeriod& thePeriod,
               std::string theVariable);

  static bool hasStory(const std::string& theName);
  Paragraph makeStory(const std::string& theName) const override;

 private:
  Paragraph none() const;
  Paragraph text() const;
  Paragraph date() const;
#if 0
	Paragraph table() const;
#endif

  SpecialStory();
  SpecialStory(const SpecialStory& theStory);
  SpecialStory& operator=(const SpecialStory& theStory);

  const TextGenPosixTime itsForecastTime;
  const TextGen::AnalysisSources& itsSources;
  const TextGen::WeatherArea& itsArea;
  const TextGen::WeatherPeriod& itsPeriod;
  const std::string itsVar;

};  // class SpecialStory
}  // namespace TextGen

// ======================================================================
