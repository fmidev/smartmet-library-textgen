// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::WindSpeedAnalyzer
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WindSpeedAnalyzer
 *
 * \brief WindSpeed analysis functions
 *
 */
// ======================================================================

#ifndef WEATHERANALYZER_WINDSPEEDANALYZER_H
#define WEATHERANALYZER_WINDSPEEDANALYZER_H

#include "ParameterAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalyzerSources;
  class WeatherArea;
  class WeatherPeriodGenerator;
  class WeatherResult;

  class WindSpeedAnalyzer : public ParameterAnalyzer
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherDataType & theDataType,
								  const WeatherFunction & theAreaFunction,
								  const WeatherFunction & theTimeFunction,
								  const WeatherFunction & theSubTimeFunction,
								  const WeatherArea & theArea,
								  const WeatherPeriodGenerator & thePeriods,
								  const Acceptor & theAreaAcceptor,
								  const Acceptor & theTimeAcceptor,
								  const Acceptor & theTester = NullAcceptor()) const;

  };
}

#endif // WEATHERANALYSIS_WINDSPEEDANALYZER_H

// ======================================================================
