// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::MaxTemperatureAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::MaxTemperatureAnalyzer
 *
 * \brief MaxTemperature analysis functions
 *
 */
// ======================================================================

#ifndef WEATHERANALYZER_MAXTEMPERATUREANALYZER_H
#define WEATHERANALYZER_MAXTEMPERATUREANALYZER_H

#include "WeatherFunction.h"
#include "ParameterAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalyzerSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class MaxTemperatureAnalyzer : public ParameterAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherFunction & theAreaFunction,
								  const WeatherFunction & theTimeFunction,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea) const;
  };
}

#endif // WEATHERANALYSIS_MAXTEMPERATUREANALYZER_H

// ======================================================================
