// ======================================================================
/*!
 * \file
 * \brief Interface of class MinimumAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::MinimumAnalyzer
 *
 * \brief Minimum analysis
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MINIMUMANALYZER_H
#define WEATHERANALYSIS_MINIMUMANALYZER_H

#include "FunctionAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class MinimumAnalyzer : public FunctionAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const std::string & theDataName,
								  const std::string & theParameterName) const;
  }; // class MinimumAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MINIMUMANALYZER_H

// ======================================================================
