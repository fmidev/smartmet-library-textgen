// ======================================================================
/*!
 * \file
 * \brief Interface of class MinMaximumAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::MinMaximumAnalyzer
 *
 * \brief Maximum in space of time maxima
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MINMAXIMUMANALYZER_H
#define WEATHERANALYSIS_MINMAXIMUMANALYZER_H

#include "FunctionAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class MinMaximumAnalyzer : public FunctionAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const std::string & theDataName,
								  const std::string & theParameterName) const;
  }; // class MinMaximumAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MINMAXIMUMANALYZER_H

// ======================================================================
