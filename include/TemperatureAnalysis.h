// ======================================================================
/*!
 * \file
 * \brief Interface of class TemperatureAnalysis
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::TemperatureAnalysis
 *
 * \brief Temperature analysis functions
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_TEMPERATUREANALYSIS_H
#define WEATHERANALYSIS_TEMPERATUREANALYSIS_H

#include "WeatherFunction.h"

namespace WeatherAnalysis
{
  class AnalysisSources;
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  class TemperatureAnalysis
  {
  public:

	static WeatherResult forecast(const AnalysisSources & theSources,
								  const WeatherFunction & theFunction,
								  const WeatherLimits & theLimits,
								  const WeatherPeriod & thePeriod,
								  const WeatherArea & theArea);
  };
}

#endif // WEATHERANALYSIS_TEMPERATUREANALYSIS_H

// ======================================================================
