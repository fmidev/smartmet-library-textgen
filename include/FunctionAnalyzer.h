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

class NFmiDataModifier;

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

	// Note! Has default implementation
	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const std::string & theDataName,
								  const std::string & theParameterName) const;

	// Has no default implementation
	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const std::string & theDataName,
								  const std::string & theParameterName,
								  int theInterval,
								  NFmiDataModifier & theSubModifier) const = 0;

  }; // class FunctionAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_FUNCTIONANALYZER_H

// ======================================================================
