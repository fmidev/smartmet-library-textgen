// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class WeatherForecaster
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherForecaster
 *
 * \brief Provides forecast analysis services to clients
 *
 * The WeatherForecaster class provides an uniform interface for
 * weather forecast analysis services. The only thing required by the
 * API is the ability to calculate a single result either for
 * an area or for a single point. All the remaining work is in
 * initializing the class implementing the abstract interface
 * with query data, map information and so on.
 *
 * The WeatherForecaster class specializes in analyzing forecast
 * data.
 *
 * \see WeatherObserver, WeatherStatistician
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERFORECASTER_H
#define WEATHERANALYSIS_WEATHERFORECASTER_H

#include "WeatherAnalyzer.h"

namespace WeatherAnalysis
{
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class WeatherForecaster : public WeatherAnalyzer
  {
  public:

	virtual WeatherResult analyze(const WeatherParameter & theParameter,
								  const WeatherFunction & theFunction,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea) const = 0;

	virtual WeatherResult analyze(const WeatherParameter & theParameter,
								  const WeatherFunction & theFunction,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const NFmiPoint & thePoint) const = 0;
  }; // class WeatherForecaster

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERFORECASTER_H

// ======================================================================
