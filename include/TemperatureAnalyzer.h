// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::TemperatureAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::TemperatureAnalyzer
 *
 * \brief Temperature analysis functions
 *
 */
// ======================================================================

#ifndef WEATHERANALYZER_TEMPERATUREANALYZER_H
#define WEATHERANALYZER_TEMPERATUREANALYZER_H

#include "WeatherFunction.h"
#include "ParameterAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalyzerSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class TemperatureAnalyzer : public ParameterAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherFunction & theFunction,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea) const;
  };
}

#endif // WEATHERANALYSIS_TEMPERATUREANALYZER_H

// ======================================================================
