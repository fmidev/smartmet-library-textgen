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

#include "ParameterAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalyzerSources;
  class WeatherArea;
  class WeatherPeriod;
  class WeatherResult;

  class MinTemperatureAnalyzer : public ParameterAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherDataType & theDataType,
								  const WeatherFunction & theAreaFunction,
								  const WeatherFunction & theTimeFunction,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const Acceptor & theAreaAcceptor,
								  const Acceptor & theTimeAcceptor,
								  const Acceptor & theTester = NullAcceptor()) const;
  };
}

#endif // WEATHERANALYSIS_MINTEMPERATUREANALYZER_H

// ======================================================================
