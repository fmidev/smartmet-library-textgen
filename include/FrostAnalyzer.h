// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::FrostAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::FrostAnalyzer
 *
 * \brief Frost analysis functions
 *
 */
// ======================================================================

#ifndef WEATHERANALYZER_FROSTANALYZER_H
#define WEATHERANALYZER_FROSTANALYZER_H

#include "WeatherFunction.h"
#include "ParameterAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalyzerSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class FrostAnalyzer : public ParameterAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherFunction & theFunction,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea) const;
  };
}

#endif // WEATHERANALYSIS_FROSTANALYZER_H

// ======================================================================