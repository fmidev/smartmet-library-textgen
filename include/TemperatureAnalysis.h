// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TemperatureAnalysis
 */
// ======================================================================
/*!
 * \namespace WeatherAnalysis::TemperatureAnalysis
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
  class WeatherArea;
  class WeatherLimits;
  class WeatherPeriod;
  class WeatherResult;

  namespace TemperatureAnalysis
  {
	WeatherResult forecast(const WeatherFunction & theFunction,
						   const WeatherLimits & theLimits,
						   const WeatherPeriod & thePeriod,
						   const WeatherArea & theArea);

	WeatherResult forecast(const WeatherFunction & theFunction,
						   const WeatherLimits & theLimits,
						   const WeatherPeriod & thePeriod,
						   const NFmiPoint & thePoint);
  }
}

#endif // WEATHERANALYSIS_TEMPERATUREANALYSIS_H

// ======================================================================
