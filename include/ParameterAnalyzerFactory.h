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
#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{
  namespace ParameterAnalyzerFactory
  {
	boost::shared_ptr<ParameterAnalyzer> create(WeatherAnalysis::WeatherParameter theParameter);

  } // namespace ParameterAnalyzerFactory
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_PARAMETERANALYZERFACTORY_H

// ======================================================================

