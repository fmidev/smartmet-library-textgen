// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::MeanMaximumAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::MeanMaximumAnalyzer
 *
 * \brief Maximum in space of time maxima
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MEANMAXIMUMANALYZER_H
#define WEATHERANALYSIS_MEANMAXIMUMANALYZER_H

#include "FunctionAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class MeanMaximumAnalyzer : public FunctionAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const std::string & theDataName,
								  const std::string & theParameterName) const;
  }; // class MeanMaximumAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MEANMAXIMUMANALYZER_H

// ======================================================================
