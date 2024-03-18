// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::HeaderFactory
 */
// ======================================================================

#pragma once

#include <string>

class TextGenPosixTime;

namespace TextGen
{
class WeatherArea;
class WeatherPeriod;
}  // namespace TextGen

namespace TextGen
{
class Header;

namespace HeaderFactory
{
Header create(const TextGenPosixTime& theForecastTime,
              const TextGen::WeatherArea& theArea,
              const TextGen::WeatherPeriod& thePeriod,
              const std::string& theVariable);

}  // namespace HeaderFactory
}  // namespace TextGen

// ======================================================================
