// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::SevereFrostAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::SevereFrostAnalyzer
 *
 * \brief SevereFrost analysis functions
 *
 */
// ======================================================================

#ifndef WEATHERANALYZER_SEVEREFROSTANALYZER_H
#define WEATHERANALYZER_SEVEREFROSTANALYZER_H

#include "WeatherFunction.h"
#include "ParameterAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalyzerSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class SevereFrostAnalyzer : public ParameterAnalyzer
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

#endif // WEATHERANALYSIS_SEVEREFROSTANALYZER_H

// ======================================================================
