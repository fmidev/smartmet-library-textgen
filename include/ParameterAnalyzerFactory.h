// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherAnalysis::ParameterAnalyzerFactory
 */
// ======================================================================

#ifndef WEATHERANALYSIS_PARAMETERANALYZERFACTORY_H
#define WEATHERANALYSIS_PARAMETERANALYZERFACTORY_H

#include "ParameterAnalyzer.h"
#include "WeatherParameter.h"

namespace WeatherAnalysis
{
  namespace ParameterAnalyzerFactory
  {
	ParameterAnalyzer * create(WeatherAnalysis::WeatherParameter theParameter);

  } // namespace ParameterAnalyzerFactory
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_PARAMETERANALYZERFACTORY_H

// ======================================================================

