// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class FunctionAnalyzer
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
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class FunctionAnalyzer
  {
  public:

	virtual ~FunctionAnalyzer() { }

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const std::string & theDataName,
								  const std::string & theParameterName) const = 0;
  }; // class FunctionAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_FUNCTIONANALYZER_H

// ======================================================================
