// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::MaxSumAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::MaxSumAnalyzer
 *
 * \brief Maximum in space of time maxima
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MAXSUMANALYZER_H
#define WEATHERANALYSIS_MAXSUMANALYZER_H

#include "FunctionAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class MaxSumAnalyzer : public FunctionAnalyzer
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

  }; // class MaxSumAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MAXSUMANALYZER_H

// ======================================================================
