// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::GridForecaster
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::GridForecaster
 *
 * \brief Provides forecast analysis services to clients
 *
 * The GridForecaster class provides weather forecast analysis
 * services. All analysis is performed by analyzing the main
 * grid forecast. Probabilities are calculated by analyzing the
 * gridded forecasts.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_GRIDFORECASTER_H
#define WEATHERANALYSIS_GRIDFORECASTER_H

#include "WeatherForecaster.h"

namespace WeatherAnalysis
{
  class Acceptor;
  class AnalysisSources;
  class WeatherArea;
  class WeatherPeriodGenerator;
  class WeatherResult;

  class GridForecaster : public WeatherForecaster
  {
  public:

	using WeatherForecaster::analyze;

	virtual WeatherResult
	analyze(const AnalysisSources & theSources,
			const WeatherParameter & theParameter,
			const WeatherFunction & theAreaFunction,
			const WeatherFunction & theTimeFunction,
			const WeatherFunction & theSubTimeFunction,
			const WeatherArea & theArea,
			const WeatherPeriodGenerator & thePeriods,
			const Acceptor & theAreaAcceptor = DefaultAcceptor(),
			const Acceptor & theTimeAcceptor = DefaultAcceptor(),
			const Acceptor & theTester = NullAcceptor()) const;
	
  }; // class GridForecaster

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_GRIDFORECASTER_H

// ======================================================================
