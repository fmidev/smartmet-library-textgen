// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::RoadTemperatureAnalyzer
 */
// ======================================================================

#ifndef WEATHERANALYZER_ROADTEMPERATUREANALYZER_H
#define WEATHERANALYZER_ROADTEMPERATUREANALYZER_H

#include "ParameterAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalyzerSources;
  class WeatherArea;
  class WeatherPeriodGenerator;
  class WeatherResult;

  class RoadTemperatureAnalyzer : public ParameterAnalyzer
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

#endif // WEATHERANALYSIS_ROADTEMPERATUREANALYZER_H

// ======================================================================
