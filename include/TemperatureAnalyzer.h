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
  class Acceptor;
  class AnalyzerSources;
  class WeatherArea;
  class WeatherPeriod;
  class WeatherResult;

  class TemperatureAnalyzer : public ParameterAnalyzer
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

#endif // WEATHERANALYSIS_TEMPERATUREANALYZER_H

// ======================================================================
