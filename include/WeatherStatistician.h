// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class WeatherAnalysis::WeatherStatistician
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERSTATISTICIAN_H
#define WEATHERANALYSIS_WEATHERSTATISTICIAN_H

#include "WeatherAnalyzer.h"

namespace WeatherAnalysis
{
  class Acceptor;
  class AnalysisSources;
  class WeatherArea;
  class WeatherPeriodGenerator;
  class WeatherResult;

  class WeatherStatistician : public WeatherAnalyzer
  {
  public:

	virtual ~WeatherStatistician() { }

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

  }; // class WeatherStatistician

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERSTATISTICIAN_H

// ======================================================================
