// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TemperatureTools
 */
// ======================================================================

#pragma once

#include <string>

namespace TextGen
{
class WeatherResult;
class WeatherPeriod;
class AnalysisSources;
class WeatherArea;
}  // namespace TextGen

namespace TextGen
{
namespace TemperatureTools
{
// ----------------------------------------------------------------------
/*!
 * \brief calculate Minimum, Maximum and Mean temperatures of
 * areal maximum temperatures
 */
// ----------------------------------------------------------------------

void min_max_mean_temperature(const std::string& theVar,
                              const TextGen::AnalysisSources& theSources,
                              const TextGen::WeatherArea& theArea,
                              const TextGen::WeatherPeriod& thePeriod,
                              TextGen::WeatherResult& theMin,
                              TextGen::WeatherResult& theMax,
                              TextGen::WeatherResult& theMean);

// ----------------------------------------------------------------------
/*!
 * \brief calculate morning temperature, default values follow finnish convention
 */
// ----------------------------------------------------------------------

void morning_temperature(const std::string& theVar,
                         const TextGen::AnalysisSources& theSources,
                         const TextGen::WeatherArea& theArea,
                         const TextGen::WeatherPeriod& thePeriod,
                         TextGen::WeatherResult& theMin,
                         TextGen::WeatherResult& theMax,
                         TextGen::WeatherResult& theMean);

// ----------------------------------------------------------------------
/*!
 * \brief calculate afternoon temperature, default values follow finnish convention
 */
// ----------------------------------------------------------------------

void afternoon_temperature(const std::string& theVar,
                           const TextGen::AnalysisSources& theSources,
                           const TextGen::WeatherArea& theArea,
                           const TextGen::WeatherPeriod& thePeriod,
                           TextGen::WeatherResult& theMin,
                           TextGen::WeatherResult& theMax,
                           TextGen::WeatherResult& theMean);

// ----------------------------------------------------------------------
/*!
 * \brief clamp the big temperature interval into smaller according to configuration file settings
 */
// ----------------------------------------------------------------------

void clamp_temperature(const std::string& theVar,
                       const bool& isWinter,
                       const bool& isDay,
                       int& theMinimum,
                       int& theMaximum);

}  // namespace TemperatureTools
}  // namespace TextGen

// ======================================================================
