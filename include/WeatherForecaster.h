// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class WeatherAnalysis::WeatherForecaster
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERFORECASTER_H
#define WEATHERANALYSIS_WEATHERFORECASTER_H

#include "WeatherAnalyzer.h"

namespace WeatherAnalysis
{
  class Acceptor;
  class AnalysisSources;
  class WeatherArea;
  class WeatherPeriodGenerator;
  class WeatherResult;

  class WeatherForecaster : public WeatherAnalyzer
  {
  public:

	virtual ~WeatherForecaster() { }

	using WeatherAnalyzer::analyze;

	virtual const WeatherResult
	analyze(const AnalysisSources & theSources,
			const WeatherParameter & theParameter,
			const WeatherFunction & theAreaFunction,
			const WeatherFunction & theTimeFunction,
			const WeatherFunction & theSubTimeFunction,
			const WeatherArea & theArea,
			const WeatherPeriodGenerator & thePeriods,
			const Acceptor & theAreaAcceptor = DefaultAcceptor(),
			const Acceptor & theTimeAcceptor = DefaultAcceptor(),
			const Acceptor & theTester = NullAcceptor()) const = 0;

  }; // class WeatherForecaster

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERFORECASTER_H

// ======================================================================
