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

#include <string>

namespace WeatherAnalysis
{
  class Acceptor;
  class AnalysisSources;
  class Calculator;
  class WeatherArea;
  class WeatherPeriod;
  class WeatherResult;

  class FunctionAnalyzer
  {
  public:

	virtual ~FunctionAnalyzer() { }

	// Note! Has default implementation
	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const Acceptor & theAreaAcceptor,
								  const Acceptor & theTimeAcceptor,
								  const Acceptor & theTester,
								  const std::string & theDataName,
								  const std::string & theParameterName) const;

	// Has no default implementation
	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const Acceptor & theAreaAcceptor,
								  const Acceptor & theTimeAcceptor,
								  const Acceptor & theTester,
								  const std::string & theDataName,
								  const std::string & theParameterName,
								  int theInterval,
								  Calculator & theSubCalculator) const = 0;

  }; // class FunctionAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_FUNCTIONANALYZER_H

// ======================================================================
