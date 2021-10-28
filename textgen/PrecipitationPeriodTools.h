// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::PrecipitationPeriodTools
 */
// ======================================================================

#pragma once

#include <list>
#include <string>

class TextGenPosixTime;

namespace TextGen
{
class AnalysisSources;
class WeatherArea;
class WeatherPeriod;

namespace PrecipitationPeriodTools
{
using RainTimes = std::list<TextGenPosixTime>;
using RainPeriods = std::list<WeatherPeriod>;

// The main function

RainPeriods analyze(const AnalysisSources& theSources,
                    const WeatherArea& theArea,
                    const WeatherPeriod& thePeriod,
                    const std::string& theVar);

// Extract parts of rainy periods

RainPeriods overlappingPeriods(const RainPeriods& thePeriods, const WeatherPeriod& thePeriod);

RainPeriods inclusivePeriods(const RainPeriods& thePeriods, const WeatherPeriod& thePeriod);

// Utility functions used by analyze

RainTimes findRainTimes(const AnalysisSources& theSources,
                        const WeatherArea& theArea,
                        const WeatherPeriod& thePeriod,
                        const std::string& theVar);

RainPeriods findRainPeriods(const RainTimes& theTimes, const std::string& theVar);

RainPeriods mergeNightlyRainPeriods(const RainPeriods& thePeriods, const std::string& theVar);

RainPeriods mergeLargeRainPeriods(const RainPeriods& thePeriods, const std::string& theVar);

}  // namespace PrecipitationPeriodTools
}  // namespace TextGen

// ======================================================================
