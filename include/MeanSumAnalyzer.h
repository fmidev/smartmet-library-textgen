// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::MeanSumAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::MeanSumAnalyzer
 *
 * \brief Maximum in space of time maxima
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MEANSUMANALYZER_H
#define WEATHERANALYSIS_MEANSUMANALYZER_H

#include "FunctionAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class MeanSumAnalyzer : public FunctionAnalyzer
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

  }; // class MeanSumAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MEANSUMANALYZER_H

// ======================================================================
