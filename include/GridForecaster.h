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
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class GridForecaster : public WeatherForecaster
  {
  public:

	virtual WeatherResult analyze(const AnalysisSources & theSources,
								  const WeatherParameter & theParameter,
								  const WeatherFunction & theFunction,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea) const;

  }; // class GridForecaster

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_GRIDFORECASTER_H

// ======================================================================
