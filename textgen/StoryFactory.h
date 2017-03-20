// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::StoryFactory
 */
// ======================================================================

#pragma once

#include <string>

class TextGenPosixTime;

namespace TextGen
{
class AnalysisSources;
class WeatherArea;
class WeatherPeriod;
}

namespace TextGen
{
class Paragraph;

namespace StoryFactory
{
Paragraph create(const TextGenPosixTime& theForecastTime,
                 const TextGen::AnalysisSources& theSources,
                 const TextGen::WeatherArea& theArea,
                 const TextGen::WeatherPeriod& thePeriod,
                 const std::string& theName,
                 const std::string& theVariable);

}  // namespace StoryFactory
}  // namespace TextGen


// ======================================================================
