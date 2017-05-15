// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::HeaderFactory
 */
// ======================================================================

#pragma once

#include <string>

namespace TextGen
{
class WeatherArea;
class WeatherPeriod;
}

namespace TextGen
{
class Header;

namespace HeaderFactory
{
Header create(const TextGen::WeatherArea& theArea,
              const TextGen::WeatherPeriod& thePeriod,
              const std::string& theVariable);

}  // namespace HeaderFactory
}  // namespace TextGen

// ======================================================================
