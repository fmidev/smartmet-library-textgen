// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::MinSumAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::MinSumAnalyzer
 *
 * \brief Maximum in space of time maxima
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MINSUMANALYZER_H
#define WEATHERANALYSIS_MINSUMANALYZER_H

#include "FunctionAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class MinSumAnalyzer : public FunctionAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const std::string & theDataName,
								  const std::string & theParameterName,
								  int theInterval,
								  NFmiDataModifier & theSubModifier) const;

  }; // class MinSumAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MINSUMANALYZER_H

// ======================================================================
