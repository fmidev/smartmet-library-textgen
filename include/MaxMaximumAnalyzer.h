// ======================================================================
/*!
 * \file
 * \brief Interface of class MaxMaximumAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::MaxMaximumAnalyzer
 *
 * \brief Maximum in space of time maxima
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MAXMAXIMUMANALYZER_H
#define WEATHERANALYSIS_MAXMAXIMUMANALYZER_H

#include "FunctionAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class MaxMaximumAnalyzer : public FunctionAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const std::string & theDataName,
								  const std::string & theParameterName) const;
  }; // class MaxMaximumAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MAXMAXIMUMANALYZER_H

// ======================================================================
