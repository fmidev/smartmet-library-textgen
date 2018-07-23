// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::PrecipitationStoryTools
 */
// ======================================================================

#pragma once

#include <string>
#include <utility>
#include <vector>

namespace TextGen
{
class AnalysisSources;
class WeatherArea;
class WeatherPeriod;
class WeatherResult;
}  // namespace TextGen

namespace TextGen
{
class Sentence;

namespace PrecipitationStoryTools
{
Sentence rain_phrase(const TextGen::AnalysisSources& theSources,
                     const TextGen::WeatherArea& theArea,
                     const TextGen::WeatherPeriod& thePeriod,
                     const std::string& theVar,
                     int theDay);

Sentence places_phrase(const TextGen::AnalysisSources& theSources,
                       const TextGen::WeatherArea& theArea,
                       const TextGen::WeatherPeriod& thePeriod,
                       const std::string& theVar,
                       int theDay);

Sentence type_phrase(const TextGen::AnalysisSources& theSources,
                     const TextGen::WeatherArea& theArea,
                     const TextGen::WeatherPeriod& thePeriod,
                     const std::string& theVar,
                     int theDay);

Sentence sum_phrase(const TextGen::WeatherResult& theMinimum,
                    const TextGen::WeatherResult& theMaximum,
                    const TextGen::WeatherResult& theMean,
                    int theMinInterval,
                    const std::string& theRangeSeparator);

}  // namespace PrecipitationStoryTools
}  // namespace TextGen

// ======================================================================
