// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class WeatherAnalysis::WeatherObserver
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherObserver
 *
 * \brief Provides observation analysis services to clients
 *
 * The WeatherObserver class provides an uniform interface for
 * weather observation analysis services. The only thing required by the
 * API is the ability to calculate a single result either for
 * an area or for a single point. All the remaining work is in
 * initializing the class implementing the abstract interface
 * with query data, map information and so on.
 *
 * The WeatherObserver class specializes in analyzing observation
 * data.
 *
 * \see WeatherForecaster, WeatherStatistician
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
  class WeatherPeriod;
  class WeatherResult;

  class WeatherObserver : public WeatherAnalyzer
  {
  public:

	virtual ~WeatherObserver() { }

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherParameter & theParameter,
								  const WeatherFunction & theAreaFunction,
								  const WeatherFunction & theTimeFunction,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea,
								  const Acceptor & theAreaAcceptor = DefaultAcceptor(),
								  const Acceptor & theTimeAcceptor = DefaultAcceptor(),
								  const Acceptor & theTester = NullAcceptor()) const = 0;

  }; // class WeatherObserver

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHEROBSERVER_H

// ======================================================================
