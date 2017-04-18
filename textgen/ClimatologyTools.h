// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::ClimatologyTools
 */
// ======================================================================

#pragma once

#include <calculator/AnalysisSources.h>
#include <calculator/WeatherPeriod.h>
#include <string>

namespace TextGen
{
namespace ClimatologyTools
{
WeatherPeriod getClimatologyPeriod(const TextGen::WeatherPeriod& thePeriod,
                                   const std::string& theDataName,
                                   const TextGen::AnalysisSources& theSources);
}  // namespace ClimatologyTools
}  // namespace TextGen

// ======================================================================
