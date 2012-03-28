// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::FunctionAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::FunctionAnalyzer
 *
 * \brief Function analysis
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_FUNCTIONANALYZER_H
#define WEATHERANALYSIS_FUNCTIONANALYZER_H

#include "WeatherDataType.h"
#include <string>

namespace WeatherAnalysis
{
  class Acceptor;
  class AnalysisSources;
  class Calculator;
  class WeatherArea;
  class WeatherPeriodGenerator;
  class WeatherResult;

  class FunctionAnalyzer
  {
  public:

	virtual ~FunctionAnalyzer() { }

	virtual WeatherResult
	analyze(const AnalysisSources & theSources,
			const WeatherDataType & theDataType,
			const WeatherArea & theArea,
			const WeatherPeriodGenerator & thePeriods,
			const Acceptor & theAreaAcceptor,
			const Acceptor & theTimeAcceptor,
			const Acceptor & theTester,
			const std::string & theDataName,
			const std::string & theParameterName) const = 0;

  }; // class FunctionAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_FUNCTIONANALYZER_H

// ======================================================================
