// ======================================================================
/*!
 * \file
 * \brief Interface of class MaximumAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::MaximumAnalyzer
 *
 * \brief Maximum analysis
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MAXIMUMANALYZER_H
#define WEATHERANALYSIS_MAXIMUMANALYZER_H

#include "FunctionAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class MaximumAnalyzer : public FunctionAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const std::string & theDataName,
								  const std::string & theParameterName) const;
  }; // class MaximumAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MAXIMUMANALYZER_H

// ======================================================================
