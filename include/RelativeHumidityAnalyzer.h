// ======================================================================
/*!
 * \file
 * \brief Interface of class RelativeHumidityAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::RelativeHumidityAnalyzer
 *
 * \brief RelativeHumidity analysis functions
 *
 */
// ======================================================================

#ifndef WEATHERANALYZER_RELATIVEHUMIDITYANALYZER_H
#define WEATHERANALYZER_RELATIVEHUMIDITYANALYZER_H

#include "WeatherFunction.h"
#include "ParameterAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalyzerSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class RelativeHumidityAnalyzer : public ParameterAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherFunction & theFunction,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea) const;
  };
}

#endif // WEATHERANALYSIS_RELATIVEHUMIDITYANALYZER_H

// ======================================================================
