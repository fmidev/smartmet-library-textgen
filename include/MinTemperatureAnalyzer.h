// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::MinTemperatureAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::MinTemperatureAnalyzer
 *
 * \brief MinTemperature analysis functions
 *
 */
// ======================================================================

#ifndef WEATHERANALYZER_MINTEMPERATUREANALYZER_H
#define WEATHERANALYZER_MINTEMPERATUREANALYZER_H

#include "WeatherFunction.h"
#include "ParameterAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalyzerSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class MinTemperatureAnalyzer : public ParameterAnalyzer
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

#endif // WEATHERANALYSIS_MINTEMPERATUREANALYZER_H

// ======================================================================
