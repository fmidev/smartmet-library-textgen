// ======================================================================
/*!
 * \file
 * \brief Interface of class MeanMinimumAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::MeanMinimumAnalyzer
 *
 * \brief Maximum in space of time maxima
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MEANMINIMUMANALYZER_H
#define WEATHERANALYSIS_MEANMINIMUMANALYZER_H

#include "FunctionAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class MeanMinimumAnalyzer : public FunctionAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const std::string & theDataName,
								  const std::string & theParameterName) const;
  }; // class MeanMinimumAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MEANMINIMUMANALYZER_H

// ======================================================================
