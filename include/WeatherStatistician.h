// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class WeatherAnalysis::WeatherStatistician
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherStatistician
 *
 * \brief Provides statistical analysis services to clients
 *
 * The WeatherStatistician class provides an uniform interface for
 * weather statistical analysis services. The only thing required by the
 * API is the ability to calculate a single result either for
 * an area or for a single point. All the remaining work is in
 * initializing the class implementing the abstract interface
 * with query data, map information and so on.
 *
 * The WeatehrStatistician class specializes in analyzing statistical
 * weather information, usually climatological values.
 *
 * \see WeatherForecaster, WeatherObserver
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERSTATISTICIAN_H
#define WEATHERANALYSIS_WEATHERSTATISTICIAN_H

#include "WeatherAnalyzer.h"

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class WeatherStatistician : public WeatherAnalyzer
  {
  public:

	virtual ~WeatherStatistician() = 0;

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherParameter & theParameter,
								  const WeatherFunction & theFunction,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea) const = 0;
  }; // class WeatherStatistician

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERSTATISTICIAN_H

// ======================================================================
