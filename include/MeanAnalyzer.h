// ======================================================================
/*!
 * \file
 * \brief Interface of class MeanAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::MeanAnalyzer
 *
 * \brief Mean analysis
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_MEANANALYZER_H
#define WEATHERANALYSIS_MEANANALYZER_H

#include "FunctionAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class MeanAnalyzer : public FunctionAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const std::string & theDataName,
								  const std::string & theParameterName) const;
  }; // class MeanAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_MEANANALYZER_H

// ======================================================================
