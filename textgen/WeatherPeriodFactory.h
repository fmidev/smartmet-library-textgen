// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::WeatherPeriodFactory
 */
// ======================================================================

#pragma once

#include <string>

class TextGenPosixTime;

namespace TextGen
{
class WeatherPeriod;
}

namespace TextGen
{
namespace WeatherPeriodFactory
{
TextGen::WeatherPeriod create(const TextGenPosixTime& theTime, const std::string& theVariable);

}  // namespace WeatherPeriodFactory

}  // namespace TextGen

// ======================================================================
