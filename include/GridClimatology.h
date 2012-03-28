// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::GridClimatology
 */
// ======================================================================

#ifndef WEATHERANALYSIS_GRIDCLIMATOLOGY_H
#define WEATHERANALYSIS_GRIDCLIMATOLOGY_H

#include "WeatherForecaster.h"

namespace WeatherAnalysis
{
  class Acceptor;
  class AnalysisSources;
  class WeatherArea;
  class WeatherPeriodGenerator;
  class WeatherResult;

  class GridClimatology : public WeatherForecaster
  {
  public:

	using WeatherAnalyzer::analyze;

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
	
  }; // class GridClimatology

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_GRIDCLIMATOLOGY_H

// ======================================================================
