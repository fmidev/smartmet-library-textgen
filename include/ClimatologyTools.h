// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherAnalysis::ClimatologyTools
 */
// ======================================================================

#ifndef WEATHERANALYSIS_CLIMATOLOGYTOOLS_H
#define WEATHERANALYSIS_CLIMATOLOGYTOOLS_H

#include "WeatherPeriod.h"
#include "AnalysisSources.h"
#include <string>

using namespace std;

namespace WeatherAnalysis
{
  namespace ClimatologyTools
  {
	
	WeatherPeriod getClimatologyPeriod(const WeatherAnalysis::WeatherPeriod& thePeriod,
									   const std::string& theDataName,
									   const WeatherAnalysis::AnalysisSources& theSources);
  } // namespace ClimatologyTools
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_SEASO NTOOLS_H

// ======================================================================
