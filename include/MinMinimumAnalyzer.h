// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::MinMinimumAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::MinMinimumAnalyzer
 *
 * \brief Maximum in space of time maxima
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MINMINIMUMANALYZER_H
#define WEATHERANALYSIS_MINMINIMUMANALYZER_H

#include "FunctionAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class MinMinimumAnalyzer : public FunctionAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const std::string & theDataName,
								  const std::string & theParameterName) const;
  }; // class MinMinimumAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MINMINIMUMANALYZER_H

// ======================================================================
