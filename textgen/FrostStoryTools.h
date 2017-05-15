// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::FrostStoryTools
 */
// ======================================================================

#pragma once

namespace TextGen
{
class WeatherPeriod;
}

namespace TextGen
{
class Sentence;

namespace FrostStoryTools
{
bool is_frost_season();

Sentence frost_sentence(const TextGen::WeatherPeriod& thePeriod, int theProbability);

Sentence severe_frost_sentence(const TextGen::WeatherPeriod& thePeriod, int theProbability);

}  // namespace FrostStoryTools
}  // namespace TextGen

// ======================================================================
