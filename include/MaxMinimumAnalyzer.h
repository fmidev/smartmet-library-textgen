// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::MaxMinimumAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::MaxMinimumAnalyzer
 *
 * \brief Maximum in space of time maxima
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MAXMINIMUMANALYZER_H
#define WEATHERANALYSIS_MAXMINIMUMANALYZER_H

#include "FunctionAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class MaxMinimumAnalyzer : public FunctionAnalyzer
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

  }; // class MaxMinimumAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MAXMINIMUMANALYZER_H

// ======================================================================
