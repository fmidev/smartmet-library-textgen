// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::PrecipitationProbabilityAnalyzer
 */
// ======================================================================

#ifndef WEATHERANALYZER_PRECIPITATIONPROBABILITYANALYZER_H
#define WEATHERANALYZER_PRECIPITATIONPROBABILITYANALYZER_H

#include "ParameterAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalyzerSources;
  class WeatherArea;
  class WeatherPeriodGenerator;
  class WeatherResult;

  class PrecipitationProbabilityAnalyzer : public ParameterAnalyzer
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

#endif // WEATHERANALYSIS_PRECIPITATIONPROBABILITYANALYZER_H

// ======================================================================
