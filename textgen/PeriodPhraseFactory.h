// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::PeriodPhraseFactory
 */
// ======================================================================

#pragma once

#include <string>
class TextGenPosixTime;
namespace TextGen
{
class WeatherPeriod;
class WeatherArea;
}

namespace TextGen
{
class Sentence;

namespace PeriodPhraseFactory
{
Sentence create(const std::string& theType,
                const std::string& theVariable,
                const TextGenPosixTime& theForecastTime,
                const TextGen::WeatherPeriod& thePeriod);
Sentence create(const std::string& theType,
                const std::string& theVariable,
                const TextGenPosixTime& theForecastTime,
                const TextGen::WeatherPeriod& thePeriod,
                const TextGen::WeatherArea& theArea);

}  // namespace PeriodPhraseFactory

}  // namespace TextGen

// ======================================================================
