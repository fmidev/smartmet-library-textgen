// ======================================================================
/*!
 * \file
 * \brief Interface of namespace WeatherAnalysis::FunctionAnalyzerFactory
 */
// ======================================================================
/*!
 * \namespace WeatherAnalysis::FunctionAnalyzerFactory
 *
 * \brief Function analysis factory
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_FUNCTIONANALYZERFACTORY_H
#define WEATHERANALYSIS_FUNCTIONANALYZERFACTORY_H

#include "FunctionAnalyzer.h"
#include "WeatherFunction.h"
#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{
  namespace FunctionAnalyzerFactory
  {
	boost::shared_ptr<FunctionAnalyzer> create(const WeatherFunction & theFunction);
  } // namespace FunctionAnalyzerFactory
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_FUNCTIONANALYZERFACTORY_H

// ======================================================================
