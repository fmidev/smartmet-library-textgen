// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class WeatherAnalysis::WeatherObserver
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHEROBSERVER_H
#define WEATHERANALYSIS_WEATHEROBSERVER_H

#include "WeatherAnalyzer.h"

namespace WeatherAnalysis
{
  class Acceptor;
  class AnalysisSources;
  class WeatherArea;
  class WeatherPeriodGenerator;
  class WeatherResult;

  class WeatherObserver : public WeatherAnalyzer
  {
  public:

	virtual ~WeatherObserver() { }

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
			const Acceptor & theTester = NullAcceptor()) const = 0;

  }; // class WeatherObserver

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHEROBSERVER_H

// ======================================================================
