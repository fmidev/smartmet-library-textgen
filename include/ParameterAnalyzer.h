// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class ParameterAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::ParameterAnalyzer
 *
 * \brief Parameter analysis functions
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_PARAMETERANALYZER_H
#define WEATHERANALYSIS_PARAMETERANALYZER_H

#include "WeatherFunction.h"

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class ParameterAnalyzer
  {
  public:

	virtual ~ParameterAnalyzer() = 0;

	virtual WeatherResult forecast(const AnalysisSources & theSources,
								   const WeatherFunction & theFunction,
								   const WeatherLimits & theLimits,
								   const WeatherPeriod & thePeriod,
								   const WeatherArea & theArea) const = 0;
  }; // class ParameterAnalyzer

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_PARAMETERANALYZER_H

// ======================================================================
