// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::ThunderAnalyzer
 */
// ======================================================================

#ifndef WEATHERANALYZER_THUNDERANALYZER_H
#define WEATHERANALYZER_THUNDERANALYZER_H

#include "WeatherFunction.h"
#include "ParameterAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalyzerSources;
  class WeatherArea;
  class WeatherPeriodGenerator;
  class WeatherResult;

  class ThunderAnalyzer : public ParameterAnalyzer
  {
  public:

	virtual const WeatherResult
	analyze(const AnalysisSources & theSources,
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

#endif // WEATHERANALYSIS_THUNDERANALYZER_H

// ======================================================================
