// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::PrecipitationAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::PrecipitationAnalyzer
 *
 * \brief Precipitation analysis functions
 *
 */
// ======================================================================

#ifndef WEATHERANALYZER_PRECIPITATIONANALYZER_H
#define WEATHERANALYZER_PRECIPITATIONANALYZER_H

#include "WeatherFunction.h"
#include "ParameterAnalyzer.h"

namespace WeatherAnalysis
{
  class Acceptor;
  class AnalyzerSources;
  class WeatherArea;
  class WeatherPeriod;
  class WeatherResult;

  class PrecipitationAnalyzer : public ParameterAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherFunction & theAreaFunction,
								  const WeatherFunction & theTimeFunction,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const Acceptor & theAcceptor) const;

  };
}

#endif // WEATHERANALYSIS_PRECIPITATIONANALYZER_H

// ======================================================================
