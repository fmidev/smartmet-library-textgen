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

#include "ParameterAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalyzerSources;
  class WeatherArea;
  class WeatherPeriod;
  class WeatherResult;

  class MaxTemperatureAnalyzer : public ParameterAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherFunction & theAreaFunction,
								  const WeatherFunction & theTimeFunction,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const Acceptor & theAreaAcceptor,
								  const Acceptor & theTimeAcceptor,
								  const Acceptor & theTester = NullAcceptor()) const;

  };
}

#endif // WEATHERANALYSIS_MAXTEMPERATUREANALYZER_H

// ======================================================================
